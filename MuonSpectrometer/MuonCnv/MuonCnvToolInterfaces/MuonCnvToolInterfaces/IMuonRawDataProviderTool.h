/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IMUONRAWDATAPROVIDERTOOL_H
#define MUONCNVTOOLINTERFACES_IMUONRAWDATAPROVIDERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventContext.h"
#include <vector>


namespace Muon
{

/**Interface defining the tools used to convert Muon BS to MuonRDOs.
*/
class IMuonRawDataProviderTool : virtual public IAlgTool
{
public:
    DeclareInterfaceID( IMuonRawDataProviderTool, 1, 0 );

public:
    /** Decoding method. - current methods: let's keep them! */
    typedef std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> ROBFragmentList;
    virtual StatusCode convert(const ROBFragmentList&) const = 0;
    virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const = 0;
    /** the new ones */
    virtual StatusCode convert() const = 0; //!< for the entire event 
    virtual StatusCode convert(const std::vector<IdentifierHash>&) const = 0; //!< for a selection of rdo collections
    virtual StatusCode convert(const std::vector<uint32_t>&) const {return StatusCode::FAILURE;}
    /** Event Context functions **/
    virtual StatusCode convert(const EventContext&) const {return StatusCode::FAILURE;}
    virtual StatusCode convert(const ROBFragmentList&, const EventContext&) const {return StatusCode::FAILURE;}
    virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&, const EventContext&) const {return StatusCode::FAILURE;}
    virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const {return StatusCode::FAILURE;}
    virtual StatusCode convert(const std::vector<uint32_t>&, const EventContext&) const {return StatusCode::FAILURE;}
    
    
};
}

#endif // !MUONCNVTOOLINTERFACES_IMUONRDOTOPREPDATATOOL_H
