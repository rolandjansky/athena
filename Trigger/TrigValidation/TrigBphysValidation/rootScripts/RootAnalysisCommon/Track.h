/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************
// Track.h
// Simple representation of a track
// Can be created from the n-tuple tree
// made by GetTrack in BPhysAnalysisTools
//***************************************

class Track {
  
private:
  int m_event;
  int m_lumiBlock;
  int m_run;
  double m_px;
  double m_py;
  double m_pz;
  double m_pt;
  double m_eta;
  double m_d0;
  double m_z0;
  double m_phi0;             
  double m_theta;
  double m_qOverP;
  double m_d0Err;
  double m_z0Err;
  double m_phi0Err;             
  double m_thetaErr;
  double m_qOverPErr;
  double m_chi2;
  int m_ndf;
  int m_nBlayer;
  int m_nPix;
  int m_nSCT;
  int m_nTRT;
  int m_index;
  int m_vtxIndex;

public:
  // Constructor
  Track(void) {
      m_event = 0;
      m_lumiBlock = 0;
      m_run = 0;
      m_px = 0.0;
      m_py = 0.0;
      m_pz = 0.0;
      m_pt = 0.0;
      m_eta = 0.0;
      m_d0 = 0.0;
      m_z0 = 0.0;
      m_theta = 0.0;
      m_phi0 = 0.0;             
      m_qOverP = 0.0;
      m_d0Err = 0.0;
      m_z0Err = 0.0;
      m_thetaErr = 0.0;
      m_phi0Err = 0.0;             
      m_qOverPErr = 0.0;
      m_chi2 = 0.0;
      m_ndf = 0;
      m_nBlayer = 0;
      m_nPix = 0;
      m_nSCT = 0;
      m_nTRT = 0;
      m_index = 0;
      m_vtxIndex = 0;
  }
  // Set methods
  void setEventNumber(int input) {m_event = input;}
  void setLumiBlock(int input) {m_lumiBlock = input;}
  void setRunNumber(int input) {m_run = input;}
  void setPx(double input) {m_px = input;}
  void setPy(double input) {m_py = input;}
  void setPz(double input) {m_pz = input;}
  void setPt(double input) {m_pt = input;}
  void setEta(double input) {m_eta = input;}
  void setD0(double input) {m_d0 = input;}
  void setZ0(double input) {m_z0 = input;}
  void setPhi0(double input) {m_phi0 = input;}
  void setTheta(double input) {m_theta = input;}
  void setQOverP(double input) {m_qOverP = input;}
  void setD0Err(double input) {m_d0Err = input;}
  void setZ0Err(double input) {m_z0Err = input;}
  void setPhi0Err(double input) {m_phi0Err = input;}
  void setThetaErr(double input) {m_thetaErr = input;}
  void setQOverPErr(double input) {m_qOverPErr = input;}
  void setChi2(double input) {m_chi2 = input;}
  void setNDF(int input) {m_ndf = input;}
  void setNBLayer(int input) {m_nBlayer = input;}
  void setNPix(int input) {m_nPix = input;}
  void setNSCT(int input) {m_nSCT = input;}
  void setNTRT(int input) {m_nTRT = input;}
  void setIndex(int input) {m_index = input;}
  void setVtxIndex(int input) {m_vtxIndex = input;}

  // Get methods
  int eventNumber(void) {return m_event;}
  int lumiBlock(void) {return m_lumiBlock;}
  int runNumber(void) {return m_run;}
  double px(void) {return m_px;}
  double py(void) {return m_py;}
  double pz(void) {return m_pz;}
  double pt(void) {return m_pt;}
  double eta(void) {return m_eta;}
  double d0(void) {return m_d0;}
  double z0(void) {return m_z0;}
  double theta(void) {return m_theta;}
  double phi0(void) {return m_phi0;}
  double qOverP(void) {return m_qOverP;}
  double d0Err(void) {return m_d0Err;}
  double z0Err(void) {return m_z0Err;}
  double thetaErr(void) {return m_thetaErr;}
  double phi0Err(void) {return m_phi0Err;}
  double qOverPErr(void) {return m_qOverPErr;}
  double chi2(void) {return m_chi2;}
  int ndf(void) {return m_ndf;}
  int nBLayer(void) {return m_nBlayer;}
  int nPix(void) {return m_nPix;}
  int nSCT(void) {return m_nSCT;}
  int nTRT(void) {return m_nTRT;}
  int index(void) {return m_index;}
  int vtxIndex(void) {return m_vtxIndex;}

}; // End of class definition 
