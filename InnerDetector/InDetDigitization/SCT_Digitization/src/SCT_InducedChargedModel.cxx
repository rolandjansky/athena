//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa 
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17  updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

#include "SCT_InducedChargedModel.h"

void SCT_InducedChargedModel::Init(float vdepl, float vbias, IdentifierHash m_hashId, ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc) {

  ATH_MSG_INFO("--- Induced Charged Model Paramter (Begin) --------");

//---------------setting basic parameters---------------------------

  m_VD = vdepl;   // full depletion voltage [Volt] negative for type-P
  m_VB = vbias;   // applied bias voltage [Volt]
  m_T = m_siConditionsSvc->temperature(m_hashId) + Gaudi::Units::STP_Temperature;

//------------------------ initialize subfunctions ------------

  loadICMParameters();
  
//------------ find delepletion deph for model=0 and 1 -------------
//   for type N (before type inversion)
  if (m_VD >= 0) {
    m_depletion_depth = m_bulk_depth;
    if (m_VB < m_VD) m_depletion_depth = sqrt(m_VB/m_VD) * m_bulk_depth;
  } else {   
    //   for type P
    m_depletion_depth = m_bulk_depth;
    if ( m_VB <= abs( m_VD) ) m_depletion_depth = 0.;
  }
  
//--------------------------------------------------------

  ATH_MSG_INFO(" EfieldModel  0(uniform E), 1(Flat Diode Model), 2 (FEM solusions)\t\t"<< m_EfieldModel );
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
//  holeTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::holeTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) {

// transport holes in the bulk 
// T. Kondo, 2010.9.9, 2017.7.20
// External parameters to be specified
//   m_transportTimeMax  [nsec]
//   m_transportTimeStep [nsec]     
//   m_bulk_depth        [cm]
// Induced currents are added to every m_transportTimeStep (defailt is 0.5 ns):
// Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] 
// means 50 ns storages of charges in each strips. 

   double x = x0;  // original hole position [cm]
   double y = y0;  // original hole position [cm]
   bool   isInBulk = true;
   double t_current = 0.;
   double qstrip[5];  // induced charges on strips due to a hole
   double vx, vy, D;
   for (int istrip = -2 ; istrip <= 2 ; istrip++) qstrip[istrip+2] = 
           induced(istrip, x, y); // original induced charge bu hole +e 
   while ( t_current < m_transportTimeMax ) {
     if ( !isInBulk ) break;
     if ( !hole( x, y, vx, vy, D, m_hashId, m_siPropertiesSvc)) break ;
     double delta_y = vy * m_transportTimeStep *1.E-9; 
     y += delta_y;
     double dt = m_transportTimeStep;
     if ( y > m_bulk_depth ) {
        isInBulk = false ;  // outside bulk region
        dt = (m_bulk_depth - (y-delta_y))/delta_y * m_transportTimeStep;
        y = m_bulk_depth;
     }
     t_current = t_current + dt;
     x += vx * dt *1.E-9;
     double diffusion = sqrt (2.* D * dt*1.E-9);
     y += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
     x += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
     if ( y > m_bulk_depth ) {
         y = m_bulk_depth;
         isInBulk = false;  // outside bulk region
     }

//   get induced current by subtracting induced charges
     for (int istrip = -2 ; istrip <= 2 ; istrip++) {
        double qnew = induced( istrip, x, y);
        int jj = istrip + 2;
        double dq = qnew - qstrip[jj];
        qstrip[jj] = qnew ;

	int jt = int( (t_current+0.001) / m_transportTimeStep) ; 
        if (jt < 100) {
	  switch(istrip) {
	  case -2: Q_m2[jt] += dq ; break;
	  case -1: Q_m1[jt] += dq ; break;
	  case  0: Q_00[jt] += dq ; break;
	  case +1: Q_p1[jt] += dq ; break;
	  case +2: Q_p2[jt] += dq ; break;
	  } 
        }

     }
   }  // end of hole tracing 

   return ;
}

//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::electronTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc ) {

// transport electrons in the bulk 
// T. Kondo, 2010.9.9, 2017.7.20
// External parameters to be specified
//   m_transportTimeMax  [nsec]
//   m_transportTimeStep [nsec]     
//   m_bulk_depth        [cm]
// Induced currents are added to every m_transportTimeStep (defailt is 0.5 ns):
// Q_m2[100],Q_m1[100],Q_00[100],Q_p1[100],Q_p2[100] 
// means 50 ns storages of charges in each strips. 

   double x = x0;  // original electron position [cm]
   double y = y0;  // original electron position [cm]
   bool isInBulk = true;
   double t_current = 0.;
   double qstrip[5];
   double vx, vy, D;
   for (int istrip = -2 ; istrip <= 2 ; istrip++) 
     qstrip[istrip+2] = -induced(istrip, x, y);
   while (t_current < m_transportTimeMax) {
     if ( !isInBulk ) break;
     if ( !electron( x, y, vx, vy, D, m_hashId, m_siPropertiesSvc)) break ;
     double delta_y = vy * m_transportTimeStep *1.E-9; 
     y += delta_y;
     double dt = m_transportTimeStep;  // [nsec]
     if ( y < m_y_origin_min ) {
        isInBulk = false ;
        dt = (m_y_origin_min - (y-delta_y))/delta_y * m_transportTimeStep;
        y = m_y_origin_min; 
     }
     t_current = t_current + dt;
     x += vx * dt *1.E-9;
     double diffusion = sqrt (2.* D * dt*1.E-9);
     y += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
     x += diffusion * CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, 1.0);
     if ( y < m_y_origin_min ) {
        y = m_y_origin_min;
        isInBulk = false;
     }

//   get induced current by subtracting induced charges
     for (int istrip = -2 ; istrip <= 2 ; istrip++) {
        double qnew = -induced( istrip, x, y);
        int jj = istrip + 2;
        double dq = qnew - qstrip[jj];
        qstrip[jj] = qnew ;

	int jt = int( (t_current + 0.001) / m_transportTimeStep);
        if (jt< 100) {
	  switch(istrip) {
	  case -2: Q_m2[jt] += dq ; break;
	  case -1: Q_m1[jt] += dq ; break;
	  case  0: Q_00[jt] += dq ; break;
	  case +1: Q_p1[jt] += dq ; break;
	  case +2: Q_p2[jt] += dq ; break;
	  } 
        }

     }
   }  // end of electron tracing 

   return ;
}

//---------------------------------------------------------------
//      parameters for electron transport
//---------------------------------------------------------------
bool SCT_InducedChargedModel::electron(double x_e, double y_e, double &vx_e, double &vy_e, double &D_e, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) {

double E, Ex, Ey, mu_e, v_e, r_e, tanLA_e, secLA, cosLA, sinLA;
EField(x_e, y_e, Ex, Ey);    // [V/cm]
if ( Ey > 0.) {
   double REx = -Ex; // because electron has negative charge 
   double REy = -Ey; // because electron has negative charge
   E = sqrt(Ex*Ex+Ey*Ey);
   mu_e = m_siPropertiesSvc->getSiProperties(m_hashId).calcElectronDriftMobility(m_T,E*CLHEP::volt/CLHEP::cm);
   mu_e *= (CLHEP::volt/CLHEP::cm)/(CLHEP::cm/CLHEP::s);
   v_e = mu_e * E;
   r_e = 0;
   r_e = 1.13+0.0008*(m_T-Gaudi::Units::STP_Temperature);
   tanLA_e = r_e * mu_e * (-m_B) * 1.E-4;  // because e has negative charge
   secLA = sqrt(1.+tanLA_e*tanLA_e);
   cosLA=1./secLA;
   sinLA = tanLA_e / secLA;
   vy_e = v_e * (REy*cosLA - REx*sinLA)/E;
   vx_e = v_e * (REx*cosLA + REy*sinLA)/E;
   D_e = m_kB * m_T * mu_e/ m_e;  

   return true;
}else return false;
}

//---------------------------------------------------------------
//      parameters for hole transport
//---------------------------------------------------------------
bool SCT_InducedChargedModel::hole(double x_h, double y_h, double &vx_h, double &vy_h, double &D_h, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) {

double E, Ex, Ey, mu_h, v_h, r_h, tanLA_h, secLA, cosLA, sinLA;
EField( x_h, y_h, Ex, Ey);  // [V/cm]
if ( Ey > 0.) {
   E = sqrt(Ex*Ex+Ey*Ey);
   mu_h = m_siPropertiesSvc->getSiProperties(m_hashId).calcHoleDriftMobility(m_T,E*CLHEP::volt/CLHEP::cm);
   mu_h *= (CLHEP::volt/CLHEP::cm)/(CLHEP::cm/CLHEP::s);
   v_h = mu_h * E;
   r_h = 0;
   r_h = 0.72 - 0.0005*(m_T-Gaudi::Units::STP_Temperature);
   tanLA_h = r_h * mu_h * m_B * 1.E-4;
   secLA = sqrt(1.+tanLA_h*tanLA_h);
   cosLA=1./secLA;
   sinLA = tanLA_h / secLA;
   vy_h = v_h * (Ey*cosLA - Ex*sinLA)/E;
   vx_h = v_h * (Ex*cosLA + Ey*sinLA)/E;
   D_h = m_kB * m_T * mu_h/ m_e;  
   return true;
}  else return false;
}

//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_InducedChargedModel::induced (int istrip, double x, double y)
{
// x and y are the location of charge (e or hole)
// induced charge on the strip "istrip" situated at the height y = d
// the center of the strip (istrip=0) is x = 0.004 [cm]
   double deltax = 0.0005, deltay = 0.00025;
   
   if ( y < 0. || y > m_bulk_depth) return 0;
   double xc = m_strip_pitch * (istrip + 0.5);
   double dx = fabs( x-xc );
   int ix = int( dx / deltax );
   if ( ix > 80 ) return 0.;
   int iy = int( y  / deltay );
   double fx = (dx - ix*deltax) / deltax;
   double fy = ( y - iy*deltay) / deltay;
   int ix1 = ix + 1;
   int iy1 = iy + 1;
   double P = m_PotentialValue[ix][iy]   *(1.-fx)*(1.-fy)
            + m_PotentialValue[ix1][iy]  *fx*(1.-fy)
            + m_PotentialValue[ix][iy1]  *(1.-fx)*fy
            + m_PotentialValue[ix1][iy1] *fx*fy ;

   return P;
}

//--------------------------------------------------------------
//   Electric Field Ex and Ey
//--------------------------------------------------------------
void SCT_InducedChargedModel::EField( double x, double y, double &Ex, double &Ey ) { 
// m_EfieldModel == 0; uniform E-field model
// m_EfieldModel == 1; flat diode model
// m_EfieldModel == 2; FEM solusions
// x == 0.0040 [cm] : at the center of strip
// x must be within 0 and 0.008 [cm]

   double deltay=0.00025, deltax=0.00025  ;   // [cm]  

   Ex = 0.;
   Ey = 0.;
   if ( y < 0. || y > m_bulk_depth) return;
//---------- case for FEM analysis solution  -------------------------
   if (m_EfieldModel==2) {       
       int iy = int (y/deltay);
       double fy = (y-iy*deltay) / deltay;
       double xhalfpitch=m_strip_pitch/2.;
       double x999 = 999.*m_strip_pitch;
       double xx = fmod (x+x999, m_strip_pitch);
       double xxx = xx;
       if( xx > xhalfpitch ) xxx = m_strip_pitch - xx;
       int ix = int(xxx/deltax) ;
       double fx = (xxx - ix*deltax) / deltax;         

       int ix1=ix+1;
       int iy1=iy+1;
       double Ex00 = 0., Ex10 = 0., Ex01 = 0., Ex11 = 0.;
       double Ey00 = 0., Ey10 = 0., Ey01 = 0., Ey11 = 0.;
 //-------- pick up appropriate data file for m_VD-----------------------

       Ex00 = m_ExValue[ix][iy];  Ex10 = m_ExValue[ix1][iy];
       Ex01 = m_ExValue[ix][iy1]; Ex11 = m_ExValue[ix1][iy1];
       Ey00 = m_EyValue[ix][iy];  Ey10 = m_EyValue[ix1][iy];
       Ey01 = m_EyValue[ix][iy1]; Ey11 = m_EyValue[ix1][iy1];

//---------------- end of data bank search---
       Ex = Ex00*(1.-fx)*(1.-fy) + Ex10*fx*(1.-fy)
              + Ex01*(1.-fx)*fy + Ex11*fx*fy ;
       if (xx > xhalfpitch ) Ex = -Ex;
       Ey = Ey00*(1.-fx)*(1.-fy) + Ey10*fx*(1.-fy)
              + Ey01*(1.-fx)*fy + Ey11*fx*fy ;
       return;
   }

//---------- case for uniform electriv field ------------------------
   if ( m_EfieldModel==0 ) {
       if ( m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0. ) {
          Ey = m_VB / m_depletion_depth ;   
       } else { 
          Ey = 0.;
       }
       return;
   }
//---------- case for flat diode model ------------------------------
   if ( m_EfieldModel==1 ) {
       if(m_VB > abs(m_VD)) { 
          Ey = (m_VB+m_VD)/m_bulk_depth 
                - 2.*m_VD*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
       } else {
          if ( m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0.) {
             double Emax = 2.* m_depletion_depth * abs(m_VD) /
                        (m_bulk_depth*m_bulk_depth);
             Ey = Emax*(1-(m_bulk_depth-y)/m_depletion_depth);
          } else { 
             Ey = 0.;
          }
       }
       return;
   }
return;
}

/////////////////////////////////////////////////////////////////////

void SCT_InducedChargedModel::loadICMParameters(){
// Loading Ramo potential and Electric field.                                 
// In strip pitch directions :
//  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
//  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
// In sensor depth directions (for both potential and Electric field):
//  114 divisions (115 points) with 2.5 nm intervals for 285 um.           

  TFile *hfile = new TFile( PathResolverFindCalibFile("SCT_Digitization/SCT_InducedChargedModel.root").c_str() );

  h_Potential_FDM = static_cast<TH2F *>(hfile->Get("Potential_FDM"));
  h_Potential_FEM = static_cast<TH2F *>(hfile->Get("Potential_FEM"));

  constexpr float MinBiasV_ICM_FEM = -180.0;
  constexpr float MaxBiasV_ICM_FEM = 70.0;
  if ( m_VD<MinBiasV_ICM_FEM || m_VD>MaxBiasV_ICM_FEM ){
    m_EfieldModel = 1; // Change to FDM
    ATH_MSG_INFO("Changed to Flat Diode Model since deplettion volage is out of range. (-180 < m_VD < 70 is allow.)");  
  }

  // For Ramo Potential 
  for (int ix=0 ; ix <81 ; ix++ ){
    for (int iy=0 ; iy<115 ; iy++ ) {
      if (m_EfieldModel==2){ // FEM
	m_PotentialValue[ix][iy] = h_Potential_FEM -> GetBinContent(ix+1,iy+1);
      } else { // FDM	
	m_PotentialValue[ix][iy] = h_Potential_FDM -> GetBinContent(ix+1,iy+1);
      }
    }
  }

  h_Potential_FDM -> Delete();
  h_Potential_FEM -> Delete();

  if ( m_EfieldModel == 2 ){

    float VFD0[9]= { -180, -150, -120, -90, -60, -30, 0, 30, 70};

    std::vector<std::string> hx_list;
    std::vector<std::string> hy_list;

    hx_list.push_back("Ex_FEM_M180_0");
    hx_list.push_back("Ex_FEM_M150_0");
    hx_list.push_back("Ex_FEM_M120_0");
    hx_list.push_back("Ex_FEM_M90_0");
    hx_list.push_back("Ex_FEM_M60_0");
    hx_list.push_back("Ex_FEM_M30_0");
    hx_list.push_back("Ex_FEM_0_0");
    hx_list.push_back("Ex_FEM_30_0");
    hx_list.push_back("Ex_FEM_70_0");

    hy_list.push_back("Ey_FEM_M180_0");
    hy_list.push_back("Ey_FEM_M150_0");
    hy_list.push_back("Ey_FEM_M120_0");
    hy_list.push_back("Ey_FEM_M90_0");
    hy_list.push_back("Ey_FEM_M60_0");
    hy_list.push_back("Ey_FEM_M30_0");
    hy_list.push_back("Ey_FEM_0_0");
    hy_list.push_back("Ey_FEM_30_0");
    hy_list.push_back("Ey_FEM_70_0");

    int iVFD = 0;
    float dVFD1 = 0;
    float dVFD2 = 0;
    for ( iVFD=0; iVFD<8; iVFD++ ){ // 2 of 9 will be seleted.
      dVFD1 = m_VD - VFD0[iVFD];
      dVFD2 = VFD0[iVFD+1] - m_VD;
      if( dVFD2 >= 0 ) break;
    }
    
    h_Ex1 = static_cast<TH2F *>(hfile->Get((hx_list.at(iVFD)).c_str()));
    h_Ex2 = static_cast<TH2F *>(hfile->Get((hx_list.at(iVFD+1)).c_str()));
    h_Ey1 = static_cast<TH2F *>(hfile->Get((hy_list.at(iVFD)).c_str()));
    h_Ey2 = static_cast<TH2F *>(hfile->Get((hy_list.at(iVFD+1)).c_str()));
    h_ExHV = static_cast<TH2F *>(hfile->Get("Ex_FEM_0_100"));
    h_EyHV = static_cast<TH2F *>(hfile->Get("Ey_FEM_0_100"));

    float scalingFactor = m_VB / 100;

    // For Electric field    
    for (int ix=0 ; ix <17 ; ix++){
      for (int iy=0 ; iy<115 ; iy++) {
 	float Ex1 = h_Ex1->GetBinContent(ix+1,iy+1);
	float Ex2 = h_Ex2->GetBinContent(ix+1,iy+1);
	float ExHV = h_ExHV-> GetBinContent(ix+1,iy+1);       
	m_ExValue[ix][iy] = (Ex1*dVFD2+Ex2*dVFD1)/(dVFD1+dVFD2);
	m_ExValue[ix][iy] += ExHV*scalingFactor;

 	float Ey1 = h_Ey1->GetBinContent(ix+1,iy+1);
	float Ey2 = h_Ey2->GetBinContent(ix+1,iy+1);
	float EyHV = h_EyHV-> GetBinContent(ix+1,iy+1);       
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
  
  hfile -> Close();

}

