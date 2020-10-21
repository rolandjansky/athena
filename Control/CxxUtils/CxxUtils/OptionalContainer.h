/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

 * @file CxxUtils/OptionalContainer.h
 * @author Stephen Nicholas Swatman <stephen.nicholas.swatman@cern.ch>
 * @date September 2020
 * @brief A container of optional values, with ergonomic methods.
 */

#ifndef CXXUTILS_OPTIONALCONTAINER_H
#define CXXUTILS_OPTIONALCONTAINER_H

#include <optional>
#include <array>
#include <vector>

namespace {
    /**
     * @brief Helper struct to allow us to partially apply the size parameter
     * to std::array.
     *
     * This class allows us to partially apply the std::array templated class
     * with only the size argument, allowing us to apply the contained type
     * typename later, completing the type.
     *
     * If this seems like an odd idea, it may be useful to approach the problem
     * from a type theoretical view, and abstract away from the cold hard C++
     * for just a moment. Specifically, we need to realize that std::array
     * (without the template parameters applied) is essentially a binary
     * function _at the type level_ (called a type constructor); it takes an
     * argument of kind (which is what we call "types" in the typed lambda
     * calculus one level above the base) typename, and a size_t, and returns
     * a concrete type. In C++ like syntax, it's a function at the type level
     * with the following signature:
     *
     * typename std::array(typename T, size_t N);
     *
     * If you're more familiar with a functional syntax like Haskell's, then
     * firstly I weep for you because trying to do any type-level computation
     * in C++ is a pain, but you may also find the following syntax more easy
     * to read (assuming DataKinds):
     *
     * std::array :: * -> Integer -> *
     *
     * Standard C++ syntax allows us to only fully apply these types, we can't
     * apply them partially (just like how we cannot partially apply normal
     * functions). In other words, we cannot pass std::array a contained type
     * only, capturing that typename in a way reminiscent of a closure, giving
     * us a partially applied type constructor with signature:
     *
     * template <typename> typename std::array'(size_t N);
     * std::array :: Integer -> *
     *
     * And we certainly cannot partially apply in the reverse order, passing it
     * a size_t for the array size and then passing it the container type to
     * get a partially applied type constructor:
     *
     * template <size_t> typename std::array''(typename T);
     * std::array :: * -> *
     *
     * However, this is something we would very much like to do. Indeed, we
     * want our optional container to be relatively type-agnostic, and for that
     * to be possible the type of our OptionalBase has to be (where C is our
     * underlying container type):
     *
     * typname OptionalBase(typename (*C)(typename), ...);
     * OptionalBase :: (* -> *) -> ... -> *
     *
     * So, for example, we could pass the std::vector type constructor as a
     * template parameter of OptionalBase directly, because it has a single
     * argument of kind typename. However, std::array does not do this by
     * default, since it has two arguments. We need to partially apply the size
     * argument before we can pass it to OptionalBase in a way that is similar
     * to the way we would pass std::vector.
     *
     * Wrapping std::array in a templated class with a templated alias allows
     * us this. Indeed, PartiallyAppliedArray has the type:
     *
     * typename PartiallyAppliedArray(size_t);
     *
     * So, for example, we can construct the type PartiallyAppliedArray<10> to
     * get a struct wrapping std::array with the size parameter partially
     * applied with the value 10. This type then contains a type alias, type,
     * or PartiallyAppliedArray<10>::type which is a unary type constructor
     * with a single argument, which is exactly what we want. So, we can now
     * pass this type directly to OptionalBase, or we can construct an array of
     * 10 integers from it by writing PartiallyAppliedArray<10>::type<int>.
     *
     * @tparam N the size of the array.
     */
    template<std::size_t N>
    struct PartiallyAppliedArray {
        template<typename T>
        using type = std::array<T, N>;
    };

    /**
     * @brief Helper struct to give compile-time info about static castability.
     *
     * In our following definitions, we want to know whether a type A is
     * castable to a type B. For this, we need to use some helper structs which
     * we can use in static assertsions. This is relatively standard procedure
     * for C++ template metaprogramming, and recipes and explanations of this
     * kind of code can be found all across the internet.
     *
     * This first type is the default case, were F turns out to not be
     * statically castable to T. It subclasses the false type, so ::value will
     * return false.
     */
    template <class F, class T, class=T>
    struct is_static_castable : std::false_type
    {};

    /**
     * This is the specialized sibling of the type above, in which F can indeed
     * be converted to T. It subclasses the true type, so ::value will return
     * true.
     */
    template <class F, class T>
    struct is_static_castable<F, T, decltype(static_cast<T>(std::declval<F>()))> : std::true_type
    {};
}

namespace CxxUtils {
    /**
     * @brief The base class representing a container filled with optional
     * values.
     *
     * This is the meat of the pudding of our optional container types. It
     * allows us to check whether values are set, set them, unset them, and a
     * few more things.
     *
     * @tparam C the underlying container type, must take a single typename
     * argument representing the contained type.
     * @tparam I the index type, which will usually be size_t but can also be
     * any value which is convertible to size_t, or other types if the
     * container is map-like.
     * @tparam V the contained type, without the optional type constructor
     * applied to it.
     *
     * As an example, OptionalBase<std::vector, MyEnum, std::string> is an
     * optional container backed by a vector, indexed by an enumeration type,
     * containing optional strings.
     *
     * Of course, I could be derived easily from C<std::optional<V>>::size_type
     * but the approach of having a separately and explicitly defined template
     * variable for this is more powerful, since it also allows us to index
     * with, for example, enumeration types in a type-safe fashion.
     */
    template<template <typename> typename C, typename I, typename V>
    class OptionalBase {
    public:
        /**
         * F is the inner type, which is the type constructor of the underlying
         * container type C applied to the std::optional type constructor,
         * applied in turn to the value type V.
         */
        using F = C<std::optional<V>>;

        static_assert(
            is_static_castable<I, typename F::size_type>::value,
            "Index type must be castable to storage class index type!"
        );

        /**
         * @brief Check whether the specified index is set.
         *
         * This method is used to determine whether a value in the array is set
         * or unset.
         *
         * @param i the index to check.
         * @return true if the value at i is set, false otherwise.
         */
        bool is_set(const I & i) const;

        /**
         * @brief Update an index that is already set.
         *
         * This method updates the value at a given index, but only if that
         * value is already set. If it is unset, this method is a noop.
         *
         * @param i the index to update.
         * @param v the value to be set.
         * @tparam W the value type, should be convertible to V.
         * @return true if the update succeeds, otherwise false.
         *
         * @note The fact that this method is templated with a seemingly
         * useless template variable W might be confusing, but it has to do
         * with a concept that is colloquially known as universal references,
         * where a single function can accept both lvalue references and rvalue
         * references in an efficient way. This relies on templating, and
         * requires a separate function level template separate from the class
         * level template variable V.
         */
        template<typename W>
        bool update(const I & i, W && v);

        /**
         * @brief Set an index that is not already set.
         *
         * This method sets the value at a given index, but only if that value
         * is not already set. If the value is set, this method is a noop.
         *
         * @param i the index to set.
         * @param v the value to be set.
         * @tparam W the value type, should be convertible to V.
         * @return true if the set succeeds, otherwise false.
         *
         * @note See the comment on update() for more info about W.
         */
        template<typename W>
        bool set_if_unset(const I & i, W && v);

        /**
         * @brief Update the value of a key, regardless of existing value.
         *
         * This method inserts a new value at a given index, regardless of
         * whether a value already exists at that location. This operation
         * always succeeds.
         *
         * @param i the index to set.
         * @param v the value to be set.
         * @tparam W the value type, should be convertible to V.
         * @return true, always.
         *
         * @note The return value of this method is superfluous, but it's there
         * to preserve the symmetry with the update and set methods.
         * @note See the comment on update() for more info about W.
         */
        template<typename W>
        bool set(const I & i, W && v);

        /**
         * @brief Unset the value at a given key.
         *
         * This method removes the value at a given index, and sets the value
         * to undefined. This also deletes the old value. This operation always
         * succeeds.
         *
         * @param i the index to unset.
         */
        void unset(const I & i);

        /**
         * @brief Increment the value at a given index by one.
         *
         * This method increases the value of a given key by one. For this
         * method to exist, the underlying type T must be incrementable. As of
         * C++20, this can be represented using the std::incrementable
         * constraint, but not yet.
         *
         * @param i the index to increment.
         */
        void increment(const I & i);

        /**
         * @brief Get the value at a given key (const).
         *
         * Getter for the value at a given location. Returns an optional
         * reference, since we cannot guarantee that the object exists.
         *
         * @param i the index to retrieve.
         * @return a reference to the optional value held at the index.
         */
        const std::optional<V> & get(const I & i) const;

        /**
         * @brief Get the value at a given key.
         *
         * Same as get(), but not const.
         *
         * @param i the index to retrieve.
         * @return a reference to the optional value held at the index.
         */
        std::optional<V> & get(const I & i);

        /**
         * @brief Coalesce a single value in the array, giving priority to the
         * left hand side.
         *
         * Updates a field in the array, giving priority to the left hand side.
         * The workings of this methods are concisely described by the
         * following case analysis:
         *
         * v << w = case (v, w) of:
         *     Just u, _ = Just u
         *     _,      u = u
         *
         * The argument value takes std::optional parameters, but the method
         * may also be passed V directly which will then be boxed implicitly.
         *
         * @param i the index to coalesce at.
         * @param v the value to coalesce with.
         */
        void coalesce_l(const I & i, const std::optional<V> & v);

        /**
         * @brief Coalesce a single value in the array, giving priority to the
         * right hand side.
         *
         * Same as coalesce_l(), except the case analysis is as follows:
         *
         * v >> w = case (v, w) of:
         *     _, Just u = Just u
         *     u, _      = u
         *
         * @param i the index to coalesce at.
         * @param v the value to coalesce with.
         */
        void coalesce_r(const I & i, const std::optional<V> & v);

        /**
         * @brief Mutable accessor to the underlying container.
         *
         * A simple mutable accessor to the underlying container of this class.
         *
         * @return a mutable reference to the underlying container.
         */
        F & data(void);

        /**
         * @brief Immutable accessor to the underlying container.
         *
         * A simple immutable accessor to the underlying container of this
         * class.
         *
         * @return an immutable reference to the underlying container.
         */
        const F & data(void) const;

    protected:
        F m_data;
    };

    /**
     * @brief Specialization of OptionalBase indexed by discrete, countable
     * indices.
     *
     * If we know that our indices are contiguous and discrete, this allows us
     * additional power, such as allowing us to coalesce over the entire set of
     * values.
     *
     * Template parameters are the same as for OptionalBase.
     */
    template<template <typename> typename C, typename I, typename V>
    class OptionalArrayLike : public OptionalBase<C, I, V> {
        static_assert(
            is_static_castable<I, std::size_t>::value,
            "Index type must be castable to std::size_t!"
        );

    public:
        /**
         * @brief Coalesce this array with another, giving priority to the left
         * hand side.
         *
         * Updates the values in the array according to the data in the right
         * hand side array, giving priority to the left hand side. This method
         * changes the values of the array on which it is called.
         *
         * For example, if we abuse notation and write left-coalesce as <<, we
         * find the following:
         *
         * [5, {}, 7, {}] << [2, 9, {}, {}] = [5, 9, 7, {}]
         *
         * @param rhs the right hand side array.
         */
        void coalesce_all_l(const OptionalArrayLike<C, I, V> & rhs);

        /**
         * @brief Coalesce this array with another, giving priority to the
         * right hand side.
         *
         * See coalesce_l() for more information. As an example, if we abuse
         * notation and write right-coalesce as >>, we find the following:
         *
         * [5, {}, 7, {}] >> [2, 9, {}, {}] = [2, 9, 7, {}]
         *
         * @param rhs the right hand side track summary.
         */
        void coalesce_all_r(const OptionalArrayLike<C, I, V> & rhs);
    };

    /**
     * @brief An alias to a specialization of an optional array.
     *
     * This is the only pecialization we provide so far, and it's for the array
     * backend. The template arguments are compatible with those of std::array,
     * with the exception that it can optionally take an indexing type as well.
     *
     * @tparam V the underlying value type.
     * @tparam N the size of the array.
     * @tparam I the indexing type, size_t by default.
     *
     * This is where our PartiallyAppliedArray class comes into play, as it
     * allows us to create a partially applied array type of given size.
     */
    template<typename V, std::size_t N, typename I=std::size_t>
    using OptionalArray = OptionalArrayLike<PartiallyAppliedArray<N>::template type, I, V>;
}

#include "CxxUtils/OptionalContainer.icc"

#endif
