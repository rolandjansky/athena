/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCALIPULSEPARAMSCOMPLETE_H
#define LARRAWCONDITIONS_LARCALIPULSEPARAMSCOMPLETE_H

#include "LArElecCalib/ILArCaliPulseParams.h" 
#include "LArRawConditions/LArCaliPulseParamsP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArCaliPulseParams interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 21/07/05
 *
 * History:
 *  - 
 * Last update (november 2006)
 * the structure has been expanedd to host Offset and dTimeCal;
 * but the corresponding Nova class LArCaliPulseParamsDB has been kept as before
 */

class LArCaliPulseParamsComplete: public ILArCaliPulseParams,
	public LArConditionsContainer<LArCaliPulseParamsP>{

  
 public: 
  
  typedef LArConditionsContainer<LArCaliPulseParamsP> CONTAINER ;

  LArCaliPulseParamsComplete();
  
  virtual ~LArCaliPulseParamsComplete( );
  
  // retrieving CaliPulseParams using online ID
  virtual  const float& Tcal(const HWIdentifier&  CellID, int gain) const ;
  virtual  const float& Fstep(const HWIdentifier&  CellID, int gain) const ;
  virtual  const float& Offset(const HWIdentifier&  CellID, int gain) const;
  virtual  const float& dTimeCal(const HWIdentifier&  CellID, int gain) const;
  virtual  const short& nCB(const HWIdentifier&  CellID, int gain) const;
  
  // retrieving CaliPulseParams using offline ID
  virtual  const float& Tcal(const Identifier&  CellID, int gain) const;
  virtual  const float& Fstep(const Identifier&  CellID, int gain) const;
  virtual  const float& Offset(const Identifier&  CellID, int gain) const;
  virtual  const float& dTimeCal(const Identifier&  CellID, int gain) const;
  virtual  const short& nCB(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually
  // (if one wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	   float tcal, float fstep);
  
  void set(const HWIdentifier& CellID, int gain,
	   float tcal, float fstep, float offset, float dtcal);

  void set(const HWIdentifier& CellID, int gain,
	   float tcal, float fstep, float offset, float dtcal, short ncb);

};

CLASS_DEF( LArCaliPulseParamsComplete,228787406,1)
#endif 
