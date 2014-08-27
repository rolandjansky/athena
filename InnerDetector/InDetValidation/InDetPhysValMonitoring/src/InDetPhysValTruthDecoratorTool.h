/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPHYSVALTRUTHDECORATORTOOL_H
#define INDETPHYSVALMONITORING_INDETPHYSVALTRUTHDECORATORTOOL_H
/**
 * @file InDetPhysValTruthDecoratorTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 27 March 2014
**/
//STL includes
#include <string>
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IExtrapolator.h"

//class to decorate xAOD::TruthParticles with additional information required by validation
class InDetPhysValTruthDecoratorTool:virtual public IInDetPhysValDecoratorTool, public AthAlgTool{
public:
	
	 InDetPhysValTruthDecoratorTool(const std::string& type,const std::string& name,const IInterface* parent);
	 virtual ~InDetPhysValTruthDecoratorTool ();
	 virtual StatusCode initialize();
	 virtual StatusCode finalize  ();
	 virtual bool decorateTruth(const xAOD::TruthParticle & particle);	
 private:
	 ToolHandle<Trk::IExtrapolator> m_extrapolator;
};
#endif
