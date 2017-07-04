/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPSHAPE_H
#define DUMPSHAPE_H

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Convenience class used to dump the parameters of a GeoShape to a  //
// printable form (represented by a QStringList)                     //
//                                                                   //
// Original code by Joe Boudreau, adapted                            //
// in the present form by T. Kittelmann                              //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <QStringList>

class GeoShape;

class DumpShape {
public:

  static QStringList shapeToStringList(const GeoShape* shape);
private:

  DumpShape(){};
  ~DumpShape(){};

};

#endif
