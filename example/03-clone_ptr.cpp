#include "value_ptr.hpp"

namespace other
{
namespace core
{
    template <typename T>
    struct method_cloner
    {
        template <typename U>
        T * operator()(U && o)
        {
            return release(std::forward<U>(o).clone());
        }

        template <typename U>
        auto release(U * p)
        {
            return p;
        }

        template <typename U>
        auto release(U p)
        {
            return p.release();
        }

    };

    // clone_ptr limited to usage on classes with clone method.
    template <typename T> // todo add deleter
    using clone_ptr = nonstd::value_ptr<T, method_cloner<T>>;
}// core

#define TEST_COMPILE_ERROR 0

    struct NoClone {};
    void testNoClone()
    {
        core::clone_ptr<NoClone> ps;
        auto ps2 = std::move(ps);
#if TEST_COMPILE_ERROR
        auto ps3 = ps;
        decltype(ps) ps4;
        ps4 = ps;
#endif
    }

    struct FwdDeclare;
    struct ClassWithFwdPtr
    {
        core::clone_ptr<FwdDeclare> ps;
    };

    struct RawClonable
    {
        RawClonable * clone() const { return new RawClonable(*this); }
        int i = 0;
    };

    struct UniquePtrClonable
    {
        std::unique_ptr<UniquePtrClonable> clone() const { return std::make_unique<UniquePtrClonable>(*this); }
        int i = 0;
    };


    template <typename C>
    void testClone()
    {
        C c;
        core::clone_ptr<C> ps(c);
        auto ps2 = std::move(ps);
        assert(!ps);
        auto ps3 = ps2;
        ps2->i = 2;
        assert(ps3->i == 0);
        decltype(ps) ps4;
        ps4 = ps;
    }

    void test()
    {
        testNoClone();
        testClone<RawClonable>();
        testClone<UniquePtrClonable>();
    }
}

int main()
{
    other::test();
    assert( true );
}

// cl  -EHsc -I../include/nonstd/ 03-clone_ptr.cpp && 03-clone_ptr.exe
// g++ -std=c++11 -I../include/nonstd/ -o 03-clone_ptr.exe 03-clone_ptr.cpp && 03-clone_ptr.exe
