#include "Math/ProbFunc.h"
#include "TMath.h"
#include "fastjet/Selector.hh"

#include "JetRecTools/Puppi.h"

using namespace std;
using namespace fastjet;

Puppi::Puppi(double R0, double Rmin, double exponent, double scaleFactorA, double scaleFactorB, double etaForward, double forwardSF):
  m_R0(R0), m_Rmin(Rmin), m_exponent(exponent), m_scaleFactorA(scaleFactorA), m_scaleFactorB(scaleFactorB), m_forwardSF(forwardSF), m_etaForward(etaForward) 
{}

//Sets the particles used for calculations, and calculates the median and RMS for the PU distribution
void Puppi::SetParticles(std::vector<fastjet::PseudoJet> chargedHS, std::vector<fastjet::PseudoJet> chargedPU, std::vector<fastjet::PseudoJet> neutralAndForward, int NPV){
  m_NPV=NPV;

  // Used to calculate weights for all other particles in the event
  m_alphaParticles = chargedHS;
  for(auto p: neutralAndForward){
    if(abs(p.eta()) > m_etaForward) m_alphaParticles.push_back(p);
  }

  // Uses charged and neutral containers to find the LHS RMS for the PU alpha distribution
  m_puChargedParticles = chargedPU;
  findMedianAndRMS();
}

/*
// Rescale all particles by their weight
void Puppi::apply(std::vector<fastjet::PseudoJet> & neutralAndForward) {

  for(int i=0; i < neutralAndForward.size(); i++){
    double weight = getWeight(neutralAndForward[i]);
    neutralAndForward[i] *= weight;
  }
}
*/

double Puppi::getWeight(const PseudoJet& centre){
  double alpha = getAlpha(centre);
  double lVal = (alpha-m_PU_median) / m_PU_RMS;
  double pWeight = 1;
  double pt = centre.pt();
        
  pWeight *=  ROOT::Math::chisquared_cdf( lVal*fabs(lVal), 1.);

  if( fabs(centre.eta()) > m_etaForward) pWeight *= m_forwardSF; 
  if( (m_scaleFactorA + m_scaleFactorB * m_NPV) >  pWeight * pt) pWeight = 0;

  return pWeight;
}

double Puppi::getAlpha(const PseudoJet& centre){
  fastjet::Selector sel = fastjet::SelectorCircle(m_R0);
  sel.set_reference(centre);
  vector<PseudoJet> near_particles = sel(m_alphaParticles);
  double var = 0;

  for(auto nearParticle: near_particles){
    float dR=centre.delta_R(nearParticle);
    if(dR==0) continue;
    if(dR<m_Rmin) continue;

    var += pow(nearParticle.pt()/dR, m_exponent);
  }

  // Can't take log of 0
  if(var < 0.00001) return 0;

  var = log(var);
  return var;
}

//Finds the median and LHS RMS for the charged PU distribution (done each event)
void Puppi::findMedianAndRMS(){
  vector<double> alphaVec;
  m_PU_median = 0;
  m_PU_RMS = 0;

  //Find alpha for all PU particles
  for(auto chargedPU: m_puChargedParticles){
    // Don't want to include particles on the boundary
    if( abs(chargedPU.eta()) > m_etaForward - m_R0) continue;

    double alpha = getAlpha(chargedPU);
    if(alpha > 0) alphaVec.push_back(alpha);
  }

  //Get the median of the alpha distribution
  int lRMSPUctr = 0;
  std::sort (alphaVec.begin(),alphaVec.end());
  if(alphaVec.size() > 0) m_PU_median = alphaVec[int( alphaVec.size()*0.5 + 0.5 )];

  // now compute the LHS RMS
  for(auto alpha: alphaVec){
    if (alpha - m_PU_median > 0) continue;
    m_PU_RMS += (alpha - m_PU_median)*(alpha - m_PU_median);
    lRMSPUctr++;
  }

  if(alphaVec.size() > 0)  m_PU_RMS = TMath::Sqrt(m_PU_RMS/lRMSPUctr);
}