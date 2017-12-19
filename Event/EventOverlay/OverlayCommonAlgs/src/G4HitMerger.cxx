/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm to merge G4Hits from different subevents from pileup service to a single event

#include "G4HitMerger.h"
#include "LArSimEvent/LArHitContainer.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "TileSimEvent/TileHitVector.h"
//#include "GeneratorObjects/McEventCollection.h" // in G4HitMerger.h
#include <iomanip>


std::ostream&
operator<<(std::ostream& o, const McEventCollection& in)
{
  boost::io::ios_flags_saver ifs(o);
  o << "# GenEvents: " << in.size() << '\n';
  int count(0);
  for (McEventCollection::const_iterator evt = in.begin(); evt != in.end(); ++evt) {
    count++;
    o << " GenEvent " << count << ": signal_process_vertex " << (*evt)->signal_process_vertex() << " # tracks: " << (*evt)->particles_size() << " # vertices: " << (*evt)->vertices_size() << '\n';
    int iTrk(0);
    for ( HepMC::GenEvent::particle_iterator p = (*evt)->particles_begin(); p != (*evt)->particles_end(); ++p ) {
      iTrk++;
      const HepMC::FourVector fv((*p)->momentum().px(),
                                 (*p)->momentum().py(),
                                 (*p)->momentum().pz(),
                                 (*p)->momentum().e());
      o << "Track " << std::setw(3) << iTrk << " pdgId " << std::setw(5) << (*p)->pdg_id() << " barcode " << std::setw(6) << (*p)->barcode()
        << " 4-mom ( " << std::setprecision(7) << std::setw(10) << fv.px() << " , " << std::setw(10) <<  fv.py() << " , " << std::setw(10) << fv.pz() << ") pt " << fv.perp() << '\n';
    }
  }
  return o;
}

G4HitMerger::G4HitMerger(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_mergeSvc("PileUpMergeSvc",name)
{
  declareProperty("Detectors", m_Detectors, "Detector selection");
  declareProperty("PileUpMergeSvc", m_mergeSvc, "");
}

G4HitMerger::~G4HitMerger(){}

StatusCode G4HitMerger::initialize()
{

  // locate the PileUpMergeSvc and initialize our local ptr
  CHECK( m_mergeSvc.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully" );

  return StatusCode::SUCCESS;

}

StatusCode G4HitMerger::execute()
{

  /////////////////////////////////////////////////////////////////////////////////////
  // Append MCEventCollections

  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  //  TimedTruthList truthList;
  std::string truthCollKey("TruthEvent");

  ATH_MSG_INFO( " asking for container " << truthCollKey );

  TimedTruthList* truthList = new TimedTruthList;

  // retrieve the list of pairs (time, container) from the PileUp service
  if( m_mergeSvc->retrieveSubEvtsData(truthCollKey, *truthList).isFailure())
    {
      ATH_MSG_ERROR("execute(): Failed to retrieve " << truthCollKey);
      delete truthList;
      return StatusCode::FAILURE;
    }

  // start with a new (blank) McEventCollection

  McEventCollection* new_collection = new McEventCollection;
  if( evtStore()->record(new_collection, truthCollKey).isFailure() )
    {
      ATH_MSG_ERROR("execute(): Failed to record " << truthCollKey);
      delete truthList;
      delete new_collection;
      return StatusCode::FAILURE;
    }

  // loop over the list, adding the component McEventCollections to the new one
  //
  int McCounter(0);
  for (auto iter : *truthList ) {
    McCounter++;

    ATH_MSG_INFO( "Input McEventCollection " << McCounter << " " << *(iter.second) );

    // Surprisingly, McEventCollection::operator= pushes_back GenEvents from the RHS
    // onto the current list without first clearing the list first.
    // Thus, '=' is really an 'append' or += operation
    //
      (*new_collection) = *(iter.second);
  }
  delete truthList;

  ATH_MSG_INFO( "Merged McEventCollection  " << *(new_collection) );
  CHECK( evtStore()->setConst(new_collection) );



  //first get the list of McEventCollections
  //  StatusCode sc(StatusCode::FAILURE);
  //  if (! (sc = m_mergeSvc->retrieveSubEvtsData(truthCollKey, truthList)).isSuccess() ) {
  //    msglog<<MSG::ERROR<< "execute: Can not find TimedTruthList with key "<< truthCollKey << endmsg;
  //    return StatusCode::RECOVERABLE;;
  //  }
  //  int nInputMcEventColls(truthList.size());
  //  //and check it is not empty
  //  if (0 == nInputMcEventColls) {
  //    msglog<<MSG::ERROR<< "execute: TimedTruthList with key " << truthCollKey<< " is empty" << endmsg;
  //    return StatusCode::RECOVERABLE;
  //  }
  //
  //  TimedTruthList::iterator lI(truthList.begin()), lE(truthList.end());
  //  //start from the original event collection
  //  McEventCollection* pOvrlMcEvColl(new McEventCollection(*(lI->second)));
  //
  //  sc = evtStore()->record(pOvrlMcEvColl, truthCollKey);
  //  if (!sc.isSuccess()) msglog<<MSG::ERROR<<"Could not write MCEventCollection!"<<endmsg;



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for LAr hits
  m_HitContainer.clear();
  m_HitContainer.push_back("LArHitEMB");
  m_HitContainer.push_back("LArHitEMEC");
  m_HitContainer.push_back("LArHitHEC");
  m_HitContainer.push_back("LArHitFCAL");
  int nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) )
        {
          ATH_MSG_ERROR("execute(): Failed to retrieve " << truthCollKey);
          return StatusCode::FAILURE;
        }


      // make new hit container
      LArHitContainer *new_collection = new LArHitContainer();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const LArHitContainer& firstCont = *(iFirstCont->second);
        LArHitContainer::const_iterator f_cell=firstCont.begin();
        LArHitContainer::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          LArHit *hit = new LArHit(**f_cell);
          ++f_cell; nhit_tot++;
          float energy = hit->energy();
          Identifier cellId = hit->cellID();
          //if (energy<1e-6) continue;
          ATH_MSG_DEBUG("LArHit "<<cellId<<" has energy "<<energy);
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );

    } // end of loop over containers
  ATH_MSG_INFO( "Total number of LArHit found "  << nhit_tot );


  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for Tile hits
  m_HitContainer.clear();
  m_HitContainer.push_back("MBTSHits");
  m_HitContainer.push_back("TileHitVec");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<TileHitVector>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );

      // make new hit container
      TileHitVector *new_collection = new TileHitVector();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const TileHitVector& firstCont = *(iFirstCont->second);
        TileHitVector::const_iterator f_cell=firstCont.begin();
        TileHitVector::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const TileHit hit(*f_cell);
          ++f_cell; nhit_tot++;
          float energy = 0; for (int i=0; i<hit.size(); ++i) energy+=hit.energy(i);
          Identifier cellId = hit.identify();
          //if (energy<1e-6) continue;
          ATH_MSG_DEBUG("TileHit "<<cellId<<" has energy "<<energy);
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of TileHit found "  << nhit_tot );


  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for TRT hits
  m_HitContainer.clear();
  m_HitContainer.push_back("TRTUncompressedHits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<TRTUncompressedHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      TRTUncompressedHitCollection *new_collection = new TRTUncompressedHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const TRTUncompressedHitCollection& firstCont = *(iFirstCont->second);
        TRTUncompressedHitCollection::const_iterator f_cell=firstCont.begin();
        TRTUncompressedHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const TRTUncompressedHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          float energy = hit.GetEnergyDeposit();
          int cellId = hit.GetHitID();
          //if (energy<1e-6) continue;
          ATH_MSG_DEBUG("TRTUncompressedHit "<<cellId<<" has energy "<<energy);
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of TRTUncompressedHit found "  << nhit_tot );



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for CSC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("CSC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<CSCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      CSCSimHitCollection *new_collection = new CSCSimHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const CSCSimHitCollection& firstCont = *(iFirstCont->second);
        CSCSimHitCollection::const_iterator f_cell=firstCont.begin();
        CSCSimHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const CSCSimHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          double energy = hit.energyDeposit();
          double time = hit.globalTime();
          int cellId = hit.CSCid();
          //if (energy<1e-6) continue;
          ATH_MSG_DEBUG("CSCSimHit "<<cellId<<" has energy "<<energy<<" and time "<<time);
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of CSCSimHit found "  << nhit_tot );



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for RPC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("RPC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<RPCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      RPCSimHitCollection *new_collection = new RPCSimHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const RPCSimHitCollection& firstCont = *(iFirstCont->second);
        RPCSimHitCollection::const_iterator f_cell=firstCont.begin();
        RPCSimHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const RPCSimHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          double time = hit.globalTime();
          int cellId = hit.RPCid();
          ATH_MSG_DEBUG("RPCSimHit "<<cellId<<" has time "<<time);
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of RPCSimHit found "  << nhit_tot );



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for MDT hits
  m_HitContainer.clear();
  m_HitContainer.push_back("MDT_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++){
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<MDTSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      MDTSimHitCollection *new_collection = new MDTSimHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const MDTSimHitCollection& firstCont = *(iFirstCont->second);
        MDTSimHitCollection::const_iterator f_cell=firstCont.begin();
        MDTSimHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const MDTSimHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          double time = hit.globalTime();
          int cellId = hit.MDTid();
          ATH_MSG_DEBUG( "MDTSimHit "<<cellId<<" has time "<<time );
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of MDTSimHit found "  << nhit_tot );



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for TGC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("TGC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<TGCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      TGCSimHitCollection *new_collection = new TGCSimHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        const TGCSimHitCollection& firstCont = *(iFirstCont->second);
        TGCSimHitCollection::const_iterator f_cell=firstCont.begin();
        TGCSimHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const TGCSimHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          double time = hit.globalTime();
          int cellId = hit.TGCid();
          ATH_MSG_DEBUG( "TGCSimHit "<<cellId<<" has time "<<time );
          new_collection->push_back(hit);
        } // loop over hits
        ++iFirstCont; ++subevent;
        //msglog << MSG::INFO << "Subevent "<<subevent<<endmsg;
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of TGCSimHit found "  << nhit_tot );



  ////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for inner detector hits
  m_HitContainer.clear();
  m_HitContainer.push_back("BCMHits");
  m_HitContainer.push_back("BLMHits");
  m_HitContainer.push_back("PixelHits");
  m_HitContainer.push_back("SCT_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      ATH_MSG_INFO( " asking for: " << m_HitContainer[iHitContainer] );
      typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      CHECK( m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList) );
      // make new hit container
      SiHitCollection *new_collection = new SiHitCollection();
      if( evtStore()->record(new_collection,m_HitContainer[iHitContainer]) )
        {
          ATH_MSG_ERROR("execute(): Failed to record " << m_HitContainer[iHitContainer]);
          delete new_collection;
          return StatusCode::FAILURE;
        }

      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());
      while (iFirstCont != iEndCont) {
        // get SiHitCollection for this subevent
        const SiHitCollection& firstCont = *(iFirstCont->second);
        // Loop over elements in this SiHitCollection
        SiHitCollection::const_iterator f_cell=firstCont.begin();
        SiHitCollection::const_iterator l_cell=firstCont.end();

        while (f_cell != l_cell) {
          const SiHit hit = *f_cell;
          ++f_cell; nhit_tot++;
          double energy = hit.energyLoss();
          unsigned int cellId = hit.identify();
          ATH_MSG_DEBUG("Sihit "<<cellId<<" has energy "<<energy);
          new_collection->push_back(hit);
        }              //  loop over hits
        ++iFirstCont;
        ATH_MSG_INFO( "Subevent "<<++subevent );
      } // loop over subevent list

      // lock finished new container
      CHECK( evtStore()->setConst(new_collection) );
    } // end of loop over containers
  ATH_MSG_INFO( "Total number of inner detector hits found "  << nhit_tot );

  return StatusCode::SUCCESS;
}

StatusCode G4HitMerger::finalize()
{
  ATH_MSG_DEBUG( " G4HitMerger finalize completed successfully" );
  return StatusCode::SUCCESS;
}
