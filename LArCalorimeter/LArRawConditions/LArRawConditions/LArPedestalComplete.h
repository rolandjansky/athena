/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRAWCONDITIONS_LARPEDESTALCOMPLETE_H
#define LARRAWCONDITIONS_LARPEDESTALCOMPLETE_H

#include "LArElecCalib/ILArPedestal.h" 
#include "LArRawConditions/LArPedestalP1.h"
#include "LArRawConditions/LArConditionsContainer.h"

class LArCablingService ;

/** This class implements the ILArPedestal interface
 *
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 10/12/03
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */

class LArPedestalComplete: public ILArPedestal,
	public LArConditionsContainer<LArPedestalP1>
 {
  
 public: 
  
  LArPedestalComplete();
  
  //typedef LArConditionsContainer<LArPedestalP1> CONTAINER ;
  virtual ~LArPedestalComplete( );
  
  // retrieving Pedestal using online ID
  
  virtual float pedestal(const HWIdentifier&  CellID, int gain) const {
    return this->get(CellID, gain).m_Pedestal;}
  
  virtual float pedestalRMS(const HWIdentifier&  CellID, int gain) const {
    return this->get(CellID, gain).m_PedestalRMS;}
  
  virtual float pedestal(const Identifier&  CellID, int gain) const;
  
  virtual float pedestalRMS(const Identifier&  CellID, int gain) const;
  
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, const int gain,
	   const float vPedestal,
	   const float vPedestalRMS);

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArPedestalComplete, 134294113,1)
CONDCONT_DEF( LArPedestalComplete, 43865217, ILArPedestal );
#endif 
