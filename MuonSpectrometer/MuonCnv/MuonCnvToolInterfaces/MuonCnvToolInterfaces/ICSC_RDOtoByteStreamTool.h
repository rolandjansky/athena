/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_ICSC_RDOTOBYTESTREAMTOOL_H
#define MUONCNVTOOLINTERFACES_ICSC_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"

class CscRawDataContainer; 
class MsgStream ; 

namespace Muon {
/*
  An AlgTool to provide conversion between CSC RDO <---> ByteStream,
  and fill it in RawEvent.
  
  @author Ketevi A. Assamagan BNL December 27 2003
*/
class ICSC_RDOtoByteStreamTool : virtual public IAlgTool 
{

public:
  DeclareInterfaceID( ICSC_RDOtoByteStreamTool, 1, 0 );

  /** to read the cosmic data */ 
  virtual bool isCosmic () const =0;

  /** for the old cosmic data before the ROB id=ROD id fix */
  virtual bool isOldCosmic () const =0;

  virtual StatusCode convert(const CscRawDataContainer* cont, MsgStream& log) const = 0;
};
}

#endif



