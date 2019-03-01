/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//#include "JetEvent/Jet.h"

#include "JetTagTools/IJetFitterNtupleWriter.h"
#include "JetTagTools/IJetFitterClassifierTool.h"
#include "JetTagTools/IJetFitterVariablesFactory.h"
#include <stdexcept>

#include "CLHEP/Vector/LorentzVector.h"

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
      m_ntupleWriter("Analysis::JetFitterNtupleWriter"),
      // m_variablesFactory("Analysis::JetFitterVariablesFactory"),
      m_classifier("Analysis::JetFitterNNTool")
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

  StatusCode JetFitterTag::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName) {

    /** author to know which jet algorithm: */
    std::string jetauthor = jetName;

    if (m_doForcedCalib) {
      jetauthor = m_ForcedCalibName;
    } 

    double jetpT = jetToTag.pt();
    double jeteta = jetToTag.eta();

    /** for the reference mode we need the true label: */
    std::string pref  = "";
    if (m_runModus == "reference" ) {
      // here we require a jet selection:
      if (jetToTag.pt()>m_jetPtMinRef && fabs(jetToTag.eta())<2.5 ) {
        // and also a truth match:
	int label = xAOD::jetFlavourLabel(&jetToTag);
	double deltaRtoClosestB = 999., deltaRtoClosestC = 999.;
	if (jetToTag.getAttribute("TruthLabelDeltaR_B",deltaRtoClosestB)) {
	  // for purification: require no b or c quark closer
	  // than dR=m_purificationDeltaR
	  jetToTag.getAttribute("TruthLabelDeltaR_C",deltaRtoClosestC);
	  double deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
          //JBdV 04/05/2006 purify also w.r.t tau
          double deltaRtoClosestT;
	  jetToTag.getAttribute("TruthLabelDeltaR_T",deltaRtoClosestT);
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

    
    //IJetFitterTagInfo* theVariables = 0;
    //   m_variablesFactory->getITagInfoObject(jetToTag);

 
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


      // generic tag class supports arbitrary double and int variables 
      // JetFitterGenericTagInfo* generic_vars = 
      // 	dynamic_cast<JetFitterGenericTagInfo*>(theVariables); 

      //if (m_svx_tagger_name.size() > 0 && generic_vars){ 
      // augment_with_svinfoplus(generic_vars, jetToTag); 
      //}
      //if (m_ipinfo_tagger_name.size() > 0 && generic_vars){ 
      //	augment_with_ipinfoplus(generic_vars, jetToTag); 
      //}

      // this function is a bit of a hack: the single_weight is just there
      // to accomidate the JetFitterCOMBNN classifier, which takes 
      // IP3D as a function argument. 
      // double single_weight = get_simple_tagger_weights(theVariables,*BTag);
      // get_tagger_weights(theVariables, *BTag); 

      // try { 
      // 	// this may throw a runtime_error if something is configured wrong
      // 	m_classifier->fillLikelihoodValues(*theVariables,
      // 					   jetauthor,
      // 					   jetToTag.pt(),
      // 					   jetToTag.eta(),
      // 					   single_weight);



      // 	if (generic_vars && !m_save_temporary_variables) { 
      // 	  // some variables which are used in the NN don't need to be 
      // 	  // stored, we can safely remove them now. 
      // 	  generic_vars->clearTemporary(); 
      // 	}
      // }
      // catch (const std::runtime_error e) {
      // 	std::string warning = "problem tagging jet with " + jetauthor + " (";
      // 	warning.append( e.what() ); 
      // 	warning.append("). No tagging will be done."); 
      // 	ATH_MSG_WARNING(warning); 
      // }
      //ELG: what to store not yet defined
      /*JetTagInfoBase* info_to_store = 0; 
      if (m_store_only_base_object) {
        info_to_store = new BaseTagInfo(*theVariables);
        delete theVariables;
        theVariables = 0;
      }
      else {
        info_to_store = theVariables;
      }

      jetToTag.addInfo(info_to_store);
      */
    } // end if "analysis" block 
    return StatusCode::SUCCESS;
  }

  // int JetFitterTag
  // ::augment_with_svinfoplus(JetFitterGenericTagInfo* tag_info, 
  // 			    const xAOD::Jet& jet_with_sv1) { 
  //   /*const SVInfoPlus* sv_info = dynamic_cast<const SVInfoPlus*>
  //     (jet_with_sv1.tagInfo(m_svx_tagger_name));

  //   int n_gt_jet = -1;
  //   int n_gt_svx = -1;
  //   int n_2t     = -1;
  //   double mass       = -1;
  //   double energyfrac = -1;
  //   double normdist   = -1;

  //   // this tag is often missing if no secondary vertex is constructed
  //   if (! sv_info){
  //     // double-check to make sure it's not just misnamed 
  //     if (dynamic_cast<const BaseTagInfo*>
  // 	  (jet_with_sv1.tagInfo(m_svx_tagger_name)))
  // 	throw std::runtime_error
  // 	  (m_svx_tagger_name + " tagger is stored as a base object."); 
  //   }
  //   else { 
  //     n_gt_jet   = sv_info->getNGTrackInJet(); 	
  //     n_gt_svx   = sv_info->getNGTrackInSvx(); 	
  //     n_2t       = sv_info->getN2T(); 		
  //     mass       = sv_info->getMass(); 		
  //     energyfrac = sv_info->getEnergyFraction(); 
  //     normdist   = sv_info->getNormDist();       
  //   }

  //   // keep track of number of overwrites
  //   int n_ow = 0; 

  //   const std::string& tn = m_svx_tagger_name; 
  //   n_ow += tag_info->setTemporaryInt("n_" + tn + "_gt_jet"     , n_gt_jet  );
  //   n_ow += tag_info->setTemporaryInt("n_" + tn + "_gt_svx"     , n_gt_svx  );
  //   n_ow += tag_info->setTemporaryInt("n_" + tn + "_2t"         , n_2t      );
  //   n_ow += tag_info->setTemporaryDouble    (tn + "_mass"       , mass      );
  //   n_ow += tag_info->setTemporaryDouble    (tn + "_energyfrac" , energyfrac);
  //   n_ow += tag_info->setTemporaryDouble    (tn + "_normdist"   , normdist  );
  //   return n_ow;*/
  //   return 0;
  // }

  // int JetFitterTag
  // ::augment_with_ipinfoplus(JetFitterGenericTagInfo* tag_info, 
  // 			    const xAOD::Jet& jet_with_ipinfo) { 
  //   /*const IPInfoPlus* ip_info = dynamic_cast<const IPInfoPlus*>
  //     (jet_with_ipinfo.tagInfo(m_ipinfo_tagger_name));

  //   // TODO: this needs to be filled out more
  //   int n_ip_tracks = -1; 

  //   // this tag is often missing if no secondary vertex is constructed
  //   if (! ip_info){
  //     // double-check to make sure it's not just misnamed 
  //     if (dynamic_cast<const BaseTagInfo*>
  // 	  (jet_with_ipinfo.tagInfo(m_ipinfo_tagger_name)))
  // 	throw std::runtime_error
  // 	  (m_ipinfo_tagger_name + 
  // 	   " tagger is stored as a base object, bad bad bad..."); 
  //   }
  //   else { 
  //     n_ip_tracks = ip_info->numTrackInfo(); 
  //   }

  //   int n_ow = 0; 
  //   const std::string& tn = m_ipinfo_tagger_name; 
  //   n_ow += tag_info->setTemporaryInt("n_" + tn + "_tracks", n_ip_tracks  );
  //   return n_ow;*/
  //   return 0; 
  // }


  // double JetFitterTag
  // ::get_simple_tagger_weights(IJetFitterTagInfo* tag_info, 
  // 			      const xAOD::BTagging& BTag) { 
  //   JetFitterGenericTagInfo* generic_vars = 
  //     dynamic_cast<JetFitterGenericTagInfo*>(tag_info); 
  //   int n_overwrite = 0; 
  //   if (!generic_vars && m_supplementalTaggers.size() > 1) { 
  //     std::string warning = "You've given more than one supplemental"
  // 	" tagger. No room for this using JetFitterTagInfo."
  // 	" Using first tagger given (" + m_supplementalTaggers.at(0) + ")"
  // 	" and dropping all others."; 
  //     ATH_MSG_WARNING(warning); 
  //     m_supplementalTaggers.erase(m_supplementalTaggers.begin() + 1, 
  // 				  m_supplementalTaggers.end()); 
  //   }

  //   double single_weight = 0; 
  //   for (std::vector<std::string>::const_iterator 
  // 	   titr = m_supplementalTaggers.begin(); 
  // 	 titr != m_supplementalTaggers.end(); 
  // 	 titr++) { 
      
  //     const std::vector<double> prob = get_likelihood_vector(BTag, *titr); 

  //     if (prob.size() > 0) {
  // 	double pb = prob.at(0);
  // 	double w  = 0.;
  // 	if (prob.size() == 1) { 
  // 	  w = pb; 
  // 	}
  // 	else {
  // 	  double pu = prob.at(1);
  // 	  if (pb <= 0. || pu <= 0.) {
  // 	  ATH_MSG_WARNING
  // 	    ("At least one " << *titr << 
  // 	     " prob. null (or negative !?) for JetFitter"
  // 	     " : pb,pu = " << pb << " " << pu << 
  // 	     ", conservatively putting the weight to 0");
  // 	  } else {
  // 	    w = log(pb/pu);
  // 	  }
  // 	}
  // 	if (generic_vars) { 
  // 	  n_overwrite += generic_vars->setTemporaryDouble(*titr,w); 
  // 	}
  // 	single_weight = w; 
  //     }
	
  //   }
  //   if (n_overwrite != 0) { 
  //     ATH_MSG_WARNING(n_overwrite << " varaibels have been overwritten"
  // 		      " while filling JetFitterGenericTagInfo"); 
  //   }
  //   return single_weight; 
  // }

  // void JetFitterTag
  // ::get_tagger_weights(IJetFitterTagInfo* tag_info, 
  // 		       const xAOD::BTagging& BTag) { 

  //   JetFitterGenericTagInfo* generic_vars = 
  //     dynamic_cast<JetFitterGenericTagInfo*>(tag_info); 

  //   int n_overwrite = 0; 
  //   if (!generic_vars && m_multiweightSupplementalTaggers.size() > 0) { 
  //     std::string warning = "You've given a supplemental"
  // 	" tagger. No room for this using JetFitterTagInfo."; 
  //     ATH_MSG_WARNING(warning); 
  //   }
  //   if (!generic_vars) return; 

  //   for (std::vector<std::string>::const_iterator 
  // 	   tagger_itr = m_multiweightSupplementalTaggers.begin(); 
  // 	 tagger_itr != m_multiweightSupplementalTaggers.end(); 
  // 	 tagger_itr++) { 

  //     std::vector<double> prob = get_likelihood_vector(BTag,*tagger_itr); 
  //     if (prob.size() > 0) { 
  // 	std::string pb_name = *tagger_itr + "_pb"; 
  // 	double pb = prob.at(0);
  // 	n_overwrite += generic_vars->setTemporaryDouble(pb_name, pb); 
  //     }
  //     if (prob.size() > 1) { 
  // 	std::string pu_name = *tagger_itr + "_pu"; 
  // 	double pu = prob.at(1);
  // 	n_overwrite += generic_vars->setTemporaryDouble(pu_name, pu); 
  //     }
  //     if (prob.size() > 2) { 
  // 	std::string pc_name = *tagger_itr + "_pc"; 
  // 	double pc = prob.at(2); 
  // 	n_overwrite += generic_vars->setTemporaryDouble(pc_name, pc); 
  //     }
  //     if (prob.size() > 3) { 
  // 	std::string ptau_name = *tagger_itr + "_ptau"; 
  // 	double ptau = prob.at(3); 
  // 	n_overwrite += generic_vars->setTemporaryDouble(ptau_name, ptau); 
  //     }
	
  //   }
  //   if (n_overwrite != 0) { 
  //     ATH_MSG_WARNING(n_overwrite << " varaibels have been overwritten"
  // 		      " while filling JetFitterGenericTagInfo"); 
  //   }
  // }

/*  std::vector<double> JetFitterTag
  ::get_likelihood_vector(const xAOD::Jet& jet, 
			  const std::string& tag_name) const { 
    const JetTagInfoBase* pos(jet.tagInfo(tag_name));
    if (pos==0) {
      ATH_MSG_WARNING("Could not find tag to combine it with JetFitter."
		      " Tag name: " << tag_name);
      return std::vector<double>(); 
    } else if (!pos->isValid() && m_proxy_likelihoods.count(tag_name)) { 
      return m_proxy_likelihoods.find(tag_name)->second; 
    }
    return pos->tagLikelihood();
      
  }*/

  // std::vector<double> JetFitterTag
  // ::get_likelihood_vector(const xAOD::BTagging& BTag,
  //                         const std::string& tag_name) const {

  //   std::vector<double> tagLikelihood = std::vector<double>();
  //   double pb = 0,  pu = 0, pc = 0;
  //   BTag.variable<double>(tag_name, "pb", pb);
  //   tagLikelihood.push_back(pb);
  //   BTag.variable<double>(tag_name, "pu", pu);
  //   tagLikelihood.push_back(pu);
  //   //if (m_useCHypo) {
  //     BTag.variable<double>(tag_name, "pc", pc);
  //     tagLikelihood.push_back(pc);
  //   //}
  //   return tagLikelihood;
  // }
}//end namespace

  
