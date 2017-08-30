// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/ClassName.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Recursively separate out template arguments in a C++ class name.
 */


#ifndef CXXUTILS_CLASSNAME_H
#define CXXUTILS_CLASSNAME_H


#include <string>
#include <vector>
#include <map>
#include <stdexcept>


namespace CxxUtils {


/**
 * @brief Recursively separate out template arguments in a C++ class name.
 *
 * This class allows making some simple transformations of C++ class names.
 * For example, given these rules:
 *
 *@code
 * ClassName::Rules rules;
 *
 *  rules.add ("std::vector<$T, std::allocator<$T> >",
 *             "std::vector<$T>");
 *  rules.add ("std::map<$K,$V, std::less<$K>, std::allocator<std::pair<const $K,$V> > >",
 *             "std::map<$K,$V>");
 *  rules.add ("DataVector<$T, $B>", "DataVector<$T>");
 *  rules.add ("std::__1", "std");
 @endcode
 *
 * then `rules.apply` can make transformations like this:
 *
 *@code
 *
 * std::__1::vector<std::__1::vector<int, std::__1::allocator<int> >, std::__1::allocator<std::__1::vector<int, std::__1::allocator<int> > > > 
 *   -> std::vector<std::vector<int> >
 *
 * std::map<int, float, std::less<int>, std::allocator<std::pair<const int, float> > >
 *   -> std::map<int, float>
 *
 * DataVector<Foo, DataModel_detail::NoBase>
 *   -> DataVector<Foo>
 @endcode
 *
 * In slightly more detail: this class analyzes C++ class names.  A name
 * like 
 *
 *@code
 *   A::B<int, double> 
 @endcode
 *
 * is broken down like this:
 *
 *  - Name: `B`.
 *  - Namespace: `A`.
 *  - Template arguments: `int` and `double`.
 *
 *  This is done recursively; both the namespace and template argument
 *  pieces can be further broken down like this.  A name can also
 *  be marked as `const', but no other parsing of C-like declarators
 *  is done.
 *
 *  Parsed names can be matched against simple patterns like this:
 *  
 *@code
 *  A::B<$T>
 @endcode
 *
 * and the variable `T` gets set to the corresponding piece of the type
 * being matched.  For example, given the above pattern,
 *
 *  - `A::C<int>` matches with `T` set to `int`.
 *  - `A::C<const Foo<Bar> >` matches with `T` set to `const Foo<Bar>`.
 *
 * If the pattern were `A::B<const $T>`, then `A::B<const int>` would
 * match with `T` set to `int`, but `A::B<int>` would not match.
 *
 * However, the root name of a pattern may not be a variable; for example,
 * you can't use `A::$T<int>` as a pattern.
 *
 * You can also substitute variables back into a pattern; for example,
 * 
 *  - `T=int` into `A::B<$T>` gives `A::B<int>`.
 *  - `T=const int` into `A::B<$T>` gives `A::B<const int>`.
 *  - `T=const int` into `A::B<const int>` gives `A::B<const int>`.
 */
class ClassName
{
public:

  /**
   * @brief Exception to signal a malformed class name.
   */
  class ExcBadClassName
    : public std::runtime_error
  {
  public:
    ExcBadClassName (const std::string& name);
  };


  /**
   * @brief Exception to signal a missing variable.
   */
  class ExcMissingVariable
    : public std::runtime_error
  {
  public:
    ExcMissingVariable (const std::string& var);
  };


  /**
   * @brief A set of transformation rules to use with @c ClassName.
   */
  class Rules
  {
  public:
    /**
     * @brief Return the number of defined rules.
     */
    size_t size() const;


    /**
     * @brief Add a new transformation rule.
     * @param pattern The pattern to match.
     * @param replacement The expression with which to replace it.
     *
     * The pattern may contain variables that are then substituted into
     * the replacement; for example, given a pattern of `A<$T, $T>` and
     * a replacement of `B<$T>`, then `A<Foo<int>, Foo<int> >` would
     * be transformed to `B<Foo<int> >'.
     */
    void add (const ClassName& pattern, const ClassName& replacement);


#if __cplusplus > 201100
    /**
     * @brief Add a new transformation rule (move version).
     * @param pattern The pattern to match.
     * @param replacement The expression with which to replace it.
     *
     * The pattern may contain variables that are then substituted into
     * the replacement; for example, given a pattern of `A<$T, $T>` and
     * a replacement of `B<$T>`, then `A<Foo<int>, Foo<int> >` would
     * be transformed to `B<Foo<int> >'.
     */
    void add (ClassName&& pattern, ClassName&& replacement);
#endif


    /**
     * @brief Apply the set of transformation rules to a class name object.
     * @param cn The object to which the rules should be applied.
     *           Will be modified in place.
     *
     * All transformation rules are matched against @c cn.  If any match,
     * the object is replaced with the replacement portion of the rule
     * with match results substituted.
     *
     * Returns true if any matches were made and false otherwise.
     */
    bool applyTo (ClassName& cn) const;


    /**
     * @brief Apply transformations to a class name.
     * @param name The class name to transform.
     *
     * Returns the transformed class name.
     *
     * This is just shorthand for calling `ClassName::applyRules`.
     */
    std::string apply (const std::string& name) const;


  private:
    /// A pattern and replacement.
    typedef std::pair<ClassName, ClassName> pat_repl_t;

    /// Map from the root of a pattern to the pattern, replacement pair.
    typedef std::multimap<std::string, pat_repl_t> rulemap_t;
    rulemap_t m_rules;
  };


  /// Map used to hold variable assignments from matching.
  typedef std::map<std::string, ClassName> match_t;


  /**
   * @brief Default constructor.
   *
   * Needed for STL compatibility.
   */
  ClassName ();


  /**
   * @brief Parse a class name into component parts.
   * @param name The name to parse.
   *
   * Raises a @c BadClassName exception if the name isn't completely parsed.
   */
  ClassName (const char* name);


  /**
   * @brief Parse a class name into component parts.
   * @param name The name to parse.
   *
   * Raises a @c BadClassName exception if the name isn't completely parsed.
   */
  ClassName (const std::string& name);


  /**
   * @brief Parse a class name into component parts.
   * @param name String containing the name to parse.
   * @param pos Position in the string at which parsing should start.
   *
   * @c pos is updated to point to past the point where parsing stopped.
   */
  ClassName (const std::string& name, std::string::size_type& pos);


  /**
   * @brief Swap this expression with another one.
   * @param other The other expression with which to swap.
   */
  void swap (ClassName& other);


  /**
   * @brief Get the const flag for this expression.
   */
  bool isConst() const;


  /**
   * @brief Set the const flag for this expression.
   */
  void setConst();


  /**
   * @brief Return the root name of the expression.
   *
   * In `A::B<C>`, the root name is `B`.
   */
  std::string name() const;

  
  /**
   * @brief Return the namespace-qualified name of the expression.
   *
   * In `A::B<C>`, this is `A::B`.
   */
  std::string qualifiedName() const;


  /**
   * @brief Return the full name of the expression.
   */
  std::string fullName() const;


  /**
   * @brief Test two expressions for equality.
   */
  bool operator== (const ClassName& other) const;


  /**
   * @brief Test two expressions for inequality.
   */
  bool operator!= (const ClassName& other) const;


  /**
   * @brief Match this expression against a pattern.
   * @param pattern The pattern to match.
   * @param[out] matches Dictionary of pattern substitutions.
   *
   * Return true if @c pattern matches the current expression.
   * @c pattern may contain dummy variables of the form `$T`.
   * On a successful return, the map @c matches contains the
   * variable assignments needed for the match.
   */
  bool match (const ClassName& pattern, match_t& matches) const;


  /**
   * @brief Substitute variables into this expression.
   * @param The dictionary of variables to substitute.
   *
   * If this expression contains variables like `$T`, they are replaced
   * with the corresponding values from @c matches.  If a variable is
   * present in the expression but is not in @c matches, @c ExcMissingVariable
   * is thrown.
   *
   * The substitutions are made in-place.
   */
  void subst (const match_t& matches);


  /**
   * @brief Return a copy of this expression with variables substituted.
   * @param The dictionary of variables to substitute.
   *
   * If this expression contains variables like `$T`, they are replaced
   * with the corresponding values from @c matches.  If a variable is
   * present in the expression but is not in @c matches, @c ExcMissingVariable
   * is thrown.
   *
   * The substitutions are made in a copy of the expression, which is returned.
   */
  ClassName substCopy (const match_t& matches) const;


  /**
   * @brief Apply a set of transformation rules to this object.
   * @param rules The set of rules to apply.
   *
   * Recursively walk this expression, trying to apply the transformation
   * rules in @c rules.  If any matches are found, this expression
   * is modified in-place and the walk is repeated.  This function terminates
   * when no further matches are found.
   *
   * Warning: An infinite loop is possible if the replacement for a pattern
   * can always be matched by another pattern.
   */
  void applyRules (const Rules& rules);


  /**
   * @brief Apply a set of transformation rules a class name.
   * param The name of the class to transform.
   * @param rules The set of rules to apply.
   *
   * This is just shorthand for
   *
   *@code
   *  ClassName cn (name);
   *  cn.applyRules (rules);
   *  return cn.fullName();
   @endcode
   */
  static std::string applyRules (const std::string& name,
                                 const Rules& rules);


private:
  /**
   * @brief Parse a string into a @c ClassName.
   * @param name The string containing the name.
   * @param pos Position in the string to start parsing.
   *
   * On return, @c pos will be updated to point just past the last
   * character consumed.
   */
  void parse (const std::string& name, std::string::size_type& pos);


  /**
   * @brief Parse a primary part of the class name.
   * @param name The string containing the name.
   * @param pos Position in the string to start parsing.
   *
   * The primary part of the class name is a string without namespace
   * and template delimiters.
   *
   * On return, @c pos will be updated to point just past the last
   * character consumed.
   */
  std::string parsePrimary (const std::string& name,
                            std::string::size_type& pos);


  /**
   * @brief Parse a namespace qualification.
   * @param name The string containing the name.
   * @param pos Position in the string to start parsing.
   *
   * When this is called, the namespace part has already been parsed,
   * and the next two characters in @c name are `::`.  This reads in the
   * remainder of the string as a @c ClassName, and then moves it inside
   * the namespace given by the current object.
   *
   * On return, @c pos will be updated to point just past the last
   * character consumed.
   */
  void parseNamespace (const std::string& name, std::string::size_type& pos);


  /**
   * @brief Parse the template part of a name.
   * @param name The string containing the name.
   * @param pos Position in the string to start parsing.
   *
   * When this is called, the qualified name part of the name has already
   * been parsed, and the next character in @c name is `::`.  This reads in
   * template arguments from @c name.
   *
   * On return, @c pos will be updated to point just past the last
   * character consumed.
   */
  void parseTemplateArgs (const std::string& name, std::string::size_type& pos);


  /**
   * @brief Skip past spaces in a string.
   * @param name The string containing the name.
   * @param pos Position in the string to start skipping.
   *
   * On return, @c pos will be updated to point just past the last
   * character consumed.
   */
  void skipSpaces (const std::string& name, std::string::size_type& pos);


  /**
   * @brief Match this expression against a pattern.
   * @param pattern The pattern to match.
   * @param topLevel True if this is the outermost level of matching.
   * @param[out] matches Dictionary of pattern substitutions.
   *
   * Return true if @c pattern matches the current expression.
   * @c pattern may contain dummy variables of the form `$T`.
   * On a successful return, the map @c matches contains the
   * variable assignments needed for the match.
   */
  bool match1 (const ClassName& pattern,
               bool topLevel,
               match_t& matches) const;


  /**
   * @brief Apply a set of transformation rules to this object.
   * @param rules The set of rules to apply.
   *
   * Recursively walk this expression, trying to apply the transformation
   * rules in @c rules.  If any matches are found, this expression
   * is modified in-place.
   *
   * Returns true if any matches were found.
   */
  bool applyRules1 (const Rules& rules);


  /// Is this expression const?
  bool m_const;

  /// The containing namespace.
  /// This vector is always either 0 or 1 elements long; this is a way of
  /// getting something sort of like a pointer but with completely automatic
  /// management.
  std::vector<ClassName> m_namespace;

  /// The primary name part of this expression.
  std::string m_name;

  /// The template arguments for this name.
  std::vector<ClassName> m_targs;
};


} // namespace CxxUtils


#endif // not CXXUTILS_CLASSNAME_H
