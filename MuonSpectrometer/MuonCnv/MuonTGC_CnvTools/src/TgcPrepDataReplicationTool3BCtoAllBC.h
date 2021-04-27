/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H
#define MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H

#include "MuonTGC_CnvTools/ITgcPrepDataReplicationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CxxUtils/checker_macros.h"

namespace Muon 
{
  class ATLAS_NOT_THREAD_SAFE TgcPrepDataReplicationTool3BCtoAllBC 
    : public extends<AthAlgTool, ITgcPrepDataReplicationTool>
  {
    public:
      /** Constructor */
      TgcPrepDataReplicationTool3BCtoAllBC(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcPrepDataReplicationTool3BCtoAllBC()=default;

      virtual StatusCode initialize() override;
      virtual StatusCode replicate() const override;
      StatusCode convert3BCtoAllBC() const;

      
    private:
      enum {BC_PREVIOUS=0, BC_CURRENT, BC_NEXT, BC_ALL, BC_NUM};

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      SG::ReadHandleKeyArray<TgcPrepDataContainer> m_3BCKeys;
      SG::WriteHandleKey<TgcPrepDataContainer> m_AllBCKey;
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL3BCtoALLBC_H
