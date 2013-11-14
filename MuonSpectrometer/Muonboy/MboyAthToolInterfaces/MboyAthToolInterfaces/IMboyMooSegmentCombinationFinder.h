/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyMooSegmentCombinationFinder_H
#define IMboyMooSegmentCombinationFinder_H

#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"

namespace Trk {
  class  PrepRawData;
}

class MboyROAStoreAth ;

namespace Muon {

  /**
   @class IMboyMooSegmentCombinationFinder

  @author samusog@cern.ch
  
  */

class IMboyMooSegmentCombinationFinder: public Muon::IMooSegmentCombinationFinder {

public:

   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth ) = 0 ;
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData) = 0 ;
   
};

}

#endif

