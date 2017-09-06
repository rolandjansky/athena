/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GEOVISATTRIBUTES_H_
#define _GEOVISATTRIBUTES_H_

#include <string>
#include <QByteArray>
#include <QMap>
#include <QString>
class SoMaterial;

class VisAttributes {

 public:

  // Constructor:
  VisAttributes();
  //delete copy
  VisAttributes(const VisAttributes &) = delete;
  //delete assignment
  VisAttributes & operator=(const VisAttributes &)=delete;
  void init();//Call at end of constructor in derived classes.
  // Destructor:
  virtual ~VisAttributes();

  // Fetch a material
  SoMaterial *get(const std::string & name) const;

  //Override transparencies of materials (will multiply current transparencies):
  void overrideTransparencies(float transpfact);

  QByteArray getState(bool onlyChangedMaterials=true);
  void applyState(QByteArray);

 protected:

  // Add a material
  void add(const std::string & name, SoMaterial *);

 private:

  class Imp;
  Imp * d;


};


// A list of Vis attributes for Detectors:
class DetVisAttributes : public VisAttributes {
  public:
  DetVisAttributes();
};


// A list of Vis attributes for Materials:
class MatVisAttributes : public VisAttributes {
  public:
  MatVisAttributes();
};


// A list of Vis attributes for Volumes:
class VolVisAttributes : public VisAttributes {
  public:
  VolVisAttributes();
};

#endif
