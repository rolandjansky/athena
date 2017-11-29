/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_SGHitInput.h"

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

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

using namespace std;

FTK_SGHitInput::FTK_SGHitInput(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc),
  FTKDataInput(),
  m_log( msgSvc() , name ),
  m_truthToTrack( "Trk::TruthToTrack/InDetTruthToTrack" ),
  m_extrapolator( "Trk::Extrapolator/AtlasExtrapolator" ),
  m_beamSpotSvc( "BeamCondSvc" , name ),
  m_pixelClustersName( "PixelClusters" ),
  m_sctClustersName( "SCT_Clusters" ),
  m_pixelSpacePointsName( "PixelSpacePoints" ),
  m_sctSpacePointsName( "SCT_SpacePoints" ),
  m_maxEta(3.3),
  m_minPt(.8*CLHEP::GeV),
  m_outFileNameRawHits( "ftksim_raw_hits.dat.bz2" ),
  m_readTruthTracks(false),
  m_UseNominalOrigin(false),
  m_dooutFileRawHits(false),
  ofl(),
  oflraw()
{
  declareInterface<FTK_SGHitInputI>(this);

  declareProperty("maxEta",                   m_maxEta);
  declareProperty("minPt",                    m_minPt);
  declareProperty("pixelClustersName",        m_pixelClustersName);
  declareProperty("SCT_ClustersName",         m_sctClustersName);
  declareProperty("overlapSpacePointsName",   m_overlapSpacePointsName);
  declareProperty("pixelSpacePointsName",     m_pixelSpacePointsName);
  declareProperty("sctSpacePointsName",       m_sctSpacePointsName);
  declareProperty("OutFileNameRawHits",       m_outFileNameRawHits);
  declareProperty("dumpHitsOnTracks",         m_dumpHitsOnTracks);
  declareProperty("dumpSpacePoints",          m_dumpSpacePoints);
  declareProperty("dumpTruthIntersections",   m_dumpTruthIntersections);
  declareProperty("tracksTruthName"         , m_tracksTruthName);
  declareProperty("TruthToTrackTool"        , m_truthToTrack);
  declareProperty("Extrapolator"            , m_extrapolator);
  declareProperty("BeamCondSvc"             , m_beamSpotSvc);
  declareProperty("useOfflineTrackSelectorTool" , m_useOfflineTrackSelectorTool);
  declareProperty("outputBeamSpotToWrapper"	, m_outputBeamSpotToWrapper);
  declareProperty("useSimpleCuts"		, m_useSimpleCuts);
  declareProperty("logBeamSpotOutput"		, m_logBeamSpotOutput);
  declareProperty("DoOutFileRawHits"            , m_dooutFileRawHits);
  declareProperty("ReadTruthTracks", m_readTruthTracks);
  declareProperty("UseNominalOrigin", m_UseNominalOrigin);
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_SGHitInput::initialize(){

  m_log << MSG::INFO << "FTK_SGHitInput::initialize()" << endmsg;

  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endmsg;
    return StatusCode::FAILURE;
  }

  if( m_truthToTrack.retrieve().isFailure() ) {
    m_log << MSG::FATAL << m_truthToTrack << " truth to track tool not found" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_log << MSG::INFO << m_truthToTrack << " retrieved" << endmsg;
  }

  if( m_extrapolator.retrieve().isFailure() ) {
    m_log << MSG::FATAL << m_extrapolator << " extrapolator tool not found" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_log << MSG::INFO << m_extrapolator << " retrieved" << endmsg;
  }

  if( m_beamSpotSvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << m_beamSpotSvc << " beam spot service not found" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_log << MSG::INFO << m_beamSpotSvc << " retrieved" << endmsg;
  }

  if( service("DetectorStore",m_detStore).isFailure() ) {
    m_log << MSG::FATAL <<"DetectorStore service not found" << endmsg;
    return StatusCode::FAILURE;
  }

  IPartPropSvc* partPropSvc = 0;
  if( service("PartPropSvc", partPropSvc, true).isFailure() ) {
    m_log << MSG::FATAL << "particle properties service unavailable" << endmsg;
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  // ID helpers
  m_idHelper = new AtlasDetectorID;
  const IdDictManager* idDictMgr( 0 );
  if( m_detStore->retrieve(idDictMgr, "IdDict").isFailure() || !idDictMgr ) {
    m_log << MSG::ERROR << "Could not get IdDictManager !" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }

  // open output to .bz2 using streams for debug //
  if(m_dooutFileRawHits) {
    oflraw.reset( new boost::iostreams::filtering_ostream );
    if( !oflraw ) { return StatusCode::FAILURE; }
    if( boost::algorithm::icontains(m_outFileNameRawHits,".bz2" ) ) {
      boost::iostreams::bzip2_params params;
      params.block_size = 9;
      oflraw->push( boost::iostreams::bzip2_compressor(params) );
    }
    oflraw->push( boost::iostreams::file_sink(m_outFileNameRawHits)); // open the file
  }


  return StatusCode::SUCCESS;
}


StatusCode FTK_SGHitInput::finalize(){
  MsgStream log(msgSvc(), name());
  m_log << MSG::INFO << "finalized" << endmsg;

  if( m_idHelper ) { delete m_idHelper; }

  return StatusCode::SUCCESS;
}


/** initilize for the input */
int FTK_SGHitInput::init(bool*)
{
  // setup clustering
  initClustering();
  return 0;
}



/** This function get from the SG the inner detector raw hits
  and prepares them for FTK simulation */
int FTK_SGHitInput::readData()
{
  int res(0); // result of the hit gathering

  // reset internal containers
  m_original_hits.clear();

  const EventInfo* eventInfo(0);
  if( m_storeGate->retrieve(eventInfo).isFailure() ) {
    m_log << MSG::ERROR << "Could not retrieve event info" << endmsg;
    return -1;
  }
  const EventID* eventID( eventInfo->event_ID() );
  //defult is DEBUG
  m_log << MSG::INFO
        << "entered execution for run " << eventID->run_number()
        << "   event " << eventID->event_number()
        << endmsg;

  const TriggerInfo *triggerInfo(eventInfo->trigger_info());

  //Filled to variable / start event
  setRunNumber(eventID->run_number());
  setEventNumber(eventID->event_number());
  setLB(eventID->lumi_block());
  setBCID(eventID->bunch_crossing_id());
  setaverageInteractionsPerCrossing(eventInfo->averageInteractionsPerCrossing());
  setactualInteractionsPerCrossing(eventInfo->actualInteractionsPerCrossing());
  setextendedLevel1ID(triggerInfo->extendedLevel1ID() );

  setlevel1TriggerType(triggerInfo->level1TriggerType () );
  setlevel1TriggerInfo(triggerInfo->level1TriggerInfo ());

  if(m_dooutFileRawHits){
    (*oflraw) << "R\t" << eventID->run_number()<<'\n';
    (*oflraw) << "F\t" << eventID->event_number()<<'\n';
  }



  HitIndexMap hitIndexMap; // keep running index event-unique to each hit
  HitIndexMap pixelClusterIndexMap;
  // get pixel and sct cluster containers
  if( m_storeGate->retrieve(m_pixelContainer, m_pixelClustersName).isFailure() ) {
    m_log << MSG::WARNING << "unable to retrieve the PixelCluster container " << m_pixelClustersName << endmsg;
  }
  if( m_storeGate->retrieve(m_sctContainer, m_sctClustersName).isFailure() ) {
    m_log << MSG::WARNING << "unable to retrieve the SCT_Cluster container " << m_sctClustersName << endmsg;
  }


  // dump raw silicon data
  m_log << MSG::INFO << "Dump raw silicon data" << endmsg;
  read_raw_silicon( hitIndexMap, pixelClusterIndexMap );

  // *** add other variables in the future (ex) SpacePoint etc... see Dump.cxx *** //
  // dump truth tracks
  if(m_readTruthTracks) {
    m_log << MSG::INFO << "Dump truth tracks" << endmsg;
    m_truth_track.clear();
    read_truth_tracks();
  }
  // event footer
  if(m_dooutFileRawHits)(*oflraw) << "L\t" << eventID->event_number()<<'\n';



  return res;
}



// dump silicon channels with geant matching information.
void
FTK_SGHitInput::read_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap ) // const cannot make variables push back to DataInput
{
  using namespace std;
  unsigned int hitIndex = 0u;
  unsigned int pixelClusterIndex=0;
  const DataHandle<PixelRDO_Container> pixel_rdocontainer_iter;
  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = m_storeGate->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    m_log << MSG::WARNING << "Missing Pixel SDO Map" << endmsg;
  }
  else {
    m_log << MSG::INFO << "Found Pixel SDO Map" << endmsg;
  }

  // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
  FTKRawHit tmpSGhit;
  //tmporary debug to output variables to text file for developer.


  if( m_storeGate->retrieve(pixel_rdocontainer_iter, "PixelRDOs").isSuccess()  ) {
    pixel_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( PixelRDO_Container::const_iterator iColl=pixel_rdocontainer_iter->begin(), fColl=pixel_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
      if( !pixel_rdoCollection ) { continue; }
      const int size = pixel_rdoCollection->size();
      m_log << MSG::DEBUG << "Pixel InDetRawDataCollection found with " << size << " RDOs" << endmsg;
      // loop on all RDOs
      for( DataVector<PixelRDORawData>::const_iterator iRDO=pixel_rdoCollection->begin(), fRDO=pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO ) {
        Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement);
        const InDetDD::SiLocalPosition localPos = sielement->localPositionOfCell(rdoId);
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

        //output//
        if(m_dooutFileRawHits){ //bz2 file
          (*oflraw) << "S\t"
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
                    << endl;
        }
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
        tmpSGhit.reset();
        tmpSGhit.setX(gPos.x());
        tmpSGhit.setY(gPos.y());
        tmpSGhit.setZ(gPos.z());
        tmpSGhit.setHitType(ftk::PIXEL);

	bool isIBL = (m_pixelId->barrel_ec(rdoId) == 0 && m_pixelId->layer_disk(rdoId) == 0) ? true : false;
	bool isIBL3D = (isIBL && FTKSetup::getFTKSetup().getIBLMode() == 2 && 
			(m_pixelId->eta_module(rdoId) <= -7 || m_pixelId->eta_module(rdoId) >= 6)) ? true : false;

	if (isIBL3D)
	  tmpSGhit.setModuleType(ftk::MODULETYPE_IBL3D);
	else if (isIBL)
	  tmpSGhit.setModuleType(ftk::MODULETYPE_IBL_PLANAR);
	else
	  tmpSGhit.setModuleType(ftk::MODULETYPE_PIXEL);
	
        tmpSGhit.setIdentifierHash(sielement->identifyHash());
        tmpSGhit.setBarrelEC(m_pixelId->barrel_ec(rdoId));
        tmpSGhit.setLayer( m_pixelId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_pixelId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_pixelId->eta_module(rdoId));
        tmpSGhit.setPhiSide(m_pixelId->phi_index(rdoId));
        tmpSGhit.setEtaStrip(m_pixelId->eta_index(rdoId));
        tmpSGhit.setNStrips((*iRDO)->getToT());
        tmpSGhit.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parent_mask.to_ulong());
        tmpSGhit.normalizeLayerID(); // Change layer info from atlas geo to FTK geo
        //cout << "DBGBC Pxl " << tmpSGhit.getEventIndex() << ", " << tmpSGhit.getBarcode() << endl;
        if (abs(m_pixelId->barrel_ec(rdoId)) != 4) // JAAA skip diamonds!
          m_original_hits.push_back(tmpSGhit);
      } // end for each RDO in the collection
    } // for each pixel RDO collection
    // dump all pixel RDO's and SDO's for debugging purposes
    if( false ) {
      for( PixelRDO_Container::const_iterator iColl=pixel_rdocontainer_iter->begin(), fColl=pixel_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
        const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
        if( !pixel_rdoCollection ) { continue; }
        for( DataVector<PixelRDORawData>::const_iterator iRDO=pixel_rdoCollection->begin(), fRDO=pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO ) {
          Identifier rdoId = (*iRDO)->identify();
          // get the det element from the det element collection
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement);
          const InDetDD::SiLocalPosition localPos = sielement->localPositionOfCell(rdoId);
          const InDetDD::SiLocalPosition rawPos = sielement->rawLocalPositionOfCell(rdoId);
          const Amg::Vector3D gPos( sielement->globalPosition(localPos) );
          if(m_dooutFileRawHits){
            (*oflraw) << "# S\t"
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
          }
        } // end for each pixel RDO
      } // end for each pixel RDO collection
      // dump SDO's
      if( have_pixel_sdo && pixelSimDataMap ) {
        for( InDetSimDataCollection::const_iterator i=pixelSimDataMap->begin(), f=pixelSimDataMap->end(); i!=f; ++i ) {
          const Identifier sdoId( i->first );
          const InDetSimData& sdo( i->second );
          const vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
          (*oflraw) << "# s"
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
            (*oflraw) << "# s q " << qdep << " " << particleLink.isValid() << " "
                      << (particleLink.isValid() ? particleLink.eventIndex() : -1)
                      << (particleLink.isValid() ? particleLink.barcode() : -1)
                      << endl;
          } // end for each deposit in SDO
        } // end for each pixel SDO
      } // if have sdo info
    } // end dump RDO's and SDO's for debugging purposes
  } // dump raw pixel data
  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = m_storeGate->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  if (!have_sct_sdo) {
    m_log << MSG::WARNING << "Missing SCT SDO Map" << endmsg;
  }
  else {
    m_log << MSG::INFO << "Found SCT SDO Map" << endmsg;
  }

  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( m_storeGate->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      const int size = SCT_Collection->size();
      m_log << MSG::DEBUG << "SCT InDetRawDataCollection found with " << size << " RDOs" << endmsg;
      for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
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
        if(m_dooutFileRawHits){
          (*oflraw) << "S\t"
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
                    << '\n';
        }
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
        tmpSGhit.reset();
        tmpSGhit.setX(gPos.x());
        tmpSGhit.setY(gPos.y());
        tmpSGhit.setZ(gPos.z());
        tmpSGhit.setHitType(ftk::SCT);
        tmpSGhit.setModuleType(ftk::MODULETYPE_SCT);
        tmpSGhit.setIdentifierHash(sielement->identifyHash());
        tmpSGhit.setBarrelEC(m_sctId->barrel_ec(rdoId));
        tmpSGhit.setLayer(m_sctId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_sctId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_sctId->eta_module(rdoId));
        tmpSGhit.setPhiSide(m_sctId->side(rdoId));
        tmpSGhit.setEtaStrip(m_sctId->strip(rdoId));
        tmpSGhit.setNStrips((*iRDO)->getGroupSize());
        tmpSGhit.setEventIndex((long)(best_parent ? best_extcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(best_parent ? best_extcode.barcode() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parent_mask.to_ulong());
        tmpSGhit.normalizeLayerID();  // Change layer info from atlas geo to FTK geo
        //cout << "DBGBC SCT " << tmpSGhit.getEventIndex() << ", " << tmpSGhit.getBarcode() << endl;
        m_original_hits.push_back(tmpSGhit);
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
    if( false ) {
      // dump SCT RDO's
      for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
        const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
        if( !SCT_Collection ) { continue; }
        const int size = SCT_Collection->size();
        m_log << MSG::DEBUG << "SCT InDetRawDataCollection found with " << size << " RDOs" << endmsg;
        for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
          const Identifier rdoId = (*iRDO)->identify();
          const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
          const InDetDD::SCT_ModuleSideDesign& design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(sielement->design());
          const InDetDD::SiLocalPosition localPos = design.positionFromStrip(m_sctId->strip(rdoId));
          const Amg::Vector3D gPos = sielement->globalPosition(localPos);

          if(m_dooutFileRawHits){
            (*oflraw) << "# S\t"
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
          }
        } // end for each SCT rdo
      }  // end for each SCT rdo collection
      // dump SCT SDO's
      if( have_sct_sdo && sctSimDataMap ) {
        for( InDetSimDataCollection::const_iterator i=sctSimDataMap->begin(), f=sctSimDataMap->end(); i!=f; ++i ) {
          const Identifier sdoId( i->first );
          const InDetSimData& sdo( i->second );
          const vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
          if(m_dooutFileRawHits){
            (*oflraw) << "# s"
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
          }
          for( vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
            const HepMcParticleLink& particleLink( iDep->first );
            const InDetSimData::Deposit::second_type qdep( iDep->second ); // energy(charge) contributed by this particle
            if(m_dooutFileRawHits){
              (*oflraw) << "# s q " << qdep << " " << particleLink.isValid() << " "
                        << (particleLink.isValid() ? particleLink.eventIndex() : -1)
                        << (particleLink.isValid() ? particleLink.barcode() : -1)
                        << endl;
            }
          } // end for each deposit associated with this SDO
        } // end for each SCT SDO
      }  // end if SDO's available, dump them
    } // end dump all RDO's and SDO's for a given event
  } // end dump raw SCT data

  // FlagJT dump pixel clusters. They're in m_pixelContainer
  m_pixelContainer->clID(); // anything to dereference the DataHandle
  for( InDet::SiClusterContainer::const_iterator iColl=m_pixelContainer->begin(), fColl=m_pixelContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection(*iColl);
    if( !pixelClusterCollection ) {
      m_log << MSG::DEBUG << "pixelClusterCollection not available!" << endmsg;
      continue;
    }
    const int size = pixelClusterCollection->size();
    m_log << MSG::DEBUG << "PixelClusterCollection found with " << size << " clusters" << endmsg;

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
          assert( sielement);
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

      if(m_dooutFileRawHits){
        (*oflraw) << "P\t"
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
                  // Cluster width:
                  << (*iCluster)->width().colRow().x() << '\t' // width in phi?
                  << (*iCluster)->width().colRow().y() << '\t' // width in eta?
                  << (*iCluster)->rdoList().size() << '\t' // number of pixels in cluster
                  // Cluster truth
                  << (long)(best_parent ? best_extcode.eventIndex() : -1) << '\t'
                  << (long)(best_parent ? best_extcode.barcode() : -1) << '\t'
                  << setprecision(5) << static_cast<unsigned long>(std::ceil(best_parent ? best_parent->momentum().perp() : 0.)) << '\t' // particle pt in MeV
                  << parent_mask.to_ulong()
                  << endl;
      }
      pixelClusterIndexMap[theId] = pixelClusterIndex;
      pixelClusterIndex++;
    } // End loop over pixel clusters
  } // End loop over pixel cluster collection

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



}

const FTK_SGHitInput::ParentBitmask
FTK_SGHitInput::construct_truth_bitmap( const HepMC::GenParticle* particle ) const
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
FTK_SGHitInput::read_truth_tracks()
{
  FTKTruthTrack tmpSGTrack;

  // retrieve truth tracks from athena
  const McEventCollection* SimTracks = 0;
  if( m_storeGate->retrieve(SimTracks,"TruthEvent").isFailure() ) {
    string key = "G4Truth";
    if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
      key = "";
      if( m_storeGate->retrieve(SimTracks,key).isFailure() ) {
        m_log << MSG::DEBUG << "could not find the McEventCollection"<< endmsg;
        return;
      }
    }
  }
  // for debugging the calculation of d0_corr
  bool showd0corrSuccess = true;

  // dump each truth track

  for( unsigned int ievt=0, fevt=(SimTracks ? SimTracks->size() : 0u); ievt!=fevt; ++ievt ) {
    const HepMC::GenEvent* genEvent = SimTracks->at( ievt );
    // retrieve the primary interaction vertex here. for now, use the dummy origin.
    HepGeom::Point3D<double>  primaryVtx(0.,0.,0.);
    // the event should have signal process vertex unless it was generated as single particles.
    // if it exists, use it for the primary vertex.
    if( genEvent->signal_process_vertex() ) {
      primaryVtx.set( genEvent->signal_process_vertex()->point3d().x(),
          genEvent->signal_process_vertex()->point3d().y(),
          genEvent->signal_process_vertex()->point3d().z() );
      m_log << MSG::DEBUG <<"using signal process vertex for eventIndex " << ievt << ":"
            << primaryVtx.x() << "\t" << primaryVtx.y()  << "\t" << primaryVtx.z()
            <<endmsg;
    }


    for( HepMC::GenEvent::particle_const_iterator it=genEvent->particles_begin(), ft=genEvent->particles_end(); it!=ft; ++it ) {


      const HepMC::GenParticle* const particle( *it );
      const int pdgcode = particle->pdg_id();
      // reject generated particles without a production vertex.
      if( !particle->production_vertex() ) { continue; }
      // reject neutral or unstable particles
      const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));
      if( !pd ) { continue; }
      //       const float charge = pd->charge();
      ///       const float charge = (pdgcode > 0) ? -1. : +1.;
      float charge = pd->charge();
      if (pdgcode < 0) charge *= -1.; // since we took absolute value above

      if( std::abs(charge)<0.5 ) { continue; }
      if( particle->status()%1000!=1 ) { continue; }
      // retrieve some particle kinematics
      //const float genPt = particle->momentum().perp()/1000.; // convert to pt in GeV.
      //const float genEta = particle->momentum().pseudoRapidity();
      // reject non-fiducial particles
      //if( (genPt*1000.) < m_minPt ) { continue; } // controled by TrigFTKBankGen

      //if( std::abs(genEta) > m_maxEta ) { continue; } controled by TrigFTKBankGen

      // new truth-to-track tool
      const Amg::Vector3D momentum( particle->momentum().px(), particle->momentum().py(), particle->momentum().pz());
      const Amg::Vector3D position( particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
      const Trk::CurvilinearParameters cParameters( position, momentum, charge);

      Trk::PerigeeSurface persf;
      if (m_UseNominalOrigin) {
	Amg::Vector3D    origin(0,0,0);
	persf = Trk::PerigeeSurface(origin);
      }
      else {
	persf = m_beamSpotSvc->beamPos();
      }

      const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters, persf, Trk::anyDirection, false);

      const double m_track_truth_d0 = tP ? tP->parameters()[Trk::d0] : 999.;
      const double m_track_truth_phi = tP ? tP->parameters()[Trk::phi] : 999.;
      const double m_track_truth_p = (tP && fabs(tP->parameters()[Trk::qOverP]) > 1.e-8) ?
        tP->charge()/tP->parameters()[Trk::qOverP] : 10E7;
      const double m_track_truth_x0 = tP ? tP->position().x() : 999.;
      const double m_track_truth_y0 = tP ? tP->position().y() : 999.;
      //const double m_track_truth_z0 = tP ? tP->position().z() : 999.;
      const double m_track_truth_z0 = tP ? tP->parameters()[Trk::z0] : 999.;
      const double m_track_truth_q = tP ? tP->charge() : 0.;
      const double m_track_truth_sinphi = tP ? std::sin(tP->parameters()[Trk::phi]) : -1.;
      const double m_track_truth_cosphi = tP ? std::cos(tP->parameters()[Trk::phi]) : -1.;
      const double m_track_truth_sintheta = tP ? std::sin(tP->parameters()[Trk::theta]) : -1.;
      const double m_track_truth_costheta = tP ? std::cos(tP->parameters()[Trk::theta]) : -1.;
      double truth_d0corr = m_track_truth_d0-( primaryVtx.y()*cos(m_track_truth_phi)-primaryVtx.x()*sin(m_track_truth_phi) );

      // old truth-to-track tool: retrieve truth track parameters at perigee
      // boost::scoped_ptr<const Trk::TrackParameters> generatedTrackPerigee( m_truthToTrack->makePerigeeParameters(particle) );
      // const double m_track_truth_d0 = generatedTrackPerigee ? generatedTrackPerigee->parameters()[Trk::d0] : 999.;
      // const double m_track_truth_phi = generatedTrackPerigee ? generatedTrackPerigee->parameters()[Trk::phi0] : 999.;
      // const double m_track_truth_p = (generatedTrackPerigee && generatedTrackPerigee->parameters()[Trk::qOverP] != 0.) ?
      //   generatedTrackPerigee->charge()/generatedTrackPerigee->parameters()[Trk::qOverP] : 10E7;
      // const double m_track_truth_x0 = generatedTrackPerigee ? generatedTrackPerigee->position().x() : 999.;
      // const double m_track_truth_y0 = generatedTrackPerigee ? generatedTrackPerigee->position().y() : 999.;
      // const double m_track_truth_z0 = generatedTrackPerigee ? generatedTrackPerigee->position().z() : 999.;
      // const double m_track_truth_q = generatedTrackPerigee ? generatedTrackPerigee->charge() : 0.;
      // const double m_track_truth_sinphi = generatedTrackPerigee ? std::sin(generatedTrackPerigee->parameters()[Trk::phi0]) : -1.;
      // const double m_track_truth_cosphi = generatedTrackPerigee ? std::cos(generatedTrackPerigee->parameters()[Trk::phi0]) : -1.;
      // const double m_track_truth_sintheta = generatedTrackPerigee ? std::sin(generatedTrackPerigee->parameters()[Trk::theta]) : -1.;
      // const double m_track_truth_costheta = generatedTrackPerigee ? std::cos(generatedTrackPerigee->parameters()[Trk::theta]) : -1.;
      // double truth_d0corr = m_track_truth_d0-( primaryVtx.y()*cos(m_track_truth_phi)-primaryVtx.x()*sin(m_track_truth_phi) );
      double truth_zvertex = 0.;

      if ( !m_useSimpleCuts ) {  // determine d0_corr based on beam position from BeamCondSvc
        //toshi/// truth_d0corr = m_track_truth_d0-( m_beamCondSvc->beamPos().y()*cos(m_track_truth_phi)-m_beamCondSvc->beamPos().x()*sin(m_track_truth_phi) );
        //toshi//truth_zvertex = m_beamCondSvc->beamPos().z();
        if ( showd0corrSuccess ) {
          m_log << MSG::DEBUG << "Beamspot from BeamCondSvc used to determine cuts in dump_truth()"<< endmsg;
          showd0corrSuccess = false;
        }
      }
      const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
      // categorize particle (prompt, secondary, etc.) based on InDetPerformanceRTT/detector paper criteria.
      bool isPrimary = true;
      bool isDetPaperCut = true;
      if( std::abs(truth_d0corr)>2. ) { isPrimary=false;}
      if( particle->barcode()>100000 || particle->barcode()==0 ) { isPrimary=false;}
      isDetPaperCut=isPrimary;
      // debug line for production_vertex variable
      if( false ) { m_log << MSG::DEBUG << "z correction -- bool particle->production_vertex() = " << particle->production_vertex()<< endmsg; }
      //
      if( isPrimary && particle->production_vertex() ) {
        const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
        if( std::abs(startVertex.z() - truth_zvertex)>100. ) { isPrimary=false;}
        // debug z vertex correction
        if( false ) { m_log << MSG::DEBUG << "z correction -- startVertex.z() = " << startVertex.z() << " , truth_zvertex = "<< truth_zvertex<< endmsg; }
        //double flight_length = -1.;
        if( particle->end_vertex() ) {
          HepGeom::Point3D<double> endVertex(particle->end_vertex()->point3d().x(), particle->end_vertex()->point3d().y(), particle->end_vertex()->point3d().z());
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
      /*if( !_ttrProbMap.empty() ) {
        TruthToRecoProbMap::const_iterator barcode=_ttrProbMap.find(extBarcode2);
        if( barcode!=_ttrProbMap.end() ) {
        vector<float> probs;
        transform( _ttrProbMap.lower_bound(extBarcode2) , _ttrProbMap.upper_bound(extBarcode2) , back_inserter(probs) ,
        boost::bind(&TruthToRecoProbMap::value_type::second,_1) );
        vector<float>::const_iterator imax = max_element(probs.begin(),probs.end());
        assert( imax!=probs.end() );
        precmatch = *imax;
        TruthToRecoTrackMap::const_iterator ibestrec = _ttrTrackMap.find( extBarcode2 );
        assert( ibestrec!=_ttrTrackMap.end() );
        irecmatch = ibestrec->second;
        }
        }*///toshi  comment out
      ParentBitmask parent_mask( construct_truth_bitmap( particle ) );
      if(m_dooutFileRawHits){
        (*oflraw) << setiosflags(ios::scientific) << "T\t"
                  << setw(14) << setprecision(10) << m_track_truth_x0 << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_y0 << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_z0 << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_d0 << '\t'
                  << setw(14) << setprecision(10) << primaryVtx.z() << '\t'
                  << (int)m_track_truth_q << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_p*(m_track_truth_cosphi*m_track_truth_sintheta) << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_p*(m_track_truth_sinphi*m_track_truth_sintheta) << '\t'
                  << setw(14) << setprecision(10) << m_track_truth_p*(m_track_truth_costheta) << '\t'
                  << pdgcode << '\t'
                  << setw(14) << (int)irecmatch << '\t'
                  << setw(14) << setprecision(10) << precmatch << '\t'
                  << extBarcode2.eventIndex() << '\t'
                  << extBarcode2.barcode() << '\t'
                  << parent_mask.to_ulong() << '\t'
                  << isDetPaperCut << '\t'
                  << resetiosflags(ios::scientific) << '\n';
      }
      //get truth track info

      tmpSGTrack.setX(m_track_truth_x0);
      tmpSGTrack.setY(m_track_truth_y0);
      tmpSGTrack.setZ(m_track_truth_z0);
      tmpSGTrack.setD0(m_track_truth_d0);
      tmpSGTrack.setVtxZ(primaryVtx.z());
      tmpSGTrack.setQ(m_track_truth_q);
      tmpSGTrack.setPX(m_track_truth_p*(m_track_truth_cosphi*m_track_truth_sintheta));
      tmpSGTrack.setPY(m_track_truth_p*(m_track_truth_sinphi*m_track_truth_sintheta));
      tmpSGTrack.setPZ(m_track_truth_p*m_track_truth_costheta);
      tmpSGTrack.setPDGCode(pdgcode);
      tmpSGTrack.setBarcode(extBarcode2.barcode());
      tmpSGTrack.setEventIndex(extBarcode2.eventIndex());
      m_truth_track.push_back(tmpSGTrack);


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
