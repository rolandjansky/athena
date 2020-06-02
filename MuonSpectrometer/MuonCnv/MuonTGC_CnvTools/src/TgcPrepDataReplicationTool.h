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

namespace Muon 
{
  class TgcPrepDataReplicationTool 
    : virtual public ITgcPrepDataReplicationTool, virtual public AthAlgTool
  {
    public:
      /** Constructor */
      TgcPrepDataReplicationTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcPrepDataReplicationTool()=default;

      /** Provide InterfaceID */
      static const InterfaceID& interfaceID() { return ITgcPrepDataReplicationTool::interfaceID(); };

      /** Query interface */
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode replicate();
      virtual StatusCode convert3BCtoAllBC();
      virtual StatusCode convertAllBCto3BC();
      
    private:
      enum {BC_PREVIOUS=0, BC_CURRENT, BC_NEXT, BC_ALL, BC_NUM};

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      /** TgcPrepRawData (hit PRD) containers */
      TgcPrepDataContainer* m_tgcPrepDataContainer[BC_NUM];

      /** Make new TgcPrepData */
      TgcPrepData* makeTgcPrepData(TgcPrepDataCollection::const_iterator itr, uint16_t bcBitMap);

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOL_H
