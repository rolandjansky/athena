/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLMT
#define MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLMT

#include "sTgcRdoToPrepDataToolCore.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class AtlasDetectorID;
class Identifier;

class ITGCcablingSvc;

namespace MuonGM 
{
  class MuonDetectorManager;
  class TgcReadoutElement; 
}

namespace Muon 
{
  class IMuonRawDataProviderTool;
  class ISTgcClusterBuilderTool;

  /** @class STGC_RawDataToPrepDataTool 
   *  This is the algorithm that convert STGC Raw data  To STGC PRD  as a tool.
   */  

  class sTgcRdoToPrepDataToolMT : virtual public sTgcRdoToPrepDataToolCore
    {
    public:
      /** Constructor */
      sTgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~sTgcRdoToPrepDataToolMT();
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      /** Standard AthAlgTool finalize method */
      virtual StatusCode finalize() override;

    protected:
      virtual SetupSTGC_PrepDataContainerStatus setupSTGC_PrepDataContainer() override;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL_H
