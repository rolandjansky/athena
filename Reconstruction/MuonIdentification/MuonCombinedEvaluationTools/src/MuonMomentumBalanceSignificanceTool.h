/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonMomentumBalanceSignificanceTool.h, (c) ATLAS Combined Muon software
///////////////////////////////////////////////////////////////////

#ifndef MUONMOMENTUMBALANCESIGNIFICANCETOOL_H
#define MUONMOMENTUMBALANCESIGNIFICANCETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// #include "GaudiKernel/IIncidentListener.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"

// class IIncidentSvc;
/*
namespace Trk {
  class ITrackingVolumesSvc;
  class Volume;
}
*/

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
    ~MuonMomentumBalanceSignificanceTool (void); // destructor
  
    StatusCode		initialize();
    StatusCode		finalize();

    /** Calculate momentum (im)balance significance of a muon (method will simply step down to the relevant track */
    double     momentumBalanceSignificance (const xAOD::Muon& muon) const;

    /** Calculate momentum (im)balance significance of a track, which needs to be a full ATLAS track */
    double     momentumBalanceSignificance (const Trk::Track& track) const;

  private:

    // tools and services
    //ServiceHandle<Trk::ITrackingVolumesSvc>  m_trackingVolumesSvc; //!< geometry for analysing track lengths

    // constants 
    //const Trk::Volume*                  m_calorimeterVolume; //!< cache the calo volume pointer
    //const Trk::Volume*                  m_indetVolume;       //!< cache the ID volume pointer

  };
 
}	// end of namespace

#endif


