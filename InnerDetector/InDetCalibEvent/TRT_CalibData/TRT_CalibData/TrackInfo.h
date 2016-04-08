/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBDATA__TRACKINFO_H
#define TRT_CALIBDATA__TRACKINFO_H
/********************************************************************

NAME:     TRT::Track
PACKAGE:  TRT_AlignData

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  "messenger"-class for track information used inside
          TRT Alignment

********************************************************************/

// INCLUDES:

#include <vector>
#include "TRT_CalibData/HitInfo.h"

namespace TRT {
  namespace Track{
    // Declare indices
    enum FloatVariables{
      d0 = 0,
      z0,
      phi,
      cotTheta,
      qOverPT,
      chiSquare,
      t0,
      TNOFV // always leave as last enum and DON*T change
    };
    // Declare names
    const std::string floatVariableName[TNOFV]={
      "d0",
      "z0",
      "phi",
      "cotTheta",
      "qOverPT",
      "chiSquare",
      "t0"
    };

    // Declare indices
    enum IntVariables{
      run = 0,
      event,
      trackNumber,
      numberOfPixelHits,
      numberOfSCTHits,
      numberOfTRTHits,
      degreesOfFreedom,
      TNOIV // always leave as last enum and DON*T change
    };
    // Declare names
    const std::string intVariableName[TNOIV]={
      "run",
      "event",
      "trackNumber",
      "numberOfPixelHits",
      "numberOfSCTHits",
      "numberOfTRTHits",
      "degreesOfFreedom"
    };
  }

  class TrackInfo : public std::vector<HitInfo*> {
  public:
    // Constructors
    TrackInfo() : std::vector<HitInfo*>(), m_Ints(Track::TNOIV),m_Floats(Track::TNOFV){}
    TrackInfo(std::vector<HitInfo*>& orig) : std::vector<HitInfo*>(orig), m_Ints(Track::TNOIV),m_Floats(Track::TNOFV){}
    TrackInfo(const TrackInfo& orig): std::vector<HitInfo*>(orig), m_Ints(orig.m_Ints),m_Floats(orig.m_Floats){}
    // Destructor
    ~TrackInfo(){for (std::vector<HitInfo*>::iterator i=this->begin();i!=this->end();++i) delete *i; } // We _OWN_ the hits!!!
    // Access
    const int& operator[](const Track::IntVariables& theIndex) const {return m_Ints[theIndex];}
    const float& operator[](const Track::FloatVariables& theIndex) const {return m_Floats[theIndex];}
    // Set/modify
    int& operator[](const Track::IntVariables& theIndex) {return m_Ints[theIndex];}
    float& operator[](const Track::FloatVariables& theIndex) {return m_Floats[theIndex];}

  private:
    std::vector<int> m_Ints;
    std::vector<float> m_Floats;
  };
}

#endif //TRT_CALIBDATA__TRACKINFO_H
