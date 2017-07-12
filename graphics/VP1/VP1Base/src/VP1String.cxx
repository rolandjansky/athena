/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1String                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1String.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Interval.h"
#include <QColor>
#include <Inventor/SbVec2d.h>
#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec3d.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec3s.h>
#include <Inventor/SbVec4d.h>
#include <Inventor/SbVec4f.h>
#include <sstream>

//____________________________________________________________________
QString VP1String::str( const QColor& c )
{
  return c.isValid() ? c.name() : "Invalid";
}

//____________________________________________________________________
QString VP1String::str( const SbColor& c )
{
  return str(VP1QtInventorUtils::sbcol2qcol(c));
}

//____________________________________________________________________
QString VP1String::str( const VP1Interval& i )
{
  return i.toString();
}

//____________________________________________________________________
QString VP1String::str(const void * p)
{
  if (p) {
    std::ostringstream s;
    s << p;
    return s.str().c_str();
  } else {
    return "NULL";
  }
}

//____________________________________________________________________
QString VP1String::str( const SbVec2d& v ) { double x,y; v.getValue(x,y); return "("+str(x)+", "+str(y)+")"; }
QString VP1String::str( const SbVec2f& v ) { float  x,y; v.getValue(x,y); return "("+str(x)+", "+str(y)+")"; }
QString VP1String::str( const SbVec2s& v ) { short  x,y; v.getValue(x,y); return "("+str(x)+", "+str(y)+")"; }
QString VP1String::str( const SbVec3d& v ) { double x,y,z; v.getValue(x,y,z); return "("+str(x)+", "+str(y)+", "+str(z)+")"; }
QString VP1String::str( const SbVec3f& v ) { float  x,y,z; v.getValue(x,y,z); return "("+str(x)+", "+str(y)+", "+str(z)+")"; }
QString VP1String::str( const SbVec3s& v ) { short  x,y,z; v.getValue(x,y,z); return "("+str(x)+", "+str(y)+", "+str(z)+")"; }
QString VP1String::str( const SbVec4d& v ) { double x,y,z,t; v.getValue(x,y,z,t); return "("+str(x)+", "+str(y)+", "+str(z)+", "+str(t)+")"; }
QString VP1String::str( const SbVec4f& v ) { float  x,y,z,t; v.getValue(x,y,z,t); return "("+str(x)+", "+str(y)+", "+str(z)+", "+str(t)+")"; }

