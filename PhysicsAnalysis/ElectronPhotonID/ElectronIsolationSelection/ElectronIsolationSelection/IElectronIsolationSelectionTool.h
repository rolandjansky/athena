// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IElectronIsolationSelectionTool.h 635994 2014-12-15 14:37:46Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H
#define ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODPrimitives/IsolationType.h"

// std include(s):
#include <map>
#include <memory>

  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 635994 $
  /// $Date: 2014-12-15 15:37:46 +0100 (Mon, 15 Dec 2014) $
  ///
  /// @brief Select isolated electrons
  ///       

namespace CP 
{
  class IElectronIsolationSelectionTool : public virtual asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( CP::IElectronIsolationSelectionTool )

  public:
         
    // Configure the isolation type
    // This can be cut based or efficiency based
    // For example,
    //   configureCutBasedIsolation( ptcone30 , 4000.0 );
    //     This will ensure that the fully corrected ptcone30 < 4000 MeV
    //
    //   configureEfficiencyBasedIsolation( topoetcone20 , "0.95" );
    //     This will ensure that the fully corrected topoetcone20
    //     has a 95% efficiency across all pT and eta
    //     as determined by Z->ee tag-and-probe
    //     your complicated final state may differ
    //
    //   configureEfficiencyBasedIsolation( topoetcone30 , "(1.0-(TMath::Exp(-x/15.)))" );
    //     This will ensure that the fully corrected topoetcone30
    //     has an efficiency that matches the function 
    //       x is the pT and is written in GeV
    //   
    //   configureEfficiencyBasedIsolation( topoetcone30 , "0.5704+0.0054*x );
    //     70% effieinct at 25GeV , 100% efficient at 80GeV
    //
    //
    //  Combinations - adding multiple isolation points
    //  This will work with a logical AND
    //  All isolations will be required to pass
    //    configureCutBasedIsolation( ptcone30 , 4000.0 );
    //    configureCutBasedIsolation( topoetcone20 , 4500.0 );
    //
    //  or something like:
    //   configureEfficiencyBasedIsolation( ptcone30 , "0.98" );
    //   configureEfficiencyBasedIsolation( topoetcone30 , "(1.0-(TMath::Exp(-x/15.)))" );
    
    // configure isolation for signal
    virtual StatusCode configureCutBasedIsolation( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt=false ) = 0;
    virtual StatusCode configureEfficiencyBasedIsolation( xAOD::Iso::IsolationType type , const std::string& isolationFunction ) = 0;
    
    // configure isolation for background
    virtual StatusCode configureCutBasedIsolationBackground( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt=false ) = 0;
    virtual StatusCode configureEfficiencyBasedIsolationBackground( xAOD::Iso::IsolationType type , const std::string& isolationFunction ) = 0;       
    
    /// Get the decision using an Electron 
    virtual const Root::TAccept& accept( const xAOD::Electron& x ) const = 0; 
    virtual const Root::TAccept& acceptBackground( const xAOD::Electron& x ) const = 0;
    
    /// Get an object describing the "selection steps" of the tool
    virtual const Root::TAccept& getTAccept() const = 0;
    virtual const Root::TAccept& getTAcceptBackground() const = 0;  
    
    // Get the cut values corresponding to the electron pT and eta
    // Very useful if your using efficiency based isolation
    virtual std::shared_ptr<std::map<TString,float>> getCutValues( const xAOD::Electron& x ) const = 0;
    virtual std::shared_ptr<std::map<TString,float>> getCutValuesBackground( const xAOD::Electron& x ) const = 0;
    
    // Get the isolation value for a given xAOD::Iso::IsolationType
    virtual float getIsolationValue( const xAOD::Electron& x , xAOD::Iso::IsolationType type ) const = 0;
    
    // Get a map of all xAOD::Iso::IsolationType and isolation values in use 
    virtual std::shared_ptr<std::map<TString,float>> getIsolationValue( const xAOD::Electron& x ) const = 0;
    
      
  }; // class IElectronIsolationSelectionTool

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H
