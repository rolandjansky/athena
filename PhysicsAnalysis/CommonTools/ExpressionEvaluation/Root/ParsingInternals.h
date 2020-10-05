/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ParsingInternals.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef PARSING_INTERNALS_H
#define PARSING_INTERNALS_H

// Define this to enable debugging
//#define BOOST_SPIRIT_QI_DEBUG

// on_error template signature changes when using this
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

#include <iostream>
#include <string>
#include <stdexcept>

#include "ExpressionEvaluation/StackElement.h"
#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/IUnitInterpreter.h"

namespace ExpressionParsing
{ 
  namespace ast
  {
    struct nil {};
    struct unaryexpr_;
    struct expression;

    typedef boost::variant<
      nil
      , double
      , unsigned int
      , bool
      , std::string
      , boost::recursive_wrapper<unaryexpr_>
      , boost::recursive_wrapper<expression>
      >
      operand;

    struct unaryexpr_
    {
      std::string operator_;
      operand operand_;
    };

    struct operation
    {
      std::string operator_;
      operand operand_;
    };

    struct expression
    {
      operand first;
      std::list<operation> rest;
    };

    inline std::ostream& operator<<(std::ostream& out, nil) { out << "nil"; return out; }
  }
}

BOOST_FUSION_ADAPT_STRUCT(
    ExpressionParsing::ast::unaryexpr_,
    (std::string, operator_)
    (ExpressionParsing::ast::operand, operand_)
    )

BOOST_FUSION_ADAPT_STRUCT(
    ExpressionParsing::ast::operation,
    (std::string, operator_)
    (ExpressionParsing::ast::operand, operand_)
    )

BOOST_FUSION_ADAPT_STRUCT(
    ExpressionParsing::ast::expression,
    (ExpressionParsing::ast::operand, first)
    (std::list<ExpressionParsing::ast::operation>, rest)
    )

namespace ExpressionParsing
{
  enum byte_code
  {
    // Unary functions
    op_neg,
    op_not,
    op_sum,
    op_count,
    op_abs,
    op_sqrt,
    op_cbrt,
    op_sin,
    op_cos,
    op_tan,
    op_asin,
    op_acos,
    op_atan,
    op_sinh,
    op_cosh,
    op_tanh,
    op_asinh,
    op_acosh,
    op_atanh,
    op_log,
    op_exp,

    // Binary boolean operators
    op_and,
    op_or,

    // Binary comparison operators
    op_eq,
    op_neq,
    op_gt,
    op_gte,
    op_lt,
    op_lte,

    // Binary math operations
    op_add,
    op_sub,
    op_pow,
    op_mul,
    op_div,
    
    //  push value into the stack
    op_val
  };

  class VirtualMachine
  {
    public:
      VirtualMachine(unsigned stackSize = 4096)
        : m_stackSize(stackSize)
      { }

      StackElement execute(std::vector<StackElement> const& code) const;
  private:
      unsigned m_stackSize;
  };


  class Compiler
  {
    public:
      typedef void result_type;

      std::vector<StackElement>& code;
      Compiler(std::vector<StackElement>& code, IProxyLoader *proxyLoader,
          IUnitInterpreter *unitInterpreter)
        : code(code), m_proxyLoader(proxyLoader), m_unitInterpreter(unitInterpreter) { }

      void operator()(ast::nil) const { BOOST_ASSERT(0); }
      void operator()(unsigned int n) const;
      void operator()(bool n) const;
      void operator()(double n) const;
      void operator()(const std::string &n) const;

      void operator()(ast::operation const& x) const;
      void operator()(ast::unaryexpr_ const& x) const;
      void operator()(ast::expression const& x) const;

    private:
      IProxyLoader *m_proxyLoader;
      IUnitInterpreter *m_unitInterpreter;
  };

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  struct error_handler_
  {
    template <typename>
      struct result { typedef void type; };

    template <typename Iterator>
      void operator()(
          qi::info const& what
          , Iterator err_pos, Iterator last) const
      {
        std::cout
          << "Error! Expecting " << what
          << " here: \"" << std::string(err_pos, last) << "\""
          << std::endl;
      }
  };

  boost::phoenix::function<error_handler_> const error_handler = error_handler_();

  template <typename Iterator>
    struct Grammar : qi::grammar<Iterator, ast::expression(), ascii::space_type>
  {
    Grammar() : Grammar::base_type(expression)
    {
      using qi::char_;
      using qi::uint_;
      using qi::double_;
      using qi::bool_;
      using qi::_2;
      using qi::_3;
      using qi::_4;
      using qi::_val;
      using qi::lexeme;
      using qi::alpha;
      using qi::alnum;

      using qi::on_error;
      using qi::fail;

     


      expression = logical_expr.alias();

      logical_expr = equality_expr >> *(
                (qi::string("&&") > equality_expr)
            |   (qi::string("||") > equality_expr)
          );

      equality_expr = relational_expr >> *(
                (qi::string("==") > relational_expr)
            |   (qi::string("!=") > relational_expr)
          );

      relational_expr = additive_expr >> *(
                (qi::string(">=") > additive_expr)
            |   (qi::string("<=") > additive_expr)
            |   (qi::string(">") > additive_expr)
            |   (qi::string("<") > additive_expr)
          );

      additive_expr = multiplicative_expr >> *(
                (char_('+') > multiplicative_expr)
            |   (char_('-') > multiplicative_expr)
          );

      multiplicative_expr = power_expr >> *(
                (char_('*') > power_expr)
            |   (char_('/') > power_expr)
          );

      power_expr = unary_expr >> *(
                (qi::string("**") > unary_expr)
          );

      unary_expr = unaryfunc_expr 
            |   (qi::string("-") > unaryfunc_expr)
            |   (qi::string("+") > unaryfunc_expr)
            |   (qi::string("!") > unaryfunc_expr)
          ;

      //moved primary_expr to end so that function matches happen first
      unaryfunc_expr = 
	   ((qi::string("sum") >> &char_('(')) > primary_expr)
	|   ((qi::string("count") >> &char_('(')) > primary_expr)
	|   ((qi::string("abs") >> &char_('(')) > primary_expr)
	|   ((qi::string("sqrt") >> &char_('(')) > primary_expr)
	|   ((qi::string("cbrt") >> &char_('(')) > primary_expr)
	|   ((qi::string("asinh") >> &char_('(')) > primary_expr)
	|   ((qi::string("acosh") >> &char_('(')) > primary_expr)
	|   ((qi::string("atanh") >> &char_('(')) > primary_expr)
	|   ((qi::string("asin") >> &char_('(')) > primary_expr)
	|   ((qi::string("acos") >> &char_('(')) > primary_expr)
	|   ((qi::string("atan") >> &char_('(')) > primary_expr)
	|   ((qi::string("sinh") >> &char_('(')) > primary_expr)
	|   ((qi::string("cosh") >> &char_('(')) > primary_expr)
	|   ((qi::string("tanh") >> &char_('(')) > primary_expr)
	|   ((qi::string("sin") >> &char_('(')) > primary_expr)
	|   ((qi::string("cos") >> &char_('(')) > primary_expr)
	|   ((qi::string("tan") >> &char_('(')) > primary_expr)
	|   ((qi::string("log") >> &char_('(')) > primary_expr)
	|   ((qi::string("exp") >> &char_('(')) > primary_expr)
        | primary_expr ;


      primary_expr =
        ('(' > expression > ')')
        |   (uint_ >> !char_('.')) | double_ | bool_ | identifier 
        ;

      identifier = lexeme[((alpha | char_('_') | char_('$') | char_('[') | char_(']') | char_('"') | char_('\''))
          >> *(alnum | char_('.') |  char_('_') | char_('$') | char_('[') | char_(']') | char_('"') | char_('\'')))];

      // Debugging and error handling and reporting support.
      BOOST_SPIRIT_DEBUG_NODES(
          (expression)(equality_expr)(relational_expr)
          (logical_expr)(additive_expr)(multiplicative_expr)
          (unary_expr)(unaryfunc_expr)(primary_expr)
          );

      // Error handling
      on_error<fail>(expression, error_handler(_4, _3, _2));
    }


    qi::rule<Iterator, ast::expression(), ascii::space_type>
            expression, equality_expr, relational_expr,
            logical_expr, additive_expr, multiplicative_expr, power_expr;

    qi::rule<Iterator, ast::operand(), ascii::space_type>
            unary_expr, unaryfunc_expr, primary_expr;

    qi::rule<Iterator, std::string(), ascii::space_type> identifier;

    qi::symbols<char> keywords;
  };
}


#endif // PARSING_INTERNALS_H
