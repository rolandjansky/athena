/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Muon_InnerDetProbeCollectorTool_H
#define Muon_InnerDetProbeCollectorTool_H

/// General Classes
#include <stdint.h>
#include <algorithm>
#include <functional>
#include <string>
/// Gaudi Tools
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
/// ROOT Classes
#include "TH1.h"
#include "TH2.h"
/// Muon Trigger
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
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

/** @class InnerDetProbeCollectorTool 

This is for the Doxygen-Documentation.  
Please delete these lines and fill in information about the Algorithm!
Please precede every member function declaration with a
short Doxygen comment stating the purpose of this function.

@author  Nektarios Benekos <nbenekos@illinois.edu>
*/  
namespace Muon 
{

  class InnerDetProbeCollectorTool : virtual public IProbeCollectorTool, public AthAlgTool
    {
    public:
      InnerDetProbeCollectorTool(const std::string&,const std::string&,const IInterface*);
	
      /** default destructor */
      virtual ~InnerDetProbeCollectorTool () {};
	
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
		
      StatusCode createProbeCollection();
		
    private:
	
      Rec::TrackParticleContainer * m_IDProbeTrackContainer = nullptr;

      /// get a handle to the InnerDetProbeCollectorTool
      ToolHandle<IInsituTrackTools> m_InsituPerformanceTools;

      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;
      std::string	m_InnerTrackContainerName;
      std::string	m_MSTrackContainerName;
      std::string	m_CombinedMuonTracksContainerName;
      bool		m_RequireTrigger;
      float m_muonPtCut;

    }; 
}

#endif 
