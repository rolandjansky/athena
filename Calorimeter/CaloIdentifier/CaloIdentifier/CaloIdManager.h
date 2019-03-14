/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter Identifier package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloIdManager.h,v 1.4 2007-06-15 14:52:20 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALOID_CALOIDMANAGER_H
# define CALOID_CALOIDMANAGER_H

#include "AthenaKernel/CLASS_DEF.h"

class CaloCell_ID;
class CaloCell_SuperCell_ID;
class CaloDM_ID;
class CaloLVL1_ID;
class LArEM_ID;
class LArEM_SuperCell_ID;
class LArFCAL_ID;
class LArFCAL_SuperCell_ID;
class LArMiniFCAL_ID;
class LArHEC_ID;
class LArHEC_SuperCell_ID;
class TileID;
class Tile_SuperCell_ID;
class TTOnlineID;
class JTower_ID;
class GTower_ID;


/**
*  @brief This class initializes the Calo (LAr and Tile) offline identifiers.
*
*  For subdetector specific functionality (online identifiers and
*  cabling for example, or Regions), one should use the LArIdManager and TileIdManager
*
*/
class CaloIdManager
{

public:

    CaloIdManager(void);
    ~CaloIdManager(void);

    ///  Access to IdHelper
    const CaloCell_ID*          getCaloCell_ID           (void) const;
    const CaloDM_ID*            getDM_ID                 (void) const;
    const CaloLVL1_ID*          getLVL1_ID               (void) const;
    const LArEM_ID*             getEM_ID                 (void) const;
    const LArFCAL_ID*           getFCAL_ID               (void) const;
    const LArMiniFCAL_ID*       getMiniFCAL_ID           (void) const;
    const LArHEC_ID*            getHEC_ID                (void) const;
    const TileID*               getTileID                (void) const;
    const TTOnlineID*           getTTOnlineID            (void) const;
    const CaloCell_SuperCell_ID*getCaloCell_SuperCell_ID (void) const;
    const LArEM_SuperCell_ID*   getEM_SuperCell_ID       (void) const;
    const LArHEC_SuperCell_ID*  getHEC_SuperCell_ID      (void) const;
    const LArFCAL_SuperCell_ID* getFCAL_SuperCell_ID     (void) const;
    const Tile_SuperCell_ID*    getTile_SuperCell_ID     (void) const;
    const JTower_ID*            getJTower_ID             (void) const;
    const GTower_ID*            getGTower_ID             (void) const;

    void                        initialize               (void);
    bool                        isInitialized            (void) const;

    /// these methods are used within Id initialisation, general public
    /// should ignore them :

    void set_helper (const CaloCell_ID* idHelper);
    void set_helper (const CaloDM_ID* idHelper);
    void set_helper (const CaloLVL1_ID* idHelper);
    void set_helper (const LArEM_ID* idHelper);
    void set_helper (const LArFCAL_ID* idHelper);
    void set_helper (const LArMiniFCAL_ID* idHelper);
    void set_helper (const LArHEC_ID* idHelper);
    void set_helper (const TileID* idHelper);
    void set_helper (const TTOnlineID* idHelper);
    void set_helper (const CaloCell_SuperCell_ID* idHelper);
    void set_helper (const LArEM_SuperCell_ID* idHelper);
    void set_helper (const LArHEC_SuperCell_ID* idHelper);
    void set_helper (const LArFCAL_SuperCell_ID* idHelper);
    void set_helper (const Tile_SuperCell_ID* idHelper);
    void set_helper (const JTower_ID* idHelper);
    void set_helper (const GTower_ID* idHelper);

private:

    bool                   m_isInitialized;

    const CaloCell_ID*     m_cell_id;
    const CaloDM_ID*       m_dm_id;
    const CaloLVL1_ID*     m_lvl1_id;
    const LArEM_ID*        m_em_id;
    const LArFCAL_ID*      m_fcal_id;
    const LArMiniFCAL_ID*  m_minifcal_id;
    const LArHEC_ID*       m_hec_id;
    const TileID*          m_tile_id;
    const TTOnlineID*      m_ttonline_id;
    const CaloCell_SuperCell_ID*  m_cell_supercell_id;
    const LArEM_SuperCell_ID*  m_em_supercell_id;
    const LArHEC_SuperCell_ID*  m_hec_supercell_id;
    const LArFCAL_SuperCell_ID*  m_fcal_supercell_id;
    const Tile_SuperCell_ID*  m_tile_supercell_id;
    const JTower_ID*     m_jTower_id;
    const GTower_ID*     m_gTower_id;

};

CLASS_DEF( CaloIdManager , 125856940 , 1 )


#endif // CALOID_CALOIDMANAGER_H
