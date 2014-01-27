#include "GaudiKernel/DeclareFactoryEntries.h"
//////////// MDT ///////////////////////////////
#include "MdtRawDataMonitoring/MdtRawDataValAlg.h"
//#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
//#include "MdtRawDataMonitoring/MuonEventInfoMonTool.h"
 
DECLARE_TOOL_FACTORY(MdtRawDataValAlg)   
//DECLARE_ALGORITHM(MuonChamberIDSelector)
//DECLARE_TOOL_FACTORY(MuonEventInfoMonTool)
   
DECLARE_FACTORY_ENTRIES(MtdRawDataMonitoring)
{
   DECLARE_ALGTOOL(MdtRawDataValAlg)         
//   DECLARE_ALGTOOL(MuonChamberIDSelector)   
//   DECLARE_ALGTOOL(MuonEventInfoMonTool)
} 
 
