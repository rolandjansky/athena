/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Top algorithm to decode the ALFA BS
///////////////////////////////////////////////////////////////////

#ifndef ALFA_RAWDATABYTESTREAMCNV_ALFA_RAWDATAPROVIDER_CHARGE_H
#define ALFA_RAWDATABYTESTREAMCNV_ALFA_RAWDATAPROVIDER_CHARGE_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ALFA_RawEv/ALFA_RawEvDict.h"

#include "ALFA_RawDataProviderTool_charge.h"

#include "DataModel/DataVector.h"

#include <map>
#include <string>

class ALFA_RawDataProviderTool_charge;
class ISvcLocator;
class StatusCode;


class ALFA_RawDataProvider_charge : public AthAlgorithm
{
 public:

  //! Constructor.
  ALFA_RawDataProvider_charge(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize()
  { return StatusCode::SUCCESS; }


  //! Destructor
  ~ALFA_RawDataProvider_charge();

 private:

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  /// Tool that does the actual work
  ToolHandle<ALFA_RawDataProviderTool_charge> m_rawDataTool_charge;

  std::string m_ALFA_RawDataCollectionKey_charge;
  std::string m_collection;

};

#endif

 
