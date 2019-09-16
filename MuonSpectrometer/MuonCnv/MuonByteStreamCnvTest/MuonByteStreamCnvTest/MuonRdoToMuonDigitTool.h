/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOMUONDIGITTOOL_H
#define MUONRDOTOMUONDIGITTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "CscCalibTools/CscCalibTool.h"
#include "CscCalibTools/ICscCalibTool.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"

class IRPCcablingSvc;
class ITGCcablingSvc;

class MdtDigitContainer;
class CscDigitContainer;
class RpcDigitContainer;
class TgcDigitContainer;
class sTgcDigitContainer;
class MmDigitContainer;

class MdtDigitCollection;
class CscDigitCollection;
class RpcDigitCollection;
class TgcDigitCollection;
class sTgcDigitCollection;
class MmDigitCollection;

class MdtCsm;
class CscRawDataCollection;
class RpcPad;
class TgcRdoCollection;
class STGC_RawDataCollection;
class STGC_RawDataContainer;
class STGC_RawData;
class MM_RawDataCollection;
class MM_RawDataContainer;
class MM_RawData;

namespace Muon {
  class IMDT_RDO_Decoder;
  class ICSC_RDO_Decoder;
  class IRPC_RDO_Decoder;
  class ITGC_RDO_Decoder;
  class ISTGC_RDO_Decoder;
  class IMM_RDO_Decoder;
  class MuonIdHelperTool;
}
// Author: Ketevi A. Assamagan
// BNL, January 24, 2004

// algorithm to decode RDO into digits
// get the RDO container from Storegate
// loop over the RDO
// Decode RDO into digits
// loop over the digits and build the digit container
// store the digit container in StoreGate

class MuonRdoToMuonDigitTool : virtual public IMuonDigitizationTool, public AthAlgTool {

 public:

  MuonRdoToMuonDigitTool(const std::string& type, const std::string& name, const IInterface* pIID);
  ~MuonRdoToMuonDigitTool();
    
  StatusCode initialize();
  StatusCode digitize();
  StatusCode finalize();

 private:

  // private method for the decoding RDO --> digits
  StatusCode decodeMdtRDO();
  StatusCode decodeMdt( const MdtCsm *, MdtDigitCollection*&, Identifier& );

  StatusCode decodeCscRDO();
  //  StatusCode decodeCsc( const CscRawDataCollection *, CscDigitCollection*, Identifier&, CscRDO_Decoder& decoder );
  StatusCode decodeCsc( const CscRawDataCollection *, CscDigitCollection*&, Identifier&);

  StatusCode decodeRpcRDO();
  StatusCode decodeRpc( const RpcPad *, RpcDigitCollection*& );
 
  StatusCode decodeTgcRDO();
  StatusCode decodeTgc( const TgcRdo *, Identifier&);

  StatusCode decodeSTGC_RDO();
  StatusCode decodeSTGC( const Muon::STGC_RawDataCollection *, sTgcDigitCollection*&, Identifier&);

  StatusCode decodeMM_RDO();
  StatusCode decodeMM( const Muon::MM_RawDataCollection *, MmDigitCollection*&, Identifier&);

  StatusCode getTgcCabling();

 private:

  // store gate transactions
  ServiceHandle<ActiveStoreSvc> m_acSvc;
  ToolHandle<ICscCalibTool>     m_cscCalibTool;
  ToolHandle<Muon::IMDT_RDO_Decoder>  m_mdtRdoDecoderTool;
  ToolHandle<Muon::ICSC_RDO_Decoder>  m_cscRdoDecoderTool;
  ToolHandle<Muon::IRPC_RDO_Decoder>  m_rpcRdoDecoderTool;
  ToolHandle<Muon::ITGC_RDO_Decoder>  m_tgcRdoDecoderTool;
  ToolHandle<Muon::ISTGC_RDO_Decoder>  m_stgcRdoDecoderTool;
  ToolHandle<Muon::IMM_RDO_Decoder>  m_mmRdoDecoderTool;

  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;

  // cabling service
  //  const MDTcablingSvc  * m_mdtCabling;
  const IRPCcablingSvc * m_rpcCabling;
  const ITGCcablingSvc * m_tgcCabling;

  // digit containers
  MdtDigitContainer *   m_mdtContainer;
  CscDigitContainer *   m_cscContainer;
  RpcDigitContainer *   m_rpcContainer;
  TgcDigitContainer *   m_tgcContainer;
  sTgcDigitContainer *   m_stgcContainer;
  MmDigitContainer *   m_mmContainer;

  // algorithm properties
  bool m_decodeMdtRDO;
  bool m_decodeCscRDO;
  bool m_decodeRpcRDO;
  bool m_decodeTgcRDO;
  bool m_decodesTgcRDO;
  bool m_decodeMmRDO;

  /** Switch for warning message disabling on one invalid channel in 
      TGC sector A09 seen in 2008 data, at least run 79772 - 91800. 
      bug #48828: TgcRdoToTgcDigit WARNING ElementID not found for 
      sub=103 rod=9 ssw=6 slb=20 bitpos=151 +offset=0 orFlag=0
  */
  bool m_show_warning_level_invalid_TGC_A09_SSW6_hit;


  /** Flag to distinguish 12-fold TGC cabling and 8-fold TGC cabling */
  bool m_is12foldTgc;

  SG::ReadHandleKey<MdtCsmContainer> m_mdtRdoKey{this, "MdtRdoContainer", "MDTCSM", "Mdt RDO Input"};
  SG::WriteHandleKey<MdtDigitContainer> m_mdtDigitKey{this, "MdtDigitContainer", "MDT_DIGITS", "Mdt Digit Output"};
  SG::ReadHandleKey<CscRawDataContainer> m_cscRdoKey{this, "CscRdoContainer", "CSCRDO", "Csc RDO Input"};
  SG::WriteHandleKey<CscDigitContainer> m_cscDigitKey{this, "CscDigitContainer", "CSC_DIGITS", "Csc Digit Output"};
  SG::ReadHandleKey<RpcPadContainer> m_rpcRdoKey{this, "RpcRdoContainer", "RPCPAD", "Rpc RDO Input"};
  SG::WriteHandleKey<RpcDigitContainer> m_rpcDigitKey{this, "RpcDigitContainer", "RPC_DIGITS", "Rpc Digit Output"};
  SG::ReadHandleKey<TgcRdoContainer> m_tgcRdoKey{this, "TgcRdoContainer", "TGCRDO", "Tgc RDO Input"};
  SG::WriteHandleKey<TgcDigitContainer> m_tgcDigitKey{this, "TgcDigitContainer", "TGC_DIGITS", "Tgc Digit Output"};
  SG::ReadHandleKey<Muon::STGC_RawDataContainer> m_stgcRdoKey{this, "sTgcRdoContainer", "sTGCRDO", "sTgc RDO Input"};
  SG::WriteHandleKey<sTgcDigitContainer> m_stgcDigitKey{this, "sTgcDigitContainer", "sTGC_DIGITS", "sTgc Digit Output"};
  SG::ReadHandleKey<Muon::MM_RawDataContainer> m_mmRdoKey{this, "MmRdoContainer", "MMRDO", "MM RDO Input"};
  SG::WriteHandleKey<MmDigitContainer> m_mmDigitKey{this, "MmDigitContainer", "MM_DIGITS", "MM Digit Output"};
};


#endif 
