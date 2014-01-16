/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: This class loads all necessary Information from AODs and converts
// them to MSTrackObjects. Moreover the Track-Assoziation is done here. This means
// That  each Track can be assoziated to another Track or Particle...
///////////////////////////////////////////////////////////////////////////

#ifndef MSTRACKOBJECTLOADER_H
#define MSTRACKOBJECTLOADER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"
//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSRootInterface.h"
#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSPhysicsObject.h"
#include "MuonValUtils/MSTrackObject.h"
#include "MuonValUtils/MSTrackAssociator.h"

//#include "MuonRecValidator/MSMuonAnalysis.h"

#include <string>
#include "TH1.h"

class MSTrackObjectLoader
{
	public:
		MSTrackObjectLoader();
		MSTrackObjectLoader(StoreGateSvc *storeGate, bool doAssoziation, bool MustBeCombinedTrack, bool fromESD, ITruthParticleCnvTool* m_cnvTool);
		~MSTrackObjectLoader();

		// Setting up the class
		void	setup(	StoreGateSvc *storeGate, 	// Which Storegate should be used
				bool doAssoziation, 		// Do we want to do Assoziation
				bool MustBeCombinedTrack,	// Should only use combined Tracks in the MuonCollections
				bool fromESD,			//Doing ESD analysis?
				ITruthParticleCnvTool* m_cnvTool);
	
		void	setupTrackPropertyAssoziator(	bool assoProperties, 		// Do we want to assoziate also Isolation Properties to the Tracks?
							bool assoTracktoTracks, 	// Do we also want to assoziate Tracks to Tracks 
											// (and not only Tracks to Truth)
							double coneDistance_min,	// Which 
							double coneDistance_max);

		// The following functions are for loading AOD-information to msEvent
		bool	loadTruthInfo		(MSEvent &msEvent);
		bool	loadTrackInfo		(std::string ContainerName, MSEvent &msEvent);
		bool	loadMuonCollectionInfo	(std::string ContainerName, MSEvent &msEvent);
		bool	loadMuonTriggerInfo	(MSEvent &msEvent);

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
		int	getMother(const TruthParticle *truthParticle);
		int	get_vertex(const TruthParticle *par, double& vx, double& vy, double& vz);
		double	cot(double theta);
		double	errorPT	(double theta, double qOverP, double D_theta, double D_qOverP);
		double	errorEta(double theta, double D_theta);

		bool				m_doAssoziation;
		bool				m_MustBeCombinedTrack;
		std::vector<std::string>	m_AssoziationMuonContainerNames;
		std::vector<std::string>	m_AssoziationTrackContainerNames;

		bool		m_ESD;
		bool		m_assoProperties;
		bool		m_assoTracksToTracks;
		double		m_maxHalo;
		double		m_minHalo;
		
		/** coverter tool McEventCollection -> TruthParticle */
		ITruthParticleCnvTool* cnvTool;

		StoreGateSvc	*m_storeGate;
		MSTrackAssociator	msTrackAssociator;
};

#endif
