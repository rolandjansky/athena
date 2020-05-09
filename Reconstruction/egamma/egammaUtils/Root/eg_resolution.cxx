/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <stdexcept>
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Egamma.h"
#include "egammaUtils/eg_resolution.h"
#include "PathResolver/PathResolver.h"

template<typename T>
T* get_object(TFile& file, const std::string& name){
  T* obj = dynamic_cast<T*>(file.Get(name.c_str()));
  if (not obj) { throw std::runtime_error("object " + name + " not found"); }
  return obj;
}

eg_resolution::eg_resolution(const std::string& configuration)
  : asg::AsgMessaging("eg_resolution"),
    m_file0(),
    m_file1(),
    m_file2(),
    m_file3()
{
  ATH_MSG_INFO("Initialize eg_resolution");
  
  if (configuration == "run1") {
    m_file0 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run1.root").c_str() );
    m_file1 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_recoUnconv_run1.root").c_str() );
    m_file2 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_recoConv_run1.root").c_str() );
    m_file3 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_trueUnconv_run1.root").c_str() );
  }
  else if (configuration == "run2_pre") {
    m_file0 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run2_pre.root").c_str());
    m_file1 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_recoUnconv_run2_pre.root").c_str());
    m_file2 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_recoConv_run2_pre.root").c_str());
    m_file3 = std::make_unique<TFile> (PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_trueUnconv_run2_pre.root").c_str());
  }

  if (!m_file0 or !m_file1 or !m_file2 or !m_file3) {
    ATH_MSG_ERROR("cannot find input file for resolutions");
    throw std::runtime_error("cannot find input file for resolutions");
  }

  m_hSampling[0][0] = get_object<TH1>(*m_file0, "hsamplingG");
  m_hSampling[0][1] = get_object<TH1>(*m_file0, "hsampling80");
  m_hSampling[0][2] = get_object<TH1>(*m_file0, "hsampling90");
  m_hSampling[1][0] = get_object<TH1>(*m_file1, "hsamplingG");
  m_hSampling[1][1] = get_object<TH1>(*m_file1, "hsampling80");
  m_hSampling[1][2] = get_object<TH1>(*m_file1, "hsampling90");
  m_hSampling[2][0] = get_object<TH1>(*m_file2, "hsamplingG");
  m_hSampling[2][1] = get_object<TH1>(*m_file2, "hsampling80");
  m_hSampling[2][2] = get_object<TH1>(*m_file2, "hsampling90");
  m_hSampling[3][0] = get_object<TH1>(*m_file3, "hsamplingG");
  m_hSampling[3][1] = get_object<TH1>(*m_file3, "hsampling80");
  m_hSampling[3][2] = get_object<TH1>(*m_file3, "hsampling90");

  m_hNoise[0][0] = get_object<TH1>(*m_file0, "hnoiseG");
  m_hNoise[0][1] = get_object<TH1>(*m_file0, "hnoise80");
  m_hNoise[0][2] = get_object<TH1>(*m_file0, "hnoise90");
  m_hNoise[1][0] = get_object<TH1>(*m_file1, "hnoiseG");
  m_hNoise[1][1] = get_object<TH1>(*m_file1, "hnoise80");
  m_hNoise[1][2] = get_object<TH1>(*m_file1, "hnoise90");
  m_hNoise[2][0] = get_object<TH1>(*m_file2, "hnoiseG");
  m_hNoise[2][1] = get_object<TH1>(*m_file2, "hnoise80");
  m_hNoise[2][2] = get_object<TH1>(*m_file2, "hnoise90");
  m_hNoise[3][0] = get_object<TH1>(*m_file3, "hnoiseG");
  m_hNoise[3][1] = get_object<TH1>(*m_file3, "hnoise80");
  m_hNoise[3][2] = get_object<TH1>(*m_file3, "hnoise90");

  m_hConst[0][0] = get_object<TH1>(*m_file0, "hconstG");
  m_hConst[0][1] = get_object<TH1>(*m_file0, "hconst80");
  m_hConst[0][2] = get_object<TH1>(*m_file0, "hconst90");
  m_hConst[1][0] = get_object<TH1>(*m_file1, "hconstG");
  m_hConst[1][1] = get_object<TH1>(*m_file1, "hconst80");
  m_hConst[1][2] = get_object<TH1>(*m_file1, "hconst90");
  m_hConst[2][0] = get_object<TH1>(*m_file2, "hconstG");
  m_hConst[2][1] = get_object<TH1>(*m_file2, "hconst80");
  m_hConst[2][2] = get_object<TH1>(*m_file2, "hconst90");
  m_hConst[3][0] = get_object<TH1>(*m_file3, "hconstG");
  m_hConst[3][1] = get_object<TH1>(*m_file3, "hconst80");
  m_hConst[3][2] = get_object<TH1>(*m_file3, "hconst90");

  TAxis* aa=m_hSampling[0][0]->GetXaxis();
  m_etaBins = aa->GetXbins();
}

//=========================================================================
eg_resolution::~eg_resolution(){
}

/*
 * inputs are 
 * particle_type (0=elec, 1=reco unconv photon, 2=reco conv photon, 3=true unconv photon)
 * energy in MeV
 * eta
 * resolution_type (0=gaussian core fit, 1=sigmaeff 80%, 2=sigma eff 90%)
 * returned value is sigmaE/E
*/
double eg_resolution::getResolution(int particle_type, double energy, double eta, int resolution_type) const
{

   if (particle_type<0 || particle_type>3) {
     ATH_MSG_ERROR("particle type must be 0, 1, 2 or 3");
     throw std::runtime_error("particle type must be 1, 2 or 3");
   }

   if (resolution_type<0 || resolution_type>2) {
     ATH_MSG_ERROR("resolution type must be 0, 1, 2");
     throw std::runtime_error("resolution type must be 0, 1, 2");
   }

   const float aeta = fabs(eta);
   int ibinEta = m_etaBins->GetSize() - 2;
   for (int i = 1; i < m_etaBins->GetSize(); ++i) {
     if (aeta < m_etaBins->GetAt(i)) {
         ibinEta = i - 1;
         break;
      }
   }

   if (ibinEta<0 || ibinEta>= m_etaBins->GetSize()) {
     ATH_MSG_ERROR("eta outside range");
     throw std::runtime_error("eta outside range");
   }

   const double energyGeV = energy * 1E-3;
   const double rsampling = m_hSampling[particle_type][resolution_type]->GetBinContent(ibinEta + 1);
   const double rnoise    = m_hNoise[particle_type][resolution_type]->GetBinContent(ibinEta + 1);
   const double rconst    = m_hConst[particle_type][resolution_type]->GetBinContent(ibinEta + 1);
   const double sigma2 = rsampling*rsampling/energyGeV + rnoise*rnoise/energyGeV/energyGeV + rconst*rconst;
   return sqrt(sigma2);
}


double eg_resolution::getResolution(const xAOD::Egamma& particle, int resolution_type) const
{
  int particle_type = -1;
  if (particle.type() == xAOD::Type::Electron) {
    particle_type = 0;
  } else if (particle.type() == xAOD::Type::Photon) {
    const xAOD::Photon* ph = static_cast<const xAOD::Photon*> (&particle);
    const xAOD::Vertex* phVertex = ph->vertex();
    if (phVertex) {
      const Amg::Vector3D& pos = phVertex->position();
      const double Rconv = static_cast<float>(hypot(pos.x(), pos.y()));
      if (Rconv > 0 and Rconv < 800) { particle_type = 2; }
      else { particle_type = 1; }
    } else {
      particle_type = 1;
    }
  }
  assert (particle_type != 1);

  // TODO: check definitions
  const double eta = particle.caloCluster()->eta();
  const double energy = particle.e();
  return getResolution(particle_type, energy, eta, resolution_type);
}
