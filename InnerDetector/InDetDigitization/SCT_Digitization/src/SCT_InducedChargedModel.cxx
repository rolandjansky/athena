/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17 updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

#include "SCT_InducedChargedModel.h"

// ROOT
#include "TH2.h"
#include "TFile.h"

// C++ Standard Library
#include <algorithm>
#include <cmath>
#include <iostream>

const double SCT_InducedChargedModel::s_kB = Gaudi::Units::k_Boltzmann / Gaudi::Units::joule; // [m^2*kg/s^2/K]
const double SCT_InducedChargedModel::s_e = Gaudi::Units::e_SI; // [Coulomb]
const std::vector<float> SCT_InducedChargedModel::s_VFD0 =
  { -180.,  -150.,  -120.,  -90.,  -60.,  -30.,  0.,  30.,  70.};
const std::vector<std::string> SCT_InducedChargedModel::s_VFD0str =
  {"M180", "M150", "M120", "M90", "M60", "M30", "0", "30", "70"};

void SCT_InducedChargedModel::init(const float vdepl, const float vbias, const IdentifierHash hashId, ToolHandle<ISiliconConditionsTool>& siConditionsTool) {

  ATH_MSG_INFO("--- Induced Charged Model Paramter (Begin) --------");

  //---------------setting basic parameters---------------------------

  m_VD = vdepl; // full depletion voltage [Volt] negative for type-P
  m_VB = vbias; // applied bias voltage [Volt]
  m_T = siConditionsTool->temperature(hashId) + Gaudi::Units::STP_Temperature;

  //------------------------ initialize subfunctions ------------

  loadICMParameters();
  
  //------------ find delepletion deph for model=0 and 1 -------------
  m_depletion_depth = m_bulk_depth;
  // for type N (before type inversion)
  if (m_VD >= 0.) {
    if (m_VB < m_VD) m_depletion_depth = std::sqrt(m_VB/m_VD) * m_bulk_depth;
  } else {   
    // for type P
    if (m_VB <= std::abs(m_VD)) m_depletion_depth = 0.;
  }
  
  //--------------------------------------------------------

  ATH_MSG_INFO(" EFieldModel  0(uniform E), 1(Flat Diode Model), 2 (FEM solusions)\t\t"<< m_EFieldModel);
  ATH_MSG_INFO(" DepletionVoltage_VD\t"<< m_VD <<" V");
  ATH_MSG_INFO(" BiasVoltage_VB     \t"<< m_VB  <<" V");
  ATH_MSG_INFO(" MagneticField_B    \t"<< m_B  <<" Tesla");
  ATH_MSG_INFO(" Temperature_T      \t"<< m_T  <<" K");
  ATH_MSG_INFO(" TransportTimeStep  \t"<< m_transportTimeStep <<" ns");
  ATH_MSG_INFO(" TransportTimeMax\t"<< m_transportTimeMax <<" ns");
  ATH_MSG_INFO(" BulkDepth\t\t"<< m_bulk_depth << " cm");
  ATH_MSG_INFO(" DepletionDepth\t\t"<< m_depletion_depth << " cm");
  ATH_MSG_INFO(" StripPitch\t\t"<< m_strip_pitch << " cm");
  ATH_MSG_INFO("--- Induced Charged Model Paramter (End) ---------");

  return;
}

//---------------------------------------------------------------------
//  transport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::transport(const bool isElectron,
                                        const double x0, const double y0,
                                        double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                        const IdentifierHash hashId,
                                        ToolHandle<ISiPropertiesTool>& siPropertiesTool) const {
  // transport electrons and holes in the bulk
  // T. Kondo, 2010.9.9, 2017.7.20
  // External parameters to be specified
  //   m_transportTimeMax  [nsec]
  //   m_transportTimeStep [nsec]
  //   m_bulk_depth        [cm]
  // Induced currents are added to every m_transportTimeStep (defailt is 0.5 ns):
  // Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] NTransportSteps=100
  // means 50 ns storages of charges in each strips. 

  double x = x0; // original electron/hole position [cm]
  double y = y0; // original electron/hole position [cm]
  bool   isInBulk = true;
  double t_current = 0.;
  double qstrip[NStrips]; // induced charges on strips due to an electron or a hole
  double vx, vy, D;
  for (int istrip=StartStrip; istrip<=EndStrip; istrip++) {
    qstrip[istrip+Offset] = (isElectron ? -1. : +1.) * induced(istrip, x, y); // original induced charge by electron or hole
  }
  while (t_current < m_transportTimeMax) {
    if (!isInBulk) break;
    if (!getVxVyD(isElectron, x, y, vx, vy, D, hashId, siPropertiesTool)) break;
    double delta_y = vy * m_transportTimeStep / Gaudi::Units::second; // ns -> s
    y += delta_y;
    double dt = m_transportTimeStep; // [nsec]
    if (isElectron) {
      if (y < m_y_origin_min) {
        isInBulk = false;
        dt = (m_y_origin_min - (y-delta_y))/delta_y * m_transportTimeStep;
        y = m_y_origin_min;
      }
    } else {
      if (y > m_bulk_depth) {
        isInBulk = false; // outside bulk region
        dt = (m_bulk_depth - (y-delta_y))/delta_y * m_transportTimeStep;
        y = m_bulk_depth;
      }
    }
    t_current += dt;
    x += vx * dt / Gaudi::Units::second; // ns -> s
    double diffusion = std::sqrt(2.* D * dt / Gaudi::Units::second); // ns -> s
    y += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
    x += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
    if (isElectron) {
      if (y < m_y_origin_min) {
        y = m_y_origin_min;
        isInBulk = false;
      }
    } else {
      if (y > m_bulk_depth) {
        y = m_bulk_depth;
        isInBulk = false; // outside bulk region
      }
    }

    // get induced current by subtracting induced charges
    for (int istrip=StartStrip; istrip<=EndStrip; istrip++) {
      double qnew = (isElectron ? -1. : +1.) * induced(istrip, x, y);
      int jj = istrip + Offset;
      double dq = qnew - qstrip[jj];
      qstrip[jj] = qnew;

      int jt = static_cast<int>((t_current+0.001) / m_transportTimeStep);
      if (jt < NTransportSteps) {
        switch (istrip) {
        case -2: Q_m2[jt] += dq; break;
        case -1: Q_m1[jt] += dq; break;
        case  0: Q_00[jt] += dq; break;
        case +1: Q_p1[jt] += dq; break;
        case +2: Q_p2[jt] += dq; break;
        } 
      }

    }
  } // end of electron or hole tracing
}

//---------------------------------------------------------------------
//  holeTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::holeTransport(const double x0, const double y0,
                                            double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                            const IdentifierHash hashId,
                                            ToolHandle<ISiPropertiesTool>& siPropertiesTool) const {
  // Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] NTransportSteps=100
  const bool isElectron = false;
  transport(isElectron,
            x0, y0,
            Q_m2, Q_m1, Q_00, Q_p1, Q_p2,
            hashId,
            siPropertiesTool);
}

//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::electronTransport(const double x0, const double y0,
                                                double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                                const IdentifierHash hashId,
                                                ToolHandle<ISiPropertiesTool>& siPropertiesTool) const {
  // Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] NTransportSteps=100
  const bool isElectron = true;
  transport(isElectron,
            x0, y0,
            Q_m2, Q_m1, Q_00, Q_p1, Q_p2,
            hashId,
            siPropertiesTool);
}

//---------------------------------------------------------------
//      Get parameters for electron or hole transport
//---------------------------------------------------------------
bool SCT_InducedChargedModel::getVxVyD(const bool isElectron,
                                       const double x, const double y, double& vx, double& vy, double& D,
                                       const IdentifierHash hashId,
                                       ToolHandle<ISiPropertiesTool>& siPropertiesTool) const {
  double Ex, Ey;
  EField(x, y, Ex, Ey); // [V/cm]
  if (Ey > 0.) {
    if (isElectron) {
      // because electron has negative charge
      Ex *= -1.;
      Ey *= -1.;
    }
    const double E = std::sqrt(Ex*Ex+Ey*Ey);
    const double mu = (isElectron ?
                       siPropertiesTool->getSiProperties(hashId).calcElectronDriftMobility(m_T, E*CLHEP::volt/CLHEP::cm) :
                       siPropertiesTool->getSiProperties(hashId).calcHoleDriftMobility    (m_T, E*CLHEP::volt/CLHEP::cm))
      * (CLHEP::volt/CLHEP::cm)/(CLHEP::cm/CLHEP::s);
    const double v = mu * E;
    const double r = (isElectron ?
                      1.13 + 0.0008*(m_T - Gaudi::Units::STP_Temperature) :
                      0.72 - 0.0005*(m_T - Gaudi::Units::STP_Temperature));
    const double tanLA = r * mu * (isElectron ? -1. : +1.) * m_B / Gaudi::Units::tesla *  Gaudi::Units::gauss; // because e has negative charge.
    const double secLA = std::sqrt(1.+tanLA*tanLA);
    const double cosLA = 1./secLA;
    const double sinLA = tanLA/secLA;
    vy = v * (Ey*cosLA - Ex*sinLA)/E;
    vx = v * (Ex*cosLA + Ey*sinLA)/E;
    D = s_kB * m_T * mu/ s_e;
    return true;
  }
  return false;
}

//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_InducedChargedModel::induced(const int istrip, const double x, const double y) const
{
  // x and y are the location of charge (e or hole)
  // induced charge on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.004 [cm]
  double deltax = 0.0005, deltay = 0.00025;
   
  if (y < 0. || y > m_bulk_depth) return 0.;
  double xc = m_strip_pitch * (istrip + 0.5);
  double dx = std::abs(x-xc);
  int ix = static_cast<int>(dx / deltax);
  if (ix >= NRamoPoints) return 0.;
  int iy = static_cast<int>(y  / deltay);
  double fx = (dx - ix*deltax) / deltax;
  double fy = ( y - iy*deltay) / deltay;
  int ix1 = ix + 1;
  int iy1 = iy + 1;
  double P = m_PotentialValue[ix ][iy ] *(1.-fx)*(1.-fy)
           + m_PotentialValue[ix1][iy ] *    fx *(1.-fy)
           + m_PotentialValue[ix ][iy1] *(1.-fx)*    fy
           + m_PotentialValue[ix1][iy1] *     fx*    fy;

  return P;
}

//--------------------------------------------------------------
//   Electric Field Ex and Ey
//--------------------------------------------------------------
void SCT_InducedChargedModel::EField(const double x, const double y, double& Ex, double& Ey) const {
  // m_EFieldModel == UniformE 0; uniform E-field model
  // m_EFieldModel == FlatDiodeModel 1; flat diode model
  // m_EFieldModel == FEMsolutions 2; FEM solusions
  // x == 0.0040 [cm] : at the center of strip
  // x must be within 0 and 0.008 [cm]

  double deltay=0.00025, deltax=0.00025; // [cm]

  Ex = 0.;
  Ey = 0.;
  if (y < 0. || y > m_bulk_depth) return;
  //---------- case for FEM analysis solution  -------------------------
  if (m_EFieldModel==FEMsolutions) {
    int iy = static_cast<int>(y/deltay);
    double fy = (y-iy*deltay) / deltay;
    double xhalfpitch = m_strip_pitch/2.;
    double x999 = 999.*m_strip_pitch;
    double xx = fmod(x+x999, m_strip_pitch);
    double xxx = xx;
    if (xx > xhalfpitch) xxx = m_strip_pitch - xx;
    int ix = static_cast<int>(xxx/deltax);
    double fx = (xxx - ix*deltax) / deltax;         

    int ix1 = ix+1;
    int iy1 = iy+1;
    double Ex00 = 0., Ex10 = 0., Ex01 = 0., Ex11 = 0.;
    double Ey00 = 0., Ey10 = 0., Ey01 = 0., Ey11 = 0.;
    //-------- pick up appropriate data file for m_VD-----------------------

    Ex00 = m_ExValue[ix][iy];  Ex10 = m_ExValue[ix1][iy];
    Ex01 = m_ExValue[ix][iy1]; Ex11 = m_ExValue[ix1][iy1];
    Ey00 = m_EyValue[ix][iy];  Ey10 = m_EyValue[ix1][iy];
    Ey01 = m_EyValue[ix][iy1]; Ey11 = m_EyValue[ix1][iy1];

    //---------------- end of data bank search---
    Ex = Ex00*(1.-fx)*(1.-fy) + Ex10*fx*(1.-fy)
       + Ex01*(1.-fx)*    fy  + Ex11*fx*    fy;
    if (xx > xhalfpitch) Ex = -Ex;
    Ey = Ey00*(1.-fx)*(1.-fy) + Ey10*fx*(1.-fy)
       + Ey01*(1.-fx)*    fy  + Ey11*fx*    fy;
  }
  //---------- case for uniform electric field ------------------------
  else if (m_EFieldModel==UniformE) {
    if (m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0.) {
      Ey = m_VB / m_depletion_depth;
    } else { 
      Ey = 0.;
    }
  }
  //---------- case for flat diode model ------------------------------
  else if (m_EFieldModel==FlatDiodeModel) {
    if (m_VB > std::abs(m_VD)) { 
      Ey = (m_VB+m_VD)/m_bulk_depth - 2.*m_VD*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
    } else {
      if (m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0.) {
        double Emax = 2.* m_depletion_depth * std::abs(m_VD) / (m_bulk_depth*m_bulk_depth);
        Ey = Emax*(1.-(m_bulk_depth-y)/m_depletion_depth);
      } else { 
        Ey = 0.;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////

void SCT_InducedChargedModel::loadICMParameters() {
  // Loading Ramo potential and Electric field.
  // In strip pitch directions :
  //  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
  //  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
  // In sensor depth directions (for both potential and Electric field):
  //  114 divisions (115 points) with 2.5 nm intervals for 285 um.

  TFile *hfile = new TFile(PathResolverFindCalibFile("SCT_Digitization/SCT_InducedChargedModel.root").c_str());

  TH2F* h_Potential_FDM = static_cast<TH2F*>(hfile->Get("Potential_FDM"));
  TH2F* h_Potential_FEM = static_cast<TH2F*>(hfile->Get("Potential_FEM"));

  const float minBiasV_ICM_FEM = *std::min_element(s_VFD0.begin(), s_VFD0.end());
  const float maxBiasV_ICM_FEM = *std::max_element(s_VFD0.begin(), s_VFD0.end());
  if (m_VD<minBiasV_ICM_FEM || m_VD>maxBiasV_ICM_FEM) {
    m_EFieldModel = FlatDiodeModel; // Change to FDM
    ATH_MSG_INFO("Changed to Flat Diode Model since deplettion volage is out of range. ("
                 << minBiasV_ICM_FEM << " < m_VD < " << maxBiasV_ICM_FEM << " is allowed.)");
  }

  // For Ramo Potential 
  for (int ix=0; ix<NRamoPoints; ix++) {
    for (int iy=0; iy<NDepthPoints; iy++) {
      if (m_EFieldModel==FEMsolutions) { // FEM
        m_PotentialValue[ix][iy] = h_Potential_FEM->GetBinContent(ix+1, iy+1);
      } else { // FDM
        m_PotentialValue[ix][iy] = h_Potential_FDM->GetBinContent(ix+1, iy+1);
      }
    }
  }

  h_Potential_FDM->Delete();
  h_Potential_FEM->Delete();

  if (m_EFieldModel==FEMsolutions) {

    std::vector<std::string> hx_list;
    std::vector<std::string> hy_list;
    for (const std::string& str : s_VFD0str) {
      hx_list.push_back("Ex_FEM_"+str+"_0");
      hy_list.push_back("Ey_FEM_"+str+"_0");
    }

    size_t iVFD = 0;
    float dVFD1 = 0.;
    float dVFD2 = 0.;
    for (iVFD=0; iVFD<s_VFD0.size()-1; iVFD++) { // 2 of 9 will be seleted.
      dVFD1 = m_VD - s_VFD0[iVFD];
      dVFD2 = s_VFD0[iVFD+1] - m_VD;
      if (dVFD2 >= 0.) break;
    }
    
    TH2F* h_Ex1 = static_cast<TH2F*>(hfile->Get((hx_list.at(iVFD)).c_str()));
    TH2F* h_Ex2 = static_cast<TH2F*>(hfile->Get((hx_list.at(iVFD+1)).c_str()));
    TH2F* h_Ey1 = static_cast<TH2F*>(hfile->Get((hy_list.at(iVFD)).c_str()));
    TH2F* h_Ey2 = static_cast<TH2F*>(hfile->Get((hy_list.at(iVFD+1)).c_str()));
    TH2F* h_ExHV = static_cast<TH2F*>(hfile->Get("Ex_FEM_0_100"));
    TH2F* h_EyHV = static_cast<TH2F*>(hfile->Get("Ey_FEM_0_100"));

    float scalingFactor = m_VB / 100.; // Reference voltage is 100 V??

    // For Electric field    
    for (int ix=0; ix <NEFieldPoints; ix++){
      for (int iy=0; iy<NDepthPoints; iy++) {
        float Ex1 = h_Ex1->GetBinContent(ix+1, iy+1);
        float Ex2 = h_Ex2->GetBinContent(ix+1, iy+1);
        float ExHV = h_ExHV->GetBinContent(ix+1, iy+1);
        m_ExValue[ix][iy] = (Ex1*dVFD2+Ex2*dVFD1)/(dVFD1+dVFD2);
        m_ExValue[ix][iy] += ExHV*scalingFactor;

        float Ey1 = h_Ey1->GetBinContent(ix+1, iy+1);
        float Ey2 = h_Ey2->GetBinContent(ix+1, iy+1);
        float EyHV = h_EyHV->GetBinContent(ix+1, iy+1);
        m_EyValue[ix][iy] = (Ey1*dVFD2+Ey2*dVFD1)/(dVFD1+dVFD2);
        m_EyValue[ix][iy] += EyHV*scalingFactor;
      }
    }
    
    h_Ex1->Delete();
    h_Ex2->Delete();
    h_Ey1->Delete();
    h_Ey2->Delete();
    h_ExHV->Delete();
    h_EyHV->Delete();
  }
  
  hfile->Close();
}
