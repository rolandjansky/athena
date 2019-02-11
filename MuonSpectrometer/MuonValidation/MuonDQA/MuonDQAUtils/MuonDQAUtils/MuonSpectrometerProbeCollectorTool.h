/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSpectrometerProbeCollectorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef Muon_MuonSpectrometerProbeCollectorTool_H
#define Muon_MuonSpectrometerProbeCollectorTool_H

/// General Classes
#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>
#include <string>
/// Gaudi Tools
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
/// Storegate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
/// ROOT Classes
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
/// Muon Trigger
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigSteeringEvent/TriggerDecision.h"
/// Track Particles
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
/// Electron
#include "egammaEvent/Electron.h"
/// Muon
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
/// common implementation of all particles
#include "ParticleEvent/ParticleBaseContainer.h"
/// particle jets
#include "JetTagEvent/JetConstituent.h"
#include "JetEvent/JetCollection.h"
#include "JetTagEvent/TrackConstituents.h" 

#include "MuonDQAUtils/IProbeCollectorTool.h"
#include "MuonDQAUtils/IInsituTrackTools.h"

class AtlasDetectorID;
class Identifier;


/** @class MuonSpectrometerProbeCollectorTool 

This is for the Doxygen-Documentation.  
Please delete these lines and fill in information about the Algorithm!
Please precede every member function declaration with a
short Doxygen comment stating the purpose of this function.

@author  Matthias Schott <mschott@cern.ch>
@author  Nektarios Chr. Benekos <nbenekos@illinois.edu>
*/  
namespace Muon 
{
  class MuonSpectrometerProbeCollectorTool : virtual public IProbeCollectorTool, public AthAlgTool
    {
    public:
      MuonSpectrometerProbeCollectorTool(const std::string&,const std::string&,const IInterface*);
	
      /** default destructor */
      virtual ~MuonSpectrometerProbeCollectorTool ();
	
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
		
      StatusCode createProbeCollection();
		
    private:
	
      Rec::TrackParticleContainer * m_MSProbeTrackContainer;

      /** class member version of retrieving MsgStream */
      mutable MsgStream	m_log;
      /// a handle on Store Gate 
      StoreGateSvc* m_storeGate;
      /// a handle on the Hist/TTree registration service
      ITHistSvc * m_thistSvc;
      /// get a handle to the MuonSpectrometerProbeCollectorTool
      ToolHandle<IInsituTrackTools> m_InsituPerformanceTools;

      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;
      std::string	m_InnerTrackContainerName;
      std::string	m_MSTrackContainerName;
      std::string	m_CombinedMuonTracksContainerName;
      bool		m_RequireTrigger;

    }; 
}
#endif 
