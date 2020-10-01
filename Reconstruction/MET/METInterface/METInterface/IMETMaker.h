///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMETMaker.h
// Header file for interface IMETMaker
//
// This is the tool that rebuilds MET at analysis level
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: TJ Khoo
///////////////////////////////////////////////////////////////////

#ifndef METINTERFACE_IMETMAKER_H
#define METINTERFACE_IMETMAKER_H

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODJet/JetContainer.h"

class IMETMaker :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETMaker)

public:

  ///////////////////////////////////////////////////////////////////
  // Ele/Gamma/Mu/Tau MET term rebuilding
  ///////////////////////////////////////////////////////////////////

  // Convenience method that creates a MET term internally
  virtual StatusCode rebuildMET(const std::string& metKey,
                                xAOD::Type::ObjectType metType,
                                xAOD::MissingETContainer* metCont,
                                const xAOD::IParticleContainer* collection,
                                xAOD::MissingETAssociationHelper* helper,
                                MissingETBase::UsageHandler::Policy objScale=MissingETBase::UsageHandler::PhysicsObject) = 0;
  // Default method that uses standard overlap removal policy
  virtual StatusCode rebuildMET(xAOD::MissingET* met,
                                const xAOD::IParticleContainer* collection,
                                xAOD::MissingETAssociationHelper* helper,
                                MissingETBase::UsageHandler::Policy objScale=MissingETBase::UsageHandler::PhysicsObject) = 0;
  // Full implementation with option flags
  virtual StatusCode rebuildMET(xAOD::MissingET* met,
                                const xAOD::IParticleContainer* collection,
                                xAOD::MissingETAssociationHelper* helper,
                                MissingETBase::UsageHandler::Policy p,
                                bool removeOverlap,
                                MissingETBase::UsageHandler::Policy objScale=MissingETBase::UsageHandler::PhysicsObject) = 0;

  ///////////////////////////////////////////////////////////////////
  // Jet/soft MET term rebuilding
  ///////////////////////////////////////////////////////////////////

  // Convenience methods that creates MET terms internally
  // Version with single soft term
  virtual StatusCode rebuildJetMET(const std::string& metJetKey,
                                   const std::string& metSoftKey,
                                   xAOD::MissingETContainer* metCont,
                                   const xAOD::JetContainer* jets,
                                   const xAOD::MissingETContainer* metCoreCont,
                                   xAOD::MissingETAssociationHelper* helper,
                                   bool doJetJVT) = 0;
  // Version with two soft terms
  virtual StatusCode rebuildJetMET(const std::string& metJetKey,
                                   const std::string& softClusKey,
                                   const std::string& softTrkKey,
                                   xAOD::MissingETContainer* metCont,
                                   const xAOD::JetContainer* jets,
                                   const xAOD::MissingETContainer* metCoreCont,
                                   xAOD::MissingETAssociationHelper* helper,
                                   bool doJetJVT) = 0;
  // Full version receiving MET pointers
  virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
                                   const xAOD::JetContainer* jets,
                                   xAOD::MissingETAssociationHelper* helper,
                                   xAOD::MissingET* metSoftClus,
                                   const xAOD::MissingET* coreSoftClus,
                                   xAOD::MissingET* metSoftTrk,
                                   const xAOD::MissingET* coreSoftTrk,
                                   bool doJetJVT,
                                   bool tracksForHardJets=false,
				   std::vector<const xAOD::IParticle*>* softConst=0) = 0;

  // Convenience methods that creates MET terms internally
  virtual StatusCode rebuildTrackMET(const std::string& metJetKey,
                                   const std::string& metSoftKey,
                                   xAOD::MissingETContainer* metCont,
                                   const xAOD::JetContainer* jets,
                                   const xAOD::MissingETContainer* metCoreCont,
                                   xAOD::MissingETAssociationHelper* helper,
                                   bool doJetJVT) = 0;
  // Full version receiving MET pointers
  virtual StatusCode rebuildTrackMET(xAOD::MissingET* metJet,
                                   const xAOD::JetContainer* jets,
                                   xAOD::MissingETAssociationHelper* helper,
                                   xAOD::MissingET* metSoftTrk,
                                   const xAOD::MissingET* coreSoftTrk,
                                   bool doJetJVT) = 0;

  ///////////////////////////////////////////////////////////////////
  // Additional utility commands
  ///////////////////////////////////////////////////////////////////

  virtual StatusCode markInvisible(const xAOD::IParticleContainer* collection,
				   xAOD::MissingETAssociationHelper* helper,
				   xAOD::MissingETContainer* metCont) = 0;
};

#endif
