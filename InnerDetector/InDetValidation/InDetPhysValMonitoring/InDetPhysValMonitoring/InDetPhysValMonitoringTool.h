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
#include "PATCore/IAsgSelectionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTruth/TruthParticleContainer.h"

#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "src/InDetPhysHitDecoratorTool.h"
#include "InDetPhysValMonitoring/ITrackTruthSelectionTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

//#include "src/GeoUtilsTool.h"

//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttPlots;
class ITrackTruthSelectionTool;
namespace Root {
  class TAccept;
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
	///Truth vertex container's name
	std::string m_truthVertexContainerName;
	///EventInfo container name
	std::string m_eventInfoContainerName;

	///Directory name
	std::string m_dirName;
	
	///histograms
	std::unique_ptr< InDetRttPlots > m_monPlots;
	///Tool for selecting tracks
	bool m_useTrackSelection;
	bool m_onlyInsideOutTracks;
	bool m_TrkSelectPV;   // make track selection relative to PV

	ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;
	ToolHandle<ITrackTruthSelectionTool> m_truthSelectionTool;
	//ToolHandle<GeoUtils::GeoUtilsTool> m_geoUtilsTool;
	
	std::vector<int> m_prospectsMatched;
	int m_twoMatchedEProb;
	int m_threeMatchedEProb;
	int m_fourMatchedEProb;
	int m_truthCounter;

	void fillTrackCutFlow(Root::TAccept& accept);
	std::vector<std::string> m_trackCutflowNames;
	std::vector<int> m_trackCutflow;

	void fillTruthCutFlow(Root::TAccept& accept);
	std::vector<std::string> m_truthCutflowNames;
	std::vector<int> m_truthCutflow;
	
	void fillCutFlow(Root::TAccept& accept, std::vector<std::string> & names, std::vector<int> & cutFlow);
	
	std::string m_pileupSwitch; // All, PileUp, or HardScatter
	
	///Jet Things
	std::string m_jetContainerName;
	float m_maxTrkJetDR;
	bool m_fillTIDEPlots;
	bool m_fillExtraTIDEPlots;

	std::string m_folder;

	void getTruthParticles(std::vector<const xAOD::TruthParticle*>& truthParticles);
	
	const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );

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
