/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcPrepDataReplicationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H
#define MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonTGC_CnvTools/ITgcPrepDataReplicationTool.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"

class TgcIdHelper;

namespace MuonGM
{
  class MuonDetectorManager;
}

namespace Muon 
{
  class TgcPrepDataReplicationTool3BCtoAllBC 
    : virtual public ITgcPrepDataReplicationTool, virtual public AthAlgTool
  {
    public:
      /** Constructor */
      TgcPrepDataReplicationTool3BCtoAllBC(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcPrepDataReplicationTool3BCtoAllBC();

      /** Provide InterfaceID */
      static const InterfaceID& interfaceID() { return ITgcPrepDataReplicationTool::interfaceID(); };

      /** Query interface */
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode replicate() override;
      StatusCode convert3BCtoAllBC();

      
    private:
      enum {BC_PREVIOUS=0, BC_CURRENT, BC_NEXT, BC_ALL, BC_NUM};

      /** muon detector manager */
      const MuonGM::MuonDetectorManager * m_muonMgr;

      /** TGC identifier helper */
      const TgcIdHelper* m_tgcHelper;

      SG::ReadHandleKeyArray<TgcPrepDataContainer> m_3BCKeys;
      SG::WriteHandleKey<TgcPrepDataContainer> m_AllBCKey;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H
