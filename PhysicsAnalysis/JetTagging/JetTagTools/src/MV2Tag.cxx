/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/MV2Tag.h"

#include "GaudiKernel/IToolSvc.h"

#include "xAODJet/Jet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "JetTagTools/JetTagUtils.h"
#include "JetTagCalibration/CalibrationBroker.h"
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
  */

  MV2Tag::MV2Tag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_calibrationTool("BTagCalibrationBroker"),
      m_runModus("analysis") {
    declareInterface<ITagTool>(this);
    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);
    // force MV2 to always use a calibration derived from MV2CalibAlias jet collection
    declareProperty("forceMV2CalibrationAlias", m_forceMV2CalibrationAlias = true);
    declareProperty("MV2CalibAlias", m_MV2CalibAlias = "AntiKt4TopoEM");
    // global configuration:
    declareProperty("Runmodus", m_runModus);
    declareProperty("DecorateMvaInputs", m_decorateBTaggingObj=false);
    // to change input weights:
    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
    declareProperty("inputIP2DSourceName", m_ip2d_infosource = "IP2D");
    declareProperty("inputIP3DSourceName", m_ip3d_infosource = "IP3D");
    declareProperty("inputJFSourceName", m_jftNN_infosource = "JetFitter");
    declareProperty("inputJFProbSourceName", m_jfprob_infosource = "JetFitterCombNN");
    declareProperty("inputSoftMuonSourceName", m_softmuon_infosource = "SMT");
    declareProperty("xAODBaseName",      m_xAODBaseName);//"MV2c20" or etc.
    // which calibration folder to use 
    declareProperty("taggerNameBase", m_taggerNameBase = "MV2");
    declareProperty("taggerName", m_taggerName = "MV2");
    declareProperty("decTagName", m_decTagName = "MV2_inputs");
    
    declareProperty("trainingConfig", m_trainingConfig = "Default");//unused flag, now only to keep backward compatibility

    /// and this what was used before the flip (comfigure from python now)
      // m_ip2d_infosource  = "IP2DNeg";
      // m_ip3d_infosource  = "IP3DNeg";
      // m_sv1_infosource   = "SV1Flip";
      // m_sv0p_infosource  = "SV0InfoPlus";
      // m_jftNN_infosource = "JetFitterTagNNFlip";
      // m_jfcNN_infosource = "JetFitterCOMBNNIP3DNeg";

    
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
    return StatusCode::SUCCESS;
  }

  StatusCode MV2Tag::tagJet(xAOD::Jet& myJet, xAOD::BTagging* BTag) {
    /*
     * #index for this function
     * #1: Preparation of MVA instance using tmva/egammaBDT
     * #2: Set necessary input variables
     * #3: Calcuation of MVA output variable(s)
     * #4: Fill MVA output variable(s) into xAOD
     */

    // #0: Check if disabled or not
    if ( m_disableAlgo ) {
      if (m_warnCounter<20) {
	ATH_MSG_WARNING("#BTAG# .... algorithm was disabled due to mis-configuration problems: no output will be written ... PLEASE WATCH OUT" );
	m_warnCounter++;
      }
      return StatusCode::SUCCESS;
    }
    
    // #1: Preparation of MVA instance using tmva/egammaBDT    
    /* jet author: */
    std::string author ;
    if (m_forceMV2CalibrationAlias) {
      author = m_MV2CalibAlias;
    }
    else {
      author = JetTagUtils::getJetAuthor(myJet);
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
	return StatusCode::SUCCESS;
      }
      else {
	const TString rootClassName=calib.first->ClassName();
	if      (rootClassName=="TDirectoryFile") m_useEgammaMethodMV2=true;
	else if (rootClassName=="TList")          m_useEgammaMethodMV2=false;//tmva method
	else {
	  ATH_MSG_WARNING("#BTAG# Unsupported ROOT class type: "<<rootClassName<<" is retrieved. Disabling algorithm..");
	  m_disableAlgo=true;
	  return StatusCode::SUCCESS;
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
	  //std::cout<<tmp<<std::endl;
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
	return StatusCode::SUCCESS;	
      }

      ATH_MSG_VERBOSE("#BTAG# MV2 m_useEgammaMethodMV2= "<<m_useEgammaMethodMV2 );

      if (!m_useEgammaMethodMV2) {
	// now configure the TMVAReaders:
	/// check if the reader for this tagger needs update
	tmvaReader = new TMVA::Reader();

	SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);
	ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( inputVars.size()!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING("#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
	  return StatusCode::SUCCESS;	
	}

	//tmvaReader->BookMVA("BDT", xmlFileName);
	TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kBDT, str.data() );
	kl = dynamic_cast<TMVA::MethodBase*>(method);

	if (m_writeRootFile) {//KM: just for developpers
	  const std::string filename= m_taggerNameBase+"_"+author+".root";
	  TObjArray trees, variables, classes;
	  TString *vars = egammaMVACalib::getVariables(tmvaReader);  assert(vars);
	  TMVA::MethodBDT* tbdt= dynamic_cast<TMVA::MethodBDT*>(method);  assert(tbdt);
	  bdt= new MVAUtils::BDT(tbdt);
	  TFile *f = new TFile(filename.data(),"recreate");
	  TTree *tree = bdt->WriteTree(m_treeName.data());
	  variables.AddAtAndExpand(new TObjString(*vars),0);
	  trees.AddAtAndExpand(tree,0);
	  ATH_MSG_INFO("#BTAG# Writing down TTree: "<<tree->GetName()<<", variables: "<<m_varStrName<<" in "<<filename);
	  f->mkdir((m_taggerNameBase+"/"+author+"/"+m_taggerNameBase+"Calib").data());
	  f->cd   ((m_taggerNameBase+"/"+author+"/"+m_taggerNameBase+"Calib").data());
	  const int option = (TObject::kSingleKey | TObject::kOverwrite);
	  trees.Write();
	  variables.Write(m_varStrName.data(),option);
	  f->Close();
	  delete bdt;
	}
      
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
	  delete bdt;
	  return StatusCode::SUCCESS;
	}
	
	SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);
	ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( inputVars.size()!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING( "#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
	  delete bdt;
	  return StatusCode::SUCCESS;
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

    // #2: Set necessary input variables
    //KM: Preparation of MVA instance over here, below should be put in some function like setInputVariables(myJet,BTag);
    //////////////////////////////////
    // Start MV2 inputs retrieving  //
    //////////////////////////////////

    m_pt     = myJet.pt();
    m_absEta = fabs(myJet.eta());

    m_trkSum_ntrk   = BTag->isAvailable<unsigned>("trkSum_ntrk") ? BTag->auxdata<unsigned>("trkSum_ntrk") : -1;
    m_trkSum_sPt    = BTag->isAvailable<float   >("trkSum_SPt" ) ? BTag->auxdata<float   >("trkSum_SPt" ) : -1000;//<== -1 GeV
    m_trkSum_vPt    = 0;
    m_trkSum_vAbsEta= -10;
    if (m_trkSum_ntrk>0) {
      m_trkSum_vPt    = BTag->isAvailable<float>("trkSum_VPt" ) ?      BTag->auxdata<float>("trkSum_VPt" )  : -1000;
      m_trkSum_vAbsEta= BTag->isAvailable<float>("trkSum_VEta") ? fabs(BTag->auxdata<float>("trkSum_VEta")) : -10  ;
    }
    
    ClearInputs();// VD: make sure that we initialize all variables  
    // these variable are not alwasy defined -> make sure to use always the same default when training
    // can rely on xAOD default at some point but not necessary the best way in case this is not set explicitly or set to a value which is desired in BDTs

    TVector3 v3_jet(myJet.px(),myJet.py(),myJet.pz());

    ///// TMVA does not accept double or int -> use doubles to get xAOD-double info and copy to float
    double ip2_pb=-1, ip2_pc=-1, ip2_pu=-1;
    double ip3_pb=-1, ip3_pc=-1, ip3_pu=-1;
    double sv1_pb=-1, sv1_pc=-1, sv1_pu=-1;
    double jfc_pb=-1, jfc_pc=-1, jfc_pu=-1;

    bool status = true;

    /*** Accessing IP2D variables ***/
    std::vector<float> weightBofTracksIP2D;
    BTag->variable<std::vector<float> >(m_ip2d_infosource, "weightBofTracks", weightBofTracksIP2D);
    int ntrk_ip2= weightBofTracksIP2D.size();
    if(ntrk_ip2>0) {
      if("IP2D"==m_ip2d_infosource){
	ip2_pb=BTag->IP2D_pb();
	ip2_pu=BTag->IP2D_pu();
	ip2_pc=BTag->IP2D_pc();
      }
      else{
	status &= BTag->variable<double>(m_ip2d_infosource, "pu", ip2_pu);
	status &= BTag->variable<double>(m_ip2d_infosource, "pb", ip2_pb);
	status &= BTag->variable<double>(m_ip2d_infosource, "pc", ip2_pc);
      }
      m_ip2   =BTag->calcLLR(ip2_pb,ip2_pu);
      m_ip2_c =BTag->calcLLR(ip2_pb,ip2_pc);
      m_ip2_cu=BTag->calcLLR(ip2_pc,ip2_pu);
    }

    /*** Accessing IP3D variables ***/
    std::vector<float> weightBofTracksIP3D;
    BTag->variable<std::vector<float> >(m_ip3d_infosource, "weightBofTracks", weightBofTracksIP3D);
    int ntrk_ip3= weightBofTracksIP3D.size();
    if(ntrk_ip3>0) {
      if("IP3D"==m_ip3d_infosource){
	ip3_pb=BTag->IP3D_pb();
	ip3_pu=BTag->IP3D_pu();
	ip3_pc=BTag->IP3D_pc();
      } 
      else {
	status &= BTag->variable<double>(m_ip3d_infosource, "pu", ip3_pu);
	status &= BTag->variable<double>(m_ip3d_infosource, "pb", ip3_pb);
	status &= BTag->variable<double>(m_ip3d_infosource, "pc", ip3_pc);
      }
      m_ip3   =BTag->calcLLR(ip3_pb,ip3_pu);
      m_ip3_c =BTag->calcLLR(ip3_pb,ip3_pc);
      m_ip3_cu=BTag->calcLLR(ip3_pc,ip3_pu);
      /// compute the new variable here
    }

    /*** Accessing SV1 output variables ***/
    if("SV1"==m_sv1_infosource){
      sv1_pb=BTag->SV1_pb();
      sv1_pu=BTag->SV1_pu();
      sv1_pc=BTag->SV1_pc();
    } 
    else {
      BTag->variable<double>(m_sv1_infosource, "pu", sv1_pu);
      BTag->variable<double>(m_sv1_infosource, "pb", sv1_pb);
      BTag->variable<double>(m_sv1_infosource, "pc", sv1_pc);
    }
    m_sv1    = BTag->calcLLR(sv1_pb,sv1_pu);
    m_sv1_c  = BTag->calcLLR(sv1_pb,sv1_pc);
    m_sv1_cu = BTag->calcLLR(sv1_pc,sv1_pu);

    /*** Accessing JetFitter output variables ***/
    // if("JetFitterCombNN"==m_jfprob_infosource){
    //   jfc_pb=BTag->JetFitterCombNN_pb();
    //   jfc_pu=BTag->JetFitterCombNN_pu();
    //   jfc_pc=BTag->JetFitterCombNN_pc();
    // }
    // else if ("JetFitter"==m_jfprob_infosource){
    //   jfc_pb=BTag->JetFitter_pb();
    //   jfc_pu=BTag->JetFitter_pu();
    //   jfc_pc=BTag->JetFitter_pc();
    // } 
    // else {
    //   BTag->variable<double>(m_jfprob_infosource, "pu", jfc_pu);
    //   BTag->variable<double>(m_jfprob_infosource, "pb", jfc_pb);
    //   BTag->variable<double>(m_jfprob_infosource, "pc", jfc_pc);
    // }
    
    /*** Accessing SV0 variables ***/    
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    // don't check the following status
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv0_infosource, "vertices", myVertices);

    if(myVertices.size()>0 && myVertices[0].isValid()){//    if(sv0OK){
      int sv0_n2t= -1, sv0_ntkv= -1;
      if ("SV0" == m_sv0_infosource){
	BTag->taggerInfo(m_sv0_mass, xAOD::BTagInfo::SV0_masssvx);
	BTag->taggerInfo(m_sv0_efrc, xAOD::BTagInfo::SV0_efracsvx);
	BTag->taggerInfo(sv0_n2t, xAOD::BTagInfo::SV0_N2Tpair);
	BTag->taggerInfo(sv0_ntkv, xAOD::BTagInfo::SV0_NGTinSvx);
	BTag->taggerInfo(m_sv0, xAOD::BTagInfo::SV0_normdist);
      }
      else{
	double sv0=-1;
	BTag->variable<float>(m_sv0_infosource, "masssvx", m_sv0_mass);
	BTag->variable<float>(m_sv0_infosource, "efracsvx", m_sv0_efrc);
	BTag->variable<int>(m_sv0_infosource, "N2Tpair", sv0_n2t);
	BTag->variable<int>(m_sv0_infosource, "NGTinSvx", sv0_ntkv);
	BTag->variable<double>(m_sv0_infosource, "significance3D", sv0);
	m_sv0 = sv0;
      }
      m_sv0_n2t   = sv0_n2t;
      m_sv0_ntkv  = sv0_ntkv;
      if ( m_trainingConfig=="Default" ) m_sv0_mass/=GeV;
    }

    /*** Accessing SV1 variables ***/        
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices1;
    // don't check the following status
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices1);
    if(myVertices1.size()>0 && myVertices1[0].isValid()) {//    if(sv1OK){
      int sv1_n2t= -1, sv1_ntkv= -1;
      if ("SV1" == m_sv1_infosource){
	status &= BTag->taggerInfo(m_sv1_mass, xAOD::BTagInfo::SV1_masssvx);
	status &= BTag->taggerInfo(m_sv1_efrc, xAOD::BTagInfo::SV1_efracsvx);
	status &= BTag->taggerInfo(sv1_n2t   , xAOD::BTagInfo::SV1_N2Tpair);
	status &= BTag->taggerInfo(sv1_ntkv  , xAOD::BTagInfo::SV1_NGTinSvx);
	status &= BTag->taggerInfo(m_sv1_sig3, xAOD::BTagInfo::SV1_normdist);
      }
      else{
	status &= BTag->variable<float>(m_sv1_infosource, "masssvx" , m_sv1_mass);
	status &= BTag->variable<float>(m_sv1_infosource, "efracsvx", m_sv1_efrc);
	status &= BTag->variable<int>(m_sv1_infosource  , "N2Tpair" , sv1_n2t);
	status &= BTag->variable<int>(m_sv1_infosource  , "NGTinSvx", sv1_ntkv);
	status &= BTag->variable<float>(m_sv1_infosource, "normdist" , m_sv1_sig3);
      }
      status &= BTag->variable<float>(m_sv1_infosource, "dstToMatLay" , m_sv1_distmatlay);
      status &= BTag->variable<float>(m_sv1_infosource, "deltaR"      , m_sv1_dR );
      status &= BTag->variable<float>(m_sv1_infosource, "Lxy"         , m_sv1_Lxy);
      status &= BTag->variable<float>(m_sv1_infosource, "L3d"         , m_sv1_L3d);
      m_sv1_n2t   = sv1_n2t;
      m_sv1_ntkv  = sv1_ntkv;
    }

    /*** Accessing JetFitter variables ***/        
    bool jfitok=false;
    std::vector< ElementLink< xAOD::BTagVertexContainer > > myVerticesJF;
    // don't check the following status
    BTag->variable<std::vector<ElementLink<xAOD::BTagVertexContainer> > >(m_jftNN_infosource, "JFvertices", myVerticesJF);
    if (myVerticesJF.size()>0 and myVerticesJF[0].isValid()) {      jfitok=true;    }
    int jf_nvtx_tmp=0;   m_jftNN_infosource=="JetFitter"? BTag->taggerInfo(jf_nvtx_tmp  , xAOD::BTagInfo::JetFitter_nVTX)         : BTag->variable<int>(m_jftNN_infosource, "nVTX"         , jf_nvtx_tmp  );
    int jf_nvtx1t_tmp=0; m_jftNN_infosource=="JetFitter"? BTag->taggerInfo(jf_nvtx1t_tmp, xAOD::BTagInfo::JetFitter_nSingleTracks): BTag->variable<int>(m_jftNN_infosource, "nSingleTracks", jf_nvtx1t_tmp);
    jfitok &= jf_nvtx_tmp + jf_nvtx1t_tmp > 0;
    
    if(jfitok){
      int jf_nvtx= -1, jf_nvtx1t= -1, jf_ntrkv= -1, jf_n2tv= -1;
      if("JetFitter" == m_jftNN_infosource){
	status &= BTag->taggerInfo(jf_nvtx, xAOD::BTagInfo::JetFitter_nVTX);
	status &= BTag->taggerInfo(jf_nvtx1t, xAOD::BTagInfo::JetFitter_nSingleTracks);
	status &= BTag->taggerInfo(jf_ntrkv, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
	status &= BTag->taggerInfo(m_jf_efrc, xAOD::BTagInfo::JetFitter_energyFraction);
	status &= BTag->taggerInfo(m_jf_mass, xAOD::BTagInfo::JetFitter_mass);
	status &= BTag->taggerInfo(m_jf_sig3, xAOD::BTagInfo::JetFitter_significance3d);
	status &= BTag->taggerInfo(m_jf_dphi, xAOD::BTagInfo::JetFitter_deltaphi);
	status &= BTag->taggerInfo(m_jf_deta, xAOD::BTagInfo::JetFitter_deltaeta);
	// don't check the following status
	BTag->taggerInfo(jf_n2tv, xAOD::BTagInfo::JetFitter_N2Tpair);
      }
      else{
	status &= BTag->variable<int>(m_jftNN_infosource, "nVTX", jf_nvtx);
	status &= BTag->variable<int>(m_jftNN_infosource, "nSingleTracks", jf_nvtx1t);
	status &= BTag->variable<int>(m_jftNN_infosource, "nTracksAtVtx", jf_ntrkv);
	status &= BTag->variable<float>(m_jftNN_infosource, "energyFraction", m_jf_efrc);
	status &= BTag->variable<float>(m_jftNN_infosource, "mass", m_jf_mass);
	status &= BTag->variable<float>(m_jftNN_infosource, "significance3d", m_jf_sig3);
	status &= BTag->variable<float>(m_jftNN_infosource, "deltaphi", m_jf_dphi);
	status &= BTag->variable<float>(m_jftNN_infosource, "deltaeta", m_jf_deta);
	// don't check the following status
	BTag->variable<int>(m_jftNN_infosource, "N2Tpair", jf_n2tv);
      }
      m_jf_nvtx   = jf_nvtx;
      m_jf_nvtx1t = jf_nvtx1t;
      m_jf_ntrkv  = jf_ntrkv;
      m_jf_n2tv   = jf_n2tv;
      m_jf_dR     = m_jf_dphi==-10 and m_jf_deta==-10 ? -1 : hypot(m_jf_dphi,m_jf_deta);
      // new jf variables
      status &= BTag->variable<float>(m_jftNN_infosource, "massUncorr" , m_jf_mass_unco);
      status &= BTag->variable<float>(m_jftNN_infosource, "dRFlightDir", m_jf_dR_flight);
    }

    /*** Generating MVb variables ***/
    std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks; //= BTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_trackAssociationName);
    bool trksOK=BTag->variable<std::vector<ElementLink<xAOD::TrackParticleContainer> > > (m_ip3d_infosource, "TrackParticleLinks", associationLinks );

    std::vector<float> vectD0, vectD0Signi, vectZ0, vectZ0Signi;    vectD0.clear(), vectD0Signi.clear(), vectZ0.clear(), vectZ0Signi.clear();
    trksOK &= BTag->variable< std::vector<float> > (m_ip3d_infosource, "valD0wrtPVofTracks", vectD0     );
    trksOK &= BTag->variable< std::vector<float> > (m_ip3d_infosource, "sigD0wrtPVofTracks", vectD0Signi);
    trksOK &= BTag->variable< std::vector<float> > (m_ip3d_infosource, "valZ0wrtPVofTracks", vectZ0     );
    trksOK &= BTag->variable< std::vector<float> > (m_ip3d_infosource, "sigZ0wrtPVofTracks", vectZ0Signi);
    if (vectD0.size() and vectD0Signi.size() and vectZ0.size() and vectZ0Signi.size()) {
      trksOK &= associationLinks.size() == vectD0.size();
      trksOK &= associationLinks.size() == vectZ0.size();
      trksOK &= associationLinks.size() == vectD0Signi.size();
      trksOK &= associationLinks.size() == vectZ0Signi.size();
    }
    //std::cout<<"debug: "<<associationLinks.size()<<" "<<vectD0.size()<<" "<<vectZ0.size()<<" "<<vectD0Signi.size()<<" "<<vectZ0Signi.size()<<std::endl;

    int ntrks = associationLinks.size();
    int n_trk_d0cut = 0;
    if (trksOK) {
      ATH_MSG_VERBOSE("#BTAG# MV2: calculating MVb inputs.");

      float sum_pt = 0., sum_pt_dr = 0.;

      std::vector<std::pair<float, float> > trk_d0_z0;
      trk_d0_z0.reserve(associationLinks.size());

      unsigned trkIndex=0;
      for(auto trkIter = associationLinks.begin(); trkIter != associationLinks.end(); ++trkIter) {
        const xAOD::TrackParticle* aTemp = **trkIter;
        TLorentzVector trk;
        trk.SetPtEtaPhiM(aTemp->pt(), aTemp->eta(), aTemp->phi(), 0.);

        // no need for a dedicated selection here, the tracks are already
        // selected by the IP3D algorithm
        const float d0sig = vectD0Signi.at(trkIndex);
        const float z0sig = vectZ0Signi.at(trkIndex);
        trkIndex++;

        if (std::fabs(d0sig) > 1.8)
          n_trk_d0cut++;

        // track width components
        sum_pt += trk.Pt();
        const float dRtoJet = trk.DeltaR(myJet.p4());
        sum_pt_dr += dRtoJet * trk.Pt();

        // for 3rd higest d0/z0 significance
        trk_d0_z0.push_back(std::make_pair(d0sig, z0sig));
      } //end of trk loop
    
      // sort by highest signed d0 sig
      std::sort(trk_d0_z0.begin(), trk_d0_z0.end(), [](const std::pair<float, float>& a, const std::pair<float, float>& b) { 
        return a.first > b.first; 
      } );

      //Assign MVb variables
      m_width          = sum_pt > 0 ? sum_pt_dr / sum_pt : 0;
      m_n_trk_sigd0cut = n_trk_d0cut;
      m_trk3_d0sig     = trk_d0_z0.size() > 2 ? trk_d0_z0[2].first : -100;
      m_trk3_z0sig     = trk_d0_z0.size() > 2 ? trk_d0_z0[2].second : -100;
      m_sv_scaled_efc  = m_sv1_ntkv>0               ? m_sv1_efrc * (static_cast<float>(ntrks) / m_sv1_ntkv)                : -1;
      m_jf_scaled_efc  = m_jf_ntrkv + m_jf_nvtx1t>0 ? m_jf_efrc * (static_cast<float>(ntrks) / (m_jf_ntrkv + m_jf_nvtx1t)) : -1;
    }

    /*** Generating MV2cl100 variables *****/

    //MV2cl100 default values
    m_nTrk_vtx1 = 0;
    m_mass_first_vtx = -100;
    m_e_first_vtx = -100;
    m_e_frac_vtx1 = 0;
    m_closestVtx_L3D = -10;
    m_JF_Lxy1 = -5;
    m_vtx1_MaxTrkRapidity_jf_path = 0;
    m_vtx1_AvgTrkRapidity_jf_path = 0;
    m_vtx1_MinTrkRapidity_jf_path = 0;
    m_MaxTrkRapidity_jf_path = 0;
    m_MinTrkRapidity_jf_path = 0;
    m_AvgTrkRapidity_jf_path = 0;   

    std::vector< ElementLink< xAOD::TrackParticleContainer > > assocTracks;
    try{
      assocTracks = BTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >("BTagTrackToJetAssociator");
    } catch (...) {}
    
    std::vector<ElementLink<xAOD::BTagVertexContainer> > jfvertices;
    try {
      jfvertices =  BTag->auxdata<std::vector<ElementLink<xAOD::BTagVertexContainer> > >("JetFitter_JFvertices");
    } catch (...) {}

    std::vector<float> fittedPosition = BTag->auxdata<std::vector<float> >("JetFitter_fittedPosition");
    
    if(fittedPosition.size() > 0){
      
      float jf_phi = fittedPosition[3];
      float jf_theta = fittedPosition[4];
      TVector3 flightDir(0,0,0);
      flightDir.SetMagThetaPhi(1., jf_theta, jf_phi ); //flight directon of JF decay chain

      //loop over position of JF vertices, find index of secondary vertex
      int secondary_vertex_index = -1;
      for (unsigned int jfv = 0; jfv < jfvertices.size(); jfv++) {
        float tmpL3D = fittedPosition[jfv + 5];
        if (tmpL3D > 0 && (m_closestVtx_L3D > tmpL3D || m_closestVtx_L3D < 0) ){
          m_closestVtx_L3D = tmpL3D;
          secondary_vertex_index = jfv;
        }
      }

      //loop over tracks, collect total 4 momentum, and 4 momentum of secondary vertex, calculate pseudo rapidity of track   
      TLorentzVector tracksTot4Mom(0,0,0,0);
      TLorentzVector tracksTot4Mom_firstVtx(0,0,0,0);
      float sumTrackRapidity = 0;
      float vtx1_sumTrackRapidity = 0;
      int vtx1_first_track = 0;
      float track_mass = 139.570;
      int trkIndex=0;



        //track loop, (slightly reused from MVb section)
        
        for(auto trkIter = assocTracks.begin(); trkIter != assocTracks.end(); ++trkIter) {

          const xAOD::TrackParticle* aTemp = **trkIter;
         
          TLorentzVector trk;
          trk.SetPtEtaPhiM(aTemp->pt(), aTemp->eta(), aTemp->phi(), track_mass);
          tracksTot4Mom += trk;

          TVector3 trkvector(0,0,0);
          trkvector = trk.Vect();
          
          float trackRapidity = (-1)*log( tan( 0.5*trkvector.Angle(flightDir) ) );
          
          sumTrackRapidity += trackRapidity;

          if(trkIndex==0){
            m_MaxTrkRapidity_jf_path = trackRapidity;
            m_MinTrkRapidity_jf_path = trackRapidity;
          }else{
            m_MaxTrkRapidity_jf_path = trackRapidity > m_MaxTrkRapidity_jf_path ? trackRapidity : m_MaxTrkRapidity_jf_path;
            m_MinTrkRapidity_jf_path = trackRapidity < m_MinTrkRapidity_jf_path ? trackRapidity : m_MinTrkRapidity_jf_path;
          }
          
            if(secondary_vertex_index >= 0){
              //get track links to secondary vertex
                const xAOD::BTagVertex *tmpVertex = *(jfvertices.at(secondary_vertex_index));
                const std::vector< ElementLink<xAOD::TrackParticleContainer> > tmpVect = tmpVertex->track_links();
              //check association to JF vertex
              int particleInCollection = 0;
              for (unsigned int iT = 0; iT < tmpVect.size(); iT++) {
                      if (aTemp == *(tmpVect.at(iT))) particleInCollection=1;
              }
              if (particleInCollection){
                m_nTrk_vtx1 += 1;
                
                tracksTot4Mom_firstVtx += trk;
                vtx1_sumTrackRapidity += trackRapidity;
                
                if(vtx1_first_track==0){
                  m_vtx1_MaxTrkRapidity_jf_path = trackRapidity;
                  m_vtx1_MinTrkRapidity_jf_path = trackRapidity;
                  vtx1_first_track=1;
                }else{
                  m_vtx1_MaxTrkRapidity_jf_path = trackRapidity > m_vtx1_MaxTrkRapidity_jf_path ? trackRapidity : m_vtx1_MaxTrkRapidity_jf_path;
                  m_vtx1_MinTrkRapidity_jf_path = trackRapidity < m_vtx1_MinTrkRapidity_jf_path ? trackRapidity : m_vtx1_MinTrkRapidity_jf_path;               
                }

              }
            }

          trkIndex++;
          } //end of trk loop

    // assign the remaining MV2cl100 variables 
      m_AvgTrkRapidity_jf_path = trkIndex > 0 ? sumTrackRapidity/trkIndex : 0;

      if(m_nTrk_vtx1 > 0){
        m_JF_Lxy1 = m_closestVtx_L3D*sin(jf_theta);
        m_mass_first_vtx = tracksTot4Mom_firstVtx.M();
        m_e_first_vtx = tracksTot4Mom_firstVtx.E();
        m_e_frac_vtx1 = m_e_first_vtx/tracksTot4Mom.E();
        m_vtx1_AvgTrkRapidity_jf_path = vtx1_sumTrackRapidity/m_nTrk_vtx1;
      }
    } // end if fittedPosition.size() > 0

  
    /*** Retrieving soft muon variables ***/
    BTag->variable<float>(m_softmuon_infosource, "mu_pt"           , m_sm_mu_pt          );
    BTag->variable<float>(m_softmuon_infosource, "dR"              , m_sm_dR             );
    BTag->variable<float>(m_softmuon_infosource, "qOverPratio"     , m_sm_qOverPratio    );
    BTag->variable<float>(m_softmuon_infosource, "mombalsignif"    , m_sm_mombalsignif   );
    BTag->variable<float>(m_softmuon_infosource, "scatneighsignif" , m_sm_scatneighsignif);
    BTag->variable<float>(m_softmuon_infosource, "pTrel"           , m_sm_pTrel          );
    BTag->variable<float>(m_softmuon_infosource, "mu_d0"           , m_sm_mu_d0          );
    BTag->variable<float>(m_softmuon_infosource, "mu_z0"           , m_sm_mu_z0          );
    BTag->variable<float>(m_softmuon_infosource, "ID_qOverP"       , m_sm_ID_qOverP      );

    //add additional variables to b-tagging object
    if(m_decorateBTaggingObj) {
      ATH_MSG_VERBOSE("#BTAG# decorating btagging object with MV2 inputs.");
      //MV2 inputs
      BTag->setVariable<float>(m_decTagName, "sv1_Lxy" , m_sv1_Lxy);//m_xAODBaseName?
      BTag->setVariable<float>(m_decTagName, "sv1_L3d" , m_sv1_L3d);
      BTag->setVariable<float>(m_decTagName, "sv1_dR"  , m_sv1_dR );
      BTag->setVariable<float>(m_decTagName, "jf_dR"   , m_jf_dR  );
      //MVb inputs
      BTag->setVariable<float>(m_decTagName, "width"         , m_width         );
      BTag->setVariable<float>(m_decTagName, "sv_scaled_efc" , m_sv_scaled_efc );
      BTag->setVariable<float>(m_decTagName, "jf_scaled_efc" , m_jf_scaled_efc );
      BTag->setVariable<float>(m_decTagName, "trk3_d0sig"    , m_trk3_d0sig    );
      BTag->setVariable<float>(m_decTagName, "trk3_z0sig"    , m_trk3_z0sig    );
      BTag->setVariable<int>  (m_decTagName, "n_trk_sigd0cut",   n_trk_d0cut   );
      //MV2cl100 inputs
      BTag->setVariable<float>(m_decTagName, "nTrk_vtx1", m_nTrk_vtx1);
      BTag->setVariable<float>(m_decTagName, "mass_first_vtx" , m_mass_first_vtx );
      BTag->setVariable<float>(m_decTagName, "e_first_vtx" , m_e_first_vtx );
      BTag->setVariable<float>(m_decTagName, "e_frac_vtx1"    , m_e_frac_vtx1    );
      BTag->setVariable<float>(m_decTagName, "closestVtx_L3D"    , m_closestVtx_L3D    );
      BTag->setVariable<int>  (m_decTagName, "JF_Lxy1",   m_JF_Lxy1   );
      BTag->setVariable<int>  (m_decTagName, "vtx1_MaxTrkRapidity_jf_path",   m_vtx1_MaxTrkRapidity_jf_path   );
      BTag->setVariable<int>  (m_decTagName, "vtx1_AvgTrkRapidity_jf_path",   m_vtx1_AvgTrkRapidity_jf_path   );
      BTag->setVariable<int>  (m_decTagName, "vtx1_MinTrkRapidity_jf_path",   m_vtx1_MinTrkRapidity_jf_path   );
      BTag->setVariable<int>  (m_decTagName, "MaxTrkRapidity_jf_path",   m_MaxTrkRapidity_jf_path   );
      BTag->setVariable<int>  (m_decTagName, "MinTrkRapidity_jf_path",   m_MinTrkRapidity_jf_path   );
      BTag->setVariable<int>  (m_decTagName, "AvgTrkRapidity_jf_path",   m_AvgTrkRapidity_jf_path   );

    }

    //KM: These aren't used anymore though, just in case...
    m_ip2_pb = ip2_pb;    m_ip2_pc = ip2_pc;    m_ip2_pu = ip2_pu;
    m_ip3_pb = ip3_pb;    m_ip3_pc = ip3_pc;    m_ip3_pu = ip3_pu;
    m_sv1_pb = sv1_pb;    m_sv1_pc = sv1_pc;    m_sv1_pu = sv1_pu;
    m_jfc_pb = jfc_pb;    m_jfc_pc = jfc_pc;    m_jfc_pu = jfc_pu;

    //////////////////////////////////
    // End of MV2 inputs retrieving //
    //////////////////////////////////
    PrintInputs();
    
    if (!status) {
      ATH_MSG_WARNING("#BTAG# Missing input data: cannot compute desired results. Assigning default light values.");

      double defaultB=1e-10;
      double defaultC=1e-10;
      double defaultL=1.-defaultB-defaultC;

      if(m_runModus=="analysis") {
        if (m_taggerNameBase.find("MV2c")!=std::string::npos) BTag->setVariable<double>(m_xAODBaseName, "discriminant", -1.);
        else {
          BTag->setVariable<double>(m_xAODBaseName, "pb", defaultB);
          BTag->setVariable<double>(m_xAODBaseName, "pu", defaultL);
          BTag->setVariable<double>(m_xAODBaseName, "pc", defaultC);
        }
      }

      return StatusCode::SUCCESS;
    }

    // #3: Calcuation of MVA output variable(s)
    /* compute MV2: */
    double mv2 = -10.;  double mv2m_pb=-10., mv2m_pu=-10., mv2m_pc=-10.;
    if (!m_useEgammaMethodMV2) {
      pos = m_tmvaReaders.find(alias);
      if(pos==m_tmvaReaders.end()) {
	int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
	if(0==alreadyWarned) {
	  ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection alias, author: "<<alias<<" "<<author);
	  m_undefinedReaders.push_back(alias);
	}
      }
      else {
	it_mb = m_tmvaMethod.find(alias);
	if((it_mb->second)!=0) {
	  if (m_taggerNameBase.find("MV2c")!=std::string::npos) mv2 = pos->second->EvaluateMVA( it_mb->second );//this gives back double
	  else {
	    std::vector<float> outputs= pos->second->EvaluateMulticlass( it_mb->second );//this gives back float
	    if (outputs.size()==m_nClasses) {
	      mv2m_pb=outputs[0]; mv2m_pu=outputs[1]; mv2m_pc=outputs[2] ;
	    }
	    else ATH_MSG_WARNING("#BTAG# Unkown error, outputs vector size not "<<m_nClasses<<"!!!" );
	  }
	}
	else ATH_MSG_WARNING("#BTAG#  kl==0 for alias, author: "<<alias<<" "<<author);
      }
    }
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
	  else {
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
      if (m_taggerNameBase.find("MV2c")!=std::string::npos) BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv2);
      else {
	BTag->setVariable<double>(m_xAODBaseName, "pb", mv2m_pb);
	BTag->setVariable<double>(m_xAODBaseName, "pu", mv2m_pu);
	BTag->setVariable<double>(m_xAODBaseName, "pc", mv2m_pc);
      }
    }


    return StatusCode::SUCCESS;
  }
  
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

  void MV2Tag::ClearInputs() {
     m_ip2=-30;
     m_ip2_c=-30;
     m_ip2_cu=-30;
     m_ip3=-30;
     m_ip3_c=-30;
     m_ip3_cu=-30;
     m_ip3_2hitGradeFrac=-0.1;
     m_sv1=-10;
     m_sv1_c=-10;
     m_sv1_cu=-10;
     m_sv1_ntkv=-1; 
     m_sv1_mass=-1000; 
     m_sv1_efrc=-1; 
     m_sv1_n2t=-1; 
     m_sv1_Lxy=-100;
     m_sv1_L3d=-100;
     m_sv1_sig3=-100;
     m_sv1_dR=-1;
     m_sv1_distmatlay=-1;
     m_jf_dR=-1;
     m_jf_dR_flight=-1;
     m_jf_mass_unco=-1000;
     m_ip2_pu=-1; 
     m_ip2_pb=-1; 
     m_ip2_pc=-1; 
     m_ip3_pu=-1; 
     m_ip3_pb=-1; 
     m_ip3_pc=-1; 
     m_sv1_pu=-1; 
     m_sv1_pb=-1; 
     m_sv1_pc=-1; 
     m_jfc_pu=-1; 
     m_jfc_pb=-1; 
     m_jfc_pc=-1; 
     m_sv0=-1; 
     m_sv0_ntkv=-1; 
     m_sv0_mass=-1; 
     m_sv0_efrc=-1; 
     m_sv0_n2t=-1; 
     //m_sv0_radius=-1; 
     m_jf_mass=-1000; 
     m_jf_efrc=-1; 
     m_jf_n2tv=-1; 
     m_jf_ntrkv=-1; 
     m_jf_nvtx=-1; 
     m_jf_nvtx1t=-1; 
     m_jf_dphi=-11; 
     m_jf_deta=-11; 
     m_jf_sig3=-100;
     m_width=-1;
     m_n_trk_sigd0cut=-1;
     m_trk3_d0sig=-10;
     m_trk3_z0sig=-10;
     m_sv_scaled_efc=-1;
     m_jf_scaled_efc=-1;

     m_pt_alternative = -1;
     m_nTrk_vtx1 = -1;
     m_mass_first_vtx = -1;
     m_e_first_vtx = -1;
     m_e_frac_vtx1 = -1;
     m_closestVtx_L3D = -1;
     m_JF_Lxy1 = -1;
     m_vtx1_MaxTrkRapidity_jf_path = -1;
     m_vtx1_AvgTrkRapidity_jf_path = -1;
     m_vtx1_MinTrkRapidity_jf_path = -1;
     m_MaxTrkRapidity_jf_path = -1;
     m_MinTrkRapidity_jf_path = -1;
     m_AvgTrkRapidity_jf_path = -1;


  }

  void MV2Tag::PrintInputs() {
    ATH_MSG_DEBUG("#BTAG# MV2 jet info: " <<
		  "  jet pt= "     << m_pt  <<
		  ", jet eta= "    << m_absEta );
    ATH_MSG_DEBUG("#BTAG# MV2 jet-trk info: " <<
		  "  jet trk n= "      << m_trkSum_ntrk   <<
		  ", jet trk pt sSum= "<< m_trkSum_sPt    <<
		  ", jet trk pt= "     << m_trkSum_vPt    <<
		  ", jet trk eta= "    << m_trkSum_vAbsEta );
    ATH_MSG_DEBUG("#BTAG# MV2 ip2d prob inputs: " <<
		  "  ip2_pu= "     << m_ip2_pu     <<
		  ", ip2_pb= "     << m_ip2_pb     <<
		  ", ip2_pc= "     << m_ip2_pc);
    ATH_MSG_DEBUG("#BTAG# MV2 ip2d llr inputs: " <<
		  "  ip2= "        << m_ip2        <<
		  ", ip2_c= "      << m_ip2_c      <<
		  ", ip2_cu= "     << m_ip2_cu);
    ATH_MSG_DEBUG("#BTAG# MV2 ip3d prob inputs: " <<
		  "  ip3_pu= "     << m_ip3_pu     <<
		  ", ip3_pb= "     << m_ip3_pb     <<
		  ", ip3_pc= "     << m_ip3_pc);
    ATH_MSG_DEBUG("#BTAG# MV2 ip3d llr inputs: " <<
		  "  ip3= "        << m_ip3        <<
		  ", ip3_c= "      << m_ip3_c      <<
		  ", ip3_cu= "     << m_ip3_cu);
    ATH_MSG_DEBUG("#BTAG# MV2 sv1 prob inputs: " <<
		  "  sv1_pu= "     << m_sv1_pu     <<
		  ", sv1_pb= "     << m_sv1_pb     <<
		  ", sv1_pc= "     << m_sv1_pc);
    ATH_MSG_DEBUG("#BTAG# MV2 sv1 llr inputs: " <<
		  "  sv1= "        << m_sv1        <<
		  ", sv1_c= "      << m_sv1_c      <<
		  ", sv1_cu= "     << m_sv1_c);
    ATH_MSG_DEBUG("#BTAG# MV2 jfcombnn prob inputs: " <<
		  "  jfc_pu= "     << m_jfc_pu     <<
		  ", jfc_pb= "     << m_jfc_pb     <<
		  ", jfc_pc= "     << m_jfc_pc);
    ATH_MSG_DEBUG("#BTAG# MV2 sv0 inputs: " <<
		  "  sv0= "        << m_sv0        <<
		  ", sv0_ntkv= "   << m_sv0_ntkv   <<
		  ", sv0_mass= "   << m_sv0_mass   <<
		  ", sv0_efrc= "   << m_sv0_efrc   <<
		  ", sv0_n2t= "    << m_sv0_n2t);
    ATH_MSG_DEBUG("#BTAG# MV2 sv1 inputs: " <<
		  "  sv1_ntkv= "   << m_sv1_ntkv   <<
		  ", sv1_mass= "   << m_sv1_mass   <<
		  ", sv1_efrc= "   << m_sv1_efrc   <<
		  ", sv1_n2t= "    << m_sv1_n2t    <<
		  ", sv1_Lxy= "    << m_sv1_Lxy    <<
		  ", sv1_L3d= "    << m_sv1_L3d    << 
		  ", sv1_sig3= "   << m_sv1_sig3   <<
		  ", sv1_dR= "     << m_sv1_dR	   <<
		  ", sv1_distmatlay= "     << m_sv1_distmatlay);
    ATH_MSG_DEBUG("#BTAG# MV2 jf inputs: " <<
		  "  jf_mass= "    << m_jf_mass    <<
		  ", jf_efrc= "    << m_jf_efrc    <<
		  ", jf_n2tv= "    << m_jf_n2tv    <<
		  ", jf_ntrkv= "   << m_jf_ntrkv   <<
		  ", jf_nvtx= "    << m_jf_nvtx    <<
		  ", jf_nvtx1t= "  << m_jf_nvtx1t  <<
		  ", jf_dphi= "    << m_jf_dphi    <<
		  ", jf_deta= "    << m_jf_deta    <<
		  ", jf_dR= "      << m_jf_dR      <<
		  ", jf_sig3= "    << m_jf_sig3    <<
		  ", jf_dR_flight= " << m_jf_dR_flight <<
		  ", jf_mass_unco= " << m_jf_mass_unco
		  );
    ATH_MSG_DEBUG("#BTAG# MV2 mvb inputs: " <<
      "  width= "         <<m_width         <<
		  ", n_trk_sigd0cut= "<<m_n_trk_sigd0cut<<
		  ", trk3_d0sig= "    <<m_trk3_d0sig    <<
		  ", trk3_z0sig= "    <<m_trk3_z0sig    <<
		  ", sv_scaled_efc= " <<m_sv_scaled_efc <<
		  ", jf_scaled_efc= " <<m_jf_scaled_efc);
    
    ATH_MSG_DEBUG("#BTAG# MV2 mv2cl100 inputs: " <<
      "nTrk_vtx1"                   <<m_nTrk_vtx1                  <<
      "mass_first_vtx"              <<m_mass_first_vtx             <<
      "e_first_vtx"                 <<m_e_first_vtx                <<
      "e_frac_vtx1"                 <<m_e_frac_vtx1                <<
      "closestVtx_L3D"              <<m_closestVtx_L3D             <<
      "JF_Lxy1"                     <<m_JF_Lxy1                    <<
      "vtx1_MaxTrkRapidity_jf_path" <<m_vtx1_MaxTrkRapidity_jf_path<<
      "vtx1_AvgTrkRapidity_jf_path" <<m_vtx1_AvgTrkRapidity_jf_path<<
      "vtx1_MinTrkRapidity_jf_path" <<m_vtx1_MinTrkRapidity_jf_path<<
      "MaxTrkRapidity_jf_path"      <<m_MaxTrkRapidity_jf_path     <<
      "MinTrkRapidity_jf_path"      <<m_MinTrkRapidity_jf_path     <<
      "AvgTrkRapidity_jf_path"      <<m_AvgTrkRapidity_jf_path);

  }

  void MV2Tag::SetVariableRefs(const std::vector<std::string> inputVars, TMVA::Reader* tmvaReader, unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers) {

    if (!m_useEgammaMethodMV2) {
      if(!tmvaReader) {
	ATH_MSG_WARNING("#BTAG# tmva method is chosen but tmvaReader==0!!");
	return;
      }
    }

    //std::cout<<"MV2 input vars: ";
    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      //std::cout<<inputVars.at(ivar)<<", ";
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
      //JetFitterCombNN output probabilities	  			                                       	  								      
      else if (inputVars.at(ivar)=="jfc_pu"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jfc_pu   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jfc_pu   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jfc_pb"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jfc_pb   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jfc_pb   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="jfc_pc"   ) { m_useEgammaMethodMV2 ? inputPointers.push_back(&m_jfc_pc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jfc_pc   ); nConfgVar++; }
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
      else {
	ATH_MSG_WARNING( "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in MV2... the algorithm will be 'disabled'.");//<<alias<<" "<<author);
	badVariableFound=true;
      }
    }
    //std::cout<<std::endl;

  }

}//end namespace
