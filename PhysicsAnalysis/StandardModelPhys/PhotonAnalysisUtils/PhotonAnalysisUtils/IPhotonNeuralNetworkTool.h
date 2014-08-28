// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/* 
 * @file PhotonNeuralNetworkTool.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date June, 2012
 */

#ifndef IPHOTONNEURALNETWORKTOOL_H
#define IPHOTONNEURALNETWORKTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Analysis{
  class Photon;
}

static const InterfaceID IID_IPhotonNeuralNetworkTool("IPhotonNeuralNetworkTool", 1, 0);

class IPhotonNeuralNetworkTool : virtual public IAlgTool {

 public:

  virtual ~IPhotonNeuralNetworkTool(){};
  
  static const InterfaceID& interfaceID() { return IID_IPhotonNeuralNetworkTool; }

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual bool Passes(const Analysis::Photon* ph) = 0;
  virtual float Discriminant(const Analysis::Photon* ph) = 0;
};


#endif
