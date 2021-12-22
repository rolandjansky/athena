/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL_H
#define MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL_H

#include "MuonTGC_CnvTools/ITgcPrepDataReplicationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CxxUtils/checker_macros.h"

namespace Muon 
{
  class ATLAS_NOT_THREAD_SAFE TgcPrepDataReplicationTool 
    : public extends<AthAlgTool, ITgcPrepDataReplicationTool>
  {
    public:
      /** Constructor */
      TgcPrepDataReplicationTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcPrepDataReplicationTool()=default;

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode replicate() const override;
      virtual StatusCode convert3BCtoAllBC() const;
      virtual StatusCode convertAllBCto3BC() const;
      
    private:
      enum {BC_PREVIOUS=0, BC_CURRENT, BC_NEXT, BC_ALL, BC_NUM};

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      /** TgcPrepRawData (hit PRD) containers */
      TgcPrepDataContainer* m_tgcPrepDataContainer[BC_NUM]{};

      /** Make new TgcPrepData */
      TgcPrepData* makeTgcPrepData(TgcPrepDataCollection::const_iterator itr, uint16_t bcBitMap) const;

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL_H
