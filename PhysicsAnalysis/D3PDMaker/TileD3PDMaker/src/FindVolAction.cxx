/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   FindVolAction.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: GeoVolumeAction to retrieve global transformation coordinates.
 *
 * Created in February 2013.
 */

#include "FindVolAction.h"

//=====================================================================================
FindVolAction::FindVolAction(PVConstLink startPV,std::string volumeName,int identifier)
    :GeoVolumeAction (GeoVolumeAction::TOP_DOWN),
    m_volumeExists(false),
    m_volumeName(volumeName),
    m_identifier(identifier),
    m_volume(0){
//=====================================================================================

    startPV->apply(this);
} // FindVolAction::FindVolAction

//==============================
FindVolAction::~FindVolAction(){
//==============================

} // FindVolAction::~FindVolAction()

//===========================================
PVConstLink FindVolAction::getVolume() const{
//===========================================
    return m_volume;
} // FindVolAction::getVolume

//=============================================================
HepGeom::Transform3D FindVolAction::getGlobalTransform() const{
//=============================================================
    return m_globalTransform;
} // FindVolAction::getGlobalTransform

//=======================================
bool FindVolAction::volumeExists() const{
//=======================================
    return m_volumeExists;
} // FindVolAction::volumeExists

//========================================================
void FindVolAction::handleVPhysVol(const GeoVPhysVol *pv){
//========================================================

    GeoTraversalState* state = getState();
    Query<int> Qint = state->getId();
    std::string pvName = state->getName();
    std::string lvName = pv->getLogVol()->getName();

    // USE IDENTIFIER IF IT EXISTS
    if( m_identifier >= 0 ){
        if( (pvName==m_volumeName || lvName==m_volumeName) &&
            Qint.isValid() && Qint == m_identifier){
            m_volumeExists = true;
            m_globalTransform = state->getAbsoluteTransform();
            m_volume = pv;
            terminate();
        } // IF
    } // IF
    else{
        if( (pvName==m_volumeName || lvName==m_volumeName) ) {
            //std::cout << " ***  FOUND! " << std::endl;
            m_volumeExists = true;
            m_globalTransform = state->getAbsoluteTransform();
            m_volume = pv;
            terminate();
        } // IF 
    } // ELSE
} // FindVolAction::handleVPhysVol


