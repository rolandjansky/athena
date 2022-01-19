///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// METNet.h
// Header file for class METNet
// Author: M.Leigh<mleigh@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef METUTILITIES_MET_METNET_H
#define METUTILITIES_MET_METNET_H

// STL includes
#include <string>

// Asg tool includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/MessageCheck.h"

// METInterface includes
#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSignificance.h"

// Network handler
#include "METNetHandler.h"

// Forward declarations
class IMETMaker;
class IMETSignificance;
class METNetHandler;

namespace met {

  /** METNet tool class used to create a missing transverse momentum estimate using a pre-trained neural network.
  * The tool inherits from the same interface as METMaker and so it should be largely interchangable.
  * However, it does not use every public method as METMaker, as it must ensure that the
  * exact variables are constructed for the network's inputs.
  * Uses an ONNX runtime environment for inference.
  */
  class METNet final: public asg::AsgTool, virtual public IMETMaker {

    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS( METNet, IMETMaker )

  public:

    /// Constructor
    METNet( const std::string& name );

    /// Destructor
    ~METNet() override;

    /// Athena algtool's initialize
    StatusCode initialize() override;

    /// Adds a collection of objects to the core MET container, must be done for photons, elections and muons.
    StatusCode rebuildMET( const std::string& metKey,
                           xAOD::Type::ObjectType metType,
                           xAOD::MissingETContainer* metCont,
                           const xAOD::IParticleContainer* collection,
                           const xAOD::MissingETAssociationMap* map,
                           MissingETBase::UsageHandler::Policy objScale ) override;

    /// Adds multiple jet definitions to seperate MET containers and creates the collection of input features for the neural network
    StatusCode rebuildJetMET( const std::string& metJetKey,
                              const std::string& softTrkKey,
                              xAOD::MissingETContainer* metCont,
                              const xAOD::JetContainer* jets,
                              const xAOD::MissingETContainer* metCoreCont,
                              const xAOD::MissingETAssociationMap* map,
                              bool doJetJVT ) override;

    /// Uses ONNX runtime to propagate the input features created in rebuildJetMET through the trained network
    StatusCode buildMETSum( const std::string& totalName,
                            xAOD::MissingETContainer* metCont,
                            MissingETBase::Types::bitmask_t softTermsSource ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildMET( xAOD::MissingET* met,
                           const xAOD::IParticleContainer* collection,
                           const xAOD::MissingETAssociationMap* map,
                           MissingETBase::UsageHandler::Policy objScale ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildMET( xAOD::MissingET* met,
                           const xAOD::IParticleContainer* collection,
                           const xAOD::MissingETAssociationMap* map,
                           MissingETBase::UsageHandler::Policy p,
                           bool removeOverlap,
                           MissingETBase::UsageHandler::Policy objScale ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildJetMET( const std::string& metJetKey,
                              const std::string& softClusKey,
                              const std::string& softTrkKey,
                              xAOD::MissingETContainer* metCont,
                              const xAOD::JetContainer* jets,
                              const xAOD::MissingETContainer* metCoreCont,
                              const xAOD::MissingETAssociationMap* map,
                              bool doJetJVT ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildJetMET( xAOD::MissingET* metJet,
                              const xAOD::JetContainer* jets,
                              const xAOD::MissingETAssociationMap* map,
                              xAOD::MissingET* metSoftClus,
                              const xAOD::MissingET* coreSoftClus,
                              xAOD::MissingET* metSoftTrk,
                              const xAOD::MissingET* coreSoftTrk,
                              bool doJetJVT,
                              bool tracksForHardJets = false,
                              std::vector<const xAOD::IParticle*>* softConst = 0 ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildTrackMET( const std::string& metJetKey,
                                const std::string& softTrkKey,
                                xAOD::MissingETContainer* metCont,
                                const xAOD::JetContainer* jets,
                                const xAOD::MissingETContainer* metCoreCont,
                                const xAOD::MissingETAssociationMap* map,
                                bool doJetJVT ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode rebuildTrackMET( xAOD::MissingET* metJet,
                                const xAOD::JetContainer* jets,
                                const xAOD::MissingETAssociationMap* map,
                                xAOD::MissingET* metSoftTrk,
                                const xAOD::MissingET* coreSoftTrk,
                                bool doJetJVT ) override;

    /// Unsuported method inherited from METMaker. Please do not use.
    StatusCode markInvisible( const xAOD::IParticleContainer* collection,
                              const xAOD::MissingETAssociationMap* map,
                              xAOD::MissingETContainer* metCont ) override;

  private:

    // Class properties
    std::string m_NetLocation;
    std::string m_JetCollection;
    std::string m_PVContainer;
    std::string m_jetJvtMomentName;
    std::string m_jetFwdJvtMomentName;
    std::string m_jetFwdJvtCutName;

    // Configurable accessors
    std::unique_ptr<SG::AuxElement::ConstAccessor<float > > m_acc_jvt;
    std::unique_ptr<SG::AuxElement::ConstAccessor<float > > m_acc_fjvt;
    std::unique_ptr<SG::AuxElement::ConstAccessor<char > > m_acc_fjvtCut;

    // Default constructor
    METNet();

    // Athena configured tools
    std::unique_ptr<METNetHandler> m_metnethandler;
    asg::AnaToolHandle<IMETMaker> m_metmaker_tight;
    asg::AnaToolHandle<IMETMaker> m_metmaker_loose;
    asg::AnaToolHandle<IMETMaker> m_metmaker_tghtr;
    asg::AnaToolHandle<IMETMaker> m_metmaker_fjvt;
    asg::AnaToolHandle<IMETSignificance> m_metsigtool;

    // Private methods
    StatusCode addMETFinal( std::string WP_name, xAOD::MissingETContainer* met_container, float actmu, std::vector<std::string>& name_vec, std::vector<float>& val_vec );
    StatusCode addMETTerm( std::string WP_name, xAOD::MissingET* met, std::vector<std::string>& name_vec, std::vector<float>& val_vec );
    StatusCode addInputValue( std::string var_name, float value,std::vector<std::string>& name_vec, std::vector<float>& val_vec );

    StatusCode copyMETContainer( xAOD::MissingETContainer* new_container,
                                 xAOD::MissingETContainer* old_container,
                                 bool excl_gama = false );

    double GetPUProb( double jet_eta, double jet_pt,
                      double jet_jvt, double jet_fjvt,
                      float avgmu );

  };

}

#endif
