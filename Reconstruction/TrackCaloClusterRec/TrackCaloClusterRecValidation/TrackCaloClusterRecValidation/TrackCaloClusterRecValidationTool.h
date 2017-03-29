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

class TCCPlots;
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
    
    /// Get the matched jet
    const xAOD::Jet* ClusterMatched(const xAOD::Jet* jet, const xAOD::JetContainer* jets);
    const xAOD::Jet* ClusterMatched(const xAOD::Jet* jet, std::vector<const xAOD::Jet*> jets);
    
    // Get a data container; implementation at end of this header file
    template<class T> const T* getContainer( const std::string & containerName);
        
    ///Truth jet container's name
    std::string m_truthJetContainerName;
    
    std::vector<std::string> m_jetContainerNames;
    
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
    
    //histograms
    std::map<std::string, TCCPlots*>    m_tccPlots;
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