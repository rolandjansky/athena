///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMETRebuilder.h
// Header file for interface IMETRebuilder
//
// This is the tool that rebuilds MET at analysis level
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: TJ Khoo
///////////////////////////////////////////////////////////////////

#ifndef METINTERFACE_IMETREBUILDER_H
#define METINTERFACE_IMETREBUILDER_H

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComponent.h"
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODJet/JetContainer.h"

class IMETRebuilder : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETRebuilder)

  public:

  virtual StatusCode execute() = 0;

  virtual StatusCode copyMET(const std::string& metKey,
			     xAOD::MissingETContainer* metCont,
			     const xAOD::MissingETComponentMap* metMap) = 0;

  virtual StatusCode rebuildMET(const std::string& metKey,
				xAOD::MissingETContainer* metCont,
				const xAOD::IParticleContainer* collection,
				const xAOD::MissingETComponentMap* metMap,
				bool doTracks=true) = 0;

  virtual StatusCode rebuildMET(xAOD::MissingET* met,
				const xAOD::IParticleContainer* collection,
				const xAOD::MissingETComponent* component,
				bool doTracks) = 0;

  ///

  virtual StatusCode rebuildJetMET(const std::string& jetKey,
				   const std::string& softKey,
				   xAOD::MissingETContainer* metCont,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETComponentMap* metMap,
				   bool doTracks=true) = 0;

  virtual StatusCode rebuildJetMET(const std::string& jetKey,
				   const std::string& softKey,
				   xAOD::MissingETContainer* metCont,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETComponentMap* metMap,
				   bool doTracks,
				   bool doJvfCut,
				   bool pureTrkSoft,
				   const std::string& softJetScale) = 0;

  virtual StatusCode rebuildJetMET(const std::string& jetKey,
				   const std::string& softKey,
				   xAOD::MissingETContainer* metCont,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETComponentMap* metMap,
				   bool doTracks,
				   bool doJvfCut,
				   bool pureTrkSoft,
				   const std::string& softJetScale,
				   float& stvf) = 0;

  virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
				   xAOD::MissingET* metSoft,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETComponent* component,
				   bool doTracks,
				   bool doJvfCut,
				   bool pureTrkSoft,
				   const std::string& softJetScale,
				   float& stvf,
				   const xAOD::MissingETComponent* comp_softtrk) = 0;
};

#endif
