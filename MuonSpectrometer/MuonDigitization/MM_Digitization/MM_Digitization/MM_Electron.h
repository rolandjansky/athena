/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_ELECTRON_H
#define MM_DIGITIZATION_MM_ELECTRON_H
//
// MM_Electron.cxx
//     Primary electron objects that are diffused, propagated and avalanched
//

#include <memory>

#include "TF1.h"
#include "TRandom3.h"
#include "TVector2.h"

class MM_Electron {

  TVector2 m_initialPosition;  
  TVector2 m_offsetPosition;
  
  float m_time;
  float m_charge;      

 public:

  std::unique_ptr<TF1> m_PolyaFunction;
  std::unique_ptr<TF1> m_LongitudinalDiffusionFunction;
  std::unique_ptr<TF1> m_TransverseDiffusionFunction;

  MM_Electron();  
  MM_Electron(float x, float y);  
  MM_Electron(const MM_Electron& other);
  
  void diffuseElectron(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm);    
  void setOffsetPosition(float x, float y);
  void propagateElectron(float driftVelx, float driftVely, float driftVel);
  void avalancheElectron(float gain, TRandom3* rndm);
  void setTime(float Time);
  void setCharge(float Charge);

  float getCharge() const;
  float getTime() const;
  float getX() const;
  float getInitialX() const;
  float getInitialY() const;
  
   
};

#endif
