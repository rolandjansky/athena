/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcID_H
#define ZdcID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <algorithm>
#include <vector>

class IdDictDictionary;

class ZdcID : public AtlasDetectorID
{
public:


    typedef Identifier::size_type size_type; 
    typedef std::vector<Identifier>::const_iterator const_id_iterator;
    typedef MultiRange::const_identifier_factory const_expanded_id_iterator;

    ZdcID(void);
    ~ZdcID(void);

    /// @name Optimized accessors  - ASSUMES id IS a sct id, i.e. NOT pixel or other
    //@{
    /// module hash from id - optimized
    IdentifierHash      module_hash      (Identifier module_id) const;

    /// channel hash from id - optimized
    IdentifierHash      channel_hash      (Identifier channel_id) const;

    /// Values of different levels (failure returns 0)
    int                 side      (const Identifier& id) const;
    int                 module    (const Identifier& id) const;
    int                 type      (const Identifier& id) const;
    int                 channel   (const Identifier& id) const;
    //@}

    /// @name Hash table maximum sizes
    //@{
    size_type           module_hash_max (void) const;
    size_type           channel_hash_max (void) const;
    //@}

    /// @name Access to all ids
    //@{
    /// Iterators over full set of ids. modules iterator is sorted
    const_id_iterator   modules_begin (void) const;
    const_id_iterator   modules_end   (void) const;

    /// Iterators over full set of ids. channels iterator is sorted
    const_id_iterator   channels_begin (void) const;
    const_id_iterator   channels_end   (void) const;
    //@}


    /// @name Creators for zdc ids
    //@{
    Identifier          module_id(int side,
                                  int module) const;

    Identifier          channel_id(int side,
                                   int module,
                                   int type,
                                   int channel) const;
    /// From hash - optimized
    Identifier          module_id ( IdentifierHash module_hash ) const;
    Identifier          channel_id ( IdentifierHash channel_hash ) const;
    //@}


    /// @name interaction with id dictionary
    //@{
    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    /// Tests of packing
    void                test_packing    (void) const;
    //@}

private:

    typedef std::vector<Identifier>     id_vec;
    typedef id_vec::const_iterator      id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    int         initLevelsFromDict(void);

    int         init_hashes(void);

    size_type                   m_zdc_region_index;
    size_type                   m_FORWARD_INDEX;
    size_type                   m_ZDC_INDEX;
    size_type                   m_SIDE_INDEX;
    size_type                   m_MODULE_INDEX;
    size_type                   m_TYPE_INDEX;
    size_type                   m_CHANNEL_INDEX;
    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_module_range;
    MultiRange                  m_full_channel_range;
    size_type                   m_module_hash_max;
    size_type                   m_channel_hash_max;
    id_vec                      m_module_vec;
    id_vec                      m_channel_vec;

    IdDictFieldImplementation   m_forward_impl;
    IdDictFieldImplementation   m_zdc_impl;
    IdDictFieldImplementation   m_side_impl;
    IdDictFieldImplementation   m_module_impl;
    IdDictFieldImplementation   m_type_impl;
    IdDictFieldImplementation   m_channel_impl;

};

CLASS_DEF(ZdcID, 190591643, 1)

// inline definitions

inline
IdentifierHash      
ZdcID::module_hash      (Identifier module_id) const
{
    // module hash is composed of side and module
    static const unsigned int nmodule = m_module_impl.field().get_maximum() + 1;
    return (nmodule*m_side_impl.unpackToIndex(module_id) + module(module_id));
}

inline
IdentifierHash      
ZdcID::channel_hash      (Identifier channel_id) const
{
    // module hash is composed of side,  module, type and channel
    static const unsigned int nmodule  = m_module_impl.field().get_maximum() + 1;
    static const unsigned int ntype    = m_type_impl.field().get_maximum() + 1;
    static const unsigned int nchannel = m_channel_impl.field().get_maximum() + 1;
    return ((nmodule*ntype*nchannel)*m_side_impl.unpackToIndex(channel_id)
            + (ntype*nchannel)*module(channel_id) 
            + nchannel*type(channel_id) 
            + channel(channel_id));
}


inline
int ZdcID::side(const Identifier& id) const
{
    return (m_side_impl.unpack(id));
}

inline
int ZdcID::module(const Identifier& id) const
{
    return (m_module_impl.unpack(id));
}

inline
int ZdcID::type(const Identifier& id) const
{
    return (m_type_impl.unpack(id));
}

inline
int ZdcID::channel(const Identifier& id) const
{
    return (m_channel_impl.unpack(id));
}

inline
Identifier 
ZdcID::module_id(int side,
                 int module) const
{
    // Build identifier
    Identifier result(0);

    // Pack fields independently
    m_forward_impl.pack (forward_field_value(), result);
    m_zdc_impl.pack     (zdc_field_value(),     result);
    m_side_impl.pack    (side,                  result);
    m_module_impl.pack  (module,                result);
    return result;
}

inline
Identifier
ZdcID::module_id ( IdentifierHash module_hash ) const
{
    return (m_module_vec[module_hash]);
}

inline
Identifier 
ZdcID::channel_id(int side,
                  int module,
                  int type,
                  int channel) const
{
    // Build identifier
    Identifier result(0);

    // Pack fields independently
    m_forward_impl.pack (forward_field_value(), result);
    m_zdc_impl.pack     (zdc_field_value(),     result);
    m_side_impl.pack    (side,                  result);
    m_module_impl.pack  (module,                result);
    m_type_impl.pack    (type,                  result);
    m_channel_impl.pack (channel,               result);
    return result;
}

inline
Identifier
ZdcID::channel_id ( IdentifierHash channel_hash ) const
{
    return (m_channel_vec[channel_hash]);
}

#endif
