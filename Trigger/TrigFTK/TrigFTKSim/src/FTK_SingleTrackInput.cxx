/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_SingleTrackInput.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <list>
#include <algorithm>

using namespace std;

FTK_SingleTrackInput::FTK_SingleTrackInput(const FTKPlaneMap *pmap, const FTKRegionMap *rmap, const FTKSSMap *ssmap) :
  FTK_RawInput(pmap), m_rmap(rmap), m_ssmap(ssmap),
  m_sectors(0), m_patterns(0), m_constants(0)
{
  if(rmap) m_nregions = rmap->getNumRegions();
}

FTK_SingleTrackInput::FTK_SingleTrackInput(const FTK_SingleTrackInput& v) :
  FTK_RawInput(v),
  m_rmap(v.m_rmap), m_ssmap(v.m_ssmap),
  m_sectors(0), m_patterns(0), m_constants(0)
{
  if(v.m_rmap) m_nregions = v.m_rmap->getNumRegions();
}

FTK_SingleTrackInput::~FTK_SingleTrackInput()
{
  if(m_sectors) {
    for(int i=0;i<m_nsubregions;i++) 
      {
	delete m_sectors[i];
      }
    delete[] m_sectors;
  }
  if(m_patterns) {
    for(int i=0;i<m_nsubregions;i++) 
      {
	delete m_patterns[i];
      }
    delete[] m_patterns;
  }
  if(m_constants) {
    for(int i=0;i<m_nsubregions;i++) 
      {
	delete m_constants[i];
      }
    delete[] m_constants;
  }
}

//////////////////////////////////////////////////////////////
// Special functions for single muon mode (bank production)
//////////////////////////////////////////////////////////////

/** Emulates filterHits function in TrigFTKLib (to be used with single muons only) 
    Returns -1 if failed
*/
int FTK_SingleTrackInput::filterHits() {
  const bool warn(false);
  std::vector<FTKHit> final_hits;
  std::vector<int> final_hits_cnt;
  
  const unsigned int un_m_nplanes = static_cast<unsigned int>(m_nplanes);
  if(m_hits.size()<un_m_nplanes) return -1;

  final_hits_cnt.resize(un_m_nplanes);
  final_hits.resize(un_m_nplanes);

  for(unsigned int i = 0; i < m_hits.size(); ++i) {
    // Some defs
#define CURHIT final_hits[m_hits[i].getPlane()]
#define CURCNT final_hits_cnt[m_hits[i].getPlane()]

    if(CURCNT == 0) {
      /* Copy hit data */
      CURCNT++;
      CURHIT=m_hits[i];
    } else if(CURCNT == 1) {
      /* Check for overlap */
      CURCNT++;
      const int sdiff = CURHIT.getSection() - m_hits[i].getSection();
      const int pdiff = CURHIT.getPhiModule() - m_hits[i].getPhiModule();
      const int ediff = CURHIT.getEtaModule() - m_hits[i].getEtaModule(); 
      const int section = CURHIT.getSection();
      const int plane = CURHIT.getPlane();
      
      if(CURHIT.getSector() == m_hits[i].getSector()) { // Same module hit, exit
	if(warn) cout << "same sector" << endl;
	return -1;
      } else {

	/* same eta and plane section */
	/* some modules overlap in phi */
	if(ediff == 0 && sdiff == 0) {
	  if(pdiff == 1 || pdiff == -m_rmap->getRegionMapItem(m_nregions-1,plane,section).getTotMod()) {
	    // Do nothing, we got the right one
	  } else if(pdiff == -1 || pdiff == m_rmap->getRegionMapItem(m_nregions-1,plane,section).getTotMod()) {
	    // Swap hits, the new one is the right one
	    CURHIT=m_hits[i];
	  } else {
	    // Not overlap, hits too far in phi
	    if(warn) cout << "far phi" << endl;
	    return -1;
	  }
	  
	  /* sector overlap is a perfectly reasonable situation with forward disks */
	  /* eta and phi will differ in general in this case */
	  /* Take the lower-z hit preferentially (right thing to do? d0/pT tradeoff) */
	  /* But something fishy is going on if we've got two hits on the same disk. */
	} else if (sdiff != 0) {
	  
	  /* If one is barrel and one endcap, it's definitely OK, take the barrel hit */
	  if (m_pmap->getPlane(plane,section).getIsEndcap()
	      && !m_pmap->getPlane(m_hits[i].getPlane(),m_hits[i].getSection()).getIsEndcap()) {
	    // Swap hits, the new one is the right one
	    CURHIT = m_hits[i];
	  } else if (!m_pmap->getPlane(plane,section).getIsEndcap()
		     && m_pmap->getPlane(m_hits[i].getPlane(),m_hits[i].getSection()).getIsEndcap()) {
	    // do nothing, we got the barrel hit

	    /* Two endcap hits : same disk: discard */
	  } else if (m_pmap->getPlane(plane,section).getPDisk()
	      == m_pmap->getPlane(m_hits[i].getPlane(),m_hits[i].getSection()).getPDisk()) {
	    if(warn) cout << "same disk" << endl;
	    return -1;

	  /* Two endcap hits on opposite sides makes no sense: discard */
	  } else if ((CURHIT.getCSide() && m_hits[i].getASide())
		     || (CURHIT.getASide() && m_hits[i].getCSide())) {
	    if(warn) cout << "opposite sides" << endl;
	    return -1;
	    
	    /* Two endcap hits on same side: different disks: take the lower-z */
	  } else {
	    /* If CURHIT disk is a lower number, we've got the right one */ 
	    if (m_pmap->getPlane(plane,section).getPDisk()
		> m_pmap->getPlane(m_hits[i].getPlane(),m_hits[i].getSection()).getPDisk()) {
	      // Swap hits, the new one is the right one
	      CURHIT = m_hits[i];
	    }
	  }

	} else { // ediff != 0: Different eta idx is no good, free and exit
	  if(warn) cout << "eta diff" << endl;
	  return -1;
	}
      }
    } else if(CURCNT > 1) {
      // More than two hits per plane, noisy track, free and exit
      if(warn) cout << "mult hits" << endl;
      return -1;
    }
  }
#undef CURHIT
#undef CURCNT

  // check if we have a hit in each layer
  bool allhits(true);
  for(int i=0; i<m_nplanes; i++) {
    if(final_hits_cnt[i]==0) {
      allhits = false;
      if(warn) cout << "no hits in plane " << i << endl;
      break;
    }
  }

  // replace m_hits with the clean hits that survive the filtering
  if(allhits)
    m_hits = final_hits;
  else
    return -1;
  
  return 0;
}


void FTK_SingleTrackInput::singleMuonMode() {
  // initialize "current sector" and "current pattern" members
  m_sector.setNPlanes(m_nplanes);
  m_sector.setNEvents(1);
  m_pattern.setNPlanes(m_nplanes);
  m_pattern.setNEvents(1);
  // initialize "current track" member
  m_track.setNCoords(m_ncoords);
  m_track.setBitmask(~(1<<(m_ncoords+1)));
  m_track.setNMissing(0);
}

/** reserves structures to store a pattern bank */
void FTK_SingleTrackInput::reservePatterns(int subreg, bool createEmpty) {
  assert(subreg<m_nsubregions);
  if(!m_patterns) {
    m_patterns = new PatternBank*[m_nsubregions];
    for(int i=0;i<m_nsubregions;i++) m_patterns[i]=0;
  }
  if(m_patterns[subreg]) {
    std::cout << "Warning: overwriting patterns in subregion " << subreg << std::endl;
    delete m_patterns[subreg];
  }
  if(createEmpty) {
    m_patterns[subreg] = new PatternBank();
    m_patterns[subreg]->setNPlanes(m_nplanes);
  }
}

/** reserves structures to store a sector bank */
void FTK_SingleTrackInput::reserveSectors(int subreg, bool createEmpty) {
  assert(subreg<m_nsubregions);
  if(!m_sectors) {
    m_sectors = new PatternBank*[m_nsubregions];
    for(int i=0;i<m_nsubregions;i++) m_sectors[i]=0;
  }
  if(m_sectors[subreg]) {
    std::cout << "Warning: overwriting sectors in subregion " << subreg << std::endl;
    delete m_sectors[subreg];
  }
  if(createEmpty) {
    m_sectors[subreg] = new PatternBank();
    m_sectors[subreg]->setNPlanes(m_nplanes);
  }
}

/** loads sector banks for fast sector lookup */
void FTK_SingleTrackInput::loadSectors(int subreg, const char *path) {
  reserveSectors(subreg);
  cout << "Loading " << m_nplanes << "-plane sectors in subregion " << subreg << ":" << endl;
  cout << path << endl;
  m_sectors[subreg] = new PatternBank(path);
  m_sectors[subreg]->InitPreload();
  m_sectors[subreg]->InitLookup(true);
}

/** loads constant banks for fast 8L track fitting lookup */
void FTK_SingleTrackInput::loadConstants(int subreg, const char *path) {
  if(!m_constants) {
    m_constants = new FTKConstantBank*[m_nsubregions];
    for(int i=0;i<m_nsubregions;i++) m_constants[i]=0;
  }
  if(m_constants[subreg]) {
    std::cout << "Warning: overwriting constants in subregion " << subreg << std::endl;
    delete m_constants[subreg];
  }
  cout << "Loading " << m_ncoords << "-coordinate constants in subregion " << subreg << ":" << endl;
  cout << path << endl;
  m_constants[subreg] = new FTKConstantBank(m_ncoords,path);
}

/* Construct a sector out of m_hits and save it in m_sector */
int FTK_SingleTrackInput::getSector() {
  int *ids = new int[m_nplanes];
  for(int i=0; i<m_nplanes; i++) ids[i]=m_hits[i].getSector();
  m_sector.setSSIDs(ids);
  
  delete [] ids;
  return 0;
}

/* Returns a region to which a sector belongs
   This assumes the hits have already been filtered
*/
int FTK_SingleTrackInput::Region() {
  std::vector<int> result(m_nplanes);

  for(int i=0;i<m_nplanes;++i) {

    int phi = m_hits[i].getPhiModule();
    int section = m_hits[i].getSection();

    /* Region 0 is special: some negative phi indices */
    if(phi <= m_rmap->getRegionMapItem(0,i,section).getMax() ||
       phi >= m_rmap->getRegionMapItem(m_nregions-1,i,section).getTotMod() +
       m_rmap->getRegionMapItem(0,i,section).getMin()+1 ) {
      result[i] += 1;
    }
    
    /* Check other regions */
    for(int k = 1; k < m_nregions; ++k) {
      if((phi <= m_rmap->getRegionMapItem(k,i,section).getMax()) &&
	 (phi >= m_rmap->getRegionMapItem(k,i,section).getMin())) {
	result[i] += 1 << k;
      }
    }

  } // loop over planes
  
  int region = result[0];
  for(int k=1; k<m_nplanes; ++k)
    region &= result[k];
  
  /* case of mismatched region */
  if(!region)
    return -1;
  
  // 7 and 0, return 0
  if(region == (1 << (m_nregions-1))+1) // FlagAK - was hardcoded to 129!
    return 0;
  
  int k = 0;
  while((region >> k) != 1)
    ++k;
  
  /* return actual region number */
  return k;
}

/* Check if this sector exists in one of m_sectors banks */
int FTK_SingleTrackInput::findSector() {
  m_subregion=-1;
  getSector();
  m_sector.setSectorID(0);
  for(int sub=0;sub<m_nsubregions;sub++) {
    if(!m_sectors[sub]) continue;
    int sid = m_sectors[sub]->getPatternID(m_sector);
    if(sid!=-1) { m_subregion=sub; m_sector.setID(sid); break; }
  }
  // not found
  if(m_subregion==-1) return -1;
  return 0;
}

int FTK_SingleTrackInput::getPattern() {
  // make sure we have the corresponding sector
  if(findSector()==-1) return -1;
  int *ids = new int[m_nplanes];
  for(int i=0; i<m_nplanes; i++) ids[i]=m_ssmap->getSS(m_hits[i]);
  m_pattern.setSSIDs(ids);
  m_pattern.setSectorID(m_sector.getSectorID());
  m_pattern.setNEvents(1);
  delete [] ids;
  return 0;
}

/* Fits the single muon track
   Assumes sector is already constructed via findSector()
*/
int FTK_SingleTrackInput::performFit() {
  const int sec = m_sector.getID();
  // set the coordinates
  for (int ip=0;ip<m_nplanes;++ip) {
    // retrieve the coordinate in the final array
    int ix = m_pmap->getDim(ip,0);
    int iy = m_pmap->getDim(ip,1);
    // decide if a 1D or 2D hit
    int ndim = iy==-1 ?  1 : 2;
    if (ndim==1) {
      m_track.setCoord(ix,m_hits[ip][0]);
    }
    else {
      m_track.setCoord(ix,m_hits[ip][0]);
      m_track.setCoord(iy,m_hits[ip][1]);
    }
  }
   m_constants[m_subregion]->linfit(sec,m_track);
   // TODO: majority recovery if chi2ndof (passed as parameter) fails!
   return 0;
}

/* Dumps track parameters in Constantinos format */
void FTK_SingleTrackInput::DumpTrack(std::ostream& out) {
  std::streamsize oldprec = out.precision();
  out << m_glob_event << "\t";
  out.precision(10);
  //out.setf(ios_base::scientific);
  out << m_track.getPhi() << "\t" 
      << m_track.getCotTheta() << "\t"
      << m_track.getHalfInvPt();
  out.precision(oldprec);
  //  out << "\t" << m_track.getChi2();
  out << std::endl;
  //  out.unsetf(ios_base::scientific);
}

/* Dumps track parameters into root file */
void FTK_SingleTrackInput::DumpTrack(FTKTrackLookup& out) {
  out.set(m_glob_event,m_track.getPhi(),m_track.getCotTheta(),m_track.getHalfInvPt());
}
