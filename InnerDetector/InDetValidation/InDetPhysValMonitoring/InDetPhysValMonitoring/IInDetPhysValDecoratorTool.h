/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_IINDETPHYSVALDECORATORTOOL_H
#define INDETPHYSVALMONITORING_IINDETPHYSVALDECORATORTOOL_H
/**
 * @file IInDetPhysValDecoratorTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 27 March 2014
**/
//STL includes
#include <string>

//Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/TrackParticle.h"

static const InterfaceID IID_IInDetPhysValDecoratorTool("IInDetPhysValDecoratorTool",1,0);

//Interface class to decorate xAOD::IParticles with additional information required by validation
class IInDetPhysValDecoratorTool:virtual public IAlgTool{
public:
	static const InterfaceID & interfaceID();
	virtual ~IInDetPhysValDecoratorTool(){/**nop**/}
	virtual bool decorateTruth(const xAOD::TruthParticle & /*particle*/, const std::string& /*prefix = ""*/){return false;} //default implementation
  virtual bool decorateTrack(const xAOD::TrackParticle & /*particle*/, const std::string& /*prefix = ""*/){return false;}
};

inline const InterfaceID & IInDetPhysValDecoratorTool::interfaceID(){
	return IID_IInDetPhysValDecoratorTool;
} 

#endif
