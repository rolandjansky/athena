/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETITKMONITORINGTOOL_H
#define INDETPHYSVALMONITORING_INDETITKMONITORINGTOOL_H
/**
 * @file InDetItkMonitoringTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 21 February 2014
**/
//STL includes
#include <string>

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"

//local include
#include "PATCore/IAsgSelectionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODTruth/TruthParticle.h" 


//fwd declaration
class IInDetItkDecoratorTool;
class InDetRttPlots;


/**
 * Tool to book and fill inner detector histograms for physics validation
 */
class InDetItkMonitoringTool:public ManagedMonitorToolBase{
public:
	///Constructor with parameters
	InDetItkMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent);
	///Destructor
	virtual ~InDetItkMonitoringTool();
	/** \name BaseclassMethods Baseclass methods reimplemented 
	}**/
	//@{
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();
    bool truthSelector(const xAOD::TruthParticle &truth);
	//@}
private:
	///prevent default construction
	InDetItkMonitoringTool();
	///TrackParticle container's name
	std::string m_trkParticleName;
	///TruthParticle container's name
	std::string m_truthParticleName;
	///Primary vertex container's name
	std::string m_vertexContainerName;
	///EventInfo container name
	std::string m_eventInfoContainerName;
  ///Directory name
  std::string m_dirName;


	///histograms
	//InDetValidationPlots m_inDetValidationPlots;
	std::unique_ptr< InDetRttPlots > m_monPlots;
	///Tool for selecting tracks
  bool m_useTrackSelection;
	bool m_onlyInsideOutTracks;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;


  ///Jet Things
  std::string m_jetContainerName;
  float m_maxTrkJetDR;
  float m_truthMinPt;
  float m_truthMaxPt;
  float m_truthMaxEta;
  float m_truthPdgId;
  bool m_fillTIDEPlots;
  bool m_fillExtraTIDEPlots;


	template<class T>
	const T* getContainer( const std::string & containerName);
};

template<class T>
	const T* InDetItkMonitoringTool::getContainer(const std::string & containerName){
		const T * ptr = evtStore()->retrieve< const T >( containerName );
    	if (!ptr) {
        	ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
    	}
    	return ptr;
	}

#endif
