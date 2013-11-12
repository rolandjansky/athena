/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_ABSMATERIALMANAGER_H
#define GEOMODELINTERFACES_ABSMATERIALMANAGER_H
//---------------------------------------------------------//
//                                                         //
// class AbsMaterialManager  abstract base class for a     //
// material manager.                                       //
//                                                         //
// Joe Boudreau March 2003                                 //
//                                                         //
//---------------------------------------------------------//
#include <string>
#include <iostream>
#include <iosfwd>

class GeoMaterial;
class GeoElement;
class AbsMaterialManager {

 public:

  // Constructor:
  AbsMaterialManager() {};
  
  // Destructor:
  virtual ~AbsMaterialManager() {};

  // Query the material:
  virtual       GeoMaterial *getMaterial(const std::string & name)       =0;
  virtual const GeoMaterial *getMaterial(const std::string & name) const =0;

  // Query the elements:
  virtual       GeoElement *getElement(const std::string & name)       = 0;
  virtual const GeoElement *getElement(const std::string & name) const = 0;

  // Query the elements (by atomic number):
  virtual       GeoElement *getElement(unsigned int atomicNumber)       = 0;
  virtual const GeoElement *getElement(unsigned int atomicNumber) const = 0;

  virtual std::ostream & printAll(std::ostream & o=std::cout) const = 0;


};


#endif
