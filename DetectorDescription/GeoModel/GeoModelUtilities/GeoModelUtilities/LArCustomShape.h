/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCustomShape_h
#define LArCustomShape_h

#include <string>

#include "GeoModelKernel/GeoShape.h"

// Forward declaration
class GeoShapeAction;

class LArCustomShape : public GeoShape
{
 public:

  // The custom shape has only one property: a string that contains
  // the name of the particular shape.  In the GeoModel->Geant4
  // conversion, this name will be matched against a list of custom
  // classes that inherit from G4VSolid to get the correct custom
  // solid.
  LArCustomShape(const std::string& shapeName);

  //	Returns the volume of the shape, for mass inventory
  virtual double volume() const;

  //	Returns the shape type, as a string.
  virtual const std::string& type() const;

  //	Returns the shape type, as a coded integer.
  virtual ShapeType typeID() const;

  //        For type identification.
  static const std::string& getClassType();

  //        For type identification.
  static const ShapeType getClassTypeID();

  // Return the shape name, supplied in the constructor.
  virtual const std::string& name() const;

  //	Executes a GeoShapeAction
  virtual void exec(GeoShapeAction* action) const;

 protected:
  virtual ~LArCustomShape();
  
 private:
  
  // Prohibited operations.
  LArCustomShape(const LArCustomShape &right);
  const LArCustomShape & operator=(const LArCustomShape &right);

  // General GeoModel shape attributes.
  static const std::string classType;
  static const ShapeType classTypeID;

  // Properties of the custom shape.
  const std::string m_shapeName;
};

inline const std::string& LArCustomShape::getClassType()
{
  return classType;
}

inline const ShapeType LArCustomShape::getClassTypeID()
{
  return classTypeID;
}

#endif 
