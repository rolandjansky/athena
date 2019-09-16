/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetPhysValMonitoring/IAthSelectionTool.h"
#include "InDetPhysValMonitoring/CutFlow.h"

//#include "PATCore/IAsgSelectionTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"
//EDM includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

//Athena
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
//STL includes
#include <string>
#include <vector>


//fwd declaration
class IInDetPhysValDecoratorTool;
class InDetRttPlots;
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
  void fillTrackCutFlow(const asg::AcceptData& accept);
  void fillCutFlow(const asg::AcceptData& accept, std::vector<std::string> & names, std::vector<int> & cutFlow);
  // Get truth particles into a vector, possibly using the pileup from the event
	const std::vector<const xAOD::TruthParticle *> getTruthParticles();
	//
	const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );
	// Do Jet/TIDE plots (Tracking In Dense Environment)
	StatusCode doJetPlots(const xAOD::TrackParticleContainer * pTracks, 
	                      IDPVM::CachedGetAssocTruth & association,
	                      const  xAOD::Vertex * primaryVtx,
                              const std::vector<const xAOD::TruthParticle*> &truthParticles);
	///TrackParticle container's name
        SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_trkParticleName
             {this,"TrackParticleContainerName", "InDetTrackParticles"};
	///TruthParticle container's name
        SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleName
             {this, "TruthParticleContainerName",  "TruthParticles", ""};

	///Primary vertex container's name
        SG::ReadHandleKey<xAOD::VertexContainer>  m_vertexContainerName
             {this,"VertexContainerName", "PrimaryVertices", ""};
	///Truth vertex container's name
        SG::ReadHandleKey<xAOD::TruthVertexContainer> m_truthVertexContainerName
             {this,"TruthVertexContainerName",  "TruthVertices",""};

	///EventInfo container name
        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoContainerName
             {this,"EventInfoContainerName", "EventInfo", ""};

        SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventKey
             {this, "TruthEventKey", "TruthEventName","Name of the truth events container probably either TruthEvent or TruthEvents"};
        SG::ReadHandleKey<xAOD::TruthPileupEventContainer> m_truthPileUpeEventKey
             {this, "TruthPileupEventKey", "TruthPileupEventName","Name of the truth pileup events container probably TruthPileupEvent(s)"};

        // needed to indicate data dependencies
        std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> > m_floatTrkDecor;
        std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> > m_intTrkDecor;
        std::vector<SG::ReadDecorHandleKey<xAOD::TruthParticleContainer> > m_floatTruthDecor;
        std::vector<SG::ReadDecorHandleKey<xAOD::TruthParticleContainer> > m_intTruthDecor;

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
        mutable std::mutex  m_mutex;
        mutable CutFlow     m_truthCutFlow;
	std::vector<int> m_prospectsMatched;
	int m_twoMatchedEProb;
	int m_threeMatchedEProb;
	int m_fourMatchedEProb;
	int m_truthCounter;

	std::vector<std::string> m_trackCutflowNames;
	std::vector<int> m_trackCutflow;
	std::string m_pileupSwitch; // All, PileUp, or HardScatter
	///Jet Things
        SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerName
             {this, "jetContainerName", "AntiKt4TruthJets" , ""};

	float m_maxTrkJetDR;
	bool m_fillTIDEPlots;
	bool m_fillExtraTIDEPlots;

	std::string m_folder;
};
#endif
