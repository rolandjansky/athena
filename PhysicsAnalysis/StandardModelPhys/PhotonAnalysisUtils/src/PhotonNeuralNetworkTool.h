// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/* 
 * @file IPhotonNeuralNetworkTool.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date June, 2012
 */

#ifndef PHOTONNEURALNETWORKTOOL_H
#define PHOTONNEURALNETWORKTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "PhotonAnalysisUtils/IPhotonNeuralNetworkTool.h"

class NeuralPhoton;

namespace eg2011{
  class EnergyRescaler;
}

class PhotonNeuralNetworkTool : public AthAlgTool, virtual public IPhotonNeuralNetworkTool {

 public:

  PhotonNeuralNetworkTool(const std::string& type,const std::string& name, const IInterface* parent);
  virtual ~PhotonNeuralNetworkTool();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual bool Passes(const Analysis::Photon* ph);
  virtual float Discriminant(const Analysis::Photon* ph);

 private:

  NeuralPhoton* m_neuralPhoton;

  eg2011::EnergyRescaler* m_eRescaler;

  std::string m_prefix;
  std::string m_weights;

  const Analysis::Photon* m_cachedPhoton;

  std::pair<float,bool> m_cachedResults;

  std::pair<float,bool> GetResults(const Analysis::Photon* ph);

};

#endif
