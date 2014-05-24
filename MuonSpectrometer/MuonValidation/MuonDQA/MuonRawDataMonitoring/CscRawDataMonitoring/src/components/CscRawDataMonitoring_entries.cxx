#include "GaudiKernel/DeclareFactoryEntries.h"
//////////// CSC ///////////////////////////////
#include "CscRawDataMonitoring/CscRdoValAlg.h"
#include "CscRawDataMonitoring/CscPrdValAlg.h"
#include "CscRawDataMonitoring/CscClusterValAlg.h"
  
  DECLARE_TOOL_FACTORY(CscRdoValAlg)
  DECLARE_TOOL_FACTORY(CscPrdValAlg)
  DECLARE_TOOL_FACTORY(CscClusterValAlg)
    
  DECLARE_FACTORY_ENTRIES(CscRawDataMonitoring)
{
  DECLARE_ALGTOOL(CscRdoValAlg)
  DECLARE_ALGTOOL(CscPrdValAlg)
  DECLARE_ALGTOOL(CscClusterValAlg)   
} 
 
