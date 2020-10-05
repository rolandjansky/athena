/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ExpressionParser.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "ExpressionEvaluation/StackElement.h"
#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/IUnitInterpreter.h"

namespace ExpressionParsing {
  class VirtualMachine;

  class ExpressionParser
  {
    public:
      ExpressionParser();
      ExpressionParser(IProxyLoader *proxyLoader);
      ExpressionParser(IUnitInterpreter *unitInterpreter);
      ExpressionParser(IProxyLoader *proxyLoader, IUnitInterpreter *unitInterpreter);
      ~ExpressionParser();

      ExpressionParser (const ExpressionParser&) = delete;
      ExpressionParser& operator= (const ExpressionParser&) = delete;

      bool loadExpression(const std::string &expression);
      StackElement evaluate() const;
      bool evaluateAsBool() const;
      double evaluateAsDouble() const;
      std::vector<int> evaluateAsVector() const;

    private:
      void setup();

    private:
      IProxyLoader *m_proxyLoader;
      IUnitInterpreter *m_unitInterpreter;
      std::vector<StackElement> m_code;
      VirtualMachine *m_vm;
  };
}

#endif // EXPRESSION_PARSER_H
