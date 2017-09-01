/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/ISF_HitAnalysis.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

// Section of includes for LAr calo tests
#include "LArSimEvent/LArHitContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GeoAdaptors/GeoLArHit.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Section of includes for tile calo tests
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "TileConditions/TileInfo.h"

#include "TileDetDescr/TileDetDescrManager.h"
#include "CaloIdentifier/TileID.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

//CaloCell
#include "CaloEvent/CaloCellContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TTree.h"
#include "TString.h"
#include "TVector3.h"
#include <sstream>
//#include "TLorentzVector.h"

// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"


//####################
#include "GaudiKernel/ListItem.h"
#include "CaloDetDescr/CaloDepthTool.h"
//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
//#########################


#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>


//const std::string MC_DIGI_PARAM = "/Digitization/Parameters";
//const std::string MC_SIM_PARAM = "/Simulation/Parameters";

ISF_HitAnalysis::ISF_HitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   //, m_storeGate(0)
   , m_geoModel(0)
   , m_tileInfo(0)
   , m_larEmID(0)
   , m_larFcalID(0)
   , m_larHecID(0)
   , m_tileID(0)
   , m_tileMgr(0)
   , m_hit_x(0)
   , m_hit_y(0)
   , m_hit_z(0)
   , m_hit_energy(0)
   , m_hit_time(0)
   , m_hit_identifier(0)
   , m_hit_cellidentifier(0)
   , m_islarbarrel(0)
   , m_islarendcap(0)
   , m_islarhec(0)
   , m_islarfcal(0)
   , m_istile(0)
   , m_hit_sampling(0)
   , m_hit_samplingfraction(0)
   , m_truth_energy(0)
   , m_truth_px(0)
   , m_truth_py(0)
   , m_truth_pz(0)
   , m_truth_pdg(0)
   , m_truth_barcode(0)
   , m_truth_vtxbarcode(0)
   , m_cell_identifier(0)
   , m_cell_energy(0)
   , m_cell_sampling(0)
   , m_g4hit_energy(0)
   , m_g4hit_time(0)
   , m_g4hit_identifier(0)
   , m_g4hit_cellidentifier(0)
   , m_g4hit_samplingfraction(0)
   , m_g4hit_sampling(0)
     //   , m_matched_cells(0)
   , m_tree(0)
   , m_ntupleFileName("/ntuples/file1")
   , m_ntupleDirName("ISF_HitAnalysis")
   , m_ntupleTreeName("CaloHitAna")
   , m_metadataTreeName("MetaData")
   , m_geoFileName("ISF_Geometry")
   , m_thistSvc(0)
   , m_calo_dd_man(0)
   //#####################
   , m_eta_calo_surf(0)
   , m_phi_calo_surf(0)
   , m_d_calo_surf(0)
   , m_ptruth_eta(0)
   , m_ptruth_phi(0)
   , m_ptruth_e(0)
   , m_ptruth_et(0)
   , m_ptruth_pt(0)
   , m_ptruth_p(0)
   , m_pdgid(0)

   //######################

/*
   , m_TTC_entrance_eta(0)
   , m_TTC_entrance_phi(0)
   , m_TTC_entrance_r(0)
   , m_TTC_entrance_z(0)
   , m_TTC_back_eta(0)
   , m_TTC_back_phi(0)
   , m_TTC_back_r(0)
   , m_TTC_back_z(0)
   , m_TTC_IDCaloBoundary_eta(0)
   , m_TTC_IDCaloBoundary_phi(0)
   , m_TTC_IDCaloBoundary_r(0)
   , m_TTC_IDCaloBoundary_z(0)
   , m_TTC_Angle3D(0)
   , m_TTC_AngleEta(0)
*/
   , m_newTTC_entrance_eta(0)
   , m_newTTC_entrance_phi(0)
   , m_newTTC_entrance_r(0)
   , m_newTTC_entrance_z(0)
   , m_newTTC_back_eta(0)
   , m_newTTC_back_phi(0)
   , m_newTTC_back_r(0)
   , m_newTTC_back_z(0)
   , m_newTTC_IDCaloBoundary_eta(0)
   , m_newTTC_IDCaloBoundary_phi(0)
   , m_newTTC_IDCaloBoundary_r(0)
   , m_newTTC_IDCaloBoundary_z(0)
   , m_newTTC_Angle3D(0)
   , m_newTTC_AngleEta(0)

   , m_caloEntrance(0)
   , m_calo_tb_coord(0)
   , m_sample_calo_surf(CaloCell_ID_FCS::noSample)
   , m_particleDataTable(0)

   ,m_MC_DIGI_PARAM("/Digitization/Parameters")
   ,m_MC_SIM_PARAM("/Simulation/Parameters")

   //######################


  //Note that m_xxx are pointers to vectors set to 0, not set to empty vector! see note around TBranch
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("GeoFileName", m_geoFileName);
  declareProperty("MetadataTreeName", m_metadataTreeName);
  declareProperty("NTruthParticles", m_NtruthParticles=1, "Number of truth particles saved from the truth collection, -1 to save all");

  declareProperty("FastCaloSimCaloExtrapolation",   m_FastCaloSimCaloExtrapolation );

  //###########################
  //declareProperty("ExtrapolatorName",               m_extrapolatorName );
  //declareProperty("ExtrapolatorInstanceName",       m_extrapolatorInstanceName );
  //declareProperty("CalosurfMiddleInstanceName",     m_calosurf_InstanceName);
  //declareProperty("CalosurfEntranceInstanceName",   m_calosurf_entrance_InstanceName);

  declareProperty("CaloBoundaryR", m_CaloBoundaryR);
  declareProperty("CaloBoundaryZ", m_CaloBoundaryZ);
  declareProperty("CaloMargin", m_calomargin);
  //######################

  declareProperty("Extrapolator",                   m_extrapolator );
  declareProperty("CaloEntrance",                   m_caloEntranceName );
  declareProperty("CaloSurfaceHelper",              m_caloSurfaceHelper );

  declareProperty("CaloGeometryHelper",             m_CaloGeometryHelper );

  declareProperty("MetaDataSim", m_MC_SIM_PARAM );
  declareProperty("MetaDataDigi", m_MC_DIGI_PARAM );

  m_surfacelist.resize(0);
  m_surfacelist.push_back(CaloCell_ID_FCS::PreSamplerB);
  m_surfacelist.push_back(CaloCell_ID_FCS::PreSamplerE);
  m_surfacelist.push_back(CaloCell_ID_FCS::EME1);
  m_surfacelist.push_back(CaloCell_ID_FCS::EME2);
  m_surfacelist.push_back(CaloCell_ID_FCS::FCAL0);

}


ISF_HitAnalysis::~ISF_HitAnalysis()
{
}

StatusCode ISF_HitAnalysis::geoInit(IOVSVC_CALLBACK_ARGS)
{
 ATH_MSG_INFO("geoInit for " << m_geoModel->atlasVersion() );

 StatusCode sc = detStore()->retrieve(m_tileMgr);
 if (sc.isFailure())
 {
  ATH_MSG_ERROR( "Unable to retrieve TileDetDescrManager from DetectorStore" );
  m_tileMgr=0;
 }

 sc = detStore()->retrieve(m_tileID);
 if (sc.isFailure())
 {
  ATH_MSG_ERROR( "Unable to retrieve TileID helper from DetectorStore" );
  m_tileID=0;
 }

 const DataHandle<CaloIdManager> caloIdManager;
 sc=detStore()->retrieve(caloIdManager);
 if(sc.isSuccess())
  ATH_MSG_DEBUG("CaloIDManager retrieved.");
 else
  throw std::runtime_error("ISF_HitAnalysis: Unable to retrieve CaloIDManeger");
 m_larEmID=caloIdManager->getEM_ID();
 if(m_larEmID==0)
  throw std::runtime_error("ISF_HitAnalysis: Invalid LAr EM ID helper");
 m_larFcalID=caloIdManager->getFCAL_ID();
 if(m_larFcalID==0)
  throw std::runtime_error("ISF_HitAnalysis: Invalid FCAL ID helper");
 m_larHecID=caloIdManager->getHEC_ID();
 if(m_larHecID==0)
  throw std::runtime_error("ISF_HitAnalysis: Invalid HEC ID helper");
 m_tileID=caloIdManager->getTileID();
 if(m_tileID==0)
  throw std::runtime_error("ISF_HitAnalysis: Invalid Tile ID helper");
 sc=detStore()->regHandle(m_dd_fSampl,"LArfSampl");
 if(sc.isFailure())
 {
  ATH_MSG_ERROR("Unable to register handle for LArfSampl");
  return StatusCode::FAILURE;
 }

 detStore()->retrieve(m_tileInfo,"TileInfo");
 if(sc.isFailure())
 {
  ATH_MSG_ERROR("Unable to retrieve TileInfo from DetectorStore");
  return StatusCode::FAILURE;
 }
 m_calo_dd_man  = CaloDetDescrManager::instance();

 // Retrieve Tools
 IToolSvc* p_toolSvc = 0;
 if ( service("ToolSvc",p_toolSvc).isFailure() )
 {
        ATH_MSG_ERROR("Cannot find ToolSvc! ");
  return StatusCode::FAILURE;
 }
 else
 {
  IAlgTool* algTool;

  // Get TimedExtrapolator  ***************************************************************************************************
  if (!m_extrapolator.empty() && m_extrapolator.retrieve().isFailure())
   return StatusCode::FAILURE;

  else ATH_MSG_DEBUG("Extrapolator retrieved "<< m_extrapolator);

  // Get CaloSurfaceHelper
  if(m_caloSurfaceHelper.retrieve().isFailure())
   ATH_MSG_INFO("CaloSurfaceHelper not found ");

  std::string CaloCoordinateTool_name="TBCaloCoordinate";
  ListItem CaloCoordinateTool(CaloCoordinateTool_name);
  if(p_toolSvc->retrieveTool(CaloCoordinateTool.type(),CaloCoordinateTool.name(), algTool, this).isFailure() )
  {
   ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
   return StatusCode::FAILURE;
  }
  m_calo_tb_coord = dynamic_cast<ICaloCoordinateTool*>(algTool);
  if(!m_calo_tb_coord )
  {
   ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
   return StatusCode::FAILURE;
  }
  else
   ATH_MSG_INFO("retrieved " << CaloCoordinateTool_name);
 } //tools

 return StatusCode::SUCCESS;

}

StatusCode ISF_HitAnalysis::updateMetaData( IOVSVC_CALLBACK_ARGS_P( I, keys ) )
{
 ATH_MSG_INFO( "Updating the Sim+Digi MetaData" );

 // Reset the internal settings:
 bool run_update = false;

 // Check what kind of keys we got. In principle the function should only
 // receive the "/Digitization/Parameters" and "/Simulation/Parameters" key.
 ATH_MSG_DEBUG("Update called with " <<I<< " folder " << keys.size() << " keys:");
 std::list< std::string >::const_iterator itr = keys.begin();
 std::list< std::string >::const_iterator end = keys.end();
 for( ; itr != end; ++itr )
 {
  if( *itr == m_MC_DIGI_PARAM ) run_update = true;
  if( *itr == m_MC_SIM_PARAM ) run_update = true;
 }
 // If that's not the key that we received after all, let's just return
 // silently...
 if( ! run_update ) return StatusCode::SUCCESS;

 const DataHandle< AthenaAttributeList > simParam;
 if( detStore()->retrieve( simParam, m_MC_SIM_PARAM ).isFailure() )
 {
   ATH_MSG_WARNING("Retrieving MC SIM metadata failed");
 }
 else
 {
  AthenaAttributeList::const_iterator attr_itr = simParam->begin();
  AthenaAttributeList::const_iterator attr_end = simParam->end();
  for( ; attr_itr != attr_end; ++attr_itr )
  {
   std::stringstream outstr;
   attr_itr->toOutputStream(outstr);
   ATH_MSG_INFO("MetaData: " << outstr.str());
  }
 }

 return StatusCode::SUCCESS;
}


StatusCode ISF_HitAnalysis::initialize()
{
 ATH_MSG_INFO( "Initializing ISF_HitAnalysis" );

 /*
 // get a handle of StoreGate for access to the Detector Store
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "ZH Unable to retrieve pointer to Detector Store");
    return sc;
  }
  // get a handle of StoreGate for access to the Event Store
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
     return sc;
  }
 */

 //
 // Register the callback(s):
 //
 StatusCode sc = service("GeoModelSvc", m_geoModel);
 if(sc.isFailure())
 {
  ATH_MSG_ERROR( "Could not locate GeoModelSvc" );
  return StatusCode::FAILURE;
 }

 // dummy parameters for the callback:
 int dummyInt=0;
 std::list<std::string> dummyList;

 if (m_geoModel->geoInitialized())
 {
  sc=geoInit(dummyInt,dummyList);
  if(sc.isFailure())
  {
   ATH_MSG_ERROR( "Call to geoInit failed" );
   return StatusCode::FAILURE;
  }
 }
 else
 {
  sc = detStore()->regFcn(&IGeoModelSvc::geoInit, m_geoModel, &ISF_HitAnalysis::geoInit,this);
  if(sc.isFailure())
  {
    ATH_MSG_ERROR( "Could not register geoInit callback" );
    return StatusCode::FAILURE;
  }
 }

 if( detStore()->contains< AthenaAttributeList >( m_MC_DIGI_PARAM ) )
 {
  const DataHandle< AthenaAttributeList > aptr;
  if( detStore()->regFcn( &ISF_HitAnalysis::updateMetaData, this, aptr,m_MC_DIGI_PARAM, true ).isFailure() )
  {
   ATH_MSG_ERROR( "Could not register callback for "<< m_MC_DIGI_PARAM );
   return StatusCode::FAILURE;
  }
 }
 else
 {
  ATH_MSG_WARNING( "MetaData not found for "<< m_MC_DIGI_PARAM );
 }

 if(detStore()->contains< AthenaAttributeList >( m_MC_SIM_PARAM ) )
 {
  const DataHandle< AthenaAttributeList > aptr;
  if( detStore()->regFcn( &ISF_HitAnalysis::updateMetaData, this, aptr,m_MC_SIM_PARAM, true ).isFailure() )
  {
   ATH_MSG_ERROR( "Could not register callback for "<< m_MC_SIM_PARAM );
   return StatusCode::FAILURE;
  }
 }
 else
  ATH_MSG_WARNING( "MetaData not found for "<< m_MC_SIM_PARAM );

 // Get CaloGeometryHelper
 if (m_CaloGeometryHelper.retrieve().isFailure())
 {
  ATH_MSG_ERROR("CaloGeometryHelper not found ");
  return StatusCode::FAILURE;
 }

 // Get FastCaloSimCaloExtrapolation
 if (m_FastCaloSimCaloExtrapolation.retrieve().isFailure())
 {
  ATH_MSG_ERROR("FastCaloSimCaloExtrapolation not found ");
  return StatusCode::FAILURE;
 }

 // Grab the Ntuple and histogramming service for the tree
 sc = service("THistSvc",m_thistSvc);
 if (sc.isFailure())
 {
  ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
  return StatusCode::FAILURE;
 }

 //#########################
 IPartPropSvc* p_PartPropSvc=0;
 if (service("PartPropSvc",p_PartPropSvc).isFailure() || p_PartPropSvc == 0)
 {
  ATH_MSG_ERROR("could not find PartPropService");
  return StatusCode::FAILURE;
 }

 m_particleDataTable = (HepPDT::ParticleDataTable*) p_PartPropSvc->PDT();
 if(m_particleDataTable == 0)
 {
        ATH_MSG_ERROR("PDG table not found");
  return StatusCode::FAILURE;
 }
 //#########################

 //
 m_tree = new TTree( TString(m_ntupleTreeName), "CaloHitAna" );
 std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
 sc = m_thistSvc->regTree(fullNtupleName, m_tree);
 if (sc.isFailure() || !m_tree )
 {
  ATH_MSG_ERROR("Unable to register TTree: " << fullNtupleName);
  return StatusCode::FAILURE;
 }

 /** now add branches and leaves to the tree */
 if (m_tree)
 {
  ATH_MSG_INFO("Successfull registered TTree: " << fullNtupleName);
  //this actually creates the vector itself! And only if it succeeds! Note that the result is not checked! And the code is probably leaking memory in the end
  m_tree->Branch("HitX",                 &m_hit_x);
  m_tree->Branch("HitY",                 &m_hit_y);
  m_tree->Branch("HitZ",                 &m_hit_z);
  m_tree->Branch("HitE",                 &m_hit_energy);
  m_tree->Branch("HitT",                 &m_hit_time);
  m_tree->Branch("HitIdentifier",        &m_hit_identifier);
  m_tree->Branch("HitCellIdentifier",    &m_hit_cellidentifier);
  m_tree->Branch("HitIsLArBarrel",       &m_islarbarrel);
  m_tree->Branch("HitIsLArEndCap",       &m_islarendcap);
  m_tree->Branch("HitIsHEC",             &m_islarhec);
  m_tree->Branch("HitIsFCAL",            &m_islarfcal);
  m_tree->Branch("HitIsTile",            &m_istile);
  m_tree->Branch("HitSampling",          &m_hit_sampling);
  m_tree->Branch("HitSamplingFraction",  &m_hit_samplingfraction);

  m_tree->Branch("TruthE",               &m_truth_energy);
  m_tree->Branch("TruthPx",              &m_truth_px);
  m_tree->Branch("TruthPy",              &m_truth_py);
  m_tree->Branch("TruthPz",              &m_truth_pz);
  m_tree->Branch("TruthPDG",             &m_truth_pdg);
  m_tree->Branch("TruthBarcode",         &m_truth_barcode);
  m_tree->Branch("TruthVtxBarcode",      &m_truth_vtxbarcode);

  m_tree->Branch("CellIdentifier",       &m_cell_identifier);
  m_tree->Branch("CellE",                &m_cell_energy);
  m_tree->Branch("CellSampling",         &m_cell_sampling);

  m_tree->Branch("G4HitE",               &m_g4hit_energy);
  m_tree->Branch("G4HitT",               &m_g4hit_time);
  m_tree->Branch("G4HitIdentifier",      &m_g4hit_identifier);
  m_tree->Branch("G4HitCellIdentifier",  &m_g4hit_cellidentifier);
  m_tree->Branch("G4HitSamplingFraction",&m_g4hit_samplingfraction);
  m_tree->Branch("G4HitSampling",        &m_g4hit_sampling);
  //And this looks like will fail since ROOT has no idea what the object is...
  //m_tree->Branch("MatchedCells", &m_matched_cells);

  //#########################
  /*
  m_tree->Branch("TTC_back_eta",&m_TTC_back_eta);
  m_tree->Branch("TTC_back_phi",&m_TTC_back_phi);
  m_tree->Branch("TTC_back_r",&m_TTC_back_r);
  m_tree->Branch("TTC_back_z",&m_TTC_back_z);
  m_tree->Branch("TTC_entrance_eta",&m_TTC_entrance_eta);
  m_tree->Branch("TTC_entrance_phi",&m_TTC_entrance_phi);
  m_tree->Branch("TTC_entrance_r",&m_TTC_entrance_r);
  m_tree->Branch("TTC_entrance_z",&m_TTC_entrance_z);
  m_tree->Branch("TTC_IDCaloBoundary_eta",&m_TTC_IDCaloBoundary_eta);
  m_tree->Branch("TTC_IDCaloBoundary_phi",&m_TTC_IDCaloBoundary_phi);
  m_tree->Branch("TTC_IDCaloBoundary_r",&m_TTC_IDCaloBoundary_r);
  m_tree->Branch("TTC_IDCaloBoundary_z",&m_TTC_IDCaloBoundary_z);
  m_tree->Branch("TTC_Angle3D",&m_TTC_Angle3D);
  m_tree->Branch("TTC_AngleEta",&m_TTC_AngleEta);
  */
  m_tree->Branch("newTTC_back_eta",&m_newTTC_back_eta);
  m_tree->Branch("newTTC_back_phi",&m_newTTC_back_phi);
  m_tree->Branch("newTTC_back_r",&m_newTTC_back_r);
  m_tree->Branch("newTTC_back_z",&m_newTTC_back_z);
  m_tree->Branch("newTTC_entrance_eta",&m_newTTC_entrance_eta);
  m_tree->Branch("newTTC_entrance_phi",&m_newTTC_entrance_phi);
  m_tree->Branch("newTTC_entrance_r",&m_newTTC_entrance_r);
  m_tree->Branch("newTTC_entrance_z",&m_newTTC_entrance_z);
  m_tree->Branch("newTTC_IDCaloBoundary_eta",&m_newTTC_IDCaloBoundary_eta);
  m_tree->Branch("newTTC_IDCaloBoundary_phi",&m_newTTC_IDCaloBoundary_phi);
  m_tree->Branch("newTTC_IDCaloBoundary_r",&m_newTTC_IDCaloBoundary_r);
  m_tree->Branch("newTTC_IDCaloBoundary_z",&m_newTTC_IDCaloBoundary_z);
  m_tree->Branch("newTTC_Angle3D",&m_newTTC_Angle3D);
  m_tree->Branch("newTTC_AngleEta",&m_newTTC_AngleEta);

  //#########################
  //m_tree->Print();
  if (!m_hit_x || !m_hit_y || !m_hit_z || !m_hit_energy || !m_hit_time || !m_hit_identifier || !m_hit_cellidentifier || !m_islarbarrel || !m_islarendcap || !m_islarfcal || !m_islarhec || !m_istile || !m_hit_sampling || !m_hit_samplingfraction || !m_truth_energy || !m_truth_px || !m_truth_py || !m_truth_pz || !m_truth_pdg || !m_truth_barcode || !m_truth_vtxbarcode)
  {
   ATH_MSG_ERROR("Unable to create TTree branch correctly");
   return StatusCode::FAILURE;
  }
  if (!m_cell_identifier || !m_cell_energy || !m_cell_sampling || !m_g4hit_energy || !m_g4hit_time || !m_g4hit_identifier || !m_g4hit_cellidentifier || !m_g4hit_samplingfraction || !m_g4hit_sampling )
  {
   ATH_MSG_ERROR("Unable to create TTree branch correctly (cell or g4hit)");
   return StatusCode::FAILURE;
  }
  /*
  if (!m_TTC_back_eta || !m_TTC_back_phi || !m_TTC_back_r || !m_TTC_back_z || !m_TTC_entrance_eta || !m_TTC_entrance_phi || !m_TTC_entrance_r || !m_TTC_entrance_z || !m_TTC_IDCaloBoundary_eta || !m_TTC_IDCaloBoundary_phi || !m_TTC_IDCaloBoundary_r || !m_TTC_IDCaloBoundary_z || !m_TTC_Angle3D || !m_TTC_AngleEta)
  {
         ATH_MSG_ERROR("Unable to create TTree branch correctly (TTC variables)");
         return StatusCode::FAILURE;
  }
  */
  if (!m_newTTC_back_eta || !m_newTTC_back_phi || !m_newTTC_back_r || !m_newTTC_back_z || !m_newTTC_entrance_eta || !m_newTTC_entrance_phi || !m_newTTC_entrance_r || !m_newTTC_entrance_z || !m_newTTC_IDCaloBoundary_eta || !m_newTTC_IDCaloBoundary_phi || !m_newTTC_IDCaloBoundary_r || !m_newTTC_IDCaloBoundary_z || !m_newTTC_Angle3D || !m_newTTC_AngleEta)
  {
         ATH_MSG_ERROR("Unable to create TTree branch correctly (newTTC variables)");
         return StatusCode::FAILURE;
  }

 }

 return StatusCode::SUCCESS;

} //initialize

StatusCode ISF_HitAnalysis::finalize()
{

 ATH_MSG_INFO( "doing finalize()" );

 TTree* geo = new TTree( m_geoModel->atlasVersion().c_str() , m_geoModel->atlasVersion().c_str() );
 std::string fullNtupleName =  "/"+m_geoFileName+"/"+m_geoModel->atlasVersion();
 StatusCode sc = m_thistSvc->regTree(fullNtupleName, geo);
 if(sc.isFailure() || !geo )
 {
  ATH_MSG_ERROR("Unable to register TTree: " << fullNtupleName);
  return StatusCode::FAILURE;
 }

 /** now add branches and leaves to the tree */

 typedef struct
 {
  Long64_t identifier;
  Int_t calosample;
  float eta,phi,r,eta_raw,phi_raw,r_raw,x,y,z,x_raw,y_raw,z_raw;
  float deta,dphi,dr,dx,dy,dz;
 } GEOCELL;

 static GEOCELL geocell;

 if(geo)
 {
  ATH_MSG_INFO("Successfull registered TTree: " << fullNtupleName);
  //this actually creates the vector itself! And only if it succeeds! Note that the result is not checked! And the code is probably leaking memory in the end
  //geo->Branch("cells", &geocell,"identifier/L:eta,phi,r,eta_raw,phi_raw,r_raw,x,y,z,x_raw,y_raw,z_raw/F:Deta,Dphi,Dr,Dx,Dy,Dz/F");
  geo->Branch("identifier", &geocell.identifier,"identifier/L");
  geo->Branch("calosample", &geocell.calosample,"calosample/I");

  geo->Branch("eta", &geocell.eta,"eta/F");
  geo->Branch("phi", &geocell.phi,"phi/F");
  geo->Branch("r", &geocell.r,"r/F");
  geo->Branch("eta_raw", &geocell.eta_raw,"eta_raw/F");
  geo->Branch("phi_raw", &geocell.phi_raw,"phi_raw/F");
  geo->Branch("r_raw", &geocell.r_raw,"r_raw/F");

  geo->Branch("x", &geocell.x,"x/F");
  geo->Branch("y", &geocell.y,"y/F");
  geo->Branch("z", &geocell.z,"z/F");
  geo->Branch("x_raw", &geocell.x_raw,"x_raw/F");
  geo->Branch("y_raw", &geocell.y_raw,"y_raw/F");
  geo->Branch("z_raw", &geocell.z_raw,"z_raw/F");

  geo->Branch("deta", &geocell.deta,"deta/F");
  geo->Branch("dphi", &geocell.dphi,"dphi/F");
  geo->Branch("dr", &geocell.dr,"dr/F");
  geo->Branch("dx", &geocell.dx,"dx/F");
  geo->Branch("dy", &geocell.dy,"dy/F");
  geo->Branch("dz", &geocell.dz,"dz/F");
 }

 if(m_calo_dd_man)
 {
  int ncells=0;
  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_calo_dd_man->element_begin();calo_iter<m_calo_dd_man->element_end();++calo_iter)
  {
   const CaloDetDescrElement* theDDE=*calo_iter;
   if(theDDE)
   {
    CaloCell_ID::CaloSample sample=theDDE->getSampling();
    //CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
    ++ncells;
    if(geo)
    {
     geocell.identifier=theDDE->identify().get_compact();
     geocell.calosample=sample;
     geocell.eta=theDDE->eta();
     geocell.phi=theDDE->phi();
     geocell.r=theDDE->r();
     geocell.eta_raw=theDDE->eta_raw();
     geocell.phi_raw=theDDE->phi_raw();
     geocell.r_raw=theDDE->r_raw();
     geocell.x=theDDE->x();
     geocell.y=theDDE->y();
     geocell.z=theDDE->z();
     geocell.x_raw=theDDE->x_raw();
     geocell.y_raw=theDDE->y_raw();
     geocell.z_raw=theDDE->z_raw();
     geocell.deta=theDDE->deta();
     geocell.dphi=theDDE->dphi();
     geocell.dr=theDDE->dr();
     geocell.dx=theDDE->dx();
     geocell.dy=theDDE->dy();
     geocell.dz=theDDE->dz();

     geo->Fill();
    }
   }
  }

  ATH_MSG_INFO( ncells<<" cells found" );
 }

 return StatusCode::SUCCESS;
} //finalize


StatusCode ISF_HitAnalysis::execute()
{

 ATH_MSG_DEBUG( "In ISF_HitAnalysis::execute()" );

 if (! m_tree)
 {
  ATH_MSG_ERROR( "tree not registered" );
  return StatusCode::FAILURE;
 }

 //now if the branches were created correctly, the pointers point to something and it is possible to clear the vectors
 TVector3 vectest;
 vectest.SetPtEtaPhi(1.,1.,1.);
 m_hit_x->clear();
 m_hit_y->clear();
 m_hit_z->clear();
 m_hit_energy->clear();
 m_hit_time->clear();
 m_hit_identifier->clear();
 m_hit_cellidentifier->clear();
 m_islarbarrel->clear();
 m_islarendcap->clear();
 m_islarhec->clear();
 m_islarfcal->clear();
 m_istile->clear();
 m_hit_sampling->clear();
 m_hit_samplingfraction->clear();
 m_truth_energy->clear();
 m_truth_px->clear();
 m_truth_py->clear();
 m_truth_pz->clear();
 m_truth_pdg->clear();
 m_truth_barcode->clear();
 m_truth_vtxbarcode->clear();
 m_cell_identifier->clear();
 m_cell_energy->clear();
 m_cell_sampling->clear();
 m_g4hit_energy->clear();
 m_g4hit_time->clear();
 m_g4hit_identifier->clear();
 m_g4hit_cellidentifier->clear();
 m_g4hit_sampling->clear();
 m_g4hit_samplingfraction->clear();
 //which fails for this one!!
 //m_matched_cells->clear();

 //##########################
 /*
 m_TTC_back_eta->clear();
 m_TTC_back_phi->clear();
 m_TTC_back_r->clear();
 m_TTC_back_z->clear();
 m_TTC_entrance_eta->clear();
 m_TTC_entrance_phi->clear();
 m_TTC_entrance_r->clear();
 m_TTC_entrance_z->clear();
 m_TTC_IDCaloBoundary_eta->clear();
 m_TTC_IDCaloBoundary_phi->clear();
 m_TTC_IDCaloBoundary_r->clear();
 m_TTC_IDCaloBoundary_z->clear();
 m_TTC_Angle3D->clear();
 m_TTC_AngleEta->clear();
 */

 m_newTTC_back_eta->clear();
 m_newTTC_back_phi->clear();
 m_newTTC_back_r->clear();
 m_newTTC_back_z->clear();
 m_newTTC_entrance_eta->clear();
 m_newTTC_entrance_phi->clear();
 m_newTTC_entrance_r->clear();
 m_newTTC_entrance_z->clear();
 m_newTTC_IDCaloBoundary_eta->clear();
 m_newTTC_IDCaloBoundary_phi->clear();
 m_newTTC_IDCaloBoundary_r->clear();
 m_newTTC_IDCaloBoundary_z->clear();
 m_newTTC_Angle3D->clear();
 m_newTTC_AngleEta->clear();
 //##########################

 //Get the FastCaloSim step info collection from store
 const ISF_FCS_Parametrization::FCS_StepInfoCollection* eventStepsES;
 StatusCode sc = evtStore()->retrieve(eventStepsES, "MergedEventSteps");
 if (sc.isFailure())
 {
  ATH_MSG_WARNING( "No FastCaloSim steps read from StoreGate?" );
  //return StatusCode::FAILURE;
 }
 else
 {
  ATH_MSG_INFO("Read: "<<eventStepsES->size()<<" position hits");
  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::const_iterator it = eventStepsES->begin(); it != eventStepsES->end(); ++it)
  {
         m_hit_x->push_back( (*it)->x() );
         m_hit_y->push_back( (*it)->y() );
         m_hit_z->push_back( (*it)->z() );
         m_hit_energy->push_back( (*it)->energy() );
   m_hit_time->push_back( (*it)->time());

         //Try to get the samplings, sampling fractions from identifiers
         bool larbarrel=false;
         bool larendcap=false;
         bool larhec=false;
         bool larfcal=false;
         bool tile=false;
         int sampling=-1;
         double sampfrac=0.0;

         Identifier id = (*it)->identify();
         Identifier cell_id = (*it)->identify(); //to be replaced by cell_id in tile

         if(m_calo_dd_man->get_element(id))
         {
          CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(id)->getSampling();
          sampling = layer; //use CaloCell layer immediately
         }
         else
          ATH_MSG_WARNING( "Warning no sampling info for "<<id.getString());

   if(m_larEmID->is_lar_em(id) || m_larHecID->is_lar_hec(id) || m_larFcalID->is_lar_fcal(id))
          sampfrac=m_dd_fSampl->FSAMPL(id);

         if(m_larEmID->is_lar_em(id))
         {
          //std::cout <<"This hit is in LAr EM ";
          if (m_larEmID->is_em_barrel(id))
                 larbarrel=true;
          else if(m_larEmID->is_em_endcap(id))
                 larendcap=true;
         }
         else if(m_larHecID->is_lar_hec(id))
    larhec = true;
         else if(m_larFcalID->is_lar_fcal(id))
    larfcal = true;
         else if(m_tileID->is_tile_barrel(id) || m_tileID->is_tile_extbarrel(id) || m_tileID->is_tile_gap(id) || m_tileID->is_tile_gapscin(id) || m_tileID->is_tile_aux(id))
         {
          cell_id = m_tileID->cell_id(id);
          Int_t tile_sampling = -1;
          if(m_calo_dd_man->get_element(cell_id))
                {
                 tile_sampling = m_calo_dd_man->get_element(cell_id)->getSampling();
                 sampfrac = m_tileInfo->HitCalib(cell_id);
                }
          tile = true;
          if(tile_sampling!= -1)
           sampling = tile_sampling; //m_calo_dd_man needs to be called with cell_id not pmt_id!!
         }
         else
    ATH_MSG_WARNING( "This hit is somewhere. Please check!");

         m_hit_identifier->push_back(id.get_compact());
         m_hit_cellidentifier->push_back(cell_id.get_compact());
         //push things into vectors:
         m_islarbarrel->push_back(larbarrel);
         m_islarendcap->push_back(larendcap);
         m_islarhec->push_back(larhec);
         m_islarfcal->push_back(larfcal);
         m_istile->push_back(tile);
         m_hit_sampling->push_back(sampling);
         m_hit_samplingfraction->push_back(sampfrac);

        } //event steps
 }//event steps read correctly

 //Get truth particle info
 //Note that there can be more truth particles, the first one is usually the one we need.
 const DataHandle<McEventCollection> mcEvent;
 sc = evtStore()->retrieve(mcEvent,"TruthEvent");
 if(sc.isFailure())
  ATH_MSG_WARNING( "No truth event!");
 else
 {
  if(mcEvent)
  {
         //std::cout<<"ISF_HitAnalysis: MC event size: "<<mcEvent->size()<<std::endl;
         if (mcEvent->size())
         {
                int particleIndex=0;
                int loopEnd = m_NtruthParticles;
                if(loopEnd==-1)
                {
                 loopEnd = (*mcEvent->begin())->particles_size(); //is this the correct thing?
                }
                //std::cout <<"ISF_HitAnalysis: MC first truth event size: "<<(*mcEvent->begin())->particles_size()<<std::endl;
          for (HepMC::GenEvent::particle_const_iterator it = (*mcEvent->begin())->particles_begin(); it != (*mcEvent->begin())->particles_end(); ++it)
                {
     ATH_MSG_DEBUG("Number truth particles="<<(*mcEvent->begin())->particles_size()<<" loopEnd="<<loopEnd);
                 particleIndex++;

                 if (particleIndex>loopEnd) break; //enough particles

     //UPDATE EXTRAPOLATION WITH ALGTOOL *********************************************************************************************

           TFCSTruthState truth((*it)->momentum().px(),(*it)->momentum().py(),(*it)->momentum().pz(),(*it)->momentum().e(),(*it)->pdg_id());

           //calculate the vertex
           TVector3 moment;
           moment.SetXYZ((*it)->momentum().px(),(*it)->momentum().py(),(*it)->momentum().pz());
           TVector3 direction=moment.Unit();

           //does it hit the barrel or the EC?
           if(abs(direction.Z())/3550.<direction.Perp()/1148.) //BARREL
            direction*=1148./direction.Perp();
     else //EC
        direction*=3550./abs(direction.Z());

           truth.set_vertex(direction.X(),direction.Y(),direction.Z()); //is this really needed?

           ATH_MSG_DEBUG("VERTEX x "<<direction.X()<<" y "<<direction.Y()<<" z "<<direction.Z());

           TFCSExtrapolationState result;
           m_FastCaloSimCaloExtrapolation->extrapolate(result,&truth);

     //write the result into the ntuple variables:

     ATH_MSG_DEBUG("IDCaloBoundary_eta() "<<result.IDCaloBoundary_eta());
     ATH_MSG_DEBUG("IDCaloBoundary_phi() "<<result.IDCaloBoundary_phi());
     ATH_MSG_DEBUG("IDCaloBoundary_r() "<<result.IDCaloBoundary_r());
     ATH_MSG_DEBUG("IDCaloBoundary_z() "<<result.IDCaloBoundary_z());
     ATH_MSG_DEBUG("AngleEta "<<result.IDCaloBoundary_AngleEta());
     ATH_MSG_DEBUG("Angle3D "<<result.IDCaloBoundary_Angle3D());

     m_newTTC_IDCaloBoundary_eta->push_back(float(result.IDCaloBoundary_eta()));
                 m_newTTC_IDCaloBoundary_phi->push_back(float(result.IDCaloBoundary_phi()));
                 m_newTTC_IDCaloBoundary_r->push_back(float(result.IDCaloBoundary_r()));
                 m_newTTC_IDCaloBoundary_z->push_back(float(result.IDCaloBoundary_z()));
                 m_newTTC_Angle3D ->push_back(float(result.IDCaloBoundary_Angle3D()));
                 m_newTTC_AngleEta->push_back(float(result.IDCaloBoundary_AngleEta()));

     std::vector<float> eta_vec_ENT;
                 std::vector<float> phi_vec_ENT;
                 std::vector<float> r_vec_ENT;
                 std::vector<float> z_vec_ENT;

     std::vector<float> eta_vec_EXT;
                 std::vector<float> phi_vec_EXT;
                 std::vector<float> r_vec_EXT;
                 std::vector<float> z_vec_EXT;

     for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample)
                 {
                        ATH_MSG_DEBUG("sample "<<sample);
                        ATH_MSG_DEBUG(" eta ENT "<<result.eta(sample,1)<<" eta EXT "<<result.eta(sample,2));
                        ATH_MSG_DEBUG(" phi ENT "<<result.phi(sample,1)<<" phi EXT "<<result.phi(sample,2));
                        ATH_MSG_DEBUG(" r   ENT "<<result.r(sample,1)  <<" r   EXT "<<result.r(sample,2)  );
                        ATH_MSG_DEBUG(" z   ENT "<<result.z(sample,1)  <<" z   EXT "<<result.z(sample,2)  );
                        eta_vec_ENT.push_back(float(result.eta(sample,1)));
                        eta_vec_EXT.push_back(float(result.eta(sample,2)));
                        phi_vec_ENT.push_back(float(result.phi(sample,1)));
                        phi_vec_EXT.push_back(float(result.phi(sample,2)));
                        r_vec_ENT.push_back(float(result.r(sample,1)));
                        r_vec_EXT.push_back(float(result.r(sample,2)));
                        z_vec_ENT.push_back(float(result.z(sample,1)));
                        z_vec_EXT.push_back(float(result.z(sample,2)));
     }

     m_newTTC_back_eta->push_back(eta_vec_EXT);
                 m_newTTC_back_phi->push_back(phi_vec_EXT);
                 m_newTTC_back_r  ->push_back(r_vec_EXT);
                 m_newTTC_back_z  ->push_back(z_vec_EXT);
                 m_newTTC_entrance_eta->push_back(eta_vec_ENT);
                 m_newTTC_entrance_phi->push_back(phi_vec_ENT);
                 m_newTTC_entrance_r  ->push_back(r_vec_ENT);
                 m_newTTC_entrance_z  ->push_back(z_vec_ENT);

     //*******************************************************************************************************************************

     //OLD EXTRAPOLATION
     std::vector<Trk::HitInfo>* hitVector = caloHits(*(*it));

     Amg::Vector3D mom((*it)->momentum().x(),(*it)->momentum().y(),(*it)->momentum().z());

                 m_truth_energy->push_back((*it)->momentum().e());
                 m_truth_px->push_back((*it)->momentum().px());
                 m_truth_py->push_back((*it)->momentum().py());
                 m_truth_pz->push_back((*it)->momentum().pz());
                 m_truth_pdg->push_back((*it)->pdg_id());
                 m_truth_barcode->push_back((*it)->barcode());

     //extrapolate only if the vertex is within the IDcaloboundary - margin
                 //bool inside_ID=false;

                 /*

                 HepMC::GenVertex* pvtx = (*it)->production_vertex();
                 if(pvtx)
                 {
                  truth.set_vertex(pvtx->position().x(),pvtx->position().y(),pvtx->position().z(),pvtx->position().t());

                  m_truth_vtxbarcode->push_back(pvtx->barcode());
                  double radius2=(pvtx->position().x()*pvtx->position().x())+(pvtx->position().y()*pvtx->position().y());
                  ATH_MSG_VERBOSE("particle "<<particleIndex<<" Mom: "<<(*it)->momentum().e()<<"|"<<(*it)->momentum().px()<<"|"<<(*it)->momentum().py()<<"|"<<(*it)->momentum().pz()<<" vertex_x "<<pvtx->position().x()<<" y "<<pvtx->position().y()<<" z "<<pvtx->position().z()<<" r2 "<<radius2<<" VertexBarcode: "<<pvtx->barcode()<<" ParticleBarcode:"<<(*it)->barcode()<<" status: "<<(*it)->status());
                  //std::cout<<"m_CaloBoundaryZ+m_calomargin "<<m_CaloBoundaryZ+m_calomargin<<" m_CaloBoundaryR+m_calomargin "<<m_CaloBoundaryR+m_calomargin<<std::endl;

      if(fabs(pvtx->position().z())<(m_CaloBoundaryZ+m_calomargin) && radius2<((m_CaloBoundaryR+m_calomargin)*(m_CaloBoundaryR+m_calomargin)))
                        {
                         //inside_ID=true;
                         //if(inside_ID)
                         //  {
                         ATH_MSG_DEBUG("*** Do extrapolation ***");
                         extrapolate(*it,hitVector);
                         ATH_MSG_DEBUG("*** Do extrapolation to ID ***");
                         extrapolate_to_ID(*it,hitVector);
                         ATH_MSG_DEBUG("Truth extrapolation done for "<<particleIndex<<" "<<(*it)->barcode());
                        } //in this case no extrapolation possible
                  else
                        {
                         //fill extrapolated angles with some weird values
                         m_TTC_IDCaloBoundary_eta->push_back(-999.);
                         m_TTC_IDCaloBoundary_phi->push_back(-999.);
                         m_TTC_IDCaloBoundary_r->push_back(-999.);
                         m_TTC_IDCaloBoundary_z->push_back(-999.);
                         m_TTC_Angle3D->push_back(-999.);
                         m_TTC_AngleEta->push_back(-999.);
                        }
                 } //if pvtx
                 else //no pvtx
                 {
                  ATH_MSG_WARNING( "No vertex found for truth particle, no extrapolation");
      m_truth_vtxbarcode->push_back(-999999);

                  //fill extrapolated angles with some weird values
                  std::vector<float> eta_safe;
                  std::vector<float> phi_safe;
                  std::vector<float> r_safe;
                  std::vector<float> z_safe;
                  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample)
                        {
                         eta_safe.push_back(-999.0);
                         phi_safe.push_back(-999.0);
                         r_safe.push_back(-999.0);
                         z_safe.push_back(-999.0);
                        }

                  m_TTC_back_eta->push_back(eta_safe);
                  m_TTC_back_phi->push_back(phi_safe);
                  m_TTC_back_r->push_back(r_safe);
                  m_TTC_back_z->push_back(z_safe);
                  m_TTC_entrance_eta->push_back(eta_safe);
                  m_TTC_entrance_phi->push_back(phi_safe);
                  m_TTC_entrance_r->push_back(r_safe);
                  m_TTC_entrance_z->push_back(z_safe);
                  m_TTC_IDCaloBoundary_eta->push_back(-999.);
                  m_TTC_IDCaloBoundary_phi->push_back(-999.);
                  m_TTC_IDCaloBoundary_r->push_back(-999.);
                  m_TTC_IDCaloBoundary_z->push_back(-999.);
                  m_TTC_Angle3D->push_back(-999.);
                  m_TTC_AngleEta->push_back(-999.);

                 } //no pvtx
                 */

     for(std::vector<Trk::HitInfo>::iterator it = hitVector->begin();it < hitVector->end();++it)
     {
      if((*it).trackParms)
      {
       delete (*it).trackParms;
       (*it).trackParms=0;
      }
     }
     delete hitVector;
    } //for mcevent
         } //mcevent size
        } //mcEvent
 }//truth event

 //Get reco cells if available
 const CaloCellContainer *cellColl = 0;
 sc = evtStore()->retrieve(cellColl, "AllCalo");

 if (sc.isFailure())
 {
  ATH_MSG_WARNING( "Couldn't read AllCalo cells from StoreGate");
  //return NULL;
 }
 else
 {
  ATH_MSG_INFO( "Found: "<<cellColl->size()<<" calorimeter cells");
  CaloCellContainer::const_iterator itrCell = cellColl->begin();
  CaloCellContainer::const_iterator itrLastCell = cellColl->end();
  for ( ; itrCell!=itrLastCell; ++itrCell)
  {
         m_cell_energy->push_back((*itrCell)->energy());
         m_cell_identifier->push_back((*itrCell)->ID().get_compact());
         if (m_calo_dd_man->get_element((*itrCell)->ID()))
         {
          CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element((*itrCell)->ID())->getSampling();
          m_cell_sampling->push_back(layer);
         }
         else
          m_cell_sampling->push_back(-1);
  }
 } //calorimeter cells

 //Get all G4Hits (from CaloHitAnalysis)
 std::string  lArKey [4] = {"LArHitEMB", "LArHitEMEC", "LArHitFCAL", "LArHitHEC"};
 for (unsigned int i=0;i<4;i++)
 {
  const DataHandle<LArHitContainer> iter;
  ATH_MSG_DEBUG( "Checking G4Hits: "<<lArKey[i]);
  if(evtStore()->retrieve(iter,lArKey[i])==StatusCode::SUCCESS)
  {
   LArHitContainer::const_iterator hi;
   int hitnumber = 0;
   for (hi=(*iter).begin();hi!=(*iter).end();hi++)
   {
          hitnumber++;
          GeoLArHit ghit(**hi);
    if (!ghit)
     continue;
    const CaloDetDescrElement *hitElement = ghit.getDetDescrElement();
          if(!hitElement)
           continue;
    Identifier larhitid = hitElement->identify();
          if(m_calo_dd_man->get_element(larhitid))
          {
           CaloCell_ID::CaloSample larlayer = m_calo_dd_man->get_element(larhitid)->getSampling();

           float larsampfrac=m_dd_fSampl->FSAMPL(larhitid);
           m_g4hit_energy->push_back( ghit.Energy() );
           m_g4hit_time->push_back( ghit.Time() );
           m_g4hit_identifier->push_back( larhitid.get_compact() );
           m_g4hit_cellidentifier->push_back( larhitid.get_compact() );
           m_g4hit_sampling->push_back( larlayer);
           m_g4hit_samplingfraction->push_back( larsampfrac );
          }
   } // End while LAr hits
   ATH_MSG_INFO( "Read "<<hitnumber<<" G4Hits from "<<lArKey[i]);
  }
  else
  {
         ATH_MSG_INFO( "Can't retrieve LAr hits");
  }// End statuscode success upon retrieval of hits
  //std::cout <<"ZH G4Hit size: "<<m_g4hit_e->size()<<std::endl;
 }// End detector type loop

 const TileHitVector * hitVec;
 if (evtStore()->retrieve(hitVec,"TileHitVec")==StatusCode::SUCCESS &&  m_tileMgr &&  m_tileID )
 {
  int hitnumber = 0;
  for(TileHitVecConstIterator i_hit=hitVec->begin() ; i_hit!=hitVec->end() ; ++i_hit)
  {
   hitnumber++;
   Identifier pmt_id = (*i_hit).identify();
   Identifier cell_id = m_tileID->cell_id(pmt_id);
   //const  CaloDetDescrElement* ddElement = m_tileMgr->get_cell_element(cell_id);

   if (m_calo_dd_man->get_element(cell_id))
         {
          CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(cell_id)->getSampling();

          float tilesampfrac = m_tileInfo->HitCalib(cell_id);

          //could there be more subhits??
          for (int tilesubhit_i = 0; tilesubhit_i<(*i_hit).size(); tilesubhit_i++)
          {
           //!!
           //std::cout <<"Tile subhit: "<<tilesubhit_i<<"/"<<(*i_hit).size()<< " E: "<<(*i_hit).energy(tilesubhit_i)<<std::endl;
           m_g4hit_energy->push_back( (*i_hit).energy(tilesubhit_i) );
           m_g4hit_time->push_back(   (*i_hit).time(tilesubhit_i)   );
           m_g4hit_identifier->push_back( pmt_id.get_compact() );
           m_g4hit_cellidentifier->push_back( cell_id.get_compact() );
           m_g4hit_sampling->push_back( layer );
           m_g4hit_samplingfraction->push_back( tilesampfrac );
          }
         }
  }
  ATH_MSG_INFO( "Read "<<hitnumber<<" G4Hits from TileHitVec");
 }

 //Fill the tree and finish
 if (m_tree) m_tree->Fill();

 return StatusCode::SUCCESS;

} //execute

std::vector<Trk::HitInfo>* ISF_HitAnalysis::caloHits(const HepMC::GenParticle& part) const
{
 // Start calo extrapolation
 ATH_MSG_DEBUG ("[ fastCaloSim transport ] processing particle "<<part.pdg_id() );

 std::vector<Trk::HitInfo>*     hitVector =  new std::vector<Trk::HitInfo>;

 int     pdgId    = part.pdg_id();
 double  charge   = HepPDT::ParticleID(pdgId).charge();

 // particle Hypothesis for the extrapolation
 Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(pdgId,charge);

 ATH_MSG_DEBUG ("particle hypothesis "<< pHypothesis );

 // geantinos not handled by PdgToParticleHypothesis - fix there
 if( pdgId == 999 ) pHypothesis = Trk::geantino;

 HepMC::GenVertex *vtx = part.production_vertex();
 Amg::Vector3D pos(0.,0.,0.);    // default

 if (vtx)
 {
  //const HepMC::ThreeVector vtxPos(vtx->point3d());
  pos = Amg::Vector3D( vtx->point3d().x(),vtx->point3d().y(), vtx->point3d().z());
 }

 Amg::Vector3D mom(part.momentum().x(),part.momentum().y(),part.momentum().z());
 ATH_MSG_DEBUG( "[ fastCaloSim transport ] starting transport from position eta="<<pos.eta()<<" phi="<<pos.phi()<<" d="<<pos.mag()<<" pT="<<mom.perp() );

 // input parameters : curvilinear parameters
 Trk::CurvilinearParameters inputPar(pos,mom,charge);

 // stable vs. unstable check : ADAPT for FASTCALOSIM
 //double freepath = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : - 1.;
 double freepath = -1.;
 //ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << freepath);
 // path limit -> time limit  ( TODO : extract life-time directly from decay helper )
 double tDec = freepath > 0. ? freepath : -1.;
 int decayProc = 0;

 /* uncomment if unstable particles used by FastCaloSim
 // beta calculated here for further use in validation
 double mass = m_particleMasses.mass[pHypothesis];
 double mom = isp.momentum().mag();
 double beta = mom/sqrt(mom*mom+mass*mass);

 if ( tDec>0.)
 {
    tDec = tDec/beta/CLHEP::c_light + isp.timeStamp();
    decayProc = 201;
 }
 */

 Trk::TimeLimit timeLim(tDec,0.,decayProc);        // TODO: set vertex time info

 // prompt decay ( uncomment if unstable particles used )
 //if ( freepath>0. && freepath<0.01 ) {
 //  if (!m_particleDecayHelper.empty()) {
 //    ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
 //    m_particleDecayHelper->decay(isp);
 //  }
 //  return 0;
 //}

 // presample interactions - ADAPT FOR FASTCALOSIM
 Trk::PathLimit pathLim(-1.,0);
 //if (absPdg!=999 && pHypothesis<99) pathLim = m_samplingTool->sampleProcess(mom,isp.charge(),pHypothesis);

 Trk::GeometrySignature nextGeoID=Trk::Calo;

 // first extrapolation to reach the ID boundary
 ATH_MSG_DEBUG( "[ fastCaloSim transport ] before calo entrance ");

 // get CaloEntrance if not done already
 if (!m_caloEntrance)
 {
  m_caloEntrance = m_extrapolator->trackingGeometry()->trackingVolume(m_caloEntranceName);
  if(!m_caloEntrance)
   ATH_MSG_INFO("CaloEntrance not found ");
  else
   ATH_MSG_INFO("CaloEntrance found ");
 }

 ATH_MSG_DEBUG( "[ fastCaloSim transport ] after calo entrance ");

 const Trk::TrackParameters* caloEntry = 0;

 if(m_caloEntrance && m_caloEntrance->inside(pos,0.001) && !m_extrapolator->trackingGeometry()->atVolumeBoundary(pos,m_caloEntrance,0.001))
 {
  std::vector<Trk::HitInfo>*     dummyHitVector = 0;
  if( charge==0 )
  {
   caloEntry = m_extrapolator->transportNeutralsWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,dummyHitVector,nextGeoID,m_caloEntrance);
  }
  else
  {
   caloEntry = m_extrapolator->extrapolateWithPathLimit(inputPar,pathLim,timeLim,Trk::alongMomentum,pHypothesis,dummyHitVector,nextGeoID,m_caloEntrance);
  }
 }
 else
  caloEntry=&inputPar;

 ATH_MSG_DEBUG( "[ fastCaloSim transport ] after calo caloEntry ");

 if(caloEntry)
 {
  const Trk::TrackParameters* eParameters = 0;

  // save Calo entry hit (fallback info)
  hitVector->push_back(Trk::HitInfo(caloEntry->clone(),timeLim.time,nextGeoID,0.));

  ATH_MSG_DEBUG( "[ fastCaloSim transport ] starting Calo transport from position eta="<<caloEntry->position().eta()<<" phi="<<caloEntry->position().phi()<<" d="<<caloEntry->position().mag() );

  if( charge==0 )
  {
   eParameters = m_extrapolator->transportNeutralsWithPathLimit(*caloEntry,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);
  }
  else
  {
   eParameters = m_extrapolator->extrapolateWithPathLimit(*caloEntry,pathLim,timeLim,Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);
  }
  // save Calo exit hit (fallback info)
  if (eParameters) hitVector->push_back(Trk::HitInfo(eParameters,timeLim.time,nextGeoID,0.));
  //delete eParameters;   // HitInfo took ownership
 }

 if(msgLvl(MSG::DEBUG))
 {
  std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
  while (it < hitVector->end() )
  {
   int sample=(*it).detID;
   Amg::Vector3D hitPos = (*it).trackParms->position();
   ATH_MSG_DEBUG(" HIT: layer="<<sample<<" sample="<<sample-3000<<" eta="<<hitPos.eta()<<" phi="<<hitPos.phi()<<" d="<<hitPos.mag());
   it++;
  }
 }

 return hitVector;
} //caloHits

/*
//#######################################################################
void ISF_HitAnalysis::extrapolate(const HepMC::GenParticle* part,std::vector<Trk::HitInfo>* hitVector)
{
 ATH_MSG_DEBUG("Start extrapolate()");

 m_ptruth_eta=part->momentum().eta();
 m_ptruth_phi=part->momentum().phi();
 m_ptruth_e  =part->momentum().e();
 m_ptruth_pt =part->momentum().perp();
 m_ptruth_p  =part->momentum().rho();
 m_pdgid=part->pdg_id();

 //////////////////////////////////////
 // Start calo extrapolation
 // First: get entry point into first calo sample
 //////////////////////////////////////

 get_calo_surface(hitVector);

 std::vector< std::vector<double> > eta_safe(3);
 std::vector< std::vector<double> > phi_safe(3);
 std::vector< std::vector<double> > r_safe(3);
 std::vector< std::vector<double> > z_safe(3);
 for(int subpos=CaloSubPos::SUBPOS_MID;subpos<=CaloSubPos::SUBPOS_EXT;++subpos)
 {
  eta_safe[subpos].resize(CaloCell_ID_FCS::MaxSample,-999.0);
        phi_safe[subpos].resize(CaloCell_ID_FCS::MaxSample,-999.0);
        r_safe[subpos].resize(CaloCell_ID_FCS::MaxSample,-999.0);
        z_safe[subpos].resize(CaloCell_ID_FCS::MaxSample,-999.0);
 }

 // only continue if inside the calo
 if( fabs(m_eta_calo_surf)<6 )
 {
  // now try to extrpolate to all calo layers, that contain energy
  ATH_MSG_DEBUG("Calo position for particle id "<<m_pdgid<<", trutheta= " << m_ptruth_eta <<", truthphi= "<<m_ptruth_phi<<", truthp="<<m_ptruth_p<<", truthpt="<<m_ptruth_pt);
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample)
  {
   for(int subpos=CaloSubPos::SUBPOS_MID;subpos<=CaloSubPos::SUBPOS_EXT;++subpos)
   {
    m_letaCalo[sample][subpos]=-12345;
    m_lphiCalo[sample][subpos]=-12345;
    m_lrCalo[sample][subpos]=-12345;
    m_lzCalo[sample][subpos]=-12345;
    m_layerCaloOK[sample][subpos]=0;
    //ATH_MSG_DEBUG("============= Getting Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << m_ptruth_eta);

    if(get_calo_etaphi(hitVector,sample,subpos))
    {
     ATH_MSG_DEBUG( "Result in sample "<<sample<<"."<<subpos<<": eta="<<m_letaCalo[sample][subpos]<<" phi="<<m_lphiCalo[sample][subpos]<<" r="<<m_lrCalo[sample][subpos]<<" z="<<m_lzCalo[sample][subpos]<<" (ok="<<m_layerCaloOK[sample][subpos]<<")");
     eta_safe[subpos][sample]=m_letaCalo[sample][subpos];
     phi_safe[subpos][sample]=m_lphiCalo[sample][subpos];
     r_safe[subpos][sample]=m_lrCalo[sample][subpos];
     z_safe[subpos][sample]=m_lzCalo[sample][subpos];
    }
    else
     ATH_MSG_DEBUG( "Extrapolation to sample "<<sample<<" failed (ok="<<m_layerCaloOK[sample][subpos]<<")");
   }
  }
 } //inside calo

 m_TTC_back_eta->push_back(eta_safe[CaloSubPos::SUBPOS_EXT]);
 m_TTC_back_phi->push_back(phi_safe[CaloSubPos::SUBPOS_EXT]);
 m_TTC_back_r->push_back(r_safe[CaloSubPos::SUBPOS_EXT]);
 m_TTC_back_z->push_back(z_safe[CaloSubPos::SUBPOS_EXT]);
 m_TTC_entrance_eta->push_back(eta_safe[CaloSubPos::SUBPOS_ENT]);
 m_TTC_entrance_phi->push_back(phi_safe[CaloSubPos::SUBPOS_ENT]);
 m_TTC_entrance_r->push_back(r_safe[CaloSubPos::SUBPOS_ENT]);
 m_TTC_entrance_z->push_back(z_safe[CaloSubPos::SUBPOS_ENT]);

 ATH_MSG_DEBUG("End extrapolate()");
} //extrapolate
*/

/*
void ISF_HitAnalysis::extrapolate_to_ID(const HepMC::GenParticle* part,std::vector<Trk::HitInfo>* hitVector)
{
 ATH_MSG_DEBUG("Start extrapolate_to_ID()");

 m_ptruth_eta=part->momentum().eta();
 m_ptruth_phi=part->momentum().phi();
 m_ptruth_e  =part->momentum().e();
 m_ptruth_pt =part->momentum().perp();
 m_ptruth_p  =part->momentum().rho();
 m_pdgid     =part->pdg_id();

 Amg::Vector3D hitpos(0,0,0);
 Amg::Vector3D hitmom(0,0,0);
 if(rz_cylinder_get_calo_etaphi(hitVector,m_CaloBoundaryR,m_CaloBoundaryZ,hitpos,hitmom))
 {
  ATH_MSG_DEBUG("BOUNDARY ID-CALO eta="<<hitpos.eta()<<" phi="<<hitpos.phi()<<" r="<<hitpos.perp()<<" z="<<hitpos[Amg::z]<<" theta="<<hitpos.theta()<<" ; momentum eta="<<hitmom.eta()<<" phi="<<hitmom.phi()<<" theta="<<hitmom.theta());
  m_TTC_IDCaloBoundary_eta->push_back(hitpos.eta());
  m_TTC_IDCaloBoundary_phi->push_back(hitpos.phi());
  m_TTC_IDCaloBoundary_r->push_back(hitpos.perp());
  m_TTC_IDCaloBoundary_z->push_back(hitpos[Amg::z]);
 }
 else
 {
  ATH_MSG_DEBUG("Extrapolation to IDCaloBoundary failed");
  m_TTC_IDCaloBoundary_eta->push_back(-999.);
  m_TTC_IDCaloBoundary_phi->push_back(-999.);
  m_TTC_IDCaloBoundary_r->push_back(-999.);
  m_TTC_IDCaloBoundary_z->push_back(-999.);
 }

 TVector3 vec(hitpos[Amg::x],hitpos[Amg::y],hitpos[Amg::z]);

 //get the tangentvector on this interaction point:
 //GlobalMomentum* mom=params_on_surface_ID->TrackParameters::momentum().unit() ;
 //Trk::GlobalMomentum* trackmom=params_on_surface_ID->Trk::TrackParameters::momentum();

 if(hitmom.mag()>0)
 {
  //angle between vec and trackmom:
  TVector3 Trackmom(hitmom[Amg::x],hitmom[Amg::y],hitmom[Amg::z]);
  double angle3D=Trackmom.Angle(vec); //isn't this the same as TVector3 vec?
  ATH_MSG_DEBUG("    3D ANGLE "<<angle3D);

  double angleEta=vec.Theta()-Trackmom.Theta();
  ATH_MSG_DEBUG("    ANGLE dTHEA"<<angleEta);

  m_TTC_Angle3D->push_back(angle3D);
  m_TTC_AngleEta->push_back(angleEta);
 }
 else
 {
  m_TTC_Angle3D->push_back(-999.);
  m_TTC_AngleEta->push_back(-999.);
 }
 ATH_MSG_DEBUG("End extrapolate_to_ID()");
} //extrapolate_to_ID
*/

//UPDATED
bool ISF_HitAnalysis::get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector, int sample,int subpos)
{
  m_layerCaloOK[sample][subpos]=false;
  m_letaCalo[sample][subpos]=m_eta_calo_surf;
  m_lphiCalo[sample][subpos]=m_phi_calo_surf;
  m_lrCalo[sample][subpos] = rpos(sample,m_eta_calo_surf,subpos);
  m_lzCalo[sample][subpos] = zpos(sample,m_eta_calo_surf,subpos);
  double distsamp =deta(sample,m_eta_calo_surf);
  double lrzpos =rzpos(sample,m_eta_calo_surf,subpos);
  //double hitdist=0;
  bool best_found=false;
  double best_target=0;

  std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
  while ( it!= hitVector->end() && it->detID != (3000+sample) ) { it++;}
  //while ((*it).detID != (3000+sample) && it < hitVector->end() )  it++;

  if (it!=hitVector->end()) {
    Amg::Vector3D hitPos1 = (*it).trackParms->position();
    int sid1=(*it).detID;
    int sid2=-1;
    Amg::Vector3D hitPos;
    Amg::Vector3D hitPos2;

    std::vector<Trk::HitInfo>::iterator itnext = it;
    ++itnext;
    if(itnext!=hitVector->end()) {
      hitPos2 = (*itnext).trackParms->position();
      sid2=(*itnext).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      if(isCaloBarrel(sample)) {
        double r=rpos(sample,eta_avg,subpos);
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
        best_target=r;
      } else {
        double z=zpos(sample,eta_avg,subpos);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        best_target=z;
      }
      hitPos=t*hitPos2+(1-t)*hitPos1;
    } else {
      hitPos=hitPos1;
      hitPos2=hitPos1;
    }
    double etaCalo = hitPos.eta();
    double phiCalo = hitPos.phi();
    m_letaCalo[sample][subpos]=etaCalo;
    m_lphiCalo[sample][subpos]=phiCalo;
    m_lrCalo[sample][subpos] = hitPos.perp();
    m_lzCalo[sample][subpos] = hitPos[Amg::z];
    //hitdist=hitPos.mag();
    m_layerCaloOK[sample][subpos]=true;
    lrzpos=rzpos(sample,etaCalo,subpos);
    distsamp=deta(sample,etaCalo);
    best_found=true;
    ATH_MSG_DEBUG(" extrapol with layer hit: id="<<sid1<<" -> "<<sid2<<
                  " target r/z="<<best_target<<
                  " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                  " re="<<m_lrCalo[sample][subpos]<<" ze="<<m_lzCalo[sample][subpos]
                  );
  }
  if(!best_found) {
    it = hitVector->begin();
    double best_dist=0.5;
    bool best_inside=false;
    int best_id1,best_id2;
    Amg::Vector3D best_hitPos=(*it).trackParms->position();
    while (it < hitVector->end()-1 ) {
      Amg::Vector3D hitPos1 = (*it).trackParms->position();
      int sid1=(*it).detID;
      it++;
      Amg::Vector3D hitPos2 = (*it).trackParms->position();
      int sid2=(*it).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      double tmp_target=0;
      if(isCaloBarrel(sample)) {
        double r=rpos(sample,eta_avg,subpos);
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
        tmp_target=r;
      } else {
        double z=zpos(sample,eta_avg,subpos);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        tmp_target=z;
      }
      Amg::Vector3D hitPos=t*hitPos2+(1-t)*hitPos1;
      double dist=TMath::Min(TMath::Abs(t-0),TMath::Abs(t-1));
      bool inside=false;
      if(t>=0 && t<=1) inside=true;
      if(!best_found || inside) {
        if(!best_inside || dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      } else {
        if(!best_inside && dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      }
      ATH_MSG_VERBOSE(" extrapol without layer hit: id="<<sid1<<" -> "<<sid2<<" dist="<<dist<<" mindist="<<best_dist<<
                    " t="<<t<<" best_inside="<<best_inside<<" target r/z="<<tmp_target<<
                    " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                    " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
      if(best_found) {
        m_letaCalo[sample][subpos]=best_hitPos.eta();
        m_lphiCalo[sample][subpos]=best_hitPos.phi();
        m_lrCalo[sample][subpos] = best_hitPos.perp();
        m_lzCalo[sample][subpos] = best_hitPos[Amg::z];
        //hitdist=best_hitPos.mag();
        lrzpos=rzpos(sample,m_letaCalo[sample][subpos],subpos);
        distsamp=deta(sample,m_letaCalo[sample][subpos]);
        m_layerCaloOK[sample][subpos]=true;
      }
    }
    if(best_found) {
      ATH_MSG_DEBUG(" extrapol without layer hit: id="<<best_id1<<" -> "<<best_id2<<" mindist="<<best_dist<<
                    " best_inside="<<best_inside<<" target r/z="<<best_target<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
    }
  }

  if(isCaloBarrel(sample)) lrzpos*=cosh(m_letaCalo[sample][subpos]);
   else                    lrzpos= fabs(lrzpos/tanh(m_letaCalo[sample][subpos]));

  m_dCalo[sample][subpos]=lrzpos;
  m_distetaCaloBorder[sample][subpos]=distsamp;

  return m_layerCaloOK[sample][subpos];
} //get_calo_etaphi

//UPDATED
bool ISF_HitAnalysis::rz_cylinder_get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector, double cylR, double cylZ, Amg::Vector3D& pos, Amg::Vector3D& mom)
{
  bool best_found=false;
  double best_dist=10000;
  bool best_inside=false;
  int best_id1,best_id2;

  std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
  Amg::Vector3D best_hitPos=(*it).trackParms->position();
  for(int rz=0;rz<=1;++rz) {
    it = hitVector->begin();
    while (it < hitVector->end()-1 ) {
      Amg::Vector3D hitPos1 = (*it).trackParms->position();
      Amg::Vector3D hitMom1 = (*it).trackParms->momentum();
      int sid1=(*it).detID;
      it++;
      Amg::Vector3D hitPos2 = (*it).trackParms->position();
      Amg::Vector3D hitMom2 = (*it).trackParms->momentum();
      int sid2=(*it).detID;

      double t;
      if(rz==1) {
        double r=cylR;
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
      } else {
        double z=cylZ;
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
      }
      Amg::Vector3D hitPos=t*hitPos2+(1-t)*hitPos1;

      double dist=hitPos.mag();
      bool inside=false;
      if(t>=-0.001 && t<=1.001) inside=true;

      if(!best_found || inside) {
        if(!best_inside || dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          mom=t*hitMom2+(1-t)*hitMom1;
        }
      } else {
        if(!best_inside && dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          mom=t*hitMom2+(1-t)*hitMom1;
        }
      }
      ATH_MSG_DEBUG(" extrapol without layer hit: id="<<sid1<<" -> "<<sid2<<" dist="<<dist<<" bestdist="<<best_dist<<
                    " t="<<t<<" inside="<<inside<<" best_inside="<<best_inside<<
                    " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                    " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
    }
  }

  if(best_found) {
    ATH_MSG_DEBUG(" extrapol to r="<<cylR<<" z="<<cylZ<<": id="<<best_id1<<" -> "<<best_id2<<" dist="<<best_dist<<
                  " best_inside="<<best_inside<<
                  " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                  );
  }
  pos=best_hitPos;

  return best_found;
} //rz_cylinder_get_calo_etaphi

//UPDATED
/*
bool ISF_HitAnalysis::get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector, CaloCell_ID_FCS::CaloSample sample)
{
  m_layerCaloOK[sample]=false;
  m_letaCalo[sample]=m_eta_calo_surf;
  m_lphiCalo[sample]=m_phi_calo_surf;
  double distsamp =deta(sample,m_eta_calo_surf);
  double rzmiddle =rzmid(sample,m_eta_calo_surf);
  double hitdist=0;
  bool best_found=false;
  double best_target=0;

  std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
  while ( it!= hitVector->end() && it->detID != (3000+sample) ) { it++;}
  //while ((*it).detID != (3000+sample) && it < hitVector->end() )  it++;

  if (it!=hitVector->end()) {
    Amg::Vector3D hitPos1 = (*it).trackParms->position();
    int sid1=(*it).detID;
    int sid2=-1;
    Amg::Vector3D hitPos;
    Amg::Vector3D hitPos2;

    std::vector<Trk::HitInfo>::iterator itnext = it;
    ++itnext;
    if(itnext!=hitVector->end()) {
      hitPos2 = (*itnext).trackParms->position();
      sid2=(*itnext).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      if(isCaloBarrel(sample)) {
        double r=rmid(sample,eta_avg);
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
        best_target=r;
      } else {
        double z=zmid(sample,eta_avg);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        best_target=z;
      }
      hitPos=t*hitPos2+(1-t)*hitPos1;
    } else {
      hitPos=hitPos1;
      hitPos2=hitPos1;
    }
    double etaCalo = hitPos.eta();
    double phiCalo = hitPos.phi();
    m_letaCalo[sample]=etaCalo;
    m_lphiCalo[sample]=phiCalo;
    hitdist=hitPos.mag();
    m_layerCaloOK[sample]=true;
    rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
    distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
    best_found=true;
    ATH_MSG_DEBUG(" extrapol with layer hit: id="<<sid1<<" -> "<<sid2<<
                  " target r/z="<<best_target<<
                  " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                  " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]
                  );
  }
  if(!best_found) {
    it = hitVector->begin();
    double best_dist=0.5;
    bool best_inside=false;
    int best_id1,best_id2;
    Amg::Vector3D best_hitPos=(*it).trackParms->position();
    while (it < hitVector->end()-1 ) {
      Amg::Vector3D hitPos1 = (*it).trackParms->position();
      int sid1=(*it).detID;
      it++;
      Amg::Vector3D hitPos2 = (*it).trackParms->position();
      int sid2=(*it).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      double tmp_target=0;
      if(isCaloBarrel(sample)) {
        double r=rmid(sample,eta_avg);
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
        tmp_target=r;
      } else {
        double z=zmid(sample,eta_avg);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        tmp_target=z;
      }
      Amg::Vector3D hitPos=t*hitPos2+(1-t)*hitPos1;
      double dist=TMath::Min(TMath::Abs(t-0),TMath::Abs(t-1));
      bool inside=false;
      if(t>=0 && t<=1) inside=true;
      if(!best_found || inside) {
        if(!best_inside || dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      } else {
        if(!best_inside && dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      }
      ATH_MSG_VERBOSE(" extrapol without layer hit: id="<<sid1<<" -> "<<sid2<<" dist="<<dist<<" mindist="<<best_dist<<
                    " t="<<t<<" best_inside="<<best_inside<<" target r/z="<<tmp_target<<
                    " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                    " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
      if(best_found) {
        m_letaCalo[sample]=best_hitPos.eta();
        m_lphiCalo[sample]=best_hitPos.phi();
        hitdist=best_hitPos.mag();
        rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,m_letaCalo[sample]);
        distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,m_letaCalo[sample]);
        m_layerCaloOK[sample]=true;
      }
    }
    if(best_found) {
      ATH_MSG_DEBUG(" extrapol without layer hit: id="<<best_id1<<" -> "<<best_id2<<" mindist="<<best_dist<<
                    " best_inside="<<best_inside<<" target r/z="<<best_target<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
    }
  }

  if(isCaloBarrel((CaloCell_ID_FCS::CaloSample)sample)) rzmiddle*=cosh(m_letaCalo[sample]);
   else                                                 rzmiddle= fabs(rzmiddle/tanh(m_letaCalo[sample]));

  m_dCalo[sample]=rzmiddle;
  m_distetaCaloBorder[sample]=distsamp;

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)<<"  Final par TTC sample "<<(int)sample;
    if(m_layerCaloOK[sample]) msg()<<" (good)";
     else msg()<<" (bad)";
    msg()<<" eta=" << m_letaCalo[sample] << "   phi=" << m_lphiCalo[sample] <<" m_dCalo="<<m_dCalo[sample]<<" dist(hit)="<<hitdist<< endmsg;
  }

  return m_layerCaloOK[sample];
}
*/

//UPDATED
bool ISF_HitAnalysis::get_calo_surface(std::vector<Trk::HitInfo>* hitVector)
{
  ATH_MSG_DEBUG("Start get_calo_surface()");
  m_sample_calo_surf=CaloCell_ID_FCS::noSample;
  m_eta_calo_surf=-999;
  m_phi_calo_surf=-999;
  m_d_calo_surf=0;
  double min_calo_surf_dist=1000;

  for(unsigned int i=0;i<m_surfacelist.size();++i) {
    CaloCell_ID_FCS::CaloSample sample=m_surfacelist[i];
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while (it != hitVector->end() && it->detID != (3000+sample) )  { it++;}
    if(it==hitVector->end()) continue;
    Amg::Vector3D hitPos = (*it).trackParms->position();

    //double offset = 0.;
    double etaCalo = hitPos.eta();

     ATH_MSG_DEBUG("test: entrance to calo surface : sample="<<sample<<" eta="<<etaCalo);

    if (fabs(etaCalo)<900) {
      double phiCalo = hitPos.phi();
      double distsamp =deta(sample,etaCalo);

      ATH_MSG_DEBUG(" phi="<<phiCalo<<" dist="<<distsamp);
      if(distsamp<min_calo_surf_dist && min_calo_surf_dist>=0)
      {
        //hitVector is ordered in r, so if first surface was hit, keep it
        m_eta_calo_surf=etaCalo;
        m_phi_calo_surf=phiCalo;
        min_calo_surf_dist=distsamp;
        m_sample_calo_surf=sample;
        m_d_calo_surf=rzent(sample,etaCalo);
        ATH_MSG_DEBUG(" r/z="<<m_d_calo_surf);
        if(isCaloBarrel(sample)) m_d_calo_surf*=cosh(etaCalo);
        else                    m_d_calo_surf= fabs(m_d_calo_surf/tanh(etaCalo));
        ATH_MSG_DEBUG(" d="<<m_d_calo_surf);
        if(distsamp<0)
        {
          break;
        }
      }
    }
    else
    {
     ATH_MSG_DEBUG(": eta > 900, not using this");
    }
  }

  if(m_sample_calo_surf==CaloCell_ID_FCS::noSample) {
    // first intersection with sensitive calo layer
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while ( it < hitVector->end() && (*it).detID != 3 ) { it++;}   // to be updated
    if (it==hitVector->end())   {  // no calo intersection, abort
      return false;
    }
    Amg::Vector3D surface_hitPos = (*it).trackParms->position();
    m_eta_calo_surf=surface_hitPos.eta();
    m_phi_calo_surf=surface_hitPos.phi();
    m_d_calo_surf=surface_hitPos.mag();

    double pT=(*it).trackParms->momentum().perp();
    if(TMath::Abs(m_eta_calo_surf)>4.9 || pT<500 || (TMath::Abs(m_eta_calo_surf)>4 && pT<1000) ) {
      ATH_MSG_DEBUG("only entrance to calo entrance layer found, no surface : eta="<<m_eta_calo_surf<<" phi="<<m_phi_calo_surf<<" d="<<m_d_calo_surf<<" pT="<<pT);
    } else {
      ATH_MSG_WARNING("only entrance to calo entrance layer found, no surface : eta="<<m_eta_calo_surf<<" phi="<<m_phi_calo_surf<<" d="<<m_d_calo_surf<<" pT="<<pT);
    }
  } else {
    ATH_MSG_DEBUG("entrance to calo surface : sample="<<m_sample_calo_surf<<" eta="<<m_eta_calo_surf<<" phi="<<m_phi_calo_surf<<" deta="<<min_calo_surf_dist<<" dsurf="<<m_d_calo_surf);
  }

  ATH_MSG_DEBUG("End get_calo_surface()");
  return true;
}



bool ISF_HitAnalysis::isCaloBarrel(int sample) const
{
  return GetCaloGeometry()->isCaloBarrel(sample);
}

double ISF_HitAnalysis::deta(int sample,double eta) const
{
  return GetCaloGeometry()->deta(sample,eta);
}

void ISF_HitAnalysis::minmaxeta(int sample,double eta,double& mineta,double& maxeta) const
{
  GetCaloGeometry()->minmaxeta(sample,eta,mineta,maxeta);
}

double ISF_HitAnalysis::rmid(int sample,double eta) const
{
  return GetCaloGeometry()->rmid(sample,eta);
}

double ISF_HitAnalysis::zmid(int sample,double eta) const
{
  return GetCaloGeometry()->zmid(sample,eta);
}

double ISF_HitAnalysis::rzmid(int sample,double eta) const
{
  return GetCaloGeometry()->rzmid(sample,eta);
}

double ISF_HitAnalysis::rent(int sample,double eta) const
{
  return GetCaloGeometry()->rent(sample,eta);
}

double ISF_HitAnalysis::zent(int sample,double eta) const
{
  return GetCaloGeometry()->zent(sample,eta);
}

double ISF_HitAnalysis::rzent(int sample,double eta) const
{
  return GetCaloGeometry()->rzent(sample,eta);
}

double ISF_HitAnalysis::rext(int sample,double eta) const
{
  return GetCaloGeometry()->rext(sample,eta);
}

double ISF_HitAnalysis::zext(int sample,double eta) const
{
  return GetCaloGeometry()->zext(sample,eta);
}

double ISF_HitAnalysis::rzext(int sample,double eta) const
{
  return GetCaloGeometry()->rzext(sample,eta);
}

double ISF_HitAnalysis::rpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->rpos(sample,eta,subpos);
}

double ISF_HitAnalysis::zpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->zpos(sample,eta,subpos);
}

double ISF_HitAnalysis::rzpos(int sample,double eta,int subpos) const
{
  return GetCaloGeometry()->rzpos(sample,eta,subpos);
}
