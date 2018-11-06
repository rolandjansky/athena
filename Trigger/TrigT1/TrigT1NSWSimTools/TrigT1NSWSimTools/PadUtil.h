/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef PADUTIL_H
#define PADUTIL_H

#include "GeoPrimitives/GeoPrimitives.h"

// forward declarations
class MsgStream;
class TVcetor3;
class PadOfflineData;

namespace std {
  template <class X,class Y> class pair;
}

namespace NSWL1 {
  /// determine the pad ieta, iphi indices using the old algorithm
  /**
     check if the PAD id can be converted into the type used by the
     standalone PAD simulation developed for the TDR. Note that the
     input TVector3 is the the global psosition of the PAD. The
     resulting ieta,iphi values are stored in result. Return false
     when the indices cannot be determined.
  */
  bool determinePad(int layer,
                    int wedgeId,
                    int wedgeType,
                    int sector,
                    int detectorNumber,
                    TVector3 pos,
                    std::pair<int,int>& result,
                    MsgStream& msg);
  /// wraps determinePad
  bool determine_pad_indices_with_old_algo(const PadOfflineData& pod, const Amg::Vector3D &global_pos,
                                           std::pair<int,int>& result, MsgStream& msg);
} // end of namespace

#endif
