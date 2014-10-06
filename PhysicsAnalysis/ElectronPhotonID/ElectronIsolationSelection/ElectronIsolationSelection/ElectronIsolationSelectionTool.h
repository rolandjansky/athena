// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronIsolationSelectionTool.h 620241 2014-10-06 18:50:02Z christos $
#ifndef ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H
#define ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "ElectronIsolationSelection/IElectronIsolationSelectionTool.h"
#include "ElectronIsolationSelection/IShowerDepthTool.h"
#include "ElectronIsolationSelection/EisoTool2012.h"


namespace CP {

   /// Implementation of the xAOD EisoTool2012 - IElectronIsolationSelectionTool
   ///
   ///
   /// @author John Morris <john.morris@cern.ch>
   ///
   /// $Revision: 620241 $
   /// $Date: 2014-10-06 20:50:02 +0200 (Mon, 06 Oct 2014) $
   ///
   class ElectronIsolationSelectionTool :  public virtual IAsgSelectionTool,
                                           public virtual IElectronIsolationSelectionTool,
                                           public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( ElectronIsolationSelectionTool, IAsgSelectionTool,
                       CP::IElectronIsolationSelectionTool )

   public:
      /// Constructor for standalone usage
      ElectronIsolationSelectionTool( const std::string& name );

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @name Function(s) implementing the IAsgSelectionTool interface
      /// @{

      /// Get an object describing the "selection steps" of the tool
      virtual const Root::TAccept& getTAccept() const;

      /// Get the decision using a generic IParticle pointer
      virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

      /// @}

      /// @name Function(s) implementing the IMuonSelectorTool interface
      /// @{

      virtual const Root::TAccept& acceptEtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const;
      virtual const Root::TAccept& acceptPtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const;
      virtual const Root::TAccept& acceptTopocone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const;
      
       // Switch between egamma PID working points
      inline virtual void SetPID_MediumPP(){m_eiso->SetPID_MediumPP();}
      inline virtual void SetPID_TightPP(){m_eiso->SetPID_TightPP();} // default - no need to do anything post constructor
      inline virtual void SetPID_LH_Medium(){m_eiso->SetPID_LH_Medium();}
      inline virtual void SetPID_LH_Tight(){m_eiso->SetPID_LH_Tight();}
      inline virtual void SetPID_LH_VeryTight(){m_eiso->SetPID_LH_VeryTight();}       
      
    protected:
      TString findInputFile_ExpectBetterSolutionFromASG();
      
      bool validEtEtaRange( const xAOD::Electron& ele ) const;
      bool  isMC() const;
      float getElectronClusterEnergy( const xAOD::Electron& ele ) const;
      float getElectronClusterEta( const xAOD::Electron& ele ) const;
      float getElectronTrackEta( const xAOD::Electron& ele ) const;
      float getElectronEtap( const xAOD::Electron& ele , const bool& isMC ) const;
      float getElectronEtaS2( const xAOD::Electron& ele ) const;
      float getElectronEtcone( const xAOD::Electron& ele , const float& coneSize ) const;
      float getElectronPtcone( const xAOD::Electron& ele , const float& coneSize  ) const;        
      float getElectronTopocone( const xAOD::Electron& ele , const float& coneSize  ) const;
      int getNPV() const;
        
    private:
     
      // String to input file
      std::string m_inputFile;
      std::string m_sgKeyEventInfo;
      std::string m_sgKeyVertex;
     
      /// The EisoTool2012
      EisoTool2012* m_eiso;
      
      
      // Shower Depth Tool
      ToolHandle< CP::IShowerDepthTool > m_showerDepthTool;

      /// Object used to store the last decision
      mutable Root::TAccept m_accept;

   }; // class xAODEisoTool2012

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_ELECTRONISOLATIONSELECTIONTOOL_H
