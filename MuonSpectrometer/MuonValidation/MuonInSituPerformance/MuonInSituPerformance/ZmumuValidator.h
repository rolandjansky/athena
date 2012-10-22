/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on AODs.
//		First it loads the AOD-information to MSEvent. The analysis itself
//		is based on MSEvent and done in MSZBosonAnalysis
///////////////////////////////////////////////////////////////////////////
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "McParticleEvent/TruthParticle.h"

#include "MuonInSituPerformance/MSZBosonAnalysis.h"
#include "MuonInSituPerformance/MSInsituPerformance.h"

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSPhysicsObject.h"
#include "MuonValUtils/MSTrackObject.h"
#include "MuonValUtils/MSTrackObjectLoader.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSRootInterface.h"

#include "TH1.h"
#include <string>

class IHistogram1D;
class IHistogram2D;
class TTree;
class IAnalysisTools;
class ITruthParticleCnvTool;
namespace Reco { class ITrackToVertex; }

/////////////////////////////////////////////////////////////////////////////
class ZmumuValidator:public AthAlgTool
{
	public:
		ZmumuValidator (const std::string & type, const std::string & name, const IInterface * parent);
~ZmumuValidator();
StatusCode initialize();
   StatusCode finalize();
   StatusCode fillNtuple();
 
	private:
		MSEvent	msCurrectEvent;

		/// get a handle to the tool helper
		IAnalysisTools *m_analysisTools;

		/// a handle on Store Gate
		StoreGateSvc* m_storeGate;

		/** a handle on the Hist/TTree registration service */
   		ITHistSvc * m_thistSvc;
             
	       /// name of the AOD truth particle container to retrieve from Store Gate
                std::string m_truthParticlesName;

		MSTrackObjectLoader msTrackObjectLoader;
		MSZBosonAnalysis msZBosonAnalysis;
		MSInsituPerformance msInsituPerformance;

		cutDefinitons cutD;

		bool m_fromESD;
		
		std::string	m_Outputfile;
		std::string	m_MuonContainer;
		std::string	m_InnerTrackContainer;
		std::string	m_MuonSpectrometerTrackContainer;
	        std::vector<string>     m_TriggerContainer;

                std::string     m_TriggerItem;
                unsigned int    m_TriggerLevel;
		double		m_MaxConeDistance;
		double		m_MinimalMuonPt;
		long 		EventNumber;
						
      /** coverter tool McEventCollection -> TruthParticle */
      ITruthParticleCnvTool* m_cnvTool;
 TTree* m_tree ;
};

 
