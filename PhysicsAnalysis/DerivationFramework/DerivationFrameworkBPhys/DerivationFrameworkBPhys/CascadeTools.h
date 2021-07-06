/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DERIVATIONFRAMEWORKBPHYS_CASCADETOOLS_H
#define DERIVATIONFRAMEWORKBPHYS_CASCADETOOLS_H
//*********************
// CascadeTools header file
//
// Eva Bouhova <e.bouhova@cern.ch>
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "xAODTracking/Vertex.h"


namespace DerivationFramework {

  static const InterfaceID IID_CascadeTools("CascadeTools", 1, 1);

  class CascadeTools : public AthAlgTool{

    public:

/**
 * Default constructor due to Athena interface
 */
    CascadeTools(const std::string& t, const std::string& n, const IInterface* p);
    
/**
 * Virtual destructor
 */
    ~CascadeTools();

/**
 * Standard AlgTool methods
 */
    //Nothing done not needed
    //StatusCode initialize() override;
    //StatusCode finalize() override;

/**   
 * AlgTool interface methods 
 */
  static const InterfaceID& interfaceID()
  {
   return IID_CascadeTools;
  }


    Amg::Vector3D momentum(const std::vector<TLorentzVector> &particleMom) const;
    Amg::Vector3D pca(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;

    double invariantMass(const std::vector<TLorentzVector> &moms, const std::vector<double> &masses) const;
    double invariantMass(const std::vector<TLorentzVector> &moms) const;
    double invariantMassError(const std::vector<TLorentzVector> &moms,  const Amg::MatrixX& cov, const std::vector<double> &masses) const;
    double invariantMassError(const std::vector<TLorentzVector> &moms,  const Amg::MatrixX& cov) const;
    double pT(const std::vector<TLorentzVector> &moms) const;
    double pTError(const std::vector<TLorentzVector> &moms, const Amg::MatrixX& cov) const;
    double lxy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double lxyError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tau(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tauError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tau(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const;
    double tauError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const;
    double a0z(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0zError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0xy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0xyError(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0Error(const std::vector<TLorentzVector> &particleMom, const Amg::MatrixX& cov, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double cosTheta(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double cosTheta_xy(const std::vector<TLorentzVector> &particleMom, const xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double massProbability(double V0Mass, double mass, double massErr) const;
    double vertexProbability(int ndf, double chi2) const;

    Amg::MatrixX * convertCovMatrix(const xAOD::Vertex * vxCandidate) const;
    Amg::MatrixX SetFullMatrix(int NTrk, const std::vector<float> & Matrix) const;

  //private:

  }; //end of class definitions

} //end of namespace definitions


#endif
