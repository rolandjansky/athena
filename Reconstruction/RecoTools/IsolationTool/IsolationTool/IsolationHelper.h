/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IsolationTool_IsolationHelper_H
#define IsolationTool_IsolationHelper_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "IsolationTool/IIsolationHelper.h"
#include "xAODPrimitives/IsolationType.h"
#include <xAODPrimitives/IsolationCorrection.h>
#include <xAODPrimitives/IsolationCorrectionHelper.h>
#include <xAODBase/IParticle.h>
#include <xAODEventShape/EventShape.h>
#include "xAODCore/ShallowCopy.h"

namespace CP {

class IsolationHelper : public asg::AsgTool,
	public virtual IIsolationHelper {
 /// Create a constructor for Athena
 ASG_TOOL_CLASS( IsolationHelper, IIsolationHelper )
 public:
  // this is a standard constructor
  IsolationHelper (const std::string& name);
  virtual ~IsolationHelper (){};
  
  /** initialize */
  virtual StatusCode initialize();
      
  /** finalize */
  virtual StatusCode finalize();

  bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const;
  bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const;
  bool correctionBitset(xAOD::Iso::IsolationCaloCorrectionBitset& mask, const xAOD::IParticle& par, xAOD::Iso::IsolationType type) const;
  bool isolation(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<xAOD::Iso::IsolationCaloCorrection>& corrs) const;
  bool correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const;
  bool getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const;
  bool getPileupCorrection(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, float coreArea) const;
  bool updateIsolation(xAOD::MuonContainer*& copy,xAOD::ShallowAuxContainer*& copyaux, std::vector<xAOD::Iso::IsolationType>& types, xAOD::Iso::IsolationCaloCorrectionBitset corrMask, std::string muonkey, bool recordSG) const;
  
 private:
  bool correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrection corr, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const;

  float m_coreCone;
  //xAOD::Iso::IsolationTrackCorrectionBitsetHelper m_trkIsoBitHeler;

};

}
#endif
