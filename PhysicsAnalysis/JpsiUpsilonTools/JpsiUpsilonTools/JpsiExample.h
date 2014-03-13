/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
#include <list>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"     // Handy toolbox for calculating values from ExtendedVxCandidates
#include "HepPDT/ParticleDataTable.hh"


class StoreGateSvc;
class IHistogram1D;
class TFile;
class TTree;

/////////////////////////////////////////////////////////////////////////////
class JpsiExample : public AthAlgorithm {

public:
  JpsiExample (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  void initializeBranches(void);
  void clearBranches(void);

  bool m_overwriteMass;
  double m_muonMass;
  const HepPDT::ParticleDataTable *m_particleDataTable;
  std::string  m_userFName;
 
  int eventCntr;
  int jpsiCntr;
 
  std::vector<double> * m_jpsiMass;
  std::vector<double> * m_jpsiMassError;
  std::vector<double> * m_jpsiMassRec;
  std::vector<double> * m_jpsiMassPullRec;
  std::vector<double> * m_jpsiMassPullMC;
  std::vector<double> * m_jpsiChi2;

  std::vector<double> * m_trkRefitCharge1;
  std::vector<double> * m_trkRefitPx1;
  std::vector<double> * m_trkRefitPy1;
  std::vector<double> * m_trkRefitPz1;
  std::vector<double> * m_trkRefitCharge2;
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

  std::vector<double> * m_rxyError;
  

  TFile* outputFile; // N-tuple output file
  TTree* auxTree; // Tree for auxilliary n-tuple

  ToolHandle<Trk::V0Tools> m_V0Tools;
  std::string m_JpsiCandidatesKey; //!< Name of J/psi container

  double getInvariantMass(std::vector<const Rec::TrackParticle*>,std::vector<double> );

};
 
