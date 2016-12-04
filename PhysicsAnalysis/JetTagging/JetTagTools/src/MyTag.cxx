/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackParticle.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "JetTagTools/MyTag.h"

#include "xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"

#include "TObjString.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "JetTagTools/JetTagUtils.h"

#include "AthenaKernel/Units.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <math.h>       /* hypot */

using Athena::Units::GeV;

namespace Analysis {

  /**
      @class MyTag
      BDT-based tagger combining more than 20 upper stream b-tagging variables
      @author K.Mochizuki, L.Vacavant, M.Ughetto
      modified May 2016 J. Shlomi
  */

  MyTag::MyTag(const std::string& name, const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_calibrationTool("BTagCalibrationBroker"),
    m_runModus("analysis") {

    declareInterface<IMultivariateJetTagger>(this);

    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);

    // force MV2 to always use a calibration derived from MV2CalibAlias jet collection
    declareProperty("forceMV2CalibrationAlias", m_forceMV2CalibrationAlias = true);
    declareProperty("MV2CalibAlias", m_MV2CalibAlias = "AntiKt4TopoEM");

    // global configuration:
    declareProperty("Runmodus", m_runModus);
    //declareProperty("DecorateMvaInputs", m_decorateBTaggingObj=false);

    declareProperty("xAODBaseName",      m_xAODBaseName);//"MV2c20" or etc.

    // which calibration folder to use
    declareProperty("taggerNameBase", m_taggerNameBase = "MV2");
    declareProperty("taggerName", m_taggerName = "MV2");
    declareProperty("decTagName", m_decTagName = "MV2_inputs");

  }


  MyTag::~MyTag() {
  }


  StatusCode MyTag::initialize() {

    m_disableAlgo=false;
    m_useEgammaMethodMV2=false;
    m_warnCounter=0;

    m_treeName = "BDT";
    m_varStrName = "variables";

    // prepare calibration tool:
    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib/"+m_treeName);
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib/"+m_varStrName);
    m_tmvaReaders.clear();
    m_tmvaMethod.clear();
    m_egammaBDTs.clear();
    return StatusCode::SUCCESS;
  }


  StatusCode MyTag::finalize() {
    ATH_MSG_DEBUG("#BTAG# Finalizing MV2.");
    if (m_useEgammaMethodMV2) {
      for( auto temp: m_egammaBDTs ) if(temp.second) delete temp.second;
    }
    else {
      // delete readers:
      std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
      for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
      std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
      for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    }
    return StatusCode::SUCCESS;
  }

  void MyTag::assignProbability(xAOD::BTagging *BTag,
         const std::map<std::string, double> &inputs,
         const std::string& assigned_jet_author){

     /*
     * #index for this function
     * #1: Preparation of MVA instance using tmva/MVAUtils BDT
     * #2: set input variables from MultivariateTagManager inputs map
     * #3: Calcuation of MVA output variable(s)
     * #4: Fill MVA output variable(s) into xAOD
     */

    // #0: Check if disabled or not
    if ( m_disableAlgo ) {
      if (m_warnCounter<20) {
        ATH_MSG_WARNING("#BTAG# .... algorithm was disabled due to mis-configuration problems: no output will be written ... PLEASE WATCH OUT" );
        m_warnCounter++;
      }
    }

    // #1: Preparation of MVA instance using tmva/egammaBDT
    /* jet author: */
    std::string author;

    if (m_forceMV2CalibrationAlias) {
      author = m_MV2CalibAlias;
    }
    else {
      author = assigned_jet_author;
    }


    std::string alias = m_calibrationTool->channelAlias(author);

    ATH_MSG_DEBUG("#BTAG# Jet author for MV2: " << author << ", alias: " << alias );

    /* check if calibration has to be updated: */
    std::pair<TObject*, bool> calib=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib");

    bool calibHasChanged = calib.second;

    TMVA::Reader* tmvaReader=0;     std::map<std::string, TMVA::Reader*>::iterator pos;
    TMVA::MethodBase * kl=0;        std::map<std::string, TMVA::MethodBase*>::iterator it_mb;
    MVAUtils::BDT *bdt=0; std::map<std::string, MVAUtils::BDT*>::iterator it_egammaBDT;

    /*KM: Retrieval of objects from the calibration file and store it back in the calibration broker temporarily*/
    if(calibHasChanged) {
      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
	ATH_MSG_WARNING("#BTAG# TObject can't be retrieved -> no calibration for"<< m_taggerNameBase<<" "<<alias<<" "<<author);
	m_disableAlgo=true;
	return;
      }
      else {
	const TString rootClassName=calib.first->ClassName();

	if      (rootClassName=="TDirectoryFile") m_useEgammaMethodMV2=true;
	else if (rootClassName=="TList")          m_useEgammaMethodMV2=false;//tmva method
	else {
	  ATH_MSG_WARNING("#BTAG# Unsupported ROOT class type: "<<rootClassName<<" is retrieved. Disabling algorithm..");
	  m_disableAlgo=true;
    return;
	}

      }
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);

      //const std::string treeName  ="BDT";
      //const std::string varStrName="variables";
      std::vector<std::string> inputVars; inputVars.clear();

      if (!m_useEgammaMethodMV2) {
	ATH_MSG_INFO("#BTAG# Booking TMVA::Reader for "<<m_taggerNameBase);
	std::ostringstream iss; //iss.clear();
	//now the new part istringstream
	TList* list = (TList*)calib.first;

  for(int i=0; i<list->GetSize(); ++i) {

    TObjString* ss = (TObjString*)list->At(i);
	  std::string sss = ss->String().Data();

    //KM: if it doesn't find "<" in the string, it starts from non-space character
	  int posi = sss.find('<')!=std::string::npos ? sss.find('<') : sss.find_first_not_of(" ");
	  std::string tmp = sss.erase(0,posi);

	  iss << tmp.data();
	  if (tmp.find("<Variable")!=std::string::npos ) {
	    if ( tmp.find("Variable VarIndex")!=std::string::npos ) {
	      std::string varIndex  =tmp.substr(tmp.find("=\"")+2, tmp.find("\" ")-(tmp.find("=\"")+2));
	      std::string tmpVar  = tmp.erase(0,tmp.find("Expression=\"")+12);
	      std::string varExpress=tmp.substr(0, tmp.find("\""));
	      inputVars.push_back(varExpress);
	    }
	  }
	  // else if (tmp.find("NClass")!=std::string::npos ) {
	  //   std::string newString=tmp.substr(tmp.find("\"")+1,tmp.find("\" ")-(tmp.find("\"")+1));
	  //   nClasses =stoi(newString);
	  // }
	}
	m_calibrationTool->storeCalib(m_taggerNameBase, alias, m_taggerNameBase+"Calib", inputVars, iss.str(), 0);

	iss.clear();
      }
      else {//if m_useEgammaMethodMV2
	std::pair<TObject*, bool> calibTree=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib/"+m_treeName);
	std::pair<TObject*, bool> calibVariables=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib/"+m_varStrName);
	TTree *tree = (TTree*) calibTree.first;
	TObjArray* toa= (TObjArray*) calibVariables.first;
	std::string commaSepVars="";
	if (toa) {
	  TObjString *tos= 0;
	  if (toa->GetEntries()>0) tos= (TObjString*) toa->At(0);
	  commaSepVars=tos->GetString().Data();
	}

	//prepare inputVars
	while (commaSepVars.find(",")!=std::string::npos) {
	  inputVars.push_back(commaSepVars.substr(0,commaSepVars.find(",")));
	  commaSepVars.erase(0,commaSepVars.find(",")+1);
	}
	inputVars.push_back(commaSepVars.substr(0,-1));

	m_calibrationTool->storeCalib(m_taggerNameBase, alias, m_taggerNameBase+"Calib", inputVars, "", tree);
      }
    }//calibHasChanged

    /*KM: Get back the calib objects from calibration broker*/
    if (!m_calibrationTool->updatedTagger(m_taggerNameBase, alias, m_taggerNameBase+"Calib", name()) ) {
      std::vector<float*>  inputPointers; inputPointers.clear();
      unsigned nConfgVar=0; bool badVariableFound=false;

      CalibrationBroker::calibMV2 calib = m_calibrationTool->getCalib(m_taggerNameBase, alias, m_taggerNameBase+"Calib");
      std::vector<std::string> inputVars = calib.inputVars;
      std::string str = calib.str;
      TTree* tree = calib.obj!=0 ? (TTree*) calib.obj->Clone() : 0;

      if      (str=="" and tree!=0) {	m_useEgammaMethodMV2=true;        }
      else if (str!="" and tree==0) {	m_useEgammaMethodMV2=false;       }
      else{
	ATH_MSG_WARNING("#BTAG# Unrecognized MV2 configuration disabling the algorithm..." );
	m_disableAlgo=true;
	return;
      }
      ATH_MSG_VERBOSE("#BTAG# MV2 m_useEgammaMethodMV2= "<<m_useEgammaMethodMV2 );

      if (!m_useEgammaMethodMV2) {
	// now configure the TMVAReaders:
	/// check if the reader for this tagger needs update
	tmvaReader = new TMVA::Reader();

  //Input variables :
  //replace NAN default values and, assign the values from the MVTM input map to the relevant variables
  //currently default values are hard coded in the definition of ReplaceNaN_andAssign()

  SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);


   ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( inputVars.size()!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING("#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
	  return;
	}

	//tmvaReader->BookMVA("BDT", xmlFileName);
	TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kBDT, str.data() );
	kl = dynamic_cast<TMVA::MethodBase*>(method);

	// add it or overwrite it in the map of readers:
	pos = m_tmvaReaders.find(alias);
	if(pos!=m_tmvaReaders.end()) {
	  delete pos->second;
	  m_tmvaReaders.erase(pos);
	}
	m_tmvaReaders.insert( std::make_pair( alias, tmvaReader ) );

	it_mb = m_tmvaMethod.find(alias);
	if(it_mb!=m_tmvaMethod.end()) {
	  delete it_mb->second;
	  m_tmvaMethod.erase(it_mb);
	}
	m_tmvaMethod.insert( std::make_pair( alias, kl ) );

      }
      else {//if m_useEgammaMethodMV2
	ATH_MSG_INFO("#BTAG# Booking MVAUtils::BDT for "<<m_taggerNameBase);

	// TDirectoryFile* f= (TDirectoryFile*)calib.first;
	// TTree *tree = (TTree*) f->Get(treeName.data());

	if (tree) {
	  bdt = new MVAUtils:: BDT(tree);
	  delete tree;//<- Crash at finalization if w/o this
	}
	else {
	  ATH_MSG_WARNING("#BTAG# No TTree with name: "<<m_treeName<<" exists in the calibration file.. Disabling algorithm.");
	  m_disableAlgo=true;
    return;

	}

  SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);

  ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( inputVars.size()!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING( "#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
    delete bdt;
    return;
	}

	bdt->SetPointers(inputPointers);

	it_egammaBDT = m_egammaBDTs.find(alias);
	if(it_egammaBDT!=m_egammaBDTs.end()) {
	  delete it_egammaBDT->second;
	  m_egammaBDTs.erase(it_egammaBDT);
	}
	m_egammaBDTs.insert( std::make_pair( alias, bdt ) );

      }
      m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase,alias, m_taggerNameBase+"Calib", false, name());
    }

    // #2 fill inputs
    //replace NAN default values and, assign the values from the MVTM input map to the relevant variables
    //currently default values are hard coded in the definition of ReplaceNaN_andAssign()
    ReplaceNaN_andAssign(inputs);

    // #3: Calcuation of MVA output variable(s)
    /* compute MV2: */
    double mv2 = -10.;  double mv2m_pb=-10., mv2m_pu=-10., mv2m_pc=-10.;

    //TMVA method
    if (!m_useEgammaMethodMV2) {

      pos = m_tmvaReaders.find(alias);
      if(pos==m_tmvaReaders.end()) {
        int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
        if(0==alreadyWarned) {
          ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection alias, author: "<<alias<<" "<<author);
	        m_undefinedReaders.push_back(alias);
        }
      } else {
        it_mb = m_tmvaMethod.find(alias);
        if( (it_mb->second)!=0 ){
          if(m_taggerNameBase.find("MV2c")!=std::string::npos) mv2 = pos->second->EvaluateMVA( it_mb->second );//this gives back double
          else {
            std::vector<float> outputs= pos->second->EvaluateMulticlass( it_mb->second );//this gives back float
            if (outputs.size()==m_nClasses){
            mv2m_pb=outputs[0]; mv2m_pu=outputs[1]; mv2m_pc=outputs[2];
            } else ATH_MSG_WARNING("#BTAG# Unkown error, outputs vector size not "<<m_nClasses<<"!!!" );
          }
        }
        else ATH_MSG_WARNING("#BTAG#  kl==0 for alias, author: "<<alias<<" "<<author);
      }

    }
    // use MVAUtils Egamma method
    else {
      it_egammaBDT = m_egammaBDTs.find(alias);
      if(it_egammaBDT==m_egammaBDTs.end()) {
        int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
        if(0==alreadyWarned) {
	         ATH_MSG_WARNING("#BTAG# no egammaBDT defined for jet collection alias, author: "<<alias<<" "<<author);
	         m_undefinedReaders.push_back(alias);
	      }
      }
      else {
        if(it_egammaBDT->second !=0) {
          if (m_taggerNameBase.find("MV2c")!=std::string::npos) mv2= GetClassResponse(it_egammaBDT->second);//this gives back double
	        else { //if it is MV2m
      	    std::vector<float> outputs= GetMulticlassResponse(it_egammaBDT->second);//this gives back float
      	    //vector size is checked in the function above
      	    mv2m_pb=outputs[0]; mv2m_pu=outputs[1]; mv2m_pc=outputs[2] ;
	        }
	      }
	     else ATH_MSG_WARNING("#BTAG# egamma BDT is 0 for alias, author: "<<alias<<" "<<author);
      }
    }

    if (m_taggerNameBase.find("MV2c")!=std::string::npos) ATH_MSG_DEBUG("#BTAG# MV2 weight: " << mv2<<", "<<alias<<", "<<author);
    else ATH_MSG_DEBUG("#BTAG# MV2 pb, pu, pc= " << mv2m_pb<<"\t"<<mv2m_pu<<"\t"<<mv2m_pc<<", "<<alias<<", "<<author);

    // #4: Fill MVA output variable(s) into xAOD
    /** give information to the info class. */
    if(m_runModus=="analysis") {

      if (m_taggerNameBase.find("MV2c")!=std::string::npos) {
        //MV2cXX
        BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv2);
      }else {
        //MV2m
        BTag->setVariable<double>(m_xAODBaseName, "pb", mv2m_pb);
        BTag->setVariable<double>(m_xAODBaseName, "pu", mv2m_pu);
        BTag->setVariable<double>(m_xAODBaseName, "pc", mv2m_pc);
      }
    }

  }//end assign probability

  float MyTag::d0sgn_wrtJet(const TLorentzVector& jet, const TLorentzVector& trk, float d0sig) {
    const double dPhi = jet.DeltaPhi(trk);
    const float d0_sign = sin(dPhi) * d0sig;

    if (std::fabs(d0_sign) < 1e-4)
      return 1.0;

    const float res = d0_sign / std::fabs(d0_sign);
    return res;
  }

  float MyTag::z0sgn_wrtJet(float trackTheta, float trackZ0, float jetEta) {
    const float trackEta = -std::log(std::tan(trackTheta/2.));
    const float zs = (jetEta - trackEta)*trackZ0;
    return (zs>=0. ? 1. : -1.);
  }


void MyTag::ReplaceNaN_andAssign(std::map<std::string, double> var_map){
    //map of default values, which replace nan values provided by MultivariateTagManager
    //TODO: read the default values from the to the calibration file
    std::map<std::string, double > defaultvals;

     defaultvals["m_pt"]                          = -99;
     defaultvals["m_absEta"]                      = -99;
     defaultvals["m_ip2"]                         = -30;
     defaultvals["m_ip2_c"]                       = -30;
     defaultvals["m_ip2_cu"]                      = -30;
     defaultvals["m_ip3"]                         = -30;
     defaultvals["m_ip3_c"]                       = -30;
     defaultvals["m_ip3_cu"]                      = -30;
     defaultvals["m_sv1"]                         = -10;
     defaultvals["m_sv1_c"]                       = -10;
     defaultvals["m_sv1_cu"]                      = -10;
     defaultvals["m_sv1_ntkv"]                    = -1;
     defaultvals["m_sv1_mass"]                    = -1000;
     defaultvals["m_sv1_efrc"]                    = -1;
     defaultvals["m_sv1_n2t"]                     = -1;
     defaultvals["m_sv1_Lxy"]                     = -100;
     defaultvals["m_sv1_L3d"]                     = -100;
     defaultvals["m_sv1_sig3"]                    = -100;
     defaultvals["m_sv1_dR"]                      = -1;
     defaultvals["m_sv1_distmatlay"]              = -1;
     defaultvals["m_jf_dR"]                       = -1;
     defaultvals["m_jf_dR_flight"]                = -1;
     defaultvals["m_jf_mass_unco"]                = -1000;
     defaultvals["m_ip2_pu"]                      = -1;
     defaultvals["m_ip2_pb"]                      = -1;
     defaultvals["m_ip2_pc"]                      = -1;
     defaultvals["m_ip3_pu"]                      = -1;
     defaultvals["m_ip3_pb"]                      = -1;
     defaultvals["m_ip3_pc"]                      = -1;
     defaultvals["m_sv1_pu"]                      = -1;
     defaultvals["m_sv1_pb"]                      = -1;
     defaultvals["m_sv1_pc"]                      = -1;
     defaultvals["m_sv0"]                         = -1;
     defaultvals["m_sv0_ntkv"]                    = -1;
     defaultvals["m_sv0_mass"]                    = -1;
     defaultvals["m_sv0_efrc"]                    = -1;
     defaultvals["m_sv0_n2t"]                     = -1;
     defaultvals["m_jf_mass"]                     = -1000;
     defaultvals["m_jf_efrc"]                     = -1;
     defaultvals["m_jf_n2tv"]                     = -1;
     defaultvals["m_jf_ntrkv"]                    = -1;
     defaultvals["m_jf_nvtx"]                     = -1;
     defaultvals["m_jf_nvtx1t"]                   = -1;
     defaultvals["m_jf_dphi"]                     = -11;
     defaultvals["m_jf_deta"]                     = -11;
     defaultvals["m_jf_sig3"]                     = -100;
     defaultvals["m_width"]                       = 0.0;
     defaultvals["m_n_trk_sigd0cut"]              = 0.0;
     defaultvals["m_trk3_d0sig"]                  = -100;
     defaultvals["m_trk3_z0sig"]                  = -100;
     defaultvals["m_sv_scaled_efc"]               = -1;
     defaultvals["m_jf_scaled_efc"]               = -1;
     defaultvals["m_nTrk_vtx1"]                   =  0.0;
     defaultvals["m_mass_first_vtx"]              =  -100.0;
     defaultvals["m_e_first_vtx"]                 =  -100.0;
     defaultvals["m_e_frac_vtx1"]                 =  0.0;
     defaultvals["m_closestVtx_L3D"]              =  -10;
     defaultvals["m_JF_Lxy1"]                     =  -5;
     defaultvals["m_vtx1_MaxTrkRapidity_jf_path"] =  0.0;
     defaultvals["m_vtx1_AvgTrkRapidity_jf_path"] =  0.0;
     defaultvals["m_vtx1_MinTrkRapidity_jf_path"] =  0.0;
     defaultvals["m_MaxTrkRapidity_jf_path"]      =  0.0;
     defaultvals["m_MinTrkRapidity_jf_path"]      =  0.0;
     defaultvals["m_AvgTrkRapidity_jf_path"]      =  0.0;
     defaultvals["m_trkSum_ntrk"]                 = -1;
     defaultvals["m_trkSum_sPt"]                  = -1000; //<== -1 GeV
     defaultvals["m_trkSum_vPt"]                  = -1000;
     defaultvals["m_trkSum_vAbsEta"]              = -10;
     defaultvals["m_sm_mu_pt"]                    = -1;
     defaultvals["m_sm_dR"]                       = -1;
     defaultvals["m_sm_qOverPratio"]              = -99;
     defaultvals["m_sm_mombalsignif"]             = -99;
     defaultvals["m_sm_scatneighsignif"]          = -99;
     defaultvals["m_sm_pTrel"]                    = -99;
     defaultvals["m_sm_mu_d0"]                    = -99;
     defaultvals["m_sm_mu_z0"]                    = -99;
     defaultvals["m_sm_ID_qOverP"]                = -99;

     defaultvals["m_my_smt"]                      = -99;

    //assign values or replace NAN to defaults
    //note that the name of the variables in MVTM is not always identical to MV2 inputs
    m_pt                          = !std::isnan(var_map["jet_pt"]) ? var_map["jet_pt"] : defaultvals["m_pt"];
    m_absEta                      = !std::isnan(var_map["jet_abs_eta"]) ? var_map["jet_abs_eta"] : defaultvals["m_absEta"];

    m_trkSum_ntrk                 = !std::isnan(var_map["trkSum_ntrk"]) ? var_map["trkSum_ntrk"] : defaultvals["m_trkSum_ntrk"];
    m_trkSum_sPt                  = !std::isnan(var_map["trkSum_sPt"]) ? var_map["trkSum_sPt"] : defaultvals["m_trkSum_sPt"];
    m_trkSum_vPt                  = !std::isnan(var_map["trkSum_vPt"]) ? var_map["trkSum_vPt"] : defaultvals["m_trkSum_vPt"];
    m_trkSum_vAbsEta              = !std::isnan(var_map["trkSum_vAbsEta"]) ? var_map["trkSum_vAbsEta"] : defaultvals["m_trkSum_vAbsEta"];

    //IP2D variables
    m_ip2                         = !std::isnan(var_map["ip2"]) ? var_map["ip2"] : defaultvals["m_ip2"];
    m_ip2_c                       = !std::isnan(var_map["ip2_c"]) ? var_map["ip2_c"] : defaultvals["m_ip2_c"];
    m_ip2_cu                      = !std::isnan(var_map["ip2_cu"]) ? var_map["ip2_cu"] : defaultvals["m_ip2_cu"];

    m_ip2_pu                      = !std::isnan(var_map["ip2d_pu"]) ? var_map["ip2d_pu"] : defaultvals["m_ip2_pu"];
    m_ip2_pb                      = !std::isnan(var_map["ip2d_pb"]) ? var_map["ip2d_pb"] : defaultvals["m_ip2_pb"];
    m_ip2_pc                      = !std::isnan(var_map["ip2d_pc"]) ? var_map["ip2d_pc"] : defaultvals["m_ip2_pc"];

    //IP3D variables
    m_ip3                         = !std::isnan(var_map["ip3"]) ? var_map["ip3"] : defaultvals["m_ip3"];
    m_ip3_c                       = !std::isnan(var_map["ip3_c"]) ? var_map["ip3_c"] : defaultvals["m_ip3_c"];
    m_ip3_cu                      = !std::isnan(var_map["ip3_cu"]) ? var_map["ip3_cu"] : defaultvals["m_ip3_cu"];

    m_ip3_pu                      = !std::isnan(var_map["ip3d_pu"]) ? var_map["ip3d_pu"] : defaultvals["m_ip3_pu"];
    m_ip3_pb                      = !std::isnan(var_map["ip3d_pb"]) ? var_map["ip3d_pb"] : defaultvals["m_ip3_pb"];
    m_ip3_pc                      = !std::isnan(var_map["ip3d_pc"]) ? var_map["ip3d_pc"] : defaultvals["m_ip3_pc"];

    //SV1 variables
    m_sv1                         = !std::isnan(var_map["sv1"]) ? var_map["sv1"] : defaultvals["m_sv1"];
    m_sv1_c                       = !std::isnan(var_map["sv1_c"]) ? var_map["sv1_c"] : defaultvals["m_sv1_c"];
    m_sv1_cu                      = !std::isnan(var_map["sv1_cu"]) ? var_map["sv1_cu"] : defaultvals["m_sv1_cu"];
    m_sv1_pu                      = !std::isnan(var_map["sv1_pu"]) ? var_map["sv1_pu"] : defaultvals["m_sv1_pu"];
    m_sv1_pb                      = !std::isnan(var_map["sv1_pb"]) ? var_map["sv1_pb"] : defaultvals["m_sv1_pb"];
    m_sv1_pc                      = !std::isnan(var_map["sv1_pc"]) ? var_map["sv1_pc"] : defaultvals["m_sv1_pc"];

    m_sv1_ntkv                    = !std::isnan(var_map["sv1_ntrkv"]) ? var_map["sv1_ntrkv"] : defaultvals["m_sv1_ntkv"];
    m_sv1_mass                    = !std::isnan(var_map["sv1_m"]) ? var_map["sv1_m"] : defaultvals["m_sv1_mass"];
    m_sv1_efrc                    = !std::isnan(var_map["sv1_efc"]) ? var_map["sv1_efc"] : defaultvals["m_sv1_efrc"];
    m_sv1_n2t                     = !std::isnan(var_map["sv1_n2t"]) ? var_map["sv1_n2t"] : defaultvals["m_sv1_n2t"];
    m_sv1_Lxy                     = !std::isnan(var_map["sv1_Lxy"]) ? var_map["sv1_Lxy"] : defaultvals["m_sv1_Lxy"];
    m_sv1_L3d                     = !std::isnan(var_map["sv1_L3d"]) ? var_map["sv1_L3d"] : defaultvals["m_sv1_L3d"];
    m_sv1_sig3                    = !std::isnan(var_map["sv1_sig3d"]) ? var_map["sv1_sig3d"] : defaultvals["m_sv1_sig3"];
    m_sv1_dR                      = !std::isnan(var_map["sv1_dR"]) ? var_map["sv1_dR"] : defaultvals["m_sv1_dR"];
    m_sv1_distmatlay              = !std::isnan(var_map["sv1_distmatlay"]) ? var_map["sv1_distmatlay"] : defaultvals["m_sv1_distmatlay"];

    //JetFitter variables
    m_jf_mass                     = !std::isnan(var_map["jf_m"]) ? var_map["jf_m"] : defaultvals["m_jf_mass"];
    m_jf_efrc                     = !std::isnan(var_map["jf_efc"]) ? var_map["jf_efc"] : defaultvals["m_jf_efrc"];
    m_jf_n2tv                     = !std::isnan(var_map["jf_n2t"]) ? var_map["jf_n2t"] : defaultvals["m_jf_n2tv"];
    m_jf_ntrkv                    = !std::isnan(var_map["jf_ntrkAtVx"]) ? var_map["jf_ntrkAtVx"] : defaultvals["m_jf_ntrkv"];
    m_jf_nvtx                     = !std::isnan(var_map["jf_nvtx"]) ? var_map["jf_nvtx"] : defaultvals["m_jf_nvtx"];
    m_jf_nvtx1t                   = !std::isnan(var_map["jf_nvtx1t"]) ? var_map["jf_nvtx1t"] : defaultvals["m_jf_nvtx1t"];
    m_jf_dphi                     = !std::isnan(var_map["jf_dphi"]) ? var_map["jf_dphi"] : defaultvals["m_jf_dphi"];
    m_jf_deta                     = !std::isnan(var_map["jf_deta"]) ? var_map["jf_deta"] : defaultvals["m_jf_deta"];
    m_jf_sig3                     = !std::isnan(var_map["jf_sig3d"]) ? var_map["jf_sig3d"] : defaultvals["m_jf_sig3"];
    m_jf_dR                       = !std::isnan(var_map["jf_dR"]) ? var_map["jf_dR"] : defaultvals["m_jf_dR"];
    m_jf_dR_flight                = !std::isnan(var_map["jf_dRFlightDir"]) ? var_map["jf_dRFlightDir"] : defaultvals["m_jf_dR_flight"];
    m_jf_mass_unco                = !std::isnan(var_map["jf_m_uncor"]) ? var_map["jf_m_uncor"] : defaultvals["m_jf_mass_unco"];

    //SV0
    m_sv0                         = !std::isnan(var_map["sv0_sig3d"]) ? var_map["sv0_sig3d"] : defaultvals["m_sv0"];
    m_sv0_ntkv                    = !std::isnan(var_map["sv0_ntrkv"]) ? var_map["sv0_ntrkv"] : defaultvals["m_sv0_ntkv"];
    m_sv0_mass                    = !std::isnan(var_map["sv0_m"]) ? var_map["sv0_m"] : defaultvals["m_sv0_mass"];
    m_sv0_efrc                    = !std::isnan(var_map["sv0_efc"]) ? var_map["sv0_efc"] : defaultvals["m_sv0_efrc"];
    m_sv0_n2t                     = !std::isnan(var_map["sv0_n2t"]) ? var_map["sv0_n2t"] : defaultvals["m_sv0_n2t"];

    //MVb variables
    m_width                       = !std::isnan(var_map["jet_width"]) ? var_map["jet_width"] : defaultvals["m_width"];
    m_n_trk_sigd0cut              = !std::isnan(var_map["jet_n_trk_sigd0cut"]) ? var_map["jet_n_trk_sigd0cut"] : defaultvals["m_n_trk_sigd0cut"];
    m_trk3_d0sig                  = !std::isnan(var_map["jet_trk3_d0sig"]) ? var_map["jet_trk3_d0sig"] : defaultvals["m_trk3_d0sig"];
    m_trk3_z0sig                  = !std::isnan(var_map["jet_trk3_z0sig"]) ? var_map["jet_trk3_z0sig"] : defaultvals["m_trk3_z0sig"];
    m_sv_scaled_efc               = !std::isnan(var_map["jet_sv_scaled_efc"]) ? var_map["jet_sv_scaled_efc"] : defaultvals["m_sv_scaled_efc"];
    m_jf_scaled_efc               = !std::isnan(var_map["jet_jf_scaled_efc"]) ? var_map["jet_jf_scaled_efc"] : defaultvals["m_jf_scaled_efc"];

    //extra JetFitter variables, used in MV2cl100 and MV2c100
    m_nTrk_vtx1                   = !std::isnan(var_map["nTrk_vtx1"]) ? var_map["nTrk_vtx1"] : defaultvals["m_nTrk_vtx1"];
    m_mass_first_vtx              = !std::isnan(var_map["mass_first_vtx"]) ? var_map["mass_first_vtx"] : defaultvals["m_mass_first_vtx"];
    m_e_first_vtx                 = !std::isnan(var_map["e_first_vtx"]) ? var_map["e_first_vtx"] : defaultvals["m_e_first_vtx"];
    m_e_frac_vtx1                 = !std::isnan(var_map["e_frac_vtx1"]) ? var_map["e_frac_vtx1"] : defaultvals["m_e_frac_vtx1"];
    m_closestVtx_L3D              = !std::isnan(var_map["closestVtx_L3D"]) ? var_map["m_closestVtx_L3D"] : defaultvals["m_closestVtx_L3D"];
    m_JF_Lxy1                     = !std::isnan(var_map["JF_Lxy1"]) ? var_map["JF_Lxy1"] : defaultvals["m_JF_Lxy1"];
    m_vtx1_MaxTrkRapidity_jf_path = !std::isnan(var_map["vtx1_MaxTrkRapidity_jf_path"]) ? var_map["vtx1_MaxTrkRapidity_jf_path"] : defaultvals["m_vtx1_MaxTrkRapidity_jf_path"];
    m_vtx1_AvgTrkRapidity_jf_path = !std::isnan(var_map["vtx1_AvgTrkRapidity_jf_path"]) ? var_map["vtx1_AvgTrkRapidity_jf_path"] : defaultvals["m_vtx1_AvgTrkRapidity_jf_path"];
    m_vtx1_MinTrkRapidity_jf_path = !std::isnan(var_map["vtx1_MinTrkRapidity_jf_path"]) ? var_map["vtx1_MinTrkRapidity_jf_path"] : defaultvals["m_vtx1_MinTrkRapidity_jf_path"];
    m_MaxTrkRapidity_jf_path      = !std::isnan(var_map["MaxTrkRapidity_jf_path"]) ? var_map["MaxTrkRapidity_jf_path"] : defaultvals["m_MaxTrkRapidity_jf_path"];
    m_MinTrkRapidity_jf_path      = !std::isnan(var_map["MinTrkRapidity_jf_path"]) ? var_map["MinTrkRapidity_jf_path"] : defaultvals["m_MinTrkRapidity_jf_path"];
    m_AvgTrkRapidity_jf_path      = !std::isnan(var_map["AvgTrkRapidity_jf_path"]) ? var_map["AvgTrkRapidity_jf_path"] : defaultvals["m_AvgTrkRapidity_jf_path"];

    //soft muon variables
    m_sm_mu_pt                    = !std::isnan(var_map["sm_mu_pt"]) ? var_map["sm_mu_pt"] : defaultvals["m_sm_mu_pt"];
    m_sm_dR                       = !std::isnan(var_map["sm_dR"]) ? var_map["sm_dR"] : defaultvals["m_sm_dR"];
    m_sm_qOverPratio              = !std::isnan(var_map["sm_qOverPratio"]) ? var_map["sm_qOverPratio"] : defaultvals["m_sm_qOverPratio"];
    m_sm_mombalsignif             = !std::isnan(var_map["sm_mombalsignif"]) ? var_map["sm_mombalsignif"] : defaultvals["m_sm_mombalsignif"];
    m_sm_scatneighsignif          = !std::isnan(var_map["sm_scatneighsignif"]) ? var_map["sm_scatneighsignif"] : defaultvals["m_sm_scatneighsignif"];
    m_sm_pTrel                    = !std::isnan(var_map["sm_pTrel"]) ? var_map["sm_pTrel"] : defaultvals["m_sm_pTrel"];
    m_sm_mu_d0                    = !std::isnan(var_map["sm_mu_d0"]) ? var_map["sm_mu_d0"] : defaultvals["m_sm_mu_d0"];
    m_sm_mu_z0                    = !std::isnan(var_map["sm_mu_z0"]) ? var_map["sm_mu_z0"] : defaultvals["m_sm_mu_z0"];
    m_sm_ID_qOverP                = !std::isnan(var_map["sm_ID_qOverP"]) ? var_map["sm_ID_qOverP"] : defaultvals["m_sm_ID_qOverP"];

    m_my_smt                      = !std::isnan(var_map["my_smt"]) ? var_map["my_smt"] : defaultvals["m_my_smt"];

  }


  void MyTag::SetVariableRefs(const std::vector<std::string> inputVars, TMVA::Reader* tmvaReader, unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers) {


    if (!m_useEgammaMethodMV2) {
      if(!tmvaReader) {
        ATH_MSG_WARNING("#BTAG# tmva method is chosen but tmvaReader==0!!");
        return;
      }
    }

    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      //pt and abs(eta)
      if      (inputVars.at(ivar)=="pt"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_pt       ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_pt       ); nConfgVar++; }
      else if (inputVars.at(ivar)=="abs(eta)" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_absEta   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_absEta   ); nConfgVar++; }
      //jet trk kinematics
      else if (inputVars.at(ivar)=="trk_n"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trkSum_ntrk   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trkSum_ntrk   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="trk_pt_sSum" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trkSum_sPt    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trkSum_sPt    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="trk_pt"      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trkSum_vPt    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trkSum_vPt    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="abs(trk_eta)") { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trkSum_vAbsEta) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trkSum_vAbsEta); nConfgVar++; }
      //IP2D output probabilities
      else if (inputVars.at(ivar)=="ip2_pu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2_pu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2_pu   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip2_pb"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2_pb   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2_pb   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip2_pc"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2_pc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2_pc   ); nConfgVar++; }
      //IP3D output probabilities
      else if (inputVars.at(ivar)=="ip3_pu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3_pu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3_pu   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip3_pb"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3_pb   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3_pb   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip3_pc"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3_pc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3_pc   ); nConfgVar++; }
      //SV1 output probabilities
      else if (inputVars.at(ivar)=="sv1_pu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_pu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_pu   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_pb"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_pb   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_pb   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_pc"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_pc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_pc   ); nConfgVar++; }
      //IP2D output LLR
      else if (inputVars.at(ivar)=="ip2"      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip2_c"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2_c    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2_c    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip2_cu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip2_cu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip2_cu   ); nConfgVar++; }
      //IP3D output LLR
      else if (inputVars.at(ivar)=="ip3"      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip3_c"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3_c    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3_c    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="ip3_cu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_ip3_cu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_ip3_cu   ); nConfgVar++; }
      //SV1 output LLR
      else if (inputVars.at(ivar)=="sv1"      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_c"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_c    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_c    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_cu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_cu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_cu   ); nConfgVar++; }
      //SV0 input variables
      else if (inputVars.at(ivar)=="sv0"      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv0      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv0      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv0_ntkv" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv0_ntkv ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv0_ntkv ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv0mass"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv0_mass ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv0_mass ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv0_efrc" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv0_efrc ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv0_efrc ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv0_n2t"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv0_n2t  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv0_n2t  ); nConfgVar++; }
      //SV1 input variables
      else if (inputVars.at(ivar)=="sv1_ntkv" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_ntkv ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_ntkv ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_mass" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_mass ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_mass ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_efrc" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_efrc ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_efrc ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_n2t"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_n2t  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_n2t  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_Lxy"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_Lxy  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_Lxy  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_L3d"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_L3d  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_L3d  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_sig3" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_sig3 ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_sig3 ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_dR"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_dR   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_dR   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv1_distmatlay"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv1_distmatlay   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv1_distmatlay   ); nConfgVar++; }
      //JetFitter input variables
      else if (inputVars.at(ivar)=="jf_mass"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_mass  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_mass  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_efrc"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_efrc  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_efrc  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_n2tv"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_n2tv  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_n2tv  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_ntrkv" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_ntrkv ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_ntrkv ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_nvtx"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_nvtx  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_nvtx  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_nvtx1t") { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_nvtx1t) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_nvtx1t); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_dphi"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_dphi  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_dphi  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_deta"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_deta  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_deta  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_dR"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_dR    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_dR    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_sig3"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_sig3  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_sig3  ); nConfgVar++; }
      //new jf variables
      else if (inputVars.at(ivar)=="jf_dR_flight") { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_dR_flight) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_dR_flight); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_mass_unco") { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_mass_unco) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_mass_unco); nConfgVar++; }
      //MVb input variables
      else if (inputVars.at(ivar)=="width"         ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_width           ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_width           ); nConfgVar++; }
      else if (inputVars.at(ivar)=="n_trk_sigd0cut") { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_n_trk_sigd0cut  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_n_trk_sigd0cut  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="trk3_d0sig"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trk3_d0sig      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trk3_d0sig      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="trk3_z0sig"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_trk3_z0sig      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_trk3_z0sig      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sv_scaled_efc" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sv_scaled_efc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sv_scaled_efc   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jf_scaled_efc" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jf_scaled_efc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jf_scaled_efc   ); nConfgVar++; }
      //MV2cl100 input variables
      else if (inputVars.at(ivar)== "nTrk_vtx1"                    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_nTrk_vtx1                  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_nTrk_vtx1); nConfgVar++; }
      else if (inputVars.at(ivar)== "mass_first_vtx"               ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_mass_first_vtx             ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mass_first_vtx); nConfgVar++; }
      else if (inputVars.at(ivar)== "e_first_vtx"                  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_e_first_vtx                ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_e_first_vtx); nConfgVar++; }
      else if (inputVars.at(ivar)== "e_frac_vtx1"                  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_e_frac_vtx1                ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_e_frac_vtx1); nConfgVar++; }
      else if (inputVars.at(ivar)== "closestVtx_L3D"               ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_closestVtx_L3D             ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_closestVtx_L3D); nConfgVar++; }
      else if (inputVars.at(ivar)== "JF_Lxy1"                      ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_JF_Lxy1                    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_JF_Lxy1); nConfgVar++; }
      else if (inputVars.at(ivar)== "vtx1_MaxTrkRapidity_jf_path"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_vtx1_MaxTrkRapidity_jf_path) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_vtx1_MaxTrkRapidity_jf_path); nConfgVar++; }
      else if (inputVars.at(ivar)== "vtx1_AvgTrkRapidity_jf_path"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_vtx1_AvgTrkRapidity_jf_path) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_vtx1_AvgTrkRapidity_jf_path); nConfgVar++; }
      else if (inputVars.at(ivar)== "vtx1_MinTrkRapidity_jf_path"  ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_vtx1_MinTrkRapidity_jf_path) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_vtx1_MinTrkRapidity_jf_path); nConfgVar++; }
      else if (inputVars.at(ivar)== "MaxTrkRapidity_jf_path"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_MaxTrkRapidity_jf_path     ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_MaxTrkRapidity_jf_path); nConfgVar++; }
      else if (inputVars.at(ivar)== "MinTrkRapidity_jf_path"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_MinTrkRapidity_jf_path     ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_MinTrkRapidity_jf_path); nConfgVar++; }
      else if (inputVars.at(ivar)== "AvgTrkRapidity_jf_path"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_AvgTrkRapidity_jf_path    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_AvgTrkRapidity_jf_path); nConfgVar++; }
      //soft muon input variables
      else if (inputVars.at(ivar)=="sm_mu_pt"           ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_mu_pt          ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_mu_pt          ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_dR"              ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_dR             ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_dR             ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_qOverPratio"     ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_qOverPratio    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_qOverPratio    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_mombalsignif"    ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_mombalsignif   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_mombalsignif   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_scatneighsignif" ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_scatneighsignif) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_scatneighsignif); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_pTrel"           ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_pTrel          ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_pTrel          ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_mu_d0"           ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_mu_d0          ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_mu_d0          ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_mu_z0"           ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_mu_z0          ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_mu_z0          ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sm_ID_qOverP"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_sm_ID_qOverP      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_sm_ID_qOverP      ); nConfgVar++; }
      else if (inputVars.at(ivar)=="my_smt"       ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_my_smt      ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_my_smt      ); nConfgVar++; }

      else {
        ATH_MSG_WARNING( "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in MV2... the algorithm will be 'disabled'.");//<<alias<<" "<<author);
        badVariableFound=true;
      }
    }
  } //end MyTag::SetVariableRefs

}//end namespace Analysis
