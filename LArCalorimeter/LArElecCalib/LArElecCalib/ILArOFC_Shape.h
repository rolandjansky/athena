/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILAROFC_SHAPE_H
#define LARELECCALIB_ILAROFC_SHAPE_H

#include "Identifier/Identifier.h" 
#include "CLIDSvc/CLASS_DEF.h" 

#include <vector> 

class ILArOFC_Shape {
/** This class defines the interface for accessing Optimal Filtering 
   * Coefficients and Pulse Shape  for each channel
   * @stereotype Interface
   * @author H. Ma
   * @version  0-0-1 , 22/08/2002 
   */


public: 
  /** access to Energy OFC 
	index by Identifier, and gain setting
	tbin is only for test beam, the time bin
   */ 

  virtual ~ILArOFC_Shape() {};

  virtual const std::vector<float>& OFC_a(const Identifier& id, 
				   int gain, int tbin) const =0 ;

  /** access to Time OFC 
  */
  virtual const std::vector<float>& OFC_b(const Identifier& id, 
				   int gain, int tbin) const =0 ;

  /** Pulse shape at each sampling
	index by Identifier, and gain setting
	ibin is only for test beam, the time bin
   */ 
  virtual const std::vector<float>& pulseShape(const Identifier& id,
				   int gain,int tbin) const =0 ;
  /** derivative of Pulse shape at each sampling 
  */ 
  virtual const std::vector<float>& pulseShapeDer(const Identifier& id,
				       int gain,int tbin) const =0 ;


} ;

CLASS_DEF( ILArOFC_Shape ,8120,1) 

#endif 
