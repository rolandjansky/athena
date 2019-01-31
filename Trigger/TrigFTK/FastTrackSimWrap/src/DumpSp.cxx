/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// FASTTRACKSIMWRAP
// ================================================================
// This algorithm is used to extract all needed information from
// Athena for use in the standalone FTK simulation. Eventually an
// Athena-based FTK simulation will exist and this code will
// disappear.
// ================================================================
//
// 2011-03-22 Antonio Boveia (boveia@hep.uchicago.edu)
//            Ability to write text files directly to bzip2 using
//            boost::iostreams.
// 2010-03-25 Antonio Boveia
//            Add SCT cluster data to wrapper output.
// 2010-03-10 Joe Tuggle (jtuggle@hep.uchicago.edu)
//            Add pixel cluster data
// 2009-05-20 Antonio Boveia
//            access SDOs and dump GEANT truth for each silicon
//            channel. perform matching of offline reconstructed
//            tracks to truth using this info. substantial rewrite 
//            and cleanup.
// 2007-12-10 Giulio Usai <giulio.usai@cern.ch>
//            Monica Dunford <mdunford@hep.uchicago.edu>
//            original version.

// include before any HAVE_VERISON_15 checks below.
#include "FastTrackSimWrap/DumpSp.h"


#include <cmath>
#include <cstdlib>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <limits>
#include <utility>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/algorithm/string/predicate.hpp>
 
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "IdDict/IdDictDefs.h" 
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TileIdentifier/TileTBID.h"
#include "VxVertex/VxContainer.h"
//#include "TrkTruthToTrack/TruthToTrack.h" 
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GeneratorObjects/McEventCollection.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"


#include "EventPrimitives/EventPrimitivesHelpers.h"

using namespace std;

DumpSp::DumpSp(const string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm( name , pSvcLocator )
  , m_idHelper()
  , m_storeGate( 0 )
  , m_detStore( 0 )
  , m_evtStore( 0 )
  , m_pixelId( 0 )
  , m_sctId( 0 )
  , m_PIX_mgr( 0 )
  , m_pixelContainer( 0 )
  , m_sctContainer( 0 )
  , m_particleDataTable( 0 )
  , m_truthToTrack( "Trk::TruthToTrack/InDetTruthToTrack" )
  , m_trkSummaryTool( "Trk::TrackSummaryTool/InDetTrackSummaryTool" )
  , m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
  , m_pixelCondSummarySvc("PixelConditionsSummarySvc",name)
  // , m_holeSearchTool( "InDetHoleSearchTool" )
  , m_pixelClustersName( "PixelClusters" )
  , m_sctClustersName( "SCT_Clusters" )
  , m_pixelSpacePointsName( "PixelSpacePoints" )
  , m_sctSpacePointsName( "SCT_SpacePoints" )
  , m_overlapSpacePointsName( "OverlapSpacePoints" )
  , m_tracksName( "ExtendedTracks" )
  , m_tracksTruthName( "ExtendedTracksTruthCollection" )
  , m_maxEta( 3.3 )
  , m_minPt( 0. )
  , m_spacePointsName( "" )
  , m_outFileName( "ftksim_hits.dat.bz2" )
  , m_outFileNameRawHits( "ftksim_raw_hits.dat.bz2" )
  , m_doTrigger (false)
  , m_doData (false)
  , m_doVertex (false)
  , m_doLumiBCID (false)
  , m_doBadMod( true )
  , m_dumpHitsOnTracks( false )
  , m_dumpSpacePoints( false )
  , m_dumpTruthIntersections( false )
  , m_dumpMBTSInfo( false ) 
  , m_useOfflineTrackSelectorTool( false )
  , m_outputBeamSpotToWrapper( false )
  , m_useSimpleCuts( true )
  , m_doRDODebug( false )
  , m_ofl()
  , m_oflraw()
{   
  declareProperty("maxEta",                   m_maxEta);
  declareProperty("minPt",                    m_minPt);
  declareProperty("pixelClustersName",        m_pixelClustersName);
  declareProperty("SCT_ClustersName",         m_sctClustersName);
  declareProperty("overlapSpacePointsName",   m_overlapSpacePointsName);
  declareProperty("pixelSpacePointsName",     m_pixelSpacePointsName);
  declareProperty("sctSpacePointsName",       m_sctSpacePointsName);
  declareProperty("OutFileName",              m_outFileName);
  declareProperty("OutFileNameRawHits",       m_outFileNameRawHits);
  declareProperty("dumpHitsOnTracks",         m_dumpHitsOnTracks);
  declareProperty("dumpSpacePoints",          m_dumpSpacePoints);
  declareProperty("dumpTruthIntersections",   m_dumpTruthIntersections);
  declareProperty("SummaryTool",              m_trkSummaryTool);
  declareProperty("tracksName" ,              m_tracksName);
  declareProperty("tracksTruthName"  ,        m_tracksTruthName);  
  declareProperty("TruthToTrackTool" ,        m_truthToTrack);
  // declareProperty("HoleSearch" ,             m_holeSearchTool);
  declareProperty("dumpMBTSInfo" ,            m_dumpMBTSInfo , "Dump X fields to output containing MBTS trigger info" );
  declareProperty("useOfflineTrackSelectorTool" , m_useOfflineTrackSelectorTool);
  declareProperty("outputBeamSpotToWrapper" , m_outputBeamSpotToWrapper);
  declareProperty("useSimpleCuts" ,           m_useSimpleCuts);
  //#ifdef HAVE_VERSION_15
  declareProperty("TrigDecisionTool",         m_trigDecTool );
  declareProperty("PixelSummarySvc" ,         m_pixelCondSummarySvc);
  declareProperty("DoTrigger" ,           m_doTrigger);
  declareProperty("DoData" ,                m_doData);
  declareProperty("DoVertex" ,                m_doVertex);
  declareProperty("DoLumiBCID" ,                m_doLumiBCID);
  declareProperty("DoBadMod" ,           m_doBadMod);
  declareProperty("DoRDODebug" ,           m_doRDODebug);
  //#endif
}

StatusCode
DumpSp::initialize()
{

  ATH_MSG_INFO("DumpSp::initialize()");
  
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    ATH_MSG_FATAL("StoreGate service not found");
    return StatusCode::FAILURE;
  }

  if( m_truthToTrack.retrieve().isFailure() ) {
    ATH_MSG_FATAL(m_truthToTrack << " truth to track tool not found");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO(m_truthToTrack << " retrieved");
  }

  // if ( m_holeSearchTool.retrieve().isFailure() ) {
  //   ATH_MSG_FATAL("Failed to retrieve tool " << m_holeSearchTool);
  //   return StatusCode::FAILURE;
  // }

  if( service("DetectorStore",m_detStore).isFailure() ) {
    ATH_MSG_FATAL("DetectorStore service not found");
    return StatusCode::FAILURE;
  }

  IPartPropSvc* partPropSvc = 0;
  if( service("PartPropSvc", partPropSvc, true).isFailure() ) {
    ATH_MSG_FATAL("particle properties service unavailable");
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  // ID helpers
  m_idHelper.reset( new AtlasDetectorID );
  const IdDictManager* idDictMgr( 0 );
  if( m_detStore->retrieve(idDictMgr, "IdDict").isFailure() || !idDictMgr ) {
    ATH_MSG_ERROR( "Could not get IdDictManager !");
    return StatusCode::FAILURE;
  }
  if( idDictMgr->initializeHelper( *m_idHelper.get() ) ) { // Returns 1 if there is a problem
    ATH_MSG_ERROR( "Unable to initialize ID helper.");
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve Pixel manager from DetectorStore");
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve Pixel helper from DetectorStore");
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve SCT helper from DetectorStore");
    return StatusCode::FAILURE;
  }

  // ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  
  if( m_trkSummaryTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSummaryTool);
    return StatusCode::FAILURE;
  }

  if(m_doTrigger || m_dumpMBTSInfo ){
    if( m_trigDecTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "Unable to retrieve TrigDecsionTool");
    } else {
      ATH_MSG_INFO("Successfully retrieved TrigDecisionTool");
    }
  }
  if( m_doBadMod) {
    // Get PixelConditionsSummarySvc
    if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_pixelCondSummarySvc);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_pixelCondSummarySvc);
    }
    // Get SctConditionsSummaryTool
    if ( m_sctCondSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_sctCondSummaryTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_sctCondSummaryTool);
    }
  } else {
    m_sctCondSummaryTool.disable();
  }


  // open output streams
  if( m_dumpSpacePoints ) { 
    m_ofl.reset( new boost::iostreams::filtering_ostream );
    if( !m_ofl ) { return StatusCode::FAILURE; }
    if( boost::algorithm::icontains( m_outFileName , ".bz2" ) ) {
      boost::iostreams::bzip2_params params;
      params.block_size = 9;
      m_ofl->push( boost::iostreams::bzip2_compressor(params) ); 
     }
    m_ofl->push( boost::iostreams::file_sink(m_outFileName) ); // open the file
  }
  if( true ) { 
    m_oflraw.reset( new boost::iostreams::filtering_ostream );
    if( !m_oflraw ) { return StatusCode::FAILURE; }
    if( boost::algorithm::icontains( m_outFileNameRawHits , ".bz2" ) ) {
      boost::iostreams::bzip2_params params;
      params.block_size = 9;
      m_oflraw->push( boost::iostreams::bzip2_compressor(params) ); 
     }
    m_oflraw->push( boost::iostreams::file_sink(m_outFileNameRawHits) ); // open the file
  }

  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
  ATH_CHECK(m_sctLorentzAngleTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode
DumpSp::execute()
{
  const EventInfo* eventInfo(0);
  if( m_storeGate->retrieve(eventInfo).isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve event info");
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );

  ATH_MSG_DEBUG( "entered execution for run " << eventID->run_number()
        << "   event " << eventID->event_number());

  if(m_doTrigger || m_doData) {
    //make trigger selection
    const bool ok_passRandom = m_trigDecTool->isPassed("EF_rd0_filled_NoAlg");
      if(!ok_passRandom){
	ATH_MSG_DEBUG( " Event failed trigger requirement ");
	return StatusCode::SUCCESS;
      }else 	ATH_MSG_DEBUG( " Event passed trigger requirement! ");

  }
  

 
  // event header
  if( m_dumpSpacePoints ) {
    (*m_ofl) << "R\t" << eventID->run_number()<<'\n';                 
    (*m_ofl) << "F\t" << eventID->event_number()<<'\t'<<eventInfo->averageInteractionsPerCrossing()<<'\t'<<eventInfo->actualInteractionsPerCrossing()<< '\n';
  }
  (*m_oflraw) << "R\t" << eventID->run_number()<<'\n';                    
  (*m_oflraw) << "F\t" << eventID->event_number()<<'\t'<<eventInfo->averageInteractionsPerCrossing()<<'\t'<<eventInfo->actualInteractionsPerCrossing()<<'\n';
  {
    // dump bad modules once at the start of each lumi block.
    static unsigned int last_lumi_block = static_cast<unsigned int>(~0);
    if( last_lumi_block==static_cast<unsigned int>(~0) || eventID->lumi_block()!=last_lumi_block ) {
      dump_bad_modules();
      last_lumi_block = eventID->lumi_block();
    }
  }

  // build truth-track geant matching

  if( !m_doData)
    build_matching_maps();

  HitIndexMap hitIndexMap; // keep running index event-unique to each hit
  HitIndexMap clusterIndexMap;
  // get pixel and sct cluster containers
  if( m_storeGate->retrieve(m_pixelContainer, m_pixelClustersName).isFailure() ) {
    ATH_MSG_WARNING( "unable to retrieve the PixelCluster container " << m_pixelClustersName);
  }
  if( m_storeGate->retrieve(m_sctContainer, m_sctClustersName).isFailure() ) {
    ATH_MSG_WARNING( "unable to retrieve the SCT_Cluster container " << m_sctClustersName);
  }

  if( m_dumpSpacePoints ) { 
    dump_spacepoints();
  }
  
  // dump raw silicon data
  dump_raw_silicon( hitIndexMap , clusterIndexMap );

  // dump ipat tracks using new format and interpretation code
  dump_tracks( hitIndexMap , clusterIndexMap );

  // dump truth tracks
  if( !m_doData)
    dump_truth();

  // dump beamspot to wrapper output
  dump_beamspot();

  
  // dump data for occupancy studies
  if( m_dumpMBTSInfo ) { dump_MBTS(  ); }
  // dump vertex data for occupancy studies
  if( m_doLumiBCID || m_doData) { dump_lumibcid( eventID ); }
  // dump vertex data for occupancy studies
  if( m_doVertex || m_doData) { dump_vertex( ); }
  

  // event footer
  if( m_dumpSpacePoints ) { (*m_ofl) << "L\t" << eventID->event_number()<<'\n'; }
  (*m_oflraw) << "L\t" << eventID->event_number()<<'\n';
  
  return StatusCode::SUCCESS;
}


StatusCode
DumpSp::finalize()
{

  ATH_MSG_INFO("finalized");
  
  return StatusCode::SUCCESS;
}

// build ttr and rtt truth matching maps for this event

void
DumpSp::build_matching_maps()
{
  m_ttrTrackMap.clear();
  m_ttrProbMap.clear();
  m_rttTrackMap.clear();

  ATH_MSG_DEBUG( "building reconstruction-truth matching maps");

  // retrieve necessary junk from Athena
  const TrackCollection* RecCollection = 0;
  if( m_storeGate->retrieve( RecCollection, m_tracksName ).isFailure() ) { 
    ATH_MSG_WARNING( "could not find TrackCollection " << m_tracksName);
    RecCollection = 0; 
  }

  const DataVector<Trk::Track>* trks = 0;
  if( m_storeGate->retrieve(trks,m_tracksName).isFailure() ) { 
    ATH_MSG_WARNING( "could not find Trk::Track collection " << m_tracksName);
    trks = 0;
  }

  const McEventCollection* SimTracks = 0;
  if( m_storeGate->retrieve(SimTracks,"TruthEvent").isFailure() ) {
    string key = "G4Truth";
    if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
      key = "";
      if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
        ATH_MSG_WARNING( "could not find the McEventCollection");
        return;
      }
    }
  }

  if( !m_particleDataTable ) { 
    ATH_MSG_WARNING( " could not find the particle properties service");
  }

  const TrackTruthCollection* TruthMap  = 0;
  if( m_storeGate->retrieve(TruthMap,m_tracksTruthName).isFailure() ) { 
    ATH_MSG_WARNING( "could not find truth map " << m_tracksTruthName);
    TruthMap = 0; 
  }

  if( !RecCollection || !trks || !TruthMap ) { 
    ATH_MSG_WARNING( "missing necessary info for truth matching");
    return;
  }

  // construct reconstruction to truth map.
  for( DataVector<Trk::Track>::const_iterator trksItr=trks->begin(), trksItrF=trks->end(); trksItr!=trksItrF; ++trksItr ) {
    // retrieve data
    ElementLink<TrackCollection> tracklink;
    tracklink.setElement( const_cast<Trk::Track*>(*trksItr) );
    tracklink.setStorableObject( *RecCollection );
    const ElementLink<TrackCollection> tracklink2( tracklink );
    TrackTruthCollection::const_iterator found( TruthMap->find(tracklink2) );
    if( found==TruthMap->end() ) { continue; }
    TrackTruth trtruth( found->second );
    HepMcParticleLink::ExtendedBarCode extBarcode(trtruth.particleLink().barcode(),trtruth.particleLink().eventIndex());
    // update m_ttrTrackMap with track index corresponding to the greatest figure of merit.
    if( m_ttrProbMap.find(extBarcode)==m_ttrProbMap.end() ) {
      // this is the only track matching this barcode so far
      m_ttrTrackMap.insert( pair<HepMcParticleLink::ExtendedBarCode,unsigned int>(extBarcode,distance(trks->begin(),trksItr)) );
    } else {
      // a reconstructed track match already exists. figure if this match is better, and keep it if so.
      //
      // retrieve probabilities for this barcode to match to each track
      static vector<float> probs;
      probs.clear();
      transform( m_ttrProbMap.lower_bound(extBarcode) , m_ttrProbMap.upper_bound(extBarcode) , back_inserter(probs) , 
                 boost::bind(&TruthToRecoProbMap::value_type::second,_1) );
      // determine track with highest figure-of-merit match and update
      // the map of barcodes => reconstructed track indices (into
      // track collection)
      vector<float>::const_iterator imax = max_element(probs.begin(),probs.end());
      if( imax!=probs.end() && (trtruth.probability() > (*imax)) ) {
        m_ttrTrackMap[extBarcode] = distance(trks->begin(),trksItr);
      }
    }
    // update map from barcodes to highest reconstructed track matching probabilities
    m_ttrProbMap.insert( pair<HepMcParticleLink::ExtendedBarCode,float>(extBarcode,trtruth.probability()) );
    // fill reco to truth map. allow multiple reconstructed tracks to point to the same barcode.
    m_rttTrackMap[ distance(trks->begin(),trksItr) ] = extBarcode;
  } // end for each reconstructed track

  ATH_MSG_DEBUG( "truth info from " << trks->size() << " tracks.");
  ATH_MSG_DEBUG( "_rttTrackMap constructed with " << m_rttTrackMap.size() << " entries.");
  ATH_MSG_DEBUG( "_ttrTrackMap constructed with " << m_ttrTrackMap.size() << " entries.");
  ATH_MSG_DEBUG( "_ttrProbMap constructed with " << m_ttrProbMap.size() << " entries.");

}

// dump truth, including geant matching data for offline reconstructed track collection

void
DumpSp::dump_truth() const
{
  ATH_MSG_DEBUG( "dumping truth information");

  // retrieve truth tracks from athena
  const McEventCollection* SimTracks = 0;
  if( m_storeGate->retrieve(SimTracks,"TruthEvent").isFailure() ) {
    string key = "G4Truth";
    if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
      key = "";
      if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
        ATH_MSG_WARNING( "could not find the McEventCollection");
        return;
      }
    }
  }

  // is PDG info is available?
  if( !m_particleDataTable ) { 
    ATH_MSG_ERROR( " could not find the particle properties service");
    return;
  }

  // for debugging the calculation of d0_corr
 
  bool showd0corrSuccess = true;

  // dump each truth track

  for( unsigned int ievt=0, fevt=(SimTracks ? SimTracks->size() : 0u); ievt!=fevt; ++ievt ) {    
    const HepMC::GenEvent* genEvent = SimTracks->at( ievt );
    // retrieve the primary interaction vertex here. for now, use the dummy origin.
    Amg::Vector3D  primaryVtx(0.,0.,0.);
    // the event should have signal process vertex unless it was generated as single particles.
    // if it exists, use it for the primary vertex.
    if( genEvent->signal_process_vertex() ) {
      primaryVtx << genEvent->signal_process_vertex()->point3d().x() , genEvent->signal_process_vertex()->point3d().y() , genEvent->signal_process_vertex()->point3d().z();
      ATH_MSG_INFO("using signal process vertex for eventIndex " << ievt << ":" 
          << primaryVtx.x() << "\t" << primaryVtx.y()  << "\t" << primaryVtx.z()
          );
    }
    for( HepMC::GenEvent::particle_const_iterator it=genEvent->particles_begin(), ft=genEvent->particles_end(); it!=ft; ++it ) {
      const HepMC::GenParticle* const particle( *it );
      const int pdgcode = particle->pdg_id();
      // reject generated particles without a production vertex.
      if( !particle->production_vertex() ) { continue; }
      // reject neutral or unstable particles
      const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));
      if( !pd ) { continue; }
      const float charge = pd->charge();
      if( std::abs(charge)<0.5 ) { continue; }
      if( particle->status()%1000!=1 ) { continue; }
      // retrieve some particle kinematics
      const float genPt = particle->momentum().perp()/1000.; // convert to pt in GeV.
      const float genEta = particle->momentum().pseudoRapidity();
      // reject non-fiducial particles
      if( (genPt*1000.) < m_minPt ) { continue; }
      if( std::abs(genEta) > m_maxEta ) { continue; }
      // retrieve truth track parameters at perigee
      boost::scoped_ptr<const Trk::TrackParameters> generatedTrackPerigee( m_truthToTrack->makePerigeeParameters(particle) );
      const float track_truth_d0 = generatedTrackPerigee ? generatedTrackPerigee->parameters()[Trk::d0] : 999.;
      const float track_truth_phi = generatedTrackPerigee ? generatedTrackPerigee->parameters()[Trk::phi0] : 999.;
      const float track_truth_p = (generatedTrackPerigee && generatedTrackPerigee->parameters()[Trk::qOverP] != 0.) ? 
        generatedTrackPerigee->charge()/generatedTrackPerigee->parameters()[Trk::qOverP] : 10E7;
      const float track_truth_x0 = generatedTrackPerigee ? generatedTrackPerigee->position().x() : 999.;
      const float track_truth_y0 = generatedTrackPerigee ? generatedTrackPerigee->position().y() : 999.;
      const float track_truth_z0 = generatedTrackPerigee ? generatedTrackPerigee->position().z() : 999.;
      const float track_truth_q = generatedTrackPerigee ? generatedTrackPerigee->charge() : 0.;
      const float track_truth_sinphi = generatedTrackPerigee ? std::sin(generatedTrackPerigee->parameters()[Trk::phi0]) : -1.;
      const float track_truth_cosphi = generatedTrackPerigee ? std::cos(generatedTrackPerigee->parameters()[Trk::phi0]) : -1.;
      const float track_truth_sintheta = generatedTrackPerigee ? std::sin(generatedTrackPerigee->parameters()[Trk::theta]) : -1.;
      const float track_truth_costheta = generatedTrackPerigee ? std::cos(generatedTrackPerigee->parameters()[Trk::theta]) : -1.;
      float truth_d0corr = track_truth_d0-( primaryVtx.y()*cos(track_truth_phi)-primaryVtx.x()*sin(track_truth_phi) );
      float truth_zvertex = 0.;
      if ( !m_useSimpleCuts ) {  // determine d0_corr based on beam position from BeamCondSvc
        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
        truth_d0corr = track_truth_d0-( beamSpotHandle->beamPos().y()*cos(track_truth_phi)-beamSpotHandle->beamPos().x()*sin(track_truth_phi) );
        truth_zvertex = beamSpotHandle->beamPos().z();
        if ( showd0corrSuccess ) {
          ATH_MSG_DEBUG( "Beamspot from BeamCondSvc used to determine cuts in dump_truth()");
          showd0corrSuccess = false;
        }
      }
      const Amg::Vector3D startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
      // categorize particle (prompt, secondary, etc.) based on InDetPerformanceRTT/detector paper criteria.
      bool isPrimary = true;
      bool isDetPaperCut = true;
      if( std::abs(truth_d0corr)>2. ) { isPrimary=false; }
      if( particle->barcode()>100000 || particle->barcode()==0 ) { isPrimary=false; }
      isDetPaperCut=isPrimary;
      // debug line for production_vertex variable
      if( false ) { ATH_MSG_DEBUG( "z correction -- bool particle->production_vertex() = " << particle->production_vertex()); }
      //
      if( isPrimary && particle->production_vertex() ) {
        const Amg::Vector3D startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
        if( std::abs(startVertex.z() - truth_zvertex)>100. ) { isPrimary=false; }
        // debug z vertex correction
        if( false ) { ATH_MSG_DEBUG( "z correction -- startVertex.z() = " << startVertex.z() << " , truth_zvertex = "<< truth_zvertex); }
        //double flight_length = -1.;
        if( particle->end_vertex() ) {
          Amg::Vector3D endVertex(particle->end_vertex()->point3d().x(), particle->end_vertex()->point3d().y(), particle->end_vertex()->point3d().z());
          if( endVertex.perp()<400. && std::abs(endVertex.z())<2300. ) { isPrimary=false; }
          //flight_length = startVertex.distance( endVertex );
        }
      } else {
        isPrimary = false; 
      }
      isDetPaperCut=isPrimary;
      // print truth track info and geant matching for highest figure-of-merit track
      int irecmatch = -1;
      float precmatch = 0.;
      HepMcParticleLink::ExtendedBarCode extBarcode2( particle->barcode(), ievt );
      if( !m_ttrProbMap.empty() ) {
        TruthToRecoProbMap::const_iterator barcode=m_ttrProbMap.find(extBarcode2);
        if( barcode!=m_ttrProbMap.end() ) {
          vector<float> probs;
          transform( m_ttrProbMap.lower_bound(extBarcode2) , m_ttrProbMap.upper_bound(extBarcode2) , back_inserter(probs) , 
                     boost::bind(&TruthToRecoProbMap::value_type::second,_1) );
          vector<float>::const_iterator imax = max_element(probs.begin(),probs.end());
          assert( imax!=probs.end() );
          precmatch = *imax;
          TruthToRecoTrackMap::const_iterator ibestrec = m_ttrTrackMap.find( extBarcode2 );
          assert( ibestrec!=m_ttrTrackMap.end() );
          irecmatch = ibestrec->second;
        }
      }
      ParentBitmask parent_mask( construct_truth_bitmap( particle ) );
      (*m_oflraw) << setiosflags(ios::scientific) << "T\t"
                << setw(14) << setprecision(10) << track_truth_x0 << '\t'
                << setw(14) << setprecision(10) << track_truth_y0 << '\t'
                << setw(14) << setprecision(10) << track_truth_z0 << '\t'
                << (int)track_truth_q << '\t'
                << setw(14) << setprecision(10) << track_truth_p*(track_truth_cosphi*track_truth_sintheta) << '\t'
                << setw(14) << setprecision(10) << track_truth_p*(track_truth_sinphi*track_truth_sintheta) << '\t'
                << setw(14) << setprecision(10) << track_truth_p*(track_truth_costheta) << '\t'
                << pdgcode << '\t'
                << setw(14) << (int)irecmatch << '\t'
                << setw(14) << setprecision(10) << precmatch << '\t'
                << extBarcode2.eventIndex() << '\t'
                << extBarcode2.barcode() << '\t'
                << parent_mask.to_ulong() << '\t'
                << isDetPaperCut << '\t'
                << resetiosflags(ios::scientific) << '\n';
      if( false && particle->production_vertex() && (particle->momentum().perp()/1000.)>10. &&
          particle->status()%1000==1 && std::abs(charge)>0.5 ) {
        using boost::format;
        const HepMC::GenParticle* tau_parent = 0;
        vector<const HepMC::GenParticle*> parents;
        parents.push_back( particle );
        while( !parents.empty() ) {
          const HepMC::GenParticle* p = parents.back();
          if( std::abs(p->pdg_id())==15 ) { tau_parent = p; break; }
          parents.pop_back();
          if( !(p->production_vertex()) ) { continue; }
          copy( p->production_vertex()->particles_begin(HepMC::parents) , p->production_vertex()->particles_end(HepMC::parents) , back_inserter(parents) );
        }
        unsigned int nprongs = 0;
        if( tau_parent && tau_parent->end_vertex() ) { nprongs = tau_parent->end_vertex()->particles_out_size(); }
        cout << " Truth Track: "
             << format("%|10d| %|10d| %|10d| %|2d| %|2d|") % ievt % particle->barcode() % particle->pdg_id() % (static_cast<bool>(tau_parent)) % nprongs
             << endl;
        if( std::abs(particle->pdg_id())>=11 && std::abs(particle->pdg_id())<=16 ) { cout << "a lepton!" << endl; }
      }
    } // end for each GenParticle in this GenEvent
  } // end for each GenEvent
}


// dump spacepoints. does not provide geant truth matching data.

void
DumpSp::dump_spacepoints() const
{
  // dump spacepoints for each of the relevant detectors.
  unsigned int nsp( 0u );
  const SpacePointContainer*          pixelSPContainer(0);
  const SpacePointContainer*          sctSPContainer(0);
  const SpacePointOverlapCollection*  overlapCollection(0);
  if( m_storeGate->retrieve(pixelSPContainer,m_pixelSpacePointsName).isFailure() ) {
    ATH_MSG_DEBUG( "Unable to retrieve PixelSpacePoint container");
  }
  if( m_storeGate->retrieve(sctSPContainer,m_sctSpacePointsName).isFailure() ) {
    ATH_MSG_DEBUG( "Unable to retrieve SCT_SpacePoint container");
  }
  if( m_storeGate->retrieve(overlapCollection,m_overlapSpacePointsName).isFailure() ) {
    ATH_MSG_DEBUG( "Unable to retrieve Overlap SpacePoint container");
  }
  if( !(pixelSPContainer || sctSPContainer || overlapCollection) ) {
    ATH_MSG_WARNING( "Unable to retrieve any SpacePoint containers");
  } else {
    if( pixelSPContainer ) {
      for( SpacePointContainer::const_iterator i=pixelSPContainer->begin(), f=pixelSPContainer->end(); i!=f; ++i ) {
        for( SpacePointCollection::const_iterator j=(**i).begin(), jf=(**i).end(); j!=jf; ++j ) {
          const Trk::SpacePoint* sp( *j );
          ++nsp;
          // dump pixel info
          IdentifierHash idh = (*sp).elementIdList().first;
          Identifier pid = m_pixelId->wafer_id( idh );
          const Amg::Vector3D point = (*sp).globalPosition();
          const double rad = point.perp();
          const double phi = point.phi();
          const double z = point.z();
          const double x = rad*cos(phi); 
          const double y = rad*sin(phi);
          (*m_ofl) << "H\t" 
                 << setw(14) << setprecision(10) << x << '\t'
                 << setw(14) << setprecision(10) << y << '\t'
                 << setw(14) << setprecision(10) << z << '\t'
                 << 1  << '\t' // 1  pixel 0 sct  
                 << m_pixelId->barrel_ec(pid) << '\t'
                 << m_pixelId->layer_disk(pid) << '\t'
                 << m_pixelId->phi_module(pid) << '\t'
                 << m_pixelId->eta_module(pid) << '\t'
                 << m_pixelId->phi_index(pid) << '\t'
                 << m_pixelId->eta_index(pid) << '\n';
        } // end of pixel space point loop
      } // end of pixel container loop 
    } // end of pixel info
    if( sctSPContainer ) { 
      for( SpacePointContainer::const_iterator i=sctSPContainer->begin(), f=sctSPContainer->end(); i!=f; ++i ) {
        for (SpacePointCollection::const_iterator j=(**i).begin(), jf=(**i).end(); j!=jf; ++j ) {
          const Trk::SpacePoint* sp( *j );
          ++nsp;
          IdentifierHash idh = (*sp).elementIdList().first;
          Identifier pid = m_sctId->wafer_id( idh );
          const Amg::Vector3D point = (*sp).globalPosition();
          const double rad = point.perp();
          const double phi = point.phi();
          const double z = point.z();
          const double x = rad*cos(phi); 
          const double y = rad*sin(phi);
          (*m_ofl) << "H\t" 
                 << setw(14) << setprecision(10) << x << '\t'
                 << setw(14) << setprecision(10) << y << '\t'
                 << setw(14) << setprecision(10) << z << '\t'
                 << 0  << '\t' // 1  pixel 0 sct  
                 << m_sctId->barrel_ec(pid) << '\t'
                 << m_sctId->layer_disk(pid) << '\t'
                 << m_sctId->phi_module(pid) << '\t'
                 << m_sctId->eta_module(pid) << '\t'
                 << m_sctId->side(pid) << '\t'
                 << m_sctId->strip(pid) << '\n';
        } // end of sct space point loop
      } // end of sct container loop
    } // end of sct info
    if( overlapCollection ) {
      for( SpacePointCollection::const_iterator i=overlapCollection->begin(), f=overlapCollection->end(); i!=f; ++i ) {
        const Trk::SpacePoint* sp( *i );
        ++nsp;
        IdentifierHash idh = (*sp).elementIdList().first;
        Identifier pid = m_sctId->wafer_id( idh );
        const Amg::Vector3D point = (*sp).globalPosition();
        const double rad = point.perp();
        const double phi = point.phi();
        const double z = point.z();
        const double x = rad*cos(phi); 
        const double y = rad*sin(phi);
        (*m_ofl) << "h\t" 
               << setw(14) << setprecision(10) << x << '\t'
               << setw(14) << setprecision(10) << y << '\t'
               << setw(14) << setprecision(10) << z << '\t'
               << 0  << '\t' // 1  pixel 0 sct  
               << m_sctId->barrel_ec(pid) << '\t'
               << m_sctId->layer_disk(pid) << '\t'
               << m_sctId->phi_module(pid) << '\t'
               << m_sctId->eta_module(pid) << '\t'
               << m_sctId->side(pid) << '\t'
               << m_sctId->strip(pid) << '\n';
      } // end of overlap space point loop
    } // end of SCT overlap info      
  } // dump all spacepoints 
  ATH_MSG_INFO(nsp << " SpacePoints found");
}

// dump silicon channels with geant matching information.

void
DumpSp::dump_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& clusterIndexMap ) const
{
  using namespace std;
  unsigned int hitIndex = 0u;
  unsigned int clusterIndex = 0u;
  // compute some statistics 
  // unsigned long nchannels = 0ul;
  // unsigned long nchannels_single_barcodes = 0ul;
  // unsigned long nchannels_single_barcodes_primary = 0ul;
  // unsigned long nchannels_single_barcodes_pt1gev = 0ul;
  // unsigned long nchannels_single_barcodes_bjet = 0ul;
  // unsigned long nchannels_multiple_barcodes = 0ul;
  // unsigned long nchannels_multiple_barcodes_primary = 0ul;
  // unsigned long nchannels_multiple_barcodes_pt1gev = 0ul;
  // unsigned long nchannels_multiple_barcodes_bjet = 0ul;

  const DataHandle<PixelRDO_Container> pixel_rdocontainer_iter;  
  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = m_storeGate->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if( m_storeGate->retrieve(pixel_rdocontainer_iter, "PixelRDOs").isSuccess()  ) {
    pixel_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( PixelRDO_Container::const_iterator iColl=pixel_rdocontainer_iter->begin(), fColl=pixel_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
      if( !pixel_rdoCollection ) { continue; }
      const int size = pixel_rdoCollection->size();     
      ATH_MSG_DEBUG( "Pixel InDetRawDataCollection found with " << size << " RDOs");
      // loop on all RDOs
      for( DataVector<PixelRDORawData>::const_iterator iRDO=pixel_rdoCollection->begin(), fRDO=pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO ) {
        Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement );
        Amg::Vector2D localPos2D = sielement->rawLocalPositionOfCell(rdoId);
        localPos2D[Trk::distPhi] += m_pixelLorentzAngleTool->getLorentzShift(sielement->identifyHash());
        const InDetDD::SiLocalPosition localPos(localPos2D);
        const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(rdoId);
        const Amg::Vector3D gPos( sielement->globalPosition(localPos) );
        // update map between pixel identifier and event-unique hit index. 
        // ganged pixels (nCells==2) get two entries.
        hitIndexMap[rdoId] = hitIndex;
        const int nCells = sielement->numberOfConnectedCells( sielement->cellIdOfPosition(rawPos) );
        if( nCells==2 ) {
          const InDetDD::SiCellId firstCell = sielement->cellIdOfPosition(rawPos);
          const InDetDD::SiCellId tmpCell = sielement->connectedCell(firstCell,1);
          const Identifier tmpId = sielement->identifierFromCellId(tmpCell);
          hitIndexMap[tmpId] = hitIndex; // add second entry for ganged pixel ID
        }
        // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel.
        const HepMC::GenParticle* best_parent = 0;
        ParentBitmask parent_mask;
        HepMcParticleLink::ExtendedBarCode best_extcode;
        if( have_pixel_sdo && pixelSimDataMap ) { 
          InDetSimDataCollection::const_iterator iter( pixelSimDataMap->find(rdoId) );
          // this might be the ganged pixel copy.
          if( nCells>1 && iter==pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId) );
            for( int ii=0; ii<nCells && iter==pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter!=pixelSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>100000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              // bcs.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // if( particleLink.eventIndex()==0 ) {
              //   bcs_prim.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // }
              // if( genPt>=1000. ) {
              //   bcs_pt1gev.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // }
              // // check whether or not parent is a b
              // {
              //   typedef pair<const HepMC::GenParticle*,unsigned int> Parent;
              //   vector<Parent> parents;
              //   parents.push_back( Parent(particle,0) );
              //   while( !parents.empty() ) {
              //     const HepMC::GenParticle* p = parents.back().first;
              //     const unsigned int level = parents.back().second;
              //     if( std::abs(p->pdg_id())==5 ) { 
              //       bcs_bjet.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              //       break;
              //     }
              //     parents.pop_back();
              //     if( !(p->production_vertex()) ) { continue; }
              //     for( HepMC::GenVertex::particle_iterator i=p->production_vertex()->particles_begin(HepMC::parents), f=p->production_vertex()->particles_end(HepMC::parents); i!=f; ++i ) {
              //       parents.push_back( Parent(*i,level+1) );
              //     }
              //   }
              // }
              parent_mask |= construct_truth_bitmap( particle );
              // check SDO
            } // end for each contributing particle
          } // end if truth found for this pixel
        } // end if pixel truth available
        ++hitIndex;
        (*m_oflraw) << "S\t" 
                  << setw(14) << setprecision(10)
                  << gPos.x() << '\t'
                  << setw(14) << setprecision(10)
                  << gPos.y() << '\t'
                  << setw(14) << setprecision(10)
                  << gPos.z() << '\t'
                  << 1  << '\t' // 1  pixel 0 sct  
                  << m_pixelId->barrel_ec(rdoId) << '\t'
                  << m_pixelId->layer_disk(rdoId) << '\t'
                  << m_pixelId->phi_module(rdoId) << '\t'
                  << m_pixelId->eta_module(rdoId) << '\t'
                  << m_pixelId->phi_index(rdoId) << '\t'
                  << m_pixelId->eta_index(rdoId) << '\t'
                  << (*iRDO)->getToT() << '\t'
                  << (long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
                  << (long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
                  << setprecision(5) << static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) << '\t' // particle pt in MeV
                  << parent_mask.to_ulong() << '\t'
                  << sielement->identifyHash() << '\t'
                  << '\n';
      } // end for each RDO in the collection
    } // for each pixel RDO collection
    // dump all pixel RDO's and SDO's for debugging purposes
    if(m_doRDODebug) {
      for( PixelRDO_Container::const_iterator iColl=pixel_rdocontainer_iter->begin(), fColl=pixel_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
        const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
        if( !pixel_rdoCollection ) { continue; }
        for( DataVector<PixelRDORawData>::const_iterator iRDO=pixel_rdoCollection->begin(), fRDO=pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO ) {
          Identifier rdoId = (*iRDO)->identify();
          // get the det element from the det element collection
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement);
          Amg::Vector2D localPos2D = sielement->rawLocalPositionOfCell(rdoId);
          localPos2D[Trk::distPhi] += m_pixelLorentzAngleTool->getLorentzShift(sielement->identifyHash());
          const InDetDD::SiLocalPosition localPos(localPos2D);
          const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(rdoId);
          const Amg::Vector3D gPos( sielement->globalPosition(localPos) );
          (*m_oflraw) << "# S\t" 
                    << setw(14) << setprecision(10)
                    << gPos.x() << '\t'
                    << setw(14) << setprecision(10)
                    << gPos.y() << '\t'
                    << setw(14) << setprecision(10)
                    << gPos.z() << '\t'
                    << 1  << '\t' // 1  pixel 0 sct  
                    << m_pixelId->barrel_ec(rdoId) << '\t'
                    << m_pixelId->layer_disk(rdoId) << '\t'
                    << m_pixelId->phi_module(rdoId) << '\t'
                    << m_pixelId->eta_module(rdoId) << '\t'
                    << m_pixelId->phi_index(rdoId) << '\t'
                    << m_pixelId->eta_index(rdoId) << '\t'
                    << (*iRDO)->getToT() << '\t'
                    << endl;
        } // end for each pixel RDO
      } // end for each pixel RDO collection
      // dump SDO's
      if( have_pixel_sdo && pixelSimDataMap ) { 
        for( InDetSimDataCollection::const_iterator i=pixelSimDataMap->begin(), f=pixelSimDataMap->end(); i!=f; ++i ) {
          const Identifier sdoId( i->first );
          const InDetSimData& sdo( i->second );
          const vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
          (*m_oflraw) << "# s"
                    << " " << m_pixelId->barrel_ec(sdoId) 
                    << " " << m_pixelId->layer_disk(sdoId) 
                    << " " << m_pixelId->phi_module(sdoId) 
                    << " " << m_pixelId->eta_module(sdoId) 
                    << " " << m_pixelId->phi_index(sdoId) 
                    << " " << m_pixelId->eta_index(sdoId) 
                    << " " << PixelSimHelper::isNoise( sdo ) 
                    << " " << PixelSimHelper::isBelowThreshold( sdo )
                    << " " << PixelSimHelper::isDisabled( sdo )
                    << " " << PixelSimHelper::hasBadTOT( sdo )
                    << " " << deposits.size()
                    << endl;
          for( vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
            const HepMcParticleLink& particleLink( iDep->first );
            const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
            (*m_oflraw) << "# s q " << qdep << " " << particleLink.isValid() << " " 
                      << (particleLink.isValid() ? particleLink.eventIndex() : std::numeric_limits<unsigned int>::max())
                      << (particleLink.isValid() ? particleLink.barcode() : std::numeric_limits<unsigned int>::max())
                      << endl;
          } // end for each deposit in SDO
        } // end for each pixel SDO
      } // if have sdo info
    } // end dump RDO's and SDO's for debugging purposes (m_doRDODebug)
  } // dump raw pixel data

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* sctElements(sctDetEle.retrieve());
  if (sctElements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return;
  }
  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = m_storeGate->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( m_storeGate->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      const int size = SCT_Collection->size();     
      ATH_MSG_DEBUG( "SCT InDetRawDataCollection found with " << size << " RDOs");
      for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const Identifier wafer_id = m_sctId->wafer_id(rdoId);
        const IdentifierHash wafer_hash = m_sctId->wafer_hash(wafer_id);
        const InDetDD::SiDetectorElement* sielement = sctElements->getDetectorElement(wafer_hash);
        const InDetDD::SCT_ModuleSideDesign& design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(sielement->design());
        const InDetDD::SiLocalPosition localPos = design.positionFromStrip(m_sctId->strip(rdoId));
        const Amg::Vector3D gPos = sielement->globalPosition(localPos);    
        hitIndexMap[rdoId] = hitIndex;
        ++hitIndex;
        // if there is simulation truth available, try to retrieve the
        // "most likely" barcode for this strip.
        const HepMC::GenParticle* best_parent = 0;
        ParentBitmask parent_mask;
        HepMcParticleLink::ExtendedBarCode best_extcode;
        if( have_sct_sdo && sctSimDataMap ) { 
          InDetSimDataCollection::const_iterator iter( sctSimDataMap->find(rdoId) );
          // if SDO found for this pixel, associate the particle
          if( iter!=sctSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              // const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pt (<400 MeV) pileup truth
              if( particle->barcode()>100000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              // bcs.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // if( particleLink.eventIndex()==0 ) {
              //   bcs_prim.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // }
              // if( genPt>=1000. ) {
              //   bcs_pt1gev.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              // }
              // {
              //   typedef pair<const HepMC::GenParticle*,unsigned int> Parent;
              //   vector<Parent> parents;
              //   parents.push_back( Parent(particle,0) );
              //   while( !parents.empty() ) {
              //     const HepMC::GenParticle* p = parents.back().first;
              //     const unsigned int level = parents.back().second;
              //     if( std::abs(p->pdg_id())==5 ) { 
              //       bcs_bjet.insert( HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() ) );
              //       break;
              //     }
              //     parents.pop_back();
              //     if( !(p->production_vertex()) ) { continue; }
              //     for( HepMC::GenVertex::particle_iterator i=p->production_vertex()->particles_begin(HepMC::parents), f=p->production_vertex()->particles_end(HepMC::parents); i!=f; ++i ) {
              //       parents.push_back( Parent(*i,level+1) );
              //     }
              //   }
              // }
              parent_mask |= construct_truth_bitmap( particle );
            } // end for each contributing particle
          } // end if truth found for this strip
        } // end if sct truth available        
        (*m_oflraw) << "S\t" 
                  << setw(14) << setprecision(10)
                  << gPos.x() << '\t'
                  << setw(14) << setprecision(10)
                  << gPos.y() << '\t'
                  << setw(14) << setprecision(10)
                  << gPos.z() << '\t'
                  << 0  << '\t' // 1  pixel 0 sct  
                  << m_sctId->barrel_ec(rdoId) << '\t'
                  << m_sctId->layer_disk(rdoId) << '\t'
                  << m_sctId->phi_module(rdoId) << '\t'
                  << m_sctId->eta_module(rdoId) << '\t'
                  << m_sctId->side(rdoId) << '\t'
                  << m_sctId->strip(rdoId) << '\t'
                  << (*iRDO)->getGroupSize() << '\t'
                  << (long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
                  << (long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
                  << setprecision(5) << static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) << '\t' // particle pt in MeV
                  << parent_mask.to_ulong() << '\t'
                  << sielement->identifyHash() << '\t'
                  << '\n';
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
    if(m_doRDODebug) { 
      // dump SCT RDO's
      for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
        const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
        if( !SCT_Collection ) { continue; }
        const int size = SCT_Collection->size();     
        ATH_MSG_DEBUG( "SCT InDetRawDataCollection found with " << size << " RDOs");
        for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
          const Identifier rdoId = (*iRDO)->identify();
          const Identifier wafer_id = m_sctId->wafer_id(rdoId);
          const IdentifierHash wafer_hash = m_sctId->wafer_hash(wafer_id);
          const InDetDD::SiDetectorElement* sielement = sctElements->getDetectorElement(wafer_hash);
          const InDetDD::SCT_ModuleSideDesign& design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(sielement->design());
          const InDetDD::SiLocalPosition localPos = design.positionFromStrip(m_sctId->strip(rdoId));
          const Amg::Vector3D gPos = sielement->globalPosition(localPos);  
          (*m_oflraw) << "# S\t" 
                    << setw(14) << setprecision(10)
                    << gPos.x() << '\t'
                    << setw(14) << setprecision(10)
                    << gPos.y() << '\t'
                    << setw(14) << setprecision(10)
                    << gPos.z() << '\t'
                    << 0  << '\t' // 1  pixel 0 sct  
                    << m_sctId->barrel_ec(rdoId) << '\t'
                    << m_sctId->layer_disk(rdoId) << '\t'
                    << m_sctId->phi_module(rdoId) << '\t'
                    << m_sctId->eta_module(rdoId) << '\t'
                    << m_sctId->side(rdoId) << '\t'
                    << m_sctId->strip(rdoId) << '\t'
                    << (*iRDO)->getGroupSize() << '\t'
                    << endl;
        } // end for each SCT rdo
      }  // end for each SCT rdo collection
      // dump SCT SDO's
      if( have_sct_sdo && sctSimDataMap ) { 
        for( InDetSimDataCollection::const_iterator i=sctSimDataMap->begin(), f=sctSimDataMap->end(); i!=f; ++i ) {
          const Identifier sdoId( i->first );
          const InDetSimData& sdo( i->second );          
          const vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
          (*m_oflraw) << "# s"
                    << " " << m_sctId->barrel_ec(sdoId) 
                    << " " << m_sctId->layer_disk(sdoId) 
                    << " " << m_sctId->phi_module(sdoId) 
                    << " " << m_sctId->eta_module(sdoId) 
                    << " " << m_sctId->side(sdoId) 
                    << " " << m_sctId->strip(sdoId) 
                    << " " << SCT_SimHelper::isNoise( sdo )
                    << " " << SCT_SimHelper::isBelowThreshold( sdo )
                    << " " << SCT_SimHelper::isDisabled( sdo )
                    << " " << deposits.size()
                    << endl;
          for( vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
            const HepMcParticleLink& particleLink( iDep->first );
            const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
            (*m_oflraw) << "# s q " << qdep << " " << particleLink.isValid() << " " 
                      << (particleLink.isValid() ? particleLink.eventIndex() : std::numeric_limits<unsigned int>::max())
                      << (particleLink.isValid() ? particleLink.barcode() : std::numeric_limits<unsigned int>::max())
                      << endl;
          } // end for each deposit associated with this SDO
        } // end for each SCT SDO
      }  // end if SDO's available, dump them
    } // end dump all RDO's and SDO's for a given event (m_doRDODebug)
  } // end dump raw SCT data

  // FlagJT dump pixel clusters. They're in m_pixelContainer
  m_pixelContainer->clID(); // anything to dereference the DataHandle
  for( InDet::SiClusterContainer::const_iterator iColl=m_pixelContainer->begin(), fColl=m_pixelContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection( *iColl );
    if( !pixelClusterCollection ) {
      ATH_MSG_DEBUG( "pixelClusterCollection not available!");
      continue; 
    }
    const unsigned int size = pixelClusterCollection->size();     
    ATH_MSG_DEBUG( "PixelClusterCollection found with " << size << " clusters");
    for( DataVector<InDet::SiCluster>::const_iterator iCluster=pixelClusterCollection->begin(), fCluster=pixelClusterCollection->end(); iCluster!=fCluster; ++iCluster ) {
      Identifier theId = (*iCluster)->identify();
      const Amg::Vector3D gPos = (*iCluster)->globalPosition();
      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      const HepMC::GenParticle* best_parent = 0;
      ParentBitmask parent_mask;
      HepMcParticleLink::ExtendedBarCode best_extcode;
      if( have_pixel_sdo && pixelSimDataMap ) { 
        for( std::vector<Identifier>::const_iterator rdoIter = (*iCluster)->rdoList().begin();
             rdoIter != (*iCluster)->rdoList().end(); rdoIter++ ) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(*rdoIter); 
          assert( sielement );
          const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(*rdoIter);
          const int nCells = sielement->numberOfConnectedCells( sielement->cellIdOfPosition(rawPos) );
          InDetSimDataCollection::const_iterator iter( pixelSimDataMap->find(*rdoIter) );
          // this might be the ganged pixel copy.
          if( nCells>1 && iter==pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(*rdoIter), m_pixelId->eta_index(*rdoIter) );
            for( int ii=0; ii<nCells && iter==pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter!=pixelSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>100000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= construct_truth_bitmap( particle );
            } // loop over deposits
          } // if found SDO of pixel
        } // loop over pixels in cluster
      } // if we have pixel sdo's available

      const InDetDD::SiDetectorElement* sielement( (*iCluster)->detectorElement() ); assert( sielement );

      // compute hit position in units of channel number.  local hit
      // position is measured in a right-handed coordinate system
      // centered on the center of a wafer
      // (SiDetectorElement::center() in global coordinates) with axes
      // SiDetectorElement::phiAxis() and SiDetectorElement::etaAxis()
      // assume all rdos in cluster are on the same physical
      // wafer. note that pixel pitch is not constant vs channel
      // number, but the Trk::Surface models of disks and barrel
      // (cylinder) treat it as such.

      float localx=-1,localy=-1;
      {
        // const InDetDD::SiLocalPosition localCentroid( (*iCluster)->localPosition() );
	// Amg::Vector2D corrLocalCentroid(localCentroid);
	// 
	// corrLocalCentroid[Trk::distPhi] -= sielement->getLorentzCorrection();
	// 
	// const InDetDD::SiCellId cellIdCentroid( sielement->cellIdOfPosition(corrLocalCentroid ));//localCentroid ) );
        // //const Amg::Vector2D localCellCentroid( sielement->localPositionOfCell( cellIdCentroid ) );
	// 
	// 
	// //using the non Lorentz shift-corrected value.  
	// const Amg::Vector2D localCellCentroid( sielement->localPositionOfCell( cellIdCentroid ) );
	// 
	// float deltaphi = (*iCluster)->localPosition()[Trk::distPhi] - localCellCentroid[Trk::distPhi];
        // 
        // 
        // //using the non Lorentz shift-corrected value.  
        // const Trk::LocalPosition localCellCentroid( sielement->localPositionOfCell( cellIdCentroid ) );
        // 
        // float deltaphi = (*iCluster)->localPosition()[Trk::distPhi] - localCellCentroid[Trk::distPhi];
        // 
        // float deltaeta = (*iCluster)->localPosition()[Trk::distEta] - localCellCentroid[Trk::distEta];
        // float deltaxphi = deltaphi / sielement->phiPitch();
        // float deltaxeta = deltaeta / sielement->etaPitch();
        // // do not add 0.5 to center in middle of strip; match SCT leading spatial edge convention
        // localx = cellIdCentroid.phiIndex() + deltaxphi; // + 0.5
        // localy = cellIdCentroid.etaIndex() + deltaxeta; // + 0.5 

        // FlagAA: THIS CODE IS NOT BACKWARD COMPATIBLE! Commited on Sep 18th, 2013
        // now stores the local position in millimiters without Lorentz Correction
        // before it was storing a floating point coordinate in units of pixels
        localx = (*iCluster)->localPosition()[Trk::distPhi] - m_pixelLorentzAngleTool->getLorentzShift(sielement->identifyHash());
        localy = (*iCluster)->localPosition()[Trk::distEta];
      }

      (*m_oflraw) << "P\t" 
                << setw(14) << setprecision(10)
                << gPos.x() << '\t'
                << setw(14) << setprecision(10)
                << gPos.y() << '\t'
                << setw(14) << setprecision(10)
                << gPos.z() << '\t'
                << m_pixelId->barrel_ec(theId) << '\t'
                << m_pixelId->layer_disk(theId) << '\t'
                << m_pixelId->phi_module(theId) << '\t'
                << m_pixelId->eta_module(theId) << '\t'
                << m_pixelId->phi_index(theId) << '\t'
                << m_pixelId->eta_index(theId) << '\t'
        // Cluster local position
                << localx << '\t'
                << localy << '\t'
        // Cluster width:
                << (*iCluster)->width().colRow().x() << '\t' // width in phi?
                << (*iCluster)->width().colRow().y() << '\t' // width in eta?
                << (*iCluster)->rdoList().size() << '\t' // number of pixels in cluster
        // Cluster truth
                << (long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
                << (long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
                << setprecision(5) << static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) << '\t' // particle pt in MeV
                << parent_mask.to_ulong()
        // Module identifier hash
                << sielement->identifyHash()
        //
                << endl;
      clusterIndexMap[theId] = clusterIndex++;
    } // End loop over pixel clusters
  } // End loop over pixel cluster collection

  // dump SCT merged clusters.
  m_sctContainer->clID(); // anything to dereference the DataHandle
  for( InDet::SiClusterContainer::const_iterator iColl=m_sctContainer->begin(), fColl=m_sctContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* sctClusterCollection( *iColl );
    if( !sctClusterCollection ) {
      ATH_MSG_DEBUG( "sctClusterCollection not available!");
      continue; 
    }
    const unsigned int size = sctClusterCollection->size();     
    ATH_MSG_DEBUG( "SctClusterCollection found with " << size << " clusters");
    for( DataVector<InDet::SiCluster>::const_iterator iCluster=sctClusterCollection->begin(), fCluster=sctClusterCollection->end(); iCluster!=fCluster; ++iCluster ) {
      Identifier theId = (*iCluster)->identify();
      const Amg::Vector3D gPos = (*iCluster)->globalPosition();
      // if there is simulation truth available, try to retrieve the "most likely" barcode for this sct cluster.
      const HepMC::GenParticle* best_parent = 0;
      ParentBitmask parent_mask;
      HepMcParticleLink::ExtendedBarCode best_extcode;
      if( have_sct_sdo && sctSimDataMap ) { 
        for( std::vector<Identifier>::const_iterator rdoIter = (*iCluster)->rdoList().begin();
             rdoIter != (*iCluster)->rdoList().end(); rdoIter++ ) {
          const Identifier wafer_id = m_sctId->wafer_id(*rdoIter);
          const IdentifierHash wafer_hash = m_sctId->wafer_hash(wafer_id);
          const InDetDD::SiDetectorElement* sielement = sctElements->getDetectorElement(wafer_hash);
          assert( sielement );
          const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(*rdoIter);
          InDetSimDataCollection::const_iterator iter( sctSimDataMap->find(*rdoIter) );
          // if SDO found for this cluster, associate the particle. otherwise leave unassociated.
          if( iter!=sctSimDataMap->end() )  {
            const InDetSimData& sdo( iter->second );
            const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
            for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
              const HepMcParticleLink& particleLink( iDep->first );
              //const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
              // RDO's without SDO's are delta rays or detector noise.
              if( !particleLink.isValid() ) { continue; }
              const HepMC::GenParticle* particle( particleLink );
              const float genEta=particle->momentum().pseudoRapidity();
              const float genPt=particle->momentum().perp(); // MeV
              // reject unstable particles
              if( particle->status()%1000!=1 ) { continue; }
              // reject secondaries and low pT (<400 MeV) pileup
              if( particle->barcode()>100000 || particle->barcode()==0 ) { continue; }
              // reject far forward particles
              if( fabs(genEta)>m_maxEta ) { continue; }
              // "best_parent" is the highest pt particle
              if( !best_parent || best_parent->momentum().perp()<genPt ) {
                best_parent = particle;
                best_extcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
              }
              parent_mask |= construct_truth_bitmap( particle );
            } // loop over deposits
          } // if found SDO of sct
        } // loop over scts in cluster
      } // if we have sct sdo's available

      const InDetDD::SiDetectorElement* sielement( (*iCluster)->detectorElement() ); assert( sielement );

      // compute hit position in units of strip number.  local hit
      // position is measured in a right-handed coordinate system
      // centered on the center of a wafer
      // (SiDetectorElement::center() in global coordinates) with axes
      // SiDetectorElement::phiAxis() and SiDetectorElement::etaAxis()
      // assume all rdos in cluster are on the same physical
      // wafer. note that pitch is not necessarily a constant vs channel
      // number, but the Trk::Surface models of disks and barrel
      // (cylinder) treat it as such.

      float localx=-1;
      {
        const InDetDD::SiLocalPosition localCentroid( (*iCluster)->localPosition() );
        const InDetDD::SiCellId cellIdCentroid( sielement->cellIdOfPosition( localCentroid ) );
        Amg::Vector2D localCellCentroid( sielement->rawLocalPositionOfCell( cellIdCentroid ) );
        localCellCentroid[Trk::distPhi] += m_sctLorentzAngleTool->getLorentzShift(sielement->identifyHash());
        float deltaphi = (*iCluster)->localPosition()[Trk::distPhi] - localCellCentroid[Trk::distPhi];
        float deltaxphi = deltaphi / sielement->phiPitch();
        // do not add 0.5 to center in middle of strip; match SCT leading spatial edge convention
        localx = cellIdCentroid.phiIndex() + deltaxphi; // + 0.5
      }

      (*m_oflraw) << "C\t" 
                << setw(14) << setprecision(10)
                << gPos.x() << '\t'
                << setw(14) << setprecision(10)
                << gPos.y() << '\t'
                << setw(14) << setprecision(10)
                << gPos.z() << '\t'
                << 0  << '\t' // 1  pixel 0 sct  
                << m_sctId->barrel_ec(theId) << '\t'
                << m_sctId->layer_disk(theId) << '\t'
                << m_sctId->phi_module(theId) << '\t'
                << m_sctId->eta_module(theId) << '\t'
                << m_sctId->side(theId) << '\t'
        // Cluster local position and width
                << m_sctId->strip(theId) << '\t'
                << localx << '\t'
                << (*iCluster)->rdoList().size() << '\t' // number of strips in cluster
                << (long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
                << (long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
                << setprecision(5) << static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) << '\t' // particle pt in MeV
                << parent_mask.to_ulong() << '\t'
                << sielement->identifyHash() << '\t'
                << endl;
      clusterIndexMap[theId] = clusterIndex++;
    } // End loop over sct clusters
  } // End loop over sct cluster collection

  // dump the statistics
  // cout << boost::format("truth parent stats: nch = %|6d| %|6d| %|6d| %|6d| %|6d| fmt = %|5g| fmtp = %|5g| fmtp1 = %|5g| fmb = %|5g|")
  //   % nchannels
  //   % nchannels_single_barcodes
  //   % nchannels_single_barcodes_primary
  //   % nchannels_single_barcodes_pt1gev
  //   % nchannels_single_barcodes_bjet
  //   % (nchannels>0ul ? nchannels_multiple_barcodes/static_cast<double>(nchannels) : 0.)
  //   % (nchannels>0ul ? nchannels_multiple_barcodes_primary/static_cast<double>(nchannels) : 0.)
  //   % (nchannels>0ul ? nchannels_multiple_barcodes_pt1gev/static_cast<double>(nchannels) : 0.)
  //   % (nchannels>0ul ? nchannels_multiple_barcodes_bjet/static_cast<double>(nchannels) : 0.)
  //      << endl;

} // end dump raw silicon data


// dump list of bad pixel and SCT modules for this event

void
DumpSp::dump_bad_modules() const
{
  if( m_doBadMod) {
    // dump list of bad pixel modules
    for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd(); i!=f; ++i ) {
      const InDetDD::SiDetectorElement* sielement( *i );
      Identifier id = sielement->identify();
      IdentifierHash idhash = sielement->identifyHash();
      const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
      if( is_bad ) { 
        (*m_oflraw) << "B\t"
                  << 1  << '\t' // 1  pixel 0 sct  
                  << m_pixelId->barrel_ec(id) << '\t'
                  << m_pixelId->layer_disk(id) << '\t'
                  << m_pixelId->phi_module(id) << '\t'
                  << m_pixelId->eta_module(id) << '\t'
          // << m_pixelId->phi_index(id) << '\t'
          // << m_pixelId->eta_index(id) << '\t'
                  // << is_bad
                  << std::endl;
      }
    } // end for each pixel module
    // dump list of bad sct modules
    SCT_ID::const_id_iterator wafer_it = m_sctId->wafer_begin();
    SCT_ID::const_id_iterator wafer_end = m_sctId->wafer_end();
    for (; wafer_it!=wafer_end; wafer_it++) {
      const Identifier id = *wafer_it;
      const IdentifierHash idhash = m_sctId->wafer_hash(id);
      const bool is_bad = !(m_sctCondSummaryTool->isGood( idhash ));
      if( is_bad ) { 
        (*m_oflraw) << "B\t"
                  << 0  << '\t' // 1  pixel 0 sct  
                  << m_sctId->barrel_ec(id) << '\t'
                  << m_sctId->layer_disk(id) << '\t'
                  << m_sctId->phi_module(id) << '\t'
                  << m_sctId->eta_module(id) << '\t'
                  << m_sctId->side(id) << '\t'
                  // << is_bad
                  << std::endl;
      }
    } // end for each SCT module
  } // end dump bad silicon modules
} // end dump list of bad silicon modules



// dump reconstructed tracks with geant matching information.
  
void
DumpSp::dump_tracks( const HitIndexMap& /*hitIndexMap*/, const HitIndexMap& clusterIndexMap ) const
{
  ATH_MSG_DEBUG( "getting the reconstructed track container.");
  const DataVector<Trk::Track>* trks = 0;
  if( m_storeGate->retrieve(trks,m_tracksName).isFailure() ) { 
    ATH_MSG_WARNING( "unable to retrieve reconstructed TrackCollection " << m_tracksName);
    return;
  }
  
  if( !(trks->empty()) ) { 
    ATH_MSG_DEBUG( "found reconstructed track container with " << trks->size() << " tracks."); 
    ATH_MSG_DEBUG( "begin dumping reconstructed tracks"); 
  }

  unsigned int itrack = 0u;
  for( DataVector<Trk::Track>::const_iterator iTrk=trks->begin(), iTrkF=trks->end(); iTrk!=iTrkF; ++iTrk, ++itrack ) {
    const Trk::Track* track( *iTrk );
    const Amg::Vector3D vertex(0.,0.,0.); // FIXME --- retrieve the fitted primary vertex position here
    const double charge = track->perigeeParameters()->charge();
    const Amg::Vector3D perigee_position( track->perigeeParameters()->position() );
    const double z0 = perigee_position.z();
    const double phi0 = track->perigeeParameters()->parameters()[Trk::phi];
    const double qpt = charge*std::abs(((*track).perigeeParameters()->pT())/CLHEP::GeV);
    // FIXME --- verify that this the correct d0 sign convention in
    // the presence of beamline displacement. low priority until
    // vertex!=0.
    const Amg::Vector3D beam_to_perigee_vector( perigee_position - vertex );
    const double d0_sign = std::sin( beam_to_perigee_vector.phi() - phi0 ) > 0. ? 1. : -1.;
    const double d0 = d0_sign*( sqrt( pow(perigee_position.x()-vertex.x(),(int)2) + pow(perigee_position.y()-vertex.y(),(int)2) ) - vertex.perp() );
    // apply track selection if requested
    if( m_useOfflineTrackSelectorTool ) {
      // v13 does not have an IDetailedTrackSelectorTool, so make the cuts manually here. use
      // exactly the cuts configured by InDetRecExample, except 1) the track parameter extrapolation
      // is the perigee extrapolation provided by the track, not the (potentially) different calculation
      // employing a custom extrapolator, and 2) the cuts on the number of silicon hits are not corrected
      // for dead SCT or pixel layers; this information is not available in v13 TrackSummary.
      //
      // get track parameters using the same extrapolation as DetailedTrackSelectorTool
      // const Trk::Perigee* perigeeBeforeExtrapolation = dynamic_cast<const Trk::Perigee*>( track->perigeeParameters() );
      const Trk::Perigee* extrapolatedPerigee = dynamic_cast<const Trk::Perigee*>( track->perigeeParameters() );
      if( false ) { 
        // recompute the extrapolated perigee using a new extrapolator. the actual
        // extrapolation is commented out, because this algorithm does not have
        // an extrapolator configured.
        Trk::PerigeeSurface perigeeSurface( vertex );
        const Trk::TrackParameters* firstmeaspar = 0;
        for( unsigned int i=0, f=track->trackParameters()->size(); i!=f; ++i ) {
          if( dynamic_cast<const Trk::TrackParameters*>((*track->trackParameters())[i]) && 
              !dynamic_cast<const Trk::Perigee*>((*track->trackParameters())[i]) ) {
            firstmeaspar=(*track->trackParameters())[i];
            break;
          }
        }
        if( !firstmeaspar ) {
          firstmeaspar = track->perigeeParameters();
          if( !firstmeaspar ) { continue; } // no track selection if firstmeas + perigee does not exist !
        }
        //Trk::PropDirection dir( Trk::oppositeMomentum );
        // for now, approximate the following statements:
        //          boost::scoped_ptr<const Trk::ParametersBase> extrapolatedParameters( firstmeaspar ? 
        //               m_extrapolator->extrapolate(*firstmeaspar,perigeeSurface,dir,true,track->particleHypothesis() ) :
        //               0 );
        //          extrapolatedPerigee = extrapolatedParameters ? dynamic_cast<const Trk::MeasuredPerigee*>(extrapolatedParameters.get()) : 0; 
        // with the default extrapolated parameters coming from the track.
      }
      if( !extrapolatedPerigee ) { continue; } // no track selection if firstmeas + perigee does not exist !
      const AmgVector(5) &perigeeParms = extrapolatedPerigee->parameters();
      if( std::abs(perigeeParms[Trk::d0]) > 1. ) { continue; }
      if( std::abs(perigeeParms[Trk::z0]) > 1000. ) { continue; }
      if( Amg::error(*(extrapolatedPerigee->covariance()),Trk::d0)>0.35 ) { continue; }
      if( Amg::error(*(extrapolatedPerigee->covariance()),Trk::z0)>2.5 ) { continue; }
      const Trk::FitQuality*  trkQuality = track->fitQuality();
      const double chi2 =  trkQuality->chiSquared();
      const int ndf = trkQuality->numberDoF();
      if( chi2/double(ndf) > 3.5 ) { continue; }
      boost::scoped_ptr<const Trk::TrackSummary> summary( m_trkSummaryTool->createSummary( *track ) );
      const unsigned int nsct = summary->get(Trk::numberOfSCTHits);
      const unsigned int npix = summary->get(Trk::numberOfPixelHits);
      //       const unsigned int nsct = (summary->get(Trk::numberOfSCTHits)+summary->get(Trk::numberOfSCTDeadSensors));
      //       const unsigned int npix = (summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors));
      if( nsct < 5 ) { continue; }
      if( npix < 1 ) { continue; }
      if( nsct+npix < 7 ) { continue; }
      if( summary->get(Trk::numberOfSCTSharedHits)>1 ) { continue; }
      // compute d0 and z0 significance.
      // const double sinTheta = sin(perigeeParms[Trk::theta]);
      //const double cosTheta = cos(perigeeParms[Trk::theta]);
      const double d0wrtPriVtx = perigeeParms[Trk::d0];
      //const double deltaZ = perigeeParms[Trk::z0];
      //const double z0wrtPriVtx = deltaZ*sinTheta;
      const double testtrackSigD0 = Amg::error(*(extrapolatedPerigee->covariance()),Trk::d0);
      //const double testtrackSigZ0 = extrapolatedPerigee->localErrorMatrix().error(Trk::z0);
      //const double testtrackSigTh = extrapolatedPerigee->localErrorMatrix().error(Trk::theta);
      // error on IP:
      //const double trackPhi = perigeeParms[Trk::phi];
      //const double dIPdx = sin(trackPhi);
      //const double dIPdy = -cos(trackPhi);
      const double DD0 = testtrackSigD0*testtrackSigD0;
      double newD0Err=0;
      //       const double DXX = dIPdx*dIPdx*vertex.errorPosition().covariance()[0][0];
      //       const double DYY = dIPdy*dIPdy*vertex.errorPosition().covariance()[1][1];
      //       const double DXY = 2.*dIPdx*dIPdy*vertex.errorPosition().covariance()[0][1];
      //           newD0Err = DD0 + DXX + DYY + DXY;
      //         }
      newD0Err = DD0; // no vertex to correct
      const double d0ErrwrtPriVtx = (newD0Err>0 ? sqrt(newD0Err) : -10e-9);
      if( std::abs(d0wrtPriVtx/d0ErrwrtPriVtx)>4. ) { continue; }
    }
    // retrieve any truth info for this track.
    RecoToTruthTrackMap::const_iterator irtt = m_rttTrackMap.find( itrack );
    HepMcParticleLink::ExtendedBarCode best_extcode;
    float mc_frac = -0.001;
    if( irtt!=m_rttTrackMap.end() ) {
      best_extcode = irtt->second;
      TruthToRecoProbMap::const_iterator ittr = m_ttrProbMap.find( irtt->second );
      assert( ittr != m_ttrProbMap.end() );
      mc_frac = ittr->second;
    }
    // dump one line for each track
    if( m_dumpSpacePoints ) {
      (*m_ofl) << "E\t" 
             << setw(14) << setprecision(10) << d0 << '\t'
             << setw(14) << setprecision(10) << z0 << '\t'
             << setw(14) << setprecision(10) << phi0 << '\t'
             << setw(14) << setprecision(10) << 1./tan(track->perigeeParameters()->parameters()[Trk::theta]) << '\t'
             << setw(15) << setprecision(10) << qpt << '\t'
             << setw(14) << (long)(mc_frac>=0. ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
             << setw(14) << (long)(mc_frac>=0. ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
             << setw(14) << setprecision(10) << mc_frac
             << endl;
    }
    (*m_oflraw) << "E\t" 
              << setw(14) << setprecision(10) << d0 << '\t'
              << setw(14) << setprecision(10) << z0 << '\t'
              << setw(14) << setprecision(10) << phi0 << '\t'
              << setw(14) << setprecision(10) << 1./tan(track->perigeeParameters()->parameters()[Trk::theta]) << '\t'
              << setw(15) << setprecision(10) << qpt << '\t'
              << setw(14) << (mc_frac>=0. ? best_extcode.eventIndex() : std::numeric_limits<long>::max()) << '\t'
              << setw(14) << (mc_frac>=0. ? best_extcode.barcode() : std::numeric_limits<long>::max()) << '\t'
              << setw(14) << setprecision(10) << mc_frac << '\t';
    // dump the indices of the clusters used in this track
    for( DataVector<const Trk::MeasurementBase>::const_iterator hitOnTrack = track->measurementsOnTrack()->begin();
         hitOnTrack != track->measurementsOnTrack()->end(); hitOnTrack++ ) {
      const Trk::RIO_OnTrack* hit = dynamic_cast<const Trk::RIO_OnTrack*>(*hitOnTrack);
      if( !hit ) { continue; }
      const Trk::PrepRawData* hitPRD( hit->prepRawData() );
      if( hitPRD ) {
        Identifier theId = hitPRD->identify();
	
	if( !m_pixelId->is_pixel(theId) && ! m_sctId->is_sct(theId)){
	  ATH_MSG_VERBOSE( "an RIO on track which is not pix or sct");
	  continue;
	}
	  
        // output the index of the hit
        std::map<Identifier,int>::const_iterator theIndex = clusterIndexMap.find(theId);
        if( theIndex != clusterIndexMap.end() ) {
          (*m_oflraw) << theIndex->second << ' ';
	  ATH_MSG_VERBOSE( "a silicon cluster identifier was  found in the index."); 	  
	} else {
          ATH_MSG_VERBOSE( "a silicon cluster identifier was not found in the index."); 	 

	  ATH_MSG_VERBOSE("Pixel "<< m_pixelId->barrel_ec(theId) << " "
			<< m_pixelId->layer_disk(theId) <<" "
			<< m_pixelId->phi_module(theId) <<" "
			<< m_pixelId->eta_module(theId) <<" "
			<< m_pixelId->phi_index(theId) << " "
			<< m_pixelId->eta_index(theId));
       	  ATH_MSG_VERBOSE("SCT "<< m_sctId->barrel_ec(theId) << " "
			<< m_sctId->layer_disk(theId) <<" "
			<< m_sctId->phi_module(theId) <<" "
			<< m_sctId->eta_module(theId));
		
        }
      } else {
        ATH_MSG_DEBUG( "a silicon cluster on track did not have any prepRawData.");
      }
      // if hit RIO on track
    } // end loop over hits on tracks
    (*m_oflraw) << endl;
    
    // Alberto's code for dumping hits on tracks. this code needs to
    // be updated to use the modern track classes.
    if( m_dumpHitsOnTracks ) {
      // // Get SCT_DetectorElementCollection
      // SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
      // const InDetDD::SiDetectorElementCollection* sctElements(sctDetEle.retrieve());
      // if (sctElements==nullptr) {
      //   ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
      //   return;
      // }
      //       // loop over list of HitOnTrack and dump them
      //       for( hit_const_iterator iHit=track->hit_list_begin(), fHit=track->hit_list_end(); iHit!=fHit; ++iHit ) {
      //         const HitOnTrack* pHit( *iHit ); 
      //         const Amg::Vector3D global_pos( pHit->position() );
      //         const Trk::TrkDetElementBase* detElem( pHit->detectorElement() );
      //         if( !detElem ) { continue; } // process only hits associated with a detector element
      //         const Identifier rdoId = detElem->identify();
      //         const InDetDD::SiDetectorElement* sielement = 0;
      //         // get the PrepRawData data( cluster for Pixel and SCT hits)
      //         const Trk::PrepRawData* pPrepClu = pHit->rio(); 
      //         // is this an SCT hit?
      //         const Identifier wafer_id = m_sctId->wafer_id(rdoId);
      //         const IdentifierHash wafer_hash = m_sctId->wafer_hash(wafer_id);
      //         sielement = sctElements->getDetectorElement( wafer_hash );
      //         if( sielement && sielement->isSCT() ) { // hit is SCT
      //           // if there is no PrepRawData associated then the hit is
      //           // not a real hit seen by the detector, so skip it.
      //           if( !pPrepClu ) { continue; }
      //           // get local position w/ Lorentz correction
      //           const Amg::Vector2D* locPos = &(pPrepClu->localPosition()); 
      //           assert( locPos );
      //           // get list of associated hit identifiers
      //           const vector<Identifier>* pHitRdoList = &(pPrepClu->rdoList());
      //           assert( pHitRdoList );
      //           const Amg::Vector2D rawPos = *locPos - sielement->getLorentzCorrection();
      //           const Identifier tmpId = sielement->identifierOfPosition(rawPos);
      //           const Amg::Vector2D cellPos = sielement->localPositionOfCell(tmpId); // w/ Lorentz correction
      //           const Amg::Vector2D deltaPos = *locPos - cellPos; // delta w.r.t. center of cell
      //           (*m_oflraw) << "e\t" 
      //                  << setw(14) << setprecision(10)
      //                  << global_pos[0] << '\t' << global_pos[1] << '\t' << global_pos[2] << '\t'
      //                  << resetiosflags(ios::scientific)
      //                  << 0 << '\t'
      //                  << m_sctId->barrel_ec(rdoId) << '\t'
      //                  << m_sctId->layer_disk(rdoId) << '\t'
      //                  << m_sctId->phi_module(rdoId) << '\t'
      //                  << m_sctId->eta_module(rdoId) << '\t'
      //                  << m_sctId->side(rdoId)       << '\t'
      //                  << m_sctId->strip(tmpId)       << '\t'  // use tmpId here
      //                  << setiosflags(ios::fixed) << setprecision(4)
      //                  << deltaPos[0] << '\t' << deltaPos[1] << '\t'  // delta w.r.t. center of cell (Lorentz correction cancels out)
      //                  << resetiosflags(ios::scientific)
      //                  << pHitRdoList->size() << '\n';
      //           // FlagAA: here the loop is over single strip hits.
      //           // It would be interesting to dump multiple hits
      //           // when they correspond to 2 separate raw hits.
      //           // E.g. when they cross the chip boundary and are thus clustered
      //           // separately at front-end level and into a single cluster offline.
      //           // This is left to be done later on.
      //           //       int hitCounter=0;
      //           //       for (pAssocHit = pHitRdoList->begin();
      //           //            pAssocHit != pHitRdoList->end(); pAssocHit++) {
      //           //         if (hitCounter<)
      //           //           hitCounter++;
      //           //         (*m_oflraw)<< "\t i=" << *pAssocHit;
      //           //         (*m_oflraw)<< "\thId=" << hitIndexMap[*pAssocHit];
      //           //       }
      //         } // end of SCT
      //         sielement = m_PIX_mgr->getDetectorElement( rdoId );
      //         if( sielement && sielement->isPixel() ) { // hit is pixel
      //           // if there is no PrepRawData associated then the hit is
      //           // not a real hit seen by the detector, so skip it.
      //           if( !pPrepClu ) { continue; }
      //           // get local position w/ Lorentz correction
      //           const Amg::Vector2D* locPos =  &(pPrepClu->localPosition()); 
      //           assert( locPos ); 
      //           // get list of associated hit identifiers
      //           const vector<Identifier>* pHitRdoList = &(pPrepClu->rdoList());
      //           assert( pHitRdoList );
      //           const Amg::Vector2D rawPos = *locPos - sielement->getLorentzCorrection();
      //           const Identifier tmpId = sielement->identifierOfPosition(rawPos);
      //           const Amg::Vector2D cellPos = sielement->localPositionOfCell(tmpId);  // w/ Lorentz correction
      //           const Amg::Vector2D deltaPos = *locPos - cellPos; // delta w.r.t. center of cell 
      //           (*m_oflraw) << "e\t" 
      //                  << setw(14) << setprecision(10)
      //                  << global_pos[0] << '\t' << global_pos[1] << '\t' << global_pos[2] << '\t'
      //                  << resetiosflags(ios::scientific)
      //                  << 1 << '\t'
      //                  << m_pixelId->barrel_ec(rdoId) << '\t'
      //                  << m_pixelId->layer_disk(rdoId) << '\t'
      //                  << m_pixelId->phi_module(rdoId) << '\t'
      //                  << m_pixelId->eta_module(rdoId) << '\t'
      //                  << m_pixelId->phi_index(tmpId) << '\t'  // use tmpId here
      //                  << m_pixelId->eta_index(tmpId) << '\t'
      //                  << setiosflags(ios::fixed) << setprecision(4)                   
      //                  << deltaPos[0] << '\t' << deltaPos[1] << '\t' // delta w.r.t. center of cell (Lorentz correction cancels out)
      //                  << resetiosflags(ios::scientific)
      //                  << pHitRdoList->size();
      //           for( vector<Identifier>::const_iterator pAssocHit=pHitRdoList->begin(), fAssocHit=pHitRdoList->end(); pAssocHit!=fAssocHit; ++pAssocHit ) {
      //             (*m_oflraw) << '\t' << hitIndexMap.find(*pAssocHit)->second;
      //           }
      //           (*m_oflraw) << '\n';
      //         } // end if hit is pixel, dump
      //       } // end for each hit on this track
    } // end dump hits on this track
  } // end for each reconstructed track
  if( !(trks->empty()) ) { ATH_MSG_DEBUG( "done dumping reconstructed tracks."); }
}

const DumpSp::ParentBitmask
DumpSp::construct_truth_bitmap( const HepMC::GenParticle* particle ) const
{
  ParentBitmask result;
  result.reset();
  typedef pair<const HepMC::GenParticle*,unsigned int> Parent;
  vector<Parent> parents;
  parents.push_back( Parent(particle,0) );
  while( !parents.empty() ) {
    const HepMC::GenParticle* p = parents.back().first;
    const unsigned int level = parents.back().second;
    if( std::abs(p->pdg_id())==15 ) { result.set( TAU_PARENT_BIT , 1 ); }
    if( std::abs(p->pdg_id())==5 ) { result.set( B_PARENT_BIT , 1 ); }
    if( std::abs(p->pdg_id())==211 ) { result.set( PION_PARENT_BIT , 1 ); }
    if( std::abs(p->pdg_id())==211 && level<=1 ) { result.set( PION_IMMEDIATE_PARENT_BIT , 1 ); }
    if( result.count()==NBITS ) { break; }
    parents.pop_back();
    if( !(p->production_vertex()) ) { continue; }
    for( HepMC::GenVertex::particle_iterator i=p->production_vertex()->particles_begin(HepMC::parents), f=p->production_vertex()->particles_end(HepMC::parents); i!=f; ++i ) {
      parents.push_back( Parent(*i,level+1) );
    }
  }
  return result;
}


void
DumpSp::dump_beamspot() const
{
  if ( m_outputBeamSpotToWrapper ) { // output beam spot to wrapper
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    (*m_oflraw) << "V\t" << beamSpotHandle->beamPos().x() << "\t" 
              << beamSpotHandle->beamPos().y() << "\t"
              << beamSpotHandle->beamPos().z() << "\t"
              << beamSpotHandle->beamSigma(0) << "\t"
              << beamSpotHandle->beamSigma(1) << "\t"
              << beamSpotHandle->beamSigma(2) << "\t"
              << beamSpotHandle->beamSigmaXY() << "\t" 
              << beamSpotHandle->beamTilt(0) << "\t"
              << beamSpotHandle->beamTilt(1) << "\n";
  }
}

void
DumpSp::dump_MBTS( ) const
{
  // variables needed
  //  number of vertices, MBTS trigger, Jinlong triggers (L1_MBTS_1 or L1_MBTS_1_1)
  //  BC ID, collision timing, etc.
  string chains_regex(".*(mb|MB).*");
  vector<string> chains = m_trigDecTool->getChainGroup(chains_regex)->getListOfTriggers();
  int nChains = chains.size();
  cout << nChains << " mb chains found." << endl;  
  BOOST_FOREACH( const string& chain , chains ) { 
    cout << " chain: " << chain << " ";
    const bool ok_physics = m_trigDecTool->isPassed( chain , TrigDefs::Physics );
    const bool ok_passthru = m_trigDecTool->isPassed( chain , TrigDefs::passedThrough );
    cout << " " << ok_physics << " " << ok_passthru << endl;
  }
  
  const bool ok_physics_A = m_trigDecTool->isPassed( "L1_MBTS_1_1_Col" , TrigDefs::Physics );
  const bool ok_passthru_A = m_trigDecTool->isPassed( "L1_MBTS_1_1_Col" , TrigDefs::passedThrough );
  const bool ok_physics_B = m_trigDecTool->isPassed( "L1_MBTS_1_1_EMPTY" , TrigDefs::Physics );
  const bool ok_passthru_B = m_trigDecTool->isPassed( "L1_MBTS_1_1_EMPTY" , TrigDefs::passedThrough );

  const bool ok_physics_C = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPARIED" , TrigDefs::Physics );
  const bool ok_passthru_C = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPAIRED" , TrigDefs::passedThrough );
  const bool ok_physics_D = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPARIED1" , TrigDefs::Physics );
  const bool ok_passthru_D = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPAIRED1" , TrigDefs::passedThrough );
  const bool ok_physics_E = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPARIED2" , TrigDefs::Physics );
  const bool ok_passthru_E = m_trigDecTool->isPassed( "L1_MBTS_1_1_UNPAIRED2" , TrigDefs::passedThrough );
  
  const bool ok_physics_F = m_trigDecTool->isPassed( "L1_MBTS_1_1" , TrigDefs::Physics );
  const bool ok_passthru_F = m_trigDecTool->isPassed( "L1_MBTS_1_1" , TrigDefs::passedThrough );
  const bool ok_physics_G = m_trigDecTool->isPassed( "L1_MBTS_1" , TrigDefs::Physics );
  const bool ok_passthru_G = m_trigDecTool->isPassed( "L1_MBTS_1" , TrigDefs::passedThrough );
  
  // eXtra info
  // mbts trigger bits
  (*m_oflraw) << "X 1 " 
	    << ok_physics_A << " " << ok_passthru_A << " "
	    << ok_physics_B << " " << ok_passthru_B << " "
	    << ok_physics_C << " " << ok_passthru_C << " "
	    << ok_physics_D << " " << ok_passthru_D << " "
	    << ok_physics_E << " " << ok_passthru_E << " "
	    << ok_physics_F << " " << ok_passthru_F << " "
	    << ok_physics_G << " " << ok_passthru_G << " "
	    << endl;
 
  return;
}



void
DumpSp::dump_lumibcid( const EventID* evid) const
{

  const int lb = evid->lumi_block();
  const int bcid = evid->bunch_crossing_id();
  (*m_oflraw) << "X 2 " 
            << lb << " " << bcid 
            << endl;
  
  return;
}

void
DumpSp::dump_vertex(  ) const
{

  // number of z vertices
  const VxContainer* vxes( 0 );
  if( m_storeGate->retrieve( vxes, "VxPrimaryCandidate" ).isSuccess() ) {
    if( !(vxes->empty()) ) {
      for( VxContainer::const_iterator i=vxes->begin(), f=vxes->end(); i!=f; ++i ) {
        const unsigned int vx_type = (*i)->vertexType();
        const unsigned int vx_ntracks = (*i)->vxTrackAtVertex()->size();
        const double vx_x = (*i)->recVertex().position().x();
        const double vx_y = (*i)->recVertex().position().y();
        const double vx_z = (*i)->recVertex().position().z();
        (*m_oflraw) << "X 3 "
		  << vx_type << " "
		  << vx_ntracks << " "
		  << vx_x << " "
		  << vx_y << " "
		  <<   vx_z << " "
		  << endl;
      }
    }
  }
  return;
  
}

