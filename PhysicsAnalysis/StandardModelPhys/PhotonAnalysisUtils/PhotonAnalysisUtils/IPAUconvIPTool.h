//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// IPAUconvIPTool.h
// author: F. Polci <francesco.polci@cern.ch>
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSYSUTILS_ICONVIPTOOL_H
#define PHOTONANALYSYSUTILS_ICONVIPTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPAUconvIPTool("IPAUconvIPTool", 1, 0);

class egamma;

class IPAUconvIPTool : virtual public IAlgTool {
  
public:

  /** Virtual destructor */
  virtual ~IPAUconvIPTool(){};

  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUconvIPTool; };
 
  // public methods:

  virtual const std::vector<float> convIP(const egamma* g) const = 0; 

};

#endif // PHOTONANALYSYSUTILS_ICONVIPTOOL_H
