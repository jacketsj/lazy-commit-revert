#pragma once

#include <memory>

namespace lcrx {

// empty namespace to hide from other files
namespace {
enum operation { COMMIT, REVERT, UNFILLED };
}

class lcr_handler {
	// could be replaced with local vector allocation
	std::shared_ptr<operation> next_operation;
	void renew_next() { next_operation = std::make_shared<operation>(UNFILLED); }

public:
	lcr_handler() : next_operation(std::make_shared<operation>(UNFILLED)) {}
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

lcr_handler lcr_global_handler;

template <typename T> class lcr {
	// assumes primitive copyable
	lcr_handler* handler;
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
		scheduled_operation = handler->next();
	}

public:
	template <typename... Args>
	lcr(Args&&... args)
			: handler(&lcr_global_handler), scheduled_operation(handler->next()),
				primary(args...), secondary(args...) {}

	template <typename... Args>
	lcr(lcr_handler& _handler, Args&&... args)
			: handler(&_handler), scheduled_operation(handler->next()),
				primary(args...), secondary(args...) {}

	void assign(lcr_handler& _handler) {
		propogate();
		// a re-assignment also commits the value
		secondary = primary;
		handler = &_handler;
		scheduled_operation = handler->next();
	}

	T& operator()() { return get(); }

	T& get() {
		propogate();
		return primary;
	}
};

}; // namespace lcrx
