/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "HepMC/GenVertex.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetTrackClusterAssValidation/TrackClusterAssValidation.h"
#include "StoreGate/ReadHandle.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TrackClusterAssValidation::TrackClusterAssValidation
(const std::string& name,ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name,pSvcLocator),
    m_truth_locationPixel( "PRD_MultiTruthPixel" ),
    m_truth_locationSCT(   "PRD_MultiTruthSCT" ),
    m_truth_locationTRT(   "PRD_MultiTruthTRT" )
{

  // TrackClusterAssValidation steering parameters
  //
  m_spacepointsSCTname     = std::string("SCT_SpacePoints")   ;
  m_spacepointsPixelname   = "PixelSpacePoints"               ;
  m_spacepointsOverlapname = "OverlapSpacePoints"             ;
  m_clustersPixelname      = "PixelClusters"                  ;
  m_clustersSCTname        = std::string("SCT_Clusters")      ;
  m_clustersTRTname        = "TRT_DriftCircles"               ;
  m_ptcut                  = 1000.                            ;
  m_ptcutmax               = 1.e20                            ;
  m_rapcut                 = 2.6                              ;
  m_rmin                   = 0.                               ;
  m_rmax                   = 30.                              ;
  m_clcut                  = 6                                ;
  m_clcutTRT               = 0                                ;
  m_spcut                  = 3                                ;
  m_usePIX                 = true                             ;
  m_useSCT                 = true                             ;
  m_useTRT                 = true                             ;
  m_useOutliers            = false                            ;
  m_pdg                    = 0                                ;
  m_tcut                   = 0.                               ;
  m_particleDataTable      = 0                                ;

  declareProperty("TracksLocation"        ,m_tracklocation         );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCTname    );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixelname  );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlapname);
  declareProperty("PixelClustesContainer" ,m_clustersPixelname     );
  declareProperty("SCT_ClustesContainer"  ,m_clustersSCTname       );
  declareProperty("TRT_ClustesContainer"  ,m_clustersTRTname       );
  declareProperty("TruthLocationSCT"      ,m_truth_locationSCT     );
  declareProperty("TruthLocationPixel"    ,m_truth_locationPixel   );
  declareProperty("TruthLocationTRT"      ,m_truth_locationTRT     );
  declareProperty("MomentumCut"           ,m_ptcut                 );
  declareProperty("MomentumMaxCut"        ,m_ptcutmax              );
  declareProperty("RapidityCut"           ,m_rapcut                );
  declareProperty("RadiusMin"             ,m_rmin                  );
  declareProperty("RadiusMax"             ,m_rmax                  );
  declareProperty("MinNumberClusters"     ,m_clcut                 );
  declareProperty("MinNumberClustersTRT"  ,m_clcutTRT              );
  declareProperty("MinNumberSpacePoints"  ,m_spcut                 );
  declareProperty("usePixel"              ,m_usePIX                );
  declareProperty("useSCT"                ,m_useSCT                );
  declareProperty("useTRT"                ,m_useTRT                );
  declareProperty("useOutliers"           ,m_useOutliers           );
  declareProperty("pdgParticle"           ,m_pdg                   );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TrackClusterAssValidation::initialize()
{

  StatusCode sc;

  m_rapcut ? m_tcut = 1./tan(2.*atan(exp(-m_rapcut))) : m_tcut = 0.;

  // get the Particle Properties Service
  //
  IPartPropSvc* partPropSvc = 0;
  sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << " Could not initialize Particle Properties Service" << endmsg;
    return StatusCode::FAILURE;
  }

  // Particle Data Table
  //
  m_particleDataTable = partPropSvc->PDT();
  if(!m_particleDataTable) {
    msg(MSG::FATAL) << " Could not initialize Particle Properties Service" << endmsg;
    return StatusCode::FAILURE;
  }

  // Erase statistic information
  //
  m_pdg          = std::abs(m_pdg)         ;

  m_trackCollectionStat.resize(m_tracklocation.size());
  m_eventStat = EventStat_t();

  if(!m_useTRT) m_clcutTRT = 0;
  if(!m_clcutTRT) m_useTRT = false;

  // Read Handle Key
  ATH_CHECK(m_truth_locationSCT.initialize(m_useSCT));
  ATH_CHECK(m_clustersSCTname.initialize(m_useSCT));
  ATH_CHECK(m_spacepointsSCTname.initialize(m_useSCT));

  ATH_CHECK( m_clustersPixelname.initialize(m_usePIX));
  ATH_CHECK( m_spacepointsPixelname.initialize(m_usePIX));
  ATH_CHECK( m_truth_locationPixel.initialize(m_usePIX));

  ATH_CHECK( m_spacepointsOverlapname.initialize(m_useSCT));

  ATH_CHECK( m_clustersTRTname.initialize(m_useTRT));
  ATH_CHECK( m_truth_locationTRT.initialize(m_useTRT));

  ATH_CHECK( m_tracklocation.initialize());

  if (msgLvl(MSG::DEBUG)) {
    dumptools(msg(),MSG::DEBUG);
    msg() << endmsg;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::TrackClusterAssValidation::execute_r(const EventContext& ctx) const
{

  if(!m_usePIX && !m_useSCT) return StatusCode::SUCCESS;;
  EventData_t event_data(m_tracklocation.size() );

  std::vector<SG::ReadHandle<PRD_MultiTruthCollection> > read_handle;
  read_handle.reserve(3);
  if(m_usePIX) {
    read_handle.push_back(SG::ReadHandle<PRD_MultiTruthCollection>(m_truth_locationPixel,ctx));
    if (not read_handle.back().isValid()) {
      ATH_MSG_FATAL( "Could not find TruthPIX" );
      return StatusCode::FAILURE;
    }
    event_data.m_truthPIX = &(*read_handle.back());
  }

  if(m_useSCT) {
    read_handle.push_back(SG::ReadHandle<PRD_MultiTruthCollection>(m_truth_locationSCT,ctx));
    if (not read_handle.back().isValid()) {
      ATH_MSG_FATAL( "Could not find TruthSCT" );
      return StatusCode::FAILURE;
    }
    event_data.m_truthSCT = &(*read_handle.back());
  }

  if(m_clcutTRT > 0) {
    read_handle.push_back(SG::ReadHandle<PRD_MultiTruthCollection>(m_truth_locationTRT,ctx));
    if (not read_handle.back().isValid()) {
      ATH_MSG_FATAL( "Could not find TruthTRT" );
      return StatusCode::FAILURE;
    }
    event_data.m_truthTRT = &(*read_handle.back());
  }

  newClustersEvent      (ctx,event_data);
  newSpacePointsEvent   (ctx,event_data);
  event_data.m_nqtracks = QualityTracksSelection(event_data);
  tracksComparison      (ctx,event_data);
  if(event_data.m_particles[0].size() > 0) {

    efficiencyReconstruction(event_data);
    if(msgLvl(MSG::DEBUG)) noReconstructedParticles(event_data);

  }

  {
    std::lock_guard<std::mutex> lock(m_statMutex);
    assert( event_data.m_trackCollectionStat.size() == m_trackCollectionStat.size());
    for (unsigned int i=0; i< m_trackCollectionStat.size(); ++i ) {
      m_trackCollectionStat[i] += event_data.m_trackCollectionStat[i];
    }
    m_eventStat += event_data.m_eventStat;
  }

  if (msgLvl(MSG::DEBUG)) {
    dumpevent(msg(),event_data);
    msg() << endmsg;
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TrackClusterAssValidation::finalize() {

  if(m_eventStat.m_events<=0) return StatusCode::SUCCESS;

  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl;
  std::cout<<"|        TrackClusterAssValidation statistic for charge truth particles with        |"
	   <<std::endl;
  std::cout<<"|                                                                                   |"
	   <<std::endl;
  std::cout<<"|                    pT                      >="
	   <<std::setw(13)<<std::setprecision(5)<<m_ptcut<<" MeV"
	   <<"                    |"<<std::endl;
  if(m_ptcutmax < 1000000.) {
    std::cout<<"|                    pT                      <="
	     <<std::setw(13)<<std::setprecision(5)<<m_ptcutmax<<" MeV"
	     <<"                    |"<<std::endl;
  }
  std::cout<<"|                    |rapidity|              <="
	   <<std::setw(13)<<std::setprecision(5)<<m_rapcut
	   <<"                        |"<<std::endl;
  std::cout<<"|                    max vertex radius       <="
	   <<std::setw(13)<<std::setprecision(5)<<m_rmax<<" mm"
	   <<"                     |"<<std::endl;
  std::cout<<"|                    min vertex radius       >="
	   <<std::setw(13)<<std::setprecision(5)<<m_rmin<<" mm"
	   <<"                     |"<<std::endl;
  std::cout<<"|                    particles pdg            ="
	   <<std::setw(8)<<m_pdg
	   <<"                             |"<<std::endl;

  std::string YN = "yes"; if(!m_usePIX) YN = "no ";
  std::cout<<"|                    use Pixels information   "
	   <<"       "<<YN
	   <<"                            |"<<std::endl;
  YN             = "yes"; if(!m_useSCT) YN = "no ";
  std::cout<<"|                    use SCT    information   "
	   <<"       "<<YN
	   <<"                            |"<<std::endl;
  YN             = "yes"; if(!m_useTRT) YN = "no ";
  std::cout<<"|                    use TRT    information   "
	   <<"       "<<YN
	   <<"                            |"<<std::endl;
  YN             = "yes"; if(!m_useOutliers) YN = "no ";
  std::cout<<"|                    take into account outliers"
	   <<"      "<<YN
	   <<"                            |"<<std::endl;

  std::cout<<"|                                                                                   |"
	   <<std::endl;
  if(!m_usePIX && !m_useSCT) return StatusCode::SUCCESS;


  for(int i=48; i>=0; --i) {
    m_eventStat.m_particleClusters      [i]   +=m_eventStat.m_particleClusters      [i+1];
    m_eventStat.m_particleClustersBTE   [i][0]+=m_eventStat.m_particleClustersBTE   [i+1][0];
    m_eventStat.m_particleClustersBTE   [i][1]+=m_eventStat.m_particleClustersBTE   [i+1][1];
    m_eventStat.m_particleClustersBTE   [i][2]+=m_eventStat.m_particleClustersBTE   [i+1][2];
    m_eventStat.m_particleClustersBTE   [i][3]+=m_eventStat.m_particleClustersBTE   [i+1][3];
    m_eventStat.m_particleSpacePoints   [i]   +=m_eventStat.m_particleSpacePoints   [i+1];
    m_eventStat.m_particleSpacePointsBTE[i][0]+=m_eventStat.m_particleSpacePointsBTE[i+1][0];
    m_eventStat.m_particleSpacePointsBTE[i][1]+=m_eventStat.m_particleSpacePointsBTE[i+1][1];
    m_eventStat.m_particleSpacePointsBTE[i][2]+=m_eventStat.m_particleSpacePointsBTE[i+1][2];
    m_eventStat.m_particleSpacePointsBTE[i][3]+=m_eventStat.m_particleSpacePointsBTE[i+1][3];
  }

  double pa   =  double(m_eventStat.m_particleClusters[0]); if(pa < 1.) pa = 1.;
  double pc2  = double(m_eventStat.m_particleClusters   [ 2])        / pa;
  double pc3  = double(m_eventStat.m_particleClusters   [ 3])        / pa;
  double pc4  = double(m_eventStat.m_particleClusters   [ 4])        / pa;
  double pc5  = double(m_eventStat.m_particleClusters   [ 5])        / pa;
  double pc6  = double(m_eventStat.m_particleClusters   [ 6])        / pa;
  double pc7  = double(m_eventStat.m_particleClusters   [ 7])        / pa;
  double pc8  = double(m_eventStat.m_particleClusters   [ 8])        / pa;
  double pc9  = double(m_eventStat.m_particleClusters   [ 9])        / pa;
  double pc10 = double(m_eventStat.m_particleClusters   [10])        / pa;
  double pc11 = double(m_eventStat.m_particleClusters   [11])        / pa;

  pa           = double(m_eventStat.m_particleClustersBTE[0][0]); if(pa < 1.) pa = 1.;
  double pcB2  = double(m_eventStat.m_particleClustersBTE[ 2][0])    / pa;
  double pcB3  = double(m_eventStat.m_particleClustersBTE[ 3][0])    / pa;
  double pcB4  = double(m_eventStat.m_particleClustersBTE[ 4][0])    / pa;
  double pcB5  = double(m_eventStat.m_particleClustersBTE[ 5][0])    / pa;
  double pcB6  = double(m_eventStat.m_particleClustersBTE[ 6][0])    / pa;
  double pcB7  = double(m_eventStat.m_particleClustersBTE[ 7][0])    / pa;
  double pcB8  = double(m_eventStat.m_particleClustersBTE[ 8][0])    / pa;
  double pcB9  = double(m_eventStat.m_particleClustersBTE[ 9][0])    / pa;
  double pcB10 = double(m_eventStat.m_particleClustersBTE[10][0])    / pa;
  double pcB11 = double(m_eventStat.m_particleClustersBTE[11][0])    / pa;
  pa           = double(m_eventStat.m_particleClustersBTE[0][1]); if(pa < 1.) pa = 1.;
  double pcT2  = double(m_eventStat.m_particleClustersBTE[ 2][1])    / pa;
  double pcT3  = double(m_eventStat.m_particleClustersBTE[ 3][1])    / pa;
  double pcT4  = double(m_eventStat.m_particleClustersBTE[ 4][1])    / pa;
  double pcT5  = double(m_eventStat.m_particleClustersBTE[ 5][1])    / pa;
  double pcT6  = double(m_eventStat.m_particleClustersBTE[ 6][1])    / pa;
  double pcT7  = double(m_eventStat.m_particleClustersBTE[ 7][1])    / pa;
  double pcT8  = double(m_eventStat.m_particleClustersBTE[ 8][1])    / pa;
  double pcT9  = double(m_eventStat.m_particleClustersBTE[ 9][1])    / pa;
  double pcT10 = double(m_eventStat.m_particleClustersBTE[10][1])    / pa;
  double pcT11 = double(m_eventStat.m_particleClustersBTE[11][1])    / pa;
  pa           = double(m_eventStat.m_particleClustersBTE[0][2]); if(pa < 1.) pa = 1.;
  double pcE2  = double(m_eventStat.m_particleClustersBTE[ 2][2])    / pa;
  double pcE3  = double(m_eventStat.m_particleClustersBTE[ 3][2])    / pa;
  double pcE4  = double(m_eventStat.m_particleClustersBTE[ 4][2])    / pa;
  double pcE5  = double(m_eventStat.m_particleClustersBTE[ 5][2])    / pa;
  double pcE6  = double(m_eventStat.m_particleClustersBTE[ 6][2])    / pa;
  double pcE7  = double(m_eventStat.m_particleClustersBTE[ 7][2])    / pa;
  double pcE8  = double(m_eventStat.m_particleClustersBTE[ 8][2])    / pa;
  double pcE9  = double(m_eventStat.m_particleClustersBTE[ 9][2])    / pa;
  double pcE10 = double(m_eventStat.m_particleClustersBTE[10][2])    / pa;
  double pcE11 = double(m_eventStat.m_particleClustersBTE[11][2])    / pa;

  pa           = double(m_eventStat.m_particleClustersBTE[0][3]); if(pa < 1.) pa = 1.;
  double pcD2  = double(m_eventStat.m_particleClustersBTE[ 2][3])    / pa;
  double pcD3  = double(m_eventStat.m_particleClustersBTE[ 3][3])    / pa;
  double pcD4  = double(m_eventStat.m_particleClustersBTE[ 4][3])    / pa;
  double pcD5  = double(m_eventStat.m_particleClustersBTE[ 5][3])    / pa;
  double pcD6  = double(m_eventStat.m_particleClustersBTE[ 6][3])    / pa;
  double pcD7  = double(m_eventStat.m_particleClustersBTE[ 7][3])    / pa;
  double pcD8  = double(m_eventStat.m_particleClustersBTE[ 8][3])    / pa;
  double pcD9  = double(m_eventStat.m_particleClustersBTE[ 9][3])    / pa;
  double pcD10 = double(m_eventStat.m_particleClustersBTE[10][3])    / pa;
  double pcD11 = double(m_eventStat.m_particleClustersBTE[11][3])    / pa;


  pa           = double(m_eventStat.m_particleSpacePoints[0]); if(pa < 1.) pa = 1.;
  double ps2   = double(m_eventStat.m_particleSpacePoints[ 2])       / pa;
  double ps3   = double(m_eventStat.m_particleSpacePoints[ 3])       / pa;
  double ps4   = double(m_eventStat.m_particleSpacePoints[ 4])       / pa;
  double ps5   = double(m_eventStat.m_particleSpacePoints[ 5])       / pa;
  double ps6   = double(m_eventStat.m_particleSpacePoints[ 6])       / pa;
  double ps7   = double(m_eventStat.m_particleSpacePoints[ 7])       / pa;
  double ps8   = double(m_eventStat.m_particleSpacePoints[ 8])       / pa;
  double ps9   = double(m_eventStat.m_particleSpacePoints[ 9])       / pa;
  double ps10  = double(m_eventStat.m_particleSpacePoints[10])       / pa;
  double ps11  = double(m_eventStat.m_particleSpacePoints[11])       / pa;
  pa           = double(m_eventStat.m_particleSpacePointsBTE[0][0]); if(pa < 1.) pa = 1.;
  double psB2  = double(m_eventStat.m_particleSpacePointsBTE[ 2][0]) / pa;
  double psB3  = double(m_eventStat.m_particleSpacePointsBTE[ 3][0]) / pa;
  double psB4  = double(m_eventStat.m_particleSpacePointsBTE[ 4][0]) / pa;
  double psB5  = double(m_eventStat.m_particleSpacePointsBTE[ 5][0]) / pa;
  double psB6  = double(m_eventStat.m_particleSpacePointsBTE[ 6][0]) / pa;
  double psB7  = double(m_eventStat.m_particleSpacePointsBTE[ 7][0]) / pa;
  double psB8  = double(m_eventStat.m_particleSpacePointsBTE[ 8][0]) / pa;
  double psB9  = double(m_eventStat.m_particleSpacePointsBTE[ 9][0]) / pa;
  double psB10 = double(m_eventStat.m_particleSpacePointsBTE[10][0]) / pa;
  double psB11 = double(m_eventStat.m_particleSpacePointsBTE[11][0]) / pa;
  pa           = double(m_eventStat.m_particleSpacePointsBTE[0][1]); if(pa < 1.) pa = 1.;
  double psT2  = double(m_eventStat.m_particleSpacePointsBTE[ 2][1]) / pa;
  double psT3  = double(m_eventStat.m_particleSpacePointsBTE[ 3][1]) / pa;
  double psT4  = double(m_eventStat.m_particleSpacePointsBTE[ 4][1]) / pa;
  double psT5  = double(m_eventStat.m_particleSpacePointsBTE[ 5][1]) / pa;
  double psT6  = double(m_eventStat.m_particleSpacePointsBTE[ 6][1]) / pa;
  double psT7  = double(m_eventStat.m_particleSpacePointsBTE[ 7][1]) / pa;
  double psT8  = double(m_eventStat.m_particleSpacePointsBTE[ 8][1]) / pa;
  double psT9  = double(m_eventStat.m_particleSpacePointsBTE[ 9][1]) / pa;
  double psT10 = double(m_eventStat.m_particleSpacePointsBTE[10][1]) / pa;
  double psT11 = double(m_eventStat.m_particleSpacePointsBTE[11][1]) / pa;
  pa           = double(m_eventStat.m_particleSpacePointsBTE[0][2]); if(pa < 1.) pa = 1.;
  double psE2  = double(m_eventStat.m_particleSpacePointsBTE[ 2][2]) / pa;
  double psE3  = double(m_eventStat.m_particleSpacePointsBTE[ 3][2]) / pa;
  double psE4  = double(m_eventStat.m_particleSpacePointsBTE[ 4][2]) / pa;
  double psE5  = double(m_eventStat.m_particleSpacePointsBTE[ 5][2]) / pa;
  double psE6  = double(m_eventStat.m_particleSpacePointsBTE[ 6][2]) / pa;
  double psE7  = double(m_eventStat.m_particleSpacePointsBTE[ 7][2]) / pa;
  double psE8  = double(m_eventStat.m_particleSpacePointsBTE[ 8][2]) / pa;
  double psE9  = double(m_eventStat.m_particleSpacePointsBTE[ 9][2]) / pa;
  double psE10 = double(m_eventStat.m_particleSpacePointsBTE[10][2]) / pa;
  double psE11 = double(m_eventStat.m_particleSpacePointsBTE[11][2]) / pa;

  pa           = double(m_eventStat.m_particleSpacePointsBTE[0][3]); if(pa < 1.) pa = 1.;
  double psD2  = double(m_eventStat.m_particleSpacePointsBTE[ 2][3]) / pa;
  double psD3  = double(m_eventStat.m_particleSpacePointsBTE[ 3][3]) / pa;
  double psD4  = double(m_eventStat.m_particleSpacePointsBTE[ 4][3]) / pa;
  double psD5  = double(m_eventStat.m_particleSpacePointsBTE[ 5][3]) / pa;
  double psD6  = double(m_eventStat.m_particleSpacePointsBTE[ 6][3]) / pa;
  double psD7  = double(m_eventStat.m_particleSpacePointsBTE[ 7][3]) / pa;
  double psD8  = double(m_eventStat.m_particleSpacePointsBTE[ 8][3]) / pa;
  double psD9  = double(m_eventStat.m_particleSpacePointsBTE[ 9][3]) / pa;
  double psD10 = double(m_eventStat.m_particleSpacePointsBTE[10][3]) / pa;
  double psD11 = double(m_eventStat.m_particleSpacePointsBTE[11][3]) / pa;


  std::cout<<"|         Propability for such charge particles to have some number silicon                          |"
	   <<std::endl;
  std::cout<<"|                     clusters                     |             space points                        |"
	   <<std::endl;
  std::cout<<"|           Total   Barrel  Transi  Endcap   DBM    |  Total   Barrel  Transi  Endcap   DBM          |"
	   <<std::endl;
  std::cout<<"|  >= 2  "
	   <<std::setw(8)<<std::setprecision(5)<<pc2
	   <<std::setw(8)<<std::setprecision(5)<<pcB2
	   <<std::setw(8)<<std::setprecision(5)<<pcT2
	   <<std::setw(8)<<std::setprecision(5)<<pcE2
	   <<std::setw(8)<<std::setprecision(5)<<pcD2<<"  |  "

	   <<std::setw(8)<<std::setprecision(5)<<ps2
	   <<std::setw(8)<<std::setprecision(5)<<psB2
	   <<std::setw(8)<<std::setprecision(5)<<psT2
	   <<std::setw(8)<<std::setprecision(5)<<psE2
	   <<std::setw(8)<<std::setprecision(5)<<psD2
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 3  "
	   <<std::setw(8)<<std::setprecision(5)<<pc3
	   <<std::setw(8)<<std::setprecision(5)<<pcB3
	   <<std::setw(8)<<std::setprecision(5)<<pcT3
	   <<std::setw(8)<<std::setprecision(5)<<pcE3
	   <<std::setw(8)<<std::setprecision(5)<<pcD3<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps3
	   <<std::setw(8)<<std::setprecision(5)<<psB3
	   <<std::setw(8)<<std::setprecision(5)<<psT3
	   <<std::setw(8)<<std::setprecision(5)<<psE3
	   <<std::setw(8)<<std::setprecision(5)<<psD3
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 4  "
	   <<std::setw(8)<<std::setprecision(5)<<pc4
	   <<std::setw(8)<<std::setprecision(5)<<pcB4
	   <<std::setw(8)<<std::setprecision(5)<<pcT4
	   <<std::setw(8)<<std::setprecision(5)<<pcE4
	   <<std::setw(8)<<std::setprecision(5)<<pcD4<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps4
	   <<std::setw(8)<<std::setprecision(5)<<psB4
	   <<std::setw(8)<<std::setprecision(5)<<psT4
	   <<std::setw(8)<<std::setprecision(5)<<psE4
	   <<std::setw(8)<<std::setprecision(5)<<psD4
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 5  "
	   <<std::setw(8)<<std::setprecision(5)<<pc5
	   <<std::setw(8)<<std::setprecision(5)<<pcB5
	   <<std::setw(8)<<std::setprecision(5)<<pcT5
	   <<std::setw(8)<<std::setprecision(5)<<pcE5
	   <<std::setw(8)<<std::setprecision(5)<<pcD5<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps5
	   <<std::setw(8)<<std::setprecision(5)<<psB5
	   <<std::setw(8)<<std::setprecision(5)<<psT5
	   <<std::setw(8)<<std::setprecision(5)<<psE5
	   <<std::setw(8)<<std::setprecision(5)<<psD5
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 6  "
	   <<std::setw(8)<<std::setprecision(5)<<pc6
	   <<std::setw(8)<<std::setprecision(5)<<pcB6
	   <<std::setw(8)<<std::setprecision(5)<<pcT6
	   <<std::setw(8)<<std::setprecision(5)<<pcE6
	   <<std::setw(8)<<std::setprecision(5)<<pcD6<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps6
	   <<std::setw(8)<<std::setprecision(5)<<psB6
	   <<std::setw(8)<<std::setprecision(5)<<psT6
	   <<std::setw(8)<<std::setprecision(5)<<psE6
	   <<std::setw(8)<<std::setprecision(5)<<psD6
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 7  "
	   <<std::setw(8)<<std::setprecision(5)<<pc7
	   <<std::setw(8)<<std::setprecision(5)<<pcB7
	   <<std::setw(8)<<std::setprecision(5)<<pcT7
	   <<std::setw(8)<<std::setprecision(5)<<pcE7
	   <<std::setw(8)<<std::setprecision(5)<<pcD7<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps7
	   <<std::setw(8)<<std::setprecision(5)<<psB7
	   <<std::setw(8)<<std::setprecision(5)<<psT7
	   <<std::setw(8)<<std::setprecision(5)<<psE7
	   <<std::setw(8)<<std::setprecision(5)<<psD7
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 8  "
	   <<std::setw(8)<<std::setprecision(5)<<pc8
	   <<std::setw(8)<<std::setprecision(5)<<pcB8
	   <<std::setw(8)<<std::setprecision(5)<<pcT8
	   <<std::setw(8)<<std::setprecision(5)<<pcE8
	   <<std::setw(8)<<std::setprecision(5)<<pcD8<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps8
	   <<std::setw(8)<<std::setprecision(5)<<psB8
	   <<std::setw(8)<<std::setprecision(5)<<psT8
	   <<std::setw(8)<<std::setprecision(5)<<psE8
	   <<std::setw(8)<<std::setprecision(5)<<psD8
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >= 9  "
	   <<std::setw(8)<<std::setprecision(5)<<pc9
	   <<std::setw(8)<<std::setprecision(5)<<pcB9
	   <<std::setw(8)<<std::setprecision(5)<<pcT9
	   <<std::setw(8)<<std::setprecision(5)<<pcE9
	   <<std::setw(8)<<std::setprecision(5)<<pcD9<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps9
	   <<std::setw(8)<<std::setprecision(5)<<psB9
	   <<std::setw(8)<<std::setprecision(5)<<psT9
	   <<std::setw(8)<<std::setprecision(5)<<psE9
	   <<std::setw(8)<<std::setprecision(5)<<psD9
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >=10  "
	   <<std::setw(8)<<std::setprecision(5)<<pc10
	   <<std::setw(8)<<std::setprecision(5)<<pcB10
	   <<std::setw(8)<<std::setprecision(5)<<pcT10
	   <<std::setw(8)<<std::setprecision(5)<<pcE10
	   <<std::setw(8)<<std::setprecision(5)<<pcD10<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps10
	   <<std::setw(8)<<std::setprecision(5)<<psB10
	   <<std::setw(8)<<std::setprecision(5)<<psT10
	   <<std::setw(8)<<std::setprecision(5)<<psE10
	   <<std::setw(8)<<std::setprecision(5)<<psD10
	   <<"       |"
	   <<std::endl;
  std::cout<<"|  >=11  "
	   <<std::setw(8)<<std::setprecision(5)<<pc11
	   <<std::setw(8)<<std::setprecision(5)<<pcB11
	   <<std::setw(8)<<std::setprecision(5)<<pcT11
	   <<std::setw(8)<<std::setprecision(5)<<pcE11
	   <<std::setw(8)<<std::setprecision(5)<<pcD11<<"  |  "
	   <<std::setw(8)<<std::setprecision(5)<<ps11
	   <<std::setw(8)<<std::setprecision(5)<<psB11
	   <<std::setw(8)<<std::setprecision(5)<<psT11
	   <<std::setw(8)<<std::setprecision(5)<<psE11
	   <<std::setw(8)<<std::setprecision(5)<<psD11
	   <<"       |"
	   <<std::endl;

  std::cout<<"|                                                                                   |"
	   <<std::endl;
  std::cout<<"|               Additional cuts for truth particles are                             |"
	   <<std::endl;

  std::cout<<"|                    number silicon clusters >="
	   <<std::setw(13)<<m_clcut
	   <<"                        |"<<std::endl;
  std::cout<<"|                    number   trt   clusters >="
	   <<std::setw(13)<<m_clcutTRT
	   <<"                        |"<<std::endl;
  std::cout<<"|                    number  space    points >="
	   <<std::setw(13)<<m_spcut
	   <<"                        |"<<std::endl;

  pa  = double(m_eventStat.m_particleClusters[0]); if(pa < 1.) pa = 1.;
  std::cout<<"|           Propability find truth particles with this cuts is "
	   <<std::setw(8)<<std::setprecision(5)<<double(m_eventStat.m_events)      /pa
	   <<"             |"
 	   <<std::endl;
  pa  = double(m_eventStat.m_particleClustersBTE[0][0]); if(pa < 1.) pa = 1.;
  std::cout<<"|                                        For barrel     region "
	   <<std::setw(8)<<std::setprecision(5)<<double(m_eventStat.m_eventsBTE[0])/pa
	   <<"             |"
 	   <<std::endl;
  pa  = double(m_eventStat.m_particleClustersBTE[0][1]); if(pa < 1.) pa = 1.;
  std::cout<<"|                                        For transition region "
	   <<std::setw(8)<<std::setprecision(5)<<double(m_eventStat.m_eventsBTE[1])/pa
	   <<"             |"
 	   <<std::endl;
  pa  = double(m_eventStat.m_particleClustersBTE[0][2]); if(pa < 1.) pa = 1.;
  std::cout<<"|                                        For endcap     region "
	   <<std::setw(8)<<std::setprecision(5)<<double(m_eventStat.m_eventsBTE[2])/pa
	   <<"             |"
 	   <<std::endl;
  pa  = double(m_eventStat.m_particleClustersBTE[0][3]); if(pa < 1.) pa = 1.;
  std::cout<<"|                                        For DBM        region "
           <<std::setw(8)<<std::setprecision(5)<<double(m_eventStat.m_eventsBTE[3])/pa
           <<"             |"
           <<std::endl;

  std::cout<<"|                                                                                   |"
	   <<std::endl;

  pa            = double(m_eventStat.m_nclustersNegBP); if(pa < 1.) pa = 1.;
  double ratio  = double(m_eventStat.m_nclustersPosBP)/pa;
  double eratio = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Ratio barrel pixels clusters for +/- particles ="
	   <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
	   <<std::setw(8)<<std::setprecision(5)<<eratio
	   <<"          |"
	   <<std::endl;
  pa            = double(m_eventStat.m_nclustersNegEP); if(pa < 1.) pa = 1.;
  ratio         = double(m_eventStat.m_nclustersPosEP)/pa;
  eratio        = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Ratio endcap pixels clusters for +/- particles ="
	   <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
	   <<std::setw(8)<<std::setprecision(5)<<eratio
	   <<"          |"
	   <<std::endl;
  pa            = double(m_eventStat.m_nclustersNegDBM); if(pa < 1.) pa = 1.;
  ratio         = double(m_eventStat.m_nclustersPosDBM)/pa;
  eratio        = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Ratio  DBM  pixels clusters for +/- particles = "
           <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
           <<std::setw(8)<<std::setprecision(5)<<eratio
           <<"          |"
           <<std::endl;
  pa            = double(m_eventStat.m_nclustersNegBS); if(pa < 1.) pa = 1.;
  ratio         = double(m_eventStat.m_nclustersPosBS)/pa;
  eratio        = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Ratio barrel   SCT  clusters for +/- particles ="
	   <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
	   <<std::setw(8)<<std::setprecision(5)<<eratio
	   <<"          |"
	   <<std::endl;
  pa            = double(m_eventStat.m_nclustersNegES); if(pa < 1.) pa = 1.;
  ratio         = double(m_eventStat.m_nclustersPosES)/pa;
  eratio        = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Ratio endcap   SCT  clusters for +/- particles ="
	   <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
	   <<std::setw(8)<<std::setprecision(5)<<eratio
	   <<"          |"
	   <<std::endl;
  pa            = double(m_eventStat.m_eventsNEG);      if(pa < 1.) pa = 1.;
  ratio         = double(m_eventStat.m_eventsPOS)/pa;
  eratio        = sqrt(ratio*(1.+ratio)/pa);
  std::cout<<"|      Number truth particles and +/- ratio ="
	   <<std::setw(10)<<m_eventStat.m_events
	   <<std::setw(8)<<std::setprecision(5)<<ratio<<" +-"
	   <<std::setw(8)<<std::setprecision(5)<<eratio
	   <<"          |"
	   <<std::endl;
  std::cout<<"|-----------------------------------------------------------------------------------|"
	   <<std::endl
	   <<std::endl;

  SG::ReadHandleKeyArray<TrackCollection>::const_iterator t=m_tracklocation.begin(),te=m_tracklocation.end();
  int nc = 0;
  for(; t!=te; ++t) {

    int   n     = 47-(t->key().size());
    std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");


    std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
    std::cout<<"|                      Statistic for "<<(t->key())<<s1<<std::endl;

    double ne = double(m_eventStat.m_events);  if(ne < 1.) ne = 1.;
    double ef [6]; for(int i=0; i!=6; ++i) ef [i] = double(m_trackCollectionStat[nc].m_efficiency   [i])   /ne;
    double ef0[6]; for(int i=0; i!=6; ++i) ef0[i] = double(m_trackCollectionStat[nc].m_efficiencyN  [i][0])/ne;
    double ef1[6]; for(int i=0; i!=6; ++i) ef1[i] = double(m_trackCollectionStat[nc].m_efficiencyN  [i][1])/ne;
    double ef2[6]; for(int i=0; i!=6; ++i) ef2[i] = double(m_trackCollectionStat[nc].m_efficiencyN  [i][2])/ne;
    /*
    double ef3[6]; for(int i=0; i!=6; ++i) ef3[i] = double(m_trackCollectionStat[nc].m_efficiencyN  [i][3])/ne;
    double ef4[6]; for(int i=0; i!=6; ++i) ef4[i] = double(m_trackCollectionStat[nc].m_efficiencyN  [i][4])/ne;
    */
    double neBTE = m_eventStat.m_eventsBTE[0]; if(neBTE < 1.) neBTE = 1;
    double efB0[6]; for(int i=0; i!=6; ++i) efB0[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][0][0])/neBTE;
    double efB1[6]; for(int i=0; i!=6; ++i) efB1[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][1][0])/neBTE;
    double efB2[6]; for(int i=0; i!=6; ++i) efB2[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][2][0])/neBTE;
    double efB3[6]; for(int i=0; i!=6; ++i) efB3[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][3][0])/neBTE;
    double efB4[6]; for(int i=0; i!=6; ++i) efB4[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][4][0])/neBTE;

    neBTE = m_eventStat.m_eventsBTE[1];        if(neBTE < 1.) neBTE = 1;
    double efT0[6]; for(int i=0; i!=6; ++i) efT0[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][0][1])/neBTE;
    double efT1[6]; for(int i=0; i!=6; ++i) efT1[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][1][1])/neBTE;
    double efT2[6]; for(int i=0; i!=6; ++i) efT2[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][2][1])/neBTE;
    double efT3[6]; for(int i=0; i!=6; ++i) efT3[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][3][1])/neBTE;
    double efT4[6]; for(int i=0; i!=6; ++i) efT4[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][4][1])/neBTE;

    neBTE = m_eventStat.m_eventsBTE[2];        if(neBTE < 1.) neBTE = 1;
    double efE0[6]; for(int i=0; i!=6; ++i) efE0[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][0][2])/neBTE;
    double efE1[6]; for(int i=0; i!=6; ++i) efE1[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][1][2])/neBTE;
    double efE2[6]; for(int i=0; i!=6; ++i) efE2[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][2][2])/neBTE;
    double efE3[6]; for(int i=0; i!=6; ++i) efE3[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][3][2])/neBTE;
    double efE4[6]; for(int i=0; i!=6; ++i) efE4[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][4][2])/neBTE;

    neBTE = m_eventStat.m_eventsBTE[3];        if(neBTE < 1.) neBTE = 1;
    double efD0[6]; for(int i=0; i!=6; ++i) efD0[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][0][3])/neBTE;
    double efD1[6]; for(int i=0; i!=6; ++i) efD1[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][1][3])/neBTE;
    double efD2[6]; for(int i=0; i!=6; ++i) efD2[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][2][3])/neBTE;
    double efD3[6]; for(int i=0; i!=6; ++i) efD3[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][3][3])/neBTE;
    double efD4[6]; for(int i=0; i!=6; ++i) efD4[i] = double(m_trackCollectionStat[nc].m_efficiencyBTE[i][4][3])/neBTE;


    double efrec  = ef0[0]+ef0[1]+ef0[2]+ef1[0]+ef1[1]+ef2[0];
    double efrecB = efB0[0]+efB0[1]+efB0[2]+efB1[0]+efB1[1]+efB2[0];
    double efrecT = efT0[0]+efT0[1]+efT0[2]+efT1[0]+efT1[1]+efT2[0];
    double efrecE = efE0[0]+efE0[1]+efE0[2]+efE1[0]+efE1[1]+efE2[0];
    double efrecD = efD0[0]+efD0[1]+efD0[2]+efD1[0]+efD1[1]+efD2[0];

    ne        = double(m_eventStat.m_eventsPOS); if(ne < 1.) ne = 1.;
    double efP[6]; for(int i=0; i!=6; ++i) efP[i] = double(m_trackCollectionStat[nc].m_efficiencyPOS[i])/ne;
    ne        = double(m_eventStat.m_eventsNEG); if(ne < 1.) ne = 1.;
    double efN[6]; for(int i=0; i!=6; ++i) efN[i] = double(m_trackCollectionStat[nc].m_efficiencyNEG[i])/ne;

    std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
    std::cout<<"| Probability to lose       0        1        2        3        4    >=5 clusters   |"
	     <<std::endl;
    std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;

    std::cout<<"| For all particles   "
	     <<std::setw(9)<<std::setprecision(5)<<ef[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef[5]<<"        |"
	     <<std::endl;
    std::cout<<"| For  +  particles   "
	     <<std::setw(9)<<std::setprecision(5)<<efP[0]
	     <<std::setw(9)<<std::setprecision(5)<<efP[1]
	     <<std::setw(9)<<std::setprecision(5)<<efP[2]
	     <<std::setw(9)<<std::setprecision(5)<<efP[3]
	     <<std::setw(9)<<std::setprecision(5)<<efP[4]
	     <<std::setw(9)<<std::setprecision(5)<<efP[5]<<"        |"
	     <<std::endl;
    std::cout<<"| For  -  particles   "
	     <<std::setw(9)<<std::setprecision(5)<<efN[0]
	     <<std::setw(9)<<std::setprecision(5)<<efN[1]
	     <<std::setw(9)<<std::setprecision(5)<<efN[2]
	     <<std::setw(9)<<std::setprecision(5)<<efN[3]
	     <<std::setw(9)<<std::setprecision(5)<<efN[4]
	     <<std::setw(9)<<std::setprecision(5)<<efN[5]<<"        |"
	     <<std::endl;
    /*
    std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
    std::cout<<"| Total                                                                             |"
	     <<std::endl;
    std::cout<<"|   0 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<ef0[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef0[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef0[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef0[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef0[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef0[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   1 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<ef1[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef1[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef1[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef1[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef1[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef1[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   2 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<ef2[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef2[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef2[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef2[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef2[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef2[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   3 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<ef3[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef3[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef3[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef3[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef3[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef3[5]<<"        |"
	     <<std::endl;
    std::cout<<"| >=4 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<ef4[0]
	     <<std::setw(9)<<std::setprecision(5)<<ef4[1]
	     <<std::setw(9)<<std::setprecision(5)<<ef4[2]
	     <<std::setw(9)<<std::setprecision(5)<<ef4[3]
	     <<std::setw(9)<<std::setprecision(5)<<ef4[4]
	     <<std::setw(9)<<std::setprecision(5)<<ef4[5]<<"        |"
	     <<std::endl;
    */
   std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
   std::cout<<"| Barrel region                                                                     |"
	     <<std::endl;
    std::cout<<"|   0 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efB0[0]
	     <<std::setw(9)<<std::setprecision(5)<<efB0[1]
	     <<std::setw(9)<<std::setprecision(5)<<efB0[2]
	     <<std::setw(9)<<std::setprecision(5)<<efB0[3]
	     <<std::setw(9)<<std::setprecision(5)<<efB0[4]
	     <<std::setw(9)<<std::setprecision(5)<<efB0[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   1 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efB1[0]
	     <<std::setw(9)<<std::setprecision(5)<<efB1[1]
	     <<std::setw(9)<<std::setprecision(5)<<efB1[2]
	     <<std::setw(9)<<std::setprecision(5)<<efB1[3]
	     <<std::setw(9)<<std::setprecision(5)<<efB1[4]
	     <<std::setw(9)<<std::setprecision(5)<<efB1[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   2 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efB2[0]
	     <<std::setw(9)<<std::setprecision(5)<<efB2[1]
	     <<std::setw(9)<<std::setprecision(5)<<efB2[2]
	     <<std::setw(9)<<std::setprecision(5)<<efB2[3]
	     <<std::setw(9)<<std::setprecision(5)<<efB2[4]
	     <<std::setw(9)<<std::setprecision(5)<<efB2[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   3 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efB3[0]
	     <<std::setw(9)<<std::setprecision(5)<<efB3[1]
	     <<std::setw(9)<<std::setprecision(5)<<efB3[2]
	     <<std::setw(9)<<std::setprecision(5)<<efB3[3]
	     <<std::setw(9)<<std::setprecision(5)<<efB3[4]
	     <<std::setw(9)<<std::setprecision(5)<<efB3[5]<<"        |"
	     <<std::endl;
    std::cout<<"| >=4 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efB4[0]
	     <<std::setw(9)<<std::setprecision(5)<<efB4[1]
	     <<std::setw(9)<<std::setprecision(5)<<efB4[2]
	     <<std::setw(9)<<std::setprecision(5)<<efB4[3]
	     <<std::setw(9)<<std::setprecision(5)<<efB4[4]
	     <<std::setw(9)<<std::setprecision(5)<<efB4[5]<<"        |"
	     <<std::endl;
   std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
   std::cout<<"| Transition region                                                                 |"
	     <<std::endl;
    std::cout<<"|   0 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efT0[0]
	     <<std::setw(9)<<std::setprecision(5)<<efT0[1]
	     <<std::setw(9)<<std::setprecision(5)<<efT0[2]
	     <<std::setw(9)<<std::setprecision(5)<<efT0[3]
	     <<std::setw(9)<<std::setprecision(5)<<efT0[4]
	     <<std::setw(9)<<std::setprecision(5)<<efT0[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   1 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efT1[0]
	     <<std::setw(9)<<std::setprecision(5)<<efT1[1]
	     <<std::setw(9)<<std::setprecision(5)<<efT1[2]
	     <<std::setw(9)<<std::setprecision(5)<<efT1[3]
	     <<std::setw(9)<<std::setprecision(5)<<efT1[4]
	     <<std::setw(9)<<std::setprecision(5)<<efT1[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   2 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efT2[0]
	     <<std::setw(9)<<std::setprecision(5)<<efT2[1]
	     <<std::setw(9)<<std::setprecision(5)<<efT2[2]
	     <<std::setw(9)<<std::setprecision(5)<<efT2[3]
	     <<std::setw(9)<<std::setprecision(5)<<efT2[4]
	     <<std::setw(9)<<std::setprecision(5)<<efT2[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   3 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efT3[0]
	     <<std::setw(9)<<std::setprecision(5)<<efT3[1]
	     <<std::setw(9)<<std::setprecision(5)<<efT3[2]
	     <<std::setw(9)<<std::setprecision(5)<<efT3[3]
	     <<std::setw(9)<<std::setprecision(5)<<efT3[4]
	     <<std::setw(9)<<std::setprecision(5)<<efT3[5]<<"        |"
	     <<std::endl;
    std::cout<<"| >=4 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efT4[0]
	     <<std::setw(9)<<std::setprecision(5)<<efT4[1]
	     <<std::setw(9)<<std::setprecision(5)<<efT4[2]
	     <<std::setw(9)<<std::setprecision(5)<<efT4[3]
	     <<std::setw(9)<<std::setprecision(5)<<efT4[4]
	     <<std::setw(9)<<std::setprecision(5)<<efT4[5]<<"        |"
	     <<std::endl;
   std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
   std::cout<<"| Endcap region                                                                     |"
	     <<std::endl;
    std::cout<<"|   0 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efE0[0]
	     <<std::setw(9)<<std::setprecision(5)<<efE0[1]
	     <<std::setw(9)<<std::setprecision(5)<<efE0[2]
	     <<std::setw(9)<<std::setprecision(5)<<efE0[3]
	     <<std::setw(9)<<std::setprecision(5)<<efE0[4]
	     <<std::setw(9)<<std::setprecision(5)<<efE0[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   1 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efE1[0]
	     <<std::setw(9)<<std::setprecision(5)<<efE1[1]
	     <<std::setw(9)<<std::setprecision(5)<<efE1[2]
	     <<std::setw(9)<<std::setprecision(5)<<efE1[3]
	     <<std::setw(9)<<std::setprecision(5)<<efE1[4]
	     <<std::setw(9)<<std::setprecision(5)<<efE1[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   2 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efE2[0]
	     <<std::setw(9)<<std::setprecision(5)<<efE2[1]
	     <<std::setw(9)<<std::setprecision(5)<<efE2[2]
	     <<std::setw(9)<<std::setprecision(5)<<efE2[3]
	     <<std::setw(9)<<std::setprecision(5)<<efE2[4]
	     <<std::setw(9)<<std::setprecision(5)<<efE2[5]<<"        |"
	     <<std::endl;
    std::cout<<"|   3 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efE3[0]
	     <<std::setw(9)<<std::setprecision(5)<<efE3[1]
	     <<std::setw(9)<<std::setprecision(5)<<efE3[2]
	     <<std::setw(9)<<std::setprecision(5)<<efE3[3]
	     <<std::setw(9)<<std::setprecision(5)<<efE3[4]
	     <<std::setw(9)<<std::setprecision(5)<<efE3[5]<<"        |"
	     <<std::endl;
    std::cout<<"| >=4 wrong clusters  "
	     <<std::setw(9)<<std::setprecision(5)<<efE4[0]
	     <<std::setw(9)<<std::setprecision(5)<<efE4[1]
	     <<std::setw(9)<<std::setprecision(5)<<efE4[2]
	     <<std::setw(9)<<std::setprecision(5)<<efE4[3]
	     <<std::setw(9)<<std::setprecision(5)<<efE4[4]
	     <<std::setw(9)<<std::setprecision(5)<<efE4[5]<<"        |"
	     <<std::endl;
    std::cout<<"|-----------------------------------------------------------------------------------|"
             <<std::endl;
    std::cout<<"| DBM region                                                                        |"
             <<std::endl;
    std::cout<<"|   0 wrong clusters  "
             <<std::setw(9)<<std::setprecision(5)<<efD0[0]
             <<std::setw(9)<<std::setprecision(5)<<efD0[1]
             <<std::setw(9)<<std::setprecision(5)<<efD0[2]
             <<std::setw(9)<<std::setprecision(5)<<efD0[3]
             <<std::setw(9)<<std::setprecision(5)<<efD0[4]
             <<std::setw(9)<<std::setprecision(5)<<efD0[5]<<"        |"
             <<std::endl;
    std::cout<<"|   1 wrong clusters  "
             <<std::setw(9)<<std::setprecision(5)<<efD1[0]
             <<std::setw(9)<<std::setprecision(5)<<efD1[1]
             <<std::setw(9)<<std::setprecision(5)<<efD1[2]
             <<std::setw(9)<<std::setprecision(5)<<efD1[3]
             <<std::setw(9)<<std::setprecision(5)<<efD1[4]
             <<std::setw(9)<<std::setprecision(5)<<efD1[5]<<"        |"
             <<std::endl;
    std::cout<<"|   2 wrong clusters  "
             <<std::setw(9)<<std::setprecision(5)<<efD2[0]
             <<std::setw(9)<<std::setprecision(5)<<efD2[1]
             <<std::setw(9)<<std::setprecision(5)<<efD2[2]
             <<std::setw(9)<<std::setprecision(5)<<efD2[3]
             <<std::setw(9)<<std::setprecision(5)<<efD2[4]
             <<std::setw(9)<<std::setprecision(5)<<efD2[5]<<"        |"
             <<std::endl;
    std::cout<<"|   3 wrong clusters  "
             <<std::setw(9)<<std::setprecision(5)<<efD3[0]
             <<std::setw(9)<<std::setprecision(5)<<efD3[1]
             <<std::setw(9)<<std::setprecision(5)<<efD3[2]
             <<std::setw(9)<<std::setprecision(5)<<efD3[3]
             <<std::setw(9)<<std::setprecision(5)<<efD3[4]
             <<std::setw(9)<<std::setprecision(5)<<efD3[5]<<"        |"
             <<std::endl;
    std::cout<<"| >=4 wrong clusters  "
             <<std::setw(9)<<std::setprecision(5)<<efD4[0]
             <<std::setw(9)<<std::setprecision(5)<<efD4[1]
             <<std::setw(9)<<std::setprecision(5)<<efD4[2]
             <<std::setw(9)<<std::setprecision(5)<<efD4[3]
             <<std::setw(9)<<std::setprecision(5)<<efD4[4]
             <<std::setw(9)<<std::setprecision(5)<<efD4[5]<<"        |"
             <<std::endl;

   std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
   pa  = double(m_eventStat.m_particleClusters[0]);       if(pa < 1.) pa = 1.;
   std::cout<<"| Efficiency reconstruction (number lose+wrong < 3) = "
	    <<std::setw(9)<<std::setprecision(5)<<efrec
	    <<" ("
	    <<std::setw(9)<<std::setprecision(5)<<efrec*double(m_eventStat.m_events)/pa
	    <<" ) "
	    <<"       |"
	    <<std::endl;
   pa  = double(m_eventStat.m_particleClustersBTE[0][0]); if(pa < 1.) pa = 1.;
   std::cout<<"|                             For barrel     region = "
	    <<std::setw(9)<<std::setprecision(5)<<efrecB
	    <<" ("
	    <<std::setw(9)<<std::setprecision(5)<<efrecB*double(m_eventStat.m_eventsBTE[0])/pa
	    <<" ) "
	    <<"       |"
	    <<std::endl;
   pa  = double(m_eventStat.m_particleClustersBTE[0][1]);  if(pa < 1.) pa = 1.;
   std::cout<<"|                             For transition region = "
	    <<std::setw(9)<<std::setprecision(5)<<efrecT
	    <<" ("
	    <<std::setw(9)<<std::setprecision(5)<<efrecT*double(m_eventStat.m_eventsBTE[1])/pa
	    <<" ) "
	    <<"       |"
	    <<std::endl;
   pa  = double(m_eventStat.m_particleClustersBTE[0][2]);  if(pa < 1.) pa = 1.;
   std::cout<<"|                             For endcap     region = "
	    <<std::setw(9)<<std::setprecision(5)<<efrecE
	    <<" ("
	    <<std::setw(9)<<std::setprecision(5)<<efrecE*double(m_eventStat.m_eventsBTE[2])/pa
	    <<" ) "
	    <<"       |"
	    <<std::endl;
   pa  = double(m_eventStat.m_particleClustersBTE[0][3]);  if(pa < 1.) pa = 1.;
   std::cout<<"|                             For DBM        region = "
            <<std::setw(9)<<std::setprecision(5)<<efrecD
            <<" ("
            <<std::setw(9)<<std::setprecision(5)<<efrecD*double(m_eventStat.m_eventsBTE[3])/pa
            <<" ) "
            <<"       |"
            <<std::endl;

   std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
    std::cout<<"| Reconstructed tracks         +          -    +/-ration    error                   |"
	     <<std::endl;
    std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;

    pa     = double(m_trackCollectionStat[nc].m_ntracksNEGB);  if(pa < 1.) pa = 1.;
    ratio  = double(m_trackCollectionStat[nc].m_ntracksPOSB)/pa;
    eratio = sqrt(ratio*(1.+ratio)/pa);

    std::cout<<"| Barrel               "
	     <<std::setw(10)<<m_trackCollectionStat[nc].m_ntracksPOSB
	     <<std::setw(11)<<m_trackCollectionStat[nc].m_ntracksNEGB
	     <<std::setw(11)<<std::setprecision(5)<<ratio
	     <<std::setw(11)<<std::setprecision(5)<<eratio<<"                  |"
	     <<std::endl;
    pa     = double(m_trackCollectionStat[nc].m_ntracksNEGE); if(pa < 1.) pa = 1.;
    ratio  = double(m_trackCollectionStat[nc].m_ntracksPOSE)/pa;
    eratio = sqrt(ratio*(1.+ratio)/pa);

    std::cout<<"| Endcap               "
	     <<std::setw(10)<<m_trackCollectionStat[nc].m_ntracksPOSE
	     <<std::setw(11)<<m_trackCollectionStat[nc].m_ntracksNEGE
	     <<std::setw(11)<<std::setprecision(5)<<ratio
	     <<std::setw(11)<<std::setprecision(5)<<eratio<<"                  |"
	     <<std::endl;
    pa     = double(m_trackCollectionStat[nc].m_ntracksNEGDBM); if(pa < 1.) pa = 1.;
    ratio  = double(m_trackCollectionStat[nc].m_ntracksPOSDBM)/pa;
    eratio = sqrt(ratio*(1.+ratio)/pa);

    std::cout<<"| DBM                  "
             <<std::setw(10)<<m_trackCollectionStat[nc].m_ntracksPOSDBM
             <<std::setw(11)<<m_trackCollectionStat[nc].m_ntracksNEGDBM
             <<std::setw(11)<<std::setprecision(5)<<ratio
             <<std::setw(11)<<std::setprecision(5)<<eratio<<"                  |"
             <<std::endl;



    int nt=0;
    int ft=0;
    int kf=0;
    for(int k = 0; k!=50; ++k) {

      nt+=m_trackCollectionStat[nc].m_total[k];
      ft+=m_trackCollectionStat[nc].m_fake [k];
      if(!kf && nt) kf = k;
    }

    if(kf) {

      std::cout<<"|-----------------------------------------------------------------------------------|"
	       <<std::endl;
      std::cout<<"|             Fake tracks rate for different number of clusters on track            |"
	       <<std::endl;
      std::cout<<"|-----------------------------------------------------------------------------------|"
	       <<std::endl;

      for(int k = kf; k!=kf+6; ++k) {
	std::cout<<"|     >= "<<std::setw(2)<<k<<"   ";
      }
      std::cout<<"|"<<std::endl;

      for(int k = kf; k!=kf+6; ++k) {
	double eff = 0.; if(nt>0) eff = double(ft)/double(nt);
	std::cout<<"|"<<std::setw(12)<<std::setprecision(5)<<eff<<" ";
	nt-=m_trackCollectionStat[nc].m_total[k];
	ft-=m_trackCollectionStat[nc].m_fake [k];
      }
      std::cout<<"|"<<std::endl;
      std::cout<<"|-----------------------------------------------------------------------------------|"
	     <<std::endl;
    }
    ++nc;
  }

   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TrackClusterAssValidation::dumptools( MsgStream& out, MSG::Level level) const
{
  SG::ReadHandleKeyArray<TrackCollection>::const_iterator t=m_tracklocation.begin(),te=m_tracklocation.end();

  int n;
  out << level << std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  for(; t!=te; ++t) {
    n     = 65-t->key().size();
    std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

    out<<"| Location of input tracks                        | "<<t->key()<<s1
       <<std::endl;
  }
  n     = 65-m_spacepointsPixelname.key().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_spacepointsSCTname.key().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_spacepointsOverlapname.key().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 65-m_clustersPixelname.key().size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");
  n     = 65-m_clustersSCTname.key().size();
  std::string s6; for(int i=0; i<n; ++i) s6.append(" "); s6.append("|");
  n     = 65-m_clustersTRTname.key().size();
  std::string s9; for(int i=0; i<n; ++i) s9.append(" "); s9.append("|");
  n     = 65-m_truth_locationPixel.key().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");
  n     = 65-m_truth_locationSCT.key().size();
  std::string s8; for(int i=0; i<n; ++i) s8.append(" "); s8.append("|");
  n     = 65-m_truth_locationTRT.key().size();
  std::string s10; for(int i=0; i<n; ++i) s10.append(" "); s10.append("|");

  out<<"| Pixel    space points                           | "<<m_spacepointsPixelname.key()  <<s2
     <<std::endl;
  out<<"| SCT      space points                           | "<<m_spacepointsSCTname.key()    <<s3
     <<std::endl;
  out<<"| Overlap  space points                           | "<<m_spacepointsOverlapname.key()<<s4
     <<std::endl;
  out<<"| Pixel    clusters                               | "<<m_clustersPixelname.key()     <<s5
     <<std::endl;
  out<<"| SCT      clusters                               | "<<m_clustersSCTname.key()       <<s6
     <<std::endl;
  out<<"| TRT      clusters                               | "<<m_clustersTRTname.key()       <<s9
     <<std::endl;
  out<<"| Truth location  for pixels                      | "<<m_truth_locationPixel.key()   <<s7
     <<std::endl;
  out<<"| Truth location  for sct                         | "<<m_truth_locationSCT.key()     <<s8
     <<std::endl;
  out<<"| Truth location  for trt                         | "<<m_truth_locationTRT.key()     <<s10
     <<std::endl;
  out<<"|         pT cut                                  | "
     <<std::setw(14)<<std::setprecision(5)<<m_ptcut
     <<"                                                   |"
     <<std::endl;
  out<<"|   max   pT cut                                  | "
     <<std::setw(14)<<std::setprecision(5)<<m_ptcutmax
     <<"                                                   |"
     <<std::endl;
  out<<"|   rapidity cut                                  | "
     <<std::setw(14)<<std::setprecision(5)<<m_rapcut
     <<"                                                   |"
     <<std::endl;
  out<<"| min Radius                                      | "
     <<std::setw(14)<<std::setprecision(5)<<m_rmin
     <<"                                                   |"
     <<std::endl;
  out<<"| max Radius                                      | "
     <<std::setw(14)<<std::setprecision(5)<<m_rmax
     <<"                                                   |"
     <<std::endl;
  out<<"| Min. number clusters      for generated track   | "
     <<std::setw(14)<<std::setprecision(5)<<m_clcut
     <<"                                                   |"
     <<std::endl;
  out<<"| Min. number sp.points     for generated track   | "
     <<std::setw(14)<<std::setprecision(5)<<m_spcut
     <<"                                                   |"
     <<std::endl;
  out<<"| Min. number TRT clusters  for generated track   | "
     <<std::setw(14)<<std::setprecision(5)<<m_clcutTRT
     <<"                                                   |"
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TrackClusterAssValidation::dumpevent( MsgStream& out, const InDet::TrackClusterAssValidation::EventData_t &event_data ) const
{
  out << MSG::DEBUG << std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| m_nspacepoints          | "
     <<std::setw(12)<<event_data.m_nspacepoints
     <<"                              |"<<std::endl;
  out<<"| m_nclusters             | "
     <<std::setw(12)<<event_data.m_nclusters
     <<"                              |"<<std::endl;
  out<<"| Kine-Clusters    size   | "
     <<std::setw(12)<<event_data.m_kinecluster.size()
     <<"                              |"<<std::endl;
  out<<"| Kine-TRTClusters size   | "
     <<std::setw(12)<<event_data.m_kineclusterTRT.size()
     <<"                              |"<<std::endl;
  out<<"| Kine-SpacePoints size   | "
     <<std::setw(12)<<event_data.m_kinespacepoint.size()
     <<"                              |"<<std::endl;
  out<<"| Number good kine tracks | "
     <<std::setw(12)<<event_data.m_nqtracks
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}


///////////////////////////////////////////////////////////////////
// New event for clusters information
///////////////////////////////////////////////////////////////////

void InDet::TrackClusterAssValidation::newClustersEvent(const EventContext& ctx,InDet::TrackClusterAssValidation::EventData_t &event_data) const
{
  // Get pixel clusters container
  //
  std::unique_ptr<SG::ReadHandle<SiClusterContainer> >       pixelcontainer;
  std::unique_ptr<SG::ReadHandle<SiClusterContainer> >       sctcontainer;
  std::unique_ptr<SG::ReadHandle<TRT_DriftCircleContainer> > trtcontainer;

  if(m_usePIX) {
    pixelcontainer = std::make_unique<SG::ReadHandle<SiClusterContainer> >(m_clustersPixelname,ctx);
    if (!pixelcontainer->isValid()) ATH_MSG_DEBUG("Failed to create Pixel clusters container read handle with key " << m_clustersPixelname.key());
  }

  // Get sct   clusters container
  //
  if(m_useSCT) {
    sctcontainer = std::make_unique<SG::ReadHandle<SiClusterContainer> >(m_clustersSCTname,ctx);
    if (!sctcontainer->isValid()) ATH_MSG_DEBUG("Failed to create SCT clusters container read handle with key " << m_clustersSCTname.key());
  }

  // Get trt   cluster container
  //
  if(m_clcutTRT > 0) {
    trtcontainer = std::make_unique<SG::ReadHandle<TRT_DriftCircleContainer> >(m_clustersTRTname,ctx);
    if (!trtcontainer->isValid()) ATH_MSG_DEBUG("Failed to create TRT drift circle container read handle with key " << m_clustersTRTname.key());
  }

  int Kine[1000];

  event_data.m_clusterHandles.reserve(3);
  // Loop through all pixel clusters
  //
  if(pixelcontainer && pixelcontainer->isValid()) {
    InDet::SiClusterContainer::const_iterator w  =  (*pixelcontainer)->begin();
    InDet::SiClusterContainer::const_iterator we =  (*pixelcontainer)->end  ();

    for(; w!=we; ++w) {

      InDet::SiClusterCollection::const_iterator c  = (*w)->begin();
      InDet::SiClusterCollection::const_iterator ce = (*w)->end  ();

      for(; c!=ce; ++c) {

	++event_data.m_nclusters;

	int nk = kine(event_data,(*c),Kine,999);
	for(int i=0; i!=nk; ++i) {
	  if(!isTheSameDetElement(event_data,Kine[i],(*c))) {
	    event_data.m_kinecluster.insert(std::make_pair(Kine[i],(*c)));
	  }
	}
      }
    }
    event_data.m_clusterHandles.push_back(std::move(pixelcontainer));

  }

  // Loop through all sct clusters
  //
  if(sctcontainer && sctcontainer->isValid()) {
    InDet::SiClusterContainer::const_iterator w  =  (*sctcontainer)->begin();
    InDet::SiClusterContainer::const_iterator we =  (*sctcontainer)->end  ();

    for(; w!=we; ++w) {

      InDet::SiClusterCollection::const_iterator c  = (*w)->begin();
      InDet::SiClusterCollection::const_iterator ce = (*w)->end  ();

      for(; c!=ce; ++c) {

	++event_data.m_nclusters;

	int nk = kine(event_data,(*c),Kine,999);
	for(int i=0; i!=nk; ++i) {
	  if(!isTheSameDetElement(event_data,Kine[i],(*c))) event_data.m_kinecluster.insert(std::make_pair(Kine[i],(*c)));
	}
      }
    }
    event_data.m_clusterHandles.push_back(std::move(sctcontainer));
  }

  if(trtcontainer && trtcontainer->isValid()) {
    // Loop through all trt clusters
    //
    InDet::TRT_DriftCircleContainer::const_iterator  w  = (*trtcontainer)->begin();
    InDet::TRT_DriftCircleContainer::const_iterator  we = (*trtcontainer)->end  ();

    for(; w!=we; ++w) {

      InDet::TRT_DriftCircleCollection::const_iterator c  = (*w)->begin();
      InDet::TRT_DriftCircleCollection::const_iterator ce = (*w)->end  ();

      for(; c!=ce; ++c) {

        ++event_data.m_nclustersTRT;
        int nk = kine(event_data,(*c),Kine,999);
        for(int i=0; i!=nk; ++i) event_data.m_kineclusterTRT.insert(std::make_pair(Kine[i],(*c)));
      }
    }
    event_data.m_clusterHandles.push_back(std::move(trtcontainer));
  }
}

///////////////////////////////////////////////////////////////////
// New event for space points information
///////////////////////////////////////////////////////////////////

void InDet::TrackClusterAssValidation::newSpacePointsEvent(const EventContext& ctx, InDet::TrackClusterAssValidation::EventData_t &event_data) const
{

  int Kine[1000];

  if(m_usePIX && !m_spacepointsPixelname.key().empty()) {
    event_data.m_spacePointContainer.push_back(SG::ReadHandle<SpacePointContainer>(m_spacepointsPixelname,ctx));
    if (!event_data.m_spacePointContainer.back().isValid()) {
      ATH_MSG_DEBUG( "Invalid Pixels space points container read handle for key " << m_spacepointsPixelname.key()  );
    }
    else  {
      SpacePointContainer::const_iterator spc  =  event_data.m_spacePointContainer.back()->begin();
      SpacePointContainer::const_iterator spce =  event_data.m_spacePointContainer.back()->end  ();
      for(; spc != spce; ++spc) {
        SpacePointCollection::const_iterator sp  = (*spc)->begin();
        SpacePointCollection::const_iterator spe = (*spc)->end  ();

        for(; sp != spe; ++sp) {

          ++event_data.m_nspacepoints;
          int nk = kine(event_data,(*sp)->clusterList().first,Kine,999);
          for(int i=0; i!=nk; ++i) {

            if(!isTheSameDetElement(event_data,Kine[i],(*sp))) {
              event_data.m_kinespacepoint.insert(std::make_pair(Kine[i],(*sp)));
            }
          }
        }
      }
    }
  }
  // Get sct space points containers from store gate
  //
  if(m_useSCT && !m_spacepointsSCTname.key().empty()) {
    event_data.m_spacePointContainer.push_back(SG::ReadHandle<SpacePointContainer>(m_spacepointsSCTname,ctx));
    if (!event_data.m_spacePointContainer.back().isValid()) {
      ATH_MSG_DEBUG( "Invalid SCT space points container read handle for key " << m_spacepointsSCTname.key() );
    }
    else  {
      SpacePointContainer::const_iterator spc  =  event_data.m_spacePointContainer.back()->begin();
      SpacePointContainer::const_iterator spce =  event_data.m_spacePointContainer.back()->end  ();

      for(; spc != spce; ++spc) {

        SpacePointCollection::const_iterator sp  = (*spc)->begin();
        SpacePointCollection::const_iterator spe = (*spc)->end  ();

        for(; sp != spe; ++sp) {


          ++event_data.m_nspacepoints;
          int nk = kine(event_data,(*sp)->clusterList().first,(*sp)->clusterList().second,Kine,999);
          for(int i=0; i!=nk; ++i) {
            if(!isTheSameDetElement(event_data,Kine[i],(*sp))) {
              event_data.m_kinespacepoint.insert(std::make_pair(Kine[i],(*sp)));
            }
          }
        }
      }
    }
  }
  // Get sct overlap space points containers from store gate
  //
  if(m_useSCT && !m_spacepointsOverlapname.key().empty()) {
    event_data.m_spacepointsOverlap=std::make_unique< SG::ReadHandle<SpacePointOverlapCollection> >(m_spacepointsOverlapname,ctx);
    if (!event_data.m_spacepointsOverlap->isValid()) {
      ATH_MSG_DEBUG( "Invalid overlap space points container read handle for key " << m_spacepointsOverlapname.key() );
    }
    else  {
      SpacePointOverlapCollection::const_iterator sp  = (*(event_data.m_spacepointsOverlap))->begin();
      SpacePointOverlapCollection::const_iterator spe = (*(event_data.m_spacepointsOverlap))->end  ();

      for (; sp!=spe; ++sp) {

        ++event_data.m_nspacepoints;
        int nk = kine(event_data,(*sp)->clusterList().first,(*sp)->clusterList().second,Kine,999);
        for(int i=0; i!=nk; ++i) {
          if(!isTheSameDetElement(event_data,Kine[i],(*sp))) {
            event_data.m_kinespacepoint.insert(std::make_pair(Kine[i],(*sp)));
          }
        }
      }
    }
  }
}
///////////////////////////////////////////////////////////////////
// Good kine tracks  selection
///////////////////////////////////////////////////////////////////

int InDet::TrackClusterAssValidation::QualityTracksSelection(InDet::TrackClusterAssValidation::EventData_t &event_data) const
{
  std::multimap<int,const Trk::PrepRawData*>::iterator c = event_data.m_kinecluster   .begin();
  std::multimap<int,const Trk::PrepRawData*>::iterator u = event_data.m_kineclusterTRT.begin();
  std::multimap<int,const Trk::SpacePoint*>::iterator  s = event_data.m_kinespacepoint.begin();

  if( c == event_data.m_kinecluster.end()) {
    return 0;
  }

  if( s == event_data.m_kinespacepoint.end()) {
    return 0;
  }

  if( m_clcutTRT > 0 && u == event_data.m_kineclusterTRT.end()) {
    return 0;
  }

  std::list<int> worskine;

  int          rp = 0;
  int          t  = 0;
  int          k0 = (*c).first;
  int          q0 = k0*charge(event_data,(*c),rp);
  unsigned int nc = 1         ;

  for(++c; c!=event_data.m_kinecluster.end(); ++c) {

    if((*c).first==k0) {++nc; continue;}
    q0 = charge(event_data,(*c),rp)*k0;

    nc < 50 ?  ++event_data.m_eventStat.m_particleClusters   [nc]     : ++event_data.m_eventStat.m_particleClusters   [49];
    nc < 50 ?  ++event_data.m_eventStat.m_particleClustersBTE[nc][rp] : ++event_data.m_eventStat.m_particleClustersBTE[49][rp];


    int ns = event_data.m_kinespacepoint.count(k0);
    ns < 50 ?  ++event_data.m_eventStat.m_particleSpacePoints   [ns]     : ++event_data.m_eventStat.m_particleSpacePoints   [49];
    ns < 50 ?  ++event_data.m_eventStat.m_particleSpacePointsBTE[ns][rp] : ++event_data.m_eventStat.m_particleSpacePointsBTE[49][rp];

    if     (nc                        < m_clcut   ) worskine.push_back(k0);
    else if(event_data.m_kinespacepoint.count(k0)< m_spcut   ) worskine.push_back(k0);
    else if(event_data.m_kineclusterTRT.count(k0)< m_clcutTRT) worskine.push_back(k0);
    else {
      InDet::Barcode BC(q0,rp); event_data.m_particles[0].push_back(BC); ++t;
    }

    k0 = (*c).first;
    q0 =charge(event_data,(*c),rp)*k0;
    nc = 1         ;
  }

  nc < 50 ?  ++event_data.m_eventStat.m_particleClusters   [nc]     : ++event_data.m_eventStat.m_particleClusters   [49];
  nc < 50 ?  ++event_data.m_eventStat.m_particleClustersBTE[nc][rp] : ++event_data.m_eventStat.m_particleClustersBTE[49][rp];
  int ns = event_data.m_kinespacepoint.count(k0);
  ns < 50 ?  ++event_data.m_eventStat.m_particleSpacePoints   [ns]     : ++event_data.m_eventStat.m_particleSpacePoints   [49];
  ns < 50 ?  ++event_data.m_eventStat.m_particleSpacePointsBTE[ns][rp] : ++event_data.m_eventStat.m_particleSpacePointsBTE[49][rp];

  if     (nc                        < m_clcut   ) worskine.push_back(k0);
  else if(event_data.m_kinespacepoint.count(k0)< m_spcut   ) worskine.push_back(k0);
  else if(event_data.m_kineclusterTRT.count(k0)< m_clcutTRT) worskine.push_back(k0);
  else {
    InDet::Barcode BC(q0,rp); event_data.m_particles[0].push_back(BC); ++t;
  }


  std::list<int>::iterator w=worskine.begin(), we=worskine.end();

  for(; w!=we; ++w) {
    event_data.m_kinecluster   .erase((*w));
    event_data.m_kineclusterTRT.erase((*w));
    event_data.m_kinespacepoint.erase((*w));
  }

  for(c = event_data.m_kinecluster.begin(); c!= event_data.m_kinecluster.end(); ++c) {

    const Trk::PrepRawData*
      d = (*c).second;
    const InDetDD::SiDetectorElement*
      de= dynamic_cast<const InDetDD::SiDetectorElement*>(d->detectorElement());
    int q  = charge(event_data,*c,rp);

    if     (q<0) {
      if (de->isDBM())
	++event_data.m_eventStat.m_nclustersNegDBM;
      else if(de->isBarrel()) {
	de->isPixel() ? ++event_data.m_eventStat.m_nclustersNegBP : ++event_data.m_eventStat.m_nclustersNegBS;
      }
      else                                     {
	de->isPixel() ? ++event_data.m_eventStat.m_nclustersNegEP : ++event_data.m_eventStat.m_nclustersNegES;
      }

    }
    else if(q>0) {
      if (de->isDBM())
	++event_data.m_eventStat.m_nclustersPosDBM;
      else if(de->isBarrel()) {
	de->isPixel() ? ++event_data.m_eventStat.m_nclustersPosBP : ++event_data.m_eventStat.m_nclustersPosBS;
      }
      else                                     {
	de->isPixel() ? ++event_data.m_eventStat.m_nclustersPosEP : ++event_data.m_eventStat.m_nclustersPosES;
      }
    }
  }


  std::list<InDet::Barcode>::iterator p = event_data.m_particles[0].begin(), pe =event_data.m_particles[0].end();

  for(; p!=pe; ++p) {
    for(SG::ReadHandleKeyArray<TrackCollection>::size_type nc=1; nc<m_tracklocation.size(); ++nc) event_data.m_particles[nc].push_back((*p));
  }
  return t;
}

///////////////////////////////////////////////////////////////////
// Recontructed track comparison with kine information
///////////////////////////////////////////////////////////////////

void InDet::TrackClusterAssValidation::tracksComparison(const EventContext& ctx, InDet::TrackClusterAssValidation::EventData_t &event_data) const
{
  if(!event_data.m_nqtracks) return;


  int nc = -1;
  event_data.m_trackcontainer.reserve(m_tracklocation.size());
  for(const SG::ReadHandleKey<TrackCollection> &track_key : m_tracklocation ) {
    if(++nc >= 100) return;
    event_data.m_tracks[nc].clear();

    event_data.m_trackcontainer.push_back( SG::ReadHandle<TrackCollection>(track_key,ctx) );
    if (!event_data.m_trackcontainer.back().isValid()) {
      continue;
    }

    // Loop through all found tracks
    //
    TrackCollection::const_iterator t,te = event_data.m_trackcontainer.back()->end();

    int KINE[200],NKINE[200];

    for (t=event_data.m_trackcontainer.back()->begin(); t!=te; ++t) {

      DataVector<const Trk::TrackStateOnSurface>::const_iterator
	s  = (*t)->trackStateOnSurfaces()->begin(),
	se = (*t)->trackStateOnSurfaces()->end  ();

      int  NK  = 0;
      int  NC  = 0;
      int  N0  = 0;
      int  nkm = 0;
      bool qp  = false;

      const Trk::TrackParameters* tpf = (*s)->trackParameters();  if(!tpf) continue;
      const AmgVector(5)&         Vpf = tpf ->parameters     ();
      double                      pTf = fabs(sin(Vpf[3])/Vpf[4]);
      bool                        qTf = pTf > m_ptcut;
      for(; s!=se; ++s) {

	if(!qp) {

	  const Trk::TrackParameters* tp = (*s)->trackParameters();

	  if(tp) {
	    qp = true;
	    const AmgVector(5)& Vp = tp->parameters();
	    double pT  = sin(Vp[3])/Vp[4]  ;
	    double rap = fabs(log(tan(.5*Vp[3])));
	    if     (pT >  m_ptcut && pT <  m_ptcutmax) {
	      if     (rap <      1. ) ++event_data.m_trackCollectionStat[nc].m_ntracksPOSB;
	      else if(rap < 3.0) ++event_data.m_trackCollectionStat[nc].m_ntracksPOSE;
	      else if(rap < m_rapcut) ++event_data.m_trackCollectionStat[nc].m_ntracksPOSDBM;
	    }
	    else if(pT < -m_ptcut && pT > -m_ptcutmax) {
	      if     (rap <      1. ) ++event_data.m_trackCollectionStat[nc].m_ntracksNEGB;
              else if(rap < 3.0) ++event_data.m_trackCollectionStat[nc].m_ntracksNEGE;
	      else if(rap < m_rapcut) ++event_data.m_trackCollectionStat[nc].m_ntracksNEGDBM;
	    }
	  }
	}

	if(!m_useOutliers && !(*s)->type(Trk::TrackStateOnSurface::Measurement)) continue;

	const Trk::MeasurementBase* mb = (*s)->measurementOnTrack();
	if(!mb) continue;

	const Trk::RIO_OnTrack*     ri = dynamic_cast<const Trk::RIO_OnTrack*>(mb);
	if(!ri) continue;

	const Trk::PrepRawData*     rd = ri->prepRawData();
	if(!rd) continue;

	const InDet::SiCluster*     si = dynamic_cast<const InDet::SiCluster*>(rd);
	if(!si) continue;

	if(!m_usePIX && dynamic_cast<const InDet::PixelCluster*>(si)) continue;
	if(!m_useSCT && dynamic_cast<const InDet::SCT_Cluster*> (si)) continue;


	int Kine[1000], nk=kine0(event_data,rd,Kine,999); ++NC; if(!nk) ++N0;

	for(int k = 0; k!=nk; ++k) {

	  int n = 0;
	  for(; n!=NK; ++n) {if(Kine[k]==KINE[n]) {++NKINE[n]; break;}}
	  if(n==NK) {KINE[NK] = Kine[k]; NKINE[NK] = 1; if (NK < 200) ++NK;}
	}
	for(int n=0; n!=NK; ++n) {if(NKINE[n]>nkm) nkm = NKINE[n];}
      }

      for(int n=0; n!=NK; ++n) {
	if(NKINE[n]==nkm) {
	  int NQ = 1000*NKINE[n]+(NC-NKINE[n]);

	  event_data.m_tracks[nc].insert(std::make_pair(KINE[n],NQ));
	  if(qTf) {
	    if(NC-N0 > 2) {
	      ++event_data.m_trackCollectionStat[nc].m_total[NC]; if(NC-NKINE[n] > 2) {++event_data.m_trackCollectionStat[nc].m_fake[NC];}
	    }
	  }
	}
      }
    }

  }
}

///////////////////////////////////////////////////////////////////
// Particles and reconstructed tracks comparision
///////////////////////////////////////////////////////////////////

void InDet::TrackClusterAssValidation::efficiencyReconstruction(InDet::TrackClusterAssValidation::EventData_t &event_data) const
{
  for(SG::ReadHandleKeyArray<TrackCollection>::size_type nc = 0; nc!=m_tracklocation.size(); ++nc) {

    event_data.m_difference[nc].clear();
    std::list<InDet::Barcode>::const_iterator p = event_data.m_particles[nc].begin(), pe =event_data.m_particles[nc].end();
    if(p==pe) return;
    std::multimap<int,int>::const_iterator t, te = event_data.m_tracks[nc].end();

    while (p!=pe) {

      int k = (*p).barcode();
      int n = event_data.m_kinecluster.count(k);
      int m = 0;
      int w = 0;
      t = event_data.m_tracks[nc].find(k);
      for(; t!=te; ++t) {
	if((*t).first!=k) break;
	int ts = (*t).second/1000;
	int ws = (*t).second%1000;
	if     (ts > m         ) {m = ts; w = ws;}
	else if(ts==m && w > ws) {        w = ws;}
      }
      int d = n-m; if(d<0) d = 0; else if(d > 5) d=5; if(w>4) w = 4;
      if(m) {
	++event_data.m_trackCollectionStat[nc].m_efficiency [d];
	++event_data.m_trackCollectionStat[nc].m_efficiencyN[d][w];
      }
      int ch = (*p).charge();
      if(m) {
	++event_data.m_trackCollectionStat[nc].m_efficiencyBTE[d][w][(*p).rapidity()];
	ch > 0 ? ++event_data.m_trackCollectionStat[nc].m_efficiencyPOS[d] : ++event_data.m_trackCollectionStat[nc].m_efficiencyNEG[d];
      }
      if(nc==0) {
	++event_data.m_eventStat.m_events; ch > 0 ? ++event_data.m_eventStat.m_eventsPOS : ++event_data.m_eventStat.m_eventsNEG;
	++event_data.m_eventStat.m_eventsBTE[(*p).rapidity()];
      }
      if(d==0) event_data.m_particles[nc].erase(p++);
      else {event_data.m_difference[nc].push_back(n-m);  ++p;}
    }
  }
}

///////////////////////////////////////////////////////////////////
// Pointer to particle production for space point
///////////////////////////////////////////////////////////////////

int InDet::TrackClusterAssValidation::kine
(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData* d1,const Trk::PrepRawData* d2,int* Kine,int nmax) const
{
  int nkine = 0;
  int Kine1[1000],Kine2[1000];
  int n1 = kine(event_data,d1,Kine1,nmax); if(!n1) return nkine;
  int n2 = kine(event_data,d2,Kine2,nmax); if(!n2) return nkine;

  for(int i = 0; i!=n1; ++i) {
    for(int j = 0; j!=n2; ++j) {
      if(Kine1[i]==Kine2[j]) {Kine[nkine++] = Kine1[i];  break;}
    }
  }
  return nkine;
}

///////////////////////////////////////////////////////////////////
// Pointer to particle production for cluster
///////////////////////////////////////////////////////////////////

int InDet::TrackClusterAssValidation::kine
(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData* d,int* Kine,int nmax) const
{

  PRD_MultiTruthCollection::const_iterator mce;
  PRD_MultiTruthCollection::const_iterator mc = findTruth(event_data,d,mce);

  Identifier ID    = d->identify();
  int        nkine = 0;

  for(; mc!=mce; ++mc) {

    if( (*mc).first != ID ) return nkine;

    int k = (*mc).second.barcode(); if(k<=0) continue;

    const HepMC::GenParticle* pa = (*mc).second.cptr();
    if(!pa || !pa->production_vertex()) continue;

    int pdg = abs(pa->pdg_id()); if(m_pdg && m_pdg != pdg ) continue;

    const HepPDT::ParticleData* pd  = m_particleDataTable->particle(pdg);
    if(!pd ||  fabs(pd->charge()) < .5) continue;

    // pT cut
    //
    double           px = pa->momentum().px();
    double           py = pa->momentum().py();
    double           pz = pa->momentum().pz();
    double           pt = sqrt(px*px+py*py);
    if( pt < m_ptcut || pt > m_ptcutmax) continue;

    // Rapidity cut
    //
    double           t  = fabs(pz)/pt;
    if( t  > m_tcut ) continue;

    // Radius cut
    //
    double           vx = pa->production_vertex()->point3d().x();
    double           vy = pa->production_vertex()->point3d().y();
    double           r = sqrt(vx*vx+vy*vy);
    if( r < m_rmin || r > m_rmax) continue;

    Kine[nkine] = k; if(++nkine >= nmax) break;
  }
  return nkine;
}

///////////////////////////////////////////////////////////////////
// Pointer to particle production for cluster
///////////////////////////////////////////////////////////////////

int InDet::TrackClusterAssValidation::kine0
(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData* d,int* Kine,int nmax) const
{

  PRD_MultiTruthCollection::const_iterator mce;
  PRD_MultiTruthCollection::const_iterator mc = findTruth(event_data, d,mce);

  Identifier ID    = d->identify();
  int        nkine = 0;

  for(; mc!=mce; ++mc) {

    if( (*mc).first != ID ) return nkine;

    int k = (*mc).second.barcode(); if(k<=0) continue;
    Kine[nkine] = k; if(++nkine >= nmax) break;
  }
  return nkine;
}

///////////////////////////////////////////////////////////////////
// Test detector element
///////////////////////////////////////////////////////////////////

bool InDet::TrackClusterAssValidation::isTheSameDetElement
(const InDet::TrackClusterAssValidation::EventData_t &event_data, int K,const Trk::PrepRawData* d) const
{
  std::multimap<int,const Trk::PrepRawData*>::const_iterator k = event_data.m_kinecluster.find(K);
  for(; k!=event_data.m_kinecluster.end(); ++k) {

    if((*k).first!= K) return false;
    if(d->detectorElement()==(*k).second->detectorElement()) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test detector element
///////////////////////////////////////////////////////////////////

bool InDet::TrackClusterAssValidation::isTheSameDetElement
(const InDet::TrackClusterAssValidation::EventData_t &event_data, int K,const Trk::SpacePoint* sp) const
{
  const Trk::PrepRawData*  p1 = sp->clusterList().first;
  const Trk::PrepRawData*  p2 = sp->clusterList().second;

  std::multimap<int,const Trk::SpacePoint*>::const_iterator  k = event_data.m_kinespacepoint.find(K);

  if(!p2) {

    for(; k!=event_data.m_kinespacepoint.end(); ++k) {
      if((*k).first!= K) return false;

      const Trk::PrepRawData*  n1 = (*k).second->clusterList().first ;
      const Trk::PrepRawData*  n2 = (*k).second->clusterList().second;

      if(p1->detectorElement() == n1->detectorElement()) return true;
      if(!n2) continue;
      if(p1->detectorElement() == n2->detectorElement()) return true;
    }
    return false;
  }

  for(; k!=event_data.m_kinespacepoint.end(); ++k) {
    if((*k).first!= K) return false;

    const Trk::PrepRawData*  n1 = (*k).second->clusterList().first ;
    const Trk::PrepRawData*  n2 = (*k).second->clusterList().second;

    if(p1->detectorElement() == n1->detectorElement()) return true;
    if(p2->detectorElement() == n1->detectorElement()) return true;
    if(!n2) continue;
    if(p1->detectorElement() == n2->detectorElement()) return true;
    if(p2->detectorElement() == n2->detectorElement()) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Dump information about no recontructed particles
//////////////recon/////////////////////////////////////////////////////

bool InDet::TrackClusterAssValidation::noReconstructedParticles(const InDet::TrackClusterAssValidation::EventData_t &event_data) const
{

  for(SG::ReadHandleKeyArray<TrackCollection>::size_type nc=0; nc!=m_tracklocation.size(); ++nc) {

    std::list<InDet::Barcode>::const_iterator p = event_data.m_particles[nc].begin(), pe =event_data.m_particles[nc].end();
    if(p==pe) continue;

    std::list<int>::const_iterator dif = event_data.m_difference[nc].begin();

    std::multimap<int,const Trk::PrepRawData*>::const_iterator c,ce = event_data.m_kinecluster.end();

    int n  = 69-m_tracklocation[nc].key().size();
    std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

    std::cout<<"|----------------------------------------------------------------------------------------|"<<std::endl;
    std::cout<<"|                   "<<m_tracklocation[nc]<<s1
	     <<std::endl;

    std::cout<<"|----------------------------------------------------------------------------------------|"<<std::endl;
    std::cout<<"|    #   pdg     kine   Ncl Ntr Nsp Lose     pT(MeV)    rapidity    radius          z    |"<<std::endl;
    std::cout<<"|----------------------------------------------------------------------------------------|"<<std::endl;
    n = 0;
    for(; p!=pe; ++p) {

      int k = (*p).barcode();

      c = event_data.m_kinecluster.find(k); if(c==ce) continue;
      const Trk::PrepRawData* d = (*c).second;

      PRD_MultiTruthCollection::const_iterator mce;
      PRD_MultiTruthCollection::const_iterator mc = findTruth(event_data,d,mce);

      Identifier ID    = d->identify();
      bool Q = false;
      for(; mc!=mce; ++mc) {
	if((*mc).first != ID) break;
	if((*mc).second.cptr()->barcode()==k) {Q=true; break;}
      }

      if(!Q) continue;

      const HepMC::GenParticle* pa = (*mc).second.cptr();

      double           px =  pa->momentum().px();
      double           py =  pa->momentum().py();
      double           pz =  pa->momentum().pz();
      double           vx = pa->production_vertex()->point3d().x();
      double           vy = pa->production_vertex()->point3d().y();
      double           vz = pa->production_vertex()->point3d().z();
      double           pt = sqrt(px*px+py*py);
      double           t  = atan2(pt,pz);
      double           ra =-log(tan(.5*t));
      double           r  = sqrt(vx*vx+vy*vy);
      ++n;
      std::cout<<"| "
	     <<std::setw(4)<<n
	       <<std::setw(6)<<pa->pdg_id()
	       <<std::setw(10)<<pa->barcode()
	       <<std::setw(4)<<event_data.m_kinecluster   .count(k)
	       <<std::setw(4)<<event_data.m_kineclusterTRT.count(k)
	       <<std::setw(4)<<event_data.m_kinespacepoint.count(k)
	       <<std::setw(4)<<(*dif)
	       <<std::setw(12)<<std::setprecision(5)<<pt
	       <<std::setw(12)<<std::setprecision(5)<<ra
	       <<std::setw(12)<<std::setprecision(5)<<r
	       <<std::setw(12)<<std::setprecision(5)<<vz
	       <<"   |"
	       <<std::endl;
      ++dif;

    }
    std::cout<<"|----------------------------------------------------------------------------------------|"<<std::endl;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Cluster truth information
//////////////recon/////////////////////////////////////////////////////

PRD_MultiTruthCollection::const_iterator
InDet::TrackClusterAssValidation::findTruth
(const InDet::TrackClusterAssValidation::EventData_t &event_data,
 const Trk::PrepRawData* d,
 PRD_MultiTruthCollection::const_iterator& mce) const
{
  const InDet::SCT_Cluster    * si = dynamic_cast<const InDet::SCT_Cluster*>    (d);
  const InDet::PixelCluster   * px = dynamic_cast<const InDet::PixelCluster*>   (d);
  const InDet::TRT_DriftCircle* tr = dynamic_cast<const InDet::TRT_DriftCircle*>(d);

  PRD_MultiTruthCollection::const_iterator mc;

  if     (px && event_data.m_truthPIX) {mc=event_data.m_truthPIX->find(d->identify()); mce=event_data.m_truthPIX->end();}
  else if(si && event_data.m_truthSCT) {mc=event_data.m_truthSCT->find(d->identify()); mce=event_data.m_truthSCT->end();}
  else if(tr && event_data.m_truthTRT) {mc=event_data.m_truthTRT->find(d->identify()); mce=event_data.m_truthTRT->end();}
  else {
    const PRD_MultiTruthCollection *truth[] {event_data. m_truthPIX,event_data.m_truthSCT, event_data.m_truthTRT};
    for (int i=0; i<3; i++) {
        if (truth[i]) {
          mce=truth[i]->end();
          return truth[i]->end();
        }
    }
    throw std::runtime_error("Neither Pixel, SCT nor TRT truth.");
  }
  return mc;
}

///////////////////////////////////////////////////////////////////
// Cluster truth information
//////////////recon/////////////////////////////////////////////////////

int InDet::TrackClusterAssValidation::charge(const InDet::TrackClusterAssValidation::EventData_t &event_data,std::pair<int,const Trk::PrepRawData*> pa,int& rap) const
{
  int                     k = pa.first;
  const Trk::PrepRawData* d = pa.second;
  PRD_MultiTruthCollection::const_iterator mce;
  PRD_MultiTruthCollection::const_iterator mc = findTruth(event_data,d,mce);

  for(; mc!=mce; ++mc) {
    if((*mc).second.cptr()->barcode()==k) {

      const HepMC::GenParticle*   pat  = (*mc).second.cptr();

      rap       = 0;
      double px =  pat->momentum().px();
      double py =  pat->momentum().py();
      double pz =  pat->momentum().pz();
      double pt = sqrt(px*px+py*py)   ;
      double t  = atan2(pt,pz)        ;
      double ra = fabs(log(tan(.5*t)));
      // DBM
      if (ra > 3.0)
	rap = 3;
      else
      // other regions
	ra > 1.6 ? rap = 2 : ra > .8 ?  rap = 1 : rap = 0;

      int                         pdg = pat->pdg_id();
      const HepPDT::ParticleData* pd  = m_particleDataTable->particle(abs(pdg));
      if(!pd) return 0;
      double ch = pd->charge(); if(pdg < 0) ch = -ch;
      if(ch >  .5) return  1;
      if(ch < -.5) return -1;
      return 0;
    }
  }
  return 0;
}
