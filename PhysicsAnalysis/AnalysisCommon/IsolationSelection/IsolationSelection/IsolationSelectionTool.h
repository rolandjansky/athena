// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationSelectionTool.h 680193 2015-07-03 15:30:37Z jdevivi $
#ifndef ISOLATIONSELECTION_ISOLATIONSELECTIONTOOL_H
#define ISOLATIONSELECTION_ISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IsolationWP.h"

// Forward declaration(s):
class TFile;
class TH1F;

namespace CP 
{

  /// Implementation of IIsolationSelectionTool
  ///
  ///
  /// @author John Morris <john.morris@cern.ch>
  /// @author Dongliang Zhang <dongliang.zhang@cern.ch>
  ///
  /// $Revision: 680193 $
  /// $Date: 2015-07-03 17:30:37 +0200 (Fri, 03 Jul 2015) $
  ///
  
  class IsolationSelectionTool :  public virtual CP::IIsolationSelectionTool,
                                  public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( IsolationSelectionTool , CP::IIsolationSelectionTool )

    public:
      /// Constructor for standalone usage, but please do use the interface and ToolHandle
      IsolationSelectionTool( const std::string& name ); 
      /// Destructor
      virtual ~IsolationSelectionTool();
      
      /// Function initialising the tool
      virtual StatusCode initialize();
      /// Function finalizing the tool
      virtual StatusCode finalize();   
      
      enum IsoWPType{Efficiency, Cut};
      virtual const Root::TAccept& accept( const xAOD::Photon& x   ) const;
      virtual const Root::TAccept& accept( const xAOD::Electron& x ) const;
      virtual const Root::TAccept& accept( const xAOD::Muon& x     ) const;
      virtual const Root::TAccept& accept( const strObj& x     ) const;
      virtual const Root::TAccept& accept(const xAOD::IParticle& x) const; // for tracks, and others?
      
      virtual const Root::TAccept& getPhotonTAccept()   const;
      virtual const Root::TAccept& getElectronTAccept() const;
      virtual const Root::TAccept& getMuonTAccept()     const;
      virtual const Root::TAccept& getObjTAccept()     const;
     
      StatusCode addWP(std::string WP, xAOD::Type::ObjectType type);
      StatusCode addWP(IsolationWP* wp, xAOD::Type::ObjectType type);
      StatusCode addMuonWP(std::string wpname);
      StatusCode addPhotonWP(std::string wpname);
      StatusCode addElectronWP(std::string wpname);
      StatusCode addUserDefinedWP(std::string WPname, xAOD::Type::ObjectType ObjType, std::vector< std::pair<xAOD::Iso::IsolationType, std::string> >& cuts, std::string key="", IsoWPType type=Efficiency);
      StatusCode setIParticleCutsFrom(xAOD::Type::ObjectType ObjType);
      void addCutToWP(IsolationWP* wp, std::string key, const xAOD::Iso::IsolationType t, const std::string expression);
      std::vector< IsolationWP* >& getMuonWPs(){return m_muWPs;}
      std::vector< IsolationWP* >& getElectronWPs(){return m_elWPs;}
      std::vector< IsolationWP* >& getPhotonWPs(){return m_phWPs;}
      std::vector< IsolationWP* >& getObjWPs(){return m_objWPs;}

    private:
      // Stupid ROOT
      TDirectory* getTemporaryDirectory(void) const;
      
      std::string m_muWPname;
      std::string m_elWPname;
      std::string m_phWPname;
      std::string m_muWPKey;
      std::string m_elWPKey;
      std::string m_phWPKey;

      /// input file 
      std::string m_calibFileName;
      std::shared_ptr<TFile> m_calibFile;

      /// internal use 
      std::vector< IsolationWP* > m_muWPs;
      std::vector< IsolationWP* > m_elWPs;
      std::vector< IsolationWP* > m_phWPs;
      std::vector< IsolationWP* > m_objWPs;

      /// TAccept's
      mutable Root::TAccept m_photonAccept;
      mutable Root::TAccept m_electronAccept;
      mutable Root::TAccept m_muonAccept;
      mutable Root::TAccept m_objAccept;

      /// Iparticle interface
      std::vector< IsolationWP* >* m_iparWPs;
      mutable Root::TAccept* m_iparAccept;
  };
} // namespace

#endif
