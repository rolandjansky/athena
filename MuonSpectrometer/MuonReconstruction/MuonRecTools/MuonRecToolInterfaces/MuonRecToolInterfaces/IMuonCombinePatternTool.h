/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IAlgTool.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

/** Must declare this, with name of interface*/
static const InterfaceID IID_IMuonCombinePatternTool("Muon::IMuonCombinePatternTool", 1, 0);

namespace Muon {

  /** comparison functor for Trk::PrepRawData* (on identfier) used for sorting set<Trk::PrepRawData*> , 
      else these sets are sorted by memory address (introduces random behaviour)
  */
  
  struct IdentifierPrdLess {
    bool operator()(const Trk::PrepRawData* pT1,const Trk::PrepRawData* pT2)const {
      return pT1->identify() < pT2->identify();
    }
  };

  /** Interface for tools combining Muon::MuonPattern objects */
  class IMuonCombinePatternTool : virtual public IAlgTool 
  {
  public:
    /**Declared here, and defined below*/
    static const InterfaceID& interfaceID();
    
    /** @brief combine a collection of Muon::MuonPattern object in the phi-plane with a collection of Muon::MuonPattern objects in the eta plane */
    virtual const MuonPrdPatternCollection* combineEtaPhiPatterns(const MuonPrdPatternCollection* phipatterns, const MuonPrdPatternCollection* etapatterns)const=0;
    
    /** @brief combine a Muon::MuonPattern object in the phi-plane with one the in the eta plane */
    virtual Muon::MuonPrdPattern* makeCombinedPattern(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const=0;

    /** @brief sets a map that associates phi and eta trigger hits (e.g. based on identical gasgap) */
    virtual void setPhiEtaHitAssMap(std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,IdentifierPrdLess> >*)=0;

    /** @brief create a collection of Muon::MuonPatternCombination from a collection of Muon::MuonPrdPattern objects */
    virtual MuonPatternCombinationCollection* makePatternCombinations(const MuonPrdPatternCollection* combinedpatterns)const=0;
  };
  
  inline const InterfaceID& IMuonCombinePatternTool::interfaceID()
    { 
      return IID_IMuonCombinePatternTool; 
    }

}
