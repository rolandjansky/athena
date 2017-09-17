/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


/**********************************************************************
 * AsgTool: TrigEgammaEFElectronSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaEFElectronSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFElectronSelectorTool::
TrigEgammaEFElectronSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  //declareProperty("TrackIsolationTool"         , m_trackIsolationTool   );
  declareProperty("ElectronOnlPPSelector"      , m_electronOnlIsEMTool          );
  declareProperty("ElectronOnlLHSelector"      , m_electronOnlLHTool            );
  declareProperty("ElectronTrigOnlLHSelector"  , m_electronTrigOnlLHTool        );
  declareProperty("LikelihoodInfo"             , m_lhinfo = ""                  );
  declareProperty("EtConeSizes"                , m_EtConeSizes = 3              );
  declareProperty("RelEtConeCut"               , m_RelEtConeCut                 );
  declareProperty("EtConeCut"                  , m_EtConeCut                    ); 
  declareProperty("PtConeSizes"                , m_PtConeSizes = 6              );
  declareProperty("RelPtConeCut"               , m_RelPtConeCut                 );
  declareProperty("RelPtConeCut_var"           , m_RelPtConeCut_var             );
  declareProperty("PtConeCut"                  , m_PtConeCut                    );
  declareProperty("UseClusETforCaloIso"        , m_useClusETforCaloIso = true   );
  declareProperty("UseClusETforTrackIso"       , m_useClusETforTrackIso = true  );
}
//**********************************************************************
StatusCode TrigEgammaEFElectronSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }
  if (m_electronOnlIsEMTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("retrieve failed for tools");
    return StatusCode::FAILURE;
  }
  if (m_electronOnlLHTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("retrieve failed for tools");
    return StatusCode::FAILURE;
  }
  if ( m_EtConeCut.size() != m_EtConeSizes ) {
    ATH_MSG_ERROR(" m_EtConeCut size is " <<  m_EtConeCut.size() << " but needs " << m_EtConeSizes);
    return StatusCode::FAILURE;
  }
  if ( m_RelEtConeCut.size() != m_EtConeSizes ) {
    ATH_MSG_ERROR(" m_RelEtConeCut size is " <<  m_RelEtConeCut.size() << " but needs " << m_EtConeSizes);
    return StatusCode::FAILURE;
  }
  if ( m_PtConeCut.size() != m_PtConeSizes ) {
    ATH_MSG_ERROR(" m_PtConeCut size is " <<  m_PtConeCut.size() << " but needs " << m_PtConeSizes);
    return StatusCode::FAILURE;
  }
  if ( m_RelPtConeCut.size() != m_PtConeSizes ) {
    ATH_MSG_ERROR(" m_RelPtConeCut size is " <<  m_RelPtConeCut.size() << " but needs " << m_PtConeSizes);
    return StatusCode::FAILURE;
  }


  // Define mapping between vector of Isolation Cone Sizes and variable names
  m_mapEtCone.insert(   std::pair<int, string>(0, "etcone20")             );
  m_mapEtCone.insert(   std::pair<int, string>(1, "etcone30")             );
  m_mapEtCone.insert(   std::pair<int, string>(2, "etcone40")             );
  m_mapPtCone.insert(   std::pair<int, string>(0, "ptcone20")             );
  m_mapPtCone.insert(   std::pair<int, string>(1, "ptcone30")             );
  m_mapPtCone.insert(   std::pair<int, string>(2, "ptcone40")             );
  m_mapPtCone.insert(   std::pair<int, string>(3, "ptvarcone20")          );
  m_mapPtCone.insert(   std::pair<int, string>(4, "ptvarcone30")          );
  m_mapPtCone.insert(   std::pair<int, string>(5, "ptvarcone40")          );
  m_mapRelEtCone.insert(std::pair<int, string>(0, "etcone20/ele_pt")      );
  m_mapRelEtCone.insert(std::pair<int, string>(1, "etcone30/ele_pt")      );
  m_mapRelEtCone.insert(std::pair<int, string>(2, "etcone40/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(0, "ptcone20/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(1, "ptcone30/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(2, "ptcone40/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(3, "ptvarcone20/ele_pt")   );
  m_mapRelPtCone.insert(std::pair<int, string>(4, "ptvarcone30/ele_pt")   );
  m_mapRelPtCone.insert(std::pair<int, string>(5, "ptvarcone40/ele_pt")   );

  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaEFElectronSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::emulation(const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info)
{
  pass=false;

  // checking when LH trigger, must be check if the lhinfo match with the trigger name
  if(info.lhinfo != m_lhinfo){
    ATH_MSG_DEBUG("This selector does not  match with the likelihood information [ " << info.lhinfo << " ]");
    return false;
  }
  if(!container){
    ATH_MSG_WARNING("Can not emulate EF electron step. Container is null");
    return false;
  }

  const xAOD::ElectronContainer *elContainer = static_cast<const xAOD::ElectronContainer *>(container);
  ATH_MSG_DEBUG("ElectronContainer size: "<<elContainer->size());

  if (boost::contains(info.pidname,"1")) return false; // Not emulating Run1

  boost::dynamic_bitset<> bitAccept(elContainer->size());
  unsigned bit = 0;
  float etthr  = info.thrHLT;
  auto avgmu = getOnlAverageMu();

  for(const auto& el : *elContainer){
    bit++;
    //emulate electron cuts
    if (info.idperf  || info.etcut || info.perf ) {
      //ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      //if (el->caloCluster()->et()/1.e3 < etthr) continue;
      bitAccept.set(bit-1,true);
    }
    else if (info.isolation || info.hltcalo) {
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr) continue;
      if (!ApplyElectronPid(el,info.pidname,avgmu)) continue;
      ATH_MSG_DEBUG("Isolation");
      bool ivarloose=false;
      if(boost::contains(info.trigName, "ivarloose"))
        ivarloose=true;
      // Apply isolation
      if(!ApplyIsolation(el,ivarloose))  continue;
      bitAccept.set(bit-1,true);
    }
    else {
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr)  continue;
      //ATH_MSG_INFO("Original LHValue: " << el->likelihoodValue("LHCaloValue"));
      if (!ApplyElectronPid(el,info.pidname,avgmu)) continue;
      ATH_MSG_DEBUG("Apply Pid " << info.pidname);
      bitAccept.set(bit-1,true);
    }
    dressDecision(el, info.trigName , pass );
  }//loop over electrons

  if(bitAccept.count() >0)  pass=true;
  return true;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname, 
                                                        double avgmu) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  eg->passSelection(passSel,pidname);
  if (pidname == "Tight") {
    passTool = m_electronOnlIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_electronOnlIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_electronOnlIsEMTool[2]->accept(eg);
  }
  else if (pidname == "VLoose") {
    passTool = m_electronOnlIsEMTool[3]->accept(eg);
  }
  else if (pidname == "LHTight") {
    passTool = m_electronOnlLHTool[0]->accept(eg,avgmu);
  }// Tight
  else if (pidname == "LHMedium") {
    passTool = m_electronOnlLHTool[1]->accept(eg,avgmu);
  }// Medium
  else if (pidname == "LHLoose") {
    passTool = m_electronOnlLHTool[2]->accept(eg,avgmu);
  }// Loose
  else if (pidname == "LHVLoose") {
    passTool = m_electronOnlLHTool[3]->accept(eg,avgmu);
  }// VeryLoose
  else {
    ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return true;
  }

  if (passTool != passSel) {
    ATH_MSG_DEBUG("Apply Pid ERROR in Tool and eg->passSelection is " << passSel << " and ApplyElectronCaloPid is " << passTool);
  }

  return passTool;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::ApplyIsolation(const xAOD::Electron *el, bool ivarloose=false ) {

  bool isIsolated = false;

  if (!el) {
    ATH_MSG_WARNING("emulateIsolation() \t Electron is NULL");
    return isIsolated;
  }

  // Declare vectors of isolation variables for different cone sizes
  std::vector<float>  EtCone, PtCone;     
  float val=-99;
  el->isolationValue(val,xAOD::Iso::etcone20);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone20) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone30);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone30) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone40);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone40) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone20);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone20) = " << val);
  PtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone30);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone30) = " << val);
  PtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone40);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone40) = " << val);
  PtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone20);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvarcone20) = " << val);
  PtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone30);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvatcone30) = " << val);
  PtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone40);
  ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvarcone40) = " << val);
  PtCone.push_back(val);


  bool absEtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) {
    if( ( m_EtConeCut[iConeSize] > 0.) && (EtCone[iConeSize] > m_EtConeCut[iConeSize])) {
        ATH_MSG_DEBUG("Absolute Calo Isolation " << m_mapEtCone[iConeSize] << " NOT satisfied: "<< EtCone[iConeSize]
                      << " > cut: " << m_EtConeCut[iConeSize]);
      absEtConeCut_ispassed = false;
      break;//skip remaining etcone sizes if one fails
    }
  }

  //if one isolation cut fails on one egamma object go to next egamma object
  if(!absEtConeCut_ispassed) return isIsolated;
 
  // Cut on Absolute Track Isolation
  bool absPtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++) {
    if( ( m_PtConeCut[iConeSize] > 0.) && (PtCone[iConeSize] > m_PtConeCut[iConeSize])) {
        ATH_MSG_DEBUG("Absolute Track Isolation " << m_mapPtCone[iConeSize] << " NOT satisfied: "<< PtCone[iConeSize]
                      << " > cut: " << m_PtConeCut[iConeSize]);
      absPtConeCut_ispassed = false;
      break;//skip remaining ptcone sizes if one fails
    }
  }
  
  //if one isolation cut fails on one egamma object go to next egamma object
  if(!absPtConeCut_ispassed) return isIsolated;

  // Relative isolation
  std::vector<float>  RelEtCone, RelPtCone;
  // Check that CaloCluster exists, if so use cluster ET as Denonimator in Relative Isolation
  float caloIso_ele_pt=-9999.;
  float trkIso_ele_pt=-9999.;
  float ele_clus_pt=-9999.;
  float ele_trk_pt=-9999.;

  // Cluster must exist set the et from cluster
  ele_clus_pt=el->caloCluster()->et();

  // Check that TrackParticle exists, if so use track ET as Denonimator in Relative Isolation
  if(!(el->trackParticle())) {
    ATH_MSG_DEBUG("TrackParticle does NOT Exist, do NOT use Electron Track PT as Denominator in Relative Isolation");
  }else{
    ATH_MSG_DEBUG("TrackParticle Exists, may use Electron Track PT as denominator in relative Isolation varariables");
    ele_trk_pt = el->trackParticle()->pt();
  }

  // Define the electron pt variable to use as denominator in Relative Isolation
  if(m_useClusETforCaloIso) {
    caloIso_ele_pt=ele_clus_pt;   
    ATH_MSG_DEBUG("For Relative Calo Isolation use ele_clus_pt = " << caloIso_ele_pt);
  }
  else {
    caloIso_ele_pt=ele_trk_pt;
    ATH_MSG_DEBUG("For Relative Calo Isolation use ele_trk_pt = " << caloIso_ele_pt);
  }

  if(m_useClusETforTrackIso) {
    trkIso_ele_pt=ele_clus_pt;
   ATH_MSG_DEBUG("For Relative Track Isolation use ele_clus_pt = " << trkIso_ele_pt );
  }
  else {
    trkIso_ele_pt=ele_trk_pt;
    ATH_MSG_DEBUG("For Relative Track Isolation use ele_trk_pt = " << trkIso_ele_pt );
  }

  // Fill vectors of Relative isolation variables for different cone sizes
  for (std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++){
    if(caloIso_ele_pt > 0.) RelEtCone.push_back(EtCone[iConeSize]/caloIso_ele_pt);
    else RelEtCone.push_back(0.);
  }
  
  for (std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++){
    if(trkIso_ele_pt > 0.) RelPtCone.push_back(PtCone[iConeSize]/trkIso_ele_pt);
    else RelPtCone.push_back(0.);
  }
 
  // Cut on Relative Calo Isolation
  bool relEtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < RelEtCone.size(); iConeSize++) {
    //NB: -ve values in cut means DO NOT CUT
    if( ( m_RelEtConeCut[iConeSize] > 0.) && (RelEtCone[iConeSize] > m_RelEtConeCut[iConeSize])) {
        ATH_MSG_DEBUG("Relative Calo Isolation " << m_mapRelEtCone[iConeSize] << " NOT satisfied: "
                      << RelEtCone[iConeSize] << " > cut: " << m_RelEtConeCut[iConeSize]);
      relEtConeCut_ispassed = false;
      break;//skip remaining etcone sizes if one fails
    }
  }
 
  //if one isolation cut fails on one egamma object go to next egamma object
  if(!relEtConeCut_ispassed) return isIsolated;
 
  // ivarloose parameters
  std::vector<float> RelPtConeCut;
  if(!ivarloose){
    ATH_MSG_DEBUG("iloose cut enabled");
    RelPtConeCut=m_RelPtConeCut;
  }else{
    ATH_MSG_DEBUG("ivarloose cut enabled");
    RelPtConeCut=m_RelPtConeCut_var;
  }  

  // Cut on Relative Track Isolation
  bool relPtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < RelPtCone.size(); iConeSize++) {
    if( ( RelPtConeCut[iConeSize] > 0.) && (RelPtCone[iConeSize] > RelPtConeCut[iConeSize])) {
      ATH_MSG_DEBUG("REGTEST Relative Track Isolation " << m_mapRelPtCone[iConeSize]
                    << " NOT satisfied: "<< RelPtCone[iConeSize] << " > cut: " << RelPtConeCut[iConeSize]);
      relPtConeCut_ispassed = false;
      break;//skip remaining ptcone sizes if one fails
    }
  }

  //if one isolation cut fails on one egamma object go to next egamma object
  if(!relPtConeCut_ispassed) return isIsolated;

  isIsolated=true;
  ATH_MSG_DEBUG("emulateIsolation() \t isIsolation = " << isIsolated);
  return isIsolated;
}
//!==========================================================================

