/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcPrepDataReplicationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOLAllBCto3BC_H
#define MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOLAllBCto3BC_H

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
  class TgcPrepDataReplicationToolAllBCto3BC 
    : virtual public ITgcPrepDataReplicationTool, virtual public AthAlgTool
  {
    public:
      /** Constructor */
      TgcPrepDataReplicationToolAllBCto3BC(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcPrepDataReplicationToolAllBCto3BC();

      /** Provide InterfaceID */
      static const InterfaceID& interfaceID() { return ITgcPrepDataReplicationTool::interfaceID(); };

      /** Query interface */
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode replicate() override;
      StatusCode convertAllBCto3BC();
      
/** Make new TgcPrepData */ //Static to avoid code duplication with sister class
      static TgcPrepData* makeTgcPrepData(TgcPrepDataCollection::const_iterator itr, uint16_t bcBitMap);

    private:
      enum {BC_PREVIOUS=0, BC_CURRENT, BC_NEXT, BC_ALL, BC_NUM};

      /** muon detector manager */
      const MuonGM::MuonDetectorManager * m_muonMgr;

      /** TGC identifier helper */
      const TgcIdHelper* m_tgcHelper;

      SG::WriteHandleKeyArray<TgcPrepDataContainer> m_3BCKeys;
      SG::ReadHandleKey<TgcPrepDataContainer> m_AllBCKey;

      
      

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCPREPDATAREPLICATIONTOOLAllBCto3BC_H
