/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/RoadFinder.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/ftkdefs.h"

#include <TSystem.h>
#include <TBits.h>
#include <TObjArray.h>
#include <TObjString.h>

#include <iostream>
#include <cmath>
using namespace std;

//#define SCTTRK_DEBUG
// #road limits for SCTtrk case (applies to manufactured 11L roads)
const int MAXROADS_SOFT = 10000000; // soft limit: upon reaching, only accept 11/11 roads
const int MAXROADS_HARD = 20000000; // hard limit: upon reaching, stop accepting all roads

RoadFinder::RoadFinder() :
  m_verbosity(0),
  m_datainput(0), m_roadoutput(0),
  m_nbanks(0), m_nsubs(1), m_sub(0), m_banks(0),
  m_goodRegion(0), m_roadwarrior(0), 
  m_SCTtrk_nlayers(11), m_SCTtrk_nsubs(1), m_SCTtrk_lastlayer(3),
  m_SCTtrk_maxMissingPlanes(0), m_sector_map(0), 
  m_SSsearch_unused(false)
{
  // pass
}


RoadFinder::~RoadFinder()
{
  if (m_nbanks>0) {
    for (int ib=0;ib!=m_nbanks;++ib) {
      //cout << ib << endl;
      if (!m_banks[ib]) continue;
      delete m_banks[ib];
    }
    delete [] m_banks;
  }

  if(m_sector_map) {
    delete m_sector_map;
  }
}


void RoadFinder::init()
{

  // check which regions/towers don't have any good set of constants
  m_goodRegion = new bool[m_nbanks];
  for (int i=0;i!=m_nbanks;++i) {
    if (m_banks[i]) m_goodRegion[i] = true;
    else m_goodRegion[i] = false;
  }

  m_datainput->setNRegions(m_nbanks);
  m_datainput->init(m_goodRegion);

  m_roadoutput->init(TREE_ROAD_BUFSIZE,m_goodRegion);

  // open the first file
  if (!m_roadoutput->getMultiOut()) {
    // the only file in this mode has to be set before
    m_roadoutput->beginFile();
  }
  else {
    TString ofname(gSystem->BaseName(m_datainput->getCurrentFileName()));
    // prepend this old prefix
    ofname.Prepend("ftkroads_");
    // Remove grid-generated numbers like .bz2.1
    TObjArray* arr = ofname.Tokenize(".");
    if( arr ) {
      TObjString* laststr = (TObjString*)arr->Last();
      if( laststr ) {
	if( laststr->String().IsDigit() ) {
	  ofname.Resize( ofname.Length() - laststr->String().Length() - 1 );
	}
      }
      delete arr;
    }
    // remove .dat extension (and bz2 and gz)
    TString extarray[3] = { "bz2", "gz", "dat" };
    for( int i = 0; i < 3; i++ ) {
      if( ofname.EndsWith( extarray[i].Data() ) ) {
	ofname.Resize( ofname.Length() - extarray[i].Length() - 1 );
      }
    }
    // add the root extensions
    ofname.Append(".root");
    m_roadoutput->beginFile(ofname.Data());
  }


  for (int ibank=0;ibank!=m_nbanks;++ibank) {
    if (!m_banks[ibank]) continue;
    m_banks[ibank]->init();
    int nlayers = m_banks[ibank]->getNPlanes();
    // in SCTtrk mode, AM operates on 4L, but the final roads will be 11L
    if (FTKSetup::getFTKSetup().getSCTtrkMode()==1) {
      nlayers = m_SCTtrk_nlayers;
    }
    // size the internal structures into the output stream
    m_roadoutput->getStream(ibank)->init(nlayers);
  }
}


/** End of the RoadFinder execution, this method can propagate the
    the event to comunicate with the AM banks to do final tasks
    before the destructor is called */
void RoadFinder::end()
{
  for (int ibank=0;ibank!=m_nbanks;++ibank) {
    if (!m_banks[ibank]) continue;
    m_banks[ibank]->end();
  }  
}


/** this method prepare the array with the AM banks */
void RoadFinder::setNBanks(int nbanks)
{
  // reset if previosly allocated
  if (m_nbanks>0) {
    for (int ib=0;ib!=m_nbanks;++ib) {
      if (!m_banks[ib]) continue;
      delete m_banks[ib];
    }
    delete [] m_banks;
  }
  
  // set the number of the banks
  m_nbanks = nbanks;

  m_roadoutput->setNBanks(m_nbanks);

  // allocate the array of the pointers
  m_banks = new FTK_AMsimulation_base *[m_nbanks];
  // initialize it
  for (int ib=0;ib!=m_nbanks;++ib) {
    m_banks[ib] = 0x0;
  }
}


/** add a bank into the RoadFinder */
void RoadFinder::setAMBank(int id, FTK_AMsimulation_base *bank)
{
  if (id>=m_nbanks) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Region ID %d too large. Wrong number of regions (%d)",id,m_nbanks);
  }

  m_banks[id] = bank;
}


/** elaborate the next event, return -1 if fails */
int RoadFinder::nextEvent()
{
  if (!m_datainput) {
    cerr << "*** input data module doen't exist";
    return -1;
  }

  // move to the next event
  int res = m_datainput->readData();
  if (res<0) {
    cerr << "*** no more input data" << endl;
    // close the current file
    m_roadoutput->endFile();
    return -1;
  }
  else if (res==1) {
    // syncronize the output
    if (m_roadoutput->getMultiOut()) {
      // close the current file
      m_roadoutput->endFile();
      TString ofname(gSystem->BaseName(m_datainput->getCurrentFileName()));
      // prepend this old prefix
      ofname.Prepend("ftkroads_");
      // Remove grid-generated numbers like .bz2.1
      TObjArray* arr = ofname.Tokenize(".");
      if( arr ) {
	TObjString* laststr = (TObjString*)arr->Last();
	if( laststr ) {
	  if( laststr->String().IsDigit() ) {
	    ofname.Resize( ofname.Length() - laststr->String().Length() - 1 );
	  }
	}
	delete arr;
      }
      // remove .dat extension (and bz2 and gz)
      TString extarray[3] = { "bz2", "gz", "dat" };
      for( int i = 0; i < 3; i++ ) {
	if( ofname.EndsWith( extarray[i].Data() ) ) {
	  ofname.Resize( ofname.Length() - extarray[i].Length() - 1 );
	}
      }
      // add the root extensions
      ofname.Append(".root");
      m_roadoutput->beginFile(ofname.Data());
    }
  }
  
  m_datainput->processEvent();

  m_roadoutput->eventBegin();

  for (int ibank=0;ibank!=m_nbanks;++ibank) { // loop on the AM banks
    m_roadoutput->setRunNumber( ibank , m_datainput->runNumber() );
    m_roadoutput->setEventNumber( ibank , m_datainput->eventNumber() );
  }


  // we must have an instance of FTK_RawInput here, not just base FTKDataInput
  /* The code should now support multiple input classes */
  /*
  FTK_RawInput *inp = dynamic_cast<FTK_RawInput*>(m_datainput);
  if(!inp&) {
    cerr << "*** FATAL: expected FTK_RawInput, but could not perform dynamic_cast" << endl;
    exit(0);
  }
  */
  // give as input of each AM the hit list
  for (int ibank=0;ibank!=m_nbanks;++ibank) { // loop on the AM banks
     FTK_AMsimulation_base *pbank = m_banks[ibank];
     
     // this happens when the RF is used in parrallel jobs
     if (!pbank) continue; 

     if (m_datainput->hasRegionalInput()) {
       // The hits are alread arranged per banks, read only the relevent hits
       pbank->passHits(m_datainput->getRegionalHitsList(ibank));
       if (m_SSsearch_unused) {
         pbank->passHitsUnused(m_datainput->getHitsListUnused());
       }

     }
     else {
       // not regional input, send all the hits
       pbank->passHits(m_datainput->getHitsList());
       if (m_SSsearch_unused) {
         pbank->passHitsUnused(m_datainput->getHitsListUnused());
       }
     }

     // save additional statistical information for Naoki's simulation
     m_roadoutput->naoSetNhitsTot(ibank,m_datainput->naoGetNhitsTot());
     m_roadoutput->naoSetNclusTot(ibank,m_datainput->naoGetNclusTot());
     m_roadoutput->naoSetNclus(ibank,pbank->naoGetNclus());
     m_roadoutput->naoSetNss(ibank,pbank->naoGetNss());
     m_roadoutput->naoSetNroadsAM(ibank,pbank->naoGetNroadsAM());
     m_roadoutput->naoSetNroadsAMComplete(ibank,pbank->naoGetNroadsAMComplete());
     m_roadoutput->naoSetNroadsAMMissPix(ibank,pbank->naoGetNroadsAMMissPix());
     m_roadoutput->naoSetNroadsAMMissSCT(ibank,pbank->naoGetNroadsAMMissSCT());
     m_roadoutput->naoSetNroadsRW(ibank,pbank->naoGetNroadsRW());


     const list<FTKRoad>& roadslist = pbank->getRoads();
     // needs to be done after getting roads
     m_roadoutput->naoSetNclus_road(ibank,pbank->naoGetNclus_road());

     // counters for a more faithful assessment of hardware-level # of 4L roads
     long int final_count(0);
     long int count_lookup(0);
     long int count_misses(0);

     list<FTKRoad>::const_iterator iroad = roadslist.begin();
     int rid(0);
     bool soft_limit(true), hard_limit(true);
     FTKRoadStream *stream = m_roadoutput->getStream(m_datainput->getCurrentRegion());
     for (;iroad!=roadslist.end();++iroad) {
       // default case: just save the road
       if (!FTKSetup::getFTKSetup().getSCTtrkMode()) {
         m_roadoutput->addRoad(pbank->getBankID(),*iroad);
       }

       /*
	 FlagAK: SCTtrk mode (Constantinos architecture)
	 The idea here is to take the newly created 4L roads and combine them with 8L SCTtrk roads.
	 The resulting 11L roads are saved in m_roadoutput, along with all 11L FTKSS maps.
	 This allows to use the resulting roads with unmodified 11L road_merger and track_fitter.
	 IMPORTANT CAVEAT:
	 8L SCT layers may contain additional hits (beyond what was used in 8L track fitting).
	 We don't want to loop over those fits in track_fitter.
	 This is handled by marking these un-needed hits using a hit mask (similar to SS-shifted mode)
       */
       else {
	 if(rid>=MAXROADS_HARD) {
	   if(hard_limit) {
	     std::cout << "WARNING: exceeded hard limit on # of roads" << std::endl;
	     hard_limit = false;
	   }
	   break;
	 }

	 const FTKRoad& road4 = *iroad;
	 assert(road4.hasHitOnLayer(m_SCTtrk_lastlayer)); // should always have sct trk layer
#ifdef SCTTRK_DEBUG
	 cout << "CY road4: " << road4 << endl;
#endif
	 const unordered_map<int,FTKSS>& imap = pbank->getStrips(m_SCTtrk_lastlayer);
	 unordered_map<int,FTKSS>::const_iterator iss = imap.find((*iroad).getSSID(m_SCTtrk_lastlayer));
	 assert(iss!=imap.end());
	 const std::vector<FTKHit>& trkhits = (*iss).second.getHits();

	 // Determine whether 11L roads were created from this 4L
	 // road. If there weren't any, then don't count this 4L road.
	 bool addedRoads(false);

	 int skipped_on_lookup(0);
	 int skipped_on_misses(0);

	 // for each SCTTRK hit, create a new 11L road
	 for(unsigned int itrk=0; itrk<trkhits.size();itrk++){
	   // back-navigate to 8L road
	   int bankID = trkhits[itrk].getBankID();
	   int roadID = trkhits[itrk].getRoadID();
	   int trackID = trkhits[itrk].getTrackID();
	   FTKRoad *road8 = m_datainput->getRoadInput()->getRoad(m_datainput->getCurrentRegion(),roadID,bankID);
	   assert(road8);
#ifdef SCTTRK_DEBUG
	   cout << "CY road8: " << *road8 << endl;
#endif
	   int road8pos=m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->findRoad(roadID,bankID);
	   // fix the 8L sector if it was run in subregions: lookup assumes 1M sectors
	   int newsector8 = road8->getSectorID()*m_SCTtrk_nsubs+road8->getSubRegion();
	   int newsector4 = road4.getSectorID()*m_nsubs+m_sub; // cy 4L sub
	   int sector11 = applySectorMap(newsector4,newsector8);
	   //cout << "\t" << road4.getSectorID() << "\t" << road8->getSectorID() << "\t" << sector11 << endl;
#ifdef SCTTRK_DEBUG
	   cout << "road4 sector: " << road4.getSectorID() << " road8 sector: " << road8->getSectorID() << " sector11: " << sector11 << endl;
#endif
	   // since sector lookup table is not 100% efficient, sometimes we must skip roads:
	   /*
	   if(sector11==-1) {	     
	     m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->detachHits(road8pos);
	     skipped_on_lookup=1;
	     continue;
	   }
	   */

	   const FTKTrack *track8 = m_datainput->getTrackInput()->getTrack(m_datainput->getCurrentRegion(),trackID,bankID);
	   assert(track8);
	   // Warning: we assume that for 8L tracks, 8 SCT hits directly map into 8 FTKTrack coordinates
	   // If we use CM arch to run some pixel layers in the first stage, this will need to be updated!
	   const int pix_nhits = road4.getNHits()-1;
	   const int sct_ncoords = track8->getNCoords();
	   const int sct_bitmask = track8->getBitmask();
	   const int sct_nhits = sct_ncoords-track8->getNMissing();

	   // if soft limit on #roads is exceeded, only accept 11/11 roads
	   if(rid>=MAXROADS_SOFT) {
	     if(soft_limit) {
	       std::cout << "WARNING: exceeded soft limit on # of roads; will only accept all-layer roads" << std::endl;
	       soft_limit = false;
	     }
	     if(sct_nhits+pix_nhits != m_SCTtrk_nlayers ) {
	       m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->detachHits(road8pos);
               continue;
	     }
	   }
	   // Apply SCTTRK_MAX_MISSING_PLANES and MAX_MISSING_SCT_PAIRS
	   int sct_nmisspair = 0;
	   for(int pairidx = 0; pairidx < sct_ncoords; pairidx+=2) {
	     // 11001011 bitmask (example)
	     // 00110100 inverted
	     // 00000011 no  (<<0)
	     // 00001100 no  (<<2)
	     // 00110000 yes (<<4)
	     // 11000000 no  (<<6)
	     if( ((~sct_bitmask)&(3<<pairidx)) == (3<<pairidx) ) sct_nmisspair++;
	   }
	   
	   // Drop the road in 2 cases:
	   if(// 1. Not enough hits 
	      ( (sct_nhits+pix_nhits) < (m_SCTtrk_nlayers - m_SCTtrk_maxMissingPlanes - 
					 FTKSetup::getFTKSetup().getMaxMissingSctPairs()) ) ||
	      // 2. Not enough missing SCT pairs for the number of hits
	      (sct_nhits+pix_nhits+sct_nmisspair) < (m_SCTtrk_nlayers - m_SCTtrk_maxMissingPlanes) ) {
	     m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->detachHits(road8pos);
	     skipped_on_misses=1;
	     continue; // drop the road
	   }

	   // since sector lookup table is not 100% efficient, sometimes we must skip roads:
	   if(sector11==-1) {	     
	     m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->detachHits(road8pos);
	     skipped_on_lookup=1;
	     continue;
	   }

	   // construct a new 11-layer road:
	   FTKRoad road11(*iroad);
	   road11.setNPlanes(m_SCTtrk_nlayers);
	   road11.setSectorID(sector11);
	   // Set pattern ID so we have some idea of the pattern's frequency
	   road11.setPatternID(road4.getPatternID());
	   road11.reserveHitsMask(m_SCTtrk_nlayers);
	   road11.setRoadID(rid++);
	   road11.setNHits(sct_nhits+pix_nhits);
	   // if the merge step will be skipped, encode the subregion information
	   // 00000001111 + 11011111 = 11011111111 (removes 4th layer and adds 8 SCT layers) 
	   unsigned int newBitMask=(road4.getBitMask()&(~(1<<m_SCTtrk_lastlayer)))|(track8->getBitmask()<<m_SCTtrk_lastlayer);
	   road11.setBitMask(newBitMask);
	   // copy over pixel ss
	   for(int ipl=0; ipl<m_SCTtrk_lastlayer;++ipl) {
	     road11.setSSID(ipl,(*iroad).getSSID(ipl));
	   }
	   // copy over sct ss
	   for(int ipl=m_SCTtrk_lastlayer; ipl<m_SCTtrk_nlayers;++ipl) {
	     road11.setSSID(ipl,road8->getSSID(ipl-m_SCTtrk_lastlayer));
	   }
	   // Now we want to mark extra hits in SCT layers so that they are not used in track_fitter
#ifdef SCTTRK_DEBUG
	   std::cout << "Comparing hit by hit: " << std::endl;
	   std::cout << *track8 << std::endl;
#endif
	   for(int ipl=0; ipl<road8->getNPlanes();++ipl) {
	     TBits v;
	     // loop over all hits in this SCT plane
#ifdef SCTTRK_DEBUG
	     std::cout << ipl << ": TRACK HIT: (" << track8->getCoord(ipl) << ")" << std::endl;
#endif
	     for(unsigned int im=0; im < road8->getNHits(ipl); im++) {
#ifdef SCTTRK_DEBUG
	       std::cout << ipl << ": ROAD HIT " << im << ": " << road8->getHit(ipl,im) << std::endl;
#endif
	       // again assume that for 8L tracks, 8 SCT hits directly map into 8 FTKTrack coordinates
	       if( road8->getHit(ipl,im)[0] == track8->getCoord(ipl) ) continue;
	       v.SetBitNumber(im);
	     }
#ifdef SCTTRK_DEBUG
	     if(v.FirstSetBit()<8) { std::cout << "Bitmask " << ipl << ": " << std::endl; v.Print(); }
#endif
	     road11.setHitsMask(m_SCTtrk_lastlayer+ipl,v);
	   }
	   m_datainput->getRoadInput()->getStream(m_datainput->getCurrentRegion())->detachHits(road8pos);
	   // finally, add our brand-new 11-layer road:
	   m_roadoutput->addRoad(pbank->getBankID(),road11);
	   addedRoads = true;
#ifdef SCTTRK_DEBUG
	   cout << "CY road11: " << road11 << endl;
#endif
	 } // end loop over trkhits
	
	 // Increment 4L road counter only if we added 11L roads based
	 // on it.
	 if( addedRoads || skipped_on_lookup) {
	   m_roadoutput->inc4LRoad(pbank->getBankID());
	   final_count++;
	 }

	 if(!addedRoads && skipped_on_lookup) count_lookup++;
	 else
	   if(!addedRoads && skipped_on_misses) count_misses++;

       } // end SCTTrk Mode case

     } // end loop over roads
     if (FTKSetup::getFTKSetup().getSCTtrkMode()) {
       if(FTKSetup::getDBG()) {
	 cout << "DBG: final 4L AM count = " << final_count << "; misses = " << count_misses << "; lookup = " << count_lookup << endl;
       }
       m_roadoutput->naoSetNroadsMOD(ibank,final_count);
     }
     
     // Create FTKSS maps
     if (!FTKSetup::getFTKSetup().getSCTtrkMode()) {
       // copy all FTKSS maps from AM bank:
       for (int ipl=0;ipl<pbank->getNPlanes();++ipl) { // planes loop
	 const unordered_map<int,FTKSS>& imap = pbank->getStrips(ipl);
	 unordered_map<int,FTKSS>::const_iterator iss = imap.begin();
	 for (;iss!=imap.end();++iss) { // SS item
	   m_roadoutput->addSS(pbank->getBankID(),ipl,(*iss).first,(*iss).second);
	 } // end SS item
       } // end planes loop

       if (m_SSsearch_unused) { // if the information on the SS hits is ingored
	 m_roadoutput->addUnusedSSMap(pbank->getBankID(),
				      pbank->getIgnoredStrips());
       }
     }
     else {
       // copy over pixel FTKSS maps from AM bank:
       for(int ipl=0; ipl<m_SCTtrk_lastlayer;++ipl) {
	 const unordered_map<int,FTKSS>& imap = pbank->getStrips(ipl);
	 unordered_map<int,FTKSS>::const_iterator iss = imap.begin();
	 for (;iss!=imap.end();++iss) { // SS item
	   m_roadoutput->addSS(pbank->getBankID(),ipl,(*iss).first,(*iss).second);
	 } // end SS item
       }
       // copy over sct FTKSS maps from 8L data:
       int bid=pbank->getBankID();
       for(int ipl=m_SCTtrk_lastlayer; ipl<m_SCTtrk_nlayers;++ipl) {
	 m_roadoutput->addSSPlane(bid,ipl,m_datainput->getRoadInput()->getSSPlane(bid,ipl-m_SCTtrk_lastlayer));
       }
     }
     
  } // end loop over the AM banks
  
  m_roadoutput->eventEnd();

  return 0;
}

int RoadFinder::applySectorMap(int sec4, int sec8) {
  return m_sector_map ? m_sector_map->GetSector(sec4,sec8) : -1; 
}
