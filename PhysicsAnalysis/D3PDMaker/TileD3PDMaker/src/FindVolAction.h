/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   FindVolAction.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: GeoVolumeAction to retrieve global transformation coordinates.
 *
 * Created in February 2013.
 */

#ifndef FINDVOLACTION_H
#define FINDVOLACTION_H

#include "GeoModelKernel/GeoVolumeAction.h"

//=========================================
class FindVolAction:public GeoVolumeAction{
//=========================================

    public:
        // CONSTRUCTOR
        FindVolAction (PVConstLink startPV,std::string volumeName,int identifier);

        // DESTRUCTOR
        ~FindVolAction ();

        // VOLUME EXISTS
        bool volumeExists() const;

        // RETURNS PHYSICAL VOLUME (OR NULL)
        PVConstLink getVolume () const;

        // RETURN GLOBAL TRANSFORMATION TO VOLUME (OR NULL)
        HepGeom::Transform3D getGlobalTransform () const;

    private:
        bool                 m_volumeExists;
        std::string          m_volumeName;
        int                  m_identifier;
        HepGeom::Transform3D m_globalTransform;
        PVConstLink          m_volume;

        void handleVPhysVol (const GeoVPhysVol *);
};


#endif 
