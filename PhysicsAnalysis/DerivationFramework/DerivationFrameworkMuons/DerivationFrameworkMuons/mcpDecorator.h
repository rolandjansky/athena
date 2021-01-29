/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H
#define DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include <string>


namespace ExpressionParsing{
    class ExpressionParser;
}


namespace DerivationFramework {
  /** @class isolationDecorator
      @author Dongliang.Zhang@cern.ch
     */

  class mcpDecorator: public ::AthAlgTool, public IAugmentationTool {
   public: 
    /** Constructor with parameters */
    mcpDecorator( const std::string& t, const std::string& n, const IInterface* p);

    /** Destructor */
    virtual ~mcpDecorator()= default; 

    
    virtual StatusCode  initialize() override;   
    virtual StatusCode addBranches() const override;

   private: 
    std::string m_containerName;
    std::string m_selectionString;
    std::string m_prefix;
    
    std::unique_ptr<ExpressionParsing::ExpressionParser> m_parser;
  }; 
}

#endif //> !DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H
