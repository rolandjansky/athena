/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H
#define DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include <string>

// namespace Muon{
// class IMuonTrackExtrapolationTool;
// }

namespace ExpressionParsing{
class ExpressionParser;
}

// namespace Trk{
//   class IExtrapolator;
// }

namespace DerivationFramework {
  /** @class isolationDecorator
      @author Dongliang.Zhang@cern.ch
     */

  class mcpDecorator: public ::AthAlgTool, public IAugmentationTool {
   public: 
    /** Constructor with parameters */
    mcpDecorator( const std::string& t, const std::string& n, const IInterface* p);

    /** Destructor */
    virtual ~mcpDecorator(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();
    virtual StatusCode addBranches() const;

   private: 
    std::string m_containerName;
    std::string m_selectionString;
    std::string m_prefix;
//     ToolHandle< Muon::IMuonTrackExtrapolationTool > m_trackExtrapoTool;
//     ToolHandle< Trk::IExtrapolator > m_extrapolator;

    ExpressionParsing::ExpressionParser* m_parser;
  }; 
}

#endif //> !DERIVATIONFRAMEWORKMUONS_MCPDECORATOR_H
