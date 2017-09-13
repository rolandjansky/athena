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
    StatusCode initialize() override;
    StatusCode finalize();

/**   
 * AlgTool interface methods 
 */
  static const InterfaceID& interfaceID()
  {
   return IID_CascadeTools;
  }


    Amg::Vector3D Momentum(std::vector<TLorentzVector> particleMom) const;
    Amg::Vector3D pca(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;

    double invariantMass(std::vector<TLorentzVector> moms, std::vector<double> masses) const;
    double invariantMass(std::vector<TLorentzVector> moms) const;
    double invariantMassError(std::vector<TLorentzVector> moms,  const Amg::MatrixX& cov, std::vector<double> masses) const;
    double invariantMassError(std::vector<TLorentzVector> moms,  const Amg::MatrixX& cov) const;
    double pT(std::vector<TLorentzVector> moms) const;
    double pTError(std::vector<TLorentzVector> moms, const Amg::MatrixX& cov) const;
    double lxy(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double lxyError(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tau(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tauError(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double tau(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const;
    double tauError(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV, double M) const;
    double a0z(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0zError(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0xy(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0xyError(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double a0Error(std::vector<TLorentzVector> particleMom, const Amg::MatrixX& cov, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double cosTheta(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double cosTheta_xy(std::vector<TLorentzVector> particleMom, xAOD::Vertex* SV, const xAOD::Vertex* PV) const;
    double massProbability(double V0Mass, double mass, double massErr) const;
    double vertexProbability(int ndf, double chi2) const;

    Amg::MatrixX * convertCovMatrix(xAOD::Vertex * vxCandidate) const;
    Amg::MatrixX SetFullMatrix(int NTrk, std::vector<float> & Matrix) const;

  //private:

  }; //end of class definitions

} //end of namespace definitions


#endif
