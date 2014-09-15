/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyPrepDataToDigiStoreTool_H
#define MboyPrepDataToDigiStoreTool_H

#include "AthenaBaseComps/AthAlgTool.h"

class MdtIdHelper ;
class RpcIdHelper ;
class TgcIdHelper ;
class CscIdHelper ;

namespace Trk {
  class  PrepRawData;
}
namespace Muon {
  class  CscPrepData;
  class  MdtPrepData;
  class  RpcPrepData;
  class  TgcPrepData;
  class  CscStripPrepData;
}

#include "MboyEDMToolInterfaces/IMboyPrepDataToDigiStoreTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MuonRecDigiStoreAth;

  /**
   @class MboyPrepDataToDigiStoreTool

   This class is filling a MuonRecDigiStoreAth with PrepRawData

  @author samusog@cern.ch
  
  */

class MboyPrepDataToDigiStoreTool:public AthAlgTool, virtual public IMboyPrepDataToDigiStoreTool{
public:
  MboyPrepDataToDigiStoreTool(const std::string&,const std::string&,const IInterface*);
  virtual ~MboyPrepDataToDigiStoreTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Fill with PrepRawData vectors */
   virtual void FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                          const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                          );

   /** Fill with PrepRawData vectors */
   virtual void FillMuonRecDigiStoreWithPrepData_Base(
                                          const Trk::PrepRawData* pPrepData,
                                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                          );

private:
///////////////////////////////////

   /** Fill with PrepRawData vectors MDT*/
   void FillMuonRecDigiStoreWithMDT_Base(
                                         const Muon::MdtPrepData* pMdtPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );

   /** Fill with PrepRawData vectors RPC*/
   void FillMuonRecDigiStoreWithRPC_Base(
                                         const Muon::RpcPrepData* pRpcPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );

   /** Fill with PrepRawData vectors TGC*/
  void FillMuonRecDigiStoreWithTGC_Base(
                                          const Muon::TgcPrepData* pTgcPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );
   /** Fill with PrepRawData vectors CSC*/
   void FillMuonRecDigiStoreWithCSC_Base(
                                         const Muon::CscStripPrepData* pCscStripPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );


   /** Fill with PrepRawData vectors CSC clustesr*/
   void FillMuonRecDigiStoreWithCSCCluster_Base(
                                         const Muon::CscPrepData* pCscPrepData,
                                         MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                         );


   int m_DoReserve ; //!< Control room reservation
   
   int m_DoDbg ; //!< Control some debug

   const MdtIdHelper * m_mdtId ; //!< IdHelper
   const CscIdHelper * m_cscId ; //!< IdHelper
   const RpcIdHelper * m_rpcId ; //!< IdHelper
   const TgcIdHelper * m_tgcId ; //!< IdHelper

};

#endif

