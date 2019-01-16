/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaIsolationSelectorTool
 * Authors:
 *      Joao Victor Pinto    <jodafons@cern.ch>
 *      Giuseppe Glerner     <glerner@cern.ch
 * Date: Sep 2017
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaIsolationSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaIsolationSelectorTool::
TrigEgammaIsolationSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("EtConeSizes"                , m_EtConeSizes = 6              );
  declareProperty("RelEtConeCut"               , m_RelEtConeCut                 );
  declareProperty("EtConeCut"                  , m_EtConeCut                    ); 
  declareProperty("PtConeSizes"                , m_PtConeSizes = 6              );
  declareProperty("RelPtConeCut"               , m_RelPtConeCut                 );
  //declareProperty("RelPtConeCut_var"           , m_RelPtConeCut_var             );
  declareProperty("PtConeCut"                  , m_PtConeCut                    );
  declareProperty("UseClusETforCaloIso"        , m_useClusETforCaloIso = true   );
  declareProperty("UseClusETforTrackIso"       , m_useClusETforTrackIso = true  );
  declareProperty("TrackIsolationTool",       m_trackIsolationTool,   "Handle of the track IsolationTool");  
  declareProperty("UseTrackIsolationTool",       m_useTrackIsolationTool= false,   "If false, use track isolation in xAOD. If True will compute track isolation with TrackIsolationTool");  
  
}
//**********************************************************************
StatusCode TrigEgammaIsolationSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
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
  m_mapEtCone.insert(   std::pair<int, string>(3, "topoetcone20")         );
  m_mapEtCone.insert(   std::pair<int, string>(4, "topoetcone30")         );
  m_mapEtCone.insert(   std::pair<int, string>(5, "topoetcone40")         );

  m_mapPtCone.insert(   std::pair<int, string>(0, "ptcone20")             );
  m_mapPtCone.insert(   std::pair<int, string>(1, "ptcone30")             );
  m_mapPtCone.insert(   std::pair<int, string>(2, "ptcone40")             );
  m_mapPtCone.insert(   std::pair<int, string>(3, "ptvarcone20")          );
  m_mapPtCone.insert(   std::pair<int, string>(4, "ptvarcone30")          );
  m_mapPtCone.insert(   std::pair<int, string>(5, "ptvarcone40")          );

  m_mapRelEtCone.insert(std::pair<int, string>(0, "etcone20/ele_pt")      );
  m_mapRelEtCone.insert(std::pair<int, string>(1, "etcone30/ele_pt")      );
  m_mapRelEtCone.insert(std::pair<int, string>(2, "etcone40/ele_pt")      );
  m_mapRelEtCone.insert(std::pair<int, string>(3, "topoetcone20/ele_pt")  );
  m_mapRelEtCone.insert(std::pair<int, string>(4, "topoetcone30/ele_pt")  );
  m_mapRelEtCone.insert(std::pair<int, string>(5, "topoetcone40/ele_pt")  );

  m_mapRelPtCone.insert(std::pair<int, string>(0, "ptcone20/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(1, "ptcone30/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(2, "ptcone40/ele_pt")      );
  m_mapRelPtCone.insert(std::pair<int, string>(3, "ptvarcone20/ele_pt")   );
  m_mapRelPtCone.insert(std::pair<int, string>(4, "ptvarcone30/ele_pt")   );
  m_mapRelPtCone.insert(std::pair<int, string>(5, "ptvarcone40/ele_pt")   );

  // Lets initialize trackIsolationTool
  if (!m_trackIsolationTool.empty()){       
      ATH_MSG_DEBUG("Retrieve TrackIsolationTool");
      if(m_trackIsolationTool.retrieve().isFailure()){
	  ATH_MSG_ERROR("Unable to retrieve " << m_trackIsolationTool);
	  return StatusCode::FAILURE;
      } else {      
	  ATH_MSG_DEBUG("Retrieved Tool "<<m_trackIsolationTool);
      }         
  }


  ATH_MSG_INFO("TrigEgammaIsolationSelectorTool initialised");

  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaIsolationSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaIsolationSelectorTool::emulation(const xAOD::IParticle* part, bool &pass, const Trig::Info &/*info*/)
{
  pass=false;
  if (!part) {
    ATH_MSG_WARNING("emulateIsolation() \t particle is NULL");
    return false;
  }

  const xAOD::Electron *el = static_cast<const xAOD::Electron *>(part);
  // Try to use *here* the trackIsolationTool




  //ATH_MSG_DEBUG("ElectronContainer size: "<<elContainer->size());

  //if(el->trackParticle()){
  //  float Z0SinTheta = std::fabs(el->trackParticle()->z0()*std::sin(el->trackParticle()->theta()));
  //  if(Z0SinTheta > m_maxZ0SinTheta){
  //    ATH_MSG_DEBUG("emulateIsolation()->Z0SinTheta = " << Z0SinTheta << " > " << m_maxZ0SinTheta);
  //    return true;
  //  }else{
  //    ATH_MSG_DEBUG("emulateIsolation()->Z0SinTheta = " << Z0SinTheta);
  //  }
  //}

  // Declare vectors of isolation variables for different cone sizes
  std::vector<float>  EtCone, PtCone_xAOD, PtCone, recalc_PtCone, recalc_PtVarCone, PtCone_recalculated; //, PtCone_recalculated_fixed;
  float val=-99;
  el->isolationValue(val,xAOD::Iso::etcone20);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone20) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone30);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone30) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone40);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::etcone40) = " << val);
  EtCone.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone20);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone20) = " << val);
  PtCone_xAOD.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone30);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone30) = " << val);
  PtCone_xAOD.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone40);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptcone40) = " << val);
  PtCone_xAOD.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone20);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvarcone20) = " << val);
  PtCone_xAOD.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone30);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvatcone30) = " << val);
  PtCone_xAOD.push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone40);
  //ATH_MSG_DEBUG("el->isolationValue(val,xAOD::Iso::ptvarcone40) = " << val);
  PtCone_xAOD.push_back(val);

  // OK, now we have the ptcone values in PtCone_xAOD. Lets compute the same, but by using the TrackIsolationTool:
  // Using code as here:
  //  https://gitlab.cern.ch:8443/atlas/athena/blob/21.0/Reconstruction/RecoTools/IsolationTool/util/test_isolaitonTool.cxx
  vector<xAOD::Iso::IsolationType> isoTypes;
  isoTypes.push_back(xAOD::Iso::ptcone40);
  isoTypes.push_back(xAOD::Iso::ptcone30);
  isoTypes.push_back(xAOD::Iso::ptcone20);

  xAOD::TrackCorrection corrlist;
  corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
 

 

  // Compute the track isolation with trackIsolationTool
  // But *before* we need to define the vertex as it is in TrigEgammaRec as the vertex of the ighest pT track in the cone.... Otherwise we can try with that of the electron candidate... ?

  //const xAOD::TrackParticleContainer* pTrackParticleContainer = 0;
  xAOD::Vertex *leadTrkVtx = new xAOD::Vertex();     
  leadTrkVtx->makePrivateStore();

  leadTrkVtx->setZ(el->trackParticle()->z0() + el->trackParticle()->vz());

  //const ElementLink< TrackParticleContainer* > link  =  el->trackParticleLink( /*0 this is the index of trackparticle*/ );
  //then I think it is 
  // std::str ld = link.dataID();
  std::string trk_container =  el->trackParticleLink().dataID();
  ATH_MSG_DEBUG("TRacks linked to " <<  trk_container << " container"); 
  ////that @brief Return the SG key that we reference, as a string.
  //
  const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(el, false);
  ATH_MSG_DEBUG("tracksToExclude size = " << tracksToExclude.size() );



  const xAOD::TrackParticleContainer *TrackParticleContainer=0;

  if ( (m_storeGate->retrieve(TrackParticleContainer,trk_container)).isFailure() ){
      ATH_MSG_ERROR("Failed to retrieve EF ID trackParticles ");
      return false;
  } else {
      ATH_MSG_DEBUG("Retrieved EF ID trackParticles "  << trk_container << " of size " << TrackParticleContainer->size());
  }

  std::vector<const xAOD::TrackParticleContainer*> vectorTrackParticleContainer;
  vectorTrackParticleContainer.push_back(TrackParticleContainer);

  xAOD::TrackIsolation result;
  recalc_PtCone.clear();
  recalc_PtVarCone.clear();
  if(m_trackIsolationTool->trackIsolation(result, *el, isoTypes, corrlist, leadTrkVtx, &tracksToExclude, TrackParticleContainer)){
      for(unsigned int i=0; i<isoTypes.size(); i++){
	  recalc_PtCone.push_back(result.ptcones[i]);
	  recalc_PtVarCone.push_back(result.ptvarcones_10GeVDivPt[i]);
      }
  } else {
      ATH_MSG_ERROR("Hey!!!! Something wrong! Couldn't run TrackIsolationTool " << m_trackIsolationTool);
  }


  // So recalc_PtCone has the result of ptcone40,30,20 and recalc_PtVarCone result of ptvarcone40,30,20 (requested in this order by the tool). So lets build PtCone_recalculated to have (as PtCone) results of ptcone20 30 40 and ptvarcone20 30 40

  PtCone_recalculated.push_back( recalc_PtCone[2]    );
  PtCone_recalculated.push_back( recalc_PtCone[1]    );
  PtCone_recalculated.push_back( recalc_PtCone[0]    );
  PtCone_recalculated.push_back( recalc_PtVarCone[2] );
  PtCone_recalculated.push_back( recalc_PtVarCone[1] );
  PtCone_recalculated.push_back( recalc_PtVarCone[0] );

  if (m_useTrackIsolationTool){
      // If we decide to use rRackIsolation Tool then instead of stored ptCone we will use this recalculated
      ATH_MSG_DEBUG("Using Track Isolation Tool. Cutting on Recalculated track isolation");
      PtCone = PtCone_recalculated;
  } else {
      ATH_MSG_DEBUG("Not Using Track Isolation Tool. Cutting on xAOD track isolation");
      PtCone = PtCone_xAOD;
  }

  // Lets try a hack... if the recomputed ptcone is larger than the sum of pt of exclude tracks, then, remove them
  float pt_tracks_to_exclude=0.;
  for (auto trk: tracksToExclude){
      pt_tracks_to_exclude+= trk->pt();
  }


  // for (std::size_t iConeSize=0; iConeSize<PtCone_recalculated.size(); iConeSize++){
  //     // In this hack, if for some reason the isolation computation didn't work, the ptcone computes ptcone *including* tracks that should have been excluded
  //     //if (PtCone_recalculated[iConeSize] > el->trackParticle()->pt()){
  //         PtCone_recalculated_fixed.push_back(PtCone_recalculated[iConeSize] - pt_tracks_to_exclude);
  //     //}else{
  //     //    PtCone_recalculated_fixed.push_back(PtCone_recalculated[iConeSize]);
  //     //}
  // }


  // Lets cut on PtCone_recalculated_fixed Isntead, and show the difference betweenPtCone_recalculated (from tool) and PtCone(from xAOD):
  std::size_t iConeSize = 3;
  ATH_MSG_DEBUG("TrackIsolationTool vs xAOD value at " << iConeSize << "  PtCone_recalculated = " << PtCone_recalculated[iConeSize] << /*" PtCone_recalculated_fixed = " << PtCone_recalculated_fixed[iConeSize] <<*/ "  PtCone_xAOD = " << PtCone_xAOD[iConeSize] << "  TiT/xAOD = "  << PtCone_recalculated[iConeSize]/PtCone_xAOD[iConeSize] << " and el_trk_pt = " << el->trackParticle()->pt());





    


  bool absEtConeCut_ispassed = true;

  for(std::size_t iConeSize = 0; iConeSize < EtCone.size(); iConeSize++) { 
    if( ( m_EtConeCut[iConeSize] > 0.) && (EtCone[iConeSize] > m_EtConeCut[iConeSize])) { 
      ATH_MSG_DEBUG("Absolute Calo Isolation " << m_mapEtCone[iConeSize] << " NOT satisfied: "<< EtCone[iConeSize] << " > cut: " << m_EtConeCut[iConeSize]); 
      absEtConeCut_ispassed = false; 
      break; //skip remaining etcone sizes if one fails
    }
  }  
  //if one isolation cut fails on one egamma object go to next egamma object
  if(!absEtConeCut_ispassed) return true; 

  
  // Cut on Absolute Track Isolation
  bool absPtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < PtCone_recalculated.size(); iConeSize++) {
    if( ( m_PtConeCut[iConeSize] > 0.) && (PtCone_recalculated[iConeSize] > m_PtConeCut[iConeSize])) {
        ATH_MSG_DEBUG("Absolute Track Isolation " << m_mapPtCone[iConeSize] << " NOT satisfied: "<< PtCone_recalculated[iConeSize] << " > cut: " << m_PtConeCut[iConeSize]);
      absPtConeCut_ispassed = false;
      break;//skip remaining ptcone sizes if one fails
    }
  }
  
  //if one isolation cut fails on one egamma object go to next egamma object
  if(!absPtConeCut_ispassed) return true;

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
    else RelEtCone.push_back(99990.);
  }
 
  // Here use PtCone for xAOD values and PtCone_recalulated if using track isolation tool 
  for (std::size_t iConeSize = 0; iConeSize < PtCone.size(); iConeSize++){
    if(trkIso_ele_pt > 0.) RelPtCone.push_back(PtCone[iConeSize]/trkIso_ele_pt);
    else RelPtCone.push_back(99990.);
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
  if(!relEtConeCut_ispassed) return true;
 
  // ivarloose parameters
  std::vector<float> RelPtConeCut;
  //if(!ivarloose){
  //  ATH_MSG_DEBUG("iloose cut enabled");
  //  RelPtConeCut=m_RelPtConeCut;
  //}else{
  //  ATH_MSG_DEBUG("ivarloose cut enabled");
  RelPtConeCut=m_RelPtConeCut;
  //}  

  // Cut on Relative Track Isolation
  bool relPtConeCut_ispassed = true;
  for(std::size_t iConeSize = 0; iConeSize < RelPtCone.size(); iConeSize++) {
    if( ( RelPtConeCut[iConeSize] > 0.) && (RelPtCone[iConeSize] > RelPtConeCut[iConeSize])) {
      ATH_MSG_DEBUG("REGTEST Relative Track Isolation " << m_mapRelPtCone[iConeSize]
                    << " NOT satisfied: "<< RelPtCone[iConeSize] << " > cut: " << RelPtConeCut[iConeSize]);
      relPtConeCut_ispassed = false;
      break;//skip remaining ptcone sizes if one fails
    } else {
      ATH_MSG_DEBUG("REGTEST Relative Track Isolation " << m_mapRelPtCone[iConeSize]
                    << " SATISFIED: "<< RelPtCone[iConeSize] << " > cut: " << RelPtConeCut[iConeSize]);
    }
  }

  //if one isolation cut fails on one egamma object go to next egamma object
  if(!relPtConeCut_ispassed) return true;

  pass=true;
  ATH_MSG_DEBUG("emulateIsolation() \t isIsolation = " << pass);

  ATH_MSG_DEBUG( "End of emulation method in TrigEgammaIsolationSelectorTool" );

  return true;
}


