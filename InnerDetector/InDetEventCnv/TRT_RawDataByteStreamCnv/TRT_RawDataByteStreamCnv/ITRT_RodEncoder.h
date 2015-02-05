/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_RAWDATABYTESTREAMCNV_TRT_RODENCODER_H
#define ITRT_RAWDATABYTESTREAMCNV_TRT_RODENCODER_H

// #include <inttypes.h>

//#include "ByteStreamData/RawEvent.h"
//#include "InDetIdentifier/TRT_ID.h"
//#include "InDetRawData/TRT_RDORawData.h"
//#include "TRT_Cabling/TRT_IdMapping.h"

#include <vector>

class TRT_ID;
class TRT_IdMapping;
class TRT_RDORawData;

/** This class provides conversion from TRT RDO to ROD format. 
   */



class ITRT_RodEncoder 
{
public: 
  typedef TRT_RDORawData          RDO ; 
  typedef std::vector<const RDO*> VRDO; 


  //! add RDO to internal vector
  virtual void addRdo(const RDO * rdo) = 0;

  //! setId Helper
  virtual void set_trt_IdHelper ( const TRT_ID * trt_id) = 0;

  //! setIdMapping
  virtual void set_trt_IdMapping ( TRT_IdMapping* trt_IdMapping) = 0;

  //! setRodMinorVersion 
  virtual void setRodMinorVersion (unsigned short RodBlockVersion) = 0;

  //! conversion of ROD into ROD fragment
  virtual StatusCode fillROD(std::vector<uint32_t>& v, MsgStream& log ) = 0;
  virtual StatusCode fillROD3(std::vector<uint32_t>& v, MsgStream& log ) = 0;

}; 

#endif
