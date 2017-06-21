/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TdrStgcTriggerLogic.h"

#include "PadWithHits.h"
#include "SingleWedgePadTrigger.h"
#include "SectorTriggerCandidate.h"
#include "tdr_compat_enum.h"
#include "vector_utils.h"

#include <vector>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>
#include "TMath.h"
#include "TVector3.h"

using namespace std;
using namespace nsw;
typedef L1TdrStgcTriggerLogic L1stl; // just to get shorter lines
typedef SingleWedgePadTrigger Swpt;

const double transitionEtaLow_S [3] = {1.43, 1.73, 2.07};
const double transitionEtaHigh_S[3] = {1.54, 1.87, 2.37};
const double transitionEtaLow_L [3] = {1.41, 1.67, 2.05};
const double transitionEtaHigh_L[3] = {1.53, 1.82, 2.35};

const int triggerLogicDbgLevel = 0;
const double padTimingEfficiency = 0.95; // set to -1 to disable

//-------------------------------------
L1stl::L1TdrStgcTriggerLogic() :
    m_verbose(false),
    m_writePickle(false),
    m_picklePrefix("./")
{
	rand.SetSeed(0);
}
//-------------------------------------
L1stl::~L1TdrStgcTriggerLogic() {

}
//-------------------------------------
bool L1stl::hitPattern(const nsw::Pad &firstPad, const nsw::Pad &otherPad, string &pattern, bool verbose = false)
{
  return L1stl::hitPattern(firstPad.ieta, firstPad.iphi, otherPad.ieta, otherPad.iphi, pattern);
}
//-------------------------------------
bool L1stl::hitPattern(const int &iEta0, const int &iPhi0,
                       const int &iEta1, const int &iPhi1,
                       string &pattern)
{
  // A la ATL-MUON-INT-2014-003 =>>
  //if(iPhi1 >= iPhi0 + 2 || iPhi1 < iPhi0) return false;
  //if(iEta1 >= iEta0 + 2 || iEta1 < iEta0) return false;
  //if(iPhi0 == iPhi1) pattern = (iEta0==iEta1 ? "11" : "21");
  //else               pattern = (iEta0==iEta1 ? "12" : "22");
  // <<== A la ATL-MUON-INT-2014-003
  // New Logic - DeltaEtaDeltaPhi
  if(iEta1 >= iEta0 + 2 || iEta1 <= iEta0 - 2) return false;
  if(iPhi1 >= iPhi0 + 2 || iPhi1 <= iPhi0 - 2) return false;
  // DeltaEta
  if     (iEta1 == iEta0 - 1) pattern = "0"; 
  else if(iEta1 == iEta0    ) pattern = "1";
  else if(iEta1 == iEta0 + 1) pattern = "2";
  // DeltaPhi
  if     (iPhi1 == iPhi0 - 1) pattern.append("0"); 
  else if(iPhi1 == iPhi0    ) pattern.append("1");
  else if(iPhi1 == iPhi0 + 1) pattern.append("2");

  return true;
}
//-------------------------------------
nsw::vswptrig_t L1stl::buildSingleWedgeTriggers(const vpads_t &pads,
                                                const vsize_t &padIndicesLayer0,
                                                const vsize_t &padIndicesLayer1,
                                                const vsize_t &padIndicesLayer2,
                                                const vsize_t &padIndicesLayer3,
                                                bool isLayer1, bool isLayer2,
                                                bool isLayer3, bool isLayer4,
                                                bool verbose)
{
    vswptrig_t triggers;

    const vsize_t &hitIdxs0 = padIndicesLayer0;
    const vsize_t &hitIdxs1 = padIndicesLayer1;
    const vsize_t &hitIdxs2 = padIndicesLayer2;
    const vsize_t &hitIdxs3 = padIndicesLayer3;
    bool isL1(isLayer1), isL2(isLayer2), isL3(isLayer3), isL4(isLayer4);
    if(verbose)
      cout<<" layer mask  ["<<isL1<<" "<< isL2 <<" "<<isL3<<" "<<isL4<<"]"
          <<" nHits/layer "<<hitIdxs0.size()<<" "<<hitIdxs1.size()<<" "<<hitIdxs2.size()<<" "<<hitIdxs3.size()<<endl;
    size_t nHL1 = (isL1 ? hitIdxs0.size() : 1);
    size_t nHL2 = (isL2 ? hitIdxs1.size() : 1);
    size_t nHL3 = (isL3 ? hitIdxs2.size() : 1);
    size_t nHL4 = (isL4 ? hitIdxs3.size() : 1);
    if(isL1 && nHL1 == 0) return triggers;
    if(isL2 && nHL2 == 0) return triggers;
    if(isL3 && nHL3 == 0) return triggers;
    if(isL4 && nHL4 == 0) return triggers;
    const vector<string> &allPatterns = sTGC_triggerPatterns();
    int iL1st = -1; // first layer index
    for(size_t il1=0; il1<nHL1; il1++) {
      int l1Idx = -1; string sl1("33");
      if(isL1){ l1Idx=hitIdxs0[il1]; sl1="11"; iL1st=l1Idx; } //if l1 is considered  its indices are always 11
      for(size_t il2=0; il2<nHL2; il2++) {
        int l2Idx = -1; string sl2("33");
        if(isL2) {
          l2Idx = hitIdxs1[il2];
          if(iL1st == -1) { sl2 = "11"; iL1st = l2Idx; }  //l1 was not considered, l2 gets the first indices
          else if(!hitPattern(pads[iL1st], pads[l2Idx], sl2, verbose)) continue;
        } // end if(isL2)
        for(size_t il3=0; il3<nHL3; il3++) {
          int l3Idx = -1; string sl3("33");
          if(isL3) {
            l3Idx = hitIdxs2[il3];
            if(!hitPattern(pads[iL1st], pads[l3Idx], sl3, verbose)) continue;
          }
          for(size_t il4=0; il4<nHL4; il4++) {
            int l4Idx = -1; string sl4("33");
            if(isL4) {
              l4Idx = hitIdxs3[il4];
              if(!hitPattern(pads[iL1st], pads[l4Idx], sl4, verbose)) continue;
            }
            // checked all layers, now store the trigger if it's a valid pattern
            string pattern(sl4+sl3+sl2+sl1);
            if(verbose) cout<<" pattern "<<pattern;
            //look for the pattern in the LUT
            if(std::find(allPatterns.begin(), allPatterns.end(), pattern)==allPatterns.end()){
              if(verbose) cout <<" ---> NOT triggered" << endl ;
              continue;
            }
            vsize_t padIndices;
            // assert : gcc is fine with a negative size_t values; however, here we should detect
            // buggy patterns (i.e. the index cannot be negative if isL_ is true).
            if(isL1) { assert(l1Idx>-1); padIndices.push_back(l1Idx); }
            if(isL2) { assert(l2Idx>-1); padIndices.push_back(l2Idx); }
            if(isL3) { assert(l3Idx>-1); padIndices.push_back(l3Idx); }
            if(isL4) { assert(l4Idx>-1); padIndices.push_back(l4Idx); }
            triggers.push_back(SingleWedgePadTrigger(pattern, pads, padIndices));
            //////////////////////////////////////////////////
            // ASM-2017-06-21
            // Don't build more than 5 SingleWedgePadTrigger
            if (triggers.size()>4) {
                cout << "STOP building SingleWedgeTriggers after reaching a total of 5 candidates!" << endl;
                return triggers;
            }
            //////////////////////////////////////////////////
          } // end for(il4)
        } // end for(il3)
      } // end for(il2)
    } // end for(il1)
    return triggers;
}
//-------------------------------------
nsw::vsize_t L1stl::removeRandomPadIndices(const vsize_t &padIndices)
{
  vsize_t out;
  out.reserve(padIndices.size());
  for(size_t i=0; i<padIndices.size(); ++i){
    if(rand.Uniform(1) < padTimingEfficiency) out.push_back(padIndices[i]);
  }
  return out;
}
//-------------------------------------
nsw::vswptrig_t L1stl::build3of4SingleWedgeTriggers(const vpads_t &pads,
                                                    const vsize_t &indicesL0,
                                                    const vsize_t &indicesL1,
                                                    const vsize_t &indicesL2,
                                                    const vsize_t &indicesL3,
                                                    bool verbose)
{
  const vsize_t &iL0=indicesL0, &iL1=indicesL1,&iL2=indicesL2, &iL3=indicesL3;
  vswptrig_t trigNoL0(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, false, true, true, true, verbose));
  vswptrig_t trigNoL1(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true, false, true, true, verbose));
  vswptrig_t trigNoL2(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true, true, false, true, verbose));
  vswptrig_t trigNoL3(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true, true, true, false, verbose));
  vswptrig_t triggers;
  triggers.insert(triggers.end(), trigNoL0.begin(), trigNoL0.end());
  triggers.insert(triggers.end(), trigNoL1.begin(), trigNoL1.end());
  triggers.insert(triggers.end(), trigNoL2.begin(), trigNoL2.end());
  triggers.insert(triggers.end(), trigNoL3.begin(), trigNoL3.end());
  return triggers;
}
nsw::vswptrig_t L1stl::build4of4SingleWedgeTriggers(const vpads_t &pads,
                                                    const vsize_t &indicesL0,
                                                    const vsize_t &indicesL1,
                                                    const vsize_t &indicesL2,
                                                    const vsize_t &indicesL3,
                                                    bool verbose)
{
  const vsize_t &iL0=indicesL0, &iL1=indicesL1,&iL2=indicesL2, &iL3=indicesL3;
  return buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true, true, true, true, verbose);
}

//-------------------------------------
/**
   @brief whether all pad indices of one trigger are a subset of another one
   A functor used to remove redundancies 3/4 from 4/4
 */
struct TrigIsSubsetOf : public std::unary_function<const nsw::SingleWedgePadTrigger&, bool>
{
  vsize_t indices; // indices of the pads from the four layers
  bool verbose;
  TrigIsSubsetOf(const nsw::SingleWedgePadTrigger &tr, bool verbose=false) :
    indices(tr.padIndices()), verbose(verbose) {
    assert(indices.size()==4);  // we want to use it with the 4 out of 4 triggers
    std::sort(indices.begin(), indices.end());
    if(verbose) cout<<" checking for redundancies of ["<<vec2str(indices)<<"]"<<endl;
  }
  bool operator() (const nsw::SingleWedgePadTrigger &tr) const {
    vsize_t idxes(tr.padIndices());
    std::sort(idxes.begin(), idxes.end()); // need to be sorted for set_intersection to work
    vsize_t commonIndices(indices.size() > idxes.size() ? indices.size() : idxes.size());
    bool allIdsPresent(std::distance(commonIndices.begin(),
                                     std::set_intersection(indices.begin(), indices.end(),
                                                           idxes.begin(),   idxes.end(),
                                                           commonIndices.begin()))
                       == static_cast<int>(idxes.size()));
    if(verbose) 
      cout<<"\t against  ["<<vec2str(idxes)<<"] --> "<<(allIdsPresent ? "drop":"keep")<<endl;
    return allIdsPresent;
  }
};
void remove3of4Redundant4of4(const nsw::vswptrig_t &trigs4of4, nsw::vswptrig_t &trigs3of4, bool verbose=false){
  for(vswptrig_t::const_iterator t4=trigs4of4.begin(); t4!=trigs4of4.end(); ++t4){
    trigs3of4.erase(std::remove_if(trigs3of4.begin(), trigs3of4.end(), TrigIsSubsetOf(*t4, verbose)),
                    trigs3of4.end());
  }
}
//-------------------------------------
bool L1stl::buildSectorTriggers(const vpads_t &pads, const vsize_t  &indicesSecN)
{
  // define two function references to shorten lines
  vsize_t (&filterByMultiplet)(const vpads_t&, const vsize_t&, int) = nsw::filterPadIndicesByMultiplet;
  vsize_t (&filterByLayer)(const vpads_t&, const vsize_t&, int) = nsw::filterPadIndicesByLayer;
  // inner/outer refers to |z|
  // it used to be (pivot/confirm) x (large/small) ... now obsolete nomenclature
  const int innerMultiplet(1), outerMultiplet(2);
  vsize_t indicesInner = filterByMultiplet(pads, indicesSecN, innerMultiplet);
  vsize_t indicesOuter = filterByMultiplet(pads, indicesSecN, outerMultiplet);
  vsize_t idxesI1(filterByLayer(pads, indicesInner, STGC_LAYER_1));
  vsize_t idxesI2(filterByLayer(pads, indicesInner, STGC_LAYER_2));
  vsize_t idxesI3(filterByLayer(pads, indicesInner, STGC_LAYER_3));
  vsize_t idxesI4(filterByLayer(pads, indicesInner, STGC_LAYER_4));
  vsize_t idxesO1(filterByLayer(pads, indicesOuter, STGC_LAYER_1));
  vsize_t idxesO2(filterByLayer(pads, indicesOuter, STGC_LAYER_2));
  vsize_t idxesO3(filterByLayer(pads, indicesOuter, STGC_LAYER_3));
  vsize_t idxesO4(filterByLayer(pads, indicesOuter, STGC_LAYER_4));
  vswptrig_t i4of4trig(build4of4SingleWedgeTriggers(pads, idxesI1, idxesI2, idxesI3, idxesI4, m_verbose));
  vswptrig_t i3of4trig(build3of4SingleWedgeTriggers(pads, idxesI1, idxesI2, idxesI3, idxesI4, m_verbose));
  vswptrig_t o4of4trig(build4of4SingleWedgeTriggers(pads, idxesO1, idxesO2, idxesO3, idxesO4, m_verbose));
  vswptrig_t o3of4trig(build3of4SingleWedgeTriggers(pads, idxesO1, idxesO2, idxesO3, idxesO4, m_verbose));

  remove3of4Redundant4of4(i4of4trig, i3of4trig, m_verbose);
  remove3of4Redundant4of4(o4of4trig, o3of4trig, m_verbose);
  if(m_verbose)
    cout<<"SingleWedge triggers :"
        <<" inner : "<<i3of4trig.size()<<"(3/4) "<<i4of4trig.size()<<"(4/4)"
        <<" outer : "<<o3of4trig.size()<<"(3/4) "<<o4of4trig.size()<<"(4/4)"
        <<endl;

  vswptrig_t innerTrigs, outerTrigs; // merge 4/4 and 3/4
  innerTrigs.insert(innerTrigs.end(), i3of4trig.begin(), i3of4trig.end());
  innerTrigs.insert(innerTrigs.end(), i4of4trig.begin(), i4of4trig.end());
  outerTrigs.insert(outerTrigs.end(), o3of4trig.begin(), o3of4trig.end());
  outerTrigs.insert(outerTrigs.end(), o4of4trig.begin(), o4of4trig.end());

  bool acceptSingleWedgeInTransition=true;
  bool skipInnerOuterMatchHack = false;
  vstcand_t trigCandidates;
  /**
     @todo fix hack DG-2015-10-08

     I am having some issue with determining the corners of the pad.
     The matching between inner/outer candidates becomes thus
     pointless.  As a temporary hack, just create one
     SectorTriggerCandidate for each SingleWedgePadTrigger.

     At this stage, we probably don't even need this feature, since
     we're using the pads only to determine the strip band. In the TDR
     we used it to check the resolution (from the pads measurements)
     of the pointing direction.
   */
  if(skipInnerOuterMatchHack){
      for(SingleWedgePadTrigger &swpt : innerTrigs)
          trigCandidates.push_back(SectorTriggerCandidate(swpt.setCombined()));
      for(SingleWedgePadTrigger &swpt : outerTrigs)
          trigCandidates.push_back(SectorTriggerCandidate(swpt.setCombined()));      
  } else {
  for(vswptrig_t::iterator it=innerTrigs.begin(); it!=innerTrigs.end(); ++it){
    if(acceptSingleWedgeInTransition && it->isInTransitionRegion()) continue;
    for(vswptrig_t::iterator ot=outerTrigs.begin(); ot!=outerTrigs.end(); ++ot){
      // Inner-Outer matching based on area
      EtaPhiRectangle innerArea = Swpt::padOverlap(it->pads());
      EtaPhiRectangle outerArea = Swpt::padOverlap(ot->pads());
      double overlap = EtaPhiRectangle::overlappingArea(innerArea,outerArea);
      if(m_verbose) {
        cout << "DEBUG :: Inner/Outer Matching :: " << endl;
        cout << "\t Inner candidate  : "; innerArea.print(cout); cout << endl;
        cout << "\t Outer candidate  : "; outerArea.print(cout); cout << endl;
        cout << "\t Overlapping area : " << overlap << endl;
        cout << "\t Is matched       : " << (overlap > 0 ? "YES" : "NO") << endl;
      }
      if(overlap>0) {
        trigCandidates.push_back(SectorTriggerCandidate(it->setCombined(), ot->setCombined()));
      } 
      //else {
      //  // ASM-2016-10-3 :: If area based matching fails - might even replace that!
      //  Swpt::EtaPhiHalf innerPos(it->halfPadCoordinates()); // should access cached datamemeber
      //  Swpt::EtaPhiHalf outerPos(ot->halfPadCoordinates());
      //  if(Swpt::areInnerOuterConsistent(innerPos, outerPos, m_verbose)) {
      //      trigCandidates.push_back(SectorTriggerCandidate(it->setCombined(), ot->setCombined()));
      //  }
      //}
    } // end for(ot)
  } // end for(it)
  if(acceptSingleWedgeInTransition){
    for(vswptrig_t::iterator it=innerTrigs.begin(); it!=innerTrigs.end(); ++it){
      if(it->alreadyCombined()) continue;
      else if(it->is4outOf4Layers() && it->isInTransitionRegion())
        trigCandidates.push_back(SectorTriggerCandidate(it->setCombined()));
    }
    for(vswptrig_t::iterator ot=outerTrigs.begin(); ot!=outerTrigs.end(); ++ot){
      if(ot->alreadyCombined()) continue;
      else if(ot->is4outOf4Layers() && ot->isInTransitionRegion())
        trigCandidates.push_back(SectorTriggerCandidate(ot->setCombined()));
    }
  } // end if(acceptSingleWedgeInTransition)
  } // if(not skipInnerOuterMatchHack)
  m_secTrigCand = trigCandidates;
  if(m_verbose){
    cout<<"found "<<trigCandidates.size()<<" triggerCandidates from "<<pads.size()<<" pads"<<endl;
    for(vstcand_t::const_iterator tc=trigCandidates.begin(); tc!=trigCandidates.end(); ++tc) {
      cout<<"trigger region : "<<tc->triggerRegion()
          <<" deltaR inner/outer : "<<tc->deltaR()
          <<endl;
    } // end for(tc)
  }

  if(m_writePickle) {
    if(trigCandidates.size()>0) {
      int sector = trigCandidates[0].wedgeTrigs()[0].pads()[0].sector;
      char buf[1024]="";
      sprintf(buf, "%s/sector%02d.txt", m_picklePrefix.c_str(), sector);
      std::ofstream fileDump;
      fileDump.open(buf);
      fileDump<<"[";
      for(size_t i=0;i<trigCandidates.size();++i)
        fileDump<<"{"<<trigCandidates[i].pickle()<<"},\n";
      fileDump<<"]";
      fileDump.close();
    }
  } // if(m_writePickle)

  return (trigCandidates.size()>0);
}

/**
   The trigger 3 out of 4 masks
   using the following translation from Daniel ....
   feaaa8  R: .+-+ -+-+  Phi: .--+ -+-+  Best R/Phi: 2/1 (out of 4)
   2013/02/10 now calculating per wedge
*/
std::vector<std::string> L1TdrStgcTriggerLogic::sTGC_triggerPatterns()
{
    vector<string> patterns;
////    patterns.push_back("11111111");
////    patterns.push_back("12111111");
////    patterns.push_back("12111211");
////    patterns.push_back("12121211");
////    patterns.push_back("21111111");
////    patterns.push_back("21112111");
////    patterns.push_back("21212111");
////    patterns.push_back("22111111");
////    patterns.push_back("22111211");
////    patterns.push_back("22112111");
////    patterns.push_back("22112211");
////    patterns.push_back("22121211");
////    patterns.push_back("22122211");
////    patterns.push_back("22212111");
////    patterns.push_back("22212211");
////    patterns.push_back("22222211");
////    patterns.push_back("00111111");
////    patterns.push_back("00111211");
////    patterns.push_back("00112111");
////    patterns.push_back("00112211");
////    patterns.push_back("00121211");
////    patterns.push_back("00122211");
////    patterns.push_back("00212111");
////    patterns.push_back("00212211");
////    patterns.push_back("00222211");
////    patterns.push_back("11001111");
////    patterns.push_back("11110011");
////    patterns.push_back("11111100");
////    patterns.push_back("12001111");
////    patterns.push_back("12001211");
////    patterns.push_back("12110011");
////    patterns.push_back("12111100");
////    patterns.push_back("12111200");
////    patterns.push_back("12120011");
////    patterns.push_back("12121200");
////    patterns.push_back("21001111");
////    patterns.push_back("21002111");
////    patterns.push_back("21110011");
////    patterns.push_back("21111100");
////    patterns.push_back("21112100");
////    patterns.push_back("21210011");
////    patterns.push_back("21212100");
////    patterns.push_back("22001111");
////    patterns.push_back("22001211");
////    patterns.push_back("22002111");
////    patterns.push_back("22002211");
////    patterns.push_back("22110011");
////    patterns.push_back("22111100");
////    patterns.push_back("22111200");
////    patterns.push_back("22112100");
////    patterns.push_back("22112200");
////    patterns.push_back("22120011");
////    patterns.push_back("22121200");
////    patterns.push_back("22122200");
////    patterns.push_back("22210011");
////    patterns.push_back("22212100");
////    patterns.push_back("22212200");
////    patterns.push_back("22220011");
////    patterns.push_back("22222200");
    // ==>> Dummy w/ all combinations w/o multiple 33
    string combinations[10] = {"00","01","02","10","11","12","20","21","22","33"};
    string current = "";
    for(unsigned int ii=0; ii<10; ii++) {
        for(unsigned int ij=0; ij<10; ij++) {
            for(unsigned int ik=0; ik<10; ik++) {
                for(unsigned int il=0; il<10; il++) {
                    current = combinations[ii]+combinations[ij]+combinations[ik]+combinations[il];
                    if ( combinations[ii] == "33" and combinations[ii] == combinations[ij] ) continue;
                    if ( combinations[ii] == "33" and combinations[ii] == combinations[ik] ) continue;
                    if ( combinations[ii] == "33" and combinations[ii] == combinations[il] ) continue;
                    if ( combinations[ij] == "33" and combinations[ij] == combinations[ik] ) continue;
                    if ( combinations[ij] == "33" and combinations[ij] == combinations[il] ) continue;
                    if ( combinations[ik] == "33" and combinations[ik] == combinations[il] ) continue;
                    patterns.push_back(current); current = "";
                }
            }
        }
    }
    //for(auto entry:patterns) cout << "SERHAN :: " << entry << endl;
    // Dummy done

    //// Patterns for C side from Rotem - Added by ASM 2016-12-14 for testing
    //patterns.push_back("00011011");
    //patterns.push_back("00011111");
    //patterns.push_back("00011133");
    //patterns.push_back("00013311");
    //patterns.push_back("00111011");
    //patterns.push_back("00331011");
    //patterns.push_back("00331111");
    //patterns.push_back("01010111");
    //patterns.push_back("01011011");
    //patterns.push_back("01011111");
    //patterns.push_back("01011133");
    //patterns.push_back("01013311");
    //patterns.push_back("01020111");
    //patterns.push_back("01021111");
    //patterns.push_back("01021133");
    //patterns.push_back("01023311");
    //patterns.push_back("01111111");
    //patterns.push_back("01111133");
    //patterns.push_back("01113311");
    //patterns.push_back("01121111");
    //patterns.push_back("01121133");
    //patterns.push_back("01123311");
    //patterns.push_back("01330111");
    //patterns.push_back("01331011");
    //patterns.push_back("01331111");
    //patterns.push_back("01331133");
    //patterns.push_back("01333311");
    //patterns.push_back("02010111");
    //patterns.push_back("02011111");
    //patterns.push_back("02011133");
    //patterns.push_back("02013311");
    //patterns.push_back("02020111");
    //patterns.push_back("02021111");
    //patterns.push_back("02021133");
    //patterns.push_back("02021211");
    //patterns.push_back("02023311");
    //patterns.push_back("02121111");
    //patterns.push_back("02121133");
    //patterns.push_back("02123311");
    //patterns.push_back("02330111");
    //patterns.push_back("02331111");
    //patterns.push_back("02331133");
    //patterns.push_back("02331211");
    //patterns.push_back("02333311");
    //patterns.push_back("10011111");
    //patterns.push_back("10011133");
    //patterns.push_back("10100111");
    //patterns.push_back("10101011");
    //patterns.push_back("10101111");
    //patterns.push_back("10101133");
    //patterns.push_back("10103311");
    //patterns.push_back("10110111");
    //patterns.push_back("10111011");
    //patterns.push_back("10111111");
    //patterns.push_back("10111133");
    //patterns.push_back("10112111");
    //patterns.push_back("10113311");
    //patterns.push_back("10211011");
    //patterns.push_back("10330111");
    //patterns.push_back("10331011");
    //patterns.push_back("10331111");
    //patterns.push_back("10332111");
    //patterns.push_back("11011011");
    //patterns.push_back("11011111");
    //patterns.push_back("11011133");
    //patterns.push_back("11013311");
    //patterns.push_back("11021111");
    //patterns.push_back("11021133");
    //patterns.push_back("11023311");
    //patterns.push_back("11100111");
    //patterns.push_back("11101111");
    //patterns.push_back("11101133");
    //patterns.push_back("11103311");
    //patterns.push_back("11110111");
    //patterns.push_back("11111011");
    //patterns.push_back("11111111");
    //patterns.push_back("11111133");
    //patterns.push_back("11111211");
    //patterns.push_back("11112011");
    //patterns.push_back("11112111");
    //patterns.push_back("11113311");
    //patterns.push_back("11120111");
    //patterns.push_back("11121111");
    //patterns.push_back("11121133");
    //patterns.push_back("11122111");
    //patterns.push_back("11123311");
    //patterns.push_back("11201111");
    //patterns.push_back("11201133");
    //patterns.push_back("11203311");
    //patterns.push_back("11211111");
    //patterns.push_back("11211133");
    //patterns.push_back("11213311");
    //patterns.push_back("11221111");
    //patterns.push_back("11221133");
    //patterns.push_back("11223311");
    //patterns.push_back("11330111");
    //patterns.push_back("11331011");
    //patterns.push_back("11331111");
    //patterns.push_back("11331133");
    //patterns.push_back("11331211");
    //patterns.push_back("11332011");
    //patterns.push_back("11332111");
    //patterns.push_back("11333311");
    //patterns.push_back("12011111");
    //patterns.push_back("12011133");
    //patterns.push_back("12013311");
    //patterns.push_back("12021111");
    //patterns.push_back("12021133");
    //patterns.push_back("12021211");
    //patterns.push_back("12023311");
    //patterns.push_back("12110111");
    //patterns.push_back("12111111");
    //patterns.push_back("12111133");
    //patterns.push_back("12111211");
    //patterns.push_back("12112111");
    //patterns.push_back("12113311");
    //patterns.push_back("12120111");
    //patterns.push_back("12121111");
    //patterns.push_back("12121133");
    //patterns.push_back("12121211");
    //patterns.push_back("12122111");
    //patterns.push_back("12123311");
    //patterns.push_back("12211211");
    //patterns.push_back("12221111");
    //patterns.push_back("12221133");
    //patterns.push_back("12221211");
    //patterns.push_back("12223311");
    //patterns.push_back("12330111");
    //patterns.push_back("12331111");
    //patterns.push_back("12331133");
    //patterns.push_back("12331211");
    //patterns.push_back("12332111");
    //patterns.push_back("12333311");
    //patterns.push_back("20101011");
    //patterns.push_back("20101111");
    //patterns.push_back("20101133");
    //patterns.push_back("20103311");
    //patterns.push_back("20111111");
    //patterns.push_back("20111133");
    //patterns.push_back("20113311");
    //patterns.push_back("20201011");
    //patterns.push_back("20201111");
    //patterns.push_back("20201133");
    //patterns.push_back("20202011");
    //patterns.push_back("20202111");
    //patterns.push_back("20203311");
    //patterns.push_back("20211011");
    //patterns.push_back("20211111");
    //patterns.push_back("20211133");
    //patterns.push_back("20212111");
    //patterns.push_back("20213311");
    //patterns.push_back("20331011");
    //patterns.push_back("20331111");
    //patterns.push_back("20332011");
    //patterns.push_back("20332111");
    //patterns.push_back("21111011");
    //patterns.push_back("21111111");
    //patterns.push_back("21111133");
    //patterns.push_back("21111211");
    //patterns.push_back("21113311");
    //patterns.push_back("21121111");
    //patterns.push_back("21121133");
    //patterns.push_back("21123311");
    //patterns.push_back("21201111");
    //patterns.push_back("21201133");
    //patterns.push_back("21203311");
    //patterns.push_back("21211011");
    //patterns.push_back("21211111");
    //patterns.push_back("21211133");
    //patterns.push_back("21211211");
    //patterns.push_back("21212011");
    //patterns.push_back("21212111");
    //patterns.push_back("21213311");
    //patterns.push_back("21221111");
    //patterns.push_back("21221133");
    //patterns.push_back("21222111");
    //patterns.push_back("21223311");
    //patterns.push_back("21331011");
    //patterns.push_back("21331111");
    //patterns.push_back("21331211");
    //patterns.push_back("21332011");
    //patterns.push_back("21332111");
    //patterns.push_back("22111111");
    //patterns.push_back("22111133");
    //patterns.push_back("22113311");
    //patterns.push_back("22121111");
    //patterns.push_back("22121133");
    //patterns.push_back("22121211");
    //patterns.push_back("22123311");
    //patterns.push_back("22211111");
    //patterns.push_back("22211133");
    //patterns.push_back("22211211");
    //patterns.push_back("22212111");
    //patterns.push_back("22213311");
    //patterns.push_back("22221111");
    //patterns.push_back("22221133");
    //patterns.push_back("22221211");
    //patterns.push_back("22222111");
    //patterns.push_back("22223311");
    //patterns.push_back("22331111");
    //patterns.push_back("22331211");
    //patterns.push_back("22332111");
    //patterns.push_back("33010111");
    //patterns.push_back("33011011");
    //patterns.push_back("33011111");
    //patterns.push_back("33011133");
    //patterns.push_back("33013311");
    //patterns.push_back("33020111");
    //patterns.push_back("33021111");
    //patterns.push_back("33021133");
    //patterns.push_back("33021211");
    //patterns.push_back("33023311");
    //patterns.push_back("33100111");
    //patterns.push_back("33101011");
    //patterns.push_back("33101111");
    //patterns.push_back("33110111");
    //patterns.push_back("33111011");
    //patterns.push_back("33111111");
    //patterns.push_back("33111133");
    //patterns.push_back("33111211");
    //patterns.push_back("33112011");
    //patterns.push_back("33112111");
    //patterns.push_back("33113311");
    //patterns.push_back("33120111");
    //patterns.push_back("33121111");
    //patterns.push_back("33121133");
    //patterns.push_back("33121211");
    //patterns.push_back("33122111");
    //patterns.push_back("33123311");
    //patterns.push_back("33201011");
    //patterns.push_back("33201111");
    //patterns.push_back("33202011");
    //patterns.push_back("33202111");
    //patterns.push_back("33211011");
    //patterns.push_back("33211111");
    //patterns.push_back("33211211");
    //patterns.push_back("33212011");
    //patterns.push_back("33212111");
    //patterns.push_back("33221111");
    //patterns.push_back("33221211");
    //patterns.push_back("33222111");
    //patterns.push_back("33330111");
    //patterns.push_back("33331011");
    //patterns.push_back("33331111");
    //patterns.push_back("33331133");
    //patterns.push_back("33331211");
    //patterns.push_back("33332011");
    //patterns.push_back("33332111");
    //patterns.push_back("33333311");

    return patterns;
}
