/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/MV1cTag.h"

#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/Jet.h"
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
      @class MV1cTag 
      Neural-net based tagger combining IP3D, SV1 and JetFitterCombNN likelihoods
      @author M. Ughetto
  */

  MV1cTag::MV1cTag(const std::string& t, const std::string& n, const IInterface* p)
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
    declareProperty("taggerNameBase", m_taggerNameBase = "MV1c");
    declareProperty("taggerName", m_taggerName = "MV1c");
  }


  MV1cTag::~MV1cTag() {
  }


  StatusCode MV1cTag::initialize() {
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


  StatusCode MV1cTag::finalize() {
    // delete readers:
    std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
    for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
    std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
    for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    return StatusCode::SUCCESS;
  }


  StatusCode MV1cTag::tagJet(xAOD::Jet& myJet, xAOD::BTagging* BTag) {

    /* jet author: */
    std::string author = JetTagUtils::getJetAuthor(myJet);
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
        ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for MV1c");
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

      // now configure the TMVAReader:
      /// check if the reader for this tagger needs update
      TMVA::Reader* tmvaReader = new TMVA::Reader();
      tmvaReader->AddVariable("ip3", &m_ip3);
      tmvaReader->AddVariable("sv1", &m_sv1);
      tmvaReader->AddVariable("jfc_pu", &m_jfc_pu);
      tmvaReader->AddVariable("jfc_pc", &m_jfc_pc);
      tmvaReader->AddVariable("jfc_pb", &m_jfc_pb);
      tmvaReader->AddVariable("categ(pt,eta)", &m_cat);
      TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kMLP, iss.str().data() );
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

    /* retrieveing weights: */
    double jpt = myJet.pt();
    double eta = myJet.eta();
    double ip3=-9999, sv1=-9999;
    BTag->loglikelihoodratio(m_inputIP3DWeightName, ip3);
    BTag->loglikelihoodratio(m_inputSV1WeightName, sv1);
    double jfc_pb=-1,jfc_pu=-1,jfc_pc=-1;
    if("JetFitterCombNN"==m_inputJetFitterWeightName){
      jfc_pb=BTag->JetFitterCombNN_pb();
      jfc_pu=BTag->JetFitterCombNN_pu();
      jfc_pc=BTag->JetFitterCombNN_pc();
    }
    else if ("JetFitter"==m_inputJetFitterWeightName){
      jfc_pb=BTag->JetFitter_pb();
      jfc_pu=BTag->JetFitter_pu();
      jfc_pc=BTag->JetFitter_pc();
    } 
    else {
      BTag->variable<double>(m_inputJetFitterWeightName, "pu", jfc_pu);
      BTag->variable<double>(m_inputJetFitterWeightName, "pb", jfc_pb);
      BTag->variable<double>(m_inputJetFitterWeightName, "pc", jfc_pc);
    }
   
    /* compute MV1c: */
    double mv1c = -10.;
    int categ = this->jetCategory(jpt, eta);
    m_ip3 = ip3;
    m_sv1 = sv1;
    m_jfc_pb = jfc_pb;
    m_jfc_pu = jfc_pu;
    m_jfc_pc = jfc_pc;
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
      if((itmap2->second)!=0) {
	mv1c = pos->second->EvaluateMVA( itmap2->second );//("MLP_ANN");
      }
      else ATH_MSG_WARNING("#BTAG#  kl==0");
    }

    ATH_MSG_VERBOSE("#BTAG# pT=" << jpt << " eta=" << eta << " categ=" << categ << 
                    " IP3D=" << ip3 << " SV1=" << sv1 << " JFC_pb=" << jfc_pb << " JFC_pu=" << jfc_pu <<  " JFC_pc=" << jfc_pc << " --> " 
                    << m_taggerName << "=" << mv1c);

    /** give information to the info class. */
    if(m_runModus=="analysis") {
      BTag->setVariable<double>(m_xAODBaseName, "discriminant", mv1c);
    }

    return StatusCode::SUCCESS;

  }

  /** helper functions to define the jet category: */
  static const int njMV1cptbin = 11;
  static const double _jMV1cptbins[njMV1cptbin] = { 15., 30., 45., 60., 100., 140., 180., 220., 260., 310., 500. };
  std::vector<double> _vMV1cptbins(_jMV1cptbins,_jMV1cptbins+njMV1cptbin);
  static const int nMV1cetabin = 5;
  static const double _etaMV1cbins[nMV1cetabin] = { 0., 0.6, 1.2, 1.8, 2.5 };
  std::vector<double> _vMV1cetabins(_etaMV1cbins,_etaMV1cbins+nMV1cetabin);

  int MV1cTag::findPtBin(double pt) const {
    std::vector<double>::iterator pos = std::lower_bound(_vMV1cptbins.begin(), 
                                                         _vMV1cptbins.end(), pt);
    return (pos-_vMV1cptbins.begin());
  }
  int MV1cTag::findEtaBin(double eta) const {
    std::vector<double>::iterator pos = std::lower_bound(_vMV1cetabins.begin(), 
                                                         _vMV1cetabins.end(), eta);
    return (pos-_vMV1cetabins.begin());
  }

  int MV1cTag::jetCategory(double pt, double eta) const {
    int cat = 0;
    double npt = pt/1000.; // input in MeV, internally in GeV
    double nMV1ceta = fabs(eta)+0.00000001; // to get same results for boundaries as Root FindBin()
    int binx = this->findPtBin(npt);
    int biny = this->findEtaBin(nMV1ceta);
    cat = binx + (njMV1cptbin+1)*biny;
    return cat;
  }


}//end namespace



