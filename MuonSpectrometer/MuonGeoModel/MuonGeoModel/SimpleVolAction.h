/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 My GeoVolumeAction: crosses a PhysVol to see the shape and materials
 --------------------------------------------------------------------
 ***************************************************************************/

#ifndef MUONGEOMODEL_SIMPLEVOLACTION_H
#define MUONGEOMODEL_SIMPLEVOLACTION_H

#include "GeoModelKernel/GeoVolumeAction.h"
class GeoVPhysVol;

namespace MuonGM {

    class SimpleVolAction : public GeoVolumeAction {
      public:
        // constructor
        SimpleVolAction();

        // desctructor
        ~SimpleVolAction();

        // Action routine
        void handleVPhysVol(const GeoVPhysVol *);
    };

} // namespace MuonGM

#endif // MUONGEOMODEL_SIMPLEVOLACTION_H
