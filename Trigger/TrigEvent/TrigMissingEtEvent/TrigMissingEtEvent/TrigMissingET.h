/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigMissingET.h
// PACKAGE:  Trigger/TrigEvent/TrigMissingEtEvent
// 
// AUTHOR:   Rashid Djilkibaev (Diego Casadei)
// CREATED:  March, 2006       (auxiliary info added in March 2008)
// 
// Description: MET trigger slice persistent class.
//              Basic info: Ex, Ey, Ez, SumE, SumEt, and a status flag.
//              Auxiliary info: collection of TrigMissingEtComponent's
//
// ********************************************************************

#ifndef TRIGMISSINGETEVENT_TRIGMISSINGET_H 
#define TRIGMISSINGETEVENT_TRIGMISSINGET_H


#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigMissingEtEvent/TrigMissingEtComponent.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <string>
#include <map>
#include <math.h>

/** Class for LVL2/EF MissingET trigger */

class TrigMissingET{
  
 public:
/** default constructor */
  TrigMissingET();
/** constructor (input is number of auxiliary components; if n==0 is like default) */
  TrigMissingET(unsigned char n); //<! 
  TrigMissingET(std::vector< TrigMissingEtComponent >&& compVec);
/** destructor */
  ~TrigMissingET();

/** reset contents */
  void clear();

/** Ex component of missing energy vector */
  float ex()    const { return m_ex; }
/** Ey component of missing energy vector */
  float ey()    const { return m_ey; }
/** Ez component of missing energy vector */
  float ez()    const { return m_ez; }
/** sum of |Et| */
  float sumEt() const { return m_sum_et; }
/** sum of |E| */
  float sumE()  const { return m_sum_e; }
/** (modulus of) transverse energy */
  float et()    const { return sqrt(m_ex*m_ex+m_ey*m_ey); }
/** (modulus of) energy */
  float e()     const { return sqrt(m_ex*m_ex+m_ey*m_ey+m_ez*m_ez); }

/** event flag: 0 (default) means OK */
  int getFlag() const {return m_flag;}
/** RoI word */
  long RoIword() const { return m_roiWord; }

/** increment Ex component */
  void addEx(float ex) { m_ex += ex; }
/** increment Ey component */
  void addEy(float ey) { m_ey += ey; }
/** increment Ez component */
  void addEz(float ez) { m_ez += ez; }

/** increment sumEt */
  void addSumEt(float ets) { m_sum_et += ets; }
/** increment sumE */
  void addSumE(float sumE) { m_sum_e += sumE; }

/** set Ex component */
  void setEx(float ex) { m_ex = ex; }
/** set Ey component */
  void setEy(float ey) { m_ey = ey; }
/** set Ez component */
  void setEz(float ez) { m_ez = ez; }
/** set sum of |Et| */
  void setSumEt(float ets) { m_sum_et = ets; }
/** set sum of |E| */
  void setSumE(float sumE) { m_sum_e = sumE; }

/** set event flag */
  void setFlag(int flag) { m_flag=flag; }
/** set RoIword */
  void set_RoIword(long word) { m_roiWord=word; }

/** get number of auxiliary components */
  unsigned int getNumOfComponents() const {return m_compVec.size();}

/** get name of component `index' */
  std::string getNameOfComponent(unsigned int index) const;
/** get Ex from component `index' */
  float getExComponent(unsigned int index) const;
/** get Ey from component `index' */
  float getEyComponent(unsigned int index) const;
/** get Ez from component `index' */
  float getEzComponent(unsigned int index) const;
/** get sum of |Et| from component `index' */
  float getSumEtComponent(unsigned int index) const;
/** get sum of |E| from component `index' */
  float getSumEComponent(unsigned int index) const;
/** get constant term for calibration of component `index' */
  float getComponentCalib0(unsigned int index) const;
/** get scale for calibration of component `index' */
  float getComponentCalib1(unsigned int index) const;
/** get sum of energy signs for calibration of component `index' */
  short getSumOfSigns(unsigned int index) const;
/** get number of used channes in component `index' */
  unsigned short getUsedChannels(unsigned int index) const;
/** get status flag for component `index' */
  short getStatus(unsigned int index) const;

/** set name of component `index' */
  void setNameOfComponent(unsigned int index, const char* str);
/** set Ex for component `index' */
  void setExComponent(unsigned int index, float value);
/** set Ey for component `index' */
  void setEyComponent(unsigned int index, float value);
/** set Ez for component `index' */
  void setEzComponent(unsigned int index, float value);
/** set sum of |Et| for component `index' */
  void setSumEtComponent(unsigned int index, float value);
/** set sum of |E| for component `index' */
  void setSumEComponent(unsigned int index, float value);
/** set constant term for calibration of component `index' */
  void setComponentCalib0(unsigned int index, float value);
/** set scale for calibration of component `index' */
  void setComponentCalib1(unsigned int index, float value);
/** set sum of energy signs for calibration of component `index' */
  void setSumOfSigns(unsigned int index, short value);
/** set number of used channels in component `index' */
  void setUsedChannels(unsigned int index, unsigned short value);
/** set status flag of component `index' */
  void setStatus(unsigned int index, short value);

/** formatted output */
  friend MsgStream& operator<< (MsgStream& m, const TrigMissingET& d);


 private:

/** Basic information: Ex component */
  float m_ex;
/** Basic information: Ey component */
  float m_ey;
/** Basic information: Ez component */
  float m_ez;
/** Basic information: scalar sum of Et */
  float m_sum_et;
/** Basic information: scalar sum of E */
  float m_sum_e;
/** Basic information: event status flag: 0 (default) means OK */
  int   m_flag;
/** Basic information: RoIword */
  long  m_roiWord;

/** Auxiliary information:
    components from different calo layers and muons */
  std::vector< TrigMissingEtComponent > m_compVec;

};


/// Operator comparing two objects for equality
bool operator== (const TrigMissingET& a, const TrigMissingET& b);

/// Operator comparing two objects for inequality
inline bool operator!= (const TrigMissingET& a, const TrigMissingET& b) {
  return !( a == b );
}

/// Comparison with feedback
void diff(const TrigMissingET& a, const TrigMissingET& b,
	  std::map<std::string, double>& variableChange);

/// conversion to formatted string: multi-line!
std::string str (const TrigMissingET& a);



CLASS_DEF(TrigMissingET, 145610168, 1)
CLASS_DEF( DataVector<TrigMissingET> , 23170091 , 1 )

#endif
