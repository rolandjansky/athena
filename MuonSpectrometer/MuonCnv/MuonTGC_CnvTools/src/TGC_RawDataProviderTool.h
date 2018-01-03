/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TGC_RawDataProviderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
#define MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "TGC_Hid2RESrcID.h"

class TgcRdoContainer;
class ActiveStoreSvc;
class IROBDataProviderSvc;
class ITGCcablingSvc;

namespace MuonGM
{
  class MuonDetectorManager;
}

namespace Muon
{
  class ITGC_RodDecoder;

  /** @class TGC_RawDataProviderTool
   *  A tool to decode TGC ROB fragments into TGC RDO.
   *
   *  @author Zvi Tarem <zvi@caliper.co.il>
   */
  
  class TGC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public AthAlgTool
    {
    public:
      /** Default constructor */
      TGC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RawDataProviderTool();
      
      /** Standard AlgTool method */
      virtual StatusCode initialize();
      /** Standard AlgTool method */
      virtual StatusCode finalize();
      
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert(const ROBFragmentList& vecRobs);
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool with IdentifierHash vector */
      virtual StatusCode convert(const ROBFragmentList& vecRobs,
				 const std::vector<IdentifierHash>& rdoIdhVect);
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert();
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool with ID Hash vector */
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect);

    private:
      /** MuonDectorManager is used to get tgcIdHelper */
      const MuonGM::MuonDetectorManager*  m_muonMgr;
      /** Decoder for ROB fragment RDO conversion */
      ToolHandle<ITGC_RodDecoder>         m_decoder;
      /** Container of RDO containers 
	  TgcRawData is an RDO.
	  TgcRdo is an RDO container. 
	  TgcRdoContiner is an container of RDO containers.  
       */
      //TgcRdoContainer*                    m_rdoContainer;
      /** RDO container key */
      SG::WriteHandleKey<TgcRdoContainer> m_rdoContainerKey{
	this, "RdoLocation", "TGCRDO", "Name of the TGCRDO produced by RawDataProvider"};	//MT
      bool 		m_useContainer;	//MT
      unsigned int 	m_maxhashtoUse;	//MT
      /** Active Store Service */ 
      ActiveStoreSvc*                     m_activeStore;
      /** ID converter */
      TGC_Hid2RESrcID                     m_hid2re;
      /** TGC cabling Svc */
      const ITGCcablingSvc                *m_cabling;
      /** Rob Data Provider handle */
      ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
      /** Avoid compiler warning **/
      virtual StatusCode convert(const std::vector<uint32_t>&) {return StatusCode::FAILURE;}

      StatusCode getCabling();
    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
