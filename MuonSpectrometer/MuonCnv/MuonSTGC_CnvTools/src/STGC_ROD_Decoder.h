/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSTGC_CNVTOOLS_STGC_ROD_DECODER_H
#define MUONSTGC_CNVTOOLS_STGC_ROD_DECODER_H

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonSTGC_CnvTools/ISTGC_ROD_Decoder.h"

class sTgcIdHelper;

namespace Muon
{

class STGC_RawData;
class STGC_RawDataCollection;

class STGC_ROD_Decoder : virtual public ISTGC_ROD_Decoder, public AthAlgTool
{
  public:
    STGC_ROD_Decoder(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~STGC_ROD_Decoder() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment&, const std::vector<IdentifierHash>&, STGC_RawDataContainer&) const override;

  protected:
    const sTgcIdHelper* m_stgcIdHelper = nullptr;
};

} // end of namespace

#endif // MUONSTGC_CNVTOOLS_STGC_ROD_DECODER_H
