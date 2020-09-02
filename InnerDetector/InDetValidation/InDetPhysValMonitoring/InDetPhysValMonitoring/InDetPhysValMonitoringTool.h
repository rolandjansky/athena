/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


#include "InDetTruthVertexValidation/IInDetVertexTruthMatchTool.h"

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
#include "CxxUtils/checker_macros.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
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
    const std::vector<const xAOD::TruthParticle *> getTruthParticles() const;
    std::pair<const std::vector<const xAOD::TruthVertex*>, const std::vector<const xAOD::TruthVertex*>> getTruthVertices() const;

    //
    const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );
    // Do Jet/TIDE plots (Tracking In Dense Environment)
    StatusCode fillJetHistograms(const xAOD::TrackParticleContainer * pTracks, 
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

    SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventName
        {this, "TruthEvents", "TruthEvents","Name of the truth events container probably either TruthEvent or TruthEvents"};

    SG::ReadHandleKey<xAOD::TruthPileupEventContainer> m_truthPileUpEventName
        {this, "TruthPileupEvents", "TruthPileupEvents","Name of the truth pileup events container probably TruthPileupEvent(s)"};

    SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerName
        {this, "JetContainerName", "AntiKt4LCTopoJets" , ""};


    // needed to indicate data dependencies
    std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> > m_floatTrkDecor;
    std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> > m_intTrkDecor;
    std::vector<SG::ReadDecorHandleKey<xAOD::TruthParticleContainer> > m_floatTruthDecor;
    std::vector<SG::ReadDecorHandleKey<xAOD::TruthParticleContainer> > m_intTruthDecor;
    std::vector<SG::ReadDecorHandleKey<xAOD::JetContainer> > m_intJetDecor;

    ///Directory name
    std::string m_dirName;

    ///histograms
    std::unique_ptr< InDetRttPlots > m_monPlots;
    ///Tool for selecting tracks
    bool m_useTrackSelection;
    bool m_useVertexTruthMatchTool;
    bool m_TrkSelectPV;   // make track selection relative to PV
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;
    ToolHandle<IInDetVertexTruthMatchTool> m_vtxValidTool;
    ToolHandle<IAthSelectionTool> m_truthSelectionTool;
    mutable std::mutex  m_mutex;
    mutable CutFlow     m_truthCutFlow ATLAS_THREAD_SAFE; // Guarded by m_mutex
    std::vector<int> m_prospectsMatched;
    float m_lowProb;
    float m_highProb;
    int m_detailLevel;
    int m_truthCounter;

    std::vector<std::string> m_trackCutflowNames;
    std::vector<int> m_trackCutflow;
    std::string m_pileupSwitch; // All, PileUp, or HardScatter
    ///Jet Things

    float m_maxTrkJetDR;
    bool m_doTrackInJetPlots;
    bool m_doBjetPlots; 

    std::string m_folder;
};
#endif
