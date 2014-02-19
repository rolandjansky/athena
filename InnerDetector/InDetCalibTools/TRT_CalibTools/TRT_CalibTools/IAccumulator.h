/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__IACCUMULATOR_H
#define TRT_CALIBTOOLS__IACCUMULATOR_H
/********************************************************************

NAME:     IAccumulator
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Alignment info accumulator Tool interface          

********************************************************************/

// INCLUDES:

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TRT_CalibData/IdentifiedProfileHistogram.h"
#include "Identifier/Identifier.h"

namespace TRT {
  class TrackInfo;
}
namespace Trk {
  class Track ;
}

typedef IdentifiedProfileHistogram<Identifier> IdentifierProfileHistogram;

static const InterfaceID IID_IAccumulator("IAccumulator", 1, 0);
  
class IAccumulator : virtual public IAlgTool {
 public:
  
  virtual StatusCode accumulate(const TRT::TrackInfo* inputTrack) = 0 ;
  virtual std::vector<IdentifierProfileHistogram*> getHistograms() { return std::vector<IdentifierProfileHistogram*>() ; }
  
  virtual StatusCode finalize() = 0;
  
  static const InterfaceID& interfaceID();
};

inline const InterfaceID& IAccumulator::interfaceID(){ 
  return IID_IAccumulator; 
}
#endif // TRT_CALIBTOOLS__IACCUMULATOR_H

