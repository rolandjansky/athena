/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARELECCALIB_ILARAUTOCORR_H
#define LARELECCALIB_ILARAUTOCORR_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArVectorProxy.h"

/** This class defines the interface for accessing AutoCorrelation 
   * parameters for each channel 
   * @stereotype Interface
   * @author H. Ma 
   * @version  0-0-1 , 22/08/2002 
   *
   * Modifications:
   * W. Lampl, S. Laplace, 15/12/2003: 
   *        - Add method using online ID
   *        - Add 3rd argument isOffline
   * S. Laplace, 08/01/2004:
   *        - float -> double
   * S. Laplace, 08/02/2004:
   *        - new online ID
   */ 

class ILArAutoCorr {

 public: 

  typedef LArVectorProxy AutoCorrRef_t;

  virtual ~ILArAutoCorr() {};

  //online ID
  virtual AutoCorrRef_t autoCorr( const HWIdentifier&  CellID, int gain) const=0 ;

  // offline ID
  virtual AutoCorrRef_t autoCorr( const Identifier&  CellID, int gain) const = 0 ;
  
} ;

CLASS_DEF( ILArAutoCorr, 8124,1) 

#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArAutoCorr>,253577051 , 1 )



#endif 
