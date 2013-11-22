/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETIDENTIFIER_XXX_SILICONID_H
#define INDETIDENTIFIER_XXX_SILICONID_H
/**
 * @file SiliconID.h
 *
 * @brief This is an Identifier helper class for both the Pixel and
 *  SCT subdetectors. This class defines identifier hashes which span
 *  BOTH the pixels and the scts.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: SiliconID.h,v 1.2 2005-06-22 14:39:49 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
//#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
//#include "IdDict/IdDictFieldImplementation.h"
#include "CLIDSvc/CLASS_DEF.h"


//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  @class SiliconID
 **  
 **  @brief This is an Identifier helper class for both the Pixel and
 **  SCT subdetectors. This class defines identifier hashes which span
 **  BOTH the pixels and the scts.
 **
 **  PixelID and SCT_ID id helpers should be used to create and decode
 **  identifiers for the two subdetectors. This class should be used
 **  to go between a ids and a hash which covers both subdetectors.
 ** 
 **/

class SiliconID : public AtlasDetectorID
{
public:
        
    /// @name public typedefs
    //@{
    typedef Identifier::size_type                       size_type; 
    //@}

    /// @name strutors
    //@{
    SiliconID(void);
    SiliconID(const PixelID* pixel_helper, const SCT_ID* sct_helper);
    ~SiliconID(void);
    //@}

    /// @name Optimized accessors: less optimal than PixelID/SCT_ID
    //@{
    /// Identifier from hash 
    Identifier       wafer_id        (IdentifierHash wafer_hash) const;

    /// wafer hash from id
    IdentifierHash   wafer_hash      (Identifier wafer_id) const;

    /// Hash table maximum sizes
    size_type        wafer_hash_max  (void) const;

    /// Test for barrel - generic, i.e. works for EITHER pixel or sct id
    bool             is_barrel       (const Identifier& id) const;  

    /// Test for pixle b-layer - generic, i.e. works for EITHER pixel or sct id
    bool             is_blayer       (const Identifier& id) const;  
    //@}

    /// Test whether hash is pixel or sct
    bool             is_hash_pixel   (IdentifierHash wafer_hash) const;

    /// Access Iterators over full set of ids. Wafer iterator is sorted
    ///Identifier          wafer_id        (unsigned int) const;


    /// @name methods from abstract interface - NOT IMPLEMENTED
    //@{
    /// Create compact id from hash id (return == 0 for OK)
    virtual int         get_id          (const IdentifierHash& hash_id,
                                         Identifier& id,
                                         const IdContext* context = 0) const;
    
    /// Create hash id from compact id (return == 0 for OK)
    virtual int         get_hash        (const Identifier& id, 
                                         IdentifierHash& hash_id,
                                         const IdContext* context = 0) const;

    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);
    //@}

    /// Tests of packing
    int                 test_wafer_hashes      (void) const;

private:
        
    size_type        m_wafer_hash_max;
    size_type        m_pixel_wafer_hash_max;
    const PixelID*   m_pixel_helper;
    const SCT_ID*    m_sct_helper;

};
    


/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(SiliconID, 129452393, 1)

//----------------------------------------------------------------------------
inline Identifier
SiliconID::wafer_id        (IdentifierHash wafer_hash) const
{
    // Identifier from hash 
    if (wafer_hash < m_pixel_wafer_hash_max) {
	// Pixel hash
	return (m_pixel_helper->wafer_id(wafer_hash));
    }
    else if (wafer_hash < m_wafer_hash_max) {
	// SCT hash
	return (m_sct_helper->wafer_id(wafer_hash - m_pixel_wafer_hash_max));
    }
    // return invalid Identifier
    Identifier result;
    return (result);
}

//----------------------------------------------------------------------------
inline IdentifierHash
SiliconID::wafer_hash      (Identifier wafer_id) const
{
    // wafer hash from id
    if (is_pixel(wafer_id)) {
	// Pixel id
	return (m_pixel_helper->wafer_hash(wafer_id));
    }
    else {
	// SCT hash + pixel hash max
	return (m_sct_helper->wafer_hash(wafer_id) + m_pixel_wafer_hash_max);
    }
}

//----------------------------------------------------------------------------
inline SiliconID::size_type
SiliconID::wafer_hash_max  (void) const
{
    // Hash table maximum sizes
    return (m_wafer_hash_max);
}


//----------------------------------------------------------------------------
inline bool
SiliconID::is_barrel       (const Identifier& id) const
{
    if (is_pixel(id)) {
	// Pixel id
	return (m_pixel_helper->is_barrel(id));
    }
    else {
	// SCT
	return (m_sct_helper->is_barrel(id));
    }
}


//----------------------------------------------------------------------------
    /// Test for pixle b-layer
inline bool
SiliconID::is_blayer       (const Identifier& id) const
{
    if (is_pixel(id)) {
	// Pixel id
	return (m_pixel_helper->is_blayer(id));
    }
    else {
	// SCT
	return (false);
    }
}

//----------------------------------------------------------------------------
/// Test of hash for pixel/sct
inline bool       
SiliconID::is_hash_pixel (IdentifierHash wafer_hash) const
{
    // Identifier from hash 
    if (wafer_hash < m_pixel_wafer_hash_max) {
	// Pixel hash
	return (true);
    }
    // SCT hash
    return (false);
}



#endif // INDETIDENTIFIER_SILICONID_H
