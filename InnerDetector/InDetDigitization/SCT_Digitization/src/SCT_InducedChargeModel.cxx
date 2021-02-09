/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17 updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

#include "SCT_InducedChargeModel.h"

// ROOT
#include "TH2.h"
#include "TFile.h"

// C++ Standard Library
#include <algorithm>
#include <cmath>
#include <iostream>

const double SCT_InducedChargeModel::s_kB = Gaudi::Units::k_Boltzmann / Gaudi::Units::joule; // [m^2*kg/s^2/K]
const double SCT_InducedChargeModel::s_e = Gaudi::Units::e_SI; // [Coulomb]
// Sorted depletion voltage values
const std::vector<float> SCT_InducedChargeModel::s_VFD0 =
  { -180.,  -150.,  -120.,  -90.,  -60.,  -30.,  0.,  30.,  70.};
const std::vector<std::string> SCT_InducedChargeModel::s_VFD0str =
  {"M180", "M150", "M120", "M90", "M60", "M30", "0", "30", "70"};

SCT_InducedChargeModel::SCT_InducedChargeModel(size_t maxHash, EFieldModel model) :
  m_msg("SCT_InducedChargeModel"),
  m_EFieldModel(model)
{
  loadICMParameters();
  m_data.resize(maxHash);
}

SCT_InducedChargeModel::SCT_InducedChargeModelData*
SCT_InducedChargeModel::setWaferData(const float vdepl,
                                     const float vbias,
                                     const InDetDD::SiDetectorElement* element,
                                     const ServiceHandle<MagField::IMagFieldSvc> magFieldSvc,
                                     const ServiceHandle<ISiliconConditionsSvc> siConditionsSvc) const {
  std::lock_guard<std::mutex> lock(m_mutex);

  // If cache exists, cache is used.
  SCT_InducedChargeModelData* p_data = m_data[element->identifyHash()].get();
  if (p_data) return p_data;

  ATH_MSG_DEBUG("--- Induced Charged Model Paramter (Begin) --------");

  HepGeom::Point3D<double> localpos(0., 0., 0.); // The center of wafer
  HepGeom::Point3D<double> globalpos = element->globalPositionHit(localpos);
  double point[3] = {globalpos.x(), globalpos.y(), globalpos.z()};
  double field[3] = {0., 0., 0.};
  magFieldSvc->getField(point, field);
  const Amg::Vector3D magneticField(field[0], field[1], field[2]); // in kTesla

  //---------------setting basic parameters---------------------------
  std::unique_ptr<SCT_InducedChargeModelData> data =
    std::make_unique<SCT_InducedChargeModelData>(vdepl,
                                                 vbias,
                                                 element,
                                                 magneticField,
                                                 m_bulk_depth,
                                                 m_EFieldModel,
                                                 siConditionsSvc);

  //--- set electric fields ---
  setEField(*data);

  //--------------------------------------------------------

  ATH_MSG_DEBUG(" EFieldModel  0 (Flat Diode Model), 1 (FEM solusions) 2 (uniform E)\t"<< data->m_EFieldModel);
  ATH_MSG_DEBUG(" DepletionVoltage_VD\t"<< data->m_VD <<" V");
  ATH_MSG_DEBUG(" BiasVoltage_VB     \t"<< data->m_VB  <<" V");
  ATH_MSG_DEBUG(" MagneticField_B    \t"
                << data->m_magneticField[Amg::x] << " "
                << data->m_magneticField[Amg::y] << " "
                << data->m_magneticField[Amg::z] <<" kTesla");
  ATH_MSG_DEBUG(" Temperature_T      \t"<< data->m_T  <<" K");
  ATH_MSG_DEBUG(" TransportTimeStep  \t"<< m_transportTimeStep <<" ns");
  ATH_MSG_DEBUG(" TransportTimeMax\t"<< m_transportTimeMax <<" ns");
  ATH_MSG_DEBUG(" BulkDepth\t\t"<< m_bulk_depth << " cm");
  ATH_MSG_DEBUG(" DepletionDepth\t\t"<< data->m_depletion_depth << " cm");
  ATH_MSG_DEBUG(" StripPitch\t\t"<< m_strip_pitch << " cm");
  ATH_MSG_DEBUG("--- Induced Charged Model Paramter (End) ---------");

  m_data[element->identifyHash()] = std::move(data);

  return m_data[element->identifyHash()].get();
}

//---------------------------------------------------------------------
//  transport
//---------------------------------------------------------------------
void SCT_InducedChargeModel::transport(const SCT_InducedChargeModelData& data,
                                       const bool isElectron,
                                       const double x0, const double y0,
                                       double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                       const IdentifierHash hashId,
                                       const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const {
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
    qstrip[istrip+Offset] = (isElectron ? -1. : +1.) * induced(data, istrip, x, y); // original induced charge by electron or hole
  }
  while (t_current < m_transportTimeMax) {
    if (!isInBulk) break;
    if (!getVxVyD(data, isElectron, x, y, vx, vy, D, hashId, siPropertiesSvc)) break;
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
      double qnew = (isElectron ? -1. : +1.) * induced(data, istrip, x, y);
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
void SCT_InducedChargeModel::holeTransport(const SCT_InducedChargeModelData& data,
                                           const double x0, const double y0,
                                           double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                           const IdentifierHash hashId,
                                           const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const {
  // Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] NTransportSteps=100
  const bool isElectron = false;
  transport(data,
            isElectron,
            x0, y0,
            Q_m2, Q_m1, Q_00, Q_p1, Q_p2,
            hashId,
            siPropertiesSvc);
}

//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void SCT_InducedChargeModel::electronTransport(const SCT_InducedChargeModelData& data,
                                               const double x0, const double y0,
                                               double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                                               const IdentifierHash hashId,
                                               const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const {
  // Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] NTransportSteps=100
  const bool isElectron = true;
  transport(data,
            isElectron,
            x0, y0,
            Q_m2, Q_m1, Q_00, Q_p1, Q_p2,
            hashId,
            siPropertiesSvc);
}

//---------------------------------------------------------------
//      Get parameters for electron or hole transport
//---------------------------------------------------------------
bool SCT_InducedChargeModel::getVxVyD(const SCT_InducedChargeModelData& data,
                                      const bool isElectron,
                                      const double x, const double y, double& vx, double& vy, double& D,
                                      const IdentifierHash hashId,
                                      const ServiceHandle<ISiPropertiesSvc> siPropertiesSvc) const {
  double Ex, Ey;
  getEField(data, x, y, Ex, Ey); // [V/cm]
  if (Ey > 0.) {
    if (isElectron) {
      // because electron has negative charge
      Ex *= -1.;
      Ey *= -1.;
    }
    const double E = std::sqrt(Ex*Ex+Ey*Ey);
    const double mu = (isElectron ?
                       siPropertiesSvc->getSiProperties(hashId).calcElectronDriftMobility(data.m_T, E*CLHEP::volt/CLHEP::cm) :
                       siPropertiesSvc->getSiProperties(hashId).calcHoleDriftMobility    (data.m_T, E*CLHEP::volt/CLHEP::cm))
      * (CLHEP::volt/CLHEP::cm)/(CLHEP::cm/CLHEP::s);
    const double v = mu * E;
    const double r = (isElectron ?
                      siPropertiesSvc->getSiProperties(hashId).calcElectronHallFactor(data.m_T) :
                      siPropertiesSvc->getSiProperties(hashId).calcHoleHallFactor(data.m_T));

    const double tanLA = data.m_element->design().readoutSide()
      * r * mu * (isElectron ? -1. : +1.) // because e has negative charge.
      * data.m_element->hitDepthDirection()
      * data.m_element->hitPhiDirection()
      * (data.m_element->normal().cross(data.m_magneticField)).dot(data.m_element->phiAxis())
      / Gaudi::Units::tesla * Gaudi::Units::gauss * 1000.; // 1000. is to convert kTesla to Tesla.

    const double secLA = std::sqrt(1.+tanLA*tanLA);
    const double cosLA = 1./secLA;
    const double sinLA = tanLA/secLA;
    vy = v * (Ey*cosLA - Ex*sinLA)/E;
    vx = v * (Ex*cosLA + Ey*sinLA)/E;
    D = s_kB * data.m_T * mu/ s_e;
    return true;
  }
  return false;
}

//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_InducedChargeModel::induced(const SCT_InducedChargeModelData& data,
                                       const int istrip, const double x, const double y) const
{
  // x and y are the location of charge (e or hole)
  // induced charge on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.004 [cm]

  // Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
  // In sensor depth directions 114 divisions (115 points) with 2.5 nm intervals for 285 um.
  const double deltax = 0.0005, deltay = 0.00025;
   
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
  double P = m_PotentialValue[data.m_EFieldModel][ix ][iy ] *(1.-fx)*(1.-fy)
           + m_PotentialValue[data.m_EFieldModel][ix1][iy ] *    fx *(1.-fy)
           + m_PotentialValue[data.m_EFieldModel][ix ][iy1] *(1.-fx)*    fy
           + m_PotentialValue[data.m_EFieldModel][ix1][iy1] *     fx*    fy;

  return P;
}

//--------------------------------------------------------------
//   Electric Field Ex and Ey
//--------------------------------------------------------------
void SCT_InducedChargeModel::getEField(const SCT_InducedChargeModelData& data,
                                       const double x, const double y, double& Ex, double& Ey) const {
  // m_EFieldModel == FlatDiodeModel 0; flat diode model
  // m_EFieldModel == FEMsolutions 1; FEM solusions
  // m_EFieldModel == UniformE 2; uniform E-field model
  // x == 0.0040 [cm] : at the center of strip
  // x must be within 0 and 0.008 [cm]

  // Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
  // In sensor depth directions 114 divisions (115 points) with 2.5 nm intervals for 285 um.
  const double deltay=0.00025, deltax=0.00025; // [cm]

  Ex = 0.;
  Ey = 0.;
  if (y < 0. || y > m_bulk_depth) return;
  //---------- case for FEM analysis solution  -------------------------
  if (data.m_EFieldModel==FEMsolutions) {
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

    Ex00 = data.m_ExValue[ix][iy];  Ex10 = data.m_ExValue[ix1][iy];
    Ex01 = data.m_ExValue[ix][iy1]; Ex11 = data.m_ExValue[ix1][iy1];
    Ey00 = data.m_EyValue[ix][iy];  Ey10 = data.m_EyValue[ix1][iy];
    Ey01 = data.m_EyValue[ix][iy1]; Ey11 = data.m_EyValue[ix1][iy1];

    //---------------- end of data bank search---
    Ex = Ex00*(1.-fx)*(1.-fy) + Ex10*fx*(1.-fy)
       + Ex01*(1.-fx)*    fy  + Ex11*fx*    fy;
    if (xx > xhalfpitch) Ex = -Ex;
    Ey = Ey00*(1.-fx)*(1.-fy) + Ey10*fx*(1.-fy)
       + Ey01*(1.-fx)*    fy  + Ey11*fx*    fy;
  }
  //---------- case for uniform electric field ------------------------
  else if (data.m_EFieldModel==UniformE) {
    if (m_bulk_depth - y < data.m_depletion_depth && data.m_depletion_depth >0.) {
      Ey = data.m_VB / data.m_depletion_depth;
    } else { 
      Ey = 0.;
    }
  }
  //---------- case for flat diode model ------------------------------
  else if (data.m_EFieldModel==FlatDiodeModel) {
    if (data.m_VB > std::abs(data.m_VD)) { 
      Ey = (data.m_VB+data.m_VD)/m_bulk_depth - 2.*data.m_VD*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
    } else {
      if (m_bulk_depth - y < data.m_depletion_depth && data.m_depletion_depth >0.) {
        double Emax = 2.* data.m_depletion_depth * std::abs(data.m_VD) / (m_bulk_depth*m_bulk_depth);
        Ey = Emax*(1.-(m_bulk_depth-y)/data.m_depletion_depth);
      } else { 
        Ey = 0.;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
size_t SCT_InducedChargeModel::getFEMIndex(SCT_InducedChargeModelData& data) const {
  // Return index for s_VFD0 and EFieldModel.
  // If vdepl is out of range of s_VFD0, EFieldModel of FlatDiodeModel is returned.
  size_t iVFD = 0;
  if (data.m_VD<s_VFD0[0] || data.m_VD>s_VFD0[s_VFD0.size()-1]) { // Out of range
    data.m_EFieldModel = FlatDiodeModel;
  } else {
    float dVFD2 = 0.;
    for (iVFD=0; iVFD<s_VFD0.size()-1; iVFD++) {
      dVFD2 = s_VFD0[iVFD+1] - data.m_VD;
      if (dVFD2 >= 0.) break;
    }
  }
  return iVFD;
}

/////////////////////////////////////////////////////////////////////

void SCT_InducedChargeModel::loadICMParameters() {
  // Loading Ramo potential and Electric field.
  // In strip pitch directions :
  //  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
  //  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
  // In sensor depth directions (for both potential and Electric field):
  //  114 divisions (115 points) with 2.5 nm intervals for 285 um.

  TFile *hfile = new TFile(PathResolverFindCalibFile("SCT_Digitization/SCT_InducedChargedModel.root").c_str());

  // For Ramo Potential 
  TH2F* h_Potential_FDM = static_cast<TH2F*>(hfile->Get("Potential_FDM"));
  TH2F* h_Potential_FEM = static_cast<TH2F*>(hfile->Get("Potential_FEM"));
  // FDM case keeps only FDM potential.
  // FEM case keeps both FDM and FEM potentials.
  if (m_EFieldModel==FlatDiodeModel || m_EFieldModel==FEMsolutions) {
    if (m_EFieldModel==FlatDiodeModel) m_PotentialValue.resize(FlatDiodeModel+1);
    else m_PotentialValue.resize(FEMsolutions+1);
    for (int ix=0; ix<NRamoPoints; ix++) {
      for (int iy=0; iy<NDepthPoints; iy++) {
        m_PotentialValue[FlatDiodeModel][ix][iy] = h_Potential_FDM->GetBinContent(ix+1, iy+1);
        if (m_EFieldModel==FEMsolutions) {
          m_PotentialValue[FEMsolutions][ix][iy] = h_Potential_FEM->GetBinContent(ix+1, iy+1);
        }
      }
    }
  }
  h_Potential_FDM->Delete();
  h_Potential_FEM->Delete();

  if (m_EFieldModel==FEMsolutions) {
    size_t i=0;
    m_ExValue.resize(s_VFD0str.size()+1);
    m_EyValue.resize(s_VFD0str.size()+1);
    for (const std::string& str : s_VFD0str) {
      TH2F* h_Ex = static_cast<TH2F*>(hfile->Get(("Ex_FEM_"+str+"_0").c_str()));
      TH2F* h_Ey = static_cast<TH2F*>(hfile->Get(("Ey_FEM_"+str+"_0").c_str()));
      for (int ix=0; ix <NEFieldPoints; ix++){
        for (int iy=0; iy<NDepthPoints; iy++) {
          m_ExValue[i][ix][iy] = h_Ex->GetBinContent(ix+1, iy+1);
          m_EyValue[i][ix][iy] = h_Ey->GetBinContent(ix+1, iy+1);
        }
      }
      h_Ex->Delete();
      h_Ey->Delete();
      i++;
    }

    TH2F* h_ExHV = static_cast<TH2F*>(hfile->Get("Ex_FEM_0_100"));
    TH2F* h_EyHV = static_cast<TH2F*>(hfile->Get("Ey_FEM_0_100"));
    for (int ix=0; ix <NEFieldPoints; ix++){
      for (int iy=0; iy<NDepthPoints; iy++) {
        m_ExValue[i][ix][iy] = h_ExHV->GetBinContent(ix+1, iy+1);
        m_EyValue[i][ix][iy] = h_EyHV->GetBinContent(ix+1, iy+1);
      }
    }
    h_ExHV->Delete();
    h_EyHV->Delete();
  }

  hfile->Close();
}

void SCT_InducedChargeModel::setEField(SCT_InducedChargeModelData& data) const {
  if (data.m_EFieldModel!=FEMsolutions) return;

  size_t iVFD = getFEMIndex(data);
  if (data.m_EFieldModel==FlatDiodeModel) {
    ATH_MSG_INFO("Changed to Flat Diode Model since deplettion volage is out of range. ("
                 << s_VFD0[0] << " < m_VD < " << s_VFD0[s_VFD0.size()-1] << " is allowed.)");
    return;
  }

  const float dVFD1 = data.m_VD - s_VFD0[iVFD];
  const float dVFD2 = s_VFD0[iVFD+1] - data.m_VD;

  const float scalingFactor = data.m_VB / 100.; // Reference voltage is 100 V

  const size_t iHV = s_VFD0.size();

  // For Electric field
  for (int ix=0; ix <NEFieldPoints; ix++){
    for (int iy=0; iy<NDepthPoints; iy++) {
      float Ex1  = m_ExValue[iVFD  ][ix][iy];
      float Ex2  = m_ExValue[iVFD+1][ix][iy];
      float ExHV = m_ExValue[iHV   ][ix][iy];
      data.m_ExValue[ix][iy] = (Ex1*dVFD2+Ex2*dVFD1)/(dVFD1+dVFD2);
      data.m_ExValue[ix][iy] += ExHV*scalingFactor;

      float Ey1  = m_EyValue[iVFD  ][ix][iy];
      float Ey2  = m_EyValue[iVFD+1][ix][iy];
      float EyHV = m_EyValue[iHV   ][ix][iy];
      data.m_EyValue[ix][iy] = (Ey1*dVFD2+Ey2*dVFD1)/(dVFD1+dVFD2);
      data.m_EyValue[ix][iy] += EyHV*scalingFactor;
    }
  }
}
