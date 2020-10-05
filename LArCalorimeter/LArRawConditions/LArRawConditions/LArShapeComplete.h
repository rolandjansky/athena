/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPECOMPLETE_H
#define LARRAWCONDITIONS_LARSHAPECOMPLETE_H

#include "LArElecCalib/ILArShape.h" 
#include "LArRawConditions/LArShapeP2.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArShape interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 29/01/04
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *
 */

class LArShapeComplete: public ILArShape,
	public LArConditionsContainer<LArShapeP2>
 {
  
 public: 
  
  typedef ILArShape::ShapeRef_t ShapeRef_t;
  typedef LArConditionsContainer<LArShapeP2> CONTAINER ;

  LArShapeComplete();
  
  virtual ~LArShapeComplete( );
  
  // retrieving Shape using online ID ('mode' is still here, but ignored)
  virtual ShapeRef_t Shape   (const HWIdentifier&  CellID,
                              int gain,
                              int tbin=0,
                              int mode=0) const override;
  virtual ShapeRef_t ShapeDer(const HWIdentifier&  CellID,
                              int gain,
                              int tbin=0,
                              int mode=0) const override;
  

  // retrieving time offset using onlineID
  virtual  float timeOffset(const HWIdentifier&  CellID, int gain) const;

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)  
  void set(const HWIdentifier& CellID, int gain,
	   const std::vector<std::vector<float> >& vShape,
	   const std::vector<std::vector<float> >& vShapeDer,
	   float timeOffset=0, float timeBinWidth=25./24.);

 protected: 

 private: 
  static const std::vector<float> m_empty;

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShapeComplete, 249350685, 1 )
CONDCONT_DEF( LArShapeComplete, 55610575, ILArShape );


#endif 
