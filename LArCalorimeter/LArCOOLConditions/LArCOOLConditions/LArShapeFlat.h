/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARSHAPEFLAT_H
#define LARCOOLCONDITIONS_LARSHAPEFLAT_H

#include "LArElecCalib/ILArShape.h" 
#include "LArCOOLConditions/LArCondFlatBase.h"
#include "LArCOOLConditions/LArShapeBlob.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>

class LArCablingService;
class CondAttrListCollection;

class LArShapeFlat: public ILArShape, 
		    public LArShapeBlob,
		    public LArCondFlatBase {

 private:
  LArShapeFlat();

 public:   
  LArShapeFlat(const CondAttrListCollection* attrList);
  virtual ~LArShapeFlat( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

 
  // retrieving coefficients using online ID
  
  virtual ShapeRef_t Shape(const HWIdentifier&  CellID,
			   int gain,
			   int tbin=0,
			   int mode=0) const ;

  virtual ShapeRef_t ShapeDer(const HWIdentifier&  CellID,
			      int gain,
			      int tbin=0,
			      int mode=0) const ;
  
  // retrieving coefficients using offline ID
  
  virtual ShapeRef_t Shape(const Identifier&  CellID,
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

};  
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArShapeFlat , 192810633 , 1 )
CONDCONT_DEF( LArShapeFlat, 264124099, ILArShape );

#endif 
