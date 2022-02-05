/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkPrepRawData/PrepRawData.h"

/** Must declare this, with name of interface*/
static const InterfaceID IID_IMuonCombinePatternTool("Muon::IMuonCombinePatternTool", 1, 0);

namespace Muon {

    /** comparison functor for Trk::PrepRawData* (on identfier) used for sorting
       set<Trk::PrepRawData*> , else these sets are sorted by memory address
       (introduces random behaviour)
    */

    struct IdentifierPrdLess {
        bool operator()(const Trk::PrepRawData* pT1, const Trk::PrepRawData* pT2) const { return pT1->identify() < pT2->identify(); }
    };

    /** Interface for tools combining Muon::MuonPattern objects */
    class IMuonCombinePatternTool : virtual public IAlgTool {
    public:
        /**Declared here, and defined below*/
        static const InterfaceID& interfaceID();

        /** @brief combine a collection of Muon::MuonPattern object in the phi-plane
         * with a collection of Muon::MuonPattern objects in the eta plane */
        virtual MuonPrdPatternCollection* combineEtaPhiPatterns(
            const MuonPrdPatternCollection* phipatterns, const MuonPrdPatternCollection* etapatterns,
            const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phiEtaHitAssMap) const = 0;

        /** @brief combine a Muon::MuonPattern object in the phi-plane with one the in
         * the eta plane */
        virtual Muon::MuonPrdPattern* makeCombinedPattern(const Muon::MuonPrdPattern* phipattern,
                                                          const Muon::MuonPrdPattern* etapattern) const = 0;

        /** @brief create a collection of Muon::MuonPatternCombination from a
         * collection of Muon::MuonPrdPattern objects */
        virtual MuonPatternCombinationCollection* makePatternCombinations(const MuonPrdPatternCollection* combinedpatterns) const = 0;
    };

    inline const InterfaceID& IMuonCombinePatternTool::interfaceID() { return IID_IMuonCombinePatternTool; }

}  // namespace Muon
