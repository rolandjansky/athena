/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ExpressionParser.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ParsingInternals.h"
#include "ProxyLoaderSingleton.h"
#include "UnitInterpreterSingleton.h"

#include <boost/algorithm/string.hpp>

namespace ExpressionParsing {

  ExpressionParser::ExpressionParser()
  {
    m_proxyLoader = ProxyLoaderSingleton::getInstance();
    m_unitInterpreter = UnitInterpreterSingleton::getInstance();
    setup();
  }

  ExpressionParser::ExpressionParser(IProxyLoader *proxyLoader)
  {
    m_proxyLoader = proxyLoader;
    m_unitInterpreter = UnitInterpreterSingleton::getInstance();
    setup();
  }

  ExpressionParser::ExpressionParser(IUnitInterpreter *unitInterpreter)
  {
    m_proxyLoader = ProxyLoaderSingleton::getInstance();
    m_unitInterpreter = unitInterpreter;
    setup();
  }

  ExpressionParser::ExpressionParser(IProxyLoader *proxyLoader, IUnitInterpreter *unitInterpreter)
  {
    m_proxyLoader = proxyLoader;
    m_unitInterpreter = unitInterpreter;
    setup();
  }

  ExpressionParser::~ExpressionParser()
  {
    if (m_vm) {
      delete m_vm;
    }
  }

  void ExpressionParser::setup()
  {
    m_vm = new VirtualMachine();
  }

  bool ExpressionParser::loadExpression(const std::string &expression)
  {
    std::string trimmedExpression = boost::algorithm::trim_copy_if(expression, boost::is_any_of("\""));

    ProxyLoaderSingleton::getInstance()->reset();
    m_code.clear();

    Grammar<std::string::const_iterator> grammar;
    ast::expression expr;

    std::string::const_iterator expressionIter = trimmedExpression.begin();
    std::string::const_iterator expressionEnd = trimmedExpression.end();
    using boost::spirit::ascii::space_type;
    bool r = phrase_parse(expressionIter, expressionEnd, grammar, space_type(), expr);
    std::string remainingExpression(expressionIter, expressionEnd);
    boost::trim(remainingExpression);  
    if (remainingExpression.length() > 0) {
      throw std::runtime_error("Did you forget an operator? This was not parsed: '" + remainingExpression +"'");
    }
    
    if (!r) return false;

    Compiler compiler(m_code, m_proxyLoader, m_unitInterpreter);
    compiler(expr);
    return true;
  }

  StackElement ExpressionParser::evaluate() const
  {
    return m_vm->execute(m_code);
  }

  bool ExpressionParser::evaluateAsBool() const
  {
    StackElement result = evaluate();
    if (result.isScalar()) return result.asBool();
    else throw std::runtime_error("Unable to evaluate vector quantity as a boolean");
  }

  double ExpressionParser::evaluateAsDouble() const
  {
    StackElement result = evaluate();
    if (result.isScalar()) return result.scalarValue<double>();
    else throw std::runtime_error("Unable to evaluate vector quantity as a double");
  }

  std::vector<int> ExpressionParser::evaluateAsVector() const
  {
    StackElement result = evaluate();
    if (result.isVector()) return result.vectorValue<int>();
    else throw std::runtime_error("Unable to evaluate scalar quantity as a vector"); 
  }
}
