/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"

namespace Muon 
{


  static const InterfaceID IID_IMooSegmentCombinationFinder("Muon::IMooSegmentCombinationFinder", 1, 0);

  class IMooSegmentCombinationFinder : virtual public IAlgTool {
  public:
    
    struct Output {
      const MuonPatternCombinationCollection* patternCombinations;
      const Trk::SegmentCollection*           segmentCollection;
      const MuonSegmentCombinationCollection* segmentCombinations;
      
      Output() : patternCombinations(0),segmentCollection(0),segmentCombinations(0) {}
    };

    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual Output* findSegments( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
				  const std::vector<const CscPrepDataCollection*>& cscCols,  
				  const std::vector<const TgcPrepDataCollection*>& tgcCols,  
				  const std::vector<const RpcPrepDataCollection*>& rpcCols ) = 0;
    
  };

  inline const InterfaceID& Muon::IMooSegmentCombinationFinder::interfaceID()
    { 
      return IID_IMooSegmentCombinationFinder; 
    }

} // end of namespace

#endif 
