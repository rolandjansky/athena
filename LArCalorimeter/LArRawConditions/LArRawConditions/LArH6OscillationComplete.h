/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARH6OSCILLATIONCOMPLETE_H
#define LARRAWCONDITIONS_LARH6OSCILLATIONCOMPLETE_H

#include "LArElecCalib/ILArH6Oscillation.h" 
#include "LArRawConditions/LArH6OscillationP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArH6Oscillation interface
 *
 * @author T. Barillari
 * @version  \$Id: LArH6OscillationComplete.h,v 1.3 2006-02-26 15:56:41 schaffer Exp $*/



class LArH6OscillationComplete: public ILArH6Oscillation ,
	public LArConditionsContainer<LArH6OscillationP>
{
  
 public: 
  
  typedef LArConditionsContainer<LArH6OscillationP> CONTAINER ;

  LArH6OscillationComplete();
  
  virtual ~LArH6OscillationComplete( );
  
  // retrieving channel phases using online ID
  
  virtual  const double& channelPhase(const HWIdentifier&  CellID) const ;
  
  virtual  const double& channelAmplitude(const HWIdentifier&  CellID) const ;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, 
	   double channelPhase,
	   double channelAmplitude);
  

 private:

};

CLASS_DEF( LArH6OscillationComplete, 106937553,1)
#endif 
