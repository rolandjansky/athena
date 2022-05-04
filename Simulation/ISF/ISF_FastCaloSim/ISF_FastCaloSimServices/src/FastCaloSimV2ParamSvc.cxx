/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "FastCaloSimV2ParamSvc.h"


// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
//for FastCaloSim-GPU
#ifdef USE_GPU
#include "ISF_FastCaloGpu/GeoLoadGpu.h"
#include "ISF_FastCaloGpu/CaloGpuGeneral.h"
#include "AthCUDACore/Info.h"
#endif

//!
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/TileID.h"
//!

// StoreGate
#include "StoreGate/StoreGateSvc.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include <fstream>

/** Constructor **/
ISF::FastCaloSimV2ParamSvc::FastCaloSimV2ParamSvc(const std::string& name, ISvcLocator* svc)
  : base_class(name, svc)
  , m_detStore( "StoreGateSvc/DetectorStore", name )

{
  declareProperty( "DetStore",
                   m_detStore,
                   "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
                   "retrieve data during the course of the job" );
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename);
  declareProperty("ParamsInputObject"              ,       m_paramsObject);
  declareProperty("PrintParametrization"           ,       m_printParametrization);
  declareProperty("CompressMemory"                 ,       m_CompressMemory);
  declareProperty("RunOnGPU"                       ,       m_runOnGPU);
}

/** framework methods */
StatusCode ISF::FastCaloSimV2ParamSvc::initialize()
{
  const CaloDetDescrManager *calo_dd_man = CaloDetDescrManager::instance();
  const FCALDetectorManager *fcalManager{};
  ATH_CHECK(detStore()->retrieve(fcalManager));

  m_caloGeo = std::make_unique<CaloGeometryFromCaloDDM>();
  m_caloGeo->LoadGeometryFromCaloDDM(calo_dd_man);
  if (!m_caloGeo->LoadFCalChannelMapFromFCalDDM(fcalManager)) {
    ATH_MSG_FATAL("Found inconsistency between FCal_Channel map and GEO file. Please, check if they are configured properly.");
    return StatusCode::FAILURE;
  }

  const std::string fileName = m_paramsFilename; //?
  std::string inputFile=PathResolverFindCalibFile(fileName);
  std::unique_ptr<TFile> paramsFile(TFile::Open( inputFile.c_str(), "READ" ));
  if (paramsFile == nullptr) {
    ATH_MSG_ERROR("file = "<<m_paramsFilename<< " not found");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Opened parametrization file = "<<m_paramsFilename);
  paramsFile->ls();
  m_param=static_cast<TFCSParametrizationBase*>(paramsFile->Get(m_paramsObject.c_str()));
  if (!m_param) {
    ATH_MSG_WARNING("file = "<<m_paramsFilename<< ", object "<< m_paramsObject<<" not found");
    return StatusCode::FAILURE;
  }

  paramsFile->Close();

  if(m_CompressMemory) m_param->RemoveDuplicates();
  m_param->set_geometry(m_caloGeo.get()); /// does not take ownership
  m_param->setLevel(msg().level());
  if (m_printParametrization) {
    m_param->Print("short");
  }
  if(m_CompressMemory) m_param->RemoveNameTitle();

#ifdef USE_GPU

  // if no GPU is found then run on CPU
  if( !m_runOnGPU )
    return StatusCode::SUCCESS;
  if( AthCUDA::Info::instance().nDevices() == 0 ) {
    ATH_MSG_WARNING( "no GPU device is found, will run everything on the CPU." );
    m_runOnGPU=false;
    return StatusCode::SUCCESS;
  }

  // FCS-GPU
  // copy the geometry and parameterization files to GPU
  ATH_MSG_INFO("start copying all parametrization files to GPU");
  m_param->Copy2GPU();
  //random seed for curand in GPU
  unsigned long long seed=23424352;
  m_rd4h = CaloGpuGeneral::Rand4Hits_init( MAXHITS, MAXBINS, seed, true );
  //construct and load Geometry in GPU
  ATH_MSG_INFO("start loading Geometry in GPU");
  m_gl = new GeoLoadGpu();
  CaloGeometry* calogeo=m_caloGeo.get();
  m_gl->set_ncells( calogeo->get_cells()->size() );

  if(calogeo->get_cells()->size() ==0 ) return StatusCode::SUCCESS;

  m_gl->set_max_sample( CaloGeometry::MAX_SAMPLING);
  int nrgns= calogeo->get_tot_regions();
  m_gl->set_nregions( nrgns);


  if(!convert_cellmap(calogeo->get_cells(),&m_cellmap_gpu)){
    ATH_MSG_ERROR("can not convert the CaloDetDescrElement to CaloDetDescrElement_GPU ");
  }
  m_gl->set_cellmap( &m_cellmap_gpu);
  GeoRegion* GR_ptr = (GeoRegion*)malloc( nrgns *sizeof( GeoRegion ));
  m_gl->set_regions( GR_ptr );

  Rg_Sample_Index* si = (Rg_Sample_Index*)malloc(CaloGeometry::MAX_SAMPLING * sizeof( Rg_Sample_Index ) )    ;

  m_gl->set_sample_index_h( si );
  //set regions
  int i = 0;
  for ( int is = 0; is < CaloGeometry::MAX_SAMPLING; ++is ) {
    si[is].index = i;
    int nr       = calogeo->get_n_regions( is );
    si[is].size  = nr;
    for ( int ir = 0; ir < nr; ++ir ) region_data_cpy( calogeo->get_region( is, ir), &GR_ptr[i++] );
  }

  if(m_gl->LoadGpu())
    ATH_MSG_INFO("calo geometry loaded successfully in GPU");
  else
    ATH_MSG_ERROR("can not load calo geoemtry in GPU");
#endif

  return StatusCode::SUCCESS;
}

/** Simulation Call */
StatusCode ISF::FastCaloSimV2ParamSvc::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
#ifdef USE_GPU
  if( m_runOnGPU ) {
    simulstate.set_gpu_rand (m_rd4h );
    simulstate.set_geold( m_gl );
  }
  else
    simulstate.set_geold( nullptr );
#endif

  FCSReturnCode status = m_param->simulate(simulstate, truth, extrapol);
  if (status != FCSSuccess) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

#ifdef USE_GPU
//convert the cellmap to GPU EDM
bool ISF::FastCaloSimV2ParamSvc::convert_cellmap(t_cellmap* cellmap, t_cellmap_Gpu* newcellmap){    
  for(t_cellmap::iterator ic=cellmap->begin();ic!=cellmap->end();++ic) {
    const CaloDetDescrElement* cell=(*ic).second;
    const CaloDetDescrElement_Gpu* newcell=new CaloDetDescrElement_Gpu(cell->identify().get_compact(),
	cell->calo_hash(),
	cell->getSampling(),
	cell->eta(),
	cell->phi(),
	cell->deta(),
	cell->dphi(),
	cell->r(),
	cell->eta_raw(),
	cell->phi_raw(),
	cell->r_raw(),
	cell->dr(),
	cell->x(),
	cell->y(),
	cell->z(),
	cell->x_raw(),
	cell->y_raw(),
	cell->z_raw(),
	cell->dx(),
	cell->dy(),
	cell->dz());
    newcellmap->insert(std::make_pair(Identifier_Gpu(cell->identify().get_compact()),newcell));
  }
  if(cellmap->size()!=newcellmap->size())return false;
  return true;
}

void ISF::FastCaloSimV2ParamSvc::region_data_cpy( CaloGeometryLookup* glkup, GeoRegion* gr ) {

  // Copy all parameters
  gr->set_xy_grid_adjustment_factor( glkup->xy_grid_adjustment_factor() );
  gr->set_index( glkup->index() );

  int neta = glkup->cell_grid_eta();
  int nphi = glkup->cell_grid_phi();
  ATH_MSG_DEBUG(" copy region " << glkup->index() << "neta= " << neta<< ", nphi=     "<<nphi) ;

  gr->set_cell_grid_eta( neta );
  gr->set_cell_grid_phi( nphi );

  gr->set_mineta( glkup->mineta() );
  gr->set_minphi( glkup->minphi() );
  gr->set_maxeta( glkup->maxeta() );
  gr->set_maxphi( glkup->maxphi() );

  gr->set_mineta_raw( glkup->mineta_raw() );
  gr->set_minphi_raw( glkup->minphi_raw() );
  gr->set_maxeta_raw( glkup->maxeta_raw() );
  gr->set_maxphi_raw( glkup->maxphi_raw() );

  gr->set_mineta_correction( glkup->mineta_correction() );
  gr->set_minphi_correction( glkup->minphi_correction() );
  gr->set_maxeta_correction( glkup->maxeta_correction() );
  gr->set_maxphi_correction( glkup->maxphi_correction() );

  gr->set_eta_correction( glkup->eta_correction() );
  gr->set_phi_correction( glkup->phi_correction() );
  gr->set_deta( glkup->deta() );
  gr->set_dphi( glkup->dphi() );
  gr->set_deta_double( glkup->deta_double() );
  gr->set_dphi_double( glkup->dphi_double() );

  // now cell array copy from GeoLookup Object
  // new cell_grid is a unsigned long array
  long long* cells = (long long*)malloc( sizeof( long long ) * neta* nphi );
  gr->set_cell_grid( cells );
  // new cell_grid is a unsigned long array
  if ( neta != (int)( *( glkup->cell_grid() ) ).size() )
    ATH_MSG_INFO("neta " << neta << ", vector eta size " << ( *( glkup->cell_grid() ) ).size() );

  for ( int ie = 0; ie < neta; ++ie ) {

    for ( int ip = 0; ip < nphi; ++ip ) {

      auto c = ( *( glkup->cell_grid() ) )[ie][ip];
      if ( c ) {
	cells[ie * nphi + ip] = c->calo_hash( );

      } else {
	cells[ie * nphi + ip] = -1;
      }
    }
  }
}
#endif

