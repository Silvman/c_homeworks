#include <iostream>

/**
 * silvman <github.com/silvman> 2018
 *
 * std::function has specializations for one- and two-argumented function, 
 * which allows to access types of arguments of this functions, 
 * but if sizeof...(Args) > 2, there is no any information about arguments types
 *
 * here is my variant how it can be possible to insert all 
 * argument types to the function template class
 * (and practice with parameter packs)
 * */

struct empty_type {};

template <class Front = empty_type, class... Another>
struct type_buf {
    typedef Front type;
    typedef type_buf<Another...> next;
};

template <>
struct type_buf<empty_type> { };


template<class Ret, class... Args>
struct my_function {
    typedef Ret return_type;
    typedef type_buf<Args...> args_type;
};

template<class Ret, class... Args>
struct my_function<Ret(Args...)> {
    typedef Ret return_type;
    typedef type_buf<Args...> args_type;

    my_function() = default;
    my_function(Ret (*func)(Args... args)) {
        p_function = func;
    }

    my_function& operator=(Ret (*func)(Args... args)) {
        p_function = func;
        return *this;
    }

    Ret operator()(Args... args) {
        if (p_function)
            return p_function(args...);
    }

    Ret (*p_function)(Args... args) = nullptr;
};


struct test {
    int operator()(int a) {
        std::cout << a << std::endl;
        return a;
    }
};

int foo(int a) {
    std::cout << a << std::endl;
}

int main() {
    typedef my_function<int(int)> func;
    func::args_type::type d = 3;
    func a = foo;
    a(d);

    return 0;
}
