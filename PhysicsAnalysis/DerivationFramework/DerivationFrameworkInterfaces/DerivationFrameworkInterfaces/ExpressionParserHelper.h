/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINTERFACES_EXPRESSIONPARSERHELPER_H
#define DERIVATIONFRAMEWORKINTERFACES_EXPRESSIONPARSERHELPER_H

// Framework include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Expression parsing include(s):
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/ExpressionParser.h"

namespace DerivationFramework {

   /// Helper class for using @c ExpressionParsing::ExpressionParser
   ///
   /// Since the correct memory management for
   /// @c ExpressionParsing::ExpressionParser is not so trivial, instead of
   /// implementing that individually in every DF class that uses it, would
   /// not be a too good design. Instead each DF class should migrate to using
   /// this helper class to avoid memory leaks coming from the expression
   /// parsing code.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class ExpressionParserHelper {

   public:
      /// Default constructor
      ExpressionParserHelper( ServiceHandle< StoreGateSvc >& evtStore )
         : m_xaodLoader( evtStore ), m_ntupLoader( evtStore ),
       m_proxyLoader({&m_xaodLoader, &m_ntupLoader}), m_parser( &m_proxyLoader ) {
      }

      /// Return a reference to the expression parser
      ExpressionParsing::ExpressionParser& parser() {
         return m_parser;
      }

   private:
      /// The xAOD proxy loader
      ExpressionParsing::SGxAODProxyLoader m_xaodLoader;
      /// The ntuple variable proxy loader
      ExpressionParsing::SGNTUPProxyLoader m_ntupLoader;
      /// The multiple proxy loader
      ExpressionParsing::MultipleProxyLoader m_proxyLoader;
      /// The main expression parser object
      ExpressionParsing::ExpressionParser m_parser;

   }; // class ExpressionParserHelper

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINTERFACES_EXPRESSIONPARSERHELPER_H
