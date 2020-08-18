/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBDATA__HITINFO_H
#define TRT_CALIBDATA__HITINFO_H
/********************************************************************

NAME:     TRT::Hit
PACKAGE:  TRT_AlignData

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  "messenger"-class for hit information used inside
          TRT Alignment

********************************************************************/

// INCLUDES:

#include <vector>
#include "InDetIdentifier/TRT_ID.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <CLHEP/Matrix/Matrix.h>

namespace InDet
{
  class TRT_DriftCircleOnTrack ;
}

namespace TRT{
  namespace Hit {
    // Declare indices
    enum FloatVariables{
      globalR = 0,
      globalPhi,
      globalZ,
      signedDriftRadius,
      errorSignedDriftRadius,
      t0,
      driftTime,
      TimeoverThreshold,
      HTLevel,
      positionOnWire,
      errorPositionOnWire,
      trackDriftRadius,
      errorTrackDriftRadius,
      driftVelocity,
      trackDriftTime,
      trackT0,
      TNOFV // always leave as last enum and DON*T change
    };
    // Declare names
    const std::string floatVariableName[TNOFV]={
      "globalR",
      "globalPhi",
      "globalZ",
      "signedDriftRadius",
      "errorSignedDriftRadius",
      "t0",
      "driftTime",
      "HTLevel",
      "TimeoverThreshold",
      "positionOnWire",
      "errorPositionOnWire",
      "trackDriftRadius",
      "errorTrackDriftRadius",
      "driftVelocity",
      "trackDriftTime",
      "trackT0"
    };

    // Declare indices
    enum IntVariables{
      ident = 0,
      detector,
      layer,
      phiModule,
      strawLayer,
      straw,
      side,
      driftTimeStatus,      
      TNOIV // always leave as last enum and DON*T change
    };
    // Declare names
    const std::string intVariableName[TNOIV]={
      "ident",
      "detector",
      "layer",
      "phiModule",
      "strawLayer",
      "straw",
      "side",
      "driftTimeStatus"
    };
  }

  class HitInfo {
  public:
    // Constructors
    HitInfo() : m_Ints(Hit::TNOIV),m_Floats(Hit::TNOFV) {}
    HitInfo(const HitInfo& orig)= default;
    HitInfo(HitInfo&& orig) noexcept = default;
    //assignment
    HitInfo & operator=(const HitInfo & other)=default;
    HitInfo & operator=(HitInfo && other) noexcept =default;
    // Destructor
    ~HitInfo()=default;
    // Access
    const int& operator[](const Hit::IntVariables& theIndex) const {return m_Ints[theIndex];}
    const float& operator[](const Hit::FloatVariables& theIndex) const {return m_Floats[theIndex];}
    // Set/modify
    int& operator[](const Hit::IntVariables& theIndex) {return m_Ints[theIndex];}
    float& operator[](const Hit::FloatVariables& theIndex) {return m_Floats[theIndex];}

    // methods
    float driftTime() const    { return operator[](Hit::driftTime) - operator[](Hit::t0); }
//    float driftTime() const    { return operator[](Hit::driftTime) - operator[](Hit::t0) - operator[](Hit::trackT0); }
    float driftRadius() const  { return operator[](Hit::signedDriftRadius) ; }
    float rawTime() const      { return operator[](Hit::driftTime) ;}
    float timeResidual() const { return driftTime() - trkDriftTime() ; }
    float residual() const     { return operator[](Hit::signedDriftRadius) - operator[](Hit::trackDriftRadius) ; }
    float trkVariance() const  { float tmp = operator[](Hit::errorTrackDriftRadius) ; return tmp*tmp ; }
    float trkDistance() const  { return operator[](Hit::trackDriftRadius) ; }
    float trkDriftTime() const { return operator[](Hit::trackDriftTime) ; }
    float trkT0() const { return operator[](Hit::trackT0) ; }
    float driftVelocity() const { return operator[](Hit::driftVelocity) ; }
    float residualVariance() const { 
    float tmp = operator[](Hit::errorSignedDriftRadius); return trkVariance() + tmp*tmp ; }
    float residualPull() const { return residual()/sqrt(residualVariance()) ; }
    float chiSquare() const { double tmp = residual() ; return tmp*tmp/residualVariance() ; }
    float phiResidual() const { return residual()/operator[](Hit::globalR) ; }
    float t0() const { return operator[](Hit::t0) ; }

    Identifier strawId(const TRT_ID& trtid) const { 
      // this method should not be inligned because it introduces a dependency
      return trtid.straw_id(operator[](TRT::Hit::detector), operator[](TRT::Hit::phiModule),
			    operator[](TRT::Hit::layer),operator[](TRT::Hit::strawLayer),operator[](TRT::Hit::straw)); }   
    
    Identifier moduleId(const TRT_ID& trtid) const { 
      // this method should not be inligned because it introduces a dependency
      return trtid.module_id(operator[](TRT::Hit::detector), operator[](TRT::Hit::phiModule),operator[](TRT::Hit::layer)) ; }
      
    Identifier ident() const { return Identifier(operator[](TRT::Hit::ident)) ; }

    bool hasValidDriftTime() const { return operator[](TRT::Hit::driftTimeStatus) ; }
    float HighLevelThreshold() const { return operator[](Hit::HTLevel) ; }
    float timeOverThreshold() const { return operator[](Hit::TimeoverThreshold) ; }
  private:
    std::vector<int> m_Ints;
    std::vector<float> m_Floats;
  };
}

#endif //TRT_CALIBDATA__HITINFO_H
