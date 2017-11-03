/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPECOMPLETE_H
#define LARRAWCONDITIONS_LARSHAPECOMPLETE_H

#include "LArElecCalib/ILArShape.h" 
#include "LArRawConditions/LArShapeP2.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

class LArCablingService ;

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
                              int mode=0) const ;
  virtual ShapeRef_t ShapeDer(const HWIdentifier&  CellID,
                              int gain,
                              int tbin=0,
                              int mode=0) const ;
  
  // retrieving Shape using offline ID ('mode' is still here, but ignored)
  virtual ShapeRef_t Shape   (const Identifier&  CellID,
                              int gain,
                              int tbin=0,
                              int mode=0) const;
  virtual ShapeRef_t ShapeDer(const Identifier&  CellID,
                              int gain,
                              int tbin=0,
                              int mode=0) const;

  // retrieving time offset using online/offline ID
  virtual  float timeOffset(const Identifier&  CellID, int gain) const;
  virtual  float timeOffset(const HWIdentifier&  CellID, int gain) const;

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const;
  virtual unsigned nTimeBins(const Identifier&  CellID, int gain) const;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const;
  virtual float timeBinWidth(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)  
  void set(const HWIdentifier& CellID, int gain,
	   const std::vector<std::vector<float> >& vShape,
	   const std::vector<std::vector<float> >& vShapeDer,
	   float timeOffset=0, float timeBinWidth=25./24.);

  //Overload the const-get method of the underlying LArConditionsContainer
  //for speed improvment
  CONTAINER::ConstReference get(const HWIdentifier id, int gain) const;

 protected: 

 private: 
  static const std::vector<float> m_empty;

  //In a typical application of this class, the methods Shape, ShapeDer, timeBinWidht, etc., 
  //are called consecutivly for the same cell and gain. Therefore it makes sense to 
  //cache a pointer to the LArShapeP1 object.
  //Used by tthe overloaded const-get method
  struct cache_t {
    HWIdentifier id;
    int gain;
    CONTAINER::ConstReference obj;
  };
  mutable cache_t m_cache;

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShapeComplete, 249350685, 1 )
CONDCONT_DEF( LArShapeComplete, 55610575, ILArShape );

inline
LArShapeComplete::CONTAINER::ConstReference
LArShapeComplete::get(const HWIdentifier id, int gain) const {  
  if (m_cache.id!=id || m_cache.gain!=gain) {
    m_cache.obj=LArConditionsContainer<LArShapeP2>::get(id,gain);
    m_cache.id=id;
    m_cache.gain=gain;
  }
  return m_cache.obj;
}

#endif 
