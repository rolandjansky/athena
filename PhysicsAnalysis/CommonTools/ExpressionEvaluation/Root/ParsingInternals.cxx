/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ParsingInternals.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ParsingInternals.h"

#define VM_CASE_UNARY(OP) case op_ ## OP: \
          stack.back() = stack.back()._ ## OP();    \
          break



#define VISITOR_UNARY(OP) else if (x.operator_ == #OP) code.push_back(op_ ## OP)

std::atomic<std::size_t> g_maxStackSize=0;

namespace ExpressionParsing {
   template <class T_func>
   void bin_op( std::vector<StackElement> &stack, T_func a_func) {
      assert( stack.size() >= 2);
      std::vector<ExpressionParsing::StackElement>::iterator last_elm = stack.end()-1;
      std::vector<ExpressionParsing::StackElement>::iterator second_last_elm = stack.end()-2;
      a_func(*second_last_elm, *last_elm);
      stack.pop_back();
   }

  StackElement VirtualMachine::execute(std::vector<StackElement> const& code) const
  {
    std::vector<StackElement> stack;
    stack.reserve(m_stackSize);
    std::size_t max_stack_size=g_maxStackSize;

    std::vector<StackElement>::const_iterator pc = code.begin();

    while (pc != code.end())
    {
       if (stack.size() > max_stack_size) max_stack_size = stack.size();
      ++pc;
      switch (pc[-1].asInt())
      {
        case op_neg: {
          stack.back() = -std::move(stack.back());
          break;
        }
        case op_not:
          stack.back() = !std::move(stack.back());
          break;

        VM_CASE_UNARY(sum);
        VM_CASE_UNARY(count);
        VM_CASE_UNARY(abs);
        VM_CASE_UNARY(sqrt);
        VM_CASE_UNARY(cbrt);
        VM_CASE_UNARY(sin);
        VM_CASE_UNARY(cos);
        VM_CASE_UNARY(tan);
        VM_CASE_UNARY(asin);
        VM_CASE_UNARY(acos);
        VM_CASE_UNARY(atan);
        VM_CASE_UNARY(sinh);
        VM_CASE_UNARY(cosh);
        VM_CASE_UNARY(tanh);
        VM_CASE_UNARY(asinh);
        VM_CASE_UNARY(acosh);
        VM_CASE_UNARY(atanh);
        VM_CASE_UNARY(log);
        VM_CASE_UNARY(exp);

        case op_and: bin_op(stack,[](StackElement &a, StackElement &b) { a=a._and(b); }); break;
        case op_or:  bin_op(stack,[](StackElement &a, StackElement &b) { a=a._or(b); });  break;
        case op_eq:  bin_op(stack,[](StackElement &a, StackElement &b) { a=a._eq(b); });  break;
        case op_neq: bin_op(stack,[](StackElement &a, StackElement &b) { a=a._neq(b); }); break;
        case op_gt:  bin_op(stack,[](StackElement &a, StackElement &b) { a=a._gt(b); });  break;
        case op_gte: bin_op(stack,[](StackElement &a, StackElement &b) { a=a._gte(b); }); break;
        case op_lt:  bin_op(stack,[](StackElement &a, StackElement &b) { a=a._lt(b); });  break;
        case op_lte: bin_op(stack,[](StackElement &a, StackElement &b) { a=a._lte(b); }); break;

        case op_add: bin_op(stack,[](StackElement &a, StackElement &b) { a += b; }); break;
        case op_sub: bin_op(stack,[](StackElement &a, StackElement &b) { a -= b; }); break;

        case op_pow: bin_op(stack,[](StackElement &a, StackElement &b) { a = a._pow(b); }); break;

        case op_mul: bin_op(stack,[](StackElement &a, StackElement &b) { a *= b; }); break;
        case op_div: bin_op(stack,[](StackElement &a, StackElement &b) { a /= b; }); break;

        case op_val:
          if (pc->isProxy()) {
             stack.emplace_back(pc->valueFromProxy());
          }
          else {
             stack.emplace_back(*pc);
          }
          ++pc;
          break;
      }
    }

    if (stack.size()!=1){
      throw std::runtime_error("ExpressionEvaluation: Virtual machine finished in undefined state. Is expression valid?");
    }
    if (max_stack_size>g_maxStackSize) {
       g_maxStackSize=max_stack_size;
    }
    return std::move(stack.back());
  }



  void Compiler::operator()(unsigned int n) const
  {
    code.push_back(op_val);
    code.push_back(n);
  }
  void Compiler::operator()(bool n) const
  {
    code.push_back(op_val);
    code.push_back((int)n);
  }
  void Compiler::operator()(double n) const
  {
    code.push_back(op_val);
    code.push_back(n);
  }
  void Compiler::operator()(const std::string &n) const
  {
    const IUnitInterpreter *units = m_unitInterpreter;

    code.push_back(op_val);
    // Intercept special values
    if (n == "pi")                    code.push_back(3.14159265359);
    else if (n == "e")                code.push_back(2.71828182846);
    else if (n == "kBigNumber")       code.push_back(1234567890);
    else if (n == "kLeinGrossNummer") code.push_back(123456789);
    else if (units->isKnownUnit(n))   code.push_back(units->unitValue(n));
    else                              code.push_back(StackElement(n, m_proxyLoader));
  }

  void Compiler::operator()(ast::operation const& x) const
  {
    boost::apply_visitor(*this, x.operand_);
    if (x.operator_ == "&&") code.push_back(op_and);
    else if (x.operator_ == "||") code.push_back(op_or);
    else if (x.operator_ == "==") code.push_back(op_eq);
    else if (x.operator_ == "!=") code.push_back(op_neq);
    else if (x.operator_ == ">") code.push_back(op_gt);
    else if (x.operator_ == ">=") code.push_back(op_gte);
    else if (x.operator_ == "<") code.push_back(op_lt);
    else if (x.operator_ == "<=") code.push_back(op_lte);
    else if (x.operator_ == "+") code.push_back(op_add);
    else if (x.operator_ == "-") code.push_back(op_sub);
    else if (x.operator_ == "**") code.push_back(op_pow);
    else if (x.operator_ == "*") code.push_back(op_mul);
    else if (x.operator_ == "/") code.push_back(op_div);
    else BOOST_ASSERT(0);
  }

  void Compiler::operator()(ast::unaryexpr_ const& x) const
  {
    boost::apply_visitor(*this, x.operand_);
    if (x.operator_ == "-") code.push_back(op_neg);
    else if (x.operator_ == "!") code.push_back(op_not);
    VISITOR_UNARY(sum);
    VISITOR_UNARY(count);
    VISITOR_UNARY(abs);
    VISITOR_UNARY(sqrt);
    VISITOR_UNARY(cbrt);
    VISITOR_UNARY(sin);
    VISITOR_UNARY(cos);
    VISITOR_UNARY(tan);
    VISITOR_UNARY(asin);
    VISITOR_UNARY(acos);
    VISITOR_UNARY(atan);
    VISITOR_UNARY(sinh);
    VISITOR_UNARY(cosh);
    VISITOR_UNARY(tanh);
    VISITOR_UNARY(asinh);
    VISITOR_UNARY(acosh);
    VISITOR_UNARY(atanh);
    VISITOR_UNARY(log);
    VISITOR_UNARY(exp);
    else if (x.operator_ == "+") ;
    else BOOST_ASSERT(0);
  }

  void Compiler::operator()(ast::expression const& x) const
  {
    boost::apply_visitor(*this, x.first);
    for(ast::operation const& oper : x.rest)
    {
      (*this)(oper);
    }
  }

}
