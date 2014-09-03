/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ToolHandle.h"

//class IPixelRunConditionsDbSvc;
class IPixelConnCoolDbSvc;
class IPixelReadoutSpeedDbSvc;
class IPixelCablingSvc;

class DumpCabling : public AthAlgorithm
{
public:

  /// Constructor
  DumpCabling(const std::string& name,
	      ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  //ServiceHandle< IPixelRunConditionsDbSvc > m_runCondDbSvc;
  ServiceHandle< IPixelConnCoolDbSvc > m_connCoolDbSvc;
  ServiceHandle< IPixelReadoutSpeedDbSvc > m_readoutspeedDbSvc;
  ServiceHandle< IPixelCablingSvc > m_pixelCablingSvc;

  bool m_write;
};
