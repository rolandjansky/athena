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
#include "TMVA/Reader.h"
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

namespace Analysis {

  /** 
      @class MV2Tag 
      BDT-based tagger combining 27 low-level b-tagging variables
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
    // to change input weights:
    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
    declareProperty("inputIP2DSourceName", m_ip2d_infosource = "IP2D");
    declareProperty("inputIP3DSourceName", m_ip3d_infosource = "IP3D");
    declareProperty("inputJFSourceName", m_jftNN_infosource = "JetFitter");
    declareProperty("inputJFProbSourceName", m_jfprob_infosource = "JetFitterCombNN");
    declareProperty("xAODBaseName",      m_xAODBaseName);
    // which calibration folder to use 
    declareProperty("taggerNameBase", m_taggerNameBase = "MV2");
    declareProperty("taggerName", m_taggerName = "MV2");

    declareProperty("trainingConfig", m_trainingConfig = "Default");

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
    m_warnCounter=0;

    // prepare calibration tool:
    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib");
    m_tmvaReaders.clear();
    m_tmvaMethod.clear();
    return StatusCode::SUCCESS;
  }


  StatusCode MV2Tag::finalize() {
    // delete readers:
    std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
    for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
    std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
    for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    return StatusCode::SUCCESS;
  }

  StatusCode MV2Tag::tagJet(xAOD::Jet& myJet, xAOD::BTagging* BTag) {
    
    // VD: make sure that we initialize all variables
    ClearInputs();
    
    if ( m_disableAlgo ) {
      if (m_warnCounter<20) {
	ATH_MSG_WARNING("#BTAG# .... algorithm was disabled due to mis-configuration problems: no output will be written ... PLEASE WATCH OUT" );
	m_warnCounter++;
      }
      return StatusCode::SUCCESS;
    }
    
    /* jet author: */
    std::string author ;
    if (m_forceMV2CalibrationAlias) {
      author = m_MV2CalibAlias;
    }
    else {
      author = JetTagUtils::getJetAuthor(myJet);
    }
    
    std::string alias = m_calibrationTool->channelAlias(author);//why this gives always the same?

    TMVA::Reader* tmvaReader;
    std::map<std::string, TMVA::Reader*>::iterator pos;

    ATH_MSG_DEBUG("#BTAG# Jet author for MV2: " << author << ", alias: " << alias );


    /* check if calibration (neural net structure or weights) has to be updated: */
    std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author,m_taggerNameBase+"Calib"); 

    bool calibHasChanged = calib.second;
    //std::cout << "CalibHasChanged is: " <<  calibHasChanged << std::endl;  
    std::ostringstream iss; //iss.clear();
    TMVA::MethodBase * kl=0;
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;
    
    if(calibHasChanged) {
      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
	ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for"<< m_taggerNameBase );
	m_disableAlgo=true;
	return StatusCode::SUCCESS;
      }
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
      
      int calibNvars=0;
      //now the new part istringstream
      TList* list = calib.first;
      for(int i=0; i<list->GetSize(); ++i) {
	TObjString* ss = (TObjString*)list->At(i);
	std::string sss = ss->String().Data();
	//KM: if it doesn't find "<" in the string, it starts from non-space character
	int posi = sss.find('<')!=-1 ? sss.find('<') : sss.find_first_not_of(" ");
	std::string tmp = sss.erase(0,posi);
	//std::cout<<tmp<<std::endl;
	iss << tmp.data();
	if (i<100) {
	  if ( tmp.find("Variables NVar")!=std::string::npos ) {
	    std::string newString=tmp.substr(tmp.find("\"")+1,tmp.find("\" ")-(tmp.find("\"")+1));
	    calibNvars=stoi(newString);
	  }
	}
      }

      // now configure the TMVAReaders:
      /// check if the reader for this tagger needs update
      tmvaReader = new TMVA::Reader();
      
      // VD: I know it could be done in a more elegant way but I prefer to keep the blocks completely separated
      int nConfgVar=0;
      if ( m_trainingConfig=="Default") {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2_pu",&m_ip2_pu);  nConfgVar++;
	tmvaReader->AddVariable("ip2_pb",&m_ip2_pb);  nConfgVar++;
	tmvaReader->AddVariable("ip2_pc",&m_ip2_pc);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3_pu",&m_ip3_pu);  nConfgVar++;
	tmvaReader->AddVariable("ip3_pb",&m_ip3_pb);  nConfgVar++;
	tmvaReader->AddVariable("ip3_pc",&m_ip3_pc);  nConfgVar++;
	//SV1 posteriors
	tmvaReader->AddVariable("sv1_pu",&m_sv1_pu);  nConfgVar++;
	tmvaReader->AddVariable("sv1_pb",&m_sv1_pb);  nConfgVar++;
	tmvaReader->AddVariable("sv1_pc",&m_sv1_pc);  nConfgVar++;
	//JetFitterCombNN posteriors
	tmvaReader->AddVariable("jfc_pu",&m_jfc_pu);  nConfgVar++;
	tmvaReader->AddVariable("jfc_pb",&m_jfc_pb);  nConfgVar++;
	tmvaReader->AddVariable("jfc_pc",&m_jfc_pc);  nConfgVar++;
	//SV0 informations
	tmvaReader->AddVariable("sv0",&m_sv0);  nConfgVar++;
	tmvaReader->AddVariable("sv0_ntkv",&m_sv0_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv0mass" ,&m_sv0_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv0_efrc",&m_sv0_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv0_n2t" ,&m_sv0_n2t);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_dphi",&m_jf_dphi);  nConfgVar++;
	tmvaReader->AddVariable("jf_deta",&m_jf_deta);  nConfgVar++;	  //tmvaReader->AddVariable("jfitvx_chi2/jfitvx_ndof",&m_chi2Ondof);
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;

      } else if (m_trainingConfig=="NoJetFitterCombNN" ) {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//SV1 posteriors
	tmvaReader->AddVariable("sv1",&m_sv1);  nConfgVar++;
	tmvaReader->AddVariable("sv1_c",&m_sv1_c);  nConfgVar++;
	tmvaReader->AddVariable("sv1_cu",&m_sv1_cu);  nConfgVar++;
	//SV0 informations
	tmvaReader->AddVariable("sv0",&m_sv0);  nConfgVar++;
	tmvaReader->AddVariable("sv0_ntkv",&m_sv0_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv0mass" ,&m_sv0_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv0_efrc",&m_sv0_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv0_n2t" ,&m_sv0_n2t);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
	
      } else if (m_trainingConfig=="NoJF_NoSV0" ) {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//SV1 posteriors
	tmvaReader->AddVariable("sv1",&m_sv1);  nConfgVar++;
	tmvaReader->AddVariable("sv1_c",&m_sv1_c);  nConfgVar++;
	tmvaReader->AddVariable("sv1_cu",&m_sv1_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else if (m_trainingConfig=="NoJF_NoSV0_V2" ) {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//SV1 posteriors
	tmvaReader->AddVariable("sv1",&m_sv1);  nConfgVar++;
	tmvaReader->AddVariable("sv1_c",&m_sv1_c);  nConfgVar++;
	tmvaReader->AddVariable("sv1_cu",&m_sv1_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	tmvaReader->AddVariable("sv1_sig3" ,&m_sv1_sig3);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else if (m_trainingConfig=="NoJF_NoSV0NoSv1" ) {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	tmvaReader->AddVariable("sv1_sig3" ,&m_sv1_sig3);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else if (m_trainingConfig=="NoJF_NoSV0NoSv1_V1" ) {
	tmvaReader->AddVariable("pt",&m_pt);	  nConfgVar++;
	tmvaReader->AddVariable("abs(eta)",&m_absEta);	  nConfgVar++;
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	tmvaReader->AddVariable("sv1_sig3" ,&m_sv1_sig3);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else if (m_trainingConfig=="NoJF_NoSV0NoSv1_V2" ) {
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	tmvaReader->AddVariable("sv1_sig3" ,&m_sv1_sig3);  nConfgVar++;
	tmvaReader->AddVariable("sv1_dR"   ,&m_sv1_dR);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else if (m_trainingConfig=="NoJF_NoSV0NoSv1_V3" ) {
	tmvaReader->AddVariable("pt",&m_pt);	  nConfgVar++;
	tmvaReader->AddVariable("abs(eta)",&m_absEta);  nConfgVar++;	
	// IP2D posteriors
	tmvaReader->AddVariable("ip2",&m_ip2);  nConfgVar++;
	tmvaReader->AddVariable("ip2_c",&m_ip2_c);  nConfgVar++;
	tmvaReader->AddVariable("ip2_cu",&m_ip2_cu);  nConfgVar++;
	// IP3D posteriors
	tmvaReader->AddVariable("ip3",&m_ip3);  nConfgVar++;
	tmvaReader->AddVariable("ip3_c",&m_ip3_c);  nConfgVar++;
	tmvaReader->AddVariable("ip3_cu",&m_ip3_cu);  nConfgVar++;
	//more sv1 informations
	tmvaReader->AddVariable("sv1_ntkv" ,&m_sv1_ntkv);  nConfgVar++;
	tmvaReader->AddVariable("sv1_mass" ,&m_sv1_mass);  nConfgVar++;
	tmvaReader->AddVariable("sv1_efrc" ,&m_sv1_efrc);  nConfgVar++;
	tmvaReader->AddVariable("sv1_n2t"  ,&m_sv1_n2t);  nConfgVar++;
	tmvaReader->AddVariable("sv1_Lxy"  ,&m_sv1_Lxy);  nConfgVar++;
	tmvaReader->AddVariable("sv1_L3d"  ,&m_sv1_L3d);  nConfgVar++;
	tmvaReader->AddVariable("sv1_sig3" ,&m_sv1_sig3);  nConfgVar++;
	tmvaReader->AddVariable("sv1_dR"   ,&m_sv1_dR);  nConfgVar++;
	//JetFitter informations
	tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);  nConfgVar++;
	tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);  nConfgVar++;
	tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);  nConfgVar++;
	tmvaReader->AddVariable("jf_mass",&m_jf_mass);  nConfgVar++;
	tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);  nConfgVar++;
	tmvaReader->AddVariable("jf_dR",&m_jf_dR);  nConfgVar++;
	tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);  nConfgVar++;
      } else {
	ATH_MSG_ERROR( "#BTAG# configString: "+m_trainingConfig+" not recognized .... returning failure");
	return StatusCode::FAILURE;
      }

      if ( calibNvars!=nConfgVar ) {
	ATH_MSG_WARNING( "#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << calibNvars << " ... the algorithm will be 'disabled' ");
	m_disableAlgo=true;
	return StatusCode::SUCCESS;	
      }

      //tmvaReader->BookMVA("BDT", xmlFileName);
      TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kBDT, iss.str().data() );
      kl = dynamic_cast<TMVA::MethodBase*>(method);

      // add it or overwrite it in the map of readers:
      pos = m_tmvaReaders.find(alias);
      if(pos!=m_tmvaReaders.end()) {
	delete pos->second;
	m_tmvaReaders.erase(pos);
      }
      itmap = m_tmvaMethod.find(alias);
      if(itmap!=m_tmvaMethod.end()) {
	delete itmap->second;
	m_tmvaMethod.erase(itmap);
      }
      m_tmvaReaders.insert( std::make_pair( alias, tmvaReader ) );
      m_tmvaMethod.insert( std::make_pair( alias, kl ) );
    }
    
    double jpt = myJet.pt();
    double jeta= myJet.eta();
    m_pt = jpt;
    m_absEta = fabs(jeta);
  
    TVector3 v3_jet(myJet.px(),myJet.py(),myJet.pz());

    /* default D3PD values*/
    // these variable are not alwasy defined -> make sure to use always the same default when training
    // can rely on xAOD default at some point but not necessary the best way in case this is not set explicitly or set to a value which is desired in BDTs

    ///// TMVA does not accept double or int -> use doubles to get xAOD-double info and copy to float

    double sv0=-1;
    double ip2_pb=-1;
    double ip2_pc=-1;
    double ip2_pu=-1;
    double ip3_pb=-1;
    double ip3_pc=-1;
    double ip3_pu=-1;
    double sv1_pb=-1;
    double sv1_pc=-1;
    double sv1_pu=-1;
    double jfc_pb=-1;
    double jfc_pc=-1;
    double jfc_pu=-1;

    int jf_nvtx   = -1;
    int jf_nvtx1t = -1;
    int jf_ntrkv  = -1;
    int jf_n2tv   = -1;
    int sv0_n2t   = -1;
    int sv0_ntkv  = -1;
    int sv1_n2t   = -1;
    int sv1_ntkv  = -1;

    bool status = true;

    status &= BTag->variable<double>(m_ip2d_infosource, "pu", ip2_pu);
    status &= BTag->variable<double>(m_ip2d_infosource, "pb", ip2_pb);
    status &= BTag->variable<double>(m_ip2d_infosource, "pc", ip2_pc);
    m_ip2=BTag->IP2D_loglikelihoodratio();

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
    m_ip3=BTag->calcLLR(ip3_pb,ip3_pu);


    if("SV1"==m_sv1_infosource){
      sv1_pb=BTag->SV1_pb();
      sv1_pu=BTag->SV1_pu();
      sv1_pc=BTag->SV1_pc();
    } 
    else {
      status &= BTag->variable<double>(m_sv1_infosource, "pu", sv1_pu);
      status &= BTag->variable<double>(m_sv1_infosource, "pb", sv1_pb);
      status &= BTag->variable<double>(m_sv1_infosource, "pc", sv1_pc);
    }
    m_sv1=BTag->calcLLR(sv1_pb,sv1_pu);

    if("JetFitterCombNN"==m_jfprob_infosource){
      jfc_pb=BTag->JetFitterCombNN_pb();
      jfc_pu=BTag->JetFitterCombNN_pu();
      jfc_pc=BTag->JetFitterCombNN_pc();
    }
    else if ("JetFitter"==m_jfprob_infosource){
      jfc_pb=BTag->JetFitter_pb();
      jfc_pu=BTag->JetFitter_pu();
      jfc_pc=BTag->JetFitter_pc();
    } 
    else {
      status &= BTag->variable<double>(m_jfprob_infosource, "pu", jfc_pu);
      status &= BTag->variable<double>(m_jfprob_infosource, "pb", jfc_pb);
      status &= BTag->variable<double>(m_jfprob_infosource, "pc", jfc_pc);
    }
   
    status &= BTag->variable<double>(m_sv0_infosource, "significance3D", sv0);
    if (!status) ATH_MSG_WARNING("#BTAG# error after reading SV0 significance information");

    bool sv0OK=false;
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    // don't check the following status
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv0_infosource, "vertices", myVertices);
    if (myVertices.size()>0 && myVertices[0].isValid()){
      const xAOD::Vertex* firstVertex = *(myVertices[0]);
      sv0OK=true;
    }

    if(sv0OK){
      if ("SV0" == m_sv0_infosource){
	status &= BTag->taggerInfo(m_sv0_mass, xAOD::BTagInfo::SV0_masssvx);
	status &= BTag->taggerInfo(m_sv0_efrc, xAOD::BTagInfo::SV0_efracsvx);
	status &= BTag->taggerInfo(sv0_n2t, xAOD::BTagInfo::SV0_N2Tpair);
	status &= BTag->taggerInfo(sv0_ntkv, xAOD::BTagInfo::SV0_NGTinSvx);
      }
      else{
	status &= BTag->variable<float>(m_sv0_infosource, "masssvx", m_sv0_mass);
	status &= BTag->variable<float>(m_sv0_infosource, "efracsvx", m_sv0_efrc);
	status &= BTag->variable<int>(m_sv0_infosource, "N2Tpair", sv0_n2t);
	status &= BTag->variable<int>(m_sv0_infosource, "NGTinSvx", sv0_ntkv);
      }
      if ( m_trainingConfig=="Default" ) m_sv0_mass/=1000.;
    }

    //Accessing primary vertex information
    float pv_x=0, pv_y=0, pv_z=0;
    if(m_priVtx) {
      pv_x=m_priVtx->x();
      pv_y=m_priVtx->y();
      pv_z=m_priVtx->z();
    }
    else {
      ATH_MSG_WARNING("#BTAG# MV2 cannot access primary vertex, PV is set as (0,0,0).");
    }
    
    bool sv1OK=false;
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices1;
    // don't check the following status
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", myVertices1);
    if (myVertices1.size()>0 && myVertices1[0].isValid()){
      const xAOD::Vertex* firstVertex = *(myVertices1[0]);

      float dx = firstVertex->x() - pv_x;
      float dy = firstVertex->y() - pv_y;
      float dz = firstVertex->z() - pv_z;
      
      TVector3 v3_PvSv(dx,dy,dz);
      m_sv1_dR = v3_PvSv.DeltaR(v3_jet);

      m_sv1_Lxy= sqrt( pow(dx,2) + pow(dy,2) );
      m_sv1_L3d= sqrt( pow(dx,2) + pow(dy,2) + pow(dz,2) );

      //ATH_MSG_WARNING("#BTAG# MV2 sv1_Lxy, sv1_L3d= "<<m_sv1_Lxy<<"\t"<<m_sv1_L3d);
      sv1OK=true;
    }

    if(sv1OK){
      if ("SV1" == m_sv1_infosource){
	status &= BTag->taggerInfo(m_sv1_mass, xAOD::BTagInfo::SV1_masssvx);
	status &= BTag->taggerInfo(m_sv1_efrc, xAOD::BTagInfo::SV1_efracsvx);
	status &= BTag->taggerInfo(sv1_n2t   , xAOD::BTagInfo::SV1_N2Tpair);
	status &= BTag->taggerInfo(sv1_ntkv  , xAOD::BTagInfo::SV1_NGTinSvx);
	status &= BTag->variable<float>(m_sv1_infosource, "significance3d" , m_sv1_sig3);
      }
      else{
	status &= BTag->variable<float>(m_sv1_infosource, "masssvx" , m_sv1_mass);
	status &= BTag->variable<float>(m_sv1_infosource, "efracsvx", m_sv1_efrc);
	status &= BTag->variable<int>(m_sv1_infosource  , "N2Tpair" , sv1_n2t);
	status &= BTag->variable<int>(m_sv1_infosource  , "NGTinSvx", sv1_ntkv);
	status &= BTag->variable<float>(m_sv1_infosource, "significance3d" , m_sv1_sig3);
      }
    }

    int jf_nvtx_tmp=0;
    int jf_nvtx1t_tmp=0;
    bool jfitok=false;
    status &= BTag->variable<int>(m_jftNN_infosource, "nVTX", jf_nvtx_tmp);
    status &= BTag->variable<int>(m_jftNN_infosource, "nSingleTracks",  jf_nvtx1t_tmp);
    //std::cout << jf_nvtx_tmp << std::endl;
    if(jf_nvtx_tmp>0 ||  jf_nvtx1t_tmp>0){
      jfitok=true;
    }

    if(jfitok){
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
    }
    
    if (!status) {
      ATH_MSG_WARNING("#BTAG# Missing input data: cannot compute desired results");
      // return StatusCode::SUCCESS;
    }

    m_sv0 = sv0;
    m_ip2_pb = ip2_pb;
    m_ip2_pc = ip2_pc;
    m_ip2_pu = ip2_pu;
    m_ip3_pb = ip3_pb;
    m_ip3_pc = ip3_pc;
    m_ip3_pu = ip3_pu;
    m_sv1_pb = sv1_pb;
    m_sv1_pc = sv1_pc;
    m_sv1_pu = sv1_pu;
    m_jfc_pb = jfc_pb;
    m_jfc_pc = jfc_pc;
    m_jfc_pu = jfc_pu;

    m_jf_nvtx   = jf_nvtx;
    m_jf_nvtx1t = jf_nvtx1t;
    m_jf_ntrkv  = jf_ntrkv;
    m_jf_n2tv   = jf_n2tv;
  
    m_sv0_n2t   = sv0_n2t;
    m_sv0_ntkv  = sv0_ntkv;
    
    if      (m_jf_dphi==-10 and m_jf_deta==-10) m_jf_dR = -1;
    else if (m_jf_dphi==-11 and m_jf_deta==-11) m_jf_dR = -1;
    else                                        m_jf_dR = hypot(m_jf_dphi,m_jf_deta) ;
					     
    if      (m_ip2==-30) m_ip2=-20;
    else if (m_ip2_pu>1) m_ip2=-20;
    m_ip2_c  = (m_ip2_pb>0 && m_ip2_pc>0 && m_ip2_pc<1) ? log(m_ip2_pb/m_ip2_pc) : -20;
    m_ip2_cu = (m_ip2_pc>0 && m_ip2_pu>0 && m_ip2_pu<1) ? log(m_ip2_pc/m_ip2_pu) : -20;
    if      (m_ip3==-30) m_ip3=-20;
    else if (m_ip3_pu>1) m_ip3=-20;
    m_ip3_c  = (m_ip3_pb>0 && m_ip3_pc>0 && m_ip3_pc<1) ? log(m_ip3_pb/m_ip3_pc) : -20;
    m_ip3_cu = (m_ip3_pc>0 && m_ip3_pu>0 && m_ip3_pu<1) ? log(m_ip3_pc/m_ip3_pu) : -20;

    m_sv1_c  = (m_sv1_pb>0 && m_sv1_pc>0) ? log(m_sv1_pb/m_sv1_pc) : -20;
    m_sv1_cu = (m_sv1_pc>0 && m_sv1_pu>0) ? log(m_sv1_pc/m_sv1_pu) : -20;
    m_sv1_n2t   = sv1_n2t;
    m_sv1_ntkv  = sv1_ntkv;

    //////////////////////////////////
    // End of MV2 inputs retrieving //
    //////////////////////////////////
    ATH_MSG_DEBUG("#BTAG# MV2 inputs: ");
    ATH_MSG_DEBUG("#BTAG# ip2d inputs: " <<
		  "  ip2_pu= "     << m_ip2_pu     <<
		  ", ip2_pb= "     << m_ip2_pb     <<
		  ", ip2_pc= "     << m_ip2_pc);
    ATH_MSG_DEBUG("#BTAG# ip2d llr inputs: " <<
		  ", ip2= "        << m_ip2        <<
		  ", ip2_c= "      << m_ip2_c      <<
		  ", ip2_cu= "     << m_ip2_cu);
    ATH_MSG_DEBUG("#BTAG# ip3d inputs: " <<
		  ", ip3_pu= "     << m_ip3_pu     <<
		  ", ip3_pb= "     << m_ip3_pb     <<
		  ", ip3_pc= "     << m_ip3_pc);
    ATH_MSG_DEBUG("#BTAG# ip3d llr inputs: " <<
		  ", ip3= "        << m_ip3        <<
		  ", ip3_c= "      << m_ip3_c      <<
		  ", ip3_cu= "     << m_ip3_cu);
    ATH_MSG_DEBUG("#BTAG# sv1 inputs: " <<
		  ", sv1_pu= "     << m_sv1_pu     <<
		  ", sv1_pb= "     << m_sv1_pb     <<
		  ", sv1_pc= "     << m_sv1_pc     <<
		  ", sv1= "        << m_sv1        <<
		  ", sv1_c= "      << m_sv1_c      <<
		  ", sv1_cu= "     << m_sv1_c);
    ATH_MSG_DEBUG("#BTAG# jf inputs: " <<
		  ", jfc_pu= "     << m_jfc_pu     <<
		  ", jfc_pb= "     << m_jfc_pb     <<
		  ", jfc_pc= "     << m_jfc_pc);
    ATH_MSG_DEBUG("#BTAG# more sv0 inputs: " <<
		  ", sv0= "        << m_sv0        <<
		  ", sv0_ntkv= "   << m_sv0_ntkv   <<
		  ", sv0_mass= "   << m_sv0_mass   <<
		  ", sv0_efrc= "   << m_sv0_efrc   <<
		  ", sv0_n2t= "    << m_sv0_n2t);
    ATH_MSG_DEBUG("#BTAG# more sv1 inputs: " <<
		  ", sv1_ntkv= "   << m_sv1_ntkv   <<
		  ", sv1_mass= "   << m_sv1_mass   <<
		  ", sv1_efrc= "   << m_sv1_efrc   <<
		  ", sv1_n2t= "    << m_sv1_n2t    <<
		  ", sv1_Lxy= "    << m_sv1_Lxy    <<
		  ", sv1_L3d= "    << m_sv1_L3d    << 
		  ", sv1_sig3= "   << m_sv1_sig3   <<
		  ", sv1_dR= "     << m_sv1_dR);
    ATH_MSG_DEBUG("#BTAG# more jf inputs: " <<
		  ", jf_mass= "    << m_jf_mass    <<
		  ", jf_efrc= "    << m_jf_efrc    <<
		  ", jf_n2tv= "    << m_jf_n2tv    <<
		  ", jf_ntrkv= "   << m_jf_ntrkv   <<
		  ", jf_nvtx= "    << m_jf_nvtx    <<
		  ", jf_nvtx1t= "  << m_jf_nvtx1t  <<
		  ", jf_dphi= "    << m_jf_dphi    <<
		  ", jf_deta= "    << m_jf_deta    <<//" chi2Ondof " << m_chi2Ondof <<
		  ", jf_dR= "      << m_jf_dR      <<
		  ", jf_sig3= "    << m_jf_sig3);
    ATH_MSG_DEBUG("#BTAG# jet info: " <<
		  ", jpt= "        << jpt          <<
		  ", jeta= "        << jeta
		  );
    
    /* compute MV2: */
    double mv2 = -10.;  double mv2m_pb=-10., mv2m_pu=-10., mv2m_pc=-10. ;
    pos = m_tmvaReaders.find(alias);
    if(pos==m_tmvaReaders.end()) {
      int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
      if(0==alreadyWarned) {
        ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << alias);
        m_undefinedReaders.push_back(alias);
      }
    }
    else {
      std::map<std::string, TMVA::MethodBase*>::iterator itmap2 = m_tmvaMethod.find(alias);
      if((itmap2->second)!=0) {
	if (m_taggerNameBase.find("MV2c")!=-1) mv2 = pos->second->EvaluateMVA( itmap2->second );
	else {
	  std::vector<float> outputs= pos->second->EvaluateMulticlass( itmap2->second );
	  if (outputs.size()==3) {
	    mv2m_pb=outputs[0]; mv2m_pu=outputs[1]; mv2m_pc=outputs[2] ;
	  }
	  else {
	    ATH_MSG_WARNING("#BTAG# Unkown error, outputs vector size not 3!!!" );
	    //return StatusCode::SUCCESS;
	  }
	}
      }
      else ATH_MSG_WARNING("#BTAG#  kl==0");
    }
    if (m_taggerNameBase.find("MV2c")!=-1) ATH_MSG_DEBUG("#BTAG# MV2 weight: " << mv2<<", "<<alias);
    else ATH_MSG_DEBUG("#BTAG# MV2 pb, pu, pc= " << mv2m_pb<<"\t"<<mv2m_pu<<"\t"<<mv2m_pc<<", "<<alias);
    
    /** give information to the info class. */
    if(m_runModus=="analysis") {
      if (m_taggerNameBase.find("MV2c")!=-1) BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv2);
      else {
	BTag->setVariable<double>(m_xAODBaseName, "pb", mv2m_pb);
	BTag->setVariable<double>(m_xAODBaseName, "pu", mv2m_pu);
	BTag->setVariable<double>(m_xAODBaseName, "pc", mv2m_pc);
      }
    }


    return StatusCode::SUCCESS;
  }
  
  void MV2Tag::ClearInputs() {
     m_ip2=-20;
     m_ip2_c=-20;
     m_ip2_cu=-20;
     m_ip3=-20;
     m_ip3_c=-20;
     m_ip3_cu=-20;
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
     m_jf_dR=-1;
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
     m_sv0_radius=-1; 
     m_jf_mass=-1000; 
     m_jf_efrc=-1; 
     m_jf_n2tv=-1; 
     m_jf_ntrkv=-1; 
     m_jf_nvtx=-1; 
     m_jf_nvtx1t=-1; 
     m_jf_dphi=-11; 
     m_jf_deta=-11; 
     m_jf_sig3=-100;
  }

}//end namespace
