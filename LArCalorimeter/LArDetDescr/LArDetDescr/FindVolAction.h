/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FINDVOLACTION_H
#define FINDVOLACTION_H

#include "GeoModelKernel/GeoVolumeAction.h"

class FindVolAction :public GeoVolumeAction
{
 public:
    //constructor
    FindVolAction (PVConstLink startPV,
		   std::string volumeName,
		   int identifier);

    //desctructor
    ~FindVolAction ();

    // volume with this name exists
    bool volumeExists() const;

    // Returns the selected physical volume or NULL if not found
    PVConstLink getVolume () const;

    // Returns the global transformation to the volume
    HepGeom::Transform3D getGlobalTransform () const;
 private:

    bool                                  m_volumeExists;
    std::string                           m_volumeName;
    int                                   m_identifier;
    HepGeom::Transform3D                        m_globalTransform;
    PVConstLink                           m_volume;

    void handleVPhysVol (const GeoVPhysVol *);
    
};


#endif 
