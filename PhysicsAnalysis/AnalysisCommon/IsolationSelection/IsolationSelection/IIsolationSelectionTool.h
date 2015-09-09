// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IIsolationSelectionTool.h 672685 2015-06-05 11:01:39Z dzhang $
// #ifndef ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
// #define ISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
#ifndef ELECTRONISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H
#define ELECTRONISOLATIONSELECTION_IISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODPrimitives/IsolationType.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
// #include "IsolationSelection/strMuon.h"

// std include(s):
#include <map>
#include <memory>

  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 672685 $
  /// $Date: 2015-06-05 13:01:39 +0200 (Fri, 05 Jun 2015) $
  ///
  /// @brief Select isolated Photons, Electrons and Muons
  ///  
  
namespace CP
{
  class IIsolationSelectionTool :  virtual public asg::IAsgTool {

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( CP::IIsolationSelectionTool )

    public: 
      // Cut on object
      virtual const Root::TAccept& accept( const xAOD::Photon& x   ) const = 0;
      virtual const Root::TAccept& accept( const xAOD::Electron& x ) const = 0;
      virtual const Root::TAccept& accept( const xAOD::Muon& x     ) const = 0;
      
      virtual const Root::TAccept& getPhotonTAccept()   const = 0;
      virtual const Root::TAccept& getElectronTAccept() const = 0;
      virtual const Root::TAccept& getMuonTAccept()     const = 0;       
      
  };
} // end namespace
#endif
