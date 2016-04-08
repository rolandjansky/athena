/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IMUONRAWDATAPROVIDERTOOL_H
#define MUONCNVTOOLINTERFACES_IMUONRAWDATAPROVIDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "Identifier/IdentifierHash.h"
#include <vector>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

static const InterfaceID IID_IMuonRawDataProviderTool( "Muon::IMuonRawDataProviderTool", 1, 0 );

namespace Muon
{

/**Interface defining the tools used to convert Muon BS to MuonRDOs.
*/
class IMuonRawDataProviderTool : virtual public IAlgTool
{
public:
    static const InterfaceID& interfaceID()
    {
        return IID_IMuonRawDataProviderTool;
    };

public:
    /** Decoding method. - current methods: let's keep them! */
    typedef std::vector<const ROBFragment*> ROBFragmentList;
    virtual StatusCode convert(const ROBFragmentList&) = 0;
    virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) = 0;
    /** the new ones */
    virtual StatusCode convert() = 0; //!< for the entire event 
    virtual StatusCode convert(const std::vector<IdentifierHash>&) = 0; //!< for a selection of rdo collections
    virtual StatusCode convert(const std::vector<uint32_t>&){return StatusCode::FAILURE;}
};
}

#endif // !MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
