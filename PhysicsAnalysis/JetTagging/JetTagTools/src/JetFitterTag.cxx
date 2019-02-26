/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class JetFitterTag - CTagging algorithm based on the old
                  "JetFitter" vertexing algorithm

   Created - 23 March 2007

   @author: 
   * Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
   * Christian Weiser (christian.weiser AT physik.uni-freiburg.de)
   * Dan Guest (dguest AT cern.ch)

   --- ( University of FREIBURG and Yale ) ---

   (c) 2007 - ATLAS Detector Software

   main changes: 
   * January 2008 tagger now supports Neural Network based calculation
   * 2012 partially redesigned for c-tagging

********************************************************/

#include "JetTagTools/JetFitterTag.h"

#include "JetTagTools/IJetFitterNtupleWriter.h"
#include "JetTagTools/IJetFitterClassifierTool.h"
#include "JetTagTools/IJetFitterVariablesFactory.h"
#include <stdexcept>

#include "CLHEP/Vector/LorentzVector.h"

#include "JetTagTools/JetTagUtils.h"
#include "ParticleJetTools/JetFlavourInfo.h"

namespace Analysis {

  JetFitterTag::JetFitterTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_runModus("analysis"),
      m_doForcedCalib(false), 
      m_ForcedCalibName("Cone4H1Tower"), 
      // m_svx_tagger_name("SVInfoPlus"), 
      m_ipinfo_tagger_name(""), 
      // m_store_only_base_object(false), 
      // m_save_temporary_variables(false), 
      m_ntupleWriter("Analysis::JetFitterNtupleWriter", this),
      // m_variablesFactory("Analysis::JetFitterVariablesFactory"),
      m_classifier("Analysis::JetFitterNNTool", this)
  {
    

    
    /** number of hypotheses = 2 : b | u */
    m_hypothese.push_back("bottom");
    m_hypothese.push_back("light");
    m_hypothese.push_back("charm");

    
    declareInterface<ITagTool>(this);
    // global configuration:
    declareProperty("Runmodus", m_runModus);
    declareProperty("ListHypotheses",m_hypothese);
    declareProperty("jetPtMinRef", m_jetPtMinRef = 15.*Gaudi::Units::GeV);

    declareProperty("jetCollectionList", m_jetCollectionList);
    declareProperty("useForcedCalibration",  m_doForcedCalib);
    declareProperty("forcedCalibrationName", m_ForcedCalibName);
    // declareProperty("supplementalTaggers", m_supplementalTaggers); 
    // declareProperty( "multiweightSupplementalTaggers", 
    // 		    m_multiweightSupplementalTaggers); 
    // declareProperty("svxTaggerName", m_svx_tagger_name); 
    declareProperty("ipinfoTaggerName", m_ipinfo_tagger_name); 
    // declareProperty("storeOnlyBaseObject", m_store_only_base_object); 
    // declareProperty("saveTemporaryVariables", m_save_temporary_variables); 
    // declareProperty("proxyLikelihoods", m_proxy_likelihoods); 
    declareProperty("SecVxFinderName",m_secVxFinderName);
    declareProperty("xAODBaseName",      m_xAODBaseName);


    // tools:
    declareProperty("jetfitterNtupleWriter",m_ntupleWriter);
    // declareProperty("jetfitterVariablesFactory",m_variablesFactory);
    declareProperty("jetfitterClassifier",m_classifier);
    
  }

  JetFitterTag::~JetFitterTag() {

  }


  StatusCode JetFitterTag::initialize() {

    StatusCode sc = m_ntupleWriter.retrieve();
    if( StatusCode::SUCCESS != sc ) {
      ATH_MSG_ERROR(" creation of Analysis::JetFitterNtupleWriter : " 
		    << m_ntupleWriter << " failed.");
      return sc;
    }

    // /** retrieving JetFitterVariablesFactory */
    // sc = m_variablesFactory.retrieve();
    // if( StatusCode::SUCCESS != sc ) {
    //   ATH_MSG_ERROR(" creation of Analysis::JetFitterVariablesFactory : " 
    // 		    << m_variablesFactory << " failed");
    //   return sc;
    // }

    /** retrieving JetFitterNNTool */
    sc = m_classifier.retrieve();
    if( StatusCode::SUCCESS != sc ) {
      ATH_MSG_ERROR(" creation of Analysis::JetFitterNNTool : " 
		    << m_classifier << " failed");
      return sc;
    }

    /** book calibration histograms if needed */
    if( m_runModus == "reference" ) {

      std::vector<std::string>::const_iterator 
	strbegin = m_jetCollectionList.begin();
      std::vector<std::string>::const_iterator 
	strend = m_jetCollectionList.end();

      for (std::vector<std::string>::const_iterator striter=strbegin;
	   striter!=strend;++striter) {
	ATH_MSG_VERBOSE("#BTAG# booking ntuple for: " << *striter);
        sc = m_ntupleWriter->bookNtuple(*striter);
	if( StatusCode::SUCCESS != sc ) {
	  ATH_MSG_ERROR(" booking ntuple : " 
			<< *striter << " failed");
	  return sc;
	}

      }

    }

    return StatusCode::SUCCESS;
  }


  void JetFitterTag::finalizeHistos() {
  }



  StatusCode JetFitterTag::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterTag::tagJet(const xAOD::Jet* jetToTag, xAOD::BTagging* BTag) {

    /** author to know which jet algorithm: */
    std::string jetauthor = JetTagUtils::getJetAuthor(jetToTag);

    if (m_doForcedCalib) {
      jetauthor = m_ForcedCalibName;
    } 

    double jetpT = jetToTag->pt();
    double jeteta = jetToTag->eta();

    /** for the reference mode we need the true label: */
    std::string pref  = "";
    if (m_runModus == "reference" ) {
      // here we require a jet selection:
      if (jetToTag->pt()>m_jetPtMinRef && fabs(jetToTag->eta())<2.5 ) {
        // and also a truth match:
	int label = xAOD::jetFlavourLabel(jetToTag);
	double deltaRtoClosestB = 999., deltaRtoClosestC = 999.;
	if (jetToTag->getAttribute("TruthLabelDeltaR_B",deltaRtoClosestB)) {
	  // for purification: require no b or c quark closer
	  // than dR=m_purificationDeltaR
	  jetToTag->getAttribute("TruthLabelDeltaR_C",deltaRtoClosestC);
	  double deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
          //JBdV 04/05/2006 purify also w.r.t tau
          double deltaRtoClosestT;
	  jetToTag->getAttribute("TruthLabelDeltaR_T",deltaRtoClosestT);
          deltaRmin = deltaRtoClosestT < deltaRmin ? 
	    deltaRtoClosestT : deltaRmin;
        } else {
          ATH_MSG_ERROR("No TruthInfo ! Cannot run on reference mode !");
          return StatusCode::FAILURE;
        }

	ATH_MSG_VERBOSE(" label " << label << " m_hypothese.size() " << 
			m_hypothese.size());

	//GP here you need then to add the charm...
	if ( ( (   5==label || 4==label || 0==label) && 
	       m_hypothese.size()==3 ) ||
	     ( ( 5==label || 0==label) && m_hypothese.size()==2 ) ) {
          if (5==label) {
            pref = m_hypothese[0];
          } else if (0==label) {
            pref = m_hypothese[1];
          } else if (4 == label) {
	    pref = m_hypothese[2];
	  }
        }
      }
    }

    
    // ====================================
    // ====================================
    // === augment variables here =========
    // ====================================
    // ====================================
    // ELG: Disable reference mode running for now
    if( m_runModus == "reference" ) {
      //  m_ntupleWriter->fillNtuple(pref, jetauthor,
      //				 *theVariables,jetToTag);
    }

    /** give information to the info class. */
    if(m_runModus=="analysis") {

      double IP3Dlike = -1e7;
      if("" != m_ipinfo_tagger_name){
	if ("IP3D"==m_ipinfo_tagger_name) {
	  IP3Dlike = BTag->IP3D_loglikelihoodratio();
	} else {
	  if( ! BTag->loglikelihoodratio(m_ipinfo_tagger_name, IP3Dlike) ) {
	    ATH_MSG_WARNING("JetFitter can not access " << m_ipinfo_tagger_name <<" LLR result");
	  }
	}
      }

      StatusCode sc = m_classifier->fillLikelihoodValues(BTag,
							 jetauthor,
							 m_secVxFinderName,
							 m_xAODBaseName,
							 jetpT,
							 jeteta,
							 IP3Dlike);
      if(sc.isFailure()) {
	    ATH_MSG_WARNING("#BTAG# jet fitter failed in fillLikelihoodValues");
      } 

    } // end if "analysis" block 
    return StatusCode::SUCCESS;
  }

}//end namespace

  
