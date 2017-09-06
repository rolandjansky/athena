/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Various flags - put here to avoid the need to include     //
//  other header files just for the flags                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VERTEXCOMMONFLAGS_H
#define VERTEXCOMMONFLAGS_H

#include <QFlags>
#include <QString>

class VertexCommonFlags {
public:

  enum ReconVertexTypeFlag {
    RVT_Primary    = 0x01,
    RVT_Secondary  = 0x02,
    RVT_V0         = 0x04,
    RVT_Conversion = 0x08,
    RVT_Kink       = 0x10,
    RVT_Pileup     = 0x20,
    RVT_Other      = 0x40,
    RVT_Any        = 0x7F
  };
  Q_DECLARE_FLAGS(ReconVertexTypeFlags, ReconVertexTypeFlag)

  enum QUANTITY { ENERGY, MOMENTUM, TRANSVERSE_MOM, MASS };
  static QString toString(const QUANTITY&);

private:
  VertexCommonFlags();
  ~VertexCommonFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VertexCommonFlags::ReconVertexTypeFlags)

#endif
