#pragma once

#include <memory>

namespace lcrt {

enum operation { COMMIT, REVERT, UNFILLED };

class operation_handler {
	// could be replaced with local vector allocation
	std::shared_ptr<operation> next_operation;
	void renew_next() { next_operation = std::make_shared<operation>(UNFILLED); }

public:
	operation_handler() : next_operation(std::make_shared<operation>(UNFILLED)) {}
	std::shared_ptr<operation> next() { return next_operation; }
	void commit() {
		*next_operation = operation.COMMIT;
		renew_next();
	}
	void revert() {
		*next_operation = operation.REVERT;
		renew_next();
	}
};

template <typename T> class lcr {
	// assumes primitive copyable
	operation_handler& ops;
	std::shared_ptr<operation> scheduled_operation;
	T primary;
	T secondary;

	void propogate() {
		operation op = *scheduled_operation;
		if (op == operation.UNFILLED)
			return;

		if (op == operation.COMMIT)
			secondary = primary;
		else if (op == operation.REVERT)
			primary = secondary;
		scheduled_operation = ops.next();
	}

public:
	template <typename... Args>
	lcr(operation_handler& ops, Args&&..args)
			: ops(_ops), scheduled_operation(ops.next()), primary(args),
				secondary(args) {}

	T& operator()() { return get(); }

	T& get() {
		propogate();
		return primary;
	}
}

template <typename T>
class global_lcr : public lcr<T> {
	static operation_handler ops;

public:
	template <typename... Args>
	lcr(operation_handler& ops, Args&&..args) : lcr<T>(ops, args) {}
}

}; // namespace lcrt
