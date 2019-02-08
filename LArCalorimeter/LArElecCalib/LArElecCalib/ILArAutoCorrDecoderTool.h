/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARELECCALIB_ILARAUTOCORRDECODERTOOL_H
#define LARELECCALIB_ILARAUTOCORRDECODERTOOL_H

#include "GaudiKernel/IAlgTool.h"
class Identifier;
class HWIdentifier;

#include <Eigen/Dense>

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_ILArAutoCorrDecoderTool("ILArAutoCorrDecoderTool", 1 , 0);

class ILArAutoCorrDecoderTool : virtual public IAlgTool  {

 public: 

  //online ID
  virtual const Eigen::MatrixXd AutoCorr( const HWIdentifier&  CellID, int gain, unsigned nSamples=5) const=0 ;

  // offline ID
  //virtual const Eigen::MatrixXd AutoCorr( const Identifier&  CellID, int gain, unsigned nSamples=5) const = 0 ;

  static const InterfaceID& interfaceID() { return IID_ILArAutoCorrDecoderTool; }

} ;

#endif 
