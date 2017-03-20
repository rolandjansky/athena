/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMATERIAL2G4_ElementFactory_H
#define GEOMATERIAL2G4_ElementFactory_H

class G4Element;
class GeoElement;

#include <map>
#include <string>
typedef std::map<std::string, G4Element*, std::less<std::string> > elList;

class Geo2G4ElementFactory {
public:
  Geo2G4ElementFactory();
  G4Element* Build(const GeoElement*);
private:
  elList definedElements;
};

#endif
