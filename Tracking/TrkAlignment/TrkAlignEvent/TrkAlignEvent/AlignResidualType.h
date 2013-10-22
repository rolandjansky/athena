/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNRESIDUALTYPE_H
#define TRKALIGNEVENT_ALIGNRESIDUALTYPE_H

#include "TrkEventPrimitives/ResidualPull.h"

/**
   @file AlignResidualType.h
   @class AlignResidualType

   @brief AlignResidualType is an enum describing the type of residual
   AlignMesType is an enum describing the type of TSOS for the residual

   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 8/29/08
*/

class MsgStream;

namespace Trk
{
  enum AlignResidualType {
    HitOnly,
    Unbiased
//    DCA
  };

  enum AlignMesType {
    Measurement,
    Scatterer,
    EnergyDeposit
  };

  MsgStream& operator<< (MsgStream& log, AlignMesType type);
  MsgStream& operator<< (MsgStream& log, AlignResidualType type);

  ResidualPull::ResidualType ResidualPullType(AlignResidualType type);
}
#endif // TRKALIGNEVENT_ALIGNRESIDUALTYPE_H
