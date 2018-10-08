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




//local include
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "InDetPhysValMonitoring/IAthSelectionTool.h"

//#include "PATCore/IAsgSelectionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"
//EDM includes
#include "xAODTruth/TruthParticleContainer.h"
//Athena
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
//STL includes
#include <string>
#include <vector>


//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttPlots;
namespace Root {
  class TAccept;
}
namespace IDPVM{
  class CachedGetAssocTruth;
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
  // Private utility methods
  void fillTrackCutFlow(Root::TAccept& accept);
  void fillCutFlow(Root::TAccept& accept, std::vector<std::string> & names, std::vector<int> & cutFlow);
  // Get truth particles into a vector, possibly using the pileup from the event
	const std::vector<const xAOD::TruthParticle *> getTruthParticles();
	//
	const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );
  // Get a data container; implementation at end of this header file
  template<class T>
	const T* getContainer( const std::string & containerName);
	// Do Jet/TIDE plots (Tracking In Dense Environment)
	StatusCode doJetPlots(const xAOD::TrackParticleContainer * pTracks, 
	                      IDPVM::CachedGetAssocTruth & association,
	                      const  xAOD::Vertex * primaryVtx);
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
  ToolHandle<IAthSelectionTool> m_truthSelectionTool;
	std::vector<int> m_prospectsMatched;
	int m_twoMatchedEProb;
	int m_threeMatchedEProb;
	int m_fourMatchedEProb;
	int m_truthCounter;

	std::vector<std::string> m_trackCutflowNames;
	std::vector<int> m_trackCutflow;
	std::vector<unsigned int> m_truthCutCounters;
	std::string m_pileupSwitch; // All, PileUp, or HardScatter
	bool m_pileupPlots;
	///Jet Things
	std::string m_jetContainerName;
	float m_maxTrkJetDR;
	bool m_fillTIDEPlots;
	bool m_fillExtraTIDEPlots;

	std::string m_folder;
};

template<class T>
  inline 
	const T* InDetPhysValMonitoringTool::getContainer(const std::string & containerName){
		const T * ptr = evtStore()->retrieve< const T >( containerName );
    	if (!ptr) {
        	ATH_MSG_WARNING("Container '"<<containerName<<"' could not be retrieved");
    	}
    	return ptr;
	}

#endif
