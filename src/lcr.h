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
		*next_operation = COMMIT;
		renew_next();
	}
	void revert() {
		*next_operation = REVERT;
		renew_next();
	}
};

template <typename T> class lcr {
	// assumes primitive copyable
	operation_handler& handler;
	std::shared_ptr<operation> scheduled_operation;
	T primary;
	T secondary;

	void propogate() {
		operation op = *scheduled_operation;
		if (op == UNFILLED)
			return;

		if (op == COMMIT)
			secondary = primary;
		else if (op == REVERT)
			primary = secondary;
		scheduled_operation = handler.next();
	}

public:
	template <typename... Args>
	lcr(operation_handler& _handler, Args&&... args)
			: handler(_handler), scheduled_operation(handler.next()),
				primary(args...), secondary(args...) {}

	T& operator()() { return get(); }

	T& get() {
		propogate();
		return primary;
	}
};

operation_handler global_handler;

template <typename T> class global_lcr : public lcr<T> {

public:
	template <typename... Args>
	global_lcr(Args&&... args) : lcr<T>(global_handler, args...) {}
};

}; // namespace lcrt
