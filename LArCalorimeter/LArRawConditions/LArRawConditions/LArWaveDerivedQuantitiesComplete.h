/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARWAVEDERIVEDQUANTITIESCOMPLETE_H
#define LARRAWCONDITIONS_LARWAVEDERIVEDQUANTITIESCOMPLETE_H

#include "LArElecCalib/ILArWaveDerivedQuantities.h" 
#include "LArRawConditions/LArWaveDerivedQuantitiesP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArWaveDerivedQuantities interface
 *
 * @author M. Delmastro
 * @version  0-0-1 , 28/11/08
 *
 * History:
 *  - 
 */

class LArWaveDerivedQuantitiesComplete: public ILArWaveDerivedQuantities ,
 public LArConditionsContainer<LArWaveDerivedQuantitiesP>
{
  
 public: 
  
  typedef LArConditionsContainer<LArWaveDerivedQuantitiesP> CONTAINER ;
  LArWaveDerivedQuantitiesComplete();
  
  virtual ~LArWaveDerivedQuantitiesComplete();
  
  // retrieving Wave Derived Quantities using online ID
  virtual const float& BaseLine(const HWIdentifier& CellID, int gain )  const;
  virtual const float& MaxAmp(const HWIdentifier& CellID, int gain )  const;
  virtual const float& TMaxAmp(const HWIdentifier& CellID, int gain )  const;
  virtual const float& Width(const HWIdentifier& CellID, int gain )  const;
  virtual const float& rT0(const HWIdentifier& CellID, int gain )  const;
  virtual const float& PosLobe(const HWIdentifier& CellID, int gain )  const;
  virtual const float& Jitter(const HWIdentifier& CellID, int gain )  const;
  virtual const unsigned& Flag(const HWIdentifier& CellID, int gain )  const;

  // retrieving WaveDerivedQuantities using offline ID
  virtual const float& BaseLine(const Identifier& CellID, int gain )  const;
  virtual const float& MaxAmp(const Identifier& CellID, int gain )  const;
  virtual const float& TMaxAmp(const Identifier& CellID, int gain )  const;
  virtual const float& Width(const Identifier& CellID, int gain )  const;
  virtual const float& rT0(const Identifier& CellID, int gain )  const;
  virtual const float& PosLobe(const Identifier& CellID, int gain )  const;
  virtual const float& Jitter(const Identifier& CellID, int gain )  const;
  virtual const unsigned& Flag(const Identifier& CellID, int gain )  const;

  // set method filling the data members individually
  void set(const HWIdentifier& CellID, int gain,
	   float baseline,
	   float maxAmp, 
	   float tmaxAmp, 
	   float width, 
	   float rT0, 
	   float posLobe,
	   float jitter,
	   unsigned flag);

};

CLASS_DEF( LArWaveDerivedQuantitiesComplete , 69648051 , 1 )

#endif 
