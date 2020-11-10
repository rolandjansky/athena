#undef NDEBUG

#include <cassert>
#include <cstddef>
#include <memory>

#include "CxxUtils/OptionalContainer.h"


class Test {
public:
    Test(int v) : m_t(v) {

    };

    int get() {
        return m_t;
    };
private:
    int m_t;
};


enum class TestEnum {
    Nil = 0,
    One = 1,
    Two = 2,
    Three = 3,
    Max = 4
};


void test_init(void) {
    CxxUtils::OptionalArray<int, 2> A1;

    assert(!A1.get(0).has_value());
    assert(!A1.get(1).has_value());
}

void test_set(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    assert(A1.set_if_unset(0, 5));
    assert(!A1.set_if_unset(0, 2));
    assert(A1.set_if_unset(1, 6));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    assert(A1.get(0) == 5);
    assert(A1.get(1) == 6);
}

void test_copy_ctor(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 6);

    CxxUtils::OptionalArray<int, 3> A2(A1);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    assert(A1.get(0) == 5);
    assert(A1.get(1) == 6);

    assert(A2.is_set(0));
    assert(A2.is_set(1));
    assert(!A2.is_set(2));

    assert(A2.get(0) == 5);
    assert(A2.get(1) == 6);
}

void test_copy_op(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 6);

    CxxUtils::OptionalArray<int, 3> A2 = A1;

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    assert(A1.get(0) == 5);
    assert(A1.get(1) == 6);

    assert(A2.is_set(0));
    assert(A2.is_set(1));
    assert(!A2.is_set(2));

    assert(A2.get(0) == 5);
    assert(A2.get(1) == 6);
}

void test_update(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    assert(A1.set_if_unset(0, 5));
    assert(A1.set_if_unset(1, 6));

    assert(A1.update(0, 2));
    assert(A1.update(1, 7));
    assert(!A1.update(2, 5));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    assert(A1.get(0) == 2);
    assert(A1.get(1) == 7);
}

void test_upset(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    assert(A1.set_if_unset(0, 5));
    assert(A1.set_if_unset(1, 6));

    assert(A1.set(0, 2));
    assert(A1.set(1, 7));
    assert(A1.set(2, 5));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));

    assert(A1.get(0) == 2);
    assert(A1.get(1) == 7);
    assert(A1.get(2) == 5);
}

void test_unset(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    assert(A1.set_if_unset(0, 5));
    assert(A1.set_if_unset(1, 6));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    A1.unset(1);

    assert(A1.is_set(0));
    assert(!A1.is_set(1));
    assert(!A1.is_set(2));
}

void test_increment(void) {
    CxxUtils::OptionalArray<int, 3> A1;

    assert(A1.set_if_unset(0, 5));

    assert(A1.is_set(0));
    assert(!A1.is_set(1));
    assert(!A1.is_set(2));

    A1.increment(0);
    A1.increment(1);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(!A1.is_set(2));

    assert(A1.get(0) == 6);
    assert(A1.get(1) == 1);
}

void test_coalese_l_whole(void) {
    CxxUtils::OptionalArray<int, 4> A1;
    CxxUtils::OptionalArray<int, 4> A2;

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 7);

    A2.set_if_unset(0, 11);
    A2.set_if_unset(2, 9);

    A1.coalesce_all_l(A2);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));
    assert(!A1.is_set(3));

    assert(A1.get(0) == 5);
    assert(A1.get(1) == 7);
    assert(A1.get(2) == 9);
}

void test_coalese_r_whole(void) {
    CxxUtils::OptionalArray<int, 4> A1;
    CxxUtils::OptionalArray<int, 4> A2;

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 7);

    A2.set_if_unset(0, 11);
    A2.set_if_unset(2, 9);

    A1.coalesce_all_r(A2);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));
    assert(!A1.is_set(3));

    assert(A1.get(0) == 11);
    assert(A1.get(1) == 7);
    assert(A1.get(2) == 9);
}

void test_coalese_l_value(void) {
    CxxUtils::OptionalArray<int, 4> A1;
    std::optional<int> V1 = 3;
    std::optional<int> V2 = {};
    std::optional<int> V3 = 9;
    std::optional<int> V4 = {};

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 7);

    A1.coalesce_l(0, V1);
    A1.coalesce_l(1, V2);
    A1.coalesce_l(2, V3);
    A1.coalesce_l(3, V4);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));
    assert(!A1.is_set(3));

    assert(A1.get(0) == 5);
    assert(A1.get(1) == 7);
    assert(A1.get(2) == 9);
}

void test_coalese_r_value(void) {
    CxxUtils::OptionalArray<int, 4> A1;
    std::optional<int> V1 = 3;
    std::optional<int> V2 = {};
    std::optional<int> V3 = 9;
    std::optional<int> V4 = {};

    A1.set_if_unset(0, 5);
    A1.set_if_unset(1, 7);

    A1.coalesce_r(0, V1);
    A1.coalesce_r(1, V2);
    A1.coalesce_r(2, V3);
    A1.coalesce_r(3, V4);

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));
    assert(!A1.is_set(3));

    assert(A1.get(0) == 3);
    assert(A1.get(1) == 7);
    assert(A1.get(2) == 9);
}

void test_coalese_upcast_value(void) {
    CxxUtils::OptionalArray<int, 8> A1;

    A1.coalesce_l(0, 5);
    assert(A1.is_set(0));
    assert(A1.get(0) == 5);
    A1.coalesce_r(0, 3);
    assert(A1.is_set(0));
    assert(A1.get(0) == 3);

    A1.coalesce_l(1, {});
    assert(!A1.is_set(1));
    A1.coalesce_r(1, 5);
    assert(A1.is_set(1));
    assert(A1.get(1) == 5);

    A1.coalesce_l(2, 2);
    assert(A1.is_set(2));
    assert(A1.get(2) == 2);
    A1.coalesce_r(2, {});
    assert(A1.is_set(2));
    assert(A1.get(2) == 2);

    A1.coalesce_l(3, {});
    assert(!A1.is_set(3));
    A1.coalesce_r(3, {});
    assert(!A1.is_set(3));
}

void test_copyable_type(void) {
    CxxUtils::OptionalArray<Test, 8> A1;

    Test t1(5);
    Test t2(7);
    const Test t3(9);
    const Test t4(11);

    A1.set_if_unset(0, Test(1));
    A1.set_if_unset(1, t1);
    A1.set_if_unset(2, std::move(t2));
    A1.set_if_unset(3, t3);
    A1.set_if_unset(4, std::move(t4));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));
    assert(A1.is_set(3));
    assert(A1.is_set(4));

    assert(A1.get(0)->get() == 1);
    assert(A1.get(1)->get() == 5);
    assert(A1.get(2)->get() == 7);
    assert(A1.get(3)->get() == 9);
    assert(A1.get(4)->get() == 11);
}

void test_non_copyable_type(void) {
    CxxUtils::OptionalArray<std::unique_ptr<int>, 8> A1;

    std::unique_ptr<int> t1(new int(5));
    std::unique_ptr<int> t2 = std::make_unique<int>(7);

    A1.set_if_unset(0, std::make_unique<int>(1));
    A1.set_if_unset(1, std::move(t1));
    A1.set_if_unset(2, std::move(t2));

    assert(A1.is_set(0));
    assert(A1.is_set(1));
    assert(A1.is_set(2));

    assert(**A1.get(0) == 1);
    assert(**A1.get(1) == 5);
    assert(**A1.get(2) == 7);
}

void test_class_enum_index(void) {
    CxxUtils::OptionalArray<int, static_cast<size_t>(TestEnum::Max), TestEnum> A1;

    A1.set_if_unset(TestEnum::Nil, 2);
    A1.set_if_unset(TestEnum::One, 3);
    A1.set_if_unset(TestEnum::Two, 4);
    A1.set_if_unset(TestEnum::Three, 5);

    assert(A1.is_set(TestEnum::Nil));
    assert(A1.is_set(TestEnum::One));
    assert(A1.is_set(TestEnum::Two));
    assert(A1.is_set(TestEnum::Three));

    assert(*A1.get(TestEnum::Nil) == 2);
    assert(*A1.get(TestEnum::One) == 3);
    assert(*A1.get(TestEnum::Two) == 4);
    assert(*A1.get(TestEnum::Three) == 5);
}

void test_implicit_convert() {
    CxxUtils::OptionalArray<float, 2> A1;

    assert(!A1.is_set(0));
    assert(!A1.is_set(1));

    A1.set_if_unset(0, 5);
    assert(A1.is_set(0));
    A1.update(0, 6);
    assert(A1.is_set(0));
    A1.set(0, 8);
    assert(A1.is_set(0));

    assert(*A1.get(0) == 8.0);
}

int main() {
    test_init();
    test_set();
    test_copy_ctor();
    test_copy_op();
    test_update();
    test_upset();
    test_unset();
    test_increment();
    test_coalese_l_whole();
    test_coalese_r_whole();
    test_coalese_l_value();
    test_coalese_r_value();
    test_coalese_upcast_value();
    test_copyable_type();
    test_non_copyable_type();
    test_class_enum_index();
    test_implicit_convert();
    return 0;
}
