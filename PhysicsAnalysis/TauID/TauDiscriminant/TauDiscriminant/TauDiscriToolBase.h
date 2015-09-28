/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDiscriToolBase.h
// package:     PhysicsAnalysis/TauID/TauDiscriminant
// authors:     M. Wolter, A. Kaczmarska, L. Hauswald
// date:        13 March 2008
//-----------------------------------------------------------------------------

#ifndef DISCRITOOLBASE_TAU_H
#define DISCRITOOLBASE_TAU_H

#include "AsgTools/AsgTool.h"
#include "xAODTau/TauJet.h"
#include "tauRecTools/ITauToolBase.h"
#include "PathResolver/PathResolver.h"

class TauDiscriToolBase: public asg::AsgTool, virtual public ITauToolBase
{
  ASG_TOOL_INTERFACE( TauDiscriToolBase )
  ASG_TOOL_CLASS1( TauDiscriToolBase, ITauToolBase )
    public:
  TauDiscriToolBase(const std::string& name) : asg::AsgTool(name) {}
  
  virtual ~TauDiscriToolBase(){}
  
  virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
  
  virtual StatusCode initialize() = 0;
  
  virtual StatusCode execute(xAOD::TauJet&) = 0;
  
  virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
  
  virtual StatusCode finalize() = 0;
  
  virtual void setTauEventData(TauEventData*) {}

 protected:
  std::string find_calibFile(const std::string& name) const;

};

inline std::string TauDiscriToolBase::find_calibFile(const std::string& name) const {
  static const std::string calibDataTag = "TauDiscriminant/02-00-00/";
  std::string location = PathResolverFindCalibFile(calibDataTag+name);
  if( location == "" ){
    //assume this is from TrigTauDiscriminant
    //    location = PathResolverFindCalibFile("TrigTauDiscriminant/share/"+name);
    location = PathResolverFindCalibFile(name);
  }
  return location;
}

#endif // DISCRITOOLBASE_TAU_H
