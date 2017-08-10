/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch
#ifndef TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
#define TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H 1

// EDM include(s):
#include "xAODEgamma/ElectronFwd.h"
#include "xAODMuon/Muon.h"

// Tool include(s):
#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"

class ITrigGlobalEfficiencyCorrectionTool : public virtual asg::IAsgTool {

public:
	ASG_TOOL_INTERFACE( ITrigGlobalEfficiencyCorrectionTool )

   /// Helper type definition
   typedef std::vector< const xAOD::IParticle* > IParticleConstVector;
   /// Helper type definition
   typedef std::vector< const xAOD::Electron* > ElectronConstVector;
   /// Helper type definition
   typedef std::vector< const xAOD::Muon* > MuonConstVector;

   virtual CP::CorrectionCode
   getEfficiencyScaleFactor( const IParticleConstVector& particles,
                             double& efficiencyScaleFactor ) = 0;
	virtual CP::CorrectionCode
   getEfficiencyScaleFactor( unsigned runNumber,
                             const IParticleConstVector& particles,
                             double& efficiencyScaleFactor ) = 0;
	virtual CP::CorrectionCode
   getEfficiency( const IParticleConstVector& particles, double& efficiencyData,
                  double& efficiencyMc ) = 0;
	virtual CP::CorrectionCode
   getEfficiency( unsigned runNumber, const IParticleConstVector& particles,
                  double& efficiencyData, double& efficiencyMc ) = 0;
	
	virtual CP::CorrectionCode
   getEfficiencyScaleFactor( const ElectronConstVector& electrons,
                             const MuonConstVector& muons,
                             double& efficiencyScaleFactor ) = 0;
	virtual CP::CorrectionCode
   getEfficiencyScaleFactor( unsigned runNumber,
                             const ElectronConstVector& electrons,
                             const MuonConstVector& muons,
                             double& efficiencyScaleFactor ) = 0;
	virtual CP::CorrectionCode
   getEfficiency( const ElectronConstVector& electrons,
                  const MuonConstVector& muons,
                  double& efficiencyData, double& efficiencyMc ) = 0;
	virtual CP::CorrectionCode
   getEfficiency( unsigned runNumber,
                  const ElectronConstVector& electrons,
                  const MuonConstVector& muons,
                  double& efficiencyData, double& efficiencyMc ) = 0;

}; // class ITrigGlobalEfficiencyCorrectionTool

#endif //> !TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
