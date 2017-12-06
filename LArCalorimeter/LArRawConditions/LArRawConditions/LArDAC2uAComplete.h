/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDAC2UACOMPLETE_H
#define LARRAWCONDITIONS_LARDAC2UACOMPLETE_H

#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArSingleFloatP.h"

/** This class implements the ILArDAC2uA interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 16/01/04
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *  - 29/04/2004, S. Laplace: added ERRORCODE

 *   - July 16, 2005  Hong Ma
 *      use LArDAC2uAP. 

 */



class LArDAC2uAComplete: public ILArDAC2uA ,
 public LArConditionsContainer<LArSingleFloatP> {

 public: 
  
  typedef LArConditionsContainer<LArSingleFloatP> CONTAINER ;
  LArDAC2uAComplete();
  
  virtual ~LArDAC2uAComplete( );
  
  // retrieving DAC2uA using online ID
  
  virtual const float& DAC2UA(const HWIdentifier& chid) const ;

    // retrieving DAC2uA using offlineId
  virtual const float& DAC2UA(const Identifier& id) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, const float vDAC2uA);

};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArDAC2uAComplete, 95178965,1)
CONDCONT_DEF( LArDAC2uAComplete, 148060277, ILArDAC2uA );
#endif 
