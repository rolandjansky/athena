/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRT_RODENCODER_H
#define TRT_RAWDATABYTESTREAMCNV_TRT_RODENCODER_H

#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include <inttypes.h>
#include <vector>

class TRT_RDORawData;
class TRT_ID;
class ITRT_CablingSvc;
class MsgStream;

/** This class provides conversion from TRT RDO to ROD format. 
   */

class TRT_RodEncoder 
{
public: 
  typedef TRT_RDORawData          RDO ; 
  typedef std::vector<const RDO*> VRDO; 

  //! constructor 
  TRT_RodEncoder(); 

  //! destructor 
  ~TRT_RodEncoder(); 

  //! add RDO to internal vector
  void addRdo(const RDO * rdo) {m_RDOs.push_back(rdo);}

  //! setId Helper
  void set_trt_IdHelper ( const TRT_ID * trt_id) { m_trt_id = trt_id   ; }

  //! setCablingSvc
  void set_trt_cabling ( ServiceHandle<ITRT_CablingSvc> trt_CablingSvc)
  { 
    m_CablingSvc = trt_CablingSvc;
  }

  //! setRodMinorVersion 
  void setRodMinorVersion (unsigned short RodBlockVersion) { m_RodBlockVersion = RodBlockVersion; }

  //! conversion of ROD into ROD fragment
  StatusCode fillROD(std::vector<uint32_t>& v ) ; 

  //! conversion of ROD into compressed ROD fragment
  StatusCode fillROD3(std::vector<uint32_t>& v ) ; 

  private:
    const TRT_ID * m_trt_id;
    ServiceHandle<ITRT_CablingSvc> m_CablingSvc;
    unsigned short m_RodBlockVersion;
    VRDO           m_RDOs;
}; 

#endif // TRT_RAWDATABYTESTREAMCNV_TRT_RODENCODER_H
