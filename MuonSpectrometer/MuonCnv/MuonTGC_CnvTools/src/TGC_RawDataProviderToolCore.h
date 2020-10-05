/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLCORE_H
#define MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TGC_Hid2RESrcID.h"
#include "ByteStreamData/RawEvent.h" 
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonTGC_CnvTools/ITGC_RodDecoder.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

class TgcRdoContainer;

namespace Muon {

  /** @class TGC_RawDataProviderToolCore
   *  A tool to decode TGC ROB fragments into TGC RDO.
   *
   *  @author Zvi Tarem <zvi@caliper.co.il>
   *  @author Mark Owen <markowen@cern.ch>
   */
  
  class TGC_RawDataProviderToolCore : public AthAlgTool
    {
    public:
      /** Default constructor */
      TGC_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RawDataProviderToolCore()=default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize();
      
      /** Method that converts the ROBFragments into the passed container */
      virtual StatusCode convertIntoContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, TgcRdoContainer& tgcRdoContainer);


    protected:
      /** Function to get the ROB data from a vector of IdentifierHash **/
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> getROBData(const std::vector<IdentifierHash>& rdoIdhVect);

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      /** Decoder for ROB fragment RDO conversion */
      ToolHandle<ITGC_RodDecoder> m_decoder{this,"Decoder","Muon::TGC_RodDecoderReadout/TGC_RodDecoderReadout"};
      /** RDO container key */
      SG::WriteHandleKey<TgcRdoContainer> m_rdoContainerKey{ this, "RdoLocation", "TGCRDO", "Name of the TGCRDO produced by RawDataProvider"};	//MT

      unsigned int 	m_maxhashtoUse;	//MT

      /** ID converter */
      TGC_Hid2RESrcID                     m_hid2re;
      /** TGC cabling Svc */
      const ITGCcablingSvc                *m_cabling;
      /** Rob Data Provider handle */
      ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;

      StatusCode getCabling();
    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLCORE_H
