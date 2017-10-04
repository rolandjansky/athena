/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITSSONODEMANAGER_H
#define HITSSONODEMANAGER_H

//This class should as far as possible be used to create all shape
//nodes for hits. This allows for shared instancing and optimal memory
//and rendering performance.
//
//Author: Thomas Kittelmann.
#include "VP1Base/VP1HelperClassBase.h"
class SoNode;
class SoTransform;

class HitsSoNodeManager : public VP1HelperClassBase {
public:

  HitsSoNodeManager( IVP1System * sys = 0 );//sys for messages
  ~HitsSoNodeManager();

  SoNode* getShapeNode_Point();
  SoNode* getShapeNode_Cross( double extent );
  // SoNode* getShapeNode_Cross( double extent, double x, double y, double z );
  SoNode* getShapeNode_DriftDisc( double radius );//Af flat drift tube. Returns point if radius is 0
  SoNode* getShapeNode_DriftTube( double halfLength, double radius );//Will return an SoCylinder unless radius is 0 - in that case returns a line.
  SoNode* getShapeNode_ProjectedDriftTube( double halfLength, double radius, bool inner, bool outer );//Will return SoCylinder(s) unless radius is 0 - in that case returns point(s).
  SoNode* getShapeNode_Strip( double length, double width = 0, double depth=0 );//if width & depth are 0, will return a line. If all are 0, will return a point.
  SoNode* getShapeNode_Wire( double length, double minWidth = 0, double maxWidth=0, double depth=0 );
  SoNode* getShapeNode_Pad( double length, double minWidth = 0, double maxWidth=0, double depth=0 );

  SoTransform * getUnitTransform();
  
private:

  HitsSoNodeManager( const HitsSoNodeManager & );
  HitsSoNodeManager & operator= ( const HitsSoNodeManager & );

  class Imp;
  Imp * m_d;

};

#endif
