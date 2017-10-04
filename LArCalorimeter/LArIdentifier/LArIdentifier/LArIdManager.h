/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr Calorimeter Identifier package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: LArIdManager.h,v 1.9 2007-06-14 16:11:51 lucotte Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef LARID_LARIDMANAGER_H
# define LARID_LARIDMANAGER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

#include <vector>
#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class LArOnlineID;
class LArHVLineID;
class LArElectrodeID;
class CaloCell_ID;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
* @brief This class provides the client interface for accessing the
*  detector description information common to LAr and Tile.
*
*  For sub-detector specific functionality (online identifiers and
*  cabling for example, or Regions), one should use the
*  LArIdManager and TileIdManager
*
*/
class LArIdManager
{

public:
    
    LArIdManager(void);
    ~LArIdManager(void);

    ///  Access to IdHelper
    const LArOnlineID*          getOnlineID      (void) const;
    const LArHVLineID*          getHVLineID      (void) const;
    const LArElectrodeID*       getLArElectrodeID(void) const;

    // -------- access online<->offline channel maps -------------

    typedef std::map<HWIdentifier, IdentifierHash > HWtoOffH_map;
    typedef HWtoOffH_map::const_iterator            HWtoOffH_const_iterator;
    typedef HWtoOffH_map::size_type                 HWtoOffH_size ;

    HWtoOffH_const_iterator  HWtoOffH_map_begin (void) const;
    HWtoOffH_const_iterator  HWtoOffH_map_end   (void) const;
    HWtoOffH_size            HWtoOffH_map_size  (void) const;

    typedef std::map<IdentifierHash, HWIdentifier >  OffHtoHW_map;
    typedef OffHtoHW_map::const_iterator             OffHtoHW_const_iterator ;
    typedef OffHtoHW_map::size_type                  OffHtoHW_size ;

    OffHtoHW_const_iterator OffHtoHW__map_begin (void) const;
    OffHtoHW_const_iterator OffHtoHW__map_end   (void) const;
    OffHtoHW_size           OffHtoHW__map_size  (void) const;

    HWIdentifier get_HWId (IdentifierHash  idcalohash) const;
    IdentifierHash get_calohash (HWIdentifier HWid ) const;

    /// these methods are used within Id initialisation, general public
    /// should ignore them :

    void                        initialize              (void);
    bool                        isInitialized           (void) const;

    void set_helper (const LArOnlineID* idHelper);
    void set_helper (const LArHVLineID* idHelper);
    void set_helper (const LArElectrodeID* idHelper);
    void set_helper (const CaloCell_ID* idHelper);

    // Ugly, ugly ...
    // this manager do not have the right to depend on the CablingSvc,
    // because of a dependency loop between LArIdentifier and LArTools
    // => maps are filled via the converter
    void add_to_maps (const IdentifierHash offlh, const HWIdentifier onlid);

private:

    bool                m_isInitialized;

    const LArOnlineID*  m_onl_id;
    const LArHVLineID*  m_larhv_id;
    const LArElectrodeID*  m_larelec_id;
    const CaloCell_ID*  m_cell_id;

    HWtoOffH_map m_HWid_to_calohash;
    OffHtoHW_map m_calohash_to_HWid;

};

CLASS_DEF( LArIdManager , 79554919 , 1 )

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // LARID_LARIDMANAGER_H
