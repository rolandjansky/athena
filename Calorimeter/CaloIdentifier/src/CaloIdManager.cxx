/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//<doc><file>	$Id: CaloIdManager.cxx,v 1.3 2007-02-08 16:18:23 prieur Exp $
//<version>	$Name: not supported by cvs2svn $


// Calo includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/LArEM_SuperCell_ID.h"
#include "CaloIdentifier/LArHEC_SuperCell_ID.h"
#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"
#include "CaloIdentifier/Tile_SuperCell_ID.h"
#include "CaloIdentifier/JTower_ID.h"
#include "CaloIdentifier/GTower_ID.h"

// Athena/Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

CaloIdManager * CaloIdManager::s_instance = 0;

CaloIdManager::CaloIdManager(void)
    :
    m_isInitialized(false),
    m_cell_id(0),
    m_dm_id(0),
    m_lvl1_id(0),
    m_em_id(0),
    m_fcal_id(0),
    m_minifcal_id(0),
    m_hec_id(0),
    m_tile_id(0),
    m_ttonline_id(0),
    m_cell_supercell_id(0),
    m_em_supercell_id(0),
    m_hec_supercell_id(0),
    m_fcal_supercell_id(0),
    m_tile_supercell_id(0),
    m_jTower_id(0),
    m_gTower_id(0)
{
}

CaloIdManager::~CaloIdManager(void)
{
}


const CaloIdManager*
CaloIdManager::instance                (void)
{
    if (s_instance == 0) {

	// The following code can be removed once all useage the singleton has
	// been removed and replaced by accessing this object via the transient
	// detector store. Use the instance that's in the transient detector
	// store if it exists. Otherwise create a new instance, register it in
	// the store and return that. First, locate the StoreGate instance that's
	// managing the transient detector store.

	StoreGateSvc* detStore = 0;
	IMessageSvc*  msgSvc;
	const CaloIdManager* theMgr;

	ISvcLocator* svcLoc = Gaudi::svcLocator( );
	StatusCode status   = svcLoc->service( "MessageSvc", msgSvc );

	if ( status.isSuccess( ) ) {

	    MsgStream log(msgSvc, "CaloIdManager" );
	    status = svcLoc->service( "DetectorStore", detStore );

	    if ( status.isSuccess( ) ) {

		// Test whether the instance already exists in the transient
		// detector store
		if ( detStore->contains<CaloIdManager>("CaloIdManager") ) {

		    // The instance already exists - retrieve it and save it locally.

		    status     = detStore->retrieve( theMgr );
		    if ( status.isSuccess( ) ) {
		      s_instance = const_cast<CaloIdManager*>(theMgr);
		    }
		    else
		      {
			log << MSG::ERROR << " Failed to retrieve CaloIdManager " << endmsg;
		      }
		}

	    } else {
		log << MSG::ERROR << "Could not locate DetectorStore" << endmsg;
	    }

	} else {
	    std::cerr << "CaloIdManager: Could not locate the MessageSvc!!!"
		      << std::endl;
	}

	if (s_instance == 0) {
	    s_instance = new CaloIdManager;

	    // Register this instance in the transient detector store if it
	    // hasn't already been done.
	    if ( 0 != detStore ) {
		status = detStore->record( s_instance, "CaloIdManager" );
		if ( ! status.isSuccess( ) ) {
		  std::cerr << "CaloIdManager: Failed to record CaloIdManager "
		      << std::endl;
		}
	    }
	}

    }

    return s_instance;
}


const CaloCell_ID*
CaloIdManager::getCaloCell_ID           (void) const
{
    return (m_cell_id);
}

const CaloDM_ID*
CaloIdManager::getDM_ID                 (void) const
{
    return (m_dm_id);
}

const CaloLVL1_ID*
CaloIdManager::getLVL1_ID              (void) const
{
    return (m_lvl1_id);
}
const LArEM_ID*
CaloIdManager::getEM_ID                (void) const
{
    return (m_em_id);
}
const LArFCAL_ID*
CaloIdManager::getFCAL_ID              (void) const
{
    return (m_fcal_id);
}
const LArMiniFCAL_ID*
CaloIdManager::getMiniFCAL_ID          (void) const
{
    return (m_minifcal_id);
}
const LArHEC_ID*
CaloIdManager::getHEC_ID               (void) const
{
    return (m_hec_id);
}
const TileID*
CaloIdManager::getTileID               (void) const
{
    return (m_tile_id);
}
const TTOnlineID*
CaloIdManager::getTTOnlineID           (void) const
{
    return (m_ttonline_id);
}
const CaloCell_SuperCell_ID*
CaloIdManager::getCaloCell_SuperCell_ID(void) const
{
    return (m_cell_supercell_id);
}
const LArEM_SuperCell_ID*
CaloIdManager::getEM_SuperCell_ID      (void) const
{
    return (m_em_supercell_id);
}
const LArHEC_SuperCell_ID*
CaloIdManager::getHEC_SuperCell_ID      (void) const
{
    return (m_hec_supercell_id);
}
const LArFCAL_SuperCell_ID*
CaloIdManager::getFCAL_SuperCell_ID      (void) const
{
    return (m_fcal_supercell_id);
}
const Tile_SuperCell_ID*
CaloIdManager::getTile_SuperCell_ID      (void) const
{
    return (m_tile_supercell_id);
}
const JTower_ID*
CaloIdManager::getJTower_ID              (void) const
{
    return (m_jTower_id);
}
const GTower_ID*
CaloIdManager::getGTower_ID              (void) const
{
    return (m_gTower_id);
}


void
CaloIdManager::initialize              (void)
{
  m_isInitialized = true;
}

bool
CaloIdManager::isInitialized           (void) const
{
    return (m_isInitialized);
}


void
CaloIdManager::set_helper              (const CaloCell_ID*     idHelper)
{
    m_cell_id = idHelper;
}

void
CaloIdManager::set_helper              (const CaloDM_ID* idHelper)
{
    m_dm_id = idHelper;
}

void
CaloIdManager::set_helper              (const CaloLVL1_ID*     idHelper)
{
    m_lvl1_id = idHelper;
}

void
CaloIdManager::set_helper              (const LArEM_ID* idHelper)
{
    m_em_id = idHelper;
}

void
CaloIdManager::set_helper              (const LArFCAL_ID*     idHelper)
{
    m_fcal_id = idHelper;
}

void
CaloIdManager::set_helper              (const LArMiniFCAL_ID*     idHelper)
{
    m_minifcal_id = idHelper;
}

void
CaloIdManager::set_helper              (const LArHEC_ID* idHelper)
{
    m_hec_id = idHelper;
}


void
CaloIdManager::set_helper              (const TileID*     idHelper)
{
    m_tile_id = idHelper;
}
void

CaloIdManager::set_helper              (const TTOnlineID*    idHelper)
{
    m_ttonline_id = idHelper;
}

void
CaloIdManager::set_helper              (const CaloCell_SuperCell_ID* idHelper)
{
    m_cell_supercell_id = idHelper;
}


void
CaloIdManager::set_helper              (const LArEM_SuperCell_ID* idHelper)
{
    m_em_supercell_id = idHelper;
}


void
CaloIdManager::set_helper              (const LArHEC_SuperCell_ID* idHelper)
{
    m_hec_supercell_id = idHelper;
}


void
CaloIdManager::set_helper              (const LArFCAL_SuperCell_ID* idHelper)
{
    m_fcal_supercell_id = idHelper;
}


void
CaloIdManager::set_helper              (const Tile_SuperCell_ID* idHelper)
{
    m_tile_supercell_id = idHelper;
}

void
CaloIdManager::set_helper              (const JTower_ID*     idHelper)
{
    m_jTower_id = idHelper;
}


void
CaloIdManager::set_helper              (const GTower_ID*     idHelper)
{
    m_gTower_id = idHelper;
}
