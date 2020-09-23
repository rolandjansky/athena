//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa 
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17  updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

#include "SCT_InducedChargedModel.h"

#include "TRandom.h"

#include "PathResolver/PathResolver.h"

void SCT_InducedChargedModel::Init(float vdepl, float vbias) {

  if (m_coutLevel >=0) {
    std::cout<<"--- Induced Charged Model Paramter (Begin) --------"<<std::endl;
  }

//---------------setting basic parameters---------------------------

  m_VD = vdepl;    // full depletion voltage [Volt] negative for type-P
  m_VB = vbias;   // applied bias voltage [Volt]    

//------------------------ initialize subfunctions ------------

  init_mud_h(m_T);
  init_mud_e(m_T);
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
  if (m_coutLevel >=0) {
    std::cout<<" EfieldModel  0(uniform E), 1(Flat Diode Model), 2 (FEM solusions)\t\t"<< m_EfieldModel <<std::endl;
    std::cout<<" DepletionVoltage_VD\t"<< m_VD <<" V"<<std::endl;
    std::cout<<" BiasVoltage_VB     \t"<< m_VB  <<" V"<<std::endl;
    std::cout<<" MagneticField_B    \t"<< m_B  <<" Tesla"<<std::endl; 
    std::cout<<" Temperature_T      \t"<< m_T  <<" K"<<std::endl;
    std::cout<<" TransportTimeStep  \t"<< m_transportTimeStep <<" ns"<<std::endl;
    std::cout<<" TransportTimeMax\t"<< m_transportTimeMax <<" ns"<<std::endl;
    std::cout<<" BulkDepth\t\t"<< m_bulk_depth << " cm"<<std::endl;
    std::cout<<" DepletionDepth\t\t"<< m_depletion_depth << " cm"<<std::endl;
    std::cout<<" StripPitch\t\t"<< m_strip_pitch << " cm"<<std::endl;
    std::cout<<" CoutLevel\t\t"<< m_coutLevel <<std::endl;
    std::cout<<"--- Induced Charged Model Paramter (End) ---------"<<std::endl;  
  }
  
  return;
}

//--------------------------------------------------------------
//   drift mobility for electrons
//--------------------------------------------------------------
double SCT_InducedChargedModel::mud_e(double E) {
  return m_vs_e/m_Ec_e/pow(1+pow(E/m_Ec_e,m_beta_e),1./m_beta_e);
}

//--------------------------------------------------------------
//   initialize drift mobility for electrons
//--------------------------------------------------------------
void SCT_InducedChargedModel::init_mud_e(double T) {
   m_vs_e=1.53E9*pow( T,-0.87);
   m_Ec_e = 1.01*pow( T, 1.55);
   m_beta_e = 2.57E-2* pow(T,0.66);
   if(m_coutLevel >= 0) {
   std::cout<<"---- parameters for electron transport -----"<<std::endl;
   std::cout<<" m_vs_e        = "<< m_vs_e << std::endl;
   std::cout<<" m_Ec_e        = "<< m_Ec_e << std::endl;
   std::cout<<" m_beta_e      = "<< m_beta_e << std::endl;
   std::cout<<"--------------------------------------------"<<std::endl;
   }

   return ;
}

//--------------------------------------------------------------
//   drift mobility for holes
//--------------------------------------------------------------
double SCT_InducedChargedModel::mud_h(double E) {
  return m_vs_h/m_Ec_h/pow(1+pow(E/m_Ec_h,m_beta_h),1./m_beta_h);
}

//--------------------------------------------------------------
//   initialize drift mobility for holes
//--------------------------------------------------------------
void SCT_InducedChargedModel::init_mud_h(double T) {
  m_vs_h = 1.62E8 * pow( T, -0.52);
  m_Ec_h = 1.24 * pow( T, 1.68);
  m_beta_h = 0.46 * pow(T,0.17);

  if(m_coutLevel >= 0) {
    std::cout<<"----parameters for hole transport -----"<<std::endl;
    std::cout<<" m_vs_h        = "<< m_vs_h << std::endl;
    std::cout<<" m_Ec_h        = "<< m_Ec_h << std::endl;
    std::cout<<" m_beta_h      = "<< m_beta_h << std::endl;
  }

  return ;
}

//---------------------------------------------------------------------
//  holeTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::holeTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2 ) {

// transport holes in the bulk 
// T. Kondo, 2010.9.9, 2017.7.20
// External parameters to be specified
//   m_transportTimeMax  [nsec]
//   m_transportTimeStep [nsec]     
//   m_bulk_depth        [cm]
// Induced currents are added to every m_transportTimeStep:
//   Q_m2[200],Q_m1[200],Q_00[200],Q_p1[200],Q_p2[200] 
// 

   double x = x0;  // original hole position [cm]
   double y = y0;  // original hole position [cm]
   bool   isInBulk = true;
   double t_current = 0.;
   double qstrip[5];  // induced charges on strips due to a hole
   double vx, vy, D;
   for (int istrip = -2 ; istrip < 3 ; istrip++) qstrip[istrip+2] = 
           induced(istrip, x, y); // original induced charge bu hole +e 
   while ( t_current < m_transportTimeMax ) {
     if ( !isInBulk ) break;
     if ( !hole( x, y, vx, vy, D)) break ;
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
     y += diffusion * gRandom->Gaus(0.,1.);
     x += diffusion * gRandom->Gaus(0.,1.);
     if( y > m_bulk_depth) {
         y = m_bulk_depth;
         isInBulk = false;  // outside bulk region
     }

//   get induced current by subtracting induced charges
     for (int istrip = -2 ; istrip < 3 ; istrip++) {
        double qnew = induced( istrip, x, y);
        int jj = istrip + 2;
        double dq = qnew - qstrip[jj];
        qstrip[jj] = qnew ;
        if(m_coutLevel>2 && istrip==0) {
           sprintf ( m_cid,"hole : %4.2f ns\tx=%6.2f \ty=%6.2f \tdq[0]=%6.4f"
               , t_current,x*1e4,y*1e4,dq);
           std::cout<<m_cid<<std::endl;
        }

	int jt = int( (t_current+0.001) / m_transportTimeStep) ; 
        if(jt < 200) {
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
   if (m_coutLevel>1) 
      std::cout<<"holeTransport : x,y=("<< x0*1.e4<<","<<y0*1.e4<<")->("
      <<x*1.e4<<"," <<y*1.e4 <<") t="<<t_current<<std::endl;

   return ;
}

//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void SCT_InducedChargedModel::electronTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2 ) {

// transport electrons in the bulk 
// T. Kondo, 2010.9.9, 2017.7.20
// External parameters to be specified
//   m_transportTimeMax  [nsec]
//   m_transportTimeStep [nsec]     
//   m_bulk_depth        [cm]
// Induced currents are added to every m_transportTimeStep:
//   Q_m2[200],Q_m1[200],Q_00[200],Q_p1[200],Q_p2[200] 
// 

   double x = x0;  // original electron position [cm]
   double y = y0;  // original electron position [cm]
   bool isInBulk = true;
   double t_current = 0.;
   double qstrip[5];
   double vx, vy, D;
   for (int istrip = -2 ; istrip < 3 ; istrip++) 
   qstrip[istrip+2] = -induced(istrip, x, y);
   while (t_current < m_transportTimeMax) {
     if ( !isInBulk ) break;
     if ( !electron( x, y, vx, vy, D)) break ;
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
     y += diffusion * gRandom->Gaus(0.,1.);
     x += diffusion * gRandom->Gaus(0.,1.);
     if( y < m_y_origin_min) {
        y = m_y_origin_min;
        isInBulk = false;
     }
//   get induced current by subtracting induced charges
     for (int istrip = -2 ; istrip < 3 ; istrip++) {
        double qnew = -induced( istrip, x, y);
        int jj = istrip + 2;
        double dq = qnew - qstrip[jj];
        qstrip[jj] = qnew ;
        if(m_coutLevel>2 && istrip==0) {
           sprintf ( m_cid,"elec : %4.2f ns\tx=%6.2f \ty=%6.2f \tdq[0]=%6.4f"
               , t_current,x*1e4,y*1e4,dq);
           std::cout<<m_cid<<std::endl;
        }

	int jt = int( (t_current + 0.001) / m_transportTimeStep);
        if(jt< 200) {
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
   if (m_coutLevel>1) 
      std::cout<<"elecTransport : x,y=("<< x0*1.e4<<","<<y0*1.e4<<")->("
      <<x*1.e4<<"," <<y*1.e4 <<") t="<<t_current<<std::endl;

   return ;
}

//---------------------------------------------------------------
//      parameters for electron transport
//---------------------------------------------------------------
bool SCT_InducedChargedModel::electron(double x_e, double y_e, double &vx_e, double &vy_e, double &D_e) {
//double kB= 1.38E-23;       // [m^2*kg/s^2/K]
//double e= 1.602E-19;       // [Coulomb]
double E, Ex, Ey, mu_e, v_e, r_e, tanLA_e, secLA, cosLA, sinLA;
EField(x_e, y_e, Ex, Ey);    // [V/cm]
if( Ey > 0.) {
   double REx = -Ex; // because electron has negative charge 
   double REy = -Ey; // because electron has negative charge
   E = sqrt(Ex*Ex+Ey*Ey);
   mu_e = mud_e(E);
   v_e = mu_e * E;
   r_e = 1.13+0.0008*(m_T-273.15);
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
bool SCT_InducedChargedModel::hole(double x_h, double y_h, double &vx_h, double &vy_h, double &D_h) {
//double kB= 1.38E-23;       // [m^2*kg/s^2/K]
//double e= 1.602E-19;       // [Coulomb]
double E, Ex, Ey, mu_h, v_h, r_h, tanLA_h, secLA, cosLA, sinLA;
EField( x_h, y_h, Ex, Ey);  // [V/cm]
if( Ey > 0.) {
   E = sqrt(Ex*Ex+Ey*Ey);
   mu_h = mud_h(E);
   v_h = mu_h * E;
   r_h = 0.72 - 0.0005*(m_T-273.15);
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
   //cout <<"x,y,iy="<<x*1e4<<" "<<y*1e4<<" "<<iy<<" istrip,xc,dx,ix="
   //<<istrip<<" "<<xc*1e4<<" " <<dx*1e4<<" "<<ix<<" fx,fy="<<fx
   //<<" " <<fy<< ", P="<<P<<endl;

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
   if( y < 0. || y > m_bulk_depth) return;
//---------- case for FEM analysis solution  -------------------------
   if(m_EfieldModel==2) {       
       int iy = int (y/deltay);
       double fy = (y-iy*deltay) / deltay;
       double xhalfpitch=m_strip_pitch/2.;
       double x999 = 999.*m_strip_pitch;
       double xx = fmod (x+x999, m_strip_pitch);
       double xxx = xx;
       if( xx > xhalfpitch ) xxx = m_strip_pitch - xx;
       int ix = int(xxx/deltax) ;
       double fx = (xxx - ix*deltax) / deltax;         
//       std::cout <<"x,y,ix,iy,fx,fy,xx,xxx= "<<x<<" "<<y<<" "<<ix
//       <<" "<<iy<<" "<<fx<<" "<<fy<<" "<<xx<<" "<<xxx<<std::endl;
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
//     std::cout <<"xx, xhalfpitch = "<< xx << " "<< xhalfpitch<<std::endl;
       if(xx > xhalfpitch ) Ex = -Ex;
       Ey = Ey00*(1.-fx)*(1.-fy) + Ey10*fx*(1.-fy)
              + Ey01*(1.-fx)*fy + Ey11*fx*fy ;
       return;
   }

//---------- case for uniform electriv field ------------------------
   if( m_EfieldModel ==0 ) {
       if ( m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0. ) {
          Ey = m_VB / m_depletion_depth ;   
       } else { 
          Ey = 0.;
       }
       return;
   }
//---------- case for flat diode model ------------------------------
   if(m_EfieldModel==1) {       
       if(m_VB > abs(m_VD)) { 
          //Ey = (m_VB+m_VD)/m_depletion_depth 
          Ey = (m_VB+m_VD)/m_bulk_depth 
                - 2.*m_VD*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
       } else {
          if ( m_bulk_depth - y < m_depletion_depth && m_depletion_depth >0.) {
             //double Emax = 2.* m_depletion_depth * m_VD /
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

  TFile *hfile = new TFile( PathResolverFindCalibFile("SCT_Digitization/SCT_InducedChargedModel.root").c_str() );

  h_Potential_FDM = (TH2F *)hfile -> Get("Potential_FDM");
  h_Potential_FEM = (TH2F *)hfile -> Get("Potential_FEM");

  if(m_VD<-180 || m_VD>70 ){
    m_EfieldModel = 1; // Change to FDM
    std::cout<<"Changed to Flat Diode Model since deplettion volage is out of range. (-180 < m_VD < 70 is allow.)"<<std::endl;  
  }

  // For Ramo Potential 
  for (int ix=0 ; ix <81 ; ix++ ){
    for (int iy=0 ; iy<115 ; iy++ ) {
      if(m_EfieldModel==2){ // FEM
	m_PotentialValue[ix][iy] = h_Potential_FEM -> GetBinContent(ix+1,iy+1);
      }else{ // FDM	
	m_PotentialValue[ix][iy] = h_Potential_FDM -> GetBinContent(ix+1,iy+1);
      }
    }
  }

  h_Potential_FDM -> Delete();
  h_Potential_FEM -> Delete();

  if( m_EfieldModel == 2 ){

    float VFD0[9]= { -180, -150, -120, -90, -60, -30, 0, 30, 70};
    char hxname[9][16]= {"Ex_FEM_M180_0","Ex_FEM_M150_0","Ex_FEM_M120_0","Ex_FEM_M90_0",
			 "Ex_FEM_M60_0","Ex_FEM_M30_0","Ex_FEM_0_0","Ex_FEM_30_0","Ex_FEM_70_0",};
    char hyname[9][16]= {"Ey_FEM_M180_0","Ey_FEM_M150_0","Ey_FEM_M120_0","Ey_FEM_M90_0",
			 "Ey_FEM_M60_0","Ey_FEM_M30_0","Ey_FEM_0_0","Ey_FEM_30_0","Ey_FEM_70_0",};
    int iVFD = 0;
    float dVFD1 = 0;
    float dVFD2 = 0;
    for (iVFD=0; iVFD<8; iVFD++)  {
      dVFD1 = m_VD - VFD0[iVFD];
      dVFD2 = VFD0[iVFD+1] - m_VD;
      if( dVFD2 >= 0 ) break;
    }
    
    h_Ex1 = (TH2F *)hfile -> Get( hxname[iVFD] );
    h_Ex2 = (TH2F *)hfile -> Get( hxname[iVFD+1] );
    h_Ey1 = (TH2F *)hfile -> Get( hyname[iVFD] );
    h_Ey2 = (TH2F *)hfile -> Get( hyname[iVFD+1] );
    h_ExHV = (TH2F *)hfile -> Get("Ex_FEM_0_100");
    h_EyHV = (TH2F *)hfile -> Get("Ey_FEM_0_100");

    float scalingFactor = m_VB / 100;

    // For Electric field    
    for (int ix=0 ; ix <17 ; ix++ ){
      for (int iy=0 ; iy<115 ; iy++ ) {
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
    
    h_Ex1 -> Delete();
    h_Ex2 -> Delete();
    h_Ey1 -> Delete();
    h_Ey2 -> Delete();
    h_ExHV -> Delete();
    h_EyHV -> Delete();
  }
  
  hfile -> Close();

}

