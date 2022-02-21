/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOL_H
#define MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

class TgcRdoContainer;
class IROBDataProviderSvc;
class ITGCcablingSvc;

namespace Muon
{
  class ISTGC_RodDecoder;

  /** @class STGC_RawDataProviderTool
   */
  
  class STGC_RawDataProviderTool : public extends<AthAlgTool, IMuonRawDataProviderTool>
    {
    public:
      using IMuonRawDataProviderTool::convert; // Otherwise get a -Woverloaded-virtual warning about hiding convert(const std::vector<IdentifierHash>& rdoIdhVect)
      /** Default constructor */
      STGC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~STGC_RawDataProviderTool();
      
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;
      /** Standard AlgTool method */
      virtual StatusCode finalize() override;
      
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert(const ROBFragmentList& vecRobs) const override;
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool with IdentifierHash vector */
      virtual StatusCode convert(const ROBFragmentList& vecRobs,
				 const std::vector<IdentifierHash>& rdoIdhVect) const override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert() const override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool with ID Hash vector */
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect) const override;

    private:

    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
