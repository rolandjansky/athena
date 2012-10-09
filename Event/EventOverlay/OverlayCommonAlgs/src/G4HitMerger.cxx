/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm to merge G4Hits from different subevents from pileup service to a single event

#include "OverlayCommonAlgs/G4HitMerger.h"
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
  : Algorithm(name, pSvcLocator), m_storeGateSvc("StoreGateSvc",name)
{
  declareProperty("Detectors", m_Detectors, "Detector selection");
}

G4HitMerger::~G4HitMerger(){}

StatusCode G4HitMerger::initialize()
{
  MsgStream  msglog(messageService(),name());

// ....... Access Event StoreGate
  StatusCode sc = m_storeGateSvc.retrieve() ;
  if (sc.isFailure()) 
  {
    msglog << MSG::ERROR
	   << "Unable to access pointer to StoreGate"
	   << endreq;
    return StatusCode::FAILURE;
  }

// locate the PileUpMergeSvc and initialize our local ptr
  if (!(service("PileUpMergeSvc", p_mergeSvc)).isSuccess() ||
       0 == p_mergeSvc) {
     msglog << MSG::ERROR << "Could not find PileUpMergeSvc" << endreq;
     return StatusCode::FAILURE;
  }
  else
  {
    msglog << MSG::INFO << "PileUpMergeSvc successfully initialized" << endreq;
  }

// ....... Access DD StoreGate
  sc = service ( "DetectorStore" , detStore ) ;

  if (sc.isFailure()) 
  {
    msglog << MSG::ERROR
	   << "Unable to access pointer to DetectorStore"
	   << endreq;
    return StatusCode::FAILURE;
  }

  msglog << MSG::DEBUG 
	 << "Initialization completed successfully" 
	 << endreq;

  return StatusCode::SUCCESS;

}

StatusCode G4HitMerger::execute()
{
  
  MsgStream  msglog(messageService(),name());
  StatusCode sc(StatusCode::FAILURE);
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Append MCEventCollections
  
  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  //  TimedTruthList truthList;
  std::string truthCollKey("TruthEvent");
  
  msglog << MSG::INFO << " asking for container " << truthCollKey << endreq;
  
  TimedTruthList* truthList = new TimedTruthList;
  
  // retrieve the list of pairs (time, container) from the PileUp service
  if (!(p_mergeSvc->retrieveSubEvtsData(truthCollKey, *truthList).isSuccess()) && truthList->size() == 0) {
    msglog << MSG::ERROR << "Could not fill truthList for " << truthCollKey << endreq;
    return StatusCode::FAILURE;
  }
  
  // Start with a new (blank) McEventCollection
  
  McEventCollection* new_collection = new McEventCollection;
  sc = m_storeGateSvc->record(new_collection, truthCollKey);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << "Cannot record new McEventCollection for " << truthCollKey << endreq;
    return sc;
  }
  
  // loop over the list, adding the component McEventCollections to the new one
  //
  int McCounter(0);
  for (TimedTruthList::iterator iter = truthList->begin(); iter != truthList->end(); ++iter) {
    McCounter++;
    
    msglog << MSG::INFO << "Input McEventCollection " << McCounter << " " << *(iter->second) << endreq;

    // Surprisingly, McEventCollection::operator= pushes_back GenEvents from the RHS
    // onto the current list without first clearing the list first. 
    // Thus, '=' is really an 'append' or += operation
    //
      (*new_collection) = *(iter->second);
  }
  msglog << MSG::INFO << "Merged McEventCollection  " << *(new_collection) << endreq;

  sc = m_storeGateSvc->setConst(new_collection);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << " Cannot lock container for " << truthCollKey << endreq;
    return(StatusCode::FAILURE);
  }
  
  delete truthList;
  
  
  //first get the list of McEventCollections
  //  StatusCode sc(StatusCode::FAILURE);
  //  if (! (sc = p_mergeSvc->retrieveSubEvtsData(truthCollKey, truthList)).isSuccess() ) {
  //    msglog<<MSG::ERROR<< "execute: Can not find TimedTruthList with key "<< truthCollKey << endreq;    
  //    return StatusCode::RECOVERABLE;;
  //  }
  //  int nInputMcEventColls(truthList.size());
  //  //and check it is not empty
  //  if (0 == nInputMcEventColls) {
  //    msglog<<MSG::ERROR<< "execute: TimedTruthList with key "	<< truthCollKey<< " is empty" << endreq;
  //    return StatusCode::RECOVERABLE;
  //  }
  //
  //  TimedTruthList::iterator lI(truthList.begin()), lE(truthList.end());
  //  //start from the original event collection
  //  McEventCollection* pOvrlMcEvColl(new McEventCollection(*(lI->second)));
  //  
  //  sc = m_storeGateSvc->record(pOvrlMcEvColl, truthCollKey);
  //  if (!sc.isSuccess()) msglog<<MSG::ERROR<<"Could not write MCEventCollection!"<<endreq;
  
  
  
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
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
      TimedHitContList *hitContList = new TimedHitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],*hitContList).isSuccess()) && hitContList->size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      LArHitContainer *new_collection = new LArHitContainer();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
      }
      
      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList->begin());
      TimedHitContList::iterator iEndCont(hitContList->end());
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
	  msglog<<MSG::DEBUG<<"LArHit "<<cellId<<" has energy "<<energy<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }

      delete hitContList;
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of LArHit found "  << nhit_tot << endreq;

  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for Tile hits
  m_HitContainer.clear();
  m_HitContainer.push_back("MBTSHits");
  m_HitContainer.push_back("TileHitVec");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<TileHitVector>::type TimedHitContList;
      TimedHitContList *hitContList = new TimedHitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],*hitContList).isSuccess()) && hitContList->size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      TileHitVector *new_collection = new TileHitVector();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
      }
      
      // loop over this list
      int subevent=0;
      TimedHitContList::iterator iFirstCont(hitContList->begin());
      TimedHitContList::iterator iEndCont(hitContList->end());
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
	  msglog<<MSG::DEBUG<<"TileHit "<<cellId<<" has energy "<<energy<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }

      delete hitContList;
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of TileHit found "  << nhit_tot << endreq;


  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for TRT hits
  m_HitContainer.clear();
  m_HitContainer.push_back("TRTUncompressedHits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<TRTUncompressedHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      TRTUncompressedHitCollection *new_collection = new TRTUncompressedHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"TRTUncompressedHit "<<cellId<<" has energy "<<energy<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of TRTUncompressedHit found "  << nhit_tot << endreq;
  
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for CSC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("CSC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<CSCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      CSCSimHitCollection *new_collection = new CSCSimHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"CSCSimHit "<<cellId<<" has energy "<<energy<<" and time "<<time<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of CSCSimHit found "  << nhit_tot << endreq;



  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for RPC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("RPC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<RPCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      RPCSimHitCollection *new_collection = new RPCSimHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"RPCSimHit "<<cellId<<" has time "<<time<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of RPCSimHit found "  << nhit_tot << endreq;


  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for MDT hits
  m_HitContainer.clear();
  m_HitContainer.push_back("MDT_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<MDTSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      MDTSimHitCollection *new_collection = new MDTSimHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"MDTSimHit "<<cellId<<" has time "<<time<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of MDTSimHit found "  << nhit_tot << endreq;
  

  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //My attempt at doing the same for TGC hits
  m_HitContainer.clear();
  m_HitContainer.push_back("TGC_Hits");
  nhit_tot=0;
  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {    
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<TGCSimHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      TGCSimHitCollection *new_collection = new TGCSimHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"TGCSimHit "<<cellId<<" has time "<<time<<endreq;
	  new_collection->push_back(hit);
	} // loop over hits
	++iFirstCont; ++subevent;
	//msglog << MSG::INFO << "Subevent "<<subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of TGCSimHit found "  << nhit_tot << endreq;



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
      msglog << MSG::INFO << " asking for: " << m_HitContainer[iHitContainer] << endreq;
      typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitContList;
      TimedHitContList hitContList;

      // retrieve list of pairs (time,container) from PileUp service
      if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer],hitContList).isSuccess()) && hitContList.size()==0) {
	msglog << MSG::ERROR << "Could not fill TimedHitContList for " << m_HitContainer[iHitContainer] << endreq;
	return StatusCode::FAILURE;
      }

      // make new hit container
      SiHitCollection *new_collection = new SiHitCollection();
      sc = m_storeGateSvc->record(new_collection,m_HitContainer[iHitContainer]);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " cannot record new container for " << m_HitContainer[iHitContainer] << endreq;
	return sc;
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
	  msglog<<MSG::DEBUG<<"Sihit "<<cellId<<" has energy "<<energy<<endreq;
	  new_collection->push_back(hit);
	}              //  loop over hits
	++iFirstCont;
	msglog << MSG::INFO << "Subevent "<<++subevent<<endreq;
      } // loop over subevent list

      // lock finished new container
      sc = m_storeGateSvc->setConst(new_collection);
      if (sc.isFailure()) {
	msglog << MSG::ERROR << " Cannot lock container for " << m_HitContainer[iHitContainer] << endreq;
	return(StatusCode::FAILURE);
      }
      
    } // end of loop over containers
  msglog << MSG::INFO << "Total number of inner detector hits found "  << nhit_tot << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode G4HitMerger::finalize()
{
  MsgStream  msglog(messageService(),name());
  msglog << MSG::DEBUG << " G4HitMerger finalize completed successfully" << endreq;
  return StatusCode::SUCCESS;
}
