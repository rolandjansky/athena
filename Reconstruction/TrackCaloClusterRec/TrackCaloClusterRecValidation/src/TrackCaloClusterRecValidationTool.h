/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TRACKCALOCLUSTERRECVALIDATIONTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TRACKCALOCLUSTERRECVALIDATIONTOOL_H
/**
 * @file TrackCaloClusterRecValidationTool.h
 * header file for class of same name
 * @author shaun roe
 * @date 21 February 2014
**/

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"
//EDM includes
#include "xAODTruth/TruthParticleContainer.h"
//Athena

//STL includes
#include <string>
#include <vector>

#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

class TCCPlots;
class IJetCalibrationTool;

/**
 * Tool to book and fill inner detector histograms for physics validation
 */
class TrackCaloClusterRecValidationTool: public ManagedMonitorToolBase {
public:
    ///Constructor with parameters
    TrackCaloClusterRecValidationTool(const std::string & type, const std::string & name, const IInterface* parent);
    ///Destructor
    virtual ~TrackCaloClusterRecValidationTool();
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
    TrackCaloClusterRecValidationTool();
    
    /// calibration tool
    ToolHandleArray<IJetCalibrationTool>  m_jetCalibrationTools;
    std::vector< std::string >            m_jetCalibrationCollections;
    bool                                  m_applyCalibration;
    
    /**Calibrate and record a shallow copy of a given jet container */
    const xAOD::JetContainer* calibrateAndRecordShallowCopyJetCollection(const xAOD::JetContainer * jetContainer, const std::string name);
    
    /// Get the matched jet
    const xAOD::Jet* ClusterMatched(const xAOD::Jet* jet, const xAOD::JetContainer* jets);
    const xAOD::Jet* ClusterMatched(const xAOD::Jet* jet, std::vector<const xAOD::Jet*> jets);
    
    // Get a data container; implementation at end of this header file
    template<class T> const T* getContainer( const std::string & containerName);
        
    bool m_saveJetInfo;
    ///Truth jet container's name
    std::string m_truthJetContainerName;
    std::string m_truthTrimmedJetContainerName;
    
    std::vector<std::string> m_jetContainerNames;
    
    std::string m_vertexContainerName;
    
    std::string m_topoJetReferenceName;
    std::string m_topoTrimmedJetReferenceName;

    ///Directory name
    std::string m_dirName;
    float m_maxJetDR;
    float m_maxEta;
    float m_minPt;
    float m_minMass;
    float m_maxMass;
    
    std::string m_folder;
    
    std::string m_trackParticleCollectionName;
   
    bool  m_saveTrackInfo;
    bool  m_saveMatchingInfo;
    float m_trackPtMin;
    
    std::vector<float> m_jetPtBins           ;
    std::vector<float> m_jetMassOverPtBins   ;
    std::vector<float> m_trackPtBins         ;
    std::vector<float> m_trackProdRadiusBins ;
    
    bool m_saveClusterInfo;
    std::string m_caloClusterCollectionName;
    float m_caloClusterEtaMax;
    
    bool m_saveTCCInfo;
    std::vector<std::string> m_TCCCombinedCollectionNames;
    float m_tccPtMin;
    float m_tccEtaMax;
    
    //histograms
    std::map<std::string, TCCPlots*>    m_tccPlots;

    // EventInfo
    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};

template<class T>
inline const T* TrackCaloClusterRecValidationTool::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_WARNING("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}

#endif //TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TRACKCALOCLUSTERRECVALIDATIONTOOL_H
