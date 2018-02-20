/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKSectorSlice_h
#define FTKSectorSlice_h

#include <TROOT.h>
#include <TrigFTKSim/FTKTrack.h>

class FTKSectorSlice {

 public:
  FTKSectorSlice();
   ~FTKSectorSlice(){};

  void loadSlices(std::string slice_file_path);
  double getAutotunePhiMin();
  double getAutotuneCMin();
  double getAutotuneDMin();
  double getAutotuneZ0Min();
  double getAutotuneCthetaMin();
  double getAutotuneEtaMin();
  double getAutotunePhiMax();
  double getAutotuneCMax();
  double getAutotuneDMax();
  double getAutotuneZ0Max();
  double getAutotuneCthetaMax();
  double getAutotuneEtaMax();

  // In case user wants to set ranges manually
  void setPhiMin(const double &v) { m_phi_min = v; }
  void setPhiMax(const double &v) { m_phi_max = v; }
  void setCMin(const double &v) { m_c_min = v; }
  void setCMax(const double &v) { m_c_max = v; }
  void setD0Min(const double &v) { m_d0_min = v; }
  void setD0Max(const double &v) { m_d0_max = v; }
  void setZ0Min(const double &v) { m_z0_min = v; }
  void setZ0Max(const double &v) { m_z0_max = v; }
  void setEtaMin(const double &v) { m_eta_min = v; m_ctheta_min = TMath::SinH(v); }
  void setEtaMax(const double &v) { m_eta_max = v; m_ctheta_max = TMath::SinH(v); }

  // JW - to be used if the user inputs parameter ranges by hand after loading the slices
  void resetStepSizes() {
    m_step_phi = (m_phi_max - m_phi_min)/m_phi_nbins;
    autotune(bits_phi,m_phi_min,m_phi_max,m_autotune_phi_min,m_autotune_phi_max,true,"phi");
    m_step_c = (m_c_max - m_c_min)/m_c_nbins;
    autotune(bits_c,m_c_min,m_c_max,m_autotune_c_min,m_autotune_c_max,false,"c");
    m_step_d0 = (m_d0_max - m_d0_min)/m_d0_nbins;
    autotune(bits_d,m_d0_min,m_d0_max,m_autotune_d0_min,m_autotune_d0_max,false,"d0");
    m_step_z0 = (m_z0_max - m_z0_min)/m_z0_nbins;
    autotune(bits_z0,m_z0_min,m_z0_max,m_autotune_z0_min,m_autotune_z0_max,false,"z0");
    m_step_ctheta = (m_ctheta_max - m_ctheta_min)/m_ctheta_nbins;
    autotune(bits_ctheta,m_ctheta_min,m_ctheta_max,m_autotune_ctheta_min,m_autotune_ctheta_max,false,"cos(theta)");
    m_autotune_eta_min=TMath::ASinH(m_autotune_ctheta_min);
    m_autotune_eta_max=TMath::ASinH(m_autotune_ctheta_max);
  }

  std::vector<int> *searchSectors(FTKTrack& track);

 private:
  TClonesArray *bits_phi;
  TClonesArray *bits_c;
  TClonesArray *bits_d;
  TClonesArray *bits_z0;
  TClonesArray *bits_ctheta;

  void autotune(const TClonesArray *array,double_t x0,double_t x1,
                double &autoMin,double &autoMax,
                bool isAzimuth,const char *debug);

  // Min and max parameters found out in the slice file
  Double_t m_phi_min;
  Double_t m_phi_max;
  Double_t m_c_min;
  Double_t m_c_max;
  Double_t m_d0_min;
  Double_t m_d0_max;
  Double_t m_z0_min;
  Double_t m_z0_max;
  Double_t m_ctheta_min;
  Double_t m_ctheta_max;
  Double_t m_eta_min;
  Double_t m_eta_max;
  
  double m_autotune_phi_min;
  double m_autotune_c_min;
  double m_autotune_d0_min;
  double m_autotune_z0_min;
  double m_autotune_ctheta_min;
  double m_autotune_eta_min;
  double m_autotune_phi_max;
  double m_autotune_c_max;
  double m_autotune_d0_max;
  double m_autotune_z0_max;
  double m_autotune_ctheta_max;
  double m_autotune_eta_max;
  
  Int_t m_phi_nbins;
  Int_t m_c_nbins;
  Int_t m_d0_nbins;
  Int_t m_z0_nbins;
  Int_t m_ctheta_nbins;

  Double_t m_step_phi;
  Double_t m_step_c;
  Double_t m_step_d0;
  Double_t m_step_z0;
  Double_t m_step_ctheta;

  // Flag to use ITk geometry
  bool m_ITkMode;

  };

#endif // FTKSectorSlice_h
