/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_ParameterErrDecoratorTool_H
#define INDETPHYSVALMONITORING_ParameterErrDecoratorTool_H

/**
 * @file ParameterErrDecoratorTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 25 April 2014
 **/

#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>


// class to decorate xAOD::TrackParticles with errors on the defining parameters
class ParameterErrDecoratorTool: virtual public IInDetPhysValDecoratorTool, public AthAlgTool {
public:
  ParameterErrDecoratorTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual
  ~ParameterErrDecoratorTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual bool decorateTrack(const xAOD::TrackParticle& particle, const std::string& prefix);
private:
};

#endif
