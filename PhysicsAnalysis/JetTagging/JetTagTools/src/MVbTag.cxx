/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/MVbTag.h"

#include "GaudiKernel/IToolSvc.h"


#include "xAODJet/Jet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "JetTagTools/JetTagUtils.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "AthenaKernel/Units.h"
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

#include "JetTagInfo/TruthInfo.h"


using Athena::Units::GeV;


namespace Analysis {

/** 
    @class MVbTag 
    BDT-based tagger including 16 quantities
    @author D. Duda dominik.duda@cern.ch, S. Fleischmann Sebastian.Fleischmann -at- cern.ch
*/

static const unsigned int njMVbetabin = 4;

MVbTag::MVbTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_calibrationTool("BTagCalibrationBroker"),
      m_runModus("analysis")
    {

    declareInterface<ITagTool>(this);
    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);
    // force MVb to always use a calibration derived from MVbCalibAlias jet collection
    declareProperty("forceMVbCalibrationAlias", m_forceMVbCalibrationAlias = false);
    declareProperty("MVbCalibAlias", m_MVbCalibAlias = "AntiKt4TopoEM");
    // global configuration:
    declareProperty("Runmodus", m_runModus);

    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
    declareProperty("inputIP2DSourceName", m_ip2d_infosource = "IP2D");
    declareProperty("inputIP3DSourceName", m_ip3d_infosource = "IP3D");
    declareProperty("inputJFSourceName", m_jftNN_infosource = "JetFitter");
    declareProperty("inputJFProbSourceName", m_jfprob_infosource = "JetFitterCombNN");
    declareProperty("xAODBaseName",      m_xAODBaseName);
    // which calibration folder to use
    declareProperty("taggerNameBase", m_taggerNameBase = "MVb");
    declareProperty("taggerName", m_taggerName = "MVb");


    /// and this what was used before the flip (comfigure from python now)
        //    m_ip2d_infosource  = "IP2DNeg";
        // //m_ip3d_infosource  = "IP3DNeg";
        // m_ip3d_infosource  = "IPInfoPlus";
        // m_sv0p_infosource  = "SV0InfoPlus";
        // m_jftNN_infosource = "JetFitterTagNNFlip";
        // m_jfcNN_infosource = "JetFitterCOMBNNIP3DNeg";
        // m_inputSV0WeightName = "SV0Flip";
        // m_inputIP3DWeightName = "IP3DNeg";
        // m_inputIP2DWeightName = "IP2DNeg";
}
     
MVbTag::~MVbTag() {
}
     
StatusCode MVbTag::initialize() {
    // prepare calibration tool:
    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
        ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
        return sc;
    } else {
        ATH_MSG_INFO("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    
    for (unsigned int ietabin=0; ietabin<njMVbetabin; ietabin++){
        TString calibname("bin"); calibname+=(ietabin+1);calibname+="/";
        calibname+=m_taggerNameBase;calibname+="Calib";
        std::string calibstring(calibname.Data());
        
        m_calibrationTool->registerHistogram(m_taggerNameBase, calibstring);
        m_tmvaReaders[ietabin].clear();
        m_tmvaMethod[ietabin].clear();
    }

    return StatusCode::SUCCESS;

}


StatusCode MVbTag::finalize() {
    // delete reader:
    for (unsigned int ietabin=0; ietabin<njMVbetabin; ietabin++){
      std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders[ietabin].begin();
      for( ; pos != m_tmvaReaders[ietabin].end(); ++pos ) {
            delete pos->second;
        }
      std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod[ietabin].begin();
      for( ; posm != m_tmvaMethod[ietabin].end(); ++posm ) {
            delete posm->second;
        }
    }
    return StatusCode::SUCCESS;
}

  StatusCode MVbTag::tagJet(xAOD::Jet& myJet, xAOD::BTagging* BTag) {
    /* jet author: */
    std::string author;
    if (m_forceMVbCalibrationAlias) {
      author = m_MVbCalibAlias;
    }
    else {
      author = JetTagUtils::getJetAuthor(myJet);
    }
    std::string alias = m_calibrationTool->channelAlias(author);
    //TString xmlFileNames[10];
    /// now configure the TMVAReaders:
    /// check if the reader for this tagger needs update

    //ATH_MSG_INFO("#BTAG# try to get xml..." );
    //std::vector<TMVA::Reader*> tmvaReaders(njMVbetabin, (TMVA::Reader*)0);
    TMVA::Reader* tmvaReaders[njMVbetabin];
    std::map<std::string, TMVA::Reader*>::iterator pos;
    std::ostringstream iss;
    TMVA::MethodBase * kl[njMVbetabin];
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;
    for (unsigned int ietabin=0; ietabin<njMVbetabin; ietabin++){
      //xmlFileNames[ietabin]="btag"; 
      //xmlFileNames[ietabin]+=m_taggerNameBase;
      //xmlFileNames[ietabin]+="bin";
      //xmlFileNames[ietabin]+=(ietabin+1); 
      //xmlFileNames[ietabin]+="Config_";
      //xmlFileNames[ietabin]+=alias; 
      //xmlFileNames[ietabin]+=".xml";
      /* check if calibration (neural net structure or weights) has to be updated: */
        
      TString calibname("bin");
      calibname+=(ietabin+1);
      calibname+="/";
      calibname+=m_taggerNameBase;
      calibname+="Calib";
      std::string calibstring(calibname.Data());
        
      //std::ifstream testfile(xmlFileNames[ietabin]);
      //if (!testfile) {
	//calibHasChanged = true;
      //}
      //ATH_MSG_INFO("#BTAG# xml file to be created: "<<xmlFileNames[ietabin]
      //    << " for " << calibstring << " with author " << author );
      ATH_MSG_DEBUG("#BTAG# Retrieve calib " << m_taggerNameBase << " " << author << " " << calibstring);
      std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase,
										author,calibstring);
      //         if(!m_calibrationTool->updatedTagger(m_taggerNameBase, 
      //                     alias, calibstring, name()) ){
      ATH_MSG_DEBUG("#BTAG# calib has changed: "<< calib.second);
      bool calibHasChanged = calib.second;
      //calibHasChanged = (calibHasChanged || (calib.second));
      if(calibHasChanged) {
	ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
	if(!calib.first) {
	  ATH_MSG_WARNING("#BTAG# TList can't be retrieved -> no calibration for MVb");
	  return StatusCode::SUCCESS;
	}
	//m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
	m_calibrationTool->updateHistogramStatus(m_taggerNameBase,
						 alias, calibstring, false);

	// now the ugly part: write an xml text file to be read by TMVAReader:
	//now the new part istringstream
	TList* list = calib.first;
        for(int i=0; i<list->GetSize(); ++i) {
	  TObjString* ss = (TObjString*)list->At(i);
	  std::string sss = ss->String().Data();
	  //KM: if it doesn't find "<" in the string, it starts from non-space character
	  int posi = sss.find('<')!=std::string::npos ? sss.find('<') : sss.find_first_not_of(" ");
	  std::string tmp = sss.erase(0,posi);
	  iss << tmp.data();      //iss << sss.Data();
	}
	//m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase, 
	//						  alias, calibstring, false, name());

      }  //end calibHasChanged

      // now configure the TMVAReaders:
      /// check if the reader for this tagger needs updatei   
      if (!m_calibrationTool->updatedTagger(m_taggerNameBase, 
					  alias, calibstring, name())) {
	tmvaReaders[ietabin] = new TMVA::Reader();
	tmvaReaders[ietabin]->AddVariable("jfit_ntrkAtVx",&m_jf_ntrkv);
	tmvaReaders[ietabin]->AddVariable("SV_2tv",&m_sv0_n2t);
	tmvaReaders[ietabin]->AddVariable("n_good_trks",&m_ngood_trks);
	tmvaReaders[ietabin]->AddVariable("IP2D",&m_ip2d);
	tmvaReaders[ietabin]->AddVariable("IP3D",&m_ip3d);
	tmvaReaders[ietabin]->AddVariable("SV0",&m_sv0);
	tmvaReaders[ietabin]->AddVariable("L_xy",&m_sv0_radius);
	tmvaReaders[ietabin]->AddVariable("SV_mass",&m_sv0_mass);
	tmvaReaders[ietabin]->AddVariable("SV_efrac",&m_sv0_efrc);
	tmvaReaders[ietabin]->AddVariable("SV_efrac2",&m_sv0_efrc2);
	tmvaReaders[ietabin]->AddVariable("jfit_mass",&m_jf_mass);
	tmvaReaders[ietabin]->AddVariable("jfit_sig3d",&m_jf_sig3);
	tmvaReaders[ietabin]->AddVariable("jfit_efrac",&m_jf_efrc);
	tmvaReaders[ietabin]->AddVariable("jfit_efrac2",&m_jf_efrc2);
	tmvaReaders[ietabin]->AddVariable("jfit_deltaR",&m_jf_deltaR);
	tmvaReaders[ietabin]->AddVariable("pT",&m_pt);
	tmvaReaders[ietabin]->AddSpectator("Eta",&m_eta);
        TMVA::IMethod* method= tmvaReaders[ietabin]->BookMVA(TMVA::Types::kBDT, iss.str().data() );
	ATH_MSG_DEBUG("#BTAG# new TMVA reader created from config"<< iss.str().data());
        kl[ietabin] = dynamic_cast<TMVA::MethodBase*>(method);
 
        // add it or overwrite it in the map of readers:
	pos = m_tmvaReaders[ietabin].find(alias);
        if (pos!=m_tmvaReaders[ietabin].end()) {
          delete pos->second;
	  m_tmvaReaders[ietabin].erase(pos);
        }
        m_tmvaReaders[ietabin].insert( std::make_pair( alias, tmvaReaders[ietabin] ) );
        itmap = m_tmvaMethod[ietabin].find(alias);
        if(itmap!=m_tmvaMethod[ietabin].end()) {
          delete itmap->second;
          m_tmvaMethod[ietabin].erase(itmap);
        }
        m_tmvaMethod[ietabin].insert( std::make_pair( alias, kl[ietabin] ) );

        m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase,
                                                          alias, calibstring, false, name());
      }

    } // end for(ietabin)

    /* retrieving MVb inputs*/    
    ATH_MSG_DEBUG("#BTAG# Jet author for MVb: " << author << ", alias: " << alias );

    m_pt   = myJet.pt()/GeV;
    m_eta  = std::fabs(myJet.eta());
    /* retrieving weights: */
    double sv0  = -999;
    double ip2d = -999;
    double ip3d = -999;

    int sv0_n2t  =-1;
    int sv0_ntrkv =-1; ///

    int jf_nvtx1t =-1; ////
    int jf_ntrkv  = 0;

    int ngood_trks = 0;

    bool has_sV_vertex=false;
    m_sv0_mass   =-1.;
    m_sv0_radius =-1.;
    m_sv0_efrc   =-1.;
    m_sv0_efrc2  =-1.;

    bool has_jfit_vertex=false;
    m_jf_efrc   = 0.;
    m_jf_efrc2  = -1.;
    m_jf_mass   = 0.;
    m_jf_sig3   = 0.;
    m_jf_deltaR = 0.5;


    BTag->variable<double>(m_sv0_infosource, "discriminant", sv0);
    BTag->loglikelihoodratio(m_ip2d_infosource, ip2d);
    BTag->loglikelihoodratio(m_ip3d_infosource, ip3d);


    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv0_infosource, "vertices", myVertices);
    if (myVertices.size()>0 && myVertices[0].isValid()){
      const xAOD::Vertex* firstVertex = *(myVertices[0]);
      double x = firstVertex->position().x();
      double y = firstVertex->position().y();
      m_sv0_radius = sqrt(pow(x,2)+pow(y,2));
      has_sV_vertex=true;
    }

    if(has_sV_vertex){
      if ("SV0" == m_sv0_infosource){
	BTag->taggerInfo(m_sv0_mass, xAOD::BTagInfo::SV0_masssvx);
	BTag->taggerInfo(m_sv0_efrc, xAOD::BTagInfo::SV0_efracsvx);
	BTag->taggerInfo(sv0_n2t,    xAOD::BTagInfo::SV0_N2Tpair);
	BTag->taggerInfo(sv0_ntrkv,  xAOD::BTagInfo::SV0_NGTinSvx);
      }
      else{
	BTag->variable<float>(m_sv0_infosource, "masssvx",  m_sv0_mass);
	BTag->variable<float>(m_sv0_infosource, "efracsvx", m_sv0_efrc);
	BTag->variable<int>(m_sv0_infosource,   "N2Tpair",  sv0_n2t);
	BTag->variable<int>(m_sv0_infosource,   "NGTinSvx", sv0_ntrkv);
      }
      m_sv0_mass/=GeV;
    }


                 
    float jf_dphi=-1, jf_deta=-1;
 
    int jf_nvtx=0;
    BTag->variable<int>(m_jftNN_infosource, "JetFitter_nVTX", jf_nvtx);
    if(jf_nvtx>0){
      has_jfit_vertex=true;
    }


    if(has_jfit_vertex){
      if("JetFitter" == m_jftNN_infosource){
	BTag->taggerInfo(jf_ntrkv,  xAOD::BTagInfo::JetFitter_nTracksAtVtx);
	BTag->taggerInfo(m_jf_efrc, xAOD::BTagInfo::JetFitter_energyFraction);
	BTag->taggerInfo(m_jf_mass, xAOD::BTagInfo::JetFitter_mass);
	BTag->taggerInfo(m_jf_sig3, xAOD::BTagInfo::JetFitter_significance3d);
	BTag->taggerInfo(jf_dphi,   xAOD::BTagInfo::JetFitter_deltaphi);
	BTag->taggerInfo(jf_deta,   xAOD::BTagInfo::JetFitter_deltaeta);
	BTag->taggerInfo(jf_nvtx1t, xAOD::BTagInfo::JetFitter_nSingleTracks);
      }
      else{
	BTag->variable<int>(m_jftNN_infosource,   "nTracksAtVtx",   jf_ntrkv);
	BTag->variable<float>(m_jftNN_infosource, "energyFraction", m_jf_efrc);
	BTag->variable<float>(m_jftNN_infosource, "mass",           m_jf_mass);
	BTag->variable<float>(m_jftNN_infosource, "significance3d", m_jf_sig3);
	BTag->variable<float>(m_jftNN_infosource, "deltaphi",       jf_dphi);
	BTag->variable<float>(m_jftNN_infosource, "deltaeta",       jf_deta);
	BTag->variable<int>(m_jftNN_infosource,   "nSingleTracks",  jf_nvtx1t);

      }
      m_jf_deltaR = std::sqrt(std::pow(jf_dphi,2)+std::pow(jf_deta,2));
      m_jf_mass/=GeV;
    }


 
    std::vector<float> ip3d_track_d0sig;
    if("IP3D"==m_ip3d_infosource){
      BTag->taggerInfo(ip3d_track_d0sig, xAOD::BTagInfo::IP3D_sigD0wrtPVofTracks);
    }
    else{
      BTag->variable<std::vector<float> >(m_ip3d_infosource, "sigD0wrtPVofTracks", ip3d_track_d0sig);
    }

    for(unsigned int i=0; i<ip3d_track_d0sig.size(); ++i){
      if(ip3d_track_d0sig[i]>1.75) {
	++ngood_trks;
      }
    }
    float nTracks = ip3d_track_d0sig.size();
      
    if(has_jfit_vertex){     
      m_jf_efrc2 =m_jf_efrc*(nTracks/(m_jf_ntrkv + jf_nvtx1t));
    }
    if(has_sV_vertex){
      m_sv0_efrc2=m_sv0_efrc*(nTracks/sv0_ntrkv);
    }   


    m_sv0 = sv0;
    m_ip2d = ip2d;
    m_ip3d = ip3d;

    m_sv0_n2t = sv0_n2t;
     
    m_jf_ntrkv = jf_ntrkv;
    m_ngood_trks = ngood_trks;

    /* compute MVb: */
    double mvb = -1.;
    int binnb = 0; //if (std::fabs(m_eta)<0.6)
    if (std::fabs(m_eta)>=0.6 && std::fabs(m_eta)<1.2) {
      binnb=1;
    } else if (std::fabs(m_eta)>=1.2 && std::fabs(m_eta)<1.8) {
      binnb=2;
    } else if (std::fabs(m_eta)>=1.8) {
      binnb=3;
    }
    ATH_MSG_DEBUG("#BTAG# MVb pt=" << std::fabs(m_eta) << " binnb = " << binnb+1);

    pos = m_tmvaReaders[binnb].find(alias);
    if(pos==m_tmvaReaders[binnb].end()) {
      int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
      if(0==alreadyWarned) {
        ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << alias);
        m_undefinedReaders.push_back(alias);
      }
    }
    else {
      std::map<std::string, TMVA::MethodBase*>::iterator itmap2 = m_tmvaMethod[binnb].find(alias);
      if((itmap2->second)!=0) {
	if (has_sV_vertex==true || has_jfit_vertex==true) {
	  mvb =(double) pos->second->EvaluateMVA(itmap2->second);
	}
      }
      else ATH_MSG_WARNING("#BTAG#  kl==0");
    }
    /// 0.5 because signal and background were scaled to the same amount (handling pt as input)   ///correct reader ????
    ATH_MSG_DEBUG("#BTAG# MVb weight: " << mvb);

    /** give information to the info class. */
    if(m_runModus=="analysis") {
      BTag->setVariable<double>(m_xAODBaseName, "discriminant", mvb);
    }

    return StatusCode::SUCCESS;

  }

}//end namespace



