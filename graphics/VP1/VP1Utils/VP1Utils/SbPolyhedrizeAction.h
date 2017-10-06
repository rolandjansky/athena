/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------//
//                                                                      //
// GeoPolyhedrizeAction.  For internal use by SoVisualizeAction         //
// Author: Joe Boudreau                                                 //
//                                                                      //
//----------------------------------------------------------------------//

#ifndef SbPolyhedrizeAction_h
#define SbPolyhedrizeAction_h 1
#include "GeoModelKernel/GeoShapeAction.h"
class SbPolyhedron;
class SbPolyhedrizeAction : public GeoShapeAction

{

  public:

      SbPolyhedrizeAction();

      virtual ~SbPolyhedrizeAction();

      virtual void handleShift(const GeoShapeShift *shift);

      virtual void handleUnion(const GeoShapeUnion *unio);

      virtual void handleIntersection(const GeoShapeIntersection *isect);

      virtual void handleSubtraction(const GeoShapeSubtraction *subtract);

      virtual void handleBox(const GeoBox *box);

      virtual void handleCons(const GeoCons *cons);

      virtual void handlePara(const GeoPara *para);

      virtual void handlePcon(const GeoPcon *pcon);

      virtual void handlePgon(const GeoPgon *pgon);

      virtual void handleTrap(const GeoTrap *trap);

      virtual void handleTrd(const GeoTrd *trd);

      virtual void handleTube(const GeoTube *tube);

      virtual void handleTubs(const GeoTubs *tubs);

      virtual void handleSimplePolygonBrep (const GeoSimplePolygonBrep *brep);

      virtual void handleTessellatedSolid (const GeoTessellatedSolid *tessellated);

      virtual void handleGenericTrap (const GeoGenericTrap *gentrap);
      
      const SbPolyhedron * getPolyhedron() const;


  private:

      SbPolyhedrizeAction(const SbPolyhedrizeAction &right);
      const SbPolyhedrizeAction & operator=(const SbPolyhedrizeAction &right);

      SbPolyhedron *m_polyhedron;

};






#endif


