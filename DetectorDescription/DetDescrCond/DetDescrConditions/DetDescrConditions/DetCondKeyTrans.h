/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_KEYTRANS_H
#define DETDESCRCOND_KEYTRANS_H
// DetCondKeyTrans.h - class to hold set of HepGeom::Transform3D keyed by string
// value for storage in the conditions DB
// typically for holding top-level transforms for GeoModel/G4
// This class can be persistified (converters in DetDescrCondAthenaPool)
//
// Richard Hawkings, started 23/6/05

#include <string>
#include <map>
#include "CLHEP/Geometry/Transform3D.h"
#include "AthenaKernel/CLASS_DEF.h"

class DetCondKeyTrans {
 public:
  DetCondKeyTrans();
  virtual ~DetCondKeyTrans();

  // set a transform corresponding to a particular key value
  // if a transform with this key is already present, it is overwritten
  void setTransform(const std::string& key, const HepGeom::Transform3D& trans);

  // get the transform corresponding to this key into result
  // returns false if the transform cannot be found
  bool getTransform(const std::string& key, HepGeom::Transform3D& result) const;

  // check if transform corresponding to this key exists
  bool exists(const std::string& key) const;

  // print the set of key/transform pairs to stdout
  void print() const;

 private:
  typedef std::map<std::string,HepGeom::Transform3D> KeyTrans;
  KeyTrans m_keytrans;
};

CLASS_DEF(DetCondKeyTrans,254546453,1)

// iterator declaration for SEAL dictionary
// removed - perhaps will not be needed for reflex, causes problem for gcc344
// template std::map<std::string,HepGeom::Transform3D>::iterator;

// virtual destructor to satify requirement for persistent classes to have vtbl
inline DetCondKeyTrans::~DetCondKeyTrans() {}

inline void DetCondKeyTrans::setTransform(const std::string& key, 
    const HepGeom::Transform3D&trans) { m_keytrans[key]=trans;}

#endif // DETDESCRCOND_KEYTRANS_H
