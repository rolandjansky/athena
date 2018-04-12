/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackParticle.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "JetTagTools/MV2Tag.h"

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
      @class MV2Tag
      BDT-based tagger combining more than 20 upper stream b-tagging variables
      @author K.Mochizuki, L.Vacavant, M.Ughetto
      modified May 2016 J. Shlomi
  */

  MV2Tag::MV2Tag(const std::string& name, const std::string& n, const IInterface* p):
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

    declareProperty("defaultvals", m_defaultvals );
    declareProperty("MVTMvariableNames", m_MVTM_name_tranlations );


  }


  MV2Tag::~MV2Tag() {

  }


  StatusCode MV2Tag::initialize() {

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
    m_inputPointers = new std::vector<float*>();
    return StatusCode::SUCCESS;
  }


  StatusCode MV2Tag::finalize() {
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

    for (auto& iter: m_local_inputvals) {
        delete iter.second;
    }
    m_inputPointers->clear();
    delete m_inputPointers;
    
    return StatusCode::SUCCESS;
  }

  void MV2Tag::assignProbability(xAOD::BTagging *BTag,
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
      m_inputPointers->clear();
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

  CreateLocalVariables( inputs );

  SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,*m_inputPointers);


   ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<m_inputPointers->size() );

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

  CreateLocalVariables( inputs );

  SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,*m_inputPointers);

  ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<m_inputPointers->size() );

	if ( inputVars.size()!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING( "#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
    delete bdt;
    return;
	}


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
        
	it_egammaBDT->second->SetPointers(*m_inputPointers);

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

  float MV2Tag::d0sgn_wrtJet(const TLorentzVector& jet, const TLorentzVector& trk, float d0sig) {
    const double dPhi = jet.DeltaPhi(trk);
    const float d0_sign = sin(dPhi) * d0sig;

    if (std::fabs(d0_sign) < 1e-4)
      return 1.0;

    const float res = d0_sign / std::fabs(d0_sign);
    return res;
  }

  float MV2Tag::z0sgn_wrtJet(float trackTheta, float trackZ0, float jetEta) {
    const float trackEta = -std::log(std::tan(trackTheta/2.));
    const float zs = (jetEta - trackEta)*trackZ0;
    return (zs>=0. ? 1. : -1.);
  }


void MV2Tag::CreateLocalVariables(std::map<std::string, double> var_map){



  for(std::map<std::string, double >::iterator iterator = var_map.begin(); iterator != var_map.end(); iterator++) {

      std::string MVTM_var_name = iterator->first;

      if (!(m_MVTM_name_tranlations.find(MVTM_var_name) == m_MVTM_name_tranlations.end()) ){
      // translate to calibration file naming convention
      std::string var_name = m_MVTM_name_tranlations.at(MVTM_var_name);
      m_local_inputvals[var_name] = new float;
      }
  }

}

void MV2Tag::ReplaceNaN_andAssign(std::map<std::string, double> var_map){
    //replace nan values provided by MultivariateTagManager


    for(std::map<std::string, double >::iterator iterator = var_map.begin(); iterator != var_map.end(); iterator++) {


      std::string MVTM_var_name = iterator->first;

       if (!(m_MVTM_name_tranlations.find(MVTM_var_name) == m_MVTM_name_tranlations.end()) ){
      // translate to calibration file naming convention
      std::string var_name = m_MVTM_name_tranlations.at(MVTM_var_name);
      *m_local_inputvals.at(var_name) =  !std::isnan(iterator->second) ? iterator->second : m_defaultvals.at(var_name);
      }

    }

  }


  void MV2Tag::SetVariableRefs(const std::vector<std::string> inputVars, TMVA::Reader* tmvaReader, unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers) {


    if (!m_useEgammaMethodMV2) {
      if(!tmvaReader) {
        ATH_MSG_WARNING("#BTAG# tmva method is chosen but tmvaReader==0!!");
        return;
      }
    }

    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      //pt and abs(eta)


      if ( m_local_inputvals.find(inputVars.at(ivar)) == m_local_inputvals.end() ){
        //if variable is not found
        ATH_MSG_WARNING( "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in MV2... the algorithm will be 'disabled'.");//<<alias<<" "<<author);
        badVariableFound=true;
      }else{
        if(m_useEgammaMethodMV2){
        inputPointers.push_back(m_local_inputvals.at(inputVars.at(ivar)) );
        }
        else{
        tmvaReader->AddVariable(inputVars.at(ivar).data(),m_local_inputvals.at(inputVars.at(ivar)));
        }
        nConfgVar++;
      }

    }

  } //end MV2Tag::SetVariableRefs


}//end namespace Analysis
