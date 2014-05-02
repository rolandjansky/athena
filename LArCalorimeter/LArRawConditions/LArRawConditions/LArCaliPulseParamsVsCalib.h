/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCALIPULSEPARAMSVSCALIB_H
#define LARRAWCONDITIONS_LARCALIPULSEPARAMSVSCALIB_H

#include "LArElecCalib/ILArCaliPulseParams.h" 
#include "LArRawConditions/LArCaliPulseParamsP.h"
#include "LArRawConditions/LArConditionsVsCalib.h"

/** This class implements the ILArCaliPulseParams interface
 *
 * @author M. Fanti
 * @version  0-0-1 ,  7/02/06
 *
 * History:
 *  - 
 */

class LArCaliPulseParamsVsCalib: public ILArCaliPulseParams,
 public LArConditionsVsCalib<LArCaliPulseParamsP>
{
  
 public:

  LArCaliPulseParamsVsCalib() ;
  ~LArCaliPulseParamsVsCalib() ;

  // gain parameter not used! it doesn't make sense for calibration channels
  // it is accepted as parameter for compatibility with other interfaces
  // but can be omitted

  // retrieving CaliPulseParams using online ID
  virtual  const float& Tcal (const HWIdentifier&  ChID, int /* gain */) const 
    { return (get(ChID)).m_Tcal ; } ;
  virtual  const float& Fstep(const HWIdentifier&  ChID, int /* gain */) const 
    { return (get(ChID)).m_Fstep ; } ;
  virtual  const float& Offset(const HWIdentifier&  ChID, int /* gain */) const 
    { return (get(ChID)).m_Offset ; } ;
  virtual  const float& dTimeCal(const HWIdentifier&  ChID, int /* gain */) const 
    { return (get(ChID)).m_dTimeCal ; } ;
  virtual  const short& nCB(const HWIdentifier&  ChID, int /* gain */) const 
    { return (get(ChID)).m_nCB ; } ;
  
  // retrieving CaliPulseParams using offline ID
  virtual  const float& Tcal (const Identifier&  CellID, int /* gain */) const 
    { return (get(CellID)).m_Tcal ; } ;
  virtual  const float& Fstep(const Identifier&  CellID, int /* gain */) const 
    { return (get(CellID)).m_Fstep ; } ;
  virtual  const float& Offset(const Identifier&  CellID, int /* gain */) const 
    { return (get(CellID)).m_Offset ; } ;
  virtual  const float& dTimeCal(const Identifier&  CellID, int /* gain */) const 
    { return (get(CellID)).m_dTimeCal ; } ;
  const short& nCB(const Identifier&  CellID, int /* gain */) const 
    { return (get(CellID)).m_nCB ; } ;

  // set method filling the data members individually
  void set(const HWIdentifier& ChID, float tcal, float fstep);
  void set(const HWIdentifier& ChID, float tcal, float fstep, float offset, float dtcal);
  void set(const HWIdentifier& ChID, float tcal, float fstep, float offset, float dtcal, short ncb);
  void set(const HWIdentifier& ChID, int /* gain */, float tcal, float fstep) 
    { set(ChID,tcal,fstep) ; } ;
  void set(const HWIdentifier& ChID, int /* gain */, float tcal, float fstep, float offset, float dtcal) 
    { set(ChID,tcal,fstep,offset,dtcal) ; } ;
  void set(const HWIdentifier& ChID, int /* gain */, float tcal, float fstep, float offset, float dtcal, short ncb) 
    { set(ChID,tcal,fstep,offset,dtcal,ncb) ; } ;
  
} ;

CLASS_DEF( LArCaliPulseParamsVsCalib,115714668,1)

#endif

