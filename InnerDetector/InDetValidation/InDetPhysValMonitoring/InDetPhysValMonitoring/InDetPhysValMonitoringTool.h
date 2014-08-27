/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPHYSVALMONITORINGTOOL_H
#define INDETPHYSVALMONITORING_INDETPHYSVALMONITORINGTOOL_H
/**
 * @file InDetPhysValMonitoringTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 21 February 2014
**/
//STL includes
#include <string>

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"

//local include
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetPhysValMonitoring/InDetValidationPlots.h"


//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttPlots;

namespace Trk{
	class ITrackSelectorTool;
}


/**
 * Tool to book and fill inner detector histograms for physics validation
 */
class InDetPhysValMonitoringTool:public ManagedMonitorToolBase{
public:
	///Constructor with parameters
	InDetPhysValMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent);
	///Destructor
	virtual ~InDetPhysValMonitoringTool();
	/** \name BaseclassMethods Baseclass methods reimplemented 
	}**/
	//@{
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();
	//@}
private:
	///prevent default construction
	InDetPhysValMonitoringTool();
	///TrackParticle container's name
	std::string m_trkParticleName;
	///TruthParticle container's name
	std::string m_truthParticleName;
	///Primary vertex container's name
	std::string m_vertexContainerName;
	///EventInfo container name
	std::string m_eventInfoContainerName;

	///histograms
	//InDetValidationPlots m_inDetValidationPlots;
	InDetRttPlots * m_monPlots;
	///Tool for selecting tracks
	ToolHandle<Trk::ITrackSelectorTool> m_trackSelectorTool;
	///bool variable to determine whether a track selector is used (default is false, then all tracks are used)
	bool m_useTrackSelection;
	ToolHandle<IInDetPhysValDecoratorTool> m_truthDecoratorTool;
	ToolHandle<IInDetPhysValDecoratorTool> m_hitDecoratorTool;
	ToolHandle<IInDetPhysValDecoratorTool> m_errDecoratorTool;
	ToolHandle<IInDetPhysValDecoratorTool> m_truthClassDecoratorTool;

	template<class T>
	const T* getContainer( const std::string & containerName);
};

template<class T>
	const T* InDetPhysValMonitoringTool::getContainer(const std::string & containerName){
		const T * ptr = evtStore()->retrieve< const T >( containerName );
    	if (!ptr) {
        	ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
    	}
    	return ptr;
	}

#endif
