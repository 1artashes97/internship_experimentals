#pragma once


namespace il { namespace fiber { namespace impl {
class fiber;
using fiber_main_function = void(*)(fiber*);

struct fiber_and_main {
    fiber*              fib;
    fiber_main_function main_function;

    fiber_and_main(fiber* f, fiber_main_function main_func)
        : fib(f), main_function(main_func) { }
};
}}}
