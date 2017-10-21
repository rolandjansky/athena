/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AscObj_TruthPoint                   //
//                                                            //
//  Description: Asc. Obj. handle for each truth point on a   //
//               truth track.                                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef ASCOBJ_TRUTHPOINT_H
#define ASCOBJ_TRUTHPOINT_H

#include "VP1TrackSystems/AssociatedObjectHandleBase.h"

namespace HepMC { class GenVertex; class GenParticle; }
class SimHitHandleBase;

class AscObj_TruthPoint : public AssociatedObjectHandleBase {
public:

  AscObj_TruthPoint( TrackHandleBase*, const HepMC::GenVertex * v, const HepMC::GenParticle * p );
  AscObj_TruthPoint( TrackHandleBase*, SimHitHandleBase* );
  virtual ~AscObj_TruthPoint();

  void buildShapes(SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  QStringList clicked();

protected:
  int regionIndex() const;
  double lodCrossOverValue() const;

private:

  class Imp;
  Imp * m_d;

};

#endif
