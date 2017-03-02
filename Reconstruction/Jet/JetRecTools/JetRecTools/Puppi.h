#include "fastjet/internal/base.hh"
#include "fastjet/PseudoJet.hh"

#ifndef JETRECTOOLS_PUPPI_h
#define JETRECTOOLS_PUPPI_h

using namespace std;
using namespace fastjet;

class Puppi{

 public:
  // default ctor
  Puppi(double R0, double Rmin, double exp, double sfa, double sfb, double m_etaForward, double forwardSF);

  void SetParticles(std::vector<fastjet::PseudoJet> chargedHS, std::vector<fastjet::PseudoJet> chargedPU, std::vector<fastjet::PseudoJet> neutralAndForward, int NPV);

  //Note that this currently treats everything like it is a neutral or forward
  std::vector<PseudoJet> operator()(const std::vector<PseudoJet> & neutralsAndForward){
    std::vector<fastjet::PseudoJet> cNeutralsAndForward = neutralsAndForward;
    apply(cNeutralsAndForward);
    return cNeutralsAndForward;
  }

  // Returns the weight that would be applied to a particle (assuming it is neutral or forward)
  double getWeight(const PseudoJet& centre);

  // Returns the value of alpha for this particle
  double getAlpha(const PseudoJet& centre);



 private:

  void findMedianAndRMS();
  void apply(std::vector<fastjet::PseudoJet> & neutralAndForward);

  std::vector<PseudoJet> m_alphaParticles; //! The particles used to calculate alpha (the HS charged and forward)
  std::vector<PseudoJet> m_puChargedParticles; //! The particles used to calculate the median and RMS

  double m_R0;
  double m_Rmin;
  double m_exponent;
  double m_scaleFactorA;
  double m_scaleFactorB;
  double m_forwardSF;
  double m_etaForward;

  double m_PU_median;
  double m_PU_RMS;

  double m_NPV;

};

#endif 
//FASTJET_END_NAMESPACE
