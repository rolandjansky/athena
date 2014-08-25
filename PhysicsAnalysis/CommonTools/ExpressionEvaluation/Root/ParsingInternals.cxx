/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ParsingInternals.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ParsingInternals.h"

#define VM_CASE_UNARY(OP) case op_ ## OP: \
          stack_ptr[-1] = stack_ptr[-1]._ ## OP(); \
          break

#define VM_CASE_BINARY(OP) case op_ ## OP: \
          --stack_ptr; \
          stack_ptr[-1] = stack_ptr[-1]._ ## OP(stack_ptr[0]); \
          break

#define VISITOR_UNARY(OP) else if (x.operator_ == #OP) code.push_back(op_ ## OP)

namespace ExpressionParsing {

  void VirtualMachine::execute(std::vector<StackElement> const& code)
  {
    std::vector<StackElement>::const_iterator pc = code.begin();
    stack_ptr = stack.begin();

    while (pc != code.end())
    {
      ++pc;
      switch (pc[-1].asInt())
      {
        case op_neg:
          stack_ptr[-1] = -stack_ptr[-1];
          break;

        case op_not:
          stack_ptr[-1] = !stack_ptr[-1];
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

        VM_CASE_BINARY(and);
        VM_CASE_BINARY(or);
        VM_CASE_BINARY(eq);
        VM_CASE_BINARY(neq);
        VM_CASE_BINARY(gt);
        VM_CASE_BINARY(gte);
        VM_CASE_BINARY(lt);
        VM_CASE_BINARY(lte);

        case op_add:
          --stack_ptr;
          stack_ptr[-1] += stack_ptr[0];
          break;

        case op_sub:
          --stack_ptr;
          stack_ptr[-1] -= stack_ptr[0];
          break;

        VM_CASE_BINARY(pow);

        case op_mul:
          --stack_ptr;
          stack_ptr[-1] *= stack_ptr[0];
          break;

        case op_div:
          --stack_ptr;
          stack_ptr[-1] /= stack_ptr[0];
          break;

        case op_val:
          *stack_ptr++ = *pc++;
          if (stack_ptr[-1].isProxy()) stack_ptr[-1].setValueFromProxy();
          break;
      }
    }

    if ((stack_ptr - stack.begin()) != 1){
      throw std::runtime_error("ExpressionEvaluation: Virtual machine finished in undefined state. Is expression valid?");
    }
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
    BOOST_FOREACH(ast::operation const& oper, x.rest)
    {
      (*this)(oper);
    }
  }

}
