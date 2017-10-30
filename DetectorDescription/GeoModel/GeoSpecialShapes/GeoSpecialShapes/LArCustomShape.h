/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCustomShape_h
#define LArCustomShape_h

#include <string>
#include <map>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "GeoSpecialShapes/LArWheelCalculatorEnums.h"

// Forward declaration
class GeoShapeAction;

/// @class LArCustomShape
/// @todo NEEDS DOCUMENTATION
///
class LArCustomShape : public GeoShape
{

 public:

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  typedef std::pair<LArG4::LArWheelCalculator_t, int> CalcDef_t;
  typedef std::map<std::string, CalcDef_t> ShapeCalc_typemap;

  /// The custom shape has only one property: a string that contains
  /// the name of the particular shape.  In the GeoModel->Geant4
  /// conversion, this name will be matched against a list of custom
  /// classes that inherit from G4VSolid to get the correct custom
  /// solid.
  LArCustomShape(const std::string& shapeName);

  /// Return the calculator:
  const LArWheelCalculator *calculator() const;

  /// Returns the volume of the shape, for mass inventory
  virtual double volume() const;

  /// Returns the shape type, as a string.
  virtual const std::string& type() const;

  /// Returns the shape type, as a coded integer.
  virtual ShapeType typeID() const;

  /// For type identification.
  static const std::string& getClassType();

  /// For type identification.
  static ShapeType getClassTypeID();

  /// Return the shape name, supplied in the constructor.
  virtual const std::string& name() const;

  /// Executes a GeoShapeAction
  virtual void exec(GeoShapeAction* action) const;

  /** @brief The standard @c StoreGateSvc/DetectorStore
    * Returns (kind of) a pointer to the @c StoreGateSvc
    */
  StoreGateSvc_t& detStore() const;

 protected:

  virtual ~LArCustomShape();

 private:

  StatusCode createCalculator(const CalcDef_t & cdef);

  /// Prohibited operations.
  LArCustomShape(const LArCustomShape &right);
  const LArCustomShape & operator=(const LArCustomShape &right);

  /// General GeoModel shape attributes.
  static const std::string s_classType;
  static const ShapeType s_classTypeID;
  static const ShapeCalc_typemap s_calculatorTypes;

  /// Properties of the custom shape.
  const std::string m_shapeName;

  /// The calculator:
  const LArWheelCalculator *m_calculator;

  /// Pointer to StoreGate (detector store by default)
  mutable StoreGateSvc_t m_detStore;

};

inline const std::string& LArCustomShape::getClassType() {
  return s_classType;
}

inline ShapeType LArCustomShape::getClassTypeID() {
  return s_classTypeID;
}

inline ServiceHandle<StoreGateSvc>& LArCustomShape::detStore() const {
  return m_detStore;
}

#endif
