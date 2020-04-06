/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_INDETDD_Defs_H
#define INDETREADOUTGEOMETRY_INDETDD_Defs_H


namespace InDetDD {
  enum FrameType {local, global, other};
  enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // INDETREADOUTGEOMETRY_INDETDD_DEFS_H
