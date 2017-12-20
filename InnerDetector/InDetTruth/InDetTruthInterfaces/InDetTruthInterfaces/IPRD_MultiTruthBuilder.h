/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Interface to build a "multi"-truth collection, which contains info
// about all contributing MC particles, for PrepRawData.
//
// A. Gaponenko, 2006

#ifndef IPRD_MULTITRUTHBUILDER_H
#define IPRD_MULTITRUTHBUILDER_H

#include "GaudiKernel/IAlgTool.h"

// Forard declarations of types.
class PRD_MultiTruthCollection;
namespace Trk { class PrepRawData; }
class InDetSimDataCollection;

namespace InDet {
  
  static const InterfaceID IID_IPRD_MultiTruthBuilder("InDet::IPRD_MultiTruthBuilder",1,0);

  class IPRD_MultiTruthBuilder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_IPRD_MultiTruthBuilder; }

    // The main tool method
    virtual void addPrepRawDatum(PRD_MultiTruthCollection *prdTruth,
				 const Trk::PrepRawData* prd,
				 const InDetSimDataCollection* simDataMap,
				 bool pixels
				 ) const = 0;
    
  };

} // namespace InDet

#endif/*IPRD_MULTITRUTHBUILDER_H*/
