/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrkAlignEvent/AlignResidualType.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

namespace Trk
{
  MsgStream& operator<< (MsgStream& log, Trk::AlignMesType type)
  {
    switch (type)
    {
      case Measurement:
        return log << " measurement      ";    
      case Scatterer:
        return log << " scatterer        ";
      case EnergyDeposit:
        return log << " energy deposit   ";
      default:
        return log << " unknown mes type ";
    }
  }

  MsgStream& operator<< (MsgStream& log, Trk::AlignResidualType type)
  {
    switch (type)
    {
      case HitOnly:
        return log << " HitOnly  ";    
      case Unbiased:
        return log << " Unbiased ";
//      case DCA:
//        return log << " DCA      ";
      default:
        return log << " unknown  ";
    }
  }

  ResidualPull::ResidualType ResidualPullType(AlignResidualType type)
  {
    // defines mapping between residual types for the alignment and those
    // used in the ResidualPullCalculator
    switch(type)
    {
      case HitOnly:
        return ResidualPull::HitOnly;
      case Unbiased:
//      case DCA:
        return ResidualPull::Unbiased;
      default:
        return ResidualPull::HitOnly;
    }
  }
}
