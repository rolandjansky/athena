/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4EMECOptions_h_
#define _LArG4EMECOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the EMEC part of the Liquid Argon      //
// simulation. A constructor sets the default values.  This class is     //
// supposed to be exported to python, configured through python commands //
// then stored in storegate and ultimately writted to the data stream.   //
// Similar structures exist for the other subcomponents.                 //
//                                                                       //
//-----------------------------------------------------------------------//

// uncomment this to turn on runtime selection 
// of the method for LArWheelCalculator::parameterized_slant_angle
//#define LARWHEELCALCULATOR_PSA_DEVELOPMENT

#include <string>

class LArG4EMECOptions 
{
 public:

  enum ECORRTYPE {NONE, CHCOLL, GAPOLD, GAPADJ, GAP_E, GAP_S, GAP_SE, CHCOLL1};

  inline LArG4EMECOptions() :

    m_EMECECorrType(CHCOLL1),
    m_EMECGapPower(1.4),
    m_EMECChMap("v03"),
    m_EMECEsr(0.2*CLHEP::mm),
    m_EMECHVMap("v02"),
#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
    m_EMECFoldA("param"),   //"param" "table" "iter"
#endif
    m_EMECBirksLaw    (false),
    m_EMECBirksk      (0.0486),
    m_EMECHVEnable    (false){}

    //    m_EMECPhiRotation("off"),            // or  "g3"
    //    m_EMECSagging("off"),                // or "" or "a b c d"
    //    m_EMECInnerSlantParam("default"),    // or "" or "a b c d e"
    //    m_EMECOuterSlantParam("default") {}  //  -- "  ---

  void saveMe();
  void printMe();

  // Set methods
  inline void EMECECorrType(ECORRTYPE value) { m_EMECECorrType = value; }
  inline void EMECGapPower(double value)     { m_EMECGapPower  = value; }
  inline void EMECChMap(std::string value)   { m_EMECChMap     = value; }
  inline void EMECEsr(double value)          { m_EMECEsr       = value; }
  inline void EMECHVMap(std::string value)   { m_EMECHVMap     = value; }
#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
  inline void EMECFoldA(std::string value)   { m_EMECFoldA     = value; } 
#endif
  inline void EMECBirksLaw(bool value)       { m_EMECBirksLaw = value; }  
  inline void EMECBirksk(double value)       { m_EMECBirksk = value; }  
  inline void EMECHVEnable (bool value)      { m_EMECHVEnable = value; }
  
  //inline void EMECPhiRotation(std::string value)     {m_EMECPhiRotation     = value;}
  //inline void EMECSagging(std::string value)         {m_EMECSagging         = value;}
  //inline void EMECInnerSlantParam(std::string value) {m_EMECInnerSlantParam = value;}
  //inline void EMECOuterSlantParam(std::string value) {m_EMECOuterSlantParam = value;}

  // Get methods
  inline ECORRTYPE   EMECECorrType(void)   { return m_EMECECorrType; }
  inline double      EMECGapPower(void)    { return m_EMECGapPower; }
  inline std::string EMECChMap(void)       { return m_EMECChMap; }
  inline double      EMECEsr(void)         { return m_EMECEsr; }
  inline std::string EMECHVMap(void)       { return m_EMECHVMap; }
#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
  inline std::string EMECFoldA(void)       { return m_EMECFoldA; }
#endif
  inline bool        EMECBirksLaw(void) const      { return m_EMECBirksLaw; }
  inline double      EMECBirksk(void) const        { return m_EMECBirksk; }
  inline bool        EMECHVEnable(void) const      { return m_EMECHVEnable; }
  //inline std::string EMECPhiRotation(void)     { return m_EMECPhiRotation; }
  //inline std::string EMECSagging(void)         { return m_EMECSagging; }
  //inline std::string EMECInnerSlantParam(void) { return m_EMECInnerSlantParam;}
  //inline std::string EMECOuterSlantParam(void) { return m_EMECOuterSlantParam;}


 private:

  // Type of energy correction.
  ECORRTYPE m_EMECECorrType;

  // Gap adjustment power
  double m_EMECGapPower;

  // Field Map Version
  std::string m_EMECChMap;

  // Signal suppression
  double m_EMECEsr;

  // High Voltage file version
  std::string m_EMECHVMap;

  // method for folding angle calculation; 
  //    "table": based on interpolation of the official table(ref:ABS.YYY.00.DR.2)
  //    "param": based on parametrization
  //    "iter": some iterational technique
  // NOTE: this option has no effect unless
  //       LARWHEELCALCULATOR_PSA_DEVELOPMENT is defined in
  //       GeoSpecialShapes/LArWheelCalculator.h

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
  std::string m_EMECFoldA;
#endif
  // for G3 compatibility
  //std::string m_EMECPhiRotation;
  
  // Sagging of fans
  //std::string m_EMECSagging;

  //
  //std::string m_EMECInnerSlantParam;
  //std::string m_EMECOuterSlantParam;

  // Birks' law  
  bool m_EMECBirksLaw;

  // Birks' law, constant k
  double m_EMECBirksk;

  // HV imperfections
  bool m_EMECHVEnable;
  


};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(LArG4EMECOptions, 321345323, 1)
#endif

#endif
