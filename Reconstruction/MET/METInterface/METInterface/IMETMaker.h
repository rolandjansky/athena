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
	
class IMETMaker : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETMaker)
	
  public:

  //  virtual StatusCode execute() = 0;

  virtual StatusCode rebuildMET(xAOD::MissingET* met,
				const xAOD::IParticleContainer* collection,
				const xAOD::MissingETAssociationMap* map,
				std::vector<const xAOD::IParticle*>& uniques) = 0;

  virtual StatusCode rebuildMET(xAOD::MissingET* met,
				const xAOD::IParticleContainer* collection,
				const xAOD::MissingETAssociationMap* map) = 0;
  virtual StatusCode rebuildMET(xAOD::MissingET* met,
				const xAOD::IParticleContainer* collection,
				const xAOD::MissingETAssociationMap* map,
				std::vector<const xAOD::IParticle*>& uniques,
				MissingETBase::UsageHandler::Policy p,
				bool removeOverlap) = 0;

  virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETAssociationMap* map,
				   std::vector<const xAOD::IParticle*>& uniques,
				   xAOD::MissingET* metSoftClus,
				   const xAOD::MissingET* coreSoftClus,
				   xAOD::MissingET* metSoftTrk,
				   const xAOD::MissingET* coreSoftTrk) = 0;

  virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
				   const xAOD::JetContainer* jets,
				   const xAOD::MissingETAssociationMap* map,
				   xAOD::MissingET* metSoftClus,
				   const xAOD::MissingET* coreSoftClus,
				   xAOD::MissingET* metSoftTrk,
				   const xAOD::MissingET* coreSoftTrk) = 0;

  virtual StatusCode buildMETSum(const std::string& totalName,
				 xAOD::MissingETContainer* metCont,
				 MissingETBase::Types::bitmask_t softTermsSource) = 0;
};
	
#endif
