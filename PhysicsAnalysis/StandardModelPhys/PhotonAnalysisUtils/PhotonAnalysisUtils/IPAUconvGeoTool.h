//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// IPAUconvGeoTool.h
// author: Kerstin Tackmann, Thomas Koffas
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSYSUTILS_ICONVGEOTOOL_H
#define PHOTONANALYSYSUTILS_ICONVGEOTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "PhotonAnalysisUtils/PAUegamma.h"
#include "PhotonAnalysisUtils/PAUnamespace.h"

static const InterfaceID IID_IPAUconvGeoTool("IPAUconvGeoTool", 1, 0);

class IPAUconvGeoTool : virtual public IAlgTool {
  
public:

  /** Virtual destructor */
  virtual ~IPAUconvGeoTool(){};

  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUconvGeoTool; };
 
  // public methods:

  virtual const std::vector<float> convGeoVars(const PAU::egamma* g=0) const = 0; 
  virtual const std::vector<float> convGeoVars(const Analysis::Photon* g=0) const = 0; 
  virtual const std::vector<float> convGeoVars(const Trk::VxCandidate* vtx=0) const = 0; 

};

#endif // PHOTONANALYSYSUTILS_ICONVGEOTOOL_H
