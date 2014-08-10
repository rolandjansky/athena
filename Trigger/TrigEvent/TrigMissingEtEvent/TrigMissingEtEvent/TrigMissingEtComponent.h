/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigMissingEtComponent.h
// PACKAGE:  Trigger/TrigEvent/TrigCaloEvent
// 
// AUTHOR:   Diego Casadei
// CREATED:  April, 2008
// 
// Description: MET trigger slice components (auxiliary information).
//
// ********************************************************************

#ifndef TRIGCALOEVENT_TRIGMISSINGETCOMPONENT_H 
#define TRIGCALOEVENT_TRIGMISSINGETCOMPONENT_H

#include "CLIDSvc/CLASS_DEF.h"
#include <string>

/** MET trigger slice components
 ** (auxiliary information for TrigMissingET)
 **/

class TrigMissingEtComponent {

 public:

/** constructor */
  TrigMissingEtComponent();
/** destructor */
  ~TrigMissingEtComponent();
/** clean values and calibration constants */
  void reset();
/** use this to print the header of the table printed by the following method */
  std::string getFormattedHeader();
/** print the (1-line) table of all attributes */
  std::string getFormattedValues();

  // Read access
/** get component name */
  std::string name()            const {return m_name;}
/** get status flag */
  short status()                const {return m_status;}
/** get Ex */
  float ex()                    const {return m_ex;}
/** get Ey */
  float ey()                    const {return m_ey;}
/** get Ez */
  float ez()                    const {return m_ez;}
/** get sum of |Et| */
  float sumEt()                 const {return m_sumEt;}
/** get sum of |E| */
  float sumE()                  const {return m_sumE;}
/** get constant term, for calibration */
  float calib0()                const {return m_calib0;}
/** get scale, for calibration */
  float calib1()                const {return m_calib1;}
/** get sum of energy signs, for calibration */
  short sumOfSigns()            const {return m_sumOfSigns;}
/** get used channels */
  unsigned short usedChannels() const {return m_usedChannels;}

  // Write access
/** set component name */
  void setName(const char* s)            {m_name = s;};
/** set status flag */
  void setStatus(short v)                {m_status = v;}
/** set Ex */
  void setEx(float v)                    {m_ex = v;}
/** set Ey */
  void setEy(float v)                    {m_ey = v;}
/** set Ez */
  void setEz(float v)                    {m_ez = v;}
/** set sum of |Et| */
  void setSumEt(float v)                 {m_sumEt = v;}
/** set sum of |E| */
  void setSumE(float v)                  {m_sumE = v;}
/** set constant term, for calibration */
  void setCalib0(float v)                {m_calib0 = v;}
/** set scale, for calibration */
  void setCalib1(float v)                {m_calib1 = v;}
/** set sum of energy signs, for calibration */
  void setSumOfSigns(short v)            {m_sumOfSigns = v;}
/** set number of used channels */
  void setUsedChannels(unsigned short v) {m_usedChannels = v;}

 private:
/** name */
  std::string m_name;
/** status flag */
  short m_status;  //<! default 0 = OK
/** Ey */
  float m_ex;
/** Ey */
  float m_ey;
/** Ez */
  float m_ez;
/** sum of |Et| */
  float m_sumEt;
/** sum of |E| */
  float m_sumE;
/** constant term, for calibration (default 0) */
  float m_calib0;
/** scale, for calibration (default 1) */
  float m_calib1;
/** calibration: sum of energy signs (-1, +1) */
  short m_sumOfSigns;
/** number of used channels */
  unsigned short m_usedChannels;

};


/// Operator comparing two objects for equality
bool operator== ( const TrigMissingEtComponent& a, const TrigMissingEtComponent& b );

/// Operator comparing two objects for inequality
inline bool operator!= ( const TrigMissingEtComponent& a, const TrigMissingEtComponent& b ) {
   return !( a == b );
}


CLASS_DEF( TrigMissingEtComponent , 129642584 , 1 )

#endif
