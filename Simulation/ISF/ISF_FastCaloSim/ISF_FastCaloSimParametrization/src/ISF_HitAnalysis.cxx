/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

#include "ISF_FastCaloSimParametrization/ISF_HitAnalysis.h"
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
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
//#########################


#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>


const std::string MC_DIGI_PARAM = "/Digitization/Parameters"; 
const std::string MC_SIM_PARAM = "/Simulation/Parameters";

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

   //######################
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
   , m_extrapolatorName("Trk::Extrapolator")   
   , m_extrapolatorInstanceName("Extrapolator")
   //######################


  //Note that m_xxx are pointers to vectors set to 0, not set to empty vector! see note around TBranch
{
  declareProperty("NtupleFileName", m_ntupleFileName); 
  declareProperty("NtupleDirectoryName", m_ntupleDirName); 
  declareProperty("NtupleTreeName", m_ntupleTreeName); 
  declareProperty("GeoFileName", m_geoFileName); 
  declareProperty("MetadataTreeName", m_metadataTreeName); 
  declareProperty("NTruthParticles", m_NtruthParticles=1, "Number of truth particles saved from the truth collection, -1 to save all");

  //###########################
  declareProperty("ExtrapolatorName",               m_extrapolatorName );
  declareProperty("ExtrapolatorInstanceName",       m_extrapolatorInstanceName );
  declareProperty("CalosurfMiddleInstanceName",     m_calosurf_InstanceName);
  declareProperty("CalosurfEntranceInstanceName",   m_calosurf_entrance_InstanceName);

  declareProperty("CaloBoundaryR", m_CaloBoundaryR);
  declareProperty("CaloBoundaryZ", m_CaloBoundaryZ);
  declareProperty("CaloMargin", m_calomargin);
  //######################

}


ISF_HitAnalysis::~ISF_HitAnalysis() 
{
}

StatusCode ISF_HitAnalysis::geoInit(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO("geoInit for " << m_geoModel->atlasVersion() );

  StatusCode sc = detStore()->retrieve(m_tileMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve TileDetDescrManager from DetectorStore" );
    m_tileMgr=0;
  }

  sc = detStore()->retrieve(m_tileID);
  if (sc.isFailure()) { 
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
    	  
    // Get TrkExtrapolator from ToolService
    if (p_toolSvc->retrieveTool(m_extrapolatorName, m_extrapolatorInstanceName , m_extrapolator).isFailure())
    {
	   ATH_MSG_ERROR("Could not find Tool " << m_extrapolatorName << " and create instance " << m_extrapolatorInstanceName <<". Exiting.");
	   return StatusCode::FAILURE;
    } else
    {
     ATH_MSG_INFO("Successfully created tool  " << m_extrapolatorName << " with instance name " << m_extrapolatorInstanceName);
    }
    
    // Get the CaloSurface tool and sets the depthtool it should use:
    if (p_toolSvc->retrieveTool("CaloSurfaceBuilder",m_calosurf_InstanceName,m_calosurf,this).isFailure())
    {
     ATH_MSG_ERROR("Could not find Tool "<< m_calosurf_InstanceName);
     return StatusCode::FAILURE;
    }     
    //#if FastCaloSim_project_release_v1 == 12
    m_calosurf->setCaloDepth(m_calodepth);
    //#endif 

    // Get the CaloSurface tool and sets the depthtool it should use:
    if (p_toolSvc->retrieveTool("CaloSurfaceBuilder",m_calosurf_entrance_InstanceName,m_calosurf_entrance,this).isFailure())
    {
     ATH_MSG_ERROR("Could not find Tool "<< m_calosurf_entrance_InstanceName);
     return StatusCode::FAILURE;
    }     
    //#if FastCaloSim_project_release_v1 == 12
    m_calosurf_entrance->setCaloDepth(m_calodepthEntrance);
    //#endif  
    
    std::string CaloCoordinateTool_name="TBCaloCoordinate";
    ListItem CaloCoordinateTool(CaloCoordinateTool_name);
    if ( p_toolSvc->retrieveTool(CaloCoordinateTool.type(),CaloCoordinateTool.name(), algTool, this).isFailure() )
    {
     ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
     return StatusCode::FAILURE;
    }
    m_calo_tb_coord = dynamic_cast<ICaloCoordinateTool*>(algTool);   
    if ( !m_calo_tb_coord )
    {
     ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
     return StatusCode::FAILURE;
    }
    else
    {
     ATH_MSG_INFO("retrieved " << CaloCoordinateTool_name);
    }
    
  } //tools
  
  return StatusCode::SUCCESS;
}

StatusCode ISF_HitAnalysis::updateMetaData( IOVSVC_CALLBACK_ARGS_P( I, keys ) ) {
  ATH_MSG_INFO( "Updating the Sim+Digi MetaData" );
  
  // Reset the internal settings:
  bool run_update = false;

  // Check what kind of keys we got. In principle the function should only
  // receive the "/Digitization/Parameters" and "/Simulation/Parameters" key.
  msg( MSG::DEBUG ) << "Update called with " <<I<< " folder " << keys.size() << " keys:";
  std::list< std::string >::const_iterator itr = keys.begin();
  std::list< std::string >::const_iterator end = keys.end();
  for( ; itr != end; ++itr ) {
     if( *itr == MC_DIGI_PARAM ) run_update = true;
     if( *itr == MC_SIM_PARAM ) run_update = true;
     msg() << *itr;
  }
  msg() << endreq;
  // If that's not the key that we received after all, let's just return
  // silently...
  if( ! run_update ) return StatusCode::SUCCESS;

  const DataHandle< AthenaAttributeList > simParam;
  if( detStore()->retrieve( simParam, MC_SIM_PARAM ).isFailure() ) {
    ATH_MSG_WARNING("Retrieving MC SIM metadata failed");
  } else {
    AthenaAttributeList::const_iterator attr_itr = simParam->begin();
    AthenaAttributeList::const_iterator attr_end = simParam->end();
    for( ; attr_itr != attr_end; ++attr_itr ) {
      std::stringstream outstr;
      attr_itr->toOutputStream(outstr);
      ATH_MSG_INFO("MetaData: " << outstr.str());
    }
  }  

  return StatusCode::SUCCESS;
}


StatusCode ISF_HitAnalysis::initialize() {
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

  if( detStore()->contains< AthenaAttributeList >( MC_DIGI_PARAM ) ) {
    const DataHandle< AthenaAttributeList > aptr;
    if( detStore()->regFcn( &ISF_HitAnalysis::updateMetaData, this, aptr,MC_DIGI_PARAM, true ).isFailure() ) {
      ATH_MSG_ERROR( "Could not register callback for "<< MC_DIGI_PARAM );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_WARNING( "MetaData not found for "<< MC_DIGI_PARAM );
  }
  
  if( detStore()->contains< AthenaAttributeList >( MC_SIM_PARAM ) ) {
    const DataHandle< AthenaAttributeList > aptr;
    if( detStore()->regFcn( &ISF_HitAnalysis::updateMetaData, this, aptr,MC_SIM_PARAM, true ).isFailure() ) {
      ATH_MSG_ERROR( "Could not register callback for "<< MC_SIM_PARAM );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_WARNING( "MetaData not found for "<< MC_SIM_PARAM );
  }
  
  // Grab the Ntuple and histogramming service for the tree
  sc = service("THistSvc",m_thistSvc);
  if (sc.isFailure()) {
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
  if (sc.isFailure() || !m_tree ) { 
    ATH_MSG_ERROR("Unable to register TTree: " << fullNtupleName); 
    return StatusCode::FAILURE;
  }
  
  /** now add branches and leaves to the tree */
  if (m_tree){
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
    //#########################
    //m_tree->Print();
  }
  
  return StatusCode::SUCCESS;
}		 

StatusCode ISF_HitAnalysis::finalize() {
  ATH_MSG_INFO( "doing finalize()" );

  TTree* m_geo = new TTree( m_geoModel->atlasVersion().c_str() , m_geoModel->atlasVersion().c_str() );
  std::string fullNtupleName =  "/"+m_geoFileName+"/"+m_geoModel->atlasVersion(); 
  StatusCode sc = m_thistSvc->regTree(fullNtupleName, m_geo); 
  if (sc.isFailure() || !m_geo ) { 
    ATH_MSG_ERROR("Unable to register TTree: " << fullNtupleName); 
    return StatusCode::FAILURE;
  }
  
  /** now add branches and leaves to the tree */
  
  typedef struct {
    Long64_t identifier;
    Int_t calosample;
    float eta,phi,r,eta_raw,phi_raw,r_raw,x,y,z,x_raw,y_raw,z_raw;
    float deta,dphi,dr,dx,dy,dz;
  } GEOCELL;
  
  static GEOCELL geocell;  
  
  if (m_geo){
    ATH_MSG_INFO("Successfull registered TTree: " << fullNtupleName); 
    //this actually creates the vector itself! And only if it succeeds! Note that the result is not checked! And the code is probably leaking memory in the end
    //m_geo->Branch("cells", &geocell,"identifier/L:eta,phi,r,eta_raw,phi_raw,r_raw,x,y,z,x_raw,y_raw,z_raw/F:Deta,Dphi,Dr,Dx,Dy,Dz/F");
    m_geo->Branch("identifier", &geocell.identifier,"identifier/L");
    m_geo->Branch("calosample", &geocell.calosample,"calosample/I");

    m_geo->Branch("eta", &geocell.eta,"eta/F");
    m_geo->Branch("phi", &geocell.phi,"phi/F");
    m_geo->Branch("r", &geocell.r,"r/F");
    m_geo->Branch("eta_raw", &geocell.eta_raw,"eta_raw/F");
    m_geo->Branch("phi_raw", &geocell.phi_raw,"phi_raw/F");
    m_geo->Branch("r_raw", &geocell.r_raw,"r_raw/F");

    m_geo->Branch("x", &geocell.x,"x/F");
    m_geo->Branch("y", &geocell.y,"y/F");
    m_geo->Branch("z", &geocell.z,"z/F");
    m_geo->Branch("x_raw", &geocell.x_raw,"x_raw/F");
    m_geo->Branch("y_raw", &geocell.y_raw,"y_raw/F");
    m_geo->Branch("z_raw", &geocell.z_raw,"z_raw/F");

    m_geo->Branch("deta", &geocell.deta,"deta/F");
    m_geo->Branch("dphi", &geocell.dphi,"dphi/F");
    m_geo->Branch("dr", &geocell.dr,"dr/F");
    m_geo->Branch("dx", &geocell.dx,"dx/F");
    m_geo->Branch("dy", &geocell.dy,"dy/F");
    m_geo->Branch("dz", &geocell.dz,"dz/F");
  }
  

  if(m_calo_dd_man) {
    int ncells=0;
    for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_calo_dd_man->element_begin();calo_iter<m_calo_dd_man->element_end();++calo_iter) {
      const CaloDetDescrElement* theDDE=*calo_iter;
      if(theDDE) {
        CaloCell_ID::CaloSample sample=theDDE->getSampling();
        //CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
        ++ncells;
        if(m_geo) {
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

          m_geo->Fill();
        }
      }  
    }

    ATH_MSG_INFO( ncells<<" cells found" );
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF_HitAnalysis::execute() {
  ATH_MSG_DEBUG( "In ISF_HitAnalysis::execute()" );
  
  if (! m_tree) {
    ATH_MSG_ERROR( "tree not registered" );
    return StatusCode::FAILURE;
  }
  
  //now if the branches were created correctly, the pointers point to something and it is possible to clear the vectors 
  TVector3 vectest;
  vectest.SetPtEtaPhi(1.,1.,1.);
  if(m_hit_x) m_hit_x->clear();
  if(m_hit_y) m_hit_y->clear();
  if(m_hit_z) m_hit_z->clear();
  if(m_hit_energy) m_hit_energy->clear();
  if(m_hit_time) m_hit_time->clear();
  if(m_hit_identifier) m_hit_identifier->clear();
  if(m_hit_cellidentifier) m_hit_cellidentifier->clear();
  if(m_islarbarrel) m_islarbarrel->clear();
  if(m_islarendcap) m_islarendcap->clear();
  if(m_islarhec) m_islarhec->clear();
  if(m_islarfcal) m_islarfcal->clear();
  if(m_istile) m_istile->clear();
  if(m_hit_sampling) m_hit_sampling->clear();
  if(m_hit_samplingfraction) m_hit_samplingfraction->clear();
  if(m_truth_energy) m_truth_energy->clear();
  if(m_truth_px) m_truth_px->clear();
  if(m_truth_py) m_truth_py->clear();
  if(m_truth_pz) m_truth_pz->clear();
  if(m_truth_pdg) m_truth_pdg->clear();
  if(m_truth_barcode) m_truth_barcode->clear();
  if(m_truth_vtxbarcode) m_truth_vtxbarcode->clear();
  if(m_cell_identifier) m_cell_identifier->clear();
  if(m_cell_energy) m_cell_energy->clear();
  if(m_cell_sampling) m_cell_sampling->clear();
  if(m_g4hit_energy) m_g4hit_energy->clear();
  if(m_g4hit_time) m_g4hit_time->clear();
  if(m_g4hit_identifier) m_g4hit_identifier->clear();
  if(m_g4hit_cellidentifier) m_g4hit_cellidentifier->clear();
  if(m_g4hit_sampling) m_g4hit_sampling->clear();
  if(m_g4hit_samplingfraction) m_g4hit_samplingfraction->clear();
  //which fails for this one!!
  //m_matched_cells->clear();
  
  //##########################
  if(m_TTC_back_eta) m_TTC_back_eta->clear();
  if(m_TTC_back_phi) m_TTC_back_phi->clear();
  if(m_TTC_back_r) m_TTC_back_r->clear();
  if(m_TTC_back_z) m_TTC_back_z->clear();
  if(m_TTC_entrance_eta) m_TTC_entrance_eta->clear();
  if(m_TTC_entrance_phi) m_TTC_entrance_phi->clear();
  if(m_TTC_entrance_r) m_TTC_entrance_r->clear();
  if(m_TTC_entrance_z) m_TTC_entrance_z->clear();

  if(m_TTC_IDCaloBoundary_eta) m_TTC_IDCaloBoundary_eta->clear();
  if(m_TTC_IDCaloBoundary_phi) m_TTC_IDCaloBoundary_phi->clear();
  if(m_TTC_IDCaloBoundary_r) m_TTC_IDCaloBoundary_r->clear();
  if(m_TTC_IDCaloBoundary_z) m_TTC_IDCaloBoundary_z->clear();
  
  if(m_TTC_Angle3D) m_TTC_Angle3D->clear();
  if(m_TTC_AngleEta) m_TTC_AngleEta->clear();
  //##########################
  
  //Get the FastCaloSim step info collection from store
  const ISF_FCS_Parametrization::FCS_StepInfoCollection* eventStepsES;
  StatusCode sc = evtStore()->retrieve(eventStepsES, "ZHMergedEventSteps");
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
	  
	  if (m_calo_dd_man->get_element(id))
	    {
	      CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(id)->getSampling();
	      sampling = layer; //use CaloCell layer immediately
	    }
	  else
	    {
	      ATH_MSG_WARNING( "Warning no sampling info for "<<id.getString());
	    }
	  
	  if (m_larEmID->is_lar_em(id) || m_larHecID->is_lar_hec(id) || m_larFcalID->is_lar_fcal(id))
	    {
	      sampfrac=m_dd_fSampl->FSAMPL(id);
	    }
	  if (m_larEmID->is_lar_em(id))
	    {
	      //std::cout <<"This hit is in LAr EM ";
	      if (m_larEmID->is_em_barrel(id))
		{
		  larbarrel=true;
		}
	      else if (m_larEmID->is_em_endcap(id))
		{
		  larendcap=true;
		}
	    }
	  else if (m_larHecID->is_lar_hec(id))
	    {
	      larhec = true;
	    }
	  else if (m_larFcalID->is_lar_fcal(id))
	    {
	      larfcal = true;
	    }
	  else if (m_tileID->is_tile_barrel(id) || m_tileID->is_tile_extbarrel(id) || m_tileID->is_tile_gap(id) || m_tileID->is_tile_gapscin(id) || m_tileID->is_tile_aux(id))
	    {
	      cell_id = m_tileID->cell_id(id);
	      Int_t tile_sampling = -1;
	      if (m_calo_dd_man->get_element(cell_id))
		{
		  tile_sampling = m_calo_dd_man->get_element(cell_id)->getSampling();
		  sampfrac = m_tileInfo->HitCalib(cell_id);
		}
	      tile = true;
	      if (tile_sampling!= -1) sampling = tile_sampling; //m_calo_dd_man needs to be called with cell_id not pmt_id!!
	    }
	  else
	    {
	      ATH_MSG_WARNING( "This hit is somewhere. Please check!");
	    }	  
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
  if (sc.isFailure())
    {
      ATH_MSG_WARNING( "No truth event!");
    }
  else
    {
      if (mcEvent)
	{
	  //std::cout<<"ISF_HitAnalysis: MC event size: "<<mcEvent->size()<<std::endl;
	  if (mcEvent->size())
	    {
	    	int particleIndex=0;
		int loopEnd = m_NtruthParticles;
		if (loopEnd==-1)
		  {
		    loopEnd = (*mcEvent->begin())->particles_size(); //is this the correct thing?
		  }
	      //std::cout <<"ISF_HitAnalysis: MC first truth event size: "<<(*mcEvent->begin())->particles_size()<<std::endl;
	      for (HepMC::GenEvent::particle_const_iterator it = (*mcEvent->begin())->particles_begin(); it != (*mcEvent->begin())->particles_end(); ++it)
		{
		  particleIndex++;
		  if (particleIndex>loopEnd) break; //enough particles
		  m_truth_energy->push_back((*it)->momentum().e());
		  m_truth_px->push_back((*it)->momentum().px());
		  m_truth_py->push_back((*it)->momentum().py());
		  m_truth_pz->push_back((*it)->momentum().pz());
		  m_truth_pdg->push_back((*it)->pdg_id());
		  m_truth_barcode->push_back((*it)->barcode());
		  
      		  //extrapolate only if the vertex is within the IDcaloboundary - margin
		  //bool inside_ID=false;
		  HepMC::GenVertex* pvtx = (*it)->production_vertex();
		  if (pvtx)
		    {
		      m_truth_vtxbarcode->push_back(pvtx->barcode());
		      double radius2=(pvtx->position().x()*pvtx->position().x())+(pvtx->position().y()*pvtx->position().y());
		      ATH_MSG_VERBOSE("particle "<<particleIndex<<" Mom: "<<(*it)->momentum().e()<<"|"<<(*it)->momentum().px()<<"|"<<(*it)->momentum().py()<<"|"<<(*it)->momentum().pz()<<" vertex_x "<<pvtx->position().x()<<" y "<<pvtx->position().y()<<" z "<<pvtx->position().z()<<" r2 "<<radius2<<" VertexBarcode: "<<pvtx->barcode()<<" ParticleBarcode:"<<(*it)->barcode()<<" status: "<<(*it)->status());
		      //std::cout<<"m_CaloBoundaryZ+m_calomargin "<<m_CaloBoundaryZ+m_calomargin<<" m_CaloBoundaryR+m_calomargin "<<m_CaloBoundaryR+m_calomargin<<std::endl;
		      if(fabs(pvtx->position().z())<(m_CaloBoundaryZ+m_calomargin) && radius2<((m_CaloBoundaryR+m_calomargin)*(m_CaloBoundaryR+m_calomargin))) 
			{
			  //inside_ID=true;
			  //if(inside_ID)
			  //  {
			  ATH_MSG_DEBUG( "Do extrapolation");
			  extrapolate(*it);
			  ATH_MSG_DEBUG(" Do extrapolation to ID");
			  extrapolate_to_ID(*it);
			  ATH_MSG_DEBUG("Truth extrapolation done for "<<particleIndex<<" "<<(*it)->barcode());
			}
		      else
			{
			  //prefill extrapolated angles with some weird values
			  m_TTC_IDCaloBoundary_eta->push_back(-999.);
			  m_TTC_IDCaloBoundary_phi->push_back(-999.);
			  m_TTC_IDCaloBoundary_r->push_back(-999.);
			  m_TTC_IDCaloBoundary_z->push_back(-999.);
			  m_TTC_Angle3D->push_back(-999.);
			  m_TTC_AngleEta->push_back(-999.);
			}
		    }
		  else
		    {
		      ATH_MSG_WARNING( "No vertex found for truth particle, no extrapolation");
		      //pvtx failed??		      
		      //prefill extrapolated angles with some weird values
		      m_truth_vtxbarcode->push_back(-999999);
		      //These are for each layer
		      std::vector<double> eta_safe;
		      std::vector<double> phi_safe;
		      std::vector<double> r_safe;
		      std::vector<double> z_safe;
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
		    }	  
		}
	    }
	}
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
      for ( ; itrCell!=itrLastCell; ++itrCell){
	m_cell_energy->push_back((*itrCell)->energy());
	m_cell_identifier->push_back((*itrCell)->ID().get_compact());
	if (m_calo_dd_man->get_element((*itrCell)->ID()))
	  {
	    CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element((*itrCell)->ID())->getSampling();
	    m_cell_sampling->push_back(layer);
	  }
	  else m_cell_sampling->push_back(-1);

      }
    }//calorimeter cells

  //Get all G4Hits (from CaloHitAnalysis)
  std::string  lArKey [4] = {"LArHitEMB", "LArHitEMEC", "LArHitFCAL", "LArHitHEC"};
  for (unsigned int i=0;i<4;i++){
    const DataHandle<LArHitContainer> iter;
    ATH_MSG_DEBUG( "Checking G4Hits: "<<lArKey[i]);
    if (evtStore()->retrieve(iter,lArKey[i])==StatusCode::SUCCESS) {
      LArHitContainer::const_iterator hi;
      int hitnumber = 0;
      for (hi=(*iter).begin();hi!=(*iter).end();hi++){
	hitnumber++;
	GeoLArHit ghit(**hi);
        if (!ghit) continue;
        const CaloDetDescrElement *hitElement = ghit.getDetDescrElement();
	if (!hitElement) continue;
	Identifier larhitid = hitElement->identify();
	if (m_calo_dd_man->get_element(larhitid))
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
    }else
      {
       	ATH_MSG_INFO( "Can't retrieve LAr hits");
      }    // End statuscode success upon retrieval of hits
    //std::cout <<"ZH G4Hit size: "<<m_g4hit_e->size()<<std::endl;
  }   // End detector type loop
  
  const TileHitVector * hitVec;
  if (evtStore()->retrieve(hitVec,"TileHitVec")==StatusCode::SUCCESS &&
      m_tileMgr &&
      m_tileID ){
      int hitnumber = 0;
    for(TileHitVecConstIterator i_hit=hitVec->begin() ; i_hit!=hitVec->end() ; ++i_hit){
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
}

//#######################################################################
void ISF_HitAnalysis::extrapolate(const HepMC::GenParticle* part)
{

 ptruth_eta=part->momentum().eta();
 ptruth_phi=part->momentum().phi();
 ptruth_e  =part->momentum().e();
 ptruth_pt =part->momentum().perp();
 ptruth_p  =part->momentum().rho();
 
 pdgid=part->pdg_id();
 
 // Get charge of particle
 double charge = 0;
 HepPDT::ParticleData* ap = m_particleDataTable->particle( abs( pdgid ) );
 if(ap)
 {
  charge = ap->charge();
  if(pdgid<0) charge = -charge;
 }
 
 // Setup truth to calo extrapolation info
 HepMC::GenVertex* pvtx = part->production_vertex();
 Amg::Vector3D pos(pvtx->position().x(),pvtx->position().y(),pvtx->position().z());
 Amg::Vector3D mom(part->momentum().px(),part->momentum().py(),part->momentum().pz());
 Trk::Perigee candidatePerigee(pos,mom,charge,pos);
 
 std::cout<<"Particle pdgid "<<pdgid<<" vertex x "<<pvtx->position().x()<<" y "<<pvtx->position().y()<<" z "<<pvtx->position().z()<<std::endl;
 
 
 //////////////////////////////////////
 // Start calo extrapolation
 // First: get entry point into first calo sample
 //////////////////////////////////////
 
  
 //std::cout<<"after get_calo_surface_middle. params_on_surface->eta() "<<params_on_surface->eta()<<" | eta_calo_surf "<<eta_calo_surf<<" phi_calo_surf "<<phi_calo_surf <<" d_calo_surf "<<d_calo_surf<<std::endl;
 
 std::vector<double> eta_safe;
 std::vector<double> phi_safe;
 std::vector<double> r_safe;
 std::vector<double> z_safe;
 for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample)
 {
 	eta_safe.push_back(-999.0);
 	phi_safe.push_back(-999.0);
 	r_safe.push_back(-999.0);
 	z_safe.push_back(-999.0);
 }

 const Trk::TrackParameters* params_on_surface=get_calo_surface(candidatePerigee,charge);

 
 // only continue if inside the calo
 if( fabs(eta_calo_surf)<6 )
 {
  // now try to extrpolate to all calo layers, that contain energy
  ATH_MSG_DEBUG("Calo position for particle id "<<pdgid<<", trutheta= " << ptruth_eta <<", truthphi= "<<ptruth_phi);
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample)
  {
   letaCalo[sample]=-12345;
   lphiCalo[sample]=-12345;
   lrCalo[sample]=-12345;
   lzCalo[sample]=-12345;
   layerCaloOK[sample]=0;
   //ATH_MSG_DEBUG("============= Getting Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << ptruth_eta);
   
   if(get_calo_etaphi(params_on_surface,(CaloCell_ID_FCS::CaloSample)sample,charge))
   {
    ATH_MSG_DEBUG( "Result in sample "<<sample<<": eta "<<letaCalo[sample]<<" phi "<<lphiCalo[sample]<<" r "<<lrCalo[sample]<<" z "<<lzCalo[sample]<<" (ok="<<layerCaloOK[sample]<<")");
    std::cout<<"Result in sample "<<sample<<": eta "<<letaCalo[sample]<<" phi "<<lphiCalo[sample]<<" r "<<lrCalo[sample]<<" z "<<lzCalo[sample]<<" (ok="<<layerCaloOK[sample]<<")"<<std::endl;
    eta_safe[sample]=letaCalo[sample];
    phi_safe[sample]=lphiCalo[sample];
    r_safe[sample]=lrCalo[sample];
    z_safe[sample]=lzCalo[sample];
   }
   else
    ATH_MSG_DEBUG( "Extrapolation to sample "<<sample<<" failed (ok="<<layerCaloOK[sample]<<")");
  }
  
 } //inside calo
 
 
 m_TTC_back_eta->push_back(eta_safe);
 m_TTC_back_phi->push_back(phi_safe);
 m_TTC_back_r->push_back(r_safe);
 m_TTC_back_z->push_back(z_safe);
 
 m_TTC_entrance_eta->push_back(eta_safe);
 m_TTC_entrance_phi->push_back(phi_safe);
 m_TTC_entrance_r->push_back(r_safe);
 m_TTC_entrance_z->push_back(z_safe);

}

void ISF_HitAnalysis::extrapolate_to_ID(const HepMC::GenParticle* part)
{

 ptruth_eta=part->momentum().eta();
 ptruth_phi=part->momentum().phi();
 ptruth_e  =part->momentum().e();
 ptruth_pt =part->momentum().perp();
 ptruth_p  =part->momentum().rho();
 
 pdgid=part->pdg_id();
 
 // Get charge of particle
 double charge = 0;
 HepPDT::ParticleData* ap = m_particleDataTable->particle( abs( pdgid ) );
 if(ap)
 {
  charge = ap->charge();
  if(pdgid<0) charge = -charge;
 }
 
 // Setup truth to calo extrapolation info
 HepMC::GenVertex* pvtx = part->production_vertex();
 // Trk::GlobalPosition pos(CLHEP::HepLorentzVector(pvtx->position().x(),pvtx->position().y(),pvtx->position().z(),pvtx->position().t()));
 //Trk::GlobalMomentum mom(CLHEP::HepLorentzVector(part->momentum().px(),part->momentum().py(),part->momentum().pz(),part->momentum().e()));
 //Trk::Perigee candidatePerigee(pos,mom,charge,pos);

 Amg::Vector3D pos(pvtx->position().x(),pvtx->position().y(),pvtx->position().z());
 Amg::Vector3D mom(part->momentum().px(),part->momentum().py(),part->momentum().pz());
 Trk::Perigee candidatePerigee(pos,mom,charge,pos);
 
 double eta_ID,phi_ID,r_ID,x_ID,y_ID,z_ID;
 eta_ID=phi_ID=r_ID=z_ID=x_ID=y_ID=-42.0;
 const Trk::TrackParameters* params_on_surface_ID=get_calo_surface_ID(candidatePerigee,eta_ID,phi_ID,r_ID,x_ID,y_ID,z_ID);
 
 ATH_MSG_VERBOSE("BOUNDARY ID-CALO eta "<<eta_ID<<" phi "<<phi_ID<<" r "<<r_ID<<" x "<<x_ID<<" y "<<y_ID<<" z "<<z_ID);
 if (params_on_surface_ID)
   {
     m_TTC_IDCaloBoundary_eta->push_back(eta_ID);
     m_TTC_IDCaloBoundary_phi->push_back(phi_ID);
     m_TTC_IDCaloBoundary_r->push_back(r_ID);
     m_TTC_IDCaloBoundary_z->push_back(z_ID);
   }
 else
   {
     ATH_MSG_DEBUG("Extrapolation to IDCaloBoundary failed");
     m_TTC_IDCaloBoundary_eta->push_back(-999.);
     m_TTC_IDCaloBoundary_phi->push_back(-999.);
     m_TTC_IDCaloBoundary_r->push_back(-999.);
     m_TTC_IDCaloBoundary_z->push_back(-999.);
   }
 //Angle 3D between 0,0,0 -> IP and Vertex -> IP:
 TVector3 vec(x_ID,y_ID,z_ID);
 TVector3 vecpart1(vec.X()-pvtx->position().x(),vec.Y()-pvtx->position().y(),vec.Z()-pvtx->position().z());
 ATH_MSG_VERBOSE("    3D ANGLE "<<vecpart1.Angle(vec));
 
 //Angle 2D between 0,0,0 -> IP and Vertex -> IP:
 TVector3 vecpart2(vec.X(),vec.Y(),vec.Z()-pvtx->position().z());
 ATH_MSG_VERBOSE("      ANGLE in eta direction "<<vecpart2.Angle(vec));
 
 //get the tangentvector on this interaction point:
 //GlobalMomentum* mom=params_on_surface_ID->TrackParameters::momentum().unit() ;
 //Trk::GlobalMomentum* trackmom=params_on_surface_ID->Trk::TrackParameters::momentum();
 if (params_on_surface_ID)
   {
     Amg::Vector3D trackmom=params_on_surface_ID->momentum().unit();
     //Amg::Vector3D vec_IP(x_ID,y_ID,z_ID);
     //std::cout<<"Momentum Vektor px="<<trackmom.x()<<" py "<<trackmom.y()<<" pz "<<trackmom.z()<<std::endl; //ok
     //angle between vec and trackmom:
     TVector3 Trackmom(trackmom[Amg::x],trackmom[Amg::y],trackmom[Amg::z]);
     double angle3D=Trackmom.Angle(vec); //isn't this the same as TVector3 vec?
     //std::cout<<"    3D Angle between vec and trackmom: "<<angle3d<<std::endl;
     TVector3 trackmom_z(x_ID,y_ID,Trackmom.z());
     double angleEta=trackmom_z.Angle(vec);
     //std::cout<<"    Angle in eta direction between vec and trackmom: "<<angleEta<<std::endl;
     
     m_TTC_Angle3D->push_back(angle3D);
     m_TTC_AngleEta->push_back(angleEta);
   }
 else
   {
     m_TTC_Angle3D->push_back(-999.);
     m_TTC_AngleEta->push_back(-999.);
   }
} //extrapolate_to_ID


bool ISF_HitAnalysis::get_calo_etaphi(const Trk::TrackParameters* params_on_surface,CaloCell_ID_FCS::CaloSample sample, double charge)
{
 double distsamp =deta(sample,eta_calo_surf);
 //        double rzmiddle =m_calodepth->radius((CaloCell_ID::CaloSample)sample,ptruth_eta,ptruth_phi);
 double rzmiddle =rzmid(sample,eta_calo_surf);
 ATH_MSG_DEBUG("in : dist="<<distsamp<<" r/z(middle)="<<rzmiddle<<" ===== charge "<<charge);
 
 double offset = 0.;
 
 Amg::Vector3D pt_calo_ctb;
 Amg::Vector3D pt_calo_local;
 
 const Trk::TrackParameters* result=TrackSeenByCalo(m_calosurf, params_on_surface, sample, offset, &pt_calo_ctb, &pt_calo_local);
 
 if(result)
 {
  delete result;
  result=0;
  double etaCalo = pt_calo_local.eta();
  double phiCalo = pt_calo_local.phi();
  double rCalo = pt_calo_local.mag();
  double zCalo = pt_calo_local.z();
  letaCalo[sample]=etaCalo;
  lphiCalo[sample]=phiCalo;
  lrCalo[sample]=rCalo;
  lzCalo[sample]=zCalo;
  //std::cout<<"---------------> r "<<lrCalo[sample]<<" z "<<lzCalo[sample]<<std::endl;
  if(fabs(etaCalo)<900)
  {
   layerCaloOK[sample]=true;
   //double testrzmiddle=m_calodepth->radius((CaloCell_ID::CaloSample)sample,etaCalo,phiCalo);
   rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
   distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
  }
  else
  {
   letaCalo[sample]=eta_calo_surf;
   lphiCalo[sample]=phi_calo_surf;
  }
  
 } //if results
 else
 {
  letaCalo[sample]=eta_calo_surf;
  lphiCalo[sample]=phi_calo_surf;
 }
 
 if(isCaloBarrel((CaloCell_ID_FCS::CaloSample)sample))
  rzmiddle*=cosh(letaCalo[sample]);
 else
  rzmiddle= fabs(rzmiddle/tanh(letaCalo[sample]));
 
 //std::cout<<"-------------> rzmiddle "<<rzmiddle<<std::endl;
 
 dCalo[sample]=rzmiddle;
 distetaCaloBorder[sample]=distsamp;

 if(msgLvl(MSG::DEBUG))
 {
  msg(MSG::DEBUG)<<"  Final par TTC sample "<<(int)sample;
  if(layerCaloOK[sample])
   msg()<<" (good)";
  else
   msg()<<" (bad)";
  msg()<<" eta=" << letaCalo[sample] << "   phi=" << lphiCalo[sample] <<" dCalo="<<dCalo[sample]<< endreq;
 }

 if(result)
 {
  delete result;
  result=0;
 }
	
 return layerCaloOK[sample];
 
}


double ISF_HitAnalysis::rzmid(CaloCell_ID_FCS::CaloSample sample,double eta) const
{
 int side=0;
 if(eta>0) side=1;
	
 if(isCaloBarrel(sample)) return rmid_map[side][sample].find_closest(eta)->second;
 else                     return zmid_map[side][sample].find_closest(eta)->second;
}

double ISF_HitAnalysis::deta(CaloCell_ID_FCS::CaloSample sample,double eta) const
{
  int side=0;
  if(eta>0) side=1;

  double mineta=min_eta_sample[side][sample];
  double maxeta=max_eta_sample[side][sample];

  if(eta<mineta)
  {
    return fabs(eta-mineta);
  }
  else if(eta>maxeta)
  {
   return fabs(eta-maxeta);
	}
	else
	{
   double d1=fabs(eta-mineta);
   double d2=fabs(eta-maxeta);
   if(d1<d2) return -d1;
   else return -d2;
  }
}


void ISF_HitAnalysis::minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta) const 
{
  int side=0;
  if(eta>0) side=1;
  
  mineta=min_eta_sample[side][sample];
  maxeta=max_eta_sample[side][sample];
}

double ISF_HitAnalysis::rmid(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return rmid_map[side][sample].find_closest(eta)->second;
}

double ISF_HitAnalysis::zmid(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return zmid_map[side][sample].find_closest(eta)->second;
}


double ISF_HitAnalysis::rent(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return rent_map[side][sample].find_closest(eta)->second;
}

double ISF_HitAnalysis::zent(CaloCell_ID_FCS::CaloSample sample,double eta) const 
{
  int side=0;
  if(eta>0) side=1;

  return zent_map[side][sample].find_closest(eta)->second;
}

double ISF_HitAnalysis::rzent(CaloCell_ID_FCS::CaloSample sample,double eta) const
{
  int side=0;
  if(eta>0) side=1;

  if(isCaloBarrel(sample)) return rent_map[side][sample].find_closest(eta)->second;
   else                    return zent_map[side][sample].find_closest(eta)->second;
}

const Trk::TrackParameters* ISF_HitAnalysis::TrackSeenByCalo(ICaloSurfaceBuilder* m_calosurf,const Trk::TrackParameters* parm, const CaloCell_ID_FCS::CaloSample sample,const double offset, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local)
{


 // Take eta as best guess of track direction and create surface :
 double trketa = parm->eta();
 const Trk::TrackParameters* param=0;
 
 Trk::Surface* surf = m_calosurf->CreateUserSurface ((CaloCell_ID::CaloSample)sample,offset,trketa);
 if (!surf) return 0;

 
  if (!parm)
 {
  ATH_MSG_WARNING("extrapolate TrkParam called with null Trk::TrackParameters*");
  return 0;
 }

 ATH_MSG_VERBOSE("Trying to propagate TrackParameters to Surface ... " << (*surf));
 param = m_extrapolator->extrapolate( *parm, *surf, Trk::alongMomentum, true, Trk::nonInteracting);
 if (param)
 {
  ATH_MSG_VERBOSE("Propagation successful ");
  //std::cout<<"sample "<<sample<<" extrapolated params eta "<<param->eta()<<std::endl;
 }
  
 else
 {
  ATH_MSG_VERBOSE("Propagation failed, try custom surface ");
  Trk::Surface* newsurf=0;
  if(isCaloBarrel(sample))
  {
   double r;
   if(m_calosurf==m_calosurf_entrance) r=rent(sample,trketa);
   else r=rmid(sample,trketa);
   Amg::Transform3D *t = new Amg::Transform3D(Amg::Transform3D::Identity()); t->setIdentity();
   newsurf=new Trk::CylinderSurface( t , r , 40000*CLHEP::mm );
  }
  else
  {
   double z;
   if(m_calosurf==m_calosurf_entrance) z=zent(sample,trketa);
   else z=zmid(sample,trketa);
   newsurf=new Trk::DiscSurface( new Amg::Transform3D(Amg::Vector3D(0.,0.,z)), 20*CLHEP::mm , 20000*CLHEP::mm );
  }
  
	if(newsurf)
	{
   delete surf;
   surf=newsurf;
   ATH_MSG_VERBOSE("Trying to propagate TrackParameters to Surface ... " << (*surf));
   param = m_extrapolator->extrapolate( *parm, *surf, Trk::alongMomentum, false, Trk::nonInteracting);
   if (param)
   {
    ATH_MSG_VERBOSE("2nd Propagation successful ");
    //std::cout<<"sample "<<sample<<" newsurf, extrapolated params eta "<<param->eta()<<std::endl;
   }
   else
   {
    ATH_MSG_VERBOSE("2nd Propagation failed");
    //std::cout<<"sample "<<sample<<" no newsurf. propagation failed :("<<std::endl;
   } 
  }
 }
 //here it writes the stuff to pt_local I guess
 CaloLocalPoint (param,pt_ctb, pt_local); 
 delete surf;

  
 return param;
 
} 


const Trk::TrackParameters* ISF_HitAnalysis::TrackSeenByCalo_ID(const Trk::TrackParameters* parm, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local)
{
 
 const Trk::TrackParameters* param=0;
 
 Trk::Surface* surf=0;
 	
 //std::cout<<"m_CaloBoundaryR "<<m_CaloBoundaryR<<" m_CaloBoundaryZ "<<m_CaloBoundaryZ<<std::endl;
 Amg::Transform3D *t = new Amg::Transform3D(Amg::Transform3D::Identity());
 t->setIdentity(); 
 Trk::Surface* newsurf=new Trk::CylinderSurface(t , m_CaloBoundaryR*CLHEP::mm , m_CaloBoundaryZ*CLHEP::mm );
 
 if(newsurf)
 {
  surf=newsurf;
  ATH_MSG_VERBOSE("Trying to propagate TrackParameters to Surface ... " << (*surf));
  param = m_extrapolator->extrapolate( *parm, *surf, Trk::alongMomentum, false, Trk::nonInteracting);
  if (param)
  {
   ATH_MSG_VERBOSE("2nd Propagation successful ");
  }
  else
  {
   ATH_MSG_VERBOSE("2nd Propagation failed");
  } 
 }
 
 //here it writes the stuff to pt_local I guess
 CaloLocalPoint (param,pt_ctb, pt_local); 
 delete surf;
 
 return param;
 
} 

void ISF_HitAnalysis::CaloLocalPoint (const Trk::TrackParameters* parm,Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local)
{
  if (!parm)
  {
   //std::cout<<"!parm"<<std::endl;
   return;
  }
  if (!pt_local)
  {
   //std::cout<<"!pt_local"<<std::endl;
   return;
  }
  
  *pt_ctb = parm->position();

  ATH_MSG_VERBOSE( "Impact point in ctb coord : x,y,z= " 
      << pt_ctb->x() << " " 
      << pt_ctb->y() << " " << pt_ctb->z() << " R=" 
      << std::sqrt( pt_ctb->x()*pt_ctb->x() + pt_ctb->y()*pt_ctb->y() 
                    + pt_ctb->z()*pt_ctb->z())
      << " eta=" << pt_ctb->eta() << " phi=" << pt_ctb->phi() );

  m_calo_tb_coord->ctb_to_local(*pt_ctb, *pt_local);
    
  ATH_MSG_VERBOSE( "Impact point in local coord : x,y,z= " 
      << pt_local->x() << " " 
      << pt_local->y() << " " << pt_local->z() << " R=" 
      << std::sqrt( pt_local->x()*pt_local->x() 
                    + pt_local->y()*pt_local->y() 
                    + pt_local->z()*pt_local->z())
      << " eta=" << pt_local->eta() << " phi=" << pt_local->phi() );
}


const Trk::TrackParameters* ISF_HitAnalysis::get_calo_surface(const Trk::Perigee& candidatePerigee,const double charge)
{
		
  const Trk::TrackParameters* params_on_surface=0;
  
  sample_calo_surf=CaloCell_ID_FCS::MaxSample;
  
  eta_calo_surf=-999;
  phi_calo_surf=-999;
  d_calo_surf=0;
  double min_calo_surf_dist=1000;
  const int n_surfacelist=5;
  CaloCell_ID_FCS::CaloSample surfacelist[n_surfacelist]={CaloCell_ID_FCS::PreSamplerB,
	                                                          CaloCell_ID_FCS::PreSamplerE,
	                                                          CaloCell_ID_FCS::EME1,
	                                                          CaloCell_ID_FCS::EME2,
	                                                          CaloCell_ID_FCS::FCAL0
	                                                         };
  m_n_surfacelist=n_surfacelist;
  for(int i=0;i<n_surfacelist;++i) m_surfacelist[i]=surfacelist[i];
  
  
  for(int i=0;i<m_n_surfacelist;++i)
  {
  
    CaloCell_ID_FCS::CaloSample sample=m_surfacelist[i];
    double offset = 0.;
    
    Amg::Vector3D pt_calo_ctb;
    Amg::Vector3D pt_calo_local;

    //std::cout<<"in get_calo_surface_middle for loop (i="<<i<<"). right before TrackSeenByCalo"<<std::endl;

    const Trk::TrackParameters* result=TrackSeenByCalo(m_calosurf, &candidatePerigee, (CaloCell_ID_FCS::CaloSample)sample, offset, &pt_calo_ctb, &pt_calo_local);
    double etaCalo = pt_calo_local.eta();
    
    //std::cout<<"in get_calo_surface_middle after TrackSeenByCalo. "<<std::endl;
    //if(result) std::cout<<"get_calo_surface_middle result for sample "<<sample<<" track->eta "<<result->eta()<<" etaCalo "<<etaCalo<<std::endl;
        
    if (result && fabs(etaCalo)<900)
    {
      double phiCalo = pt_calo_local.phi();
      double distsamp =deta(sample,etaCalo);
      
      msg(MSG::VERBOSE)<<" phi="<<phiCalo<<" dist="<<distsamp;
      if(distsamp<min_calo_surf_dist)
      {
        if(params_on_surface) delete params_on_surface;
        params_on_surface=result;
        eta_calo_surf=etaCalo;
        phi_calo_surf=phiCalo;
        min_calo_surf_dist=distsamp;
        sample_calo_surf=sample;
        d_calo_surf=rzent(sample,etaCalo);
//          d_calo_surf=m_calodepthEntrance->radius(sample,etaCalo,phiCalo);

        msg(MSG::VERBOSE)<<" r/z="<<d_calo_surf;
//          log<<" _r="<<rent(sample,etaCalo);
//          log<<" _z="<<zent(sample,etaCalo);

        if(isCaloBarrel(sample)) d_calo_surf*=cosh(etaCalo);
         else                    d_calo_surf= fabs(d_calo_surf/tanh(etaCalo));
        
        msg(MSG::VERBOSE)<<" d="<<d_calo_surf;

        if(distsamp<0)
        {
          msg(MSG::VERBOSE)<<endreq;
          break;
        }  
      }
    }
    
    if(result!=params_on_surface && result) delete result;
    msg(MSG::VERBOSE)<<endreq;
   
  }
  
  if(sample_calo_surf==CaloCell_ID_FCS::MaxSample)
  {
    double maxradius=0;
    if(charge!=0)
    {                  
      const double const_c=299792458.0;   // Speed of light in m/s
      const double const_bField=2;
      const double const_mm=0.001;        // Unitconversion m -> CLHEP::mm
      const double const_MeV=1000000.0;   // Unitconversion CLHEP::MeV -> CLHEP::eV, pt is assumed in MeV/c, r is given in mm
      double radius = ptruth_pt * const_MeV / ( const_c * const_bField * std::abs(charge) ) / const_mm; 
      maxradius=2*radius; //max detector radius is 2*helix radius
    } else {
      const double const_z_endcap=4700.0; // distance of endcap in mm
      double tanheta=tanh(ptruth_eta);
      maxradius=const_z_endcap*sqrt(1.0/tanheta/tanheta - 1.0);
    }
    if(maxradius>100 && ptruth_pt>400 && fabs(ptruth_eta)<4.5)
    {
     msg(MSG::WARNING);
    }
    else
    {
     msg(MSG::VERBOSE);
    }  
    msg()<<"entrance to calo surface not found... " 
         <<" initial par id="<<pdgid<<" e="<<ptruth_e
         <<" pt="<<ptruth_pt
         <<" eta="<<ptruth_eta<<" phi="<<ptruth_phi<<" charge="<<charge
         <<" vx="<<candidatePerigee.position().x()<<" vy="<<candidatePerigee.position().y()<<" vz="<<candidatePerigee.position().z()
         <<" : maxradius="<<maxradius<<" at FCAL/of helix. Skipping particle..."
         <<endreq;
    return 0;   
  }
  else
  {
   ATH_MSG_VERBOSE("middle of calo surface : sample="<<sample_calo_surf<<" eta="<<eta_calo_surf<<" phi="<<phi_calo_surf<<" deta="<<min_calo_surf_dist<<" dsurf="<<d_calo_surf);
  }
  
  
  return params_on_surface;
}


const Trk::TrackParameters* ISF_HitAnalysis::get_calo_surface_ID(const Trk::Perigee& candidatePerigee,
	                                                               double& eta_ID, double& phi_ID, double& r_ID, double& x_ID, double& y_ID, double& z_ID)
{
	
  //const Trk::TrackParameters* params_on_surface=0;
  
  //HepGeom::Point3D<double> pt_calo_ctb;
  //HepGeom::Point3D<double> pt_calo_local;
  Amg::Vector3D pt_calo_ctb;
  Amg::Vector3D pt_calo_local;
  
  const Trk::TrackParameters* params_on_surface=TrackSeenByCalo_ID(&candidatePerigee, &pt_calo_ctb, &pt_calo_local);
  

  TVector3 vec(pt_calo_local[Amg::x], pt_calo_local[Amg::y], pt_calo_local[Amg::z]);
  /*
  eta_ID = pt_calo_local.eta();
  phi_ID = pt_calo_local.phi();
  r_ID = pt_calo_local.r();
  x_ID = pt_calo_local.x();
  y_ID = pt_calo_local.y();
  z_ID = pt_calo_local.z();
  */
  eta_ID = vec.Eta();
  phi_ID = vec.Phi();
  r_ID = vec.Mag();
  x_ID = vec.X();
  y_ID = vec.Y();
  z_ID = vec.Z();
  //std::cout<<"in get_calo_surface_ID, eta_ID = "<<eta_ID<<std::endl;
  
  return params_on_surface;

} //get_calo_surface_ID

