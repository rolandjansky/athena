/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: This class loads all necessary Information from AODs and converts
// them to MSTrackObjects. Moreover the Track-Assoziation is done here. This means
// That  each Track can be assoziated to another Track or Particle...
///////////////////////////////////////////////////////////////////////////

#ifndef Muon_MuonDQATrackObjectLoader_H
#define Muon_MuonDQATrackObjectLoader_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"
//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include "MuonDQAUtils/MSEfficiency.h"
#include "MuonDQAUtils/MSRootInterface.h"
#include "MuonDQAUtils/MSPhysicsObject.h"

#include "MuonDQAUtils/MuonDQATrackAssociator.h"
#include "MuonDQAUtils/MuonDQAEvent.h"
#include "MuonDQAUtils/MuonDQATrackObject.h"

#include <string>
#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include "TH1.h"
#include "TTree.h"

static const InterfaceID IID_MuonDQATrackObjectLoader("Muon::MuonDQATrackObjectLoader",1,0);

namespace Muon {

  class MuonDQATrackObjectLoader: public AlgTool {
  public:
    
    /** @brief constructor */
    MuonDQATrackObjectLoader(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonDQATrackObjectLoader();
   
    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQATrackObjectLoader; }
    
    /** @brief AlgTool finalize */
    StatusCode finalize();

    // Setting up the class
    void  setup( StoreGateSvc *storeGate, 	// Which Storegate should be used
		 bool doAssoziation, 		// Do we want to do Assoziation
		 bool MustBeCombinedTrack,	// Should only use combined Tracks in the MuonCollections
		 bool fromESD,			//Doing ESD analysis?
		 ITruthParticleCnvTool* m_cnvTool );
    
    void  setupTrackPropertyAssoziator(	bool assoProperties, 		// Do we want to assoziate also Isolation Properties to the Tracks?
					bool assoTracktoTracks, 	// Do we also want to assoziate Tracks to Tracks (and not only Tracks to Truth)
					double coneDistance_min,	// Which 
					double coneDistance_max);
    
    // The following functions are for loading AOD-information to msEvent
    bool	loadTruthInfo(MuonDQAEvent &msEvent);
    bool	loadTrackInfo(std::string ContainerName, MuonDQAEvent &msEvent);
    bool	loadMuonCollectionInfo(std::string ContainerName, MuonDQAEvent &msEvent);
    bool	loadMuonTriggerInfo(MuonDQAEvent &msEvent);
    
    // Define which MuonContainerNames should be assoziated to other MuonCollections or MuonTrackContainers
    void	setAssoziationMuonContainerName(std::string name)
      {
	m_AssoziationMuonContainerNames.push_back(name);
      }
    
    // Define which MuonTrackContainers should be assoziated to other MuonCollections or MuonTrackContainers
    void	setAssoziationTrackContainerName(std::string name)
      {
	m_AssoziationTrackContainerNames.push_back(name);
      }
    
  private:
    int	getMother(TruthParticle *truthParticle);
    
    int	get_vertex(const TruthParticle *par, double& vx, double& vy, double& vz);
    double cot(double theta);
    
    double errorPT(double theta, double qOverP, double D_theta, double D_qOverP);
    double errorEta(double theta, double D_theta);
    
    bool       			m_doAssoziation;
    bool       			m_MustBeCombinedTrack;
    std::vector<std::string>	m_AssoziationMuonContainerNames;
    std::vector<std::string>	m_AssoziationTrackContainerNames;
    
    bool	        	m_ESD;
    bool		        m_assoProperties;
    bool	        	m_assoTracksToTracks;
    double	        	m_maxHalo;
    double	        	m_minHalo;
		
    /** coverter tool McEventCollection -> TruthParticle */
    ITruthParticleCnvTool*      cnvTool;
    
    StoreGateSvc*                m_storeGate;
    MuonDQATrackAssociator     	 msTrackAssociator;
    MsgStream*                   m_log;
  };
}
#endif
