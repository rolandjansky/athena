/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/ClassName.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Recursively separate out template arguments in a C++ class name.
 */


#include "CxxUtils/ClassName.h"
#include <cassert>


namespace CxxUtils {



/**
 * @brief Exception to signal a malformed class name.
 */
ClassName::ExcBadClassName::ExcBadClassName (const std::string& name)
  : std::runtime_error ("ExcBadClassName: Malformed class name: `" +
                        name + "'")
{
}


/**
 * @brief Exception to signal a missing variable.
 */
ClassName::ExcMissingVariable::ExcMissingVariable (const std::string& var)
  : std::runtime_error ("ExcMissingVariable: Variable `" + var +
                        "' referenced in substitution but not present "
                        "in matches map.")
{
}


/**
 * @brief Return the number of defined rules.
 */
size_t ClassName::Rules::size() const
{
  return m_rules.size();
}


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
void ClassName::Rules::add (const ClassName& pattern,
                            const ClassName& replacement)
{
  m_rules.insert (std::make_pair (pattern.name(),
                                  std::make_pair (pattern, replacement)));
}


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
void ClassName::Rules::add (ClassName&& pattern,
                            ClassName&& replacement)
{
  std::string name = pattern.name();
  m_rules.insert (std::make_pair (name,
                                  std::make_pair (std::move(pattern),
                                                  std::move(replacement))));
}
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
bool ClassName::Rules::applyTo (ClassName& cn) const
{
  bool ret = false;
  rulemap_t::const_iterator it = m_rules.find (cn.name());
  while (it != m_rules.end() && it->first == cn.name()) {
    ClassName::match_t matches;
    if (cn.match (it->second.first, matches)) {
      bool const_save = cn.isConst();
      cn = it->second.second.substCopy (matches);
      if (const_save) {
        cn.setConst();
      }
      ret = true;
    }
    ++it;
  }
  return ret;
}


/**
 * @brief Apply transformations to a class name.
 * @param name The class name to transform.
 *
 * Returns the transformed class name.
 *
 * This is just shorthand for calling `ClassName::applyRules`.
 */
std::string ClassName::Rules::apply (const std::string& name) const
{
  return ClassName::applyRules (name, *this);
}


/**
 * @brief Default constructor.
 *
 * Needed for STL compatibility.
 */
ClassName::ClassName ()
  : m_const(false)
{
}


/**
 * @brief Parse a class name into component parts.
 * @param name The name to parse.
 *
 * Raises a @c ExcBadClassName exception if the name isn't completely parsed.
 */
ClassName::ClassName (const char* name)
  : m_const(false)
{
  std::string sname = name;
  std::string::size_type pos = 0;
  parse (sname, pos);
  skipSpaces (sname, pos);
  if (pos != sname.size())
    throw ExcBadClassName (sname);
}


/**
 * @brief Parse a class name into component parts.
 * @param name The name to parse.
 *
 * Raises a @c ExcBadClassName exception if the name isn't completely parsed.
 */
ClassName::ClassName (const std::string& name)
  : m_const(false)
{
  std::string::size_type pos = 0;
  parse (name, pos);
  skipSpaces (name, pos);
  if (pos != name.size())
    throw ExcBadClassName (name);
}


/**
 * @brief Parse a class name into component parts.
 * @param name String containing the name to parse.
 * @param pos Position in the string at which parsing should start.
 *
 * @c pos is updated to point to past the point where parsing stopped.
 */
ClassName::ClassName (const std::string& name, std::string::size_type& pos)
  : m_const (false)
{
  parse (name, pos);
}


/**
 * @brief Swap this expression with another one.
 * @param other The other expression with which to swap.
 */
void ClassName::swap (ClassName& other)
{
  std::swap (m_const, other.m_const);
  std::swap (m_namespace, other.m_namespace);
  std::swap (m_name, other.m_name);
  m_targs.swap (other.m_targs);
}


/**
 * @brief Get the const flag for this expression.
 */
bool ClassName::isConst() const
{
  return m_const;
}


/**
 * @brief Set the const flag for this expression.
 */
void ClassName::setConst()
{
  m_const = true;
}


/**
 * @brief Return the root name of the expression.
 *
 * In `A::B<C>`, the root name is `B`.
 */
std::string ClassName::name() const
{
  return m_name;
}


/**
 * @brief Return the root name of the expression.
 *
 * In `A::B<C>`, the root name is `B`.
 */
std::string ClassName::qualifiedName() const
{
  std::string nsname;
  if (m_namespace.size() > 0)
    nsname = m_namespace[0].fullName() + "::";
  return nsname + m_name;
}


/**
 * @brief Return the full name of the expression.
 */
std::string ClassName::fullName() const
{
  std::string name = qualifiedName();
  if (m_const)
    name = "const " + name;
  if (m_targs.size() > 0) {
    name += '<';
    for (const ClassName& cn : m_targs) {
      if (name[name.size()-1] != '<')
        name += ',';
      name += cn.fullName();
    }
    if (name[name.size()-1] == '>')
      name += ' ';
    name += '>';
  }
  return name;
}


/**
 * @brief Test two expressions for equality.
 */
bool ClassName::operator== (const ClassName& other) const
{
  if (m_name != other.m_name)
    return false;

  if (m_const != other.m_const)
    return false;

  if (m_namespace.size() != other.m_namespace.size())
    return false;

  if (m_targs.size() != other.m_targs.size())
    return false;

  if (m_namespace.size() > 0 && m_namespace[0] != other.m_namespace[0])
    return false;

  for (size_t i = 0; i < m_targs.size(); ++i) {
    if (m_targs[i] != other.m_targs[i])
      return false;
  }

  return true;
}


/**
 * @brief Test two expressions for inequality.
 */
bool ClassName::operator!= (const ClassName& other) const
{
  return !(*this==other);
}


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
bool ClassName::match (const ClassName& pattern, match_t& matches) const
{
  matches.clear();
  return match1 (pattern, true, matches);
}


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
void ClassName::subst (const match_t& matches)
{
  if (m_name[0] == '$') {
    std::string var = m_name.substr (1, std::string::npos);
    match_t::const_iterator it = matches.find (var);
    if (it != matches.end()) {
      bool const_save = m_const;
      *this = it->second;
      m_const |= const_save;
    }
    else {
      throw ExcMissingVariable (var);
    }
  }

  for (ClassName& c : m_namespace)
    c.subst (matches);
  for (ClassName& c : m_targs)
    c.subst (matches);
}


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
ClassName ClassName::substCopy (const match_t& matches) const
{
  ClassName cn (*this);
  cn.subst (matches);
  return cn;
}


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
void ClassName::applyRules (const Rules& rules)
{
  while (applyRules1 (rules))
    ;
}


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
std::string ClassName::applyRules (const std::string& name,
                                   const Rules& rules)
{
  ClassName cn (name);
  cn.applyRules (rules);
  return cn.fullName();
}


/**
 * @brief Parse a string into a @c ClassName.
 * @param name The string containing the name.
 * @param pos Position in the string to start parsing.
 *
 * On return, @c pos will be updated to point just past the last
 * character consumed.
 */
void ClassName::parse (const std::string& name, std::string::size_type& pos)
{
  m_name = parsePrimary (name, pos);
  if (m_name.substr (0, 6) == "const ") {
    m_const = true;
    m_name.erase (0, 6);
  }
  if (m_name.size() >= 6 && m_name.substr (m_name.size()-6, 6) == " const") {
    m_const = true;
    m_name.erase (m_name.size()-6, 6);
  }

  skipSpaces (name, pos);
  while (pos < name.size()) {
    if (name[pos] == '<')
      parseTemplateArgs (name, pos);
    else if (name[pos] == ':' && pos+1 < name.size() && name[pos+1] == ':')
      parseNamespace (name, pos);
    else
      break;
  }
  skipSpaces (name, pos);
  if (name.substr (pos, 5) == "const") {
    m_const = true;
    pos += 5;
  }
}


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
std::string
ClassName::parsePrimary (const std::string& name, std::string::size_type& pos)
{
  skipSpaces (name, pos);
  std::string out;
  size_t nest = 0;
  while (pos < name.size()) {
    char c = name[pos];
    if (c == '(')
      ++nest;
    else if (c == ')' && nest > 0)
      --nest;
    else if (nest == 0 && (c == '<' || c == '>' || c == ',' || c == ':'))
      break;

    out += c;
    ++pos;
  }
  return out;
}


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
void ClassName::parseNamespace (const std::string& name,
                                std::string::size_type& pos)
{
  assert (pos+1 < name.size() && name[pos] == ':' && name[pos+1] == ':');
  pos += 2;
  skipSpaces (name, pos);

  ClassName ns (name, pos);
  ns.swap (*this);
  if (ns.isConst()) {
    this->setConst();
    ns.m_const = false;
  }
  ClassName* p = this;
  while (p->m_namespace.size() > 0)
    p = &p->m_namespace[0];
#if __cplusplus > 201100
  p->m_namespace.push_back (std::move(ns));
#else
  p->m_namespace.resize(1);
  p->m_namespace[0].swap (ns);
#endif
}


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
void ClassName::parseTemplateArgs (const std::string& name,
                                   std::string::size_type& pos)
{
  assert (pos < name.size() && name[pos] == '<');
  ++pos;
  while (true) {
    skipSpaces (name, pos);
#if __cplusplus > 201100
    m_targs.emplace_back (name, pos);
#else
    m_targs.push_back (ClassName (name, pos));
#endif
    skipSpaces (name, pos);
    if (pos == name.size()) break;
    if (name[pos] == '>') {
      ++pos;
      break;
    }
    else if (name[pos] == ',')
      ++pos;
    else
      break;
  }
}


/**
 * @brief Skip past spaces in a string.
 * @param name The string containing the name.
 * @param pos Position in the string to start skipping.
 *
 * On return, @c pos will be updated to point just past the last
 * character consumed.
 */
void ClassName::skipSpaces (const std::string& name,
                            std::string::size_type& pos)
{
  while (pos < name.size() && name[pos] == ' ')
    ++pos;
}


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
bool ClassName::match1 (const ClassName& pattern,
                        bool topLevel,
                        match_t& matches) const
{
  if (pattern.m_name[0] == '$') {
    std::string var = pattern.m_name.substr (1, std::string::npos);
    match_t::iterator it = matches.find (var);
    if (it != matches.end()) {
      if (pattern.m_const && !it->second.m_const) {
        ClassName cn (it->second);
        cn.setConst();
        return *this == cn;
      }
      return *this == it->second;
    }

    matches[var] = *this;
    if (pattern.m_const) {
      if (m_const)
        matches[var].m_const = false;
      else
        return false;
    }
    return true;
  }

  // Require that const qualifiers match.
  // However, if this is the top level, we allow a pattern with no explicit
  // const to match something that is const.  Otherwise, we'd need to repeat
  // all patterns for the const case.
  if (topLevel) {
    if (pattern.m_const && !m_const)
      return false;
  }
  else if (m_const != pattern.m_const) {
    return false;
  }

  if (m_name != pattern.m_name)
    return false;

  if (m_namespace.size() != pattern.m_namespace.size())
    return false;

  if (m_targs.size() != pattern.m_targs.size())
    return false;

  if (m_namespace.size() > 0) {
    if (!m_namespace[0].match1 (pattern.m_namespace[0], false, matches))
      return false;
  }

  for (size_t i = 0; i < m_targs.size(); i++) {
    if (!m_targs[i].match1 (pattern.m_targs[i], false, matches))
      return false;
  }

  return true;
}


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
bool ClassName::applyRules1 (const Rules& rules)
{
  bool ret = rules.applyTo (*this);

  if (m_namespace.size() > 0)
    ret |= m_namespace[0].applyRules1 (rules);

  for (size_t i = 0; i < m_targs.size(); i++)
    ret |= m_targs[i].applyRules1 (rules);

  return ret;
}


} // namespace CxxUtils

