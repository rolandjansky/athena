/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/KinObjectSelect.h>

#include <QuickAna/MessageCheck.h>
#include <stdexcept>

//
// method implementations
//

namespace ana
{
  namespace
  {
    /// \brief the different types of logic operator a term can result
    /// from
    enum class LogicOp
    {
      /// \brief not a logic operator
      NONE,

      /// \brief and(&&) operator
      AND,

      /// \brief or(||) operator
      OR
    };

    /// \brief the summary description of a term in the formula
    struct Term
    {
      /// \brief if this is the a boolean term, this is it
    public:
      std::function<bool(const xAOD::IParticle& p)> bool_func;

      /// \brief if this is the a numeric term, this is it
    public:
      std::function<double(const xAOD::IParticle& p)> float_func;

      /// \brief if this term is a logic operation, this is it
      ///
      /// This is used to make sure that I do not mix and(&&) and
      /// or(or) without the use of parenthesis, as it can be
      /// confusing to the user.
    public:
      LogicOp logicOp = LogicOp::NONE;

    public:
      operator bool () const {return bool_func || float_func;}
    };

    /// \brief standard output operator for \ref Term
    std::ostream& operator << (std::ostream& str, Term obj)
    {
      if (obj.bool_func)
	str << "bool";
      else if (obj.float_func)
	str << "float";
      else
	str << "null";
      switch (obj.logicOp)
      {
      case LogicOp::NONE:
	break;
      case LogicOp::AND:
	str << "(and)";
	break;
      case LogicOp::OR:
	str << "(or)";
	break;
      }
      return str;
    }



    /// \todo needs documentation
    enum class Precedence
    {
      CORE,
      COMPARE,
      AND_OR,
      ALL
    };

    /// \brief standard output operator for \ref Precedence
    std::ostream& operator << (std::ostream& str, Precedence obj)
    {
      switch (obj)
      {
      case Precedence::CORE:
	str << "core";
	break;
      case Precedence::COMPARE:
	str << "compare";
	break;
      case Precedence::AND_OR:
	str << "and_or";
	break;
      case Precedence::ALL:
	str << "all";
	break;
      }
      return str;
    }



    struct RecurseLevel final
    {
      RecurseLevel () {++ m_level;}
      ~RecurseLevel () {-- m_level;}

      RecurseLevel (const RecurseLevel&) = delete;
      RecurseLevel& operator = (const RecurseLevel&) = delete;

      unsigned level () const {
	return m_level;}

    private:
      static unsigned m_level;
    };
    unsigned RecurseLevel::m_level = 0;

    std::ostream& operator << (std::ostream& str, const RecurseLevel& obj)
    {
      for (unsigned iter = 0, end = obj.level(); iter != end; ++ iter)
	str << " ";
      return str;
    }



    /// \brief parse the next/remaining terms in the formula
    ///
    /// The formula is parsed via recursion, on token at a time,
    /// i.e. after one token is parsed this function gets called again
    /// to parse the remaining tokens.  There isn't any read-ahead,
    /// instead how the next token gets handled is dependent on the
    /// current parser state (as determined by the arguments).
    ///
    /// The way binary expressions (e.g. "a>b") are handled is that
    /// first the token "a" will be parsed, then the parser gets
    /// called recursively with "a" as the already parsed term
    /// (preterm).  It will then encounter the token ">" realize that
    /// it is a binary expression and recurse once more to parse "b".
    /// For that second recursion it will not set a preterm and set
    /// the precedence to indicate that only stronger expressions than
    /// ">" may be evaluated (e.g. it would end when encountering
    /// "&&").  Once that second recursion returns that term("b") is
    /// combined with the preterm("a") to form a logic term("a>b").
    /// This is then passed as a preterm into the next level of the
    /// recursion.
    ///
    /// \param[in] preterm the last term evaluated.  this is to form
    /// binary expressions as explained above
    ///
    /// \param[in,out] formula the remaining formula to evaluate.  as
    /// we parse tokens they get removed from the beginning of the
    /// formula
    ///
    /// \param[in] precedence the maximum precedence to which to
    /// evaluate the next term. this is needed to handle nested binary
    /// expressions like "a>b&&c>d" correctly, as explained above

    Term parse (const Term& preterm, std::string& formula,
		Precedence precedence)
    {
      RecurseLevel recurse;

      using namespace msgKinSelect;
      ANA_MSG_DEBUG (recurse << "begin,preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);

      while (!formula.empty() && isspace (formula[0]))
	formula = formula.substr (1);
      if (formula.empty())
      {
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return preterm;
      }
      Term result;

      if (formula[0] == '(')
      {
	formula = formula.substr (1);
	ANA_MSG_DEBUG (recurse << "middle" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	result = parse (Term (), formula, Precedence::ALL);
	if (formula.empty() || formula[0] != ')')
	  throw std::runtime_error ("expected to find token ')'");
	formula = formula.substr (1);
	result.logicOp = LogicOp::NONE;
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }
      {
	std::istringstream str (formula);
	double number = 0;
	if (str >> number)
	{
	  if (preterm)
	    throw std::runtime_error ("invalid token");
	  result.float_func = [number] (const xAOD::IParticle& /*p*/) {return number;};
	  formula = formula.substr (formula.size() - str.rdbuf()->in_avail());
	  ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	  return parse (result, formula, precedence);
	}
      }
      if (formula.find ("pt") == 0)
      {
	if (preterm)
	  throw std::runtime_error ("invalid token");
	result.float_func = [] (const xAOD::IParticle& p) {return p.pt();};
	formula = formula.substr (2);
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }
      if (formula.find ("eta") == 0)
      {
	if (preterm)
	  throw std::runtime_error ("invalid token");
	result.float_func = [] (const xAOD::IParticle& p) {return p.eta();};
	formula = formula.substr (3);
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }
      if (formula.find ("phi") == 0)
      {
	if (preterm)
	  throw std::runtime_error ("invalid token");
	result.float_func = [] (const xAOD::IParticle& p) {return p.phi();};
	formula = formula.substr (3);
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }

      if (precedence <= Precedence::COMPARE)
      {
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return preterm;
      }
      if (formula.find (">") == 0)
      {
	if (!preterm.float_func)
	  throw std::runtime_error ("need float argument to comparison");
	formula = formula.substr (1);

	ANA_MSG_DEBUG (recurse << "middle" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	Term postterm = parse (Term (), formula, Precedence::COMPARE);
	if (!postterm.float_func)
	  throw std::runtime_error ("need float argument to comparison");

	auto left = preterm.float_func;
	auto right = postterm.float_func;
	result.bool_func = [left, right] (const xAOD::IParticle& p) {return left (p) > right (p);};
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }
      if (formula.find ("<") == 0)
      {
	if (!preterm.float_func)
	  throw std::runtime_error ("need float argument to comparison");
	formula = formula.substr (1);

	ANA_MSG_DEBUG (recurse << "middle" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	Term postterm = parse (Term (), formula, Precedence::COMPARE);
	if (!postterm.float_func)
	  throw std::runtime_error ("need float argument to comparison");

	auto left = preterm.float_func;
	auto right = postterm.float_func;
	result.bool_func = [left, right] (const xAOD::IParticle& p) {return left (p) < right (p);};
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }

      if (precedence <= Precedence::AND_OR)
      {
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return preterm;
      }
      if (formula.find ("&&") == 0)
      {
	if (!preterm.bool_func)
	  throw std::runtime_error ("need bool argument to logic operator");
	if (preterm.logicOp != LogicOp::NONE &&
	    preterm.logicOp != LogicOp::AND)
	  throw std::runtime_error ("can't mix different logic operators without paranthesis");
	formula = formula.substr (2);

	ANA_MSG_DEBUG (recurse << "middle" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	Term postterm = parse (Term (), formula, Precedence::AND_OR);
	if (!postterm.bool_func)
	  throw std::runtime_error ("need bool argument to comparison");

	auto left = preterm.bool_func;
	auto right = postterm.bool_func;
	result.bool_func = [left, right] (const xAOD::IParticle& p) {return left (p) && right (p);};
	result.logicOp = LogicOp::AND;
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }
      if (formula.find ("||") == 0)
      {
	if (!preterm.bool_func)
	  throw std::runtime_error ("need bool argument to logic operator");
	if (preterm.logicOp != LogicOp::NONE &&
	    preterm.logicOp != LogicOp::OR)
	  throw std::runtime_error ("can't mix different logic operators without paranthesis");
	formula = formula.substr (2);

	ANA_MSG_DEBUG (recurse << "middle" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	Term postterm = parse (Term (), formula, Precedence::AND_OR);
	if (!postterm.bool_func)
	  throw std::runtime_error ("need bool argument to comparison");

	auto left = preterm.bool_func;
	auto right = postterm.bool_func;
	result.bool_func = [left, right] (const xAOD::IParticle& p) {return left (p) || right (p);};
	result.logicOp = LogicOp::OR;
	ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
	return parse (result, formula, precedence);
      }

      ANA_MSG_DEBUG (recurse << "end" << __LINE__ << ",preterm=" << preterm << ",formula=" << formula << ",precendence=" << precedence);
      return preterm;
    }
  }



  KinObjectSelect ::
  KinObjectSelect (const std::string& formula)
  {
    std::string myformula = formula;
    try
    {
      Term term = parse (Term (), myformula, Precedence::ALL);
      if (!myformula.empty())
	throw std::runtime_error ("extra text at end of formula");
      if (!term.bool_func)
	throw std::runtime_error ("didn't find boolean expression");
      m_function = term.bool_func;
    } catch (std::exception& e)
    {
      throw std::runtime_error ("failed parsing \"" +
				formula.substr (0, formula.size() - myformula.size()) +
				"\" \"" + myformula + "\": " + e.what());
    }
  }



  bool KinObjectSelect ::
  select (const xAOD::IParticle& particle) const
  {
    return m_function (particle);
  }
}
