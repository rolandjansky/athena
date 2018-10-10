/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTK_AMsimulation_base.h"
#include <iostream>
#include <iomanip>

//#define VERBOSE_DEBUG

#define PRINT_IBL_HITS 0

using namespace std;

/* class FTK_AMsimulation_base
 *
 * base class to perform an emulation of the FTK pattern recognition
 *  (simulation of the Associated Memory hardware)
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 * many of the methods coded below have been moved from the class
 *  FTK_AMBank to this class, which has become the base class of FTK_AMBank
 *
 */

FTK_AMsimulation_base::FTK_AMsimulation_base(int id, int subid) :
   m_BankID(id), m_SubID(subid),
   m_npatterns(0),
   m_do_pattern_stats(0),
   m_StoreAllSS(false),
   m_SaveAllRoads(0),
   m_nplanes(0),
   m_require_first(false), m_require_last(false),
   m_ssmap(0x0), m_ssmap_unused(0x0),
   m_nao_nroads_am(0), m_nao_nroads_am_complete(0), m_nao_nroads_am_misspix(0), m_nao_nroads_am_misssct(0),
   m_nao_nroads_rw(0),
   m_stat_totroads(0.),
   m_stat_nevents(0),
   m_nroads(0),
   m_useMinimalAMIN(false)
{   
}

FTK_AMsimulation_base::~FTK_AMsimulation_base() {
}

void FTK_AMsimulation_base::setNPlanes(int nPlanes) {
   m_nplanes = nPlanes;
}

void FTK_AMsimulation_base::naoClear(void) {
   m_nao_nclus.clear();
   m_nao_nclus_road.clear();
   // 
   m_nao_nclus_road.resize(m_nplanes);
   m_nao_nss.clear();
   m_nao_nroads_am = 0;
   m_nao_nroads_am_complete = 0;
   m_nao_nroads_am_misspix = 0;
   m_nao_nroads_am_misssct = 0;
   m_nao_nroads_rw = 0;
}

int FTK_AMsimulation_base::readASCIIBank(const char*, int) {
   return -1;
}

int FTK_AMsimulation_base::readROOTBank(const char*, int) {
   return -1;
}

int FTK_AMsimulation_base::readDBBank(const char*, int, int ) {
   return -1;
}

int FTK_AMsimulation_base::readROOTBankCache(const char *) {
  return -1;
}

void FTK_AMsimulation_base::init() {
   m_stat_totroads = 0.;
   m_stat_nevents = 0;
}

void FTK_AMsimulation_base::end() {
}

void FTK_AMsimulation_base::addTotStat(long nRoads) {
   m_stat_totroads += nRoads;
   m_stat_nevents += 1;
   //std::cout<<"nroad="<<m_stat_totroads<<" nevent="<<m_stat_nevents<<"\n";
}

const std::map< int, std::map<int,FTKSS> >&
FTK_AMsimulation_base::getIgnoredStrips() {
   return m_usedssmap_ignored;
}

/** this function get the hit list and then populate the associative
    memory */
int FTK_AMsimulation_base::passHits(const vector<FTKHit> &hitlist)
{
   // this function partially replicate the structure of the old 
   // ftksim main loop, sorting, organizing, and populatig the AM

  // check if  the number of patterns is 0
  if (!m_npatterns)
    return 0;
#ifdef VERBOSE_DEBUG
  std::cout<<"clear()\n";
#endif
  clear();  
#ifdef VERBOSE_DEBUG
  std::cout<<"sort_hits()\n";
#endif
  sort_hits(hitlist);
    
  //readout_hits();
  
  //routing();


  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  
  if (ftkset.getEnableFTKSim()) {
#ifdef VERBOSE_DEBUG
     std::cout<<"data_organizer()\n";
#endif
      data_organizer();
#ifdef VERBOSE_DEBUG
      std::cout<<"am_in()\n";
#endif
      am_in();
  }
#ifdef VERBOSE_DEBUG
  std::cout<<"am_output()\n";
#endif
  am_output();

  if (FTKSetup::getFTKSetup().getRoadWarrior()>0) {
#ifdef VERBOSE_DEBUG
     std::cout<<"road_warrior()\n";
#endif
    road_warrior();
  }

  addTotStat(getNRoads());

#ifdef VERBOSE_DEBUG
  std::cout<<"getNRoads()\n";
#endif
  int r=getNRoads();
  // exit(0);
  return r;
}

/**
   this method elaborate the hits in the layer not used in the pattern 
    matching, those hits are read and grouped according the SS map
    in super-strip
    Comments:
       S.Schmitt 01/2015  moved from class FTK_AMBank to this class
*/
int FTK_AMsimulation_base::passHitsUnused(const std::vector<FTKHit> &hitlist) {
  int res(0);

  // clear the memory used to store the SS results
  m_usedssmap_ignored.clear();

  int error=0;
  std::vector<FTKHit>::const_iterator ihit = hitlist.begin();
  for (;ihit!=hitlist.end();++ihit) { // hit loop
    // reference to the current hit
    const FTKHit &tmphit = (*ihit);

    // by convention the hits in the ignored layers have negative plane number
    int plane = tmphit.getPlane();
    // search the map for this plane
    std::map<int, std::map<int,FTKSS> >::iterator
       issmap = m_usedssmap_ignored.find(plane);
    if (issmap==m_usedssmap_ignored.end()) {
       m_usedssmap_ignored[plane] = std::map<int,FTKSS>();
       issmap = m_usedssmap_ignored.find(plane);
    }
    // reference the map for this plane
    std::map<int,FTKSS> &ssmap= (*issmap).second;
    // identify the SS on the given plane
    int ssid=-1;
    try {
       ssid= FTKSetup::getFTKSetup().getHWModeSS()==0 ?
          getSSMapUnused()->getSSGlobal(tmphit) :
          getSSMapUnused()->getSSTower(tmphit,getBankID(),true);
    } catch(FTKException &e) {
       error++;
    }
    if (ssid<0) {
      // This means that hits had a recoverable issue and has to be skipped
      continue;
    }

    std::map<int,FTKSS>::iterator issitem = ssmap.find(ssid);
    if (issitem==ssmap.end()) {
      // add the item related to this SS number
      ssmap[ssid] = FTKSS();
      issitem = ssmap.find(ssid);
    }
    
    FTKSS &ss = (*issitem).second;
    ss.addHit(tmphit);
  } // end hit loop
  if(error) {
     FTKSetup::PrintMessageFmt(ftk::warn,"FTK_AMsimulation_base::passHitsUnused number or errors=%d\n",error );
  }
  static int print=PRINT_IBL_HITS;
  if(print) {
     cout<<"PassHitsUnused\n";
     for(std::map< int, std::map< int, FTKSS > >::const_iterator
            ipl=m_usedssmap_ignored.begin();
         ipl!=m_usedssmap_ignored.end();ipl++) {
        cout<<"unused plane="<<(*ipl).first<<" nSS="<<(*ipl).second.size();
        int nhit=0;
        int lastHash=-1;
        for(std::map< int, FTKSS >::const_iterator ihit=(*ipl).second.begin();
            ihit!=(*ipl).second.end();ihit++) {
           nhit+= (*ihit).second.getNHits();
           if((*ipl).first==0) {
              int localID,localX,localY;
              getSSMapUnused()->decodeSSTowerXY((*ihit).first,
                                                getBankID(),(*ipl).first,0,
                                                localID,localX,localY,true);
              int hash=getSSMapUnused()->getRegionMap()
                 ->getGlobalId(getBankID(),(*ipl).first,localID);
              if(hash!=lastHash) {
                 cout<<"\nModule="<<hash;
                 lastHash=hash;
              }
              cout<<" "<<(*ihit).first;
              //<<":"<<localX<<","<<localY;
              for(int ih=0;ih<(*ihit).second.getNHits();ih++) {
                 const FTKHit &h=(*ihit).second.getHit(ih);
                 cout<<"["<<h[0]<<","<<h[1]<<"]";
              }
           }
        }
        cout<<" nhit="<<nhit;
        cout<<"\n";
     }
     print--;
  }

  return res;
}

/**
 * Function: road_warrior()
 * Purpose:  1) sorts roads: N/N first, (N-1)/N after, (N-2)/N last (etc)
             2) applies road warrior to the roads and correspondingly updates am_out[]
 * Arguments: none
 * Returns: none
 * Comments: 
 *   S.Schmitt, 01/2015 moved from FTK_AMBank to this class
 *                      arguments of information_match changed
*******************************************/
void FTK_AMsimulation_base::road_warrior() {

#ifdef VERBOSE_DEBUG
  printf("*** roadwarrior\n");
#endif
  
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();

  // sort the roads list
  m_roads.sort();
  m_roads.reverse();
  list<FTKRoad>::iterator itroad = m_roads.begin();
  if (getNRoads()>0 && ftkset.getVerbosity()>1) {
    cout << "From list<FTKRoad>:" << endl;
    for (;itroad!=m_roads.end();++itroad) {
      printf("\t\tRoad %d has %d hits\n",(*itroad).getPatternID(),
	     (*itroad).getNHits());
    }
    itroad = m_roads.begin();
  }
  if( m_roads.empty() ) return;

  int totGhosts = 0;
#ifdef USEKDTREE
  // Build the KD Tree
  FTKRoad *head = &(*itroad);
  FTKRoadKDTree *kd_tree = new FTKRoadKDTree(head->getNPlanes(),head,0);
  int roadcounter = 1;
  for(++itroad; itroad != m_roads.end(); ++itroad, ++roadcounter) {
    head = &(*itroad);
    kd_tree->addNode(head,roadcounter);
  }

  roadcounter=0;
  for(itroad=m_roads.begin(); itroad != m_roads.end(); ++itroad,++roadcounter) { 
    // loop over the found roads
    if( itroad->getRWRejected() ) continue; // don't use rejected
    // skip full roads, only MJ roads can be deleted at this point
    if (itroad->getNHits()==itroad->getNPlanes()) continue;

    int nmatch = 0; /* Start the cycle. No match. */

    // build the mask of majority planes
    unsigned int unconstrmask(0);
    // form the mask of the empty SS, that not constrains the search
    for (int ipl=0;ipl!=itroad->getNPlanes();++ipl) {
      if (!(itroad->hasHitOnLayer(ipl))) 
	unconstrmask |= (1<<ipl);
    }

    // ask for the list of the road sharing all the SSs, except
    // the ones in the majority planes of the reference road
    list<FTKRoadKDTree*> sims;
    kd_tree->findSimilar(&(*itroad),unconstrmask,sims);

    // makes RW between similar road
    list<FTKRoadKDTree*>::iterator isim = sims.begin();
    for (;isim!=sims.end();++isim) {
      // the refroad can be removed only by another in a greater position
      // skip this otherwise
      if ((*isim)->getPosition()>roadcounter) continue;

      // compare the two roads
      /* when ref and chk are both N/N tracks the RW will fail,
	 in future this will be discarded but now is here as debug 
	 or to permit to test other algorithms */
      FTKRoad *ref = (*isim)->getNode();
      if( ref->getRWRejected() ) continue; // don't use rejected
      //if (informationMatch(ref->getPatternID(),itroad->getPatternID())) {
      if (informationMatch(ref,itroad)) {
	if(!itroad->getRWRejected()) nmatch += 1; // only increment if not already rejected
	// the patt2 is removed because if a match is found
	// it is a majority road, or both of them are MJ roads
#ifdef VERBOSE_DEBUG
	printf("\t\t%d ghosts found\n",nmatch);
#endif
	
	// flag the road as rejected by RW
	itroad->setRWRejected(1);
	itroad->setKillerRoad(ref);
      } 
    } // end loop over the other roads to look for matches
    totGhosts += nmatch;
  } // end loop over the found roads
  delete kd_tree; kd_tree = 0;
#endif
  
#ifndef USEKDTREE
  for(; itroad != m_roads.end(); ++itroad) { // loop over the found roads
    if( (*itroad).getRWRejected() ) continue; // don't use rejected

    int nmatch = 0; /* Start the cycle. No match. */

    list<FTKRoad>::iterator itroad2 = itroad; // point to the next
    ++itroad2;
    for(; itroad2 != m_roads.end(); ++itroad2) {
      // end loop over the other roads to look for matches
      if( (*itroad2).getRWRejected() ) continue; // already rejected

#ifdef VERBOSE_DEBUG
      int patt1 = (*itroad).getPatternID(); // reference pattern
      int patt2 = (*itroad2).getPatternID(); // pattern to compare
      printf("\t\tComparing %d with %d\n",patt1,patt2);
#endif
      if(informationMatch(& *itroad,& *itroad2)) { // check for the match
	nmatch += 1;
	// the patt2 is removed because if a match is found
	// it is a majority road, or both of them are MJ roads
#ifdef VERBOSE_DEBUG
	printf("\t\t%d ghosts found\n",nmatch);
#endif

	// flag the road as rejected by RW
	(*itroad2).setRWRejected(1);
	(*itroad2).setKillerRoad(&(*itroad));
      } 
    } // end loop over the other roads to look for matches
    totGhosts += nmatch;
  } // end loop over the found roads
#endif

  // so far, m_nroads is not adjusted for ghosts
  naoSetNroadsRW(getNRoads()-totGhosts);
  if(FTKSetup::getDBG()) {
     cout << "DBG: after roadwarrior we have  " << naoGetNroadsRW() << " roads" << endl;
  }
  
  // loop to clean the removed roads 
  if (!ftkset.getKeepRemoved()) {
    for(itroad = m_roads.begin(); itroad != m_roads.end(); ++itroad) { 
      if( itroad->getRWRejected()>0 ) {
         // Make sure the iterators are correct
         list<FTKRoad>::iterator ittmp = itroad;
	--ittmp;
	m_roads.erase(itroad);
	itroad = ittmp;
      }
    }
    // adjust m_nroads
    setNRoads(naoGetNroadsRW());
  }
  // make sure everything is consistent
  assert(m_roads.size()==static_cast<unsigned int>(getNRoads()));
  
  if (getNRoads()>0 && ftkset.getVerbosity()>1) {
    cout << "From list<FTKRoad>:" << endl;
    for (itroad = m_roads.begin();itroad!=m_roads.end();++itroad) {
      printf("\t\tRoad %d has %d hits\n",(*itroad).getPatternID(),
	     (*itroad).getNHits());
    }
  }

#ifdef VERBOSE_DEBUG
  printf("%d ghosts found, %d roads left\n", totGhosts,
         (int)m_roads.size());
#endif
}


/**
   helper class to assists print-out if roads
 */
class SSIDordering {
   virtual bool operator()(vector<int> const &a,vector<int> const &b) {
      unsigned n=a.size();
      if(n>b.size()) n=b.size();
      for(unsigned i=0;i<n;i++) {
         if(a[i]<b[i]) return true;
         if(a[i]>b[i]) return false;
      }
      if(a.size()<b.size()) return true;
      return false;
   }
};

/**
   print a list of roads
   the second argument selects roads in a specific sector
   sector=-1 means that all roads are printed
 */
void FTK_AMsimulation_base::printRoads(list<FTKRoad> const &roads,
                                       int printSectorID) const {
   map<int,map<vector<int>,FTKRoad const *> > roadMap;
   for(list<FTKRoad>::const_iterator iroad=roads.begin();
       iroad!=roads.end();iroad++) {
      vector<int> ssid(getNPlanes());
      for(int i=0;i<getNPlanes();i++) {
         ssid[i]=(*iroad).getSSID(i);
      }
      roadMap[(*iroad).getSectorID()][ssid]=& *iroad;
   }
   cout<<"number of sectors with roads: "<<roadMap.size()<<"\n";
   for(map<int,map<vector<int>,FTKRoad const *> >::const_iterator i=
          roadMap.begin();i!=roadMap.end();i++) {
      if(((*i).first==printSectorID)||
         (printSectorID<0)) {
         cout<<"sector="<<(*i).first<<" nroad="<<(*i).second.size()<<"\n";
         cout<<"===SEC";
         for(int i=0;i<getNPlanes();i++) {
            cout<<"=SSID"<<i;
         }
         cout<<"==BITMASK==ROAD===PATTID====DBID======HLIDbIDregSub=SR====SubSS=DCmask=HLmask\n";
         for(map<vector<int>,FTKRoad const *>::const_iterator
                iroad=(*i).second.begin();
             iroad!=(*i).second.end();iroad++) {
            FTKRoad const *road=(*iroad).second;
            //(*iroad).second->Print();
            cout<<setw(6)<<road->getSectorID();
            for(int i=0;i<road->getNPlanes();i++) {
               cout<<" "<<setw(5)<<road->getSSID(i);
            }
            cout<<" ";
            for(int i=0;i<road->getNPlanes();i++) {
               if(road->hasHitOnLayer(i)) cout<<"1";
               else cout<<"0";
            }
            cout<<setw(6)<<road->getRoadID()
                <<setw(9)<<road->getPatternID()
                <<setw(9)<<road->getPatternDBID()<<" ";
             for(int i=0;i<road->getNPlanes();i++) {
                cout<<road->getHLID(i);
             }
             cout<<setw(3)<<road->getBankID()
                <<setw(3)<<road->getRegion()
                <<setw(3)<<road->getSubRegion()
                <<setw(3)<<road->getNSubRoads()<<" "<<setbase(16);
            for(int i=0;i<road->getNPlanes();i++) {
               cout<<road->getSubSSMask(i);
            }
            cout<<setw(7)<<road->getDCBitmask()<<setw(7)<<road->getHLBitmask();
            cout<<setbase(10);
            cout<<"\n";
         }
      }
   }
}
