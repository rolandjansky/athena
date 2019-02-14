/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMooSegmentCombinationFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMOOSEGMENTCOMBINATIONFINDER_H
#define IMOOSEGMENTCOMBINATIONFINDER_H

#include "GaudiKernel/IAlgTool.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"

namespace Muon 
{


  static const InterfaceID IID_IMooSegmentCombinationFinder("Muon::IMooSegmentCombinationFinder", 1, 0);

  class IMooSegmentCombinationFinder : virtual public IAlgTool {
  public:
    
    struct Output {
      MuonPatternCombinationCollection* patternCombinations;
      Trk::SegmentCollection*           segmentCollection;

    Output() : patternCombinations(0),segmentCollection(0) {}
    };

    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void findSegments( const std::vector<const MdtPrepDataCollection*>& mdtCols,
			       const std::vector<const CscPrepDataCollection*>& cscCols,
			       const std::vector<const TgcPrepDataCollection*>& tgcCols,
			       const std::vector<const RpcPrepDataCollection*>& rpcCols,
			       Output& output) = 0;
    
  };

  inline const InterfaceID& Muon::IMooSegmentCombinationFinder::interfaceID()
    { 
      return IID_IMooSegmentCombinationFinder; 
    }

} // end of namespace

#endif 
