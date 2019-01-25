/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ThinningUtils_ApplyFunctorSwitch_H
#define ThinningUtils_ApplyFunctorSwitch_H

#include <typeinfo>
#include <memory>
#include <utility>
#include <exception>
#include <tuple>
#include <type_traits>

/**
 * @file
 * @brief Helper variadic functions for performing a template switch to pick an
 *        appropriate function to call.
 *
 * **COMPONENTS**:
 *
 * There are several components involved in these functions. 
 *
 * The most important one is a functor-like object (F) which holds the candidate
 * functions. The candidate functions should all be called apply and
 * distinguishable from an explicit template parameter and the signature of the
 * arguments (which are passed to it using perfect forwarding). Multiple
 * versions of apply<T> can be present with different signatures but each
 * signature must be present for each candidate template type T.
 *
 * The return type can be dependent on the template parameter but in this case
 * they must all be convertible to a common base class (the Return parameter in
 * the applySwitch function).
 *
 * See the ConstructorHelper struct for an example.
 *
 * Another component is the first template parameter 'Return'. This is the
 * common return type of all the functors (or a type to which they are all
 * convertible). In the case of the ConstructorHelper this is a std::unique_ptr
 * to the 'Base' class.
 *
 * The third component is the template list. This is a list of candidate
 * template types to be checked against the type_info object. The one that
 * matches will be used to template the functor which gets called.
 *
 * The fourth component is the type_info object which is passed as the first
 * argument.
 *
 * The final component is the arguments for the functor. These are passed as all
 * the remaining arguments to the applySwitch function and are forwarded through
 * to the functor.
 */
namespace FunctorHelpers {

  /**
   * @class UnknownTypeException
   * @brief Report that the provided type_info didn't correspond to a type in
   *        template switch.
   */
  class UnknownTypeException : public std::invalid_argument
  {
    public:
      UnknownTypeException(const std::type_info& ti)
        : std::invalid_argument("TypeInfo '" + std::string(ti.name() )
            + "' does not match any known type!"),
          ti(ti) {}
      const std::type_info& ti;
  };

  /**
   * @brief Call the function using the supplied arguments.
   * @tparam F The type of the functor-like object.
   * @tparam Args The argument parameter pack.
   * @tparam I The index pack for Args.
   * @param f The functor-like object.
   * @param args The argument tuple.
   *
   * The arguments for construct have been packaged into a tuple, this function
   * takes this tuple and unwraps it back into a parameter list.
   */
  template <typename T,
            typename F, 
            typename... Args, 
            std::size_t... I>
    decltype(auto) applyCall_impl(
        F&& f,
        const std::tuple<Args...>& args,
        std::index_sequence<I...>)
    {
      return f.template apply<T>(std::forward<Args>(std::get<I>(args) )...);
    }

  /**
   * @brief Call the function using the supplied arguments.
   * @tparam F The type of the functor-like object.
   * @tparam Args The argument parameter pack.
   * @param f The functor-like object.
   * @param args The argument tuple.
   *
   * The arguments for construct have been packaged into a tuple, this function
   * takes this tuple and unwraps it back into a parameter list.
   */
  template <typename Return,
            typename T, 
            typename F, 
            typename... Args>
    Return applyCall(
        F&& f,
        const std::tuple<Args...>& args)
    {
      return applyCall_impl<T>( 
          std::forward<F>(f), args,
          std::make_index_sequence<sizeof...(Args)>{});
    }

  /**
   * @brief Base case of variadic template.
   * @tparam Return The return value, the functor's return value must be
   *         convertible to this.
   * @tparam F The type of the functor-like object.
   * @tparam ArgTuple The functor's argument parameter tuple type.
   * @tparam T The candidate type for the match to the type_info object.
   * @param ti The type_info object being considered.
   * @param f The functor-like object.
   * @param args The arguments for the functor.
   *
   * Checks the type_info against the final candidate type T. If this doesn't
   * match throw an UnknownTypeException. If it does, call the functor.
   */
  template <typename Return,
            typename F,
            typename ArgTuple,
            typename T>
    Return applySwitch_impl(
        const std::type_info& ti,
        F&& f,
        const ArgTuple& args,
        const T*)
    {
      // Rearrange for the base case so we don't run afoul of gcc not liking
      // program branches without return types.
      if (ti != typeid(T) )
        throw UnknownTypeException(ti);
      return applyCall<Return, T>(std::forward<F>(f), args);
    }

  /**
   * @brief Main part of variadic template.
   * @tparam Return The return value, the functor's return value must be
   *         convertible to this.
   * @tparam F The type of the functor-like object.
   * @tparam ArgTuple The functor's argument parameter tuple type.
   * @tparam T The candidate type for the match to the type_info object.
   * @tparam Ts The remaining candidate types.
   * @param ti The type_info object being considered.
   * @param f The functor-like object.
   * @param args The arguments for the functor.
   * @param ts The remaining dummy pointers.
   *
   * Checks the type_info against the final candidate type T. If this doesn't
   * match unwind the next template type. If it does, call the relevant functor.
   */
  template <typename Return,
            typename F,
            typename ArgTuple, 
            typename T, 
            typename... Ts>
    Return applySwitch_impl(
        const std::type_info& ti,
        F&& f,
        const ArgTuple& args,
        const T*,
        const Ts*... ts)
    {
      if (ti == typeid(T) )
        return applyCall<Return, T>(std::forward<F>(f), args);
      return applySwitch_impl<Return>(
          ti, std::forward<F>(f), args, ts...);
    }

  /**
   * @brief Call a templated functor-like object whose template type is
   *        determined from a type_info and a set of compile-time known types.
   * @tparam Return The desired return value from this function, the return
   *         values of all possible functors must be convertible to this.
   * @tparam Ts The set of candidate types.
   * @tparam F The type of the functor-like object.
   * @tparam Args The arguments accepted by the functors.
   * @param ti The type_info object used to determine the template type.
   * @param f The functor-like object.
   * @param args The arguments for the functor.
   */
  template <typename Return,
            typename... Ts,
            typename F,
            typename... Args>
    Return applySwitch(
        const std::type_info& ti,
        F&& f,
        Args&&... args)
    {
      return applySwitch_impl<Return>(ti, std::forward<F>(f),
          std::forward_as_tuple(std::forward<Args>(args)...),
          static_cast<Ts*>(nullptr)...);
    }

  /**
   * @brief Helper functor that constructs a std::unique_ptr to the templated
   *        type.
   * @tparam Constructed The template class to be constructed.
   */
  template <template <typename> class Constructed>
    struct ConstructorHelper 
  {
    /**
     * @brief Create an instance of Constructed<T> using the supplied arguments
     * and return a unique_ptr to it.
     * @tparam T The template parameter to use.
     * @tparam Args The constructor argument types.
     * @param args The constructor arguments.
     */
    template <typename T, typename... Args>
      std::unique_ptr<Constructed<T>> apply(Args&&... args) const
      {
        return std::make_unique<Constructed<T>>(
            std::forward<Args>(args)...);
      }
  };

  /**
   * @brief Helper function to construct a templated object whose type is
   *        determined from a type_info.
   * @tparam Base A base type common to all objects that could possibly be
   *         constructed by this function.
   * @tparam Constructed Template type to be constructed.
   * @tparam Ts The candidate types to match against the type_info.
   * @tparam Args The arguments to the constructors.
   * @param ti The type_info object being considered.
   * @param args The arguments to be supplied to the constructors.
   *
   * A unique_ptr to the Base class will be returned so all specialisations of
   * the Constructed template must inherit publicly from this type.
   */
  template <typename Base,
            template <typename> class Constructed,
            typename... Ts,
            typename... Args>
    std::unique_ptr<Base> constructSwitch(
        const std::type_info& ti,
        Args&&... args)
    {
      return applySwitch<
        std::unique_ptr<Base>, 
        Ts...>(ti, ConstructorHelper<Constructed>{}, std::forward<Args>(args)...);
    }
}
#endif //> ThinningUtils_ApplyFunctorSwitch_H
