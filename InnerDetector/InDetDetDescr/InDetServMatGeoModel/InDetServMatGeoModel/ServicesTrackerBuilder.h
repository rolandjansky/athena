/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesTrackerBuilder_H
#define ServicesTrackerBuilder_H

class ServicesTracker;
class InDetServMatGeometryManager;

class ServicesTrackerBuilder{
public:

  ServicesTracker* buildGeometry(const InDetServMatGeometryManager& geoMgr) const;

};

#endif
