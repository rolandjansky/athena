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
    declareProperty("forceMV2CalibrationAlias", m_forceMV2CalibrationAlias = false);
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
        
    std::ostringstream iss; //iss.clear();
    TMVA::MethodBase * kl=0;
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;
    
    if(calibHasChanged) {
      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
	ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for"<< m_taggerNameBase );
	return StatusCode::SUCCESS;
      }
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
      
      //now the new part istringstream
      TList* list = calib.first;
      for(int i=0; i<list->GetSize(); ++i) {
	TObjString* ss = (TObjString*)list->At(i);
	std::string sss = ss->String().Data();
	//KM: if it doesn't find "<" in the string, it starts from non-space character
	int posi = sss.find('<')!=-1 ? sss.find('<') : sss.find_first_not_of(" ");
	std::string tmp = sss.erase(0,posi);
	//std::cout<<tmp<<std::endl;
	iss << tmp.data();      //iss << sss.Data();
      }

      // now configure the TMVAReaders:
      /// check if the reader for this tagger needs update
      tmvaReader = new TMVA::Reader();
      // IP2D posteriors
      tmvaReader->AddVariable("ip2_pu",&m_ip2_pu);
      tmvaReader->AddVariable("ip2_pb",&m_ip2_pb);
      tmvaReader->AddVariable("ip2_pc",&m_ip2_pc);
      // IP3D posteriors
      tmvaReader->AddVariable("ip3_pu",&m_ip3_pu);
      tmvaReader->AddVariable("ip3_pb",&m_ip3_pb);
      tmvaReader->AddVariable("ip3_pc",&m_ip3_pc);
      //SV1 posteriors
      tmvaReader->AddVariable("sv1_pu",&m_sv1_pu);
      tmvaReader->AddVariable("sv1_pb",&m_sv1_pb);
      tmvaReader->AddVariable("sv1_pc",&m_sv1_pc);
      //JetFitterCombNN posteriors
      tmvaReader->AddVariable("jfc_pu",&m_jfc_pu);
      tmvaReader->AddVariable("jfc_pb",&m_jfc_pb);
      tmvaReader->AddVariable("jfc_pc",&m_jfc_pc);
      //SV0 informations
      tmvaReader->AddVariable("sv0",&m_sv0);
      tmvaReader->AddVariable("sv0_ntkv",&m_sv0_ntkv);
      tmvaReader->AddVariable("sv0mass",&m_sv0mass);
      tmvaReader->AddVariable("sv0_efrc",&m_sv0_efrc);
      tmvaReader->AddVariable("sv0_n2t",&m_sv0_n2t);
      //JetFitter informations
      tmvaReader->AddVariable("jf_mass",&m_jf_mass);
      tmvaReader->AddVariable("jf_efrc",&m_jf_efrc);
      tmvaReader->AddVariable("jf_n2tv",&m_jf_n2tv);
      tmvaReader->AddVariable("jf_ntrkv",&m_jf_ntrkv);
      tmvaReader->AddVariable("jf_nvtx",&m_jf_nvtx);
      tmvaReader->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);
      tmvaReader->AddVariable("jf_dphi",&m_jf_dphi);
      tmvaReader->AddVariable("jf_deta",&m_jf_deta);	  //tmvaReader->AddVariable("jfitvx_chi2/jfitvx_ndof",&m_chi2Ondof);
      tmvaReader->AddVariable("jf_sig3",&m_jf_sig3);
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
  
    /* default D3PD values*/
    // these variable are not alwasy defined -> make sure to use always the same default when training
    // can rely on xAOD default at some point but not necessary the best way in case this is not set explicitly or set to a value which is desired in BDTs

    m_jf_efrc   = -999;
    m_jf_mass   = -999;
    m_jf_sig3   = -999;
    m_jf_dphi   = -999;
    m_jf_deta   = -999;

    m_sv0mass    = -1;
    m_sv0_efrc   = -1;

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
    int jf_n2tv   =-1;

    int sv0_n2t    = -1;
    int sv0_ntkv   = -1;


    BTag->variable<double>(m_ip2d_infosource, "pu", ip2_pu);
    BTag->variable<double>(m_ip2d_infosource, "pb", ip2_pb);
    BTag->variable<double>(m_ip2d_infosource, "pc", ip2_pc);


    if("IP3D"==m_ip3d_infosource){
      ip3_pb=BTag->IP3D_pb();
      ip3_pu=BTag->IP3D_pu();
      ip3_pc=BTag->IP3D_pc();
    } 
    else {
      BTag->variable<double>(m_ip3d_infosource, "pu", ip3_pu);
      BTag->variable<double>(m_ip3d_infosource, "pb", ip3_pb);
      BTag->variable<double>(m_ip3d_infosource, "pc", ip3_pc);
    }


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
      BTag->variable<double>(m_jfprob_infosource, "pu", jfc_pu);
      BTag->variable<double>(m_jfprob_infosource, "pb", jfc_pb);
      BTag->variable<double>(m_jfprob_infosource, "pc", jfc_pc);
    }
   

    BTag->variable<double>(m_sv0_infosource, "discriminant", sv0);

    bool sv0OK=false;
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv0_infosource, "vertices", myVertices);
    if (myVertices.size()>0 && myVertices[0].isValid()){
      const xAOD::Vertex* firstVertex = *(myVertices[0]);
      sv0OK=true;
    }

    if(sv0OK){
      if ("SV0" == m_sv0_infosource){
	BTag->taggerInfo(m_sv0mass, xAOD::BTagInfo::SV0_masssvx);
	BTag->taggerInfo(m_sv0_efrc, xAOD::BTagInfo::SV0_efracsvx);
	BTag->taggerInfo(sv0_n2t, xAOD::BTagInfo::SV0_N2Tpair);
	BTag->taggerInfo(sv0_ntkv, xAOD::BTagInfo::SV0_NGTinSvx);
      }
      else{
	BTag->variable<float>(m_sv0_infosource, "masssvx", m_sv0mass);
	BTag->variable<float>(m_sv0_infosource, "efracsvx", m_sv0_efrc);
	BTag->variable<int>(m_sv0_infosource, "N2Tpair", sv0_n2t);
	BTag->variable<int>(m_sv0_infosource, "NGTinSvx", sv0_ntkv);
      }
      m_sv0mass/=1000.;
    }

    int jf_nvtx_tmp=0;
    bool jfitok=false;
    BTag->variable<int>(m_jftNN_infosource, "JetFitter_nVTX", jf_nvtx_tmp);
    if(jf_nvtx_tmp>0){
      jfitok=true;
    }

    if(jfitok){
      if("JetFitter" == m_jftNN_infosource){
	BTag->taggerInfo(jf_nvtx, xAOD::BTagInfo::JetFitter_nVTX);
	BTag->taggerInfo(jf_nvtx1t, xAOD::BTagInfo::JetFitter_nSingleTracks);
	BTag->taggerInfo(jf_ntrkv, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
	BTag->taggerInfo(m_jf_efrc, xAOD::BTagInfo::JetFitter_energyFraction);
	BTag->taggerInfo(m_jf_mass, xAOD::BTagInfo::JetFitter_mass);
	BTag->taggerInfo(m_jf_sig3, xAOD::BTagInfo::JetFitter_significance3d);
	BTag->taggerInfo(m_jf_dphi, xAOD::BTagInfo::JetFitter_deltaphi);
	BTag->taggerInfo(m_jf_deta, xAOD::BTagInfo::JetFitter_deltaeta);
	BTag->taggerInfo(jf_n2tv, xAOD::BTagInfo::JetFitter_N2Tpair);
      }
      else{
	BTag->variable<int>(m_jftNN_infosource, "JetFitter_nVTX", jf_nvtx);
	BTag->variable<int>(m_jftNN_infosource, "JetFitter_nSingleTracks", jf_nvtx1t);
	BTag->variable<int>(m_jftNN_infosource, "JetFitter_nTracksAtVtx", jf_ntrkv);
	BTag->variable<float>(m_jftNN_infosource, "JetFitter_energyFraction", m_jf_efrc);
	BTag->variable<float>(m_jftNN_infosource, "JetFitter_mass", m_jf_mass);
	BTag->variable<float>(m_jftNN_infosource, "JetFitter_significance3d", m_jf_sig3);
	BTag->variable<float>(m_jftNN_infosource, "JetFitter_deltaphi", m_jf_dphi);
	BTag->variable<float>(m_jftNN_infosource, "JetFitter_deltaeta", m_jf_deta);
	BTag->variable<int>(m_jftNN_infosource, "JetFitter_N2Tpair", jf_n2tv);
      }
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

    //////////////////////////////////
    // End of MV2 inputs retrieving //
    //////////////////////////////////
    ATH_MSG_DEBUG("#BTAG# MV2 inputs: "  <<
		  "  ip2_pu= "     << m_ip2_pu     <<
		  ", ip2_pb= "     << m_ip2_pb     <<
		  ", ip2_pc= "     << m_ip2_pc     <<
		  ", ip3_pu= "     << m_ip3_pu     <<
		  ", ip3_pb= "     << m_ip3_pb     <<
		  ", ip3_pc= "     << m_ip3_pc     <<
		  ", sv1_pu= "     << m_sv1_pu     <<
		  ", sv1_pb= "     << m_sv1_pb     <<
		  ", sv1_pc= "     << m_sv1_pc     <<
		  ", jfc_pu= "     << m_jfc_pu     <<
		  ", jfc_pb= "     << m_jfc_pb     <<
		  ", jfc_pc= "     << m_jfc_pc     <<
		  ", sv0= "        << m_sv0        <<
		  ", sv0_ntkv= "   << m_sv0_ntkv   <<
		  ", sv0mass= "    << m_sv0mass    <<
		  ", sv0_efrc= "   << m_sv0_efrc   <<
		  ", sv0_n2t= "    << m_sv0_n2t    <<
		  ", jf_mass= "    << m_jf_mass    <<
		  ", jf_efrc= "    << m_jf_efrc    <<
		  ", jf_n2tv= "    << m_jf_n2tv    <<
		  ", jf_ntrkv= "   << m_jf_ntrkv   <<
		  ", jf_nvtx= "    << m_jf_nvtx    <<
		  ", jf_nvtx1t= "  << m_jf_nvtx1t  <<
		  ", jf_dphi= "    << m_jf_dphi    <<
		  ", jf_deta= "    << m_jf_deta    <<//" chi2Ondof " << m_chi2Ondof <<
		  ", jf_sig3= "    << m_jf_sig3    <<
		  ", jpt= "        << jpt
		  );

    /* compute MV2: */
    double mv2 = -10.;
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
	mv2 = pos->second->EvaluateMVA( itmap2->second );
      }
      else ATH_MSG_WARNING("#BTAG#  kl==0");
    }
    ATH_MSG_DEBUG("#BTAG# MV2 weight: " << mv2);
    
    /** give information to the info class. */
    if(m_runModus=="analysis") {
      BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv2);
    }


    return StatusCode::SUCCESS;
  }
}//end namespace
