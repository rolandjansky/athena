/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/MV1Tag.h"

#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/Jet.h"
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

using Athena::Units::GeV;

namespace Analysis {

  /** 
      @class MV1Tag 
      Neural-net based tagger combining IP3D, SV1 and JetFitterCombNN
      @author L. Vacavant
  */

  MV1Tag::MV1Tag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_calibrationTool("BTagCalibrationBroker"),
      m_runModus("analysis") {
    declareInterface<ITagTool>(this);
    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);
    // global configuration:
    declareProperty("Runmodus", m_runModus);
    declareProperty("xAODBaseName",      m_xAODBaseName="didyouforgettoconfigthename");
    // to change input weights:
    declareProperty("inputIP3DWeightName", m_inputIP3DWeightName = "IP3D");
    declareProperty("inputSV1WeightName", m_inputSV1WeightName = "SV1");
    declareProperty("inputJetFitterWeightName", m_inputJetFitterWeightName = "JetFitterCombNN");
    // which calibration folder to use
    declareProperty("taggerNameBase", m_taggerNameBase = "MV1");
    declareProperty("taggerName", m_taggerName = "MV1");
  }


  MV1Tag::~MV1Tag() {
  }


  StatusCode MV1Tag::initialize() {
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


  StatusCode MV1Tag::finalize() {
    // delete readers:
    std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
    for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
    std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
    for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    return StatusCode::SUCCESS;
  }


  StatusCode MV1Tag::tagJet(const xAOD::Jet* jetToTag, xAOD::BTagging* BTag) {

    /* jet author: */
    std::string author = JetTagUtils::getJetAuthor(jetToTag);
    std::string alias = m_calibrationTool->channelAlias(author);

    ATH_MSG_DEBUG("#BTAG# Jet author: " << author << ", alias: " << alias );

    /* tweak name for Flip versions: */

    TMVA::Reader* tmvaReader;
    std::map<std::string, TMVA::Reader*>::iterator pos;
    /* check if calibration (neural net structure or weights) has to be updated: */
    std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib");
    bool calibHasChanged = calib.second;

    std::ostringstream iss; //iss.clear();
    TMVA::MethodBase * kl=0;
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;

    if(calibHasChanged) {
      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
        ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for MV1");
        return StatusCode::SUCCESS;
      }
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);

      //now the new part istringstream
      TList* list = calib.first; 
      for(int i=0; i<list->GetSize(); ++i) {
        TObjString* ss = (TObjString*)list->At(i);
	std::string sss = ss->String().Data();
	//KM: if it doesn't find "<" in the string, it starts from non-space character
	int posi = sss.find('<')!=std::string::npos ? sss.find('<') : sss.find_first_not_of(" ");
	std::string tmp = sss.erase(0,posi);
	//std::cout<<tmp<<std::endl;
	iss << tmp.data();	//iss << sss.Data();
      }
      
      m_calibrationTool->storeStr(m_taggerNameBase, alias, m_taggerNameBase+"Calib",iss.str());
    }
    
    if (!m_calibrationTool->updatedTagger(m_taggerNameBase, alias, m_taggerNameBase+"Calib") ) {
      std::string str = m_calibrationTool->getStr(m_taggerNameBase, alias, m_taggerNameBase+"Calib");
      // now configure the TMVAReader:
      // check if the reader for this tagger needs update
      tmvaReader = new TMVA::Reader();
      tmvaReader->AddVariable("ip3", &m_ip3);
      tmvaReader->AddVariable("sv1", &m_sv1);
      tmvaReader->AddVariable("jfc", &m_jfc);
      tmvaReader->AddVariable("categ(pt,eta)", &m_cat);
      TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kMLP, str.data() );//iss.str().data() );
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
      
      m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase,alias, m_taggerNameBase+"Calib", false);
    }

    /* retrieveing weights: */
    double jpt = jetToTag->pt();
    double eta = jetToTag->eta();
    double ip3=-9999, sv1=-9999, jfc=-9999;
    if( !BTag->loglikelihoodratio(m_inputIP3DWeightName, ip3) ){
        ATH_MSG_WARNING("#BTAG# tagger weight not found for  " << m_inputIP3DWeightName);
    }
    if( !BTag->loglikelihoodratio(m_inputSV1WeightName, sv1) ){
      ATH_MSG_WARNING("#BTAG# tagger weight not found for  " << m_inputSV1WeightName);
    }
    if( !BTag->loglikelihoodratio(m_inputJetFitterWeightName, jfc) ){
      ATH_MSG_WARNING("#BTAG# tagger weight not found for  " << m_inputJetFitterWeightName);
    }

    /* compute MV1: */
    double mv1 = -10.;
    int categ = this->jetCategory(jpt, eta);
    m_ip3 = ip3;
    m_sv1 = sv1;
    m_jfc = jfc;
    m_cat = (double)categ;
    pos = m_tmvaReaders.find(alias);
    if(pos==m_tmvaReaders.end()) {
      int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
      if(0==alreadyWarned) {
        ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << alias);
        m_undefinedReaders.push_back(alias);
      }
    } else {
      std::map<std::string, TMVA::MethodBase*>::iterator itmap2 = m_tmvaMethod.find(alias);
      if((itmap2->second)!=0){
	mv1 = pos->second->EvaluateMVA( itmap2->second );//("MLP_ANN");
      }
      else ATH_MSG_WARNING("#BTAG#  kl==0"); 
    }

    ATH_MSG_VERBOSE("#BTAG# pT=" << jpt << " eta=" << eta << " categ=" << categ << 
                    " IP3D=" << ip3 << " SV1=" << sv1 << " JFC=" << jfc << " --> " 
                    << m_taggerName << "=" << mv1);

    /** give information to the info class. */
    if(m_runModus=="analysis") {
      if (m_xAODBaseName == "MV1") // just to be clear, specify enum explicitely
	{
	  BTag->setMV1_discriminant(mv1);
	}
      else{
	BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv1);
      }

    }


    return StatusCode::SUCCESS;

  }

  /** helper functions to define the jet category: */
  static const int njptbin = 11;
  static const double jptbins[njptbin] = { 15., 30., 45., 60., 100., 140., 180., 220., 260., 310., 500. };
  std::vector<double> vptbins(jptbins,jptbins+njptbin);
  static const int netabin = 5;
  static const double etabins[netabin] = { 0., 0.6, 1.2, 1.8, 2.5 };
  std::vector<double> vetabins(etabins,etabins+netabin);

  int MV1Tag::findPtBin(double pt) const {
    std::vector<double>::iterator pos = std::lower_bound(vptbins.begin(), 
                                                         vptbins.end(), pt);
    return (pos-vptbins.begin());
  }
  int MV1Tag::findEtaBin(double eta) const {
    std::vector<double>::iterator pos = std::lower_bound(vetabins.begin(), 
                                                         vetabins.end(), eta);
    return (pos-vetabins.begin());
  }

  int MV1Tag::jetCategory(double pt, double eta) const {
    int cat = 0;
    double npt = pt/GeV; // input in MeV, internally in GeV
    double neta = fabs(eta)+0.00000001; // to get same results for boundaries as Root FindBin()
    int binx = this->findPtBin(npt);
    int biny = this->findEtaBin(neta);
    cat = binx + (njptbin+1)*biny;
    return cat;
  }


}//end namespace



