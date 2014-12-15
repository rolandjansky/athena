// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronIsolationSelectionTool.h 635994 2014-12-15 14:37:46Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H
#define ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgTools/ToolHandle.h"

// EDM include(s)
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "ElectronIsolationSelection/IElectronIsolationSelectionTool.h"

// Forward declaration(s):
namespace CP
{
  class IsolationCondition;
}
class TFile;
class TH1D;

namespace CP 
{

  /// Implementation of IElectronIsolationSelectionTool
  ///
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 635994 $
  /// $Date: 2014-12-15 15:37:46 +0100 (Mon, 15 Dec 2014) $
  ///
  class ElectronIsolationSelectionTool :  public virtual IElectronIsolationSelectionTool,
                                          public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( ElectronIsolationSelectionTool , CP::IElectronIsolationSelectionTool )

  public:
    /// Constructor for standalone usage, but please do use the interface and ToolHandle
    ElectronIsolationSelectionTool( const std::string& name );

    /// Function initialising the tool
    virtual StatusCode initialize();
    /// Function finalizing the tool
    virtual StatusCode finalize();
    
    // Configure the isolation type
    StatusCode configureCutBasedIsolation( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt=false );
    StatusCode configureEfficiencyBasedIsolation( xAOD::Iso::IsolationType type , const std::string& isolationFunction ); 
    StatusCode configureCutBasedIsolationBackground( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt=false );
    StatusCode configureEfficiencyBasedIsolationBackground( xAOD::Iso::IsolationType type , const std::string& isolationFunction );     
    
    /// Get the decision using an Electron   
    const Root::TAccept& accept( const xAOD::Electron& x ) const; 
    const Root::TAccept& acceptBackground( const xAOD::Electron& x ) const;  
    
    /// Get an object describing the "selection steps" of the tool
    const Root::TAccept& getTAccept() const;
    const Root::TAccept& getTAcceptBackground() const; 
    
    // Get the cut values corresponding to the electron pT and eta
    // Very useful if your using efficiency based isolation
    std::shared_ptr<std::map<TString,float>> getCutValues( const xAOD::Electron& x ) const;
    std::shared_ptr<std::map<TString,float>> getCutValuesBackground( const xAOD::Electron& x ) const;  
    
    // Get the isolation value for a given xAOD::Iso::IsolationType
    virtual float getIsolationValue( const xAOD::Electron& x , xAOD::Iso::IsolationType type ) const ;
    
    // Get a map of all xAOD::Iso::IsolationType and isolation values in use 
    virtual std::shared_ptr<std::map<TString,float>> getIsolationValue( const xAOD::Electron& x ) const ;  
      
  protected:
    
    void setupBinning();
    
    bool validEtEtaRange( const xAOD::Electron& x ) const;
    float getElectronClusterEnergy( const xAOD::Electron& x ) const;
    float getElectronClusterEta( const xAOD::Electron& x ) const;
    float getElectronTrackEta( const xAOD::Electron& x ) const;
        
  private:
    ElectronIsolationSelectionTool();
    ElectronIsolationSelectionTool( const ElectronIsolationSelectionTool& rhs );
    ElectronIsolationSelectionTool( const ElectronIsolationSelectionTool&& rhs );
    void operator=( const ElectronIsolationSelectionTool& rhs );
    
    // String to input file
    std::string m_configFileName;
    std::shared_ptr<TFile> m_configFile; 
    std::shared_ptr<TH1D> h_etBins;
    std::shared_ptr<TH1D> h_etaBins;
    
    std::string m_sgKeyEventInfo;
    std::string m_sgKeyVertex;
    
    // Isolation Conditions
    std::vector<std::shared_ptr<CP::IsolationCondition>> m_isolationConditions;
    std::vector<std::shared_ptr<CP::IsolationCondition>> m_isolationConditionsBackground;
    
    /// Object used to store the  decision
    mutable Root::TAccept m_accept;
    mutable Root::TAccept m_acceptBackground;    
  };

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H
