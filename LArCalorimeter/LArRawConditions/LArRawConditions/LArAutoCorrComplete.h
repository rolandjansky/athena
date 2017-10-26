/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRAWCONDITIONS_LARAUTOCORRCOMPLETE_H
#define LARRAWCONDITIONS_LARAUTOCORRCOMPLETE_H

#include "LArElecCalib/ILArAutoCorr.h" 
#include "LArRawConditions/LArAutoCorrP1.h"
#include "LArRawConditions/LArConditionsContainer.h"

class LArCablingService ;

/** This class implements the ILArAutoCorr interface
 *
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 10/12/03
 * changes:
 *   - 08/01/2004, S. Laplace: float -> double
 *   - 08/02/2004, S. Laplace: new online ID and cablingSvc
 *
 *
 *   - July 15, 2005  Hong Ma
 *      use LArRampP. 
 */



class LArAutoCorrComplete: public ILArAutoCorr,
			   public LArConditionsContainer<LArAutoCorrP1>{
  
 public: 
  

  typedef LArAutoCorrP1                         LArCondObj;
  typedef LArConditionsContainer<LArCondObj>    CONTAINER ;
  LArAutoCorrComplete();
  
  virtual ~LArAutoCorrComplete( );
  
  // retrieving AutoCorr 
  
  virtual AutoCorrRef_t autoCorr(const HWIdentifier&  CellID, int gain) const ;

  virtual AutoCorrRef_t autoCorr(const Identifier&  CellID, int gain) const ;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	 const   std::vector<float>& vAutoCorr);
  
 protected:
  
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArAutoCorrComplete, 255786016,1)
CONDCONT_DEF( LArAutoCorrComplete, 122986158, ILArAutoCorr );

#endif 
