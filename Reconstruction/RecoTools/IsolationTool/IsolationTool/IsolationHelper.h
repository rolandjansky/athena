/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODMuon/MuonContainer.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#ifdef XAOD_ANALYSIS
#include "IsolationSelection/IIsolationSelectionTool.h"
#endif // XAOD_ANALYSIS

#ifndef XAOD_ANALYSIS
#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"
// #include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#endif // XAOD_ANALYSIS

struct strObj;

// struct strObj{
// 	float pt;
// 	float eta;
// 	std::vector< float > isolationValues;
// };

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
  bool removeOverlap(std::vector<float>& results, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, float trackRadiusMin = 0.0, float caloRadiusMin = 0.0, int topoetconeModel = -1, const xAOD::Vertex* vertex = 0, const xAOD::CaloClusterContainer* topoClusters = 0) const;
  void setCoreSubtraction(bool isCoreSubtracted) { m_isCoreSubtracted = isCoreSubtracted; }
//#ifdef ROOTCORE
#ifdef XAOD_ANALYSIS
  virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, const std::vector<xAOD::Iso::IsolationType>& isoTypes, float trackRadiusMin = 0.0, float caloRadiusMin = 0.0, int topoetconeModel = -1, const xAOD::Vertex* vertex = 0, const xAOD::CaloClusterContainer* topoClusters = 0) const;
  virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x) const;
  bool setSelectorProperty(const char* field, const char* property);
#endif
//#endif
  
 private:
  bool correction(float& value, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, xAOD::Iso::IsolationCaloCorrection corr, xAOD::Iso::IsolationCaloCorrectionBitset corrMask) const;
  const xAOD::IParticle* getReferenceParticle(const xAOD::IParticle& par) const;
  bool calculateRemovalTrackIso(float& removal, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, const xAOD::Vertex* vertex, float coneSize, float trackRadiusMin = 0.0) const;
  bool calculateRemovalTopoetcone(float& removal, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, float coneSize, float caloRadiusMin = 0.0, int modelOption = -1, const xAOD::CaloClusterContainer* topoClusters = 0) const;
  const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;
  bool getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const;
  
  /// hepler function to ensure phi is within +-Pi
  inline float phiInRange(float phi) const { 
	while (phi >= M_PI) phi -= 2*M_PI;
	while (phi < -M_PI) phi += 2*M_PI;
	return phi;
  }
  
  /// hepler function for maximum
  inline float getMax(float f1, float f2) const { 
	if(f1 >= f2) {
	    return f1;
	} else {
	    return f2;
	}
  }
  
  /// hepler function for minimum
  inline float getMin(float f1, float f2) const { 
	if(f1 <= f2) {
	    return f1;
	} else {
	    return f2;
	}
  }
  
  /** retrieve track particles */
  const xAOD::TrackParticleContainer* retrieveTrackParticleContainer() const;
  
#ifdef XAOD_ANALYSIS // particlesInCone tool will not be avaible. Write our own...
    bool getparticlesInCone( float eta, float phi, float dr, std::vector< const xAOD::TrackParticle*>& output ) const;
#endif // XAOD_ANALYSIS

  float m_overlapCone;
  float m_coreCone;
  float m_ptvarconeRadius;
#ifdef XAOD_ANALYSIS
  mutable Root::TAccept m_accept;
#endif // XAOD_ANALYSIS
  bool m_isInitialised;
  xAOD::Iso::IsolationCaloCorrectionBitsetHelper m_caloIsoBitHelper;
  //xAOD::Iso::IsolationTrackCorrectionBitsetHelper m_trkIsoBitHeler;
#ifndef XAOD_ANALYSIS
  ToolHandle<xAOD::ITrackParticlesInConeTool> m_tracksInConeTool; /// tracks in cone tool
// //  ToolHandle<Trk::IParticleCaloExtensionTool>  m_caloExtTool;
#endif // XAOD_ANALYSIS

#ifdef XAOD_ANALYSIS
   ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;
#endif // XAOD_ANALYSIS


  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool; /// selection of tracks

//   ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;
  

  bool m_isCoreSubtracted;
  std::string m_indetTrackParticleLocation; /// track particle location

};

}
#endif
