/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------//
//                                                                      //
// SoVisualizeAction.  For internal use only.                           //
// Author: Joe Boudreau                                                 //
//                                                                      //
//----------------------------------------------------------------------//

#ifndef SOVISUALIZEACTION_H
#define SOVISUALIZEACTION_H 1

#include "GeoModelKernel/GeoShapeAction.h"

class SoShape;

class SoVisualizeAction : public GeoShapeAction {

public:

  SoVisualizeAction();

  virtual ~SoVisualizeAction();

  virtual void handleShape(const GeoShape *subtract);

  virtual void handleBox(const GeoBox *box);

  virtual void handleCons(const GeoCons *cons);

  virtual void handlePcon(const GeoPcon *pcon);

  virtual void handleTrap(const GeoTrap *trap);

  virtual void handleTrd(const GeoTrd *trd);

  virtual void handleTube(const GeoTube *tube);

  virtual void handleTubs(const GeoTubs *tubs);

  virtual void handleLArCustom(const LArCustomShape *);

  virtual void handleSimplePolygonBrep(const GeoSimplePolygonBrep *);

  virtual void handleTessellatedSolid (const GeoTessellatedSolid *);

  virtual void handleGenericTrap (const GeoGenericTrap *);

  SoShape * getShape() { return m_shape; }
  void reset() { m_shape = 0; }

private:

  SoVisualizeAction(const SoVisualizeAction &);
  const SoVisualizeAction & operator=(const SoVisualizeAction &);

  SoShape *m_shape;
};






#endif


