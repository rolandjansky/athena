/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_CSCROD_ENCODER_H
#define MUONCSC_CNVTOOL_CSCROD_ENCODER_H

#include <inttypes.h>

#include "ByteStreamData/RawEvent.h"
#include "CSC_Hid2RESrcID.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonRDO/CscRawDataCollection.h"

class CscRawDataCollection;
class MsgStream;
class StatusCode;

/** This class provides conversion from CSC RDO to ROD format. 
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   * following T. Maeno's example for TGC
   */

class CscROD_Encoder
{
public: 

  /** constructor 
   */
  CscROD_Encoder(); 

  ~CscROD_Encoder()=default; 

   /** set CscRawDataCollection
   */ 
  void setRdo(const CscRawDataCollection * rdo) {m_cscRdo = rdo;} 

  /** convert all CscRawDataCollections in the current list to 
      a vector of 32bit words
  */ 
  StatusCode fillROD(std::vector<uint32_t>& v, MsgStream& mLog) ; 

  /** setIdHelper
   */
  void setIdHelper (const CscIdHelper* cscIdHelper) { m_cscIdHelper = cscIdHelper; }

private:
  const CscRawDataCollection * m_cscRdo; 
  const CscIdHelper* m_cscIdHelper;
 
}; 

#endif


