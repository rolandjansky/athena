/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InsituTrackTools.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef Muon_InsituTrackTools_H
#define Muon_InsituTrackTools_H

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

#include "EventKernel/INavigable4Momentum.h"
#include "MuonDQAUtils/IInsituTrackTools.h"

class AtlasDetectorID;
class Identifier;


/** @class InsituTrackTools 

This is for the Doxygen-Documentation.  
Please delete these lines and fill in information about the Algorithm!
Please precede every member function declaration with a
short Doxygen comment stating the purpose of this function.

@author  Nektarios Benekos <nbenekos@illinois.edu>
*/  
namespace Muon 
{
  class InsituTrackTools : virtual public IInsituTrackTools, public AthAlgTool
    {
    public:
      InsituTrackTools(const std::string&,const std::string&,const IInterface*);
	
      /** default destructor */
      virtual ~InsituTrackTools ();
	
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
		
      bool	isZBosonCandidate(const INavigable4Momentum *track1, const INavigable4Momentum *track2);
      bool	isTriggeredMuon(INavigable4Momentum *track1);
      bool	isTriggeredElectron(INavigable4Momentum *track1);
      bool	isCloseTrack(const INavigable4Momentum *track1, const INavigable4Momentum *track2);
      bool	isIsolatedTrack(const INavigable4Momentum *track1);
      bool	isIsolatedMuon(const INavigable4Momentum *track1);
      bool	isElectronCandidate(INavigable4Momentum *track1);
		
      double	getInvariantMass(const INavigable4Momentum *track1, const INavigable4Momentum *track2);
      double	getDistance(const INavigable4Momentum *track1, const INavigable4Momentum *track2);
      bool	getTrackIsolation(const INavigable4Momentum *trackParticle, float &PtIsolation, int &NIsolation);
      float	getTrackPtIsolation(INavigable4Momentum *trackParticle);
      int	getNTrackIsolation(INavigable4Momentum *trackParticle);
      float	getJetIsolation(const INavigable4Momentum *trackParticle);
       	
    private:
	
      /** class member version of retrieving MsgStream */
      mutable MsgStream	m_log;
      /// a handle on Store Gate 
      StoreGateSvc* m_storeGate;
      /// a handle on the Hist/TTree registration service
      ITHistSvc * m_thistSvc;
		
      /** member variables for algorithm properties: */
      std::string	m_InnerTrackContainerName;
      std::string	m_ConeJetContainerName;
		
      /// Cut variables
      double		m_MaximalMassDifferenceToZBoson;
      double		m_MaximalPTofTracksInCone;
      double		m_MaximalNumberofTracksInCone;
      double		m_MaximalJetEnergyInCone;
    }; 
}

#endif 
