/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelDCSData.h
// 
// Class that handles Pixel DCS data
//
// 09-13-2007
//
// author Georges AAD <aad@cppm.in2p3.fr>
//
//****************************************************************************



#ifndef PIXEL_DSC_DATA_H
#define PIXEL_DSC_DATA_H


#include <string>
#include <iostream>
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/Identifier.h"



class PixelModuleDCSData;

std::ostream& operator<<(std::ostream& out, const PixelModuleDCSData&);

typedef DataVector<PixelModuleDCSData> PixelDCSData;

class PixelModuleDCSData{

 public:

  PixelModuleDCSData();
  explicit PixelModuleDCSData(const Identifier& ident);
  explicit PixelModuleDCSData(Identifier& ident);


  ~PixelModuleDCSData(){};

  void setModuleID(const Identifier& ident);
  void setModuleID(Identifier& ident);
  const Identifier& getModuleID() const;

  void setTemperature(float temp);  
  float getTemperature() const;

  void setHV(float hv);
  float getHV() const;

  void setFSMStatus(std::string fsms);
  std::string getFSMStatus() const;

  void setFSMState(std::string fsms);
  std::string getFSMState() const;


 private:

  Identifier m_ident;

  float m_temperature;
  float m_hightVoltage;
  std::string m_fsmStatus;
  std::string m_fsmState;

};


inline void PixelModuleDCSData::setModuleID(const Identifier& ident)
{
  m_ident = ident;
}

inline void PixelModuleDCSData::setModuleID(Identifier& ident)
{
  m_ident = ident;
}


inline const Identifier& PixelModuleDCSData::getModuleID() const
{
  return m_ident;
}

inline void PixelModuleDCSData::setTemperature(float temp)
{
  m_temperature = temp;
}

inline float PixelModuleDCSData::getTemperature() const
{
  return m_temperature;
}

inline void PixelModuleDCSData::setHV(float hv)
{
  m_hightVoltage = hv;
}

inline float PixelModuleDCSData::getHV() const
{
  return m_hightVoltage;
}

inline void PixelModuleDCSData::setFSMStatus(std::string fsms)
{
  m_fsmStatus = fsms;
}

inline std::string PixelModuleDCSData::getFSMStatus() const
{
  return m_fsmStatus;
}


inline void PixelModuleDCSData::setFSMState(std::string fsms)
{
  m_fsmState = fsms;
}

inline std::string PixelModuleDCSData::getFSMState() const
{
  return m_fsmState;
}

CLASS_DEF(PixelModuleDCSData, 88201164, 1) 
CLASS_DEF(PixelDCSData, 21396826, 1)

#endif // PIXEL_DSC_DATA_H
