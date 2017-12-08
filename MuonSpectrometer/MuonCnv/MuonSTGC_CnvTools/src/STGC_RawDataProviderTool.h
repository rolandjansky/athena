/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOL_H
#define MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

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
  class ISTGC_RodDecoder;

  /** @class STGC_RawDataProviderTool
   */
  
  class STGC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public AthAlgTool
    {
    public:
      using IMuonRawDataProviderTool::convert; // Otherwise get a -Woverloaded-virtual warning about hiding convert(const std::vector<IdentifierHash>& rdoIdhVect)
      /** Default constructor */
      STGC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~STGC_RawDataProviderTool();
      
      /** Standard AlgTool method */
      virtual StatusCode initialize();
      /** Standard AlgTool method */
      virtual StatusCode finalize();
      
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert(const ROBFragmentList& vecRobs) override;
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool with IdentifierHash vector */
      virtual StatusCode convert(const ROBFragmentList& vecRobs,
				 const std::vector<IdentifierHash>& rdoIdhVect) override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert() override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool with ID Hash vector */
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect) override;

    private:
      /** MuonDectorManager is used to get tgcIdHelper */
      const MuonGM::MuonDetectorManager*  m_muonMgr;

    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
