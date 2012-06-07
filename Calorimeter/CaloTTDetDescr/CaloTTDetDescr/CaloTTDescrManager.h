/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter LVL1 TT description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescrManager.h,v 1.9 2007-06-19 12:58:56 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALODETDESCR_CALOTTDESCRMANAGER_H
#define CALODETDESCR_CALOTTDESCRMANAGER_H

#include "Identifier/Identifier.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>
#include <map>

// Forward declarations
class CaloTTDescrRegion;
class CaloTTDescriptor;
class CaloLVL1_ID;

/**
*  @brief This class is used to build the CaloTTDescriptors and CaloTTDetDescrRegions
*  carrying the geometry information of the Calorimeter Trigger Towers. 
*
*  It is initialized by the CaloTTMgrDetDescrCnv converter from package CaloCnv/CaloDetMgrDetDescrCnv
*  
*  @warning The hadronic part of the FCAL calorimeter calorimeter is constructed
*   with 4 channels in eta. These 4 channels actually correspond to 2 channels in eta
*   FOR EACH of the hadronic samplings, i.e. FCAL2 and FCAL3. Therefore the geometry
*   of these channels is wrong.
*
*  Access to the DetDescrElements: <br>
*  ------------------------------- <br>
*<br>
*    Access is either individually using identifiers and a find
*    method, or one can iterate over the whole set. <p>
* <br>
*  The information provided: <br>
*  ------------------------- <br>
*<br>
*    Each DetDescrElement provides 
*    access to a detector descriptor object. <p>
*<br>
*  Access to CaloTTDescrManager: <br>
*  ----------------------------- <br>
*  <br>
*    This is NOT a singleton class anymore
*    one now has to access the manager from the detector store <p>
*<br>
*  Initialization of DetDescrElements: <br>
*  ----------------------------------- <br>
* <br>
*    Once an initialization has been performed, subsequent calls are
*    ignored. Also, the method: <p>
*<pre>
*      bool is_initialized() 
*
*    returns true. 
*</pre>
*
*
*/

class CaloTTDescrManager
{
  public:
    
    typedef std::vector <CaloTTDescrRegion*>		calo_region_vec;
    typedef calo_region_vec::size_type		        calo_region_vec_size;
    typedef calo_region_vec::const_iterator		calo_region_const_iterator;

    typedef std::vector <CaloTTDescriptor*>		calo_descr_vec;
    typedef calo_descr_vec::size_type		        calo_descr_size;
    typedef calo_descr_vec::const_iterator		calo_descr_const_iterator;

    CaloTTDescrManager(void);
    
    ~CaloTTDescrManager();
    

    /**  Access to the Det Descr region (==Trigger Tower) corresponding to a TT identifier */
    CaloTTDescrRegion*			find_calo_region     (const Identifier& tower_id) const;
    /**  Begin iterator on Det Descr regions (== Trigger Towers) */
    calo_region_const_iterator 		calo_region_begin    (void) const;
    /**  End iterator on Det Descr regions (== Trigger Towers) */
    calo_region_const_iterator 		calo_region_end	     (void) const;
    /**  Total number of Det Descr regions (i.e. Trigger Towers) */
    calo_region_vec_size                calo_region_size     (void) const;

    /**  Begin iterator on descriptors */
    calo_descr_const_iterator 		calo_descriptors_begin	(void) const;
    /**  End iterator on descriptors */
    calo_descr_const_iterator 		calo_descriptors_end 	(void) const;
    /**  Total number of descriptors */
    calo_descr_size 			calo_descriptors_size	(void) const;

    /**  access to the offline identifier helper */
    const CaloLVL1_ID*			get_lvl1_id		(void) const;

    void				print			(void) const;
    
    /** dummy. only sets is_initialized. actual initialization performed in CaloTTMgrDetDescrCnv */
    void                                initialize              (void);
    bool                                is_initialized          (void);

    /**  Insertion in the vector of Det Descr regions (== Trigger Towers) */
    void				add        	        (CaloTTDescrRegion*    region);
    /**  Insertion in the vector of descriptors */
    void				add          	        (CaloTTDescriptor*     descriptor);

    /** set the helper used to decode the TT offline identifiers*/
    void		      	set_helper (const CaloLVL1_ID* id_helper);

    
  private:
    
    
    /** undefined semantics */
    CaloTTDescrManager (const CaloTTDescrManager &);
    CaloTTDescrManager &operator= (const CaloTTDescrManager &);

    /** Delete towers and descriptors */
    void		clear(void);

    typedef std::map <Identifier, CaloTTDescrRegion* >	calo_region_map;

    bool		m_is_initialized;
    /** Helper used to decode the TT offline identifiers */
    const CaloLVL1_ID*	m_calo_lvl1_id;
    /** vector of Det Descr regions (== Trigger Towers) */
    calo_region_vec 	m_calo_region_vec;
    calo_region_map 	m_calo_region_map;
    /** vector of descriptors */
    calo_descr_vec 	m_calo_descr_vec;

};

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( CaloTTDescrManager , 117659265 , 1 )

#endif // CALODETDESCR_CALOTTDESCRMANAGER_H
