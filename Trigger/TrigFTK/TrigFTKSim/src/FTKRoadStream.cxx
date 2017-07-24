/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRoadStream.h"

#include <list>
#include <iostream>
#include <iomanip>
using namespace std;

FTKRoadStream:: FTKRoadStream() :
  m_run_number(0ul), m_event_number(0ul),
  m_nroads(0), m_nroads_4L(0), 
  m_nroads_tot(0), m_nroads_good(), m_nroads_maj(0), m_nroads_maj_good(0),
  m_ssmaps(),
  m_nao_nhits_tot(0), m_nao_nclus_tot(0),
  m_nao_nroads_am(0), m_nao_nroads_am_complete(0), m_nao_nroads_am_misspix(0), m_nao_nroads_am_misssct(0), 
  m_nao_nroads_mod(0), m_nao_nroads_rw(0),
  m_evtidx(0x0), m_barcode(0x0), m_frac(0x0),
  m_fit_list()
{
  m_roads = new TClonesArray( "FTKRoad" ,10000);
}


FTKRoadStream:: ~FTKRoadStream()
{
  delete m_roads;
}


/** initialize the ssmaps */
void FTKRoadStream::init( int nplanes)
{
  m_ssmaps.clear();
  m_ssmaps.reserve(nplanes);
  for ( int i=0;i!=nplanes;++i)
    m_ssmaps.push_back(map< int ,FTKSS>());
}


void FTKRoadStream::clear()
{
  m_run_number = 0ul;
  m_event_number = 0ul;

  m_nroads = 0;
  m_nroads_4L = 0;
  m_roads->Delete();

  vector< map< int,FTKSS> >::iterator imap = m_ssmaps.begin();
  for (;imap!=m_ssmaps.end();++imap) {
    (*imap).clear();
  }
 
  m_ssmaps_unused.clear();

  m_roadIdMap.clear();
}


FTKRoad* FTKRoadStream:: addRoad( const FTKRoad &road)
{
  // create a new element into the clones array
  FTKRoad* returnme = new ((*m_roads)[m_nroads++]) FTKRoad(road);
  m_roadIdMap[std::make_pair< int, int>(road.getRoadID(),road.getBankID())] = m_nroads-1;
  return returnme;
}

FTKRoad* FTKRoadStream:: getRoad( int ipos) const
{
  assert(ipos>=0);
  if (ipos>=m_nroads) return 0;
  else return (FTKRoad*)m_roads->At(ipos);
}

/** this method sorts the roads according the ordering method
    used in the FTK_AMBank::road_warrior().
 */
void FTKRoadStream::sortRoads()
{
  m_roads->Sort();
  buildRoadMap();
}


/** remove a road, by position, from the list of the roads */
void FTKRoadStream::removeRoad( int pos)
{
  assert(pos>=0);
  FTKRoad* tmproad = (FTKRoad*)m_roads->At(pos);
  if( tmproad && m_roadIdMap.size() ) {
    std::map< std::pair< int ,int >, int >::iterator it = m_roadIdMap.find( std::make_pair<int , int>(tmproad->getRoadID(),tmproad->getBankID()));
    if( it != m_roadIdMap.end() )
      m_roadIdMap.erase(it);
  }
  m_roads->RemoveAt(pos);
}


/** compress the TClonesArray to keep the occupancy low */
void FTKRoadStream::optimize()
{
  m_roads->Compress();
  m_nroads = m_roads->GetEntries();
  buildRoadMap();
} 


/** this method, passing a road ID and a bank ID, return the ID
    of the corresponding road. If it doesn't exist return -1 */
int FTKRoadStream::findRoad( int roadid, int bankid)
{
  if( !m_roadIdMap.size() )
    buildRoadMap();

  std::map< std::pair< int, int>, int >::iterator it = m_roadIdMap.find( std::make_pair(roadid,bankid));
  if( it != m_roadIdMap.end() )
    return it->second;
  else
    return -1;
}

void FTKRoadStream::addSS( int plane,const int ssid, const FTKSS &ss)
{

  // map for this plane
  map< int,FTKSS> &imap = m_ssmaps[plane];
  // find this ss is in the bank
  map< int,FTKSS>::iterator item = imap.find(ssid);
  if (item==imap.end()) { // not found
    imap[ssid] = ss;
  }
  else {
    // add a counter of the connected roads
    // TODO
  }
}


void FTKRoadStream::addUnusedSS( int plane,const int ssid, const FTKSS &ss)
{
  // search if the plane is in the list
  map< int, map< int ,FTKSS> >::iterator issmap = m_ssmaps_unused.find(plane);
  if (issmap == m_ssmaps_unused.end()) {
    // first SS for this plane, create the map for this plane
    m_ssmaps_unused[plane] = map< int ,FTKSS>();
    issmap = m_ssmaps_unused.find(plane);
  }

  // map for this plane
  map< int,FTKSS> &imap = (*issmap).second;

  // find this ss is in the bank
  map< int,FTKSS>::iterator item = imap.find(ssid);
  if (item==imap.end()) { // not found
    imap[ssid] = ss;
  }
  else {
    // add a counter of the connected roads
    // TODO
  }
}


/** print a debug message summarizing the information for the roads
    and hits found in one event for a given bank */
void FTKRoadStream::Print( bool printCoords)
{
   cout << "*** Run/Event " << m_run_number << "/" << m_event_number << " ***" << endl;

   cout << "*** Roads: ***" << endl;
   for ( int ir=0;ir<m_nroads;++ir) {
     FTKRoad *tmpr = dynamic_cast <FTKRoad*>(m_roads->At(ir));
     if (tmpr) cout << (*tmpr) << endl;
   }
   cout << endl;

   cout << "*** Super-Strips ***" << endl;
   vector< map< int,FTKSS> >::iterator ipl = m_ssmaps.begin();
   if (ipl==m_ssmaps.end()&&m_nroads>0) {
      cerr << "*** no roads found" << endl;
   }

   for ( int npl=0;ipl!=m_ssmaps.end();++ipl,++npl) { // plane loop
      map< int ,FTKSS>::iterator item = (*ipl).begin();
      cout << "Layer " << setw(2) << npl << ": ";
      for(;item!=(*ipl).end();++item) { // hits loop
        cout << setw(9) << (*item).first;
        // print coordinates in each superstrip
        if (printCoords) {
          std::cout << "{" ;
          const std::vector<FTKHit>& hits = (*item).second.getHits();
          for (unsigned int ii=0;ii<hits.size();ii++) {
            std::cout << hits[ii];
          }
          std::cout << "}" ;
        }
      } // end hits loop
      cout << endl;
   } // end plane loop
   cout << endl;

   cout << "*** Super-Strips (Ignored) ***" << endl;
   map< int, map< int ,FTKSS> >::iterator iupl = m_ssmaps_unused.begin();  
   for ( int npl=0;iupl!=m_ssmaps_unused.end();++iupl,++npl) { // plane loop
      map< int ,FTKSS>::iterator item = (*iupl).second.begin();
      cout << "Ignored Layer " << setw(2) << (*iupl).first << '(' << npl << "): ";
      for(;item!=(*iupl).second.end();++item) { // hits loop
        cout << setw(9) << (*item).first;
        // print coordinates in each superstrip
        if (printCoords) {
          std::cout << "{" ;
          const std::vector<FTKHit>& hits = (*item).second.getHits();
          for (unsigned int ii=0;ii<hits.size();ii++) {
            std::cout << hits[ii];
          }
          std::cout << "}" ;
        }
      } // end hits loop
      cout << endl;
   } // end plane loop
   cout << endl;
}


/** this method populate the hits in a given road.
    The road ID follow the road list for this event */
void FTKRoadStream::attachHits( int roadid)
{
  // get the road pointer, this is a road at the top level, AM level
  FTKRoad *curroad = getRoad(roadid);
  assert(curroad);
  curroad->prepareHits();

  if (curroad->getDCMatchMode()>0) {

    // offset of the bitmask, required to read the DC mask
    unsigned int maskoffset(0);

    // retrive the DC information, obtain for all the layers
    unsigned int dcmask = curroad->getDCBitmask();

    for ( int ipl=0;ipl!=curroad->getNPlanes();++ipl) { // plane loop
      // number of DC bits for this layer
      unsigned int ndcbits = curroad->getHLID(ipl);

      if (!curroad->hasHitOnLayer(ipl)) {
        maskoffset += ndcbits;
        continue;
      }

      vector<FTKHit> hits;
      int basessid = curroad->getSSID(ipl); // base full resolution SS for this layer

      // extract DC mask for this layer, reading only the related bits
      unsigned int localdcmask = dcmask>>maskoffset & (~(~0u<<ndcbits));
      // in the basessid mask the bits involved by the DC placing them to 0
      basessid &= ~localdcmask;

      // check how many bits are set
      unsigned int nuseddcbits(0);
      for ( unsigned int ibit = 0; ibit!=ndcbits; ++ibit) {
        if ((localdcmask>>ibit)&1) nuseddcbits += 1;
      }

      // number of combination of SS to read, it is a power of 2
      unsigned int ncombs(1<<nuseddcbits);
      for ( unsigned int icomb=0;icomb!=ncombs;++icomb) { // loop over the bit combinations
        int ssid(basessid); // initiaze the SS ID using the masked full resolution SS

        // selector of the bit to be copied
        unsigned int activepos(0);
        for (unsigned ibit=0;ibit!=ndcbits;++ibit) { // loop over the DC bits
          if ((localdcmask>>ibit)&1) {
            // if this is one of the bits masked by the DC,
            // copy the given bit content of the combination into the ssid
            ssid |= ((icomb>>activepos)&1)<<ibit;
            activepos += 1; // next position
          }
        } // end loop over the DC bits

        if (activepos!=nuseddcbits) cout << "Invalid DC scan" << endl;

#if 0
        cout << "# " << curroad->getPatternID() << ", " << ipl << " " << basessid << ", " << localdcmask << ", " << ssid << " ";
#endif
        // check the SSID and retrieve the hits for the current layer of this road
        map< int ,FTKSS>::iterator iss = m_ssmaps[ipl].find(ssid);
        if (iss!=m_ssmaps[ipl].end()) {
          // hash hits, copy the hits into the current list of hits
          // reference the base SS related to the AM level road
          FTKSS &curss = (*iss).second;
#if 0
          cout << 1 << endl;
#endif
          /* copy the hits in this SS in the current roads */
          hits.insert(hits.end(),curss.getHits().begin(),curss.getHits().end());
        }
#if 0
        else {
          cout << 0 << endl;
        }
#endif
      } // end loop over the bit combinations

      static int msgcounter(0);
      if (hits.empty()&&msgcounter<100) {
        msgcounter += 1;
        cerr << "*** No hits found for road while expected in layer " << ipl <<", SSID=" << basessid << ", DC=" << localdcmask << ", Patt# " << curroad->getPatternID() << endl;
      }
      curroad->addHits(ipl,hits);

      // increment the mask offset going to the next layer
      maskoffset += ndcbits;
    } // end plane loop
  }
  else { // no DC matching
    for ( int ipl=0;ipl!=curroad->getNPlanes();++ipl) { // plane loop
      int ssid = curroad->getSSID(ipl);

      // do we expect hits in this layer?
      map< int ,FTKSS>::iterator iss = m_ssmaps[ipl].find(ssid);
      // these conditions aren't identical in CM architecture:
      //    4L roadwarrior may set a bitmask even though ss container has hits in this plane!
      if (iss==m_ssmaps[ipl].end() || curroad->hasHitOnLayer(ipl)== false ) {
        // not found
        continue ;
      }


      // reference the base SS related to the AM level road
      FTKSS &curss = (*iss).second;

      /* copy the hits in this SS in the current roads */
      curss.copyHitsToRoad(curroad,ipl);
    } // end plane loop
  }
}


/** this method clean the coordinate content of a road previously
    composed using attachHits() method */
void FTKRoadStream::detachHits( int ipos)
{
  FTKRoad *curroad = getRoad(ipos);
  assert(curroad);
  curroad->clearHits();
}


/** build the kd Tree */
FTKRoadKDTree *FTKRoadStream:: buildKDTree() {
 
  // if the list of the road is empty return 0
  if (m_nroads==0) return 0;

  // the first road is the head node
  FTKRoad *head = getRoad(0);
  FTKRoadKDTree *kd_tree = new FTKRoadKDTree(head->getNPlanes(),head,0);

  for ( int iroad=1;iroad!=m_nroads;++iroad) {
    kd_tree->addNode(getRoad(iroad),iroad);
  }

  return kd_tree;
}

void FTKRoadStream::buildRoadMap() {

  m_roadIdMap.clear();
  for ( int iroad =0;iroad!=m_nroads;++iroad) { // loop over the roads
    FTKRoad *cur_road = getRoad(iroad);

    if (!cur_road) continue ; // a null pointer is possible
    m_roadIdMap[std::make_pair<int , int>(cur_road->getRoadID(),cur_road->getBankID())] = iroad;
  }

}


const FTKSS& FTKRoadStream::getUnusedSS( int pl, int id) const
{
  static FTKSS empty;

  // search for the plane
  map< int , map< int ,FTKSS> >::const_iterator iussplane = m_ssmaps_unused.find(pl);
  if (iussplane!=m_ssmaps_unused.end()) {
    // search for the SS id
    map< int,FTKSS>::const_iterator iuss = (*iussplane).second.find(id);
    if (iuss!=(*iussplane).second.end()) {
      return (*iuss).second;
    }
  }

  return empty;
}



/** This method computed all the hit combinations
    and for each return a MultiTruth element with
    the best barcode and weight related to a
    combination */
int FTKRoadStream::computeUnusedSegmentsTruthList()
{
  // get the number of unused planes with hits
  int nuplanes = getNUnusedPlanes();

  // cut the streams with less than 3 unused layer with hits
  if (nuplanes<3) return 0;

  // iterators to loop to fill the combinations
  int *plid = new int [nuplanes];
  vector<FTKHit> *hitlist  = new vector<FTKHit>[nuplanes];
  vector<FTKHit>::const_iterator *startlist = new vector<FTKHit>::const_iterator[nuplanes];
  vector<FTKHit>::const_iterator *position = new vector<FTKHit>::const_iterator[nuplanes];
  vector<FTKHit>::const_iterator *endlist = new vector<FTKHit>::const_iterator[nuplanes];


  std::map< int, std::map< int ,FTKSS> >::const_iterator iuplane = m_ssmaps_unused.begin();
  std::map< int, std::map< int ,FTKSS> >::const_iterator iuplane_end = m_ssmaps_unused.end();
 
 
  // compute the number of combinations
  int ncombs(1); 
  int ip(0);
  for (;iuplane!=iuplane_end;++iuplane,++ip) {
    plid[ip] = (*iuplane).first;
    std::map< int,FTKSS>::const_iterator iss = (*iuplane).second.begin();
    std::map< int,FTKSS>::const_iterator iss_end = (*iuplane).second.end();
    for (;iss!=iss_end;++iss) {
      const FTKSS &ss = (*iss).second;
      hitlist[ip].insert(hitlist[ip].end(),
                     ss.getHits().begin(),ss.getHits().end());
    }
    startlist[ip] = position[ip] = hitlist[ip].begin();
    endlist[ip] = hitlist[ip].end();
    if (hitlist[ip].size()>0)
      ncombs *= hitlist[ip].size();
  }
 
  if (m_evtidx) {
    delete [] m_evtidx;
    delete [] m_barcode;
    delete [] m_frac;
  }
  m_evtidx = new long int [ncombs];
  m_barcode = new long int [ncombs];
  m_frac = new float[ncombs];

  for ( int icomb=0;icomb!=ncombs;++icomb) { // combination loop   
    vector<MultiTruth> tmpmtv(nuplanes);

    for ( int iup=0;iup!=nuplanes;++iup) { // plane loop
      // get the truth for the current hit
      MultiTruth this_mt( (*position[iup]).getTruth() );
      // equalize  multiple truths
      this_mt.assign_equal_normalization();
      // add in the temporary list
      tmpmtv.push_back(this_mt);
    } // end plane loop

    // build the final truth accumating the partial results
    MultiTruth mt( std::accumulate(tmpmtv.begin(),tmpmtv.end(),MultiTruth(),MultiTruth::AddAccumulator()) );

    MultiTruth::Barcode tbarcode;
    MultiTruth::Weight tfrac(0);
    mt.best(tbarcode,tfrac);
    m_evtidx[icomb] = tbarcode.first;
    m_barcode[icomb] = tbarcode.second;
    m_frac[icomb] = tfrac;

    /* compose the next combination */
    for ( int iup=0;iup!=nuplanes;++iup) {
      if (startlist[iup]==endlist[iup]) // empty hit
        continue ;
      
      // move to the next
      ++position[iup];
      if (position[iup]!=endlist[iup]) {
        // possible combination
        break ;
      }
      else {
        // restart on this plane, move the next plane
       position[iup] = startlist[iup];
      }      
    }
  } // end combination loop

  delete [] plid;
  delete [] hitlist;
  delete [] startlist;
  delete [] position;
  delete [] endlist;

  return ncombs;
}


FTKRoadStream::FTKRoadCursor:: FTKRoadCursor() :
  TObject(), m_stream(0x0), m_road(0x0), m_position(-1), m_first( false )
{}


/** method used to iteratote in the road */
FTKRoadStream::FTKRoadCursor:: FTKRoadCursor(FTKRoadStream *stream,
                         FTKRoad *road, int pos,
                         bool init) :
  TObject(), m_stream(stream), m_road(road), m_position(pos), m_first(init)
{
  // nothing to do
}

FTKRoadStream::FTKRoadCursor:: ~FTKRoadCursor()
{}

FTKRoadStream::FTKRoadCursor:: FTKRoadCursor( const FTKRoadCursor &copy) :
  TObject(copy),
  m_stream(copy.m_stream),
  m_road(copy.m_road), m_position(copy.m_position),
  m_first(copy.m_first)
{}

FTKRoad* FTKRoadStream::FTKRoadCursor:: operator()()
{
  if (m_first) {
    if (m_position>0) m_stream->detachHits(m_position-1);
    m_stream->attachHits(m_position);
  }
  return m_road;
}


/** This function navigate trought the roads, at the different TSP level,
    and generate a linear list fo roads. The roads in this list can be used for
    the fitting */
unsigned FTKRoadStream::prepareFitList()
{
  // initize the list
  m_fit_list.clear();

  for ( int iroad=0;iroad!=m_nroads;++iroad) { // AM road loop
    // get the pointer the AM road at this position
    FTKRoad *curAMRoad = getRoad(iroad);
    curAMRoad->setAMRoad(curAMRoad); // reflecting

    if (curAMRoad->getNSubRoads()==0) {
      // add the AM road to the list
      m_fit_list.push_back(FTKRoadCursor( this ,curAMRoad,iroad, true));
    }
    else {
      // if there are sub-roads search for the first at the lowest level
      expandSubRoads(curAMRoad,curAMRoad,iroad, true );
    }
  } // end AM road loop

  // initialize the iterator
  m_fit_iter = m_fit_list.begin();

  // return the begin iterator
  return m_fit_list.size();
}


/** return the current road and move the cursor to the next road */
FTKRoad* FTKRoadStream:: fetchRoad()
{
  FTKRoad *res = 0x0;
  if (m_fit_iter!=m_fit_list.end()) {
    res = (*m_fit_iter)();
    ++m_fit_iter;
  }
  else if (m_nroads>0) {
    // clean the last road, if there are roads
    detachHits(m_nroads-1);
  }

  return res;
}

/** get the first road and move the cursor to the next road */
FTKRoad* FTKRoadStream:: fetchFirstRoad()
{
  FTKRoad *res = 0x0;
  m_fit_iter = m_fit_list.begin();
  if (m_fit_iter!=m_fit_list.end()) {
    res = (*m_fit_iter)();
    ++m_fit_iter;
  }
  else if (m_nroads>0) {
    // clean the last road, if there are roads
    detachHits(m_nroads-1);
  }

  return res;
}


/** Expand the sub-roads of this road and if these are final roads are apppended to the fit list,
    if there are further level the navigation continues  */
void FTKRoadStream::expandSubRoads(FTKRoad *AMroad, FTKRoad *uproad, int position, bool first)
{

  // first is true if the road is the one that has to populate the hits for the group

  for ( int iroad=0;iroad!=uproad->getNSubRoads();++iroad) { // sub-roads loop
    FTKRoad *curroad = uproad->getSubRoad(iroad);
    // set the AM road temporary field
    curroad->setAMRoad(AMroad);

    if (curroad->getNSubRoads()==0) {
      m_fit_list.push_back(FTKRoadCursor( this ,curroad,position,first));
      first = false ;
    }
    else {
      expandSubRoads(AMroad,curroad,position,first);
      first = false ;
    }
  } // end sub-roads loop
}
