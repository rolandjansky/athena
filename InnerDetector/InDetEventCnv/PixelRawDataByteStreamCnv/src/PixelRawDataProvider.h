/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Top algorithm to decode the Pixel BS
///////////////////////////////////////////////////////////////////

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

// Forward declarations
class IPixelRawDataProviderTool;
class PixelID;
class IROBDataProviderSvc;
class IPixelCablingSvc;

class PixelRawDataProvider : public AthAlgorithm {

 public:

  //! Constructor.
  PixelRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  StatusCode initialize();
  //! Execute
  StatusCode execute();
  //! Finalize
  StatusCode finalize() { return StatusCode::SUCCESS;}
  
  //! Destructur
  ~PixelRawDataProvider();

private:

  ServiceHandle<IPixelCablingSvc>      m_pixelCabling;
  ServiceHandle<IROBDataProviderSvc>   m_robDataProvider;
  ToolHandle<IPixelRawDataProviderTool> m_rawDataTool;
  const PixelID*                       m_pixel_id; 
  std::string                          m_RDO_Key;
};

#endif

