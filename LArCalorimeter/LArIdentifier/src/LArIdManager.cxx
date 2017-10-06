/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LArrimeter identifier package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: LArIdManager.cxx,v 1.8 2007-03-05 09:14:07 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

// LAr includes
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "CaloIdentifier/CaloCell_ID.h"

// Athena/Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

LArIdManager::LArIdManager(void)
    :
    m_isInitialized(false),
    m_onl_id(0),
    m_larhv_id(0),
    m_larelec_id(0),
    m_cell_id(0)
{
}

LArIdManager::~LArIdManager(void)
{
}

const LArOnlineID*  LArIdManager::getOnlineID(void) const
{
    return (m_onl_id);
}
const LArHVLineID*  LArIdManager::getHVLineID(void) const
{
    return (m_larhv_id);
}
const LArElectrodeID*  LArIdManager::getLArElectrodeID(void) const
{
    return (m_larelec_id);
}


/// ------ access to online<->offline channel map -------------

LArIdManager::HWtoOffH_const_iterator  			
LArIdManager::HWtoOffH_map_begin (void) const 
{
    return m_HWid_to_calohash.begin();
}

LArIdManager::HWtoOffH_const_iterator  			
LArIdManager::HWtoOffH_map_end   (void) const 
{
  return m_HWid_to_calohash.end();
}

LArIdManager::HWtoOffH_size            			
LArIdManager::HWtoOffH_map_size  (void) const 
{
    return m_HWid_to_calohash.size();
}

LArIdManager::OffHtoHW_const_iterator   			
LArIdManager::OffHtoHW__map_begin (void) const 
{
    return m_calohash_to_HWid.begin();
}

LArIdManager::OffHtoHW_const_iterator   			
LArIdManager::OffHtoHW__map_end   (void) const 
{
  return m_calohash_to_HWid.end();
}

LArIdManager::OffHtoHW_size   			
LArIdManager::OffHtoHW__map_size  (void) const 
{
    return m_calohash_to_HWid.size();
}

HWIdentifier                                 
LArIdManager::get_HWId (IdentifierHash  idcalohash) const
{
    return (*m_calohash_to_HWid.find(idcalohash) ).second;
}

IdentifierHash                                 
LArIdManager::get_calohash (HWIdentifier HWid ) const
{
    return (*m_HWid_to_calohash.find(HWid) ).second;
}
    
void                                
LArIdManager::initialize              (void)
{
  m_isInitialized = true;
}

bool                                
LArIdManager::isInitialized           (void) const
{
    return (m_isInitialized);
}

void LArIdManager::set_helper(const LArOnlineID*     idHelper)
{
    m_onl_id = idHelper;
}

void LArIdManager::set_helper(const CaloCell_ID*     idHelper)
{
    m_cell_id = idHelper;
}

void LArIdManager::set_helper(const LArHVLineID*     idHelper)
{
    m_larhv_id = idHelper;
}

void LArIdManager::set_helper(const LArElectrodeID*     idHelper)
{
    m_larelec_id = idHelper;
}

void LArIdManager::add_to_maps (const IdentifierHash offlh, const HWIdentifier onlid)
{
      m_calohash_to_HWid[offlh] = onlid;
      m_HWid_to_calohash[onlid] = offlh;
}
