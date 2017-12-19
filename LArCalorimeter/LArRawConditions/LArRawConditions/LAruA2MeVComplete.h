/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARUA2MEVCOMPLETE_H
#define LARRAWCONDITIONS_LARUA2MEVCOMPLETE_H

#include "LArElecCalib/ILAruA2MeV.h" 
#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArSingleFloatP.h"

/** This class implements the ILAruA2MeV interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 16/01/04
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *
 */

class LAruA2MeVComplete: public ILAruA2MeV ,
	public LArConditionsContainer<LArSingleFloatP>{
 public: 
  typedef LArConditionsContainer<LArSingleFloatP> CONTAINER ;
  LAruA2MeVComplete();
  virtual ~LAruA2MeVComplete();
  
  // retrieving uA2MeV using online ID
  virtual const float& UA2MEV(const HWIdentifier&  chid) const ;

    // retrieving uA2MeV using offline ID
  virtual const float& UA2MEV(const Identifier&  id) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, const float vuA2MeV);
    
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LAruA2MeVComplete, 64178609,1)
CONDCONT_DEF( LAruA2MeVComplete, 59678833, ILAruA2MeV );
#endif 
