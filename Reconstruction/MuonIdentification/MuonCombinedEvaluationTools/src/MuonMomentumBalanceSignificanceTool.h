/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMOMENTUMBALANCESIGNIFICANCETOOL_H
#define MUONMOMENTUMBALANCESIGNIFICANCETOOL_H

#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Rec
{

  /** @class MuonMomentumBalanceSignificanceTool
      @brief Tool to calculate the momentum balance significance from the detailed
             information (parameters, scattering angles) of a track fit.
    */
  
  class MuonMomentumBalanceSignificanceTool : public AthAlgTool,
			   virtual public IMuonMomentumBalanceSignificance
    {
  public:
    MuonMomentumBalanceSignificanceTool (const std::string& type, 
                                         const std::string& name,
                                         const IInterface* parent);
    ~MuonMomentumBalanceSignificanceTool()=default;

    /** Calculate momentum (im)balance significance of a muon (method will simply step down to the relevant track */
    double     momentumBalanceSignificance (const xAOD::Muon& muon) const;

    /** Calculate momentum (im)balance significance of a track, which needs to be a full ATLAS track */
    double     momentumBalanceSignificance (const Trk::Track& track) const;

  private:

  };
 
}	// end of namespace

#endif


