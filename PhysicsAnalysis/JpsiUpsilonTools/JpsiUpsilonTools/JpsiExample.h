/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"

class TFile;
class TTree;
class TVector3;
class TLorentzVector;

/////////////////////////////////////////////////////////////////////////////
class JpsiExample : public AthAlgorithm {

public:
  JpsiExample (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

private:

  void initializeBranches(void);
  void clearBranches(void);

  // The following methods will eventually be implemented in new "V0Tools". For now,
  // we will keep them here. 
  TVector3       trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const;
  TLorentzVector track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
  TVector3       origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
  TLorentzVector origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
  double         invariantMassError(const xAOD::Vertex* vxCandidate, const std::vector<double> &masses) const;
  double         massErrorVKalVrt(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const;
  double         trackCharge(const xAOD::Vertex * vxCandidate, int i) const;
  Amg::MatrixX*  convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const;
  double m_muonMass;
  std::string  m_userFName;
 
  int m_eventCntr;
  int m_jpsiCntr;
 
  std::vector<double> * m_jpsiMass;
  std::vector<double> * m_jpsiMassError;
  std::vector<double> * m_jpsiMassRec;
  std::vector<double> * m_jpsiMassPullRec;
  std::vector<double> * m_jpsiMassPullMC;
  std::vector<double> * m_jpsiChi2;

//  std::vector<double> * m_trkRefitCharge1;
  std::vector<double> * m_trkRefitPx1;
  std::vector<double> * m_trkRefitPy1;
  std::vector<double> * m_trkRefitPz1;
//  std::vector<double> * m_trkRefitCharge2;
  std::vector<double> * m_trkRefitPx2;
  std::vector<double> * m_trkRefitPy2;
  std::vector<double> * m_trkRefitPz2;

  std::vector<double> * m_vx;
  std::vector<double> * m_vy;
  std::vector<double> * m_vz;
  
  std::vector<double> * m_trkOrigCharge1;
  std::vector<double> * m_trkOrigPx1;
  std::vector<double> * m_trkOrigPy1;
  std::vector<double> * m_trkOrigPz1;
  std::vector<double> * m_trkOrigCharge2;
  std::vector<double> * m_trkOrigPx2;
  std::vector<double> * m_trkOrigPy2;
  std::vector<double> * m_trkOrigPz2;

//  std::vector<double> * m_rxyError;
  

  TFile* m_outputFile; // N-tuple output file
  TTree* m_auxTree; // Tree for auxilliary n-tuple

  SG::ReadHandleKey<xAOD::VertexContainer> m_JpsiCandidatesKey; //!< Name of J/psi container
  SG::ReadDecorHandleKey<xAOD::VertexContainer> m_refPX;
  SG::ReadDecorHandleKey<xAOD::VertexContainer> m_refPY;
  SG::ReadDecorHandleKey<xAOD::VertexContainer> m_refPZ;

};
 
