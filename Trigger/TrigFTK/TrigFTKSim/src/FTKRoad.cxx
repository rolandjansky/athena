/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRoad.h"

#include <sstream>
#include <iomanip>
#include <cassert>

using namespace std;

// ALLDCMASK define the default DC mask, with the bit set for all layers
#define ALLDCMASK (~0)


int FTKRoad::m_verbose(0);

FTKRoad::FTKRoad()
  : TObject(), m_DCMatchMode(0), m_roadID(-1),
    m_bankID(-1), m_patternID(-1), m_patternDBID(0), m_sectorID(-1), 
    m_DCmask(ALLDCMASK), m_HLmask(0),
    m_HF_status(0), m_HF_rejected(0),
    m_RW_rejected(0), m_RW_roadID(-1), m_RW_bankID(-1),
    m_nhits(0), m_bitmask(0), 
    m_nplanes(0),
    m_ssid(0x0), m_hlid(0x0), m_hlidoff(0x0),
    m_SubSSMask(0x0),
    m_nsubroads(0),
    m_AMroad(0), m_hits(0), m_hits_mask(),
    m_evtidx(0), m_barcode(0), m_frac(0)
{
  m_subroads = new TClonesArray("FTKRoad",5);
}


FTKRoad::FTKRoad(int uid, int bankid, int pattid, int nplanes,
		 int nhits, unsigned int bitmask)
  : TObject(), m_DCMatchMode(0), m_roadID(uid),
    m_bankID(bankid), m_patternID(pattid),
    m_patternDBID(0), m_sectorID(-1),
    m_DCmask(ALLDCMASK), m_HLmask(0),
    m_HF_status(0), m_HF_rejected(0),
    m_RW_rejected(0), m_RW_roadID(-1), m_RW_bankID(-1),
    m_nhits(nhits), m_bitmask(bitmask),
    m_nplanes(nplanes), m_hlidoff(0x0), m_nsubroads(0),
    m_AMroad(0), m_hits(nplanes), m_hits_mask(),
    m_evtidx(0), m_barcode(0), m_frac(0)
{
  m_ssid = new int[m_nplanes];
  m_hlid = new int[m_nplanes];
  m_SubSSMask = new int[m_nplanes];
  for (int ip=0;ip!=m_nplanes;++ip) m_SubSSMask[ip] = m_ssid[ip] = m_hlid[ip] = 0;
  m_subroads = new TClonesArray("FTKRoad",5);
}


FTKRoad::FTKRoad(const FTKRoad &copy) :
  TObject(copy), 
  m_DCMatchMode(copy.m_DCMatchMode),
  m_roadID(copy.m_roadID),
  m_bankID(copy.m_bankID), m_patternID(copy.m_patternID),
  m_patternDBID(copy.m_patternDBID),
  m_sectorID(copy.m_sectorID), 
  m_DCmask(copy.m_DCmask), m_HLmask(copy.m_HLmask),
  m_HF_status(copy.m_HF_status), m_HF_rejected(copy.m_HF_rejected),
  m_RW_rejected(copy.m_RW_rejected), m_RW_roadID(copy.m_RW_roadID),
  m_RW_bankID(copy.m_RW_bankID),
  m_nhits(copy.m_nhits),
  m_bitmask(copy.m_bitmask),
  m_nplanes(copy.m_nplanes),
  m_hlidoff(0x0),
  m_nsubroads(copy.m_nsubroads),
  m_AMroad(0),
  m_hits(copy.m_hits), 
  m_hits_mask(copy.m_hits_mask),
  m_evtidx(0), m_barcode(0), m_frac(0)
{

  m_ssid = new int[m_nplanes];
  m_hlid = new int[m_nplanes];
  m_SubSSMask = new int[m_nplanes];

  for (int ipl=0;ipl<m_nplanes;++ipl) {
    m_ssid[ipl] = copy.m_ssid[ipl]; 
    m_hlid[ipl] = copy.m_hlid[ipl];
    m_SubSSMask[ipl] = copy.m_SubSSMask[ipl];
  }
  
  m_subroads = new TClonesArray(*copy.m_subroads);
}


FTKRoad::~FTKRoad()
{
  if (m_ssid) {
    delete [] m_ssid;
    delete [] m_hlid;
    delete [] m_SubSSMask;
  }

  if (m_evtidx) {
    delete [] m_evtidx;
    delete [] m_barcode;
    delete [] m_frac;
  }

  if (m_hlidoff) delete [] m_hlidoff;

  delete m_subroads;
}


void FTKRoad::setBankID(int v)
{ 
  m_bankID = v; 

  for (int isr=0;isr!=m_nsubroads;++isr) 
    getSubRoad(isr)->setBankID(m_bankID);
}


/** set the number of planes and sizes the array */
void FTKRoad::setNPlanes(int dim)
{
   // check if the nplanes if >0, this means that roads were already allocated
   if (m_nplanes>0) {
      m_hits.clear();
      delete [] m_ssid;
      delete [] m_hlid;
      delete [] m_SubSSMask;
   }

  if (m_hlidoff) delete [] m_hlidoff;

   // set the current number of planes
   m_nplanes = dim;
   
   // resize SS id array and reserved hit space
   m_ssid = new int[m_nplanes];
   m_hlid = new int[m_nplanes];
   m_SubSSMask = new int[m_nplanes];
   m_hits.resize(m_nplanes);
}


const int& FTKRoad::getHLIDOffset(int pl)
{
  if (!m_hlidoff) { // init the offsets array
    m_hlidoff = new int[m_nplanes];
    m_hlidoff[0] = 0;
    for (int ip=1;ip!=m_nplanes;++ip) 
      m_hlidoff[ip] = m_hlidoff[ip-1]+m_hlid[ip-1];
  }

  return m_hlidoff[pl];
}
unsigned int FTKRoad::getNHits(int pl) const
{ 
  if (!hasHitOnLayer(pl)) return 0;

  assert(m_hits.size()>0);
  return m_hits[pl].size();
}


/** set the informations on the road that killed that one */
void FTKRoad::setKillerRoad(const FTKRoad *road)
{
  m_RW_roadID = road->m_roadID;
  m_RW_bankID = road->m_bankID;
}

/** this function prepares the hits array */
void FTKRoad::prepareHits()
{
  m_hits.clear();
  m_hits.resize(m_nplanes);
  for (int isr=0;isr!=m_nsubroads;++isr) { // subroads loop
    FTKRoad *cursr = getSubRoad(isr);
    cursr->prepareHits();
  } // end subroads loop
}


FTKRoad* FTKRoad::addSubRoad(const FTKRoad &road)
{
  return new ((*m_subroads)[m_nsubroads++]) FTKRoad(road);
}

/** add a hit into this road */
void FTKRoad::addHits(int pl, const vector<FTKHit> &x)
{
  assert(m_hits.size()>0);
  m_hits[pl] = x;
}

string FTKRoad::getBitmaskASCII() const
{
  ostringstream asciimask;

  for (int ipl=m_nplanes-1;ipl!=-1;--ipl) { // inverse plane loop
    asciimask << ((m_bitmask&(1<<ipl))>>ipl);
  } // end inverse plane loop

  return asciimask.str();
}


/** clear the content of the array containing the hit lists */
void FTKRoad::clearHits()
{
  m_hits.clear();
  for (int isr=0;isr!=m_nsubroads;++isr) { // subroads loop
    FTKRoad *cursr = getSubRoad(isr);
    cursr->clearHits();
  } // end subroads loop  
}

/** this operator is used to order in the road list the hits.
    The road with more fired bits is greater. */
bool FTKRoad::operator>(const FTKRoad& right) const
{
  // the road with more good hits win
  if (m_nhits>right.m_nhits) return true;
  // if the roads are using the same number of bins 
  // the one with more coverage win
  else if (m_nhits==right.m_nhits && m_patternID<right.m_patternID) 
    return true;
  // otherwise the right member is greater or equal
  else return false;
}


bool FTKRoad::operator<(const FTKRoad& right) const
{
  // the road with more good hits win
  if (m_nhits<right.m_nhits) return true;
  // if the roads are using the same number of bins 
  // the one with more coverage win
  else if (m_nhits==right.m_nhits && m_patternID>right.m_patternID) 
    return true;
  // otherwise the right member is less or equal
  else return false;
}


/** Compare method ROOT compliant. In particular because this is made
    having in mind road sorted to optimize RW the result is reversed,
    because the TClonesArray object doesn't have a ::reverse() method
    as the std::list class. */
Int_t FTKRoad::Compare(const TObject* pobj) const
{
  const FTKRoad *proad = dynamic_cast<const FTKRoad*>(pobj);
  if (!proad) return 0;

  if ((*this)>(*proad)) return -1;
  else if((*this)<(*proad)) return 1;
  else return 0;
}


/*
 * This method applies a static RW method, that compares two patterns,
 * and decides if they seem similar or not. two roads are similar if
 * they differ by no more than one SSID on a plane in which the roads
 * use different modules.
 */
bool FTKRoad::doStaticRoadWarrior(const FTKRoad &patt1,
				  const PatternDefinition &sec1,
				  const FTKRoad &patt2,
				  const PatternDefinition &sec2)
{
  // this function is called hundreds of billions of times during
  // a typical 10^34 merge job, so make sure it is fast.
  const int nplanes(patt1.getNPlanes());
  int ndiffs(0);
  int diff_plane(-1); // do not waste time initializing this.
  for (int i=0;i!=nplanes;++i) { 
    // do the two patterns share the same SS? if not count it.
    // these are two integer array lookups.
    if( patt1.getSSID(i) != patt2.getSSID(i) ) {
      if( ndiffs==1 ) { return false; }
      ++ndiffs;
      diff_plane = i;
    }
  } // end loop over the planes
  // return true only if there is a single difference.
  if( ndiffs!=1 ) { return false; }
  // return true only if the difference is from different modules.
  return( sec1.getSSID(diff_plane) != sec2.getSSID(diff_plane) );
}

ostream& operator<<(ostream &out, const FTKRoad &road)
{
  out << "ROAD " << std::setw(4) << road.m_roadID;
  out << std::setw(6) << road.m_bankID;
  out << std::setw(9) << road.m_patternID << " ";
  out << std::setw(9) << road.m_patternDBID << " ";
  out << std::setw(4) << road.m_sectorID;
  out << " ; " << road.m_nhits << ": " << std::setw(road.m_nhits+1) << road.getBitmaskASCII();
  out << " ; " << "flags=[" 
      <<  road.m_HF_rejected << "," << road.m_HF_status << "," << road.m_RW_rejected
      << "(" << road.m_RW_roadID << "," << road.m_RW_bankID << ")]";
  out << "\n";
  out << std::setw(8) << "ss=(";
  for (int i=0;i<road.m_nplanes-1;++i)
    out << road.m_ssid[i] << ',';
  out << road.m_ssid[road.m_nplanes-1] << ')';

  if (road.m_verbose>0) {
    out << endl << "+ Sub roads found = " << road.m_nsubroads << endl;
    for (int isr=0;isr!=road.m_nsubroads;++isr) {
      road.setVerbose(road.getVerbose()-1);
      out << "+ [" << isr << "] " << endl;
      out << "+ " << *((const FTKRoad*) road.getSubRoad(isr)) << endl;
      road.setVerbose(road.getVerbose()+1);
    }
  }
  return out;
}


int FTKRoad::getNCombs()
{
  if (m_hits.empty()) {
    return -1;
  }

  unsigned ncombs(1);
  for (int ip=0;ip!=m_nplanes;++ip) { // plane loop
    // the second check is temporary needed because in the TSP
    // simulation not all the bitmasks are supposed to be corretly
    // set.
    if (m_bitmask&(1<<ip) && !m_hits[ip].empty()) { 
      ncombs *= m_hits[ip].size();
    }
  } // end plane loop

  return ncombs;
}

/** This method computed all the hit combinations
    and for each return a MultiTruth element with
    the best barcode and weight related to a
    combination */
int FTKRoad::computeTruthList()
{
  // iterators to loop to fill the combinations
  vector<FTKHit>::const_iterator *startlist = new vector<FTKHit>::const_iterator[m_nplanes];
  vector<FTKHit>::const_iterator *position = new vector<FTKHit>::const_iterator[m_nplanes];
  vector<FTKHit>::const_iterator *endlist = new vector<FTKHit>::const_iterator[m_nplanes];


  // compute the number of combinations
  int ncombs(1);
  for (int ip=0;ip!=m_nplanes;++ip) {
    
    if (m_bitmask&(1<<ip)) {
      ncombs *= m_hits[ip].size();
      startlist[ip] = position[ip] = m_hits[ip].begin();
      endlist[ip] =  m_hits[ip].end();
    }
    else {
      startlist[ip] = endlist[ip] = position[ip] = vector<FTKHit>::const_iterator(); // empty iterator, this will flag empty lists
    }
  }

  if (m_evtidx) {
    delete [] m_evtidx;
    delete [] m_barcode;
    delete [] m_frac;
  }
  m_evtidx = new long int[ncombs];
  m_barcode = new long int[ncombs];
  m_frac = new float[ncombs];

  for (int icomb=0;icomb!=ncombs;++icomb) { // combination loop    
    vector<MultiTruth> tmpmtv(m_nplanes);

    for (int ip=0;ip!=m_nplanes;++ip) { // plane loop
      if (! (m_bitmask & (1<<ip))) continue; // skip empty

      // get the truth for the current hit
      MultiTruth this_mt( (*position[ip]).getTruth() );
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
    for (int ip=0;ip!=m_nplanes;++ip) {
      if (startlist[ip]==endlist[ip]) // empty hit
	continue;
       
      // move to the next
      ++position[ip]; 
      if (position[ip]!=endlist[ip]) {
	// possible combination
	break; 
      }
      else {
	// restart on this plane, move the next plane
	position[ip] = startlist[ip]; 
      }	 
    }
  } // end combination loop

  delete [] startlist;
  delete [] endlist;
  delete [] position;
  return ncombs;
}

void FTKRoad::Print(Option_t* /*option = ""*/) const
{
  cout << *this << endl;
}

#undef ALLDCMASK
