/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyMuonSegmentMaker_H
#define IMboyMuonSegmentMaker_H

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"

namespace Trk {
  class  PrepRawData;
}

class MboyROAStoreAth ;

namespace Muon {

  /**
   @class IMboyMuonSegmentMaker

  @author samusog@cern.ch
  
  */

class IMboyMuonSegmentMaker: public Muon::IMuonSegmentMaker {

public:

   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth ) = 0 ;
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData  ) = 0 ;
   
};

}

#endif

