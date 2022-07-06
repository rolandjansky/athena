/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"

#include "TrigT1NSWSimTools/L1TdrStgcTriggerLogic.h"
#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"

#include <fstream>
#include <functional>
#include <numeric>

namespace {
    const double padTimingEfficiency = 0.95; // set to -1 to disable
}

namespace NSWL1{
    L1TdrStgcTriggerLogic::L1TdrStgcTriggerLogic():
      AthMessaging(Athena::getMessageSvc(), "L1TdrStgcTriggerLogic") {}
    //-------------------------------------

    bool L1TdrStgcTriggerLogic::hitPattern(const std::shared_ptr<PadOfflineData> &firstPad, const std::shared_ptr<PadOfflineData> &otherPad,
                        std::string &pattern) const {
        return L1TdrStgcTriggerLogic::hitPattern(firstPad->padEtaId(), firstPad->padPhiId(), otherPad->padEtaId(), otherPad->padPhiId(), pattern);
    }
    //-------------------------------------
    bool L1TdrStgcTriggerLogic::hitPattern(const int iEta0, const int iPhi0, const int iEta1, const int iPhi1, std::string &pattern) const {
        pattern = "33";
        if (iEta1 >= iEta0 + 2 || iEta1 <= iEta0 - 2)
            return false;
        if (iPhi1 >= iPhi0 + 2 || iPhi1 <= iPhi0 - 2)
            return false;
        // DeltaEta
        if (iEta1 == iEta0 - 1)
            pattern = "0";
        else if (iEta1 == iEta0)
            pattern = "1";
        else if (iEta1 == iEta0 + 1)
            pattern = "2";
        // DeltaPhi
        if (iPhi1 == iPhi0 - 1)
            pattern.append("0");
        else if (iPhi1 == iPhi0)
            pattern.append("1");
        else if (iPhi1 == iPhi0 + 1)
            pattern.append("2");

        return true;
    }
    //-------------------------------------
    //S.I : a method should not have so many arguments..
    std::vector< SingleWedgePadTrigger > L1TdrStgcTriggerLogic::buildSingleWedgeTriggers(
    const std::vector< std::shared_ptr<PadOfflineData> > &pads, const std::vector< size_t > &padIndicesLayer0,
    const std::vector< size_t > &padIndicesLayer1, const std::vector< size_t > &padIndicesLayer2,
    const std::vector< size_t > &padIndicesLayer3, bool isLayer1, bool isLayer2,
    bool isLayer3, bool isLayer4) const {
    
    std::vector< SingleWedgePadTrigger > triggers;
    size_t nHL1 = (isLayer1 ? padIndicesLayer0.size() : 1);
    size_t nHL2 = (isLayer2 ? padIndicesLayer1.size() : 1);
    size_t nHL3 = (isLayer3 ? padIndicesLayer2.size() : 1);
    size_t nHL4 = (isLayer4 ? padIndicesLayer3.size() : 1);

    if (isLayer1 && nHL1 == 0)
        return triggers;
    if (isLayer2 && nHL2 == 0)
        return triggers;
    if (isLayer3 && nHL3 == 0)
        return triggers;
    if (isLayer4 && nHL4 == 0)
        return triggers;

    const std::array<std::string_view, 10> PatternsEtaUp = sTGC_triggerPatternsEtaUp();
    const std::array<std::string_view, 10> PatternsEtaDown = sTGC_triggerPatternsEtaDown();
    const std::array<std::string_view, 16> PatternsPhiUp = sTGC_triggerPatternsPhiUp();
    const std::array<std::string_view, 16> PatternsPhiDown = sTGC_triggerPatternsPhiDown();
    const std::array<std::string_view, 10> PatternsPhiDownUp = sTGC_triggerPatternsPhiDownUp();
    const std::array<std::string_view, 10> PatternsPhiUpDown = sTGC_triggerPatternsPhiUpDown();

    int iL1st = -1;
    for (size_t il1 = 0; il1 < nHL1; il1++) {
        int l1Idx = -1;
        std::string sl1("33");
        if (isLayer1) {
            l1Idx = padIndicesLayer0.at(il1);
            sl1 = "11";
            iL1st = l1Idx;
        }
        for (size_t il2 = 0; il2 < nHL2; il2++) {
            int l2Idx = -1;
            std::string sl2("33");
            if (isLayer2) {
                l2Idx = padIndicesLayer1.at(il2);
                if (iL1st == -1) {
                    sl2 = "11";
                    iL1st = l2Idx;
                } // l1 was not considered, l2 gets the first indices
                else if (!hitPattern(pads.at(iL1st), pads.at(l2Idx), sl2))
                continue;
            }

            for (size_t il3 = 0; il3 < nHL3; il3++) {
                int l3Idx = -1;
                std::string sl3("33");
                if (isLayer3) {
                    l3Idx = padIndicesLayer2.at(il3);
                    if (!hitPattern(pads.at(iL1st), pads.at(l3Idx), sl3))
                        continue;
                }
                for (size_t il4 = 0; il4 < nHL4; il4++) {
                    int l4Idx = -1;
                    std::string sl4("33");
                    if (isLayer4) {
                        l4Idx = padIndicesLayer3.at(il4);
                        if (!hitPattern(pads.at(iL1st), pads.at(l4Idx), sl4))
                        continue;
                    }

                    std::string pattern(sl4 + sl3 + sl2 + sl1);
                    // the above line is replaced by a normal order l1,l2,l3,l4 and
                    // separated in phi and eta but it remains the same for later usage in
                    // the trigger selection
                    // Hence the following is only for internal ease of calculation. The
                    // pattern to be passed is the "pattern" not "patternPhi" or
                    // "patternEta"
                    std::string patternPhi;
                    patternPhi.push_back(sl1.at(1));
                    patternPhi.push_back(sl2.at(1));
                    patternPhi.push_back(sl3.at(1));
                    patternPhi.push_back(sl4.at(1));

                    std::string patternEta;
                    patternEta.push_back(sl1.at(0));
                    patternEta.push_back(sl2.at(0));
                    patternEta.push_back(sl3.at(0));
                    patternEta.push_back(sl4.at(0));

                    int multipletid=-999;
                    int moduleid=-999;
                    int sectortype=-999;
                    //Please mind the indentation
                    if (sl1 == "11") {
                        multipletid = pads.at(l1Idx)->multipletId() ;
                        moduleid = pads.at(l1Idx)->moduleId();
                        sectortype = pads.at(l1Idx)->sectorType();
                    }
                    else if (sl2 == "11") {
                        multipletid = pads.at(l2Idx)->multipletId() ;
                        moduleid = pads.at(l2Idx)->moduleId();
                        sectortype = pads.at(l2Idx)->sectorType();
                    }
                    else continue;
                    std::string etamove, phimove;
                    if (sectortype == 1) {
                        if (multipletid == 1) {
                            etamove = "D";
                            phimove = "U";
                            if (moduleid == 2) {
                                etamove = "U";
                                phimove = "U";
                            }
                        }
                        else{ // multiplet = 2   15-7-18 YR - Confirm   new geometry
                            etamove = "U";
                            phimove = "DU";
                        }
                    }
 
                    if (sectortype == 0) {
                        if (multipletid == 1) { // 15-7-18 YR - Confirm   new geometry
                            if (moduleid == 1) {
                                etamove = "U";
                                phimove = "DU";
                            }
                            if (moduleid == 2) {
                                etamove = "D";
                                phimove = "UD";
                            }
                            if (moduleid == 3) {
                                etamove = "U";
                                phimove = "UD";
                            }
                        }
                        else { // multiplet = 2
                            if (moduleid == 1) {
                                etamove = "U";
                                phimove = "D";
                            }
                            else{
                                etamove = "U";
                                phimove = "U";
                            }
                        }
                    }

                    if (etamove == "D") {
                        if (find(PatternsEtaDown.begin(), PatternsEtaDown.end(),
                            patternEta) == PatternsEtaDown.end()) {
                            continue;
                        }
                    }

                    if (etamove == "U") {
                        if (find(PatternsEtaUp.begin(), PatternsEtaUp.end(),
                            patternEta) == PatternsEtaUp.end()) {
                            continue;
                        }
                    }

                    if (phimove == "U") {
                        if (find(PatternsPhiUp.begin(), PatternsPhiUp.end(),
                            patternPhi) == PatternsPhiUp.end()) {
                            continue;
                        }
                    }

                    if (phimove == "D") {
                        if (find(PatternsPhiDown.begin(), PatternsPhiDown.end(),
                            patternPhi) == PatternsPhiDown.end()) {
                            continue;
                        }
                    }

                    if (phimove == "UD") {
                        if (find(PatternsPhiUpDown.begin(), PatternsPhiUpDown.end(),
                            patternPhi) == PatternsPhiUpDown.end()) {
                            continue;
                        }
                    }

                    if (phimove == "DU") {
                        if (find(PatternsPhiDownUp.begin(), PatternsPhiDownUp.end(),
                            patternPhi) == PatternsPhiDownUp.end()) {
                            continue;
                        }
                    }

                    std::vector< size_t > padIndices;

                    if (isLayer1) {
                        assert(l1Idx > -1);
                        padIndices.push_back(l1Idx);
                    }
                    if (isLayer2) {
                        assert(l2Idx > -1);
                        padIndices.push_back(l2Idx);
                    }

                    if (isLayer3) {
                        assert(l3Idx > -1);
                        padIndices.push_back(l3Idx);
                    }

                    if (isLayer4) {
                        assert(l4Idx > -1);
                        padIndices.push_back(l4Idx);
                    }

                    triggers.push_back(SingleWedgePadTrigger(pattern, pads, padIndices));

                    if (triggers.size() > 4) {
                        return triggers;
                    }
                }
            }
        }
    }

    return triggers;
    }

    //-------------------------------------
    std::vector< size_t > L1TdrStgcTriggerLogic::removeRandomPadIndices(const std::vector< size_t > &padIndices) const {
        std::vector< size_t > out;
        TRandom rand;
        out.reserve(padIndices.size());
        for (size_t i = 0; i < padIndices.size(); ++i) {
            if (rand.Uniform(1) < padTimingEfficiency)
            out.push_back(padIndices.at(i));
        }
        return out;
    }
    //-------------------------------------Inner:
    std::vector< SingleWedgePadTrigger > L1TdrStgcTriggerLogic::build34swt(const std::vector< std::shared_ptr<PadOfflineData> > &pads,
                                                                           const std::vector< size_t > &iL0,
                                                                           const std::vector< size_t > &iL1,
                                                                           const std::vector< size_t > &iL2,
                                                                           const std::vector< size_t > &iL3) const {

        std::vector< SingleWedgePadTrigger > trigNoL0(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, false,true, true, true));
        std::vector< SingleWedgePadTrigger > trigNoL1(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true,false, true, true));
        std::vector< SingleWedgePadTrigger > trigNoL2(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true,true, false, true));
        std::vector< SingleWedgePadTrigger > trigNoL3(buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true,true, true, false));
        std::vector< SingleWedgePadTrigger > triggers;
        triggers.insert(triggers.end(), trigNoL0.begin(), trigNoL0.end());
        triggers.insert(triggers.end(), trigNoL1.begin(), trigNoL1.end());
        triggers.insert(triggers.end(), trigNoL2.begin(), trigNoL2.end());
        triggers.insert(triggers.end(), trigNoL3.begin(), trigNoL3.end());
        return triggers;
    }
    std::vector< SingleWedgePadTrigger > L1TdrStgcTriggerLogic::build44swt(const std::vector< std::shared_ptr<PadOfflineData> > &pads,
                                                                      const std::vector< size_t > &iL0,
                                                                      const std::vector< size_t > &iL1,
                                                                      const std::vector< size_t > &iL2,
                                                                      const std::vector< size_t > &iL3) const {
        return buildSingleWedgeTriggers(pads, iL0, iL1, iL2, iL3, true, true, true,true);
    }

    //-------------------------------------
    /**
    @brief whether all pad indices of one trigger are a subset of another one
    A functor used to remove redundancies 3/4 from 4/4
    */
    struct TrigIsSubsetOf {
        std::vector< size_t > indices; // indices of the pads from the four layers
        TrigIsSubsetOf(const SingleWedgePadTrigger &tr) : indices(tr.padIndices()) {
            assert(indices.size() ==4);                        // we want to use it with the 4 out of 4 triggers
            sort(indices.begin(), indices.end());
        }
        bool operator()(const SingleWedgePadTrigger &tr) const {
            std::vector< size_t > idxes(tr.padIndices());
            sort(idxes.begin(),idxes.end()); // need to be sorted for set_intersection to work
            std::vector< size_t > commonIndices(indices.size() > idxes.size() ? indices.size(): idxes.size());
            bool allIdsPresent(
                 std::distance(commonIndices.begin(),std::set_intersection(indices.begin(), indices.end(),idxes.begin(), idxes.end(),commonIndices.begin())) ==static_cast<int>(idxes.size())
              );
        return allIdsPresent;
       }
    };
    
    void remove3of4Redundant4of4(const std::vector< SingleWedgePadTrigger > &trigs4of4,std::vector< SingleWedgePadTrigger > &trigs3of4){//haha :)
      for (std::vector< SingleWedgePadTrigger >::const_iterator t4 = trigs4of4.begin(); t4 != trigs4of4.end();++t4) {
         trigs3of4.erase(std::remove_if(trigs3of4.begin(), trigs3of4.end(),TrigIsSubsetOf(*t4)),trigs3of4.end());
      }
    }
    //-------------------------------------
    std::vector<SectorTriggerCandidate> L1TdrStgcTriggerLogic::buildSectorTriggers(const std::vector< std::shared_ptr<PadOfflineData> > &pads) const {

        std::vector<SectorTriggerCandidate> secTrigCand;

        std::vector< size_t > indicesSecN(pads.size());
        std::iota(indicesSecN.begin(),indicesSecN.end(),0);
        const int innerMultiplet(1), outerMultiplet(2);
        std::vector< size_t > indicesInner = filterByMultiplet(pads, indicesSecN, innerMultiplet);
        std::vector< size_t > indicesOuter = filterByMultiplet(pads, indicesSecN, outerMultiplet);
        std::vector< size_t > idxesI1(filterByLayer(pads, indicesInner, STGC_LAYER_1));
        std::vector< size_t > idxesI2(filterByLayer(pads, indicesInner, STGC_LAYER_2));
        std::vector< size_t > idxesI3(filterByLayer(pads, indicesInner, STGC_LAYER_3));
        std::vector< size_t > idxesI4(filterByLayer(pads, indicesInner, STGC_LAYER_4));
        std::vector< size_t > idxesO1(filterByLayer(pads, indicesOuter, STGC_LAYER_1));
        std::vector< size_t > idxesO2(filterByLayer(pads, indicesOuter, STGC_LAYER_2));
        std::vector< size_t > idxesO3(filterByLayer(pads, indicesOuter, STGC_LAYER_3));
        std::vector< size_t > idxesO4(filterByLayer(pads, indicesOuter, STGC_LAYER_4));
        std::vector< SingleWedgePadTrigger > i4of4trig(build44swt(pads, idxesI1, idxesI2, idxesI3, idxesI4));
        std::vector< SingleWedgePadTrigger > i3of4trig(build34swt(pads, idxesI1, idxesI2, idxesI3, idxesI4));
        std::vector< SingleWedgePadTrigger > o4of4trig(build44swt(pads, idxesO1, idxesO2, idxesO3, idxesO4));
        std::vector< SingleWedgePadTrigger > o3of4trig(build34swt(pads, idxesO1, idxesO2, idxesO3, idxesO4));

        remove3of4Redundant4of4(i4of4trig, i3of4trig);
        remove3of4Redundant4of4(o4of4trig, o3of4trig);

        ATH_MSG_DEBUG("SingleWedge triggers :"
                << " inner : " << i3of4trig.size() << "(3/4) " << i4of4trig.size()
                << "(4/4)"
                << " outer : " << o3of4trig.size() << "(3/4) " << o4of4trig.size()
                << "(4/4)" );
        
        std::vector< SingleWedgePadTrigger > innerTrigs, outerTrigs; // merge 4/4 and 3/4
        innerTrigs.insert(innerTrigs.end(), i3of4trig.begin(), i3of4trig.end());
        innerTrigs.insert(innerTrigs.end(), i4of4trig.begin(), i4of4trig.end());
        outerTrigs.insert(outerTrigs.end(), o3of4trig.begin(), o3of4trig.end());
        outerTrigs.insert(outerTrigs.end(), o4of4trig.begin(), o4of4trig.end());
        bool acceptSingleWedgeInTransition = true;
        //=============S.I this should absoultely be removed why do we keep this ? ======
        bool skipInnerOuterMatchHack = false;
        if (skipInnerOuterMatchHack) {
            for (auto &swpt : innerTrigs){
                secTrigCand.emplace_back(swpt.setCombined());
            }
            for (auto &swpt : outerTrigs){
                secTrigCand.emplace_back(swpt.setCombined());
            }
        } 
        //SI ================================================================================
        else {
            for (auto& it : innerTrigs) {
                for (auto& ot: outerTrigs) {
                    // Inner-Outer matching based on area
                    Polygon innerArea=SingleWedgePadTrigger::padOverlap3(it.pads());
                    Polygon outerArea=SingleWedgePadTrigger::padOverlap3(ot.pads());

                    float Z1=ot.pads().at(0)->m_cornerXyz[1][2];
                    float Z0=it.pads().at(0)->m_cornerXyz[1][2];

                    Polygon inoutovl=largestIntersection(innerArea,Project(outerArea,Z1,Z0));

                    float overlap=area(inoutovl);

                    if (overlap >0) {
                       ATH_MSG_DEBUG("OVERLAP  "<<overlap<<" Inner "<<area(innerArea)<<" Outer "<<area(outerArea));
                        secTrigCand.emplace_back(it.setCombined(), ot.setCombined());
                    }
                } // end for(ot)
            }   // end for(it)
            //***** Transition Region *****


            if (acceptSingleWedgeInTransition) {
                for ( auto& it : innerTrigs){
                    if (it.alreadyCombined()){
                         ATH_MSG_DEBUG("Inner SingleWedge trigger already combined, skipping");
                        continue;
                    }
                    else if ((it.is4outOf4Layers()||it.is3outOf4Layers()) && it.isInTransitionRegion()){
                        secTrigCand.emplace_back(it.setCombined());
                    }
                }
                for ( auto& ot : outerTrigs) {
                    if (ot.alreadyCombined()){
                         ATH_MSG_DEBUG("Outer SingleWedge trigger already combined, skipping");
                        continue;
                    }
                    else if ((ot.is4outOf4Layers()||ot.is3outOf4Layers()) && ot.isInTransitionRegion()){
                        secTrigCand.emplace_back(ot.setCombined());
                    }
                }
            } // end if(acceptSingleWedgeInTransition)
        }   // if(not skipInnerOuterMatchHack)

        if (msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG("found " << secTrigCand.size() << " triggerCandidates from "<< pads.size() << " pads");
          for (const auto& tc : secTrigCand) ATH_MSG_DEBUG("trigger region area : " << area(tc.triggerRegion3()));
        }
        return secTrigCand;
    }

    /**
    The trigger 3 out of 4 masks
    using the following translation from Daniel ....
    feaaa8  R: .+-+ -+-+  Phi: .--+ -+-+  Best R/Phi: 2/1 (out of 4)
    2013/02/10 now calculating per wedge
    */

    std::array<std::string_view, 10> L1TdrStgcTriggerLogic::sTGC_triggerPatternsEtaUp() const {
      constexpr static std::array<std::string_view, 10> patterns = {"1111", "1122", "3111", "3122", "1311", "1322", "1131", "1132", "1113", "1123"};
      return patterns;
    }

    std::array<std::string_view, 10> L1TdrStgcTriggerLogic::sTGC_triggerPatternsEtaDown() const {
      constexpr static std::array<std::string_view, 10> patterns = {"1111", "1100", "3111", "3100", "1311", "1300", "1131", "1130", "1113", "1103"};
      return patterns;
    }

    std::array<std::string_view, 16> L1TdrStgcTriggerLogic::sTGC_triggerPatternsPhiUp() const {
      constexpr static std::array<std::string_view, 16> patterns = {"1111", "1112", "1122", "1222", "1113", "1123", "1223", "1131", "1132", "1232",
                                                                    "1311", "1312", "1322", "3111", "3112", "3122"};
      return patterns;
    }

    std::array<std::string_view, 16> L1TdrStgcTriggerLogic::sTGC_triggerPatternsPhiDown() const {
      constexpr static std::array<std::string_view, 16> patterns = {"1111", "1110", "1100", "1000", "1113", "1103", "1003", "1131", "1130", "1230",
                                                                    "1311", "1310", "1300", "3111", "3110", "3100"};
      return patterns;
    }

    std::array<std::string_view, 10> L1TdrStgcTriggerLogic::sTGC_triggerPatternsPhiUpDown() const {
      constexpr static std::array<std::string_view, 10> patterns = {"1111", "1212", "1113", "1213", "1131", "1232", "1311", "1312", "3111", "3101"};
      return patterns;
    }

    std::array<std::string_view, 10> L1TdrStgcTriggerLogic::sTGC_triggerPatternsPhiDownUp() const {
      constexpr static std::array<std::string_view, 10> patterns = {"1111", "1010", "1113", "1013", "1131", "1030", "1311", "1310", "3111", "3121"};
      return patterns;
    }

    //orphant functions .. mut be members TrigT1NSWSimTools or move into a separate file. 
  //-------------------------------------
  std::vector<size_t> L1TdrStgcTriggerLogic::filterByLayer(const std::vector<std::shared_ptr<PadOfflineData>> &pads,
                                         const std::vector<size_t> &padSelectedIndices,
                                         int layer) const
  {
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(layer==pads[idx]->gasGapId()) indices.push_back(idx);
    }
    return indices;
  }

  std::vector<size_t> L1TdrStgcTriggerLogic::filterByMultiplet(const std::vector<std::shared_ptr<PadOfflineData>> &pads,
                                             const std::vector<size_t> &padSelectedIndices,
                                             int multiplet) const
  {
    std::vector<size_t> indices;
    for(size_t i=0; i<padSelectedIndices.size(); i++){
      const size_t &idx=padSelectedIndices[i];
      if(multiplet==pads[idx]->multipletId()) indices.push_back(idx);
    }
    return indices;
  }
}
