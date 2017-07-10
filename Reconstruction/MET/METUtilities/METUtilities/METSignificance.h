///////////////////////// -*- C++ -*- /////////////////////////////
// METSignificance.h
// Header file for class METSignificance
// Author: P. Francavilla<francav@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METSIGNIFICANCE_H
#define METUTILITIES_MET_METSIGNIFICANCE_H 1

// STL includes
#include <string>
#include <tuple>
//#include <TMatrixD.h>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METInterface includes
#include "METInterface/IMETSignificance.h"

// EDM includes
#include "xAODJet/JetContainer.h"

// Tracking Tool
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Tool interfaces
class IJERTool;
namespace CP {
  class IMuonCalibrationAndSmearingTool;
  class IEgammaCalibrationAndSmearingTool;
}

namespace TauAnalysisTools {
  class ITauSmearingTool;
}

// Forward declaration
namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METSignificance
  : public asg::AsgTool,
  virtual public IMETSignificance

  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METSignificance, IMETSignificance)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    METSignificance(const std::string& name);

    /// Destructor:
    virtual ~METSignificance();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
      
    StatusCode varianceMET(xAOD::MissingETContainer* metCont);


    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:

    /// Default constructor:
    METSignificance();

    // tools
    ToolHandle<IJERTool> m_jerTool;
    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;
    ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_egammaCalibTool;
    ToolHandle<TauAnalysisTools::ITauSmearingTool> m_tauSmearingTool;

    double GetPUProb(double jet_eta, double jet_phi,double jet_pt,  double jet_jvt);
    double GetPhiUnc(double jet_eta, double jet_phi,double jet_pt);

    std::tuple<double,double,double> CovMatrixRotation(double var_x, double var_y, double cv_xy, double Phi);
    //TMatrixD MatrixRotation(TMatrixD CovM_xy , double Phi);

    double Significance_LT(double Numerator, double var_parall, double var_perpen, double cov);

    double m_GeV;

    int  m_softTermParam;
    bool m_treatPUJets;
    bool m_doPhiReso;

    bool m_isData;
    bool m_isAFII;

  };

} //> end namespace met
#endif //> !METUTILITIES_MET_METSIGNIFICANCE_H
