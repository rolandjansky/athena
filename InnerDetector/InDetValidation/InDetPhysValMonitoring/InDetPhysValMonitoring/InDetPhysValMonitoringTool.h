/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"

#include "InDetTruthVertexValidation/IInDetVertexTruthMatchTool.h"

#include "InDetRecToolInterfaces/IInDetHardScatterSelectionTool.h"

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
struct InDetRttPlotConfig; 
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
    
    /// Generate an Rtt config struct based on the user-passed properties
    InDetRttPlotConfig getFilledPlotConfig() const;  

    //
    const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );
    // Do Jet/TIDE plots (Tracking In Dense Environment)
    StatusCode fillJetHistograms(const xAOD::TrackParticleContainer * pTracks, 
                        IDPVM::CachedGetAssocTruth & association,
                        const  xAOD::Vertex * primaryVtx,
                        const std::vector<const xAOD::TruthParticle*> &truthParticles);

	// accessors/decorators
    SG::AuxElement::Accessor<bool>  m_acc_hasTruthFilled{"hasTruthFilled"};
    SG::AuxElement::Decorator<bool> m_dec_hasTruthFilled{"hasTruthFilled"};	
    SG::AuxElement::Decorator<bool> m_dec_passedTruthSelection{"passedTruthSelection"};	
    SG::AuxElement::Decorator<bool> m_dec_passedTrackSelection{"passedTrackSelection"};	
    SG::AuxElement::Accessor<bool>  m_acc_selectedByPileupSwitch{"selectedByPileupSwitch"};
    SG::AuxElement::Decorator<bool> m_dec_selectedByPileupSwitch{"selectedByPileupSwitch"};

    // decorate track particle for ntuple writing
    void decorateTrackParticle(const xAOD::TrackParticle & track, const asg::AcceptData & passed) const;

    // decorate truth particle for ntuple writing
    void decorateTruthParticle(const xAOD::TruthParticle & truth, const IAthSelectionTool::CutResult & passed) const;

    // safely check the "hasTruthFilled" decoration on a truth particle
    bool hasTruthFilled(const xAOD::TruthParticle & truth) const;
    
    // safely check the "selectedByPileupSwitch" decoration on a truth particle
    bool isSelectedByPileupSwitch(const xAOD::TruthParticle & truth) const;
   
    // set the "selectedByPileupSwitch" decoration for all particles in the passed vector
    void markSelectedByPileupSwitch(const std::vector<const xAOD::TruthParticle*> & truthParticles) const;

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

    ///histograms
    std::unique_ptr< InDetRttPlots > m_monPlots;


    /// Properties to fine-tune the tool behaviour
    BooleanProperty m_useTrackSelection {this, "useTrackSelection", false, "plot only tracks accepted by selection tool"};
    BooleanProperty m_doTruthOriginPlots {this, "doTruthOriginPlots", false, "do plots split by track truth origin"};
    BooleanProperty m_doPerAuthorPlots {this, "doPerAuthorPlots", false, "do plots split by track author"};
    BooleanProperty m_doTrackInJetPlots {this, "FillTrackInJetPlots", true, "Fill plots for tracks in jets"}; 
    BooleanProperty m_doHitLevelPlots {this, "doHitLevelPlots", false, "Fill hit efficiency and residual plots"}; 
    BooleanProperty m_doBjetPlots {this, "FillTrackInBJetPlots", false, "Fill plots for tracks in truth-b-tagged jets"}; 
    BooleanProperty m_fillTruthToRecoNtuple {this, "FillTruthToRecoNtuple", false, "Fill an ntuple with truth and matching reco info for more detailed stuidies"}; 
    BooleanProperty m_useVertexTruthMatchTool {this, "useVertexTruthMatchTool", false, "Use the vertex truth matching tool"}; 
    BooleanProperty m_onlyFillMatched {this, "onlyFillTruthMatched", false, "Only fill truth-to-reco (eff, reso, matched hit) plots"}; 
    BooleanProperty m_doTRTExtensionPlots {this, "doTRTExtensionPlots", true, "do TRT extension plots"};

    FloatProperty m_maxTrkJetDR{this,"maxTrkJetDR",0.4,"the maximum dR to jets to allow for track-in-jet plots"}; 
    StringProperty m_dirName {this, "DirName", "SquirrelPlots/", "Top level directory to write histograms into"}; 
    StringProperty m_folder {this, "SubFolder", "", "Subfolder to add for plots if desired. Used when working with multiple IDPVM tool instances."}; 
    StringProperty m_pileupSwitch {this, "PileupSwitch", "HardScatter", "Pileup truth strategy to use. May be \"All\", \"HardScatter\", or \"PileUp\""}; 
    FloatProperty m_lowProb{this,"LowProb",0.5,"Truth match prob. cutoff for efficiency (lower bound) and fake (upper bound) classification."}; 
    FloatProperty m_highProb{this,"HighProb",0.8,"Truth match prob. cutoff - currently unused"}; 
    IntegerProperty m_detailLevel{this, "DetailLevel",10,"High-level steering flag for plot detail level. A value of 100 will activate the PhysVal set of plots, and a value of 200 the full set"}; 

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool{this, "TrackSelectionTool", "InDet::InDetTrackSelectionTool/TrackSelectionTool", "Track selection tool to use"};
    ToolHandle<IInDetVertexTruthMatchTool> m_vtxValidTool{this, "VertexTruthMatchTool", "InDetVertexTruthMatchTool/VtxTruthMatchTool", "Vertex truth matching tool to use"};
    ToolHandle<IAthSelectionTool> m_truthSelectionTool{this, "TruthSelectionTool","AthTruthSelectionTool", "Truth selection tool (for efficiencies and resolutions)"};
    ToolHandle<InDet::IInDetTrackTruthOriginTool> m_trackTruthOriginTool{this, "trackTruthOriginTool", "InDet::InDetTrackTruthOriginTool","truth track origin tool"};
    ToolHandle<InDet::IInDetHardScatterSelectionTool> m_hardScatterSelectionTool{this, "hardScatterSelectionTool", "InDet::InDetHardScatterSelectionTool","tool to select the hard scatter reco vertex"};

    mutable std::mutex  m_mutex;
    mutable CutFlow     m_truthCutFlow ATLAS_THREAD_SAFE; // Guarded by m_mutex
    std::vector<int> m_prospectsMatched;
    int m_truthCounter = 0;

    std::vector<std::string> m_trackCutflowNames;
    std::vector<int> m_trackCutflow;
};
#endif
