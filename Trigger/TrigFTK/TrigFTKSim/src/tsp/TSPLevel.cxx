/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/tsp/TSPLevel.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"

#include <boost/lexical_cast.hpp>

#include <TTree.h>
#include <TBranch.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

using namespace boost;
using namespace std;

/** This constructors provides a minimal initialization of the internal variable, it is meant
  for constructions where the FTKTSPBank object can set all the fields, as it happens for
  cached banks */
TSPLevel::TSPLevel(int bankID, int nAMpatterns, unsigned threshold, FTKSSMap* ssTopMap, FTKSSMap* ssBottomMap, map<int,FTKSS> *ssmaps, bool onlydc) :
  m_bankID(bankID), m_threshold(threshold), m_nroads(0), m_npatterns(0),
  m_dcOnly(onlydc),
  m_subPatternID(nAMpatterns), m_hbmask(nAMpatterns), m_dcmask(nAMpatterns),
  m_subssmasks(nAMpatterns), m_splitted_ssmap(ssmaps),
  run_ipatt(0), run_fatherID(0), run_dcmask(0), run_hbmask(0),
  run_subssmasks(), run_TSPPatternIDs(), run_hbmasks()
{
  m_tspmap.generate(ssTopMap, ssBottomMap);
  m_nplanes = m_tspmap.getNPlanes();
}

/** ROOT file format constructor */
TSPLevel::TSPLevel(TFile *file, int bankID, FTKTSPBank *AMbank,
		   const list<AMSelection> &AMlist, unsigned threshold, FTKSSMap* ssTopMap, FTKSSMap* ssBottomMap, map<int,FTKSS> *ssmaps, bool onlydc) :
  m_bankID(bankID), m_threshold(threshold), m_nroads(0), m_npatterns(0),
  m_dcOnly(onlydc),
  m_splitted_ssmap(ssmaps),
  run_fatherID(0),
  run_subssmasks(), run_TSPPatternIDs(), run_hbmasks(){

  // prepare the statement to require, for each AM pattern, the list of TSP patterns
  // generate the TSP map description
  m_tspmap.generate(ssTopMap, ssBottomMap);
  m_nplanes = m_tspmap.getNPlanes();

  // prepare the statement to require, for each AM pattern, the list of TSP patterns
  int nMaxPatterns = AMbank->getNPatternsTSP();
  int MinCoverage = AMbank->getTSPMinCoverage();
  // dummy lines to remove warnings
  nMaxPatterns *= 1;
  MinCoverage *= 1;

  cout << "Sub-patterns read and DC mask creation in TSP processor: [" << flush;
  // define the step size
  int ipatt_step = (AMlist.size()+9)/10;
  if (!ipatt_step) ipatt_step = 1;


  // patterns counter, at the first is incremented indeed -1 is the good initialization
  run_ipatt = -1;
  // initialize DC/TSP structures, used to keep compatibility with the DB version
  TTree *TSPRel = (TTree*) file->Get("TSPRelation0");
  UInt_t hbmask;

  TBranch *hbmask_b;
  TSPRel->SetBranchAddress("HalfPlaneMask",&hbmask,&hbmask_b);

  // preload all the hbmask values to allow an efficient random access to the data
  vector<UInt_t> TSPRelVec(TSPRel->GetEntries());
  Long64_t ipatt_end = TSPRel->GetEntriesFast();
  for (Long64_t ipatt=0;ipatt!=ipatt_end;++ipatt) { // TSP pattern loop
    hbmask_b->GetEntry(ipatt);
    TSPRelVec[ipatt] = hbmask;
  } // end TSP pattern loop

  storeAMInfo();  // first call to initialize internal data structures
  list<AMSelection>::const_iterator iAM = AMlist.begin();

  for(;iAM!=AMlist.end();++iAM) { // AM pattern loop
    /* The 2nd field of the pair is the list of sub-patterns,
       looping over this vector completes the building of the TSP level
       and the DC information */
    vector<int>::const_iterator iTSP = (*iAM).getTSPIDs().begin();

    /* The vector has at leat one child, this is the reference for the
       DC mask and starts the information block for the AM pattern */
    run_hbmask = TSPRelVec[*iTSP];
    ++run_ipatt;

    // change the AM pattern ID
    run_fatherID = (*iAM).getAMID();
    run_hbmasks.push_back(run_hbmask);
    run_TSPPatternIDs.push_back(*iTSP);

    // check the progress bar increment
    if (run_ipatt%ipatt_step==0) cout << run_ipatt/ipatt_step << flush;

    // move to the next
    ++iTSP;
    m_npatterns += 1;

    for (;iTSP!=(*iAM).getTSPIDs().end();++iTSP) { // TSP pattern loop
      // read the current TSP pattern
      const UInt_t &tmp_hbmask = TSPRelVec[*iTSP];

      run_dcmask |= run_hbmask ^ tmp_hbmask;

      /* additional hbmasks and pattern ids are saved only if the
	 m_dcOnly flag is false */
      if (!m_dcOnly) {
	run_hbmasks.push_back(tmp_hbmask);
	run_TSPPatternIDs.push_back(*iTSP);
      }

      m_npatterns += 1;
    } // end TSP pattern loop

    // store all the variables for this AM pattern
    storeAMInfo();
  }

  ++run_ipatt; // only for debugging purpose

  cout << "]" << endl;

  FTKSetup::getFTKSetup().usageStat();

}


void TSPLevel::storeAMInfo()
{
  // this function is also used for initialization, check if it is the case
  if (run_ipatt!=-1) {
    // Add the HB and DC mask for this pattern to the vector
    m_hbmask.push_back(run_hbmasks);
    m_dcmask.push_back(run_dcmask);
    // add the DB ID of the TSP patterns
    m_subPatternID.push_back(run_TSPPatternIDs);

    // prepare the vector of the SS mask for this AM pattern
    m_subssmasks.push_back(vector<unsigned int>());
    // get the reference to be used next
    vector<unsigned int> &subss_masks = m_subssmasks.back();

    for (int iplane=0;iplane!=m_nplanes;++iplane) { // plane loop
      // retrieve the information of the TSP in this plane
      const int &ndim = m_tspmap.getDim(iplane);
      const int &bitoffset = m_tspmap.getBitOffset(iplane);

      // total number of positions in a layer
      const int &npos = m_tspmap.getNPositions(iplane);

      // final mask of the allowed regions in the AM SS
      unsigned int mask(0);
      if (ndim==1) {
        // get the number of the bits
        const int &nbits = m_tspmap.getNBits(iplane);

        // SCTs have a single linear mask
        mask = evaluateSubSSMask(nbits,bitoffset,run_dcmask,run_hbmask);
      } // end SCT encoding block
      else if (ndim==2) {
        /* if the layer has 2 dimensions each one has a different number of bits,
	   the X mask is at the begin, the Y mask is after. The result for
	   the allowed sub-SS is obtained crossing these two information */
        const int &nbitsX = m_tspmap.getNBits(iplane,0);
        const int &nbitsY = m_tspmap.getNBits(iplane,1);

        // fill the X information
        int maskX = evaluateSubSSMask(nbitsX,bitoffset,run_dcmask,run_hbmask);

        // fill the Y information
        int maskY = evaluateSubSSMask(nbitsY,bitoffset+nbitsX,run_dcmask,run_hbmask);

        if (!maskY) { // this means the TSP don't use any bit in Y
          mask = maskX;
        }
        else if (!maskX) { // no bit in X
          mask = maskY;
        }
        else { 	// derive a matrix from the 2 vectors
          int nposX = 1<<nbitsX;
          int nposY = 1<<nbitsY;
          for (int ipX=0;ipX!=nposX;++ipX) {
            for (int ipY=0;ipY!=nposY;++ipY) {
              int bitval = (((maskX&(1<<ipX))>>ipX)&((maskY&(1<<ipY))>>ipY));
              mask |= bitval<<(ipX+nposX*ipY);
            }
          }
        }
      } // end Pixel encoding block
      else {
        FTKSetup::PrintMessageFmt(ftk::sevr,"TSP with number of dimension %d is not allowed",ndim);
      }

      /* check is a valid mask: a valid mask has at least 1 bit set,
	 there isn't any bit further the maximum set */
      unsigned int maskForInvalidBits=~0<<npos;
      if (npos==32) maskForInvalidBits = 0;
      if (npos>32) FTKSetup::PrintMessageFmt(ftk::sevr,"Error: npos>32 (i.e. more than 5 DC bits) is an invalid configuration. npos=%d\n",npos);
      if (!mask || mask&(maskForInvalidBits)) {
        FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the Sub-SS mask for pattern %d, mask[%d]=%d\n",run_ipatt,iplane,mask);
      }
      // add the mask for this layer
      subss_masks.push_back(mask);
    } // end plane loop
  } // end storing block

  // reset all the run time variables
  run_dcmask = 0;
  run_hbmask = 0;
  // ensure the clear of the running lists
  run_hbmasks.clear();
  run_TSPPatternIDs.clear();
}

/** This method evalute the SS mask for the sub-SS position in the 1D case,
    good for the SCTs and each dimension of he pixels */
int TSPLevel::evaluateSubSSMask(int nbits, int position, int dcmask, int hbmask)
{
  // prepare the result
  int mask(0);
  if (!nbits) return 0;

  // build mask to select only the first nbits
  const int ref_ok = ~(~0<<nbits);
  // shift the masks to remove the previous bits
  dcmask >>= position;
  hbmask >>= position;

  int nfields = 1<<nbits;
  for (int ifield=0;ifield!=nfields;++ifield) {
    // convert the field position in GC representation
    int ifieldGC = m_tspmap.IEEE2GC(nbits,ifield);

    /* to set the mask check that the binary representation of
       the field position is confirmed by DC and HB.
       The first part returns the bits in common between the field position
       and tbe hbmask. The OR ensure the agreement between the bits with the
       DC. The test succeeds if all the bits assigned to the layer are ok. */
    if ( (((~(ifieldGC^hbmask))|dcmask)&ref_ok) == ref_ok) mask |=  1<<ifield;
  }

  return mask;
}

/** This method get a road, fired in the previous level, and use
    the internal position of the hits in the SS to select which
    sub-patterns fire.
    The fired sub-roads are nested in the original road */
void TSPLevel::simulate(FTKRoad& road){
  // get the begin and the end of the hbmask
  vector<int>::const_iterator hbmask_i = m_hbmask[road.getPatternID()].begin();
  vector<int>::const_iterator hbmask_e = m_hbmask[road.getPatternID()].end();
  // store the maximum number of hits in the sub-roads
  int maxhits = -1;
  /* store the bitmask of the planes used by the subroads, this
     accumulate all the active planes, could maybe contains more acrive
     bits than the maximum number of used planes */
  unsigned fullbitmask(0);

  for (unsigned i=0;hbmask_i!=hbmask_e;++hbmask_i,++i) { // sub-pattern loop
    // get the bit representation of the internal positions
    const int &hbmask = *hbmask_i;
    // the number off fired planes is set to the maximum possible value
    int firedPlanes = road.getNHits(); // the number of hits cannot execeed the number of fired layers for he original roads
    // the starting layer bitmask is the mask of the parent road
    unsigned bitmask = road.getBitMask();
    for(int iplane = 0; iplane != m_nplanes; ++iplane) { // plane loop
      // check the plane has hitsa
      if (!road.hasHitOnLayer(iplane)) continue; // skip if not

      // get the reference to the current SS using the previous level sementation
      FTKSS& ss = m_splitted_ssmap[iplane][road.getSSID(iplane)];

      // in the bitmask filter the position for this layer
      int pos = (hbmask>>m_tspmap.getBitOffset(iplane)) & (~(~0<<m_tspmap.getNBits(iplane)));

      // the position value has to be converted in binary form from the Gray encoding
      pos = m_tspmap.getPosition(iplane,pos);

      // if this position is empty decrement the counter and update the bitmask
      if (!ss.hasHits(pos))	{
	firedPlanes -= 1;
	bitmask &= (~(1<<iplane));
      }

      // if one patern goes below threshold the rest of the loop is useless
      if (firedPlanes<m_threshold) break;
    } // end loop

    if(firedPlanes >= m_threshold) {
      FTKRoad subroad(m_nroads++, road.getBankID(), m_subPatternID[road.getPatternID()][i], m_nplanes, firedPlanes, bitmask);
      subroad.setPatternDBID(subroad.getPatternID()); // there is no difference at this level between 2 Ids
      subroad.setSectorID(road.getSectorID());
      for (int ip=0;ip!=m_nplanes;++ip) {
	int pos = (hbmask>>m_tspmap.getBitOffset(ip)) & (~(~0<<m_tspmap.getNBits(ip)));
	// the "pos" value is encoded using the Gray encoding, has to trasformed in standard binary code
	pos = m_tspmap.getPosition(ip,pos);

	subroad.setSSID(ip,pos);
	subroad.setHLID(ip,0);
      }

      // update the maximum number of used planes
      if (firedPlanes>maxhits) maxhits = firedPlanes;
      // update the full bitmask
      fullbitmask |= bitmask;

      // append this TSP road to the original AM road
      road.addSubRoad(subroad);
    }
  } // end sub-pattern loop

  if (maxhits>0) {
    // update the number hits and the bitmask for the AM road
    road.setNHits(maxhits);
    // update the bitmask
    road.setBitMask(fullbitmask);
  }
}


/** This function, giving a pattern, fill this with all its information
    related with this TSP level */
void TSPLevel::fillPatternInfo(int ipatt, FTKPattern &patt, bool writeTSP)
{
  const vector<int> &hbmask = m_hbmask[ipatt];

  // set DC and HB masks
  patt.setDCMask(m_dcmask[ipatt]);
  patt.setHBMask(hbmask[0]);

  // prepare to loop over the sub-patterns
  vector<int>::const_iterator hb_i = hbmask.begin();
  vector<int>::const_iterator hb_e = hbmask.end();

  vector<int>::const_iterator dbid_i = m_subPatternID[ipatt].begin();

  // prepare the constant field of the
  // because the subpatterns are special, with just one 32 bit field
  FTKPattern tmpspatt(1);
  tmpspatt.setSectorID(-1);
  // the pattern loop doesn't happen if writeTSP is 0
  for (;hb_i!=hb_e&&writeTSP;++hb_i,++dbid_i) { // sub-patterns loop
    tmpspatt.setSSID(0,*hb_i);
    tmpspatt.setPatternDBID(*dbid_i);

    patt.addSubPattern(tmpspatt);
  } // end sub-patterns loop
}


/** this function hs the reverse effect of fillPatternInfo: from an FTKPattern object,
  meant as AM pattern, fills the related information for DC and TSP patterns */
int TSPLevel::getPatternInfo(int ipatt, const FTKPattern *ampatt, bool readTSP)
{
  // retrieve the DC mask
  m_dcmask[ipatt] = ampatt->getDCMask();
  // prepare the HB mask to describe eventual children
  vector<int> hbmask;
  vector<int> subpattID;

  int nsubs = ampatt->getNSubPatterns();
  if (nsubs && readTSP) {
    /* in this case the whole structure with the sub-patterns has been reported
      and the information on the reference HB and ID of all the patterns is in the
       bank */
    for (int isub=0;isub!=nsubs;++isub) { // loop over the sub-patterns
      // get the pointer to the current pattern
      const FTKPattern *tmppatt = ampatt->getSubPattern(isub);

      hbmask.push_back(tmppatt->getSSID(0));
      subpattID.push_back(tmppatt->getPatternDBID());
    } // end loop over the sub-patterns
  }
  else {
    // if there aren't sub-pattern the HB information is retrieved from the pattern itself
    hbmask.push_back(ampatt->getHBMask());
    subpattID.push_back(-1); // it means not a real pattern
  }

  // set the information on hbmask and ID for the TSP patterns
  m_hbmask[ipatt] = hbmask;
  m_subPatternID[ipatt] = subpattID;


  vector<unsigned int> &subss_masks = m_subssmasks[ipatt];

  for (int iplane=0;iplane!=m_nplanes;++iplane) { // plane loop
    // retrieve the information of the TSP in this plane
    const int &ndim = m_tspmap.getDim(iplane);
    const int &bitoffset = m_tspmap.getBitOffset(iplane);

    // total number of positions in a layer
    const int &npos = m_tspmap.getNPositions(iplane);

    // final mask of the allowed regions in the AM SS
    unsigned int mask(0);
    if (ndim==1) {
      // get the number of the bits
      const int &nbits = m_tspmap.getNBits(iplane);

      // SCTs have a single linear mask
      mask = evaluateSubSSMask(nbits,bitoffset,ampatt->getDCMask(),ampatt->getHBMask());
    } // end SCT encoding block
    else if (ndim==2) {
      /* if the layer has 2 dimensions each one has a different number of bits,
	   the X mask is at the begin, the Y mask is after. The result for
	   the allowed sub-SS is obtained crossing these two information */
      const int &nbitsX = m_tspmap.getNBits(iplane,0);
      const int &nbitsY = m_tspmap.getNBits(iplane,1);

      // fill the X information
      int maskX = evaluateSubSSMask(nbitsX,bitoffset,ampatt->getDCMask(),ampatt->getHBMask());

      // fill the Y information
      int maskY = evaluateSubSSMask(nbitsY,bitoffset+nbitsX,ampatt->getDCMask(),ampatt->getHBMask());

      if (!maskY) { // this means the TSP don't use any bit in Y
        mask = maskX;
      }
      else if (!maskX) { // no bit in X
        mask = maskY;
      }
      else { 	// derive a matrix from the 2 vectors
        int nposX = 1<<nbitsX;
        int nposY = 1<<nbitsY;
        for (int ipX=0;ipX!=nposX;++ipX) {
          for (int ipY=0;ipY!=nposY;++ipY) {
            int bitval = (((maskX&(1<<ipX))>>ipX)&((maskY&(1<<ipY))>>ipY));
            mask |= bitval<<(ipX+nposX*ipY);
          }
        }
      }
    } // end Pixel encoding block
    else {
      FTKSetup::PrintMessageFmt(ftk::sevr,"TSP with number of dimension %d is not allowed",ndim);
    }

    /* check is a valid mask: a valid mask has at least 1 bit set,
	 there isn't any bit further the maximum set */
    unsigned int maskForInvalidBits=~0<<npos;
    if (npos==32) maskForInvalidBits = 0;
    if (npos>32) FTKSetup::PrintMessageFmt(ftk::sevr,"Error: npos>32 (i.e. more than 5 DC bits) is an invalid configuration. npos=%d\n",npos);
    if (!mask || mask&(maskForInvalidBits)) {
      FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the Sub-SS mask for pattern %d, mask[%d]=%d\n",run_ipatt,iplane,mask);
    }
    // add the mask for this layer
    subss_masks.push_back(mask);
  } // end plane loop


  return nsubs;
}

/** this function hs the reverse effect of fillPatternInfo: from an FTKPattern object,
  meant as AM pattern, fills the related information for DC and TSP patterns */
int TSPLevel::getPatternInfo(int ipatt, int _dcmask, int _hbmask/*, bool readTSP */)
{
  // retrieve the DC mask
  m_dcmask[ipatt] = _dcmask;
  // prepare the HB mask to describe eventual children
  vector<int> hbmask;
  vector<int> subpattID;

  // if there aren't sub-pattern the HB information is retrieved from the pattern itself
  hbmask.push_back(_hbmask);
  subpattID.push_back(-1); // it means not a real pattern

  // set the information on hbmask and ID for the TSP patterns
  m_hbmask[ipatt] = hbmask;
  m_subPatternID[ipatt] = subpattID;


  vector<unsigned int> &subss_masks = m_subssmasks[ipatt];

  for (int iplane=0;iplane!=m_nplanes;++iplane) { // plane loop
    // retrieve the information of the TSP in this plane
    const int &ndim = m_tspmap.getDim(iplane);
    const int &bitoffset = m_tspmap.getBitOffset(iplane);

    // total number of positions in a layer
    const int &npos = m_tspmap.getNPositions(iplane);

    // final mask of the allowed regions in the AM SS
    unsigned int mask(0);
    if (ndim==1) {
      // get the number of the bits
      const int &nbits = m_tspmap.getNBits(iplane);

      // SCTs have a single linear mask
      mask = evaluateSubSSMask(nbits,bitoffset,m_dcmask[ipatt],_hbmask);
    } // end SCT encoding block
    else if (ndim==2) {
      /* if the layer has 2 dimensions each one has a different number of bits,
	   the X mask is at the begin, the Y mask is after. The result for
	   the allowed sub-SS is obtained crossing these two information */
      const int &nbitsX = m_tspmap.getNBits(iplane,0);
      const int &nbitsY = m_tspmap.getNBits(iplane,1);

      // fill the X information
      int maskX = evaluateSubSSMask(nbitsX,bitoffset,m_dcmask[ipatt],_hbmask);

      // fill the Y information
      int maskY = evaluateSubSSMask(nbitsY,bitoffset+nbitsX,m_dcmask[ipatt],_hbmask);

      if (!maskY) { // this means the TSP don't use any bit in Y
        mask = maskX;
      }
      else if (!maskX) { // no bit in X
        mask = maskY;
      }
      else { 	// derive a matrix from the 2 vectors
        int nposX = 1<<nbitsX;
        int nposY = 1<<nbitsY;
        for (int ipX=0;ipX!=nposX;++ipX) {
          for (int ipY=0;ipY!=nposY;++ipY) {
            int bitval = (((maskX&(1<<ipX))>>ipX)&((maskY&(1<<ipY))>>ipY));
            mask |= bitval<<(ipX+nposX*ipY);
          }
        }
      }
    } // end Pixel encoding block
    else {
      FTKSetup::PrintMessageFmt(ftk::sevr,"TSP with number of dimension %d is not allowed",ndim);
    }

    /* check is a valid mask: a valid mask has at least 1 bit set,
	 there isn't any bit further the maximum set */
    unsigned int maskForInvalidBits=~0<<npos;
    if (npos==32) maskForInvalidBits = 0;
    if (npos>32) FTKSetup::PrintMessageFmt(ftk::sevr,"Error: npos>32 (i.e. more than 5 DC bits) is an invalid configuration. npos=%d\n",npos);
    if (!mask || mask&(maskForInvalidBits)) {
      FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the Sub-SS mask for pattern %d, mask[%d]=%d\n",run_ipatt,iplane,mask);
    }
    // add the mask for this layer
    subss_masks.push_back(mask);
  } // end plane loop


  return 0;
}

/** Return the reference bitmask for the high precision part for the layer "patt"
 * in the requestd "layer"
 */
unsigned int TSPLevel::getHBMask(int patt, int layer) const {
  // retrieve the number of bits associated to this logical layer
  const int &npos = m_tspmap.getNBits(layer);
  // retrieve the offset of the mask to reach the given layer
  const int &bitoffset = m_tspmap.getBitOffset(layer);

  // retrieve the bitmask associated to the selected pattern
  const int &curhbmask =  m_hbmask[patt][0];

  // shift the word to have the bits for the layer in LSB position and set to bits after npos
  return (curhbmask>>bitoffset)&(~(~0<<npos));
}

/** Return the DC mask for the high precision part for the layer "patt"
 * in the requestd "layer"
 */
unsigned int TSPLevel::getDCMask(int patt, int layer) const {
  // retrieve the number of bits associated to this logical layer
  const int &npos = m_tspmap.getNBits(layer);
  // retrieve the offset of the mask to reach the given layer
  const int &bitoffset = m_tspmap.getBitOffset(layer);

  // retrieve the bitmask associated to the selected pattern
  const int &curdcmask =  m_dcmask[patt];

  // shift the word to have the bits for the layer in LSB position and set to bits after npos
  return (curdcmask>>bitoffset)&(~(~0<<npos));
}


/** This method removes part of the information that is not used required by some emulation methods
 */
void TSPLevel::clearExtraInfo() {
  m_subssmasks.clear();
  m_subPatternID.clear();
}
