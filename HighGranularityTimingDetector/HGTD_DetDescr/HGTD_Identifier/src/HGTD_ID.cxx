/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "HGTD_Identifier/HGTD_ID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"  
#include <set>
#include <algorithm>
#include <iostream>

// Constructor
HGTD_ID::HGTD_ID(void):
    m_ENDCAP_INDEX(0),
    m_DISK_INDEX(1),
    m_SIDE_INDEX(2),
    m_QUADRANT_INDEX(3),
    m_ROW_INDEX(4),
    m_MODULE_INDEX(5),
    m_PHI_INDEX_INDEX(6),
    m_ETA_INDEX_INDEX(7),
    m_dict(0),
    m_wafer_hash_max(0),
    m_pixel_hash_max(0) {

}

// Destructor
HGTD_ID::~HGTD_ID(void)
{}

void
HGTD_ID::wafer_id_checks ( int endcap,
                           int disk,
                           int side,
                           int quadrant,
                           int row,
                           int module ) const
{
    // TODO: actually write implementation, begin with the one from PixelID
}

void
HGTD_ID::pixel_id_checks ( int endcap,
                           int disk,
                           int side,
                           int quadrant,
                           int row,
                           int module,
                           int phi_index,
                           int eta_index) const
{
    // TODO: actually write implementation, begin with the one from PixelID
}

int
HGTD_ID::disk_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& disk_field = range[m_DISK_INDEX];
            if (disk_field.has_maximum()) {
                return (disk_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::side_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& side_field = range[m_SIDE_INDEX];
            if (side_field.has_maximum()) {
                return (side_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::quadrant_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& quadrant_field = range[m_QUADRANT_INDEX];
            if (quadrant_field.has_maximum()) {
                return (quadrant_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::row_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& row_field = range[m_ROW_INDEX];
            if (row_field.has_maximum()) {
                return (row_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::module_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& module_field = range[m_MODULE_INDEX];
            if (module_field.has_maximum()) {
                return (module_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::phi_index_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& phi_field = range[m_PHI_INDEX_INDEX];
            if (phi_field.has_maximum()) {
                return (phi_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int
HGTD_ID::eta_index_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& eta_field = range[m_ETA_INDEX_INDEX];
            if (eta_field.has_maximum()) {
                return (eta_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

bool
HGTD_ID::is_quadrant_max(const Identifier& id) const
{
    return (quadrant(id) == quadrant_max(id));
}

int
HGTD_ID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    // TODO: actually write implementation, begin with the one from PixelID
    return 0;
}

int
HGTD_ID::init_hashes(void)
{
    // TODO: actually write implementation, begin with the one from PixelID
    return 0;
}

int
HGTD_ID::initLevelsFromDict(void)
{
    // TODO: actually write implementation, begin with the one from PixelID
    return 0;
}

HGTD_ID::size_type
HGTD_ID::wafer_hash_max (void) const
{
    return m_wafer_hash_max;
}

HGTD_ID::size_type
HGTD_ID::pixel_hash_max (void) const
{
    return m_pixel_hash_max;
}

HGTD_ID::const_id_iterator      HGTD_ID::wafer_begin            (void) const
{
    return (m_wafer_vec.begin());
}

HGTD_ID::const_id_iterator      HGTD_ID::wafer_end              (void) const
{
    return (m_wafer_vec.end());
}

HGTD_ID::const_expanded_id_iterator     HGTD_ID::pixel_begin    (void) const
{
    return (m_full_pixel_range.factory_begin());
}

HGTD_ID::const_expanded_id_iterator     HGTD_ID::pixel_end      (void) const
{
    return (m_full_pixel_range.factory_end());
}

void
HGTD_ID::get_expanded_id        (const Identifier& id,
                                 ExpandedIdentifier& exp_id,
                                 const IdContext* context) const
{
    exp_id.clear();
    // // TODO: from PixelID:
    // exp_id << indet_field_value()
    //        << pixel_field_value()
    // // TODO: include hgtd_field_value()?
    // exp_id << hgtd_field_value()
    exp_id
           << endcap(id)
           << disk(id)
           << side(id)
           << quadrant(id)
           << row(id)
           << module(id);
    if(!context || context->end_index() == m_ETA_INDEX_INDEX) {
        exp_id << phi_index(id)
               << eta_index(id);
    }
}

// From hash get Identifier
int
HGTD_ID::get_id (const IdentifierHash& hash_id,
                 Identifier& id,
                 const IdContext* context) const
{
    int result = 1;

    // // TODO: adapt this code taken from PixelID

    // size_t begin = (context) ? context->begin_index(): 0;
    // // cannot get hash if end is 0:
    // size_t end   = (context) ? context->end_index()  : 0; 
    // if (0 == begin) { 
    //     // No hashes yet for ids with prefixes
    //     if (m_ETA_MODULE_INDEX == end) {
    //         if (hash_id < (unsigned int)(m_wafer_vec.end() - m_wafer_vec.begin())) {
    //             id = m_wafer_vec[hash_id];
    //             result = 0;
    //         }
    //     }
    //     else if (m_ETA_INDEX_INDEX == end) {
    //         // Do not know how to calculate pixel id from hash yet!!
    //         std::cout << "Do not know how to calculate pixel id from hash yet!!" << std::endl;
    //     }
    // }

    return (result);
}

int
HGTD_ID::get_hash       (const Identifier& id, 
                         IdentifierHash& hash_id,
                         const IdContext* context) const
{

    // Get the hash code from either a vec (for wafers) or calculate
    // it (pixels). For the former, we convert to compact and call
    // get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;

    // // TODO: adapt this code taken from PixelID

    // hash_id = 0;
    // size_t begin = (context) ? context->begin_index(): 0;
    // size_t end   = (context) ? context->end_index()  : 0; 
    // if (0 == begin) {
    //     // No hashes yet for ids with prefixes
    //     if (m_ETA_MODULE_INDEX  == end) {
    //         hash_id = wafer_hash(id);
    //         if (hash_id.is_valid()) result = 0;
    //     }
    //     else if (context && context->end_index() == m_ETA_INDEX_INDEX) {
    //         // Must calculate for pixel hash
    //         ExpandedIdentifier new_id;
    //         get_expanded_id(id, new_id, context);
    //         hash_id =  m_full_pixel_range.cardinalityUpTo(new_id);
    //         result = 0;
    //     }
    // }

    return (result);
}

void
HGTD_ID::test_wafer_packing     (void) const
{
    // TODO: actually write implementation, begin with the one from PixelID
}
