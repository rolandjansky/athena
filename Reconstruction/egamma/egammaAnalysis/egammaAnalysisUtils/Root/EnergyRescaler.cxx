/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Internal versions for testing only
///
/// - 29/01/12 : check of new central scales
/// - 03/02/12 : include forward scales, and updated constant terms
///


#include "egammaAnalysisUtils/EnergyRescaler.h"

#include <vector>
#include <iostream>
#include <fstream>

#include <string>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <cctype>


using namespace std;

namespace eg2011 {

///default constructor
EnergyRescaler::EnergyRescaler()
{

   //default seed
   SetRandomSeed();
}


//destructor
EnergyRescaler::~EnergyRescaler()
{


}

void EnergyRescaler::SetRandomSeed( unsigned seed){

   m_random3.SetSeed(seed);

}

  
bool EnergyRescaler::readCalibConstants(std::string fname)
{ 

   
   if( m_corrVec.size()) {
      std::cout<<" WARNING having already  "<<m_corrVec.size()<<"  corrections "<<std::endl;
      m_corrVec.clear();

   }

   
   std::ifstream infile; 
  
   infile.open(fname.c_str()); 


   if(!infile.good()) 
   { 
      cout<<"CANT OPEN CUT FILE " << fname <<" GOING TO EXIT"<<endl; 
      exit(1); 
   }     
   
   std::cout<<"READING FILE  "<< fname<<std::endl;

   calibMap myMap;


   while( !infile.eof() )
   {

      double eta=-10.,etaErr=-10.,  phi=-10., phiErr=-10., alpha=-100., err=-100.;

     

      infile >>eta>>etaErr>>phi>> phiErr>>alpha>>err;
      if( !infile.good()  ) break; 

      myMap.eta= eta;
      myMap.etaBinSize =etaErr;
      myMap.phi = phi;
      myMap.phiBinSize = phiErr;
      myMap.alpha = alpha;
      myMap.alphaErr = err;
     
      m_corrVec.push_back(myMap);


   }
  


   return true; 
} 
 

double EnergyRescaler::applyEnergyCorrectionGeV(double eta, double phi, double energy, double et,  int value, std::string ptype) const
{ 

   double corrEnergy=-999.0;
   
   if(m_corrVec.size()==0)
   {
      std::cout<<"NO CORRECTIONS EXISTS, PLEASE EITHER SUPPLY A CORRECTION FILE OR USE THE DEFAULT CORRECTIONS"<<std::endl;
   }

   for (unsigned int i=0; i< m_corrVec.size(); i++)
   {

      if( 
         eta>=( m_corrVec.at(i).eta - m_corrVec.at(i).etaBinSize/2.) && eta< ( m_corrVec.at(i).eta+m_corrVec.at(i).etaBinSize/2.)  &&
         phi>=( m_corrVec.at(i).phi - m_corrVec.at(i).phiBinSize/2.) && phi< ( m_corrVec.at(i).phi+m_corrVec.at(i).phiBinSize/2.) 
         ) 
      { 

         
        
         for(std::string::iterator p = ptype.begin(); ptype.end() != p; ++p)
         *p = toupper(*p);


         double er_up=-99,er_do=0; 
         double scale=0.;


         switch (value)
         {
            default:
            {
               scale=m_corrVec.at(i).alpha;
               break;
            }
            case NOMINAL:
            {
               scale=m_corrVec.at(i).alpha;
               break;
            }
            case ERR_UP:
            {
               scale=m_corrVec.at(i).alpha;
               getErrorGeV(eta,et, er_up, er_do, ptype);
               scale+=er_do;
               break;
            }
            case ERR_DOWN:
            {
               scale=m_corrVec.at(i).alpha;
               getErrorGeV(eta,et, er_up, er_do, ptype);
               scale+=er_up;
               break;
            }
         }

        

         corrEnergy =  energy/(1.+ scale);

         // std::cout<<" eta : "<<eta <<" uncorrected energy : "<<    energy <<" corr energy : "<< corrEnergy<<" scale : "<< m_corrVec.at(i).alpha <<endl;
         break;
      }
   }

   if( corrEnergy==-999.)return energy;
   else return corrEnergy;

} 


std::vector<float> EnergyRescaler::applyEnergyCorrectionMeV(std::vector<float> cl_eta, 
   std::vector<float> cl_phi, std::vector<float> cl_E, std::vector<float> eta, 
   std::vector<int> ph_isConv/*=EMPTY*/, int value/*=NOMINAL */ ) const
{
   std::vector<float> result;
   std::string part_type = "ELECTRON";
   for (unsigned int i = 0; i < cl_eta.size(); ++i)
   {
      if (ph_isConv.size())
         part_type = (ph_isConv[i] ? "CONVERTED_PHOTON" : "UNCONVERTED_PHOTON");
      result.push_back( applyEnergyCorrectionMeV(cl_eta[i], cl_phi[i], cl_E[i], 
         cl_E[i]/cosh(eta[i]), value, part_type) );
   }
   return result;
}


void EnergyRescaler::getErrorGeV(double cl_eta,double cl_et, double &er_up, double &er_do, std::string ptype,bool withXMAT,bool withPS) const
{

  er_up=-1;
  er_do=-1;

  static const int nbins=8;
  static double boundaries[nbins+1]={0,0.6,1.00,1.37,1.52,1.8,2.47,3.2,4.9};

  //systematics 

  static double stat[nbins]               ={0.0010, 0.0020, 0.0020, 777, 0.0020, 0.0020, 0.005, 0.01};
  static double sys_mcclosure[nbins]      ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.002, 0.002};
  static double sys_comparison[nbins]     ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.01, 0.008}; 
  static double sys_pileup[nbins]         ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001}; //check forward? 
  static double sys_medium2tight_up[nbins]={0.0010, 0.0010, 0.0010, 777, 0.0020, 0.0020, 0,0};
  static double sys_loose2tight_forward[nbins]    ={0.0000, 0.0000, 0.0000, 777, 0.0000, 0.0000, 0.012, 0.01};

  static double sys_masscut[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0020, 0.0020, 0.002, 0.006};
  static double sys_elecLin[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001};//forward?
  static double sys_xtalkE1[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001};//forward?
  static double sys_lowpt  [nbins]        ={0.0100, 0.0100, 0.0100, 777, 0.0100, 0.0100, 0.01,  0.01};
  static double sys_HV     [nbins]        ={0.0000, 0.0000, 0.0000, 777, 0.0000, 0.0000, 0.006, 0.008};

  static double PS_B[nbins]={-0.00025312, -0.0011569, -0.00211677, 777, -0.00175762*2, 000, 000, 000};
 
  static double elec_XMAT_a[nbins] = {-0.0083,  -0.013,	-0.025,	777, -0.023, -0.019, -0.042, -0.014  };
  static double elec_XMAT_b[nbins] = {-0.055, -0.019, -0.014, 777, -0.026, -0.019, -0.041, -0.016};
  static double elec_XMAT_MAX[nbins]={  0.003,  0.008,  0.015,  777,   0.010,   0.01  ,0.01, 0.01};  

  static double pho_XMAT_MAX[nbins]={  0.003,  0.005,  0.010,  777,   0.010,   0.01  ,0, 0.} ;
  static double pho_PS_shift[nbins] ={  0.001,  0.002,  0.003,  777,   0.002*2,   0.000  ,0, 0.} ;

  // find bin 

  int bin =-1;
  for(int i=0;i<nbins;i++) {

    if(fabs(cl_eta)>=boundaries[i] && fabs(cl_eta)<boundaries[i+1]) {
      bin=i;
      break;
    }
    
  }
  if(bin==-1) return;

  //crack region

  if(bin==3) { 

    er_up=0.05;
    er_do=-0.05;
    return;

  }

  //PS

  double PS_up=0,PS_do=0;
  
  if(withPS==true) {          

    if(abs(cl_eta)<1.8) {

      double shift=0;
      if(ptype=="UNCONVERTED_PHOTON" || ptype =="CONVERTED_PHOTON")
	shift=pho_PS_shift[bin];

      double PS=PS_B[bin]*(log(cl_et)-log(40.))-shift;
      
      if(PS>=0) {
	PS_up=PS;
	PS_do=-PS;
      } else {
	PS_up=-PS;
	PS_do=PS;
      }
    }  
  }

  // material

  double XMat_up = 0;
  double XMat_do = 0;
  
  if(withXMAT==true) {
      
    if(ptype=="ELECTRON") {

      double xmat= 0;
      xmat= elec_XMAT_a[bin]*exp(cl_et* elec_XMAT_b[bin])+elec_XMAT_MAX[bin];
      
      if(xmat>0)
	XMat_up=xmat;
      else  
	XMat_do=xmat;
    
    } else if(ptype=="UNCONVERTED_PHOTON" || ptype =="CONVERTED_PHOTON") {      

      XMat_up=pho_XMAT_MAX[bin];
      XMat_do=0;

    }                 
  }

  //lowpt
  
  double lowpt=0;
  if(cl_et<20)
    lowpt =sys_lowpt[bin]/(10-20)*(cl_et-20);

  // done
  
  er_up= sqrt(stat[bin]*stat[bin]+
 	      sys_mcclosure[bin]*sys_mcclosure[bin]+
 	      sys_comparison[bin]*sys_comparison[bin]+
 	      sys_pileup[bin]*sys_pileup[bin]+
  	      sys_medium2tight_up[bin]*sys_medium2tight_up[bin]+
	      sys_loose2tight_forward[bin]*sys_loose2tight_forward[bin]+
  	      sys_masscut[bin]*sys_masscut[bin]+
  	      sys_HV[bin]*sys_HV[bin]+
  	      sys_elecLin[bin]*sys_elecLin[bin]+
	      sys_xtalkE1[bin]*sys_xtalkE1[bin]+
 	      PS_up*PS_up+
 	      XMat_up*XMat_up+
	      lowpt*lowpt);
  
  er_do= -sqrt(stat[bin]*stat[bin]+
	      sys_mcclosure[bin]*sys_mcclosure[bin]+
	      sys_comparison[bin]*sys_comparison[bin]+
	      sys_pileup[bin]*sys_pileup[bin]+
	      sys_loose2tight_forward[bin]*sys_loose2tight_forward[bin]+
	      sys_masscut[bin]*sys_masscut[bin]+
  	      sys_HV[bin]*sys_HV[bin]+
	      sys_elecLin[bin]*sys_elecLin[bin]+
	      sys_xtalkE1[bin]*sys_xtalkE1[bin]+
	      PS_do*PS_do+
	      XMat_do*XMat_do+
	      lowpt*lowpt);

  return;

}



void EnergyRescaler::getErrorGeV_PS(double cl_eta,double cl_et, double &er_up, double &er_do, std::string ptype) const
{
  er_up=-1;
  er_do=-1;

  static const int nbins=8;
  static double boundaries[nbins+1]={0,0.6,1.00,1.37,1.52,1.8,2.47,3.2,4.9};

  //systematics 

  static double PS_B[nbins]={-0.00025312, -0.0011569, -0.00211677, 777, -0.00175762*2, 000, 000, 000};
  static double pho_PS_shift[nbins] ={  0.001,  0.002,  0.003,  777,   0.002*2,   0.000  ,0, 0.} ;

  // find bin 

  int bin =-1;
  for(int i=0;i<nbins;i++) {

    if(fabs(cl_eta)>=boundaries[i] && fabs(cl_eta)<boundaries[i+1]) {
      bin=i;
      break;
    }

  }
  if(bin==-1) return;

  //crack region

  if(bin==3) { 

    er_up=0.05;
    er_do=-0.05;
    return;

  }

  // PS

  double PS_up=0, PS_do=0;
  
  if(abs(cl_eta)<1.8) {

    double shift=0;
    if(ptype=="UNCONVERTED_PHOTON" || ptype =="CONVERTED_PHOTON")
      shift=pho_PS_shift[bin];
    
    double PS=PS_B[bin]*(log(cl_et)-log(40.))-shift;

    if(PS>=0) {
      PS_up=PS;
      PS_do=-PS;
    } else {
      PS_up=-PS;
      PS_do=PS;
    }

  }
      
  // done

  er_up= PS_up;  
  er_do= PS_do;
  return;

}


void EnergyRescaler::getErrorGeV_MAT(double cl_eta,double cl_et, double &er_up, double &er_do, std::string ptype) const
{
  er_up=-1;
  er_do=-1;

  static const int nbins=8;
  static double boundaries[nbins+1]={0,0.6,1.00,1.37,1.52,1.8,2.47,3.2,4.9};

  //systematics 

  static double elec_XMAT_a[nbins]   = {-0.0083,  -0.013, -0.025, 777, -0.023, -0.019, -0.042, -0.014 };
  static double elec_XMAT_b[nbins]   = { -0.055,  -0.019, -0.014, 777, -0.026, -0.019, -0.041, -0.016 };
  static double elec_XMAT_MAX[nbins] = {  0.003,   0.008,  0.015, 777,  0.010,   0.01,   0.01,   0.01 };  
  static double pho_XMAT_MAX[nbins]  = {  0.003,   0.005,  0.010, 777,  0.010,   0.01,      0,     0. } ;

  // find bin
 
  int bin =-1;
  for(int i=0;i<nbins;i++) {

    if(fabs(cl_eta)>=boundaries[i] && fabs(cl_eta)<boundaries[i+1]) {
      bin=i;
      break;
    }

  }
  if(bin==-1) return;

  //crack region

  if(bin==3) { 

    er_up=0.05;
    er_do=-0.05;
    return;

  }

  //material

  double XMat_up = 0;
  double XMat_do = 0;
  
  if(ptype=="ELECTRON") {

    double xmat=0;
    xmat= elec_XMAT_a[bin]*exp(cl_et* elec_XMAT_b[bin])+elec_XMAT_MAX[bin];
    if(xmat>0)
      XMat_up=xmat;
    else  
      XMat_do=xmat;
	  
  } else if(ptype=="UNCONVERTED_PHOTON" || ptype =="CONVERTED_PHOTON") {      

    XMat_up=pho_XMAT_MAX[bin];
    XMat_do=0;

  }                 

  // done

  er_up = XMat_up;  
  er_do = XMat_do;
  return;

}


void EnergyRescaler::getErrorGeV_REST(double cl_eta,double cl_et, double &er_up, double &er_do, std::string /*ptype*/) const
{
  er_up=-1;
  er_do=-1;

  static const int nbins=8;
  static double boundaries[nbins+1]={0,0.6,1.00,1.37,1.52,1.8,2.47,3.2,4.9};

  // systematics 

  static double stat[nbins]               ={0.0010, 0.0020, 0.0020, 777, 0.0020, 0.0020, 0.005, 0.01};
  static double sys_mcclosure[nbins]      ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.002, 0.002};
  static double sys_comparison[nbins]     ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.01, 0.008}; 
  static double sys_pileup[nbins]         ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001}; //check forward? 
  static double sys_medium2tight_up[nbins]={0.0010, 0.0010, 0.0010, 777, 0.0020, 0.0020, 0,0};
  static double sys_loose2tight_forward[nbins]    ={0.0000, 0.0000, 0.0000, 777, 0.0000, 0.0000, 0.012, 0.01};

  static double sys_masscut[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0020, 0.0020, 0.002, 0.006};
  static double sys_elecLin[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001};//forward?
  static double sys_xtalkE1[nbins]        ={0.0010, 0.0010, 0.0010, 777, 0.0010, 0.0010, 0.001, 0.001};//forward?
  static double sys_lowpt  [nbins]        ={0.0100, 0.0100, 0.0100, 777, 0.0100, 0.0100, 0.01,  0.01};
  static double sys_HV     [nbins]        ={0.0000, 0.0000, 0.0000, 777, 0.0000, 0.0000, 0.006, 0.008};

  // find bin

  int bin =-1;
  for(int i=0;i<nbins;i++) {

    if(fabs(cl_eta)>=boundaries[i] && fabs(cl_eta)<boundaries[i+1]) {
      bin=i;
      break;
    }

  }
  if(bin==-1) return;

  //crack region

  if(bin==3)
    { 
      er_up=0.05;
      er_do=-0.05;
      return;
    }

  //lowpt

  double lowpt=0;
  if(cl_et<20)
    lowpt =sys_lowpt[bin]/(10-20)*(cl_et-20);

  // done

  er_up= sqrt(stat[bin]*stat[bin]+
 	      sys_mcclosure[bin]*sys_mcclosure[bin]+
 	      sys_comparison[bin]*sys_comparison[bin]+
 	      sys_pileup[bin]*sys_pileup[bin]+
  	      sys_medium2tight_up[bin]*sys_medium2tight_up[bin]+
	      sys_loose2tight_forward[bin]*sys_loose2tight_forward[bin]+
  	      sys_masscut[bin]*sys_masscut[bin]+
  	      sys_HV[bin]*sys_HV[bin]+
  	      sys_elecLin[bin]*sys_elecLin[bin]+
	      sys_xtalkE1[bin]*sys_xtalkE1[bin]+
 	      lowpt*lowpt);
  
  er_do= -sqrt(stat[bin]*stat[bin]+
	      sys_mcclosure[bin]*sys_mcclosure[bin]+
	      sys_comparison[bin]*sys_comparison[bin]+
	      sys_pileup[bin]*sys_pileup[bin]+
	      sys_loose2tight_forward[bin]*sys_loose2tight_forward[bin]+
	      sys_masscut[bin]*sys_masscut[bin]+
  	      sys_HV[bin]*sys_HV[bin]+
	      sys_elecLin[bin]*sys_elecLin[bin]+
	      sys_xtalkE1[bin]*sys_xtalkE1[bin]+
	      lowpt*lowpt);

  return;

}









///// Calibration constants

bool  EnergyRescaler::useDefaultCalibConstants( std::string corr_version)
{

  m_corr_version = corr_version;
    
  const int netaBins=58;
  const int nphiBins=58;
  
  
  double m_2pi = 2.*acos(-1.); 
  //cout<<" pi : "<<acos(-1.)<<"  M_PI : "<<M_PI<<" 2pi : "<<m_2pi<<endl;
  

  if( m_corrVec.size()) {
    std::cout<<" WARNING having already  "<<m_corrVec.size()<<"  corrections "<<std::endl;
    m_corrVec.clear();
    
  }
   

   

  
   double eta_tmp[netaBins]={   
      -4.45, -3.60, -3.00, -2.65, -2.435, -2.35, -2.25, -2.15, -2.05, -1.95, -1.85,	-1.75,	-1.65,
      -1.56, -1.445,	-1.335,	-1.25, -1.15,	-1.05,	-0.95,	-0.85,	-0.75,	-0.65,	-0.55,	-0.45,	-0.35,
      -0.25, -0.15,	-0.05,	0.05,  0.15,	0.25,	0.35,	0.45,	0.55,	0.65,	0.75,	0.85,	0.95,
       1.05, 1.15,	1.25,	1.335, 1.445,	1.56,	1.65,	1.75,	1.85,	1.95,	2.05,	2.15,	2.25,
       2.35,	2.435,   2.65,	3.00,	3.60,	4.45
         };
   

   


    double etaBinWidth_tmp[netaBins]= {
       0.9, 0.8, 0.4, 0.3, 0.07,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.08,	0.15,	0.07,	0.1,	0.1,	0.1,	        0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	        0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.1,	0.07,	0.15,	0.08,	0.1,	0.1,	0.1,	0.1,	        0.1,	0.1,	0.1,	0.1,	0.07,    0.3,	0.4,	0.8,	0.9 };

   
    

   
    double phi_tmp[nphiBins]={
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 	0, 0, 0, 0, 0, 0, 0, 0,	0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0 };

    double phiBinWidth_tmp[nphiBins]={
       m_2pi, m_2pi, m_2pi, m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,
       m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi, m_2pi,m_2pi,m_2pi,m_2pi,
       m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi, m_2pi,
       m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,m_2pi,
       m_2pi,m_2pi, m_2pi,m_2pi,m_2pi,m_2pi,m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi };


    
    

   double alpha_tmp[netaBins]= {
    0.046962, 0.044332,	-0.007777,-0.045341, -0.053683,-0.005040,0.003357,0.003830,0.016857,0.012810,
    0.008882,-0.016864,-0.021280,-0.011215,-0.008958,-0.010248, 0.005199,0.007815,-0.000701,0.003837,0.003222,
    -0.004017,-0.005908,-0.009560,-0.005382,-0.012158,-0.005060,-0.008274, -0.006701,-0.002734,-0.012920, 
    -0.010972,-0.006823,-0.007234,-0.002612,-0.004301,0.001580,-0.001986,-0.001306,0.005748, 0.002906, 
    0.001381,-0.001584,0.000799,-0.002511,-0.030902,-0.016416,-0.004976,0.002408,0.018706,-0.004309,-0.002673,
    -0.001735,-0.050173, -0.034703,	-0.003296,	0.047351,	0.028374
   };

   

   

   double alphaError_tmp[netaBins]={ 
      0.019646, 0.011500, 0.008761, 0.009514, 0.008341,0.006345,0.005522,0.005089,0.005387,0.005861,
      0.005821,0.005119,0.006227,0.009148,0.003920,0.004309,0.002929,  0.003882,0.004054,0.003716,0.003673,
      0.003832,0.003275,0.002075,0.004004,0.002497,0.003182,0.002512,0.003974,0.002302, 0.003670,0.003322,
      0.003978,0.002164,0.001984,0.002093,0.002372,0.003843,0.004138,0.004277,0.003003,0.004690,0.005480,
      0.006306,	0.007338,0.005939,0.004472,0.004535,0.005453,0.008538,0.004554,0.003382,0.005504,0.007577,
      0.010095,	0.009122,	0.013400,	0.02588
   };


   ////// NUMBERS FOR 2011 DATA

   const int netaBinsFor2011=32;
   const int nphiBinsFor2011=32;

   double etaFor2011_tmp[netaBinsFor2011]={ 
     -4.05,    -3.00,  -2.65,  
     -2.385, -2.2,   -2.00,  -1.8,   -1.61,  -1.445, -1.285, -1.1,   -0.9,   -0.7,   -0.5,   -0.3,   -0.1,   
     0.1,    0.3,    0.5,    0.7,    0.9,    1.1,    1.285,  1.445,  1.61,   1.8,    2.0,    2.2,    2.385,  
     2.65,   3.0,    4.05
   };
	
   double etaBinWidthFor2011_tmp[netaBinsFor2011]= {
     1.7,     0.4,    0.3,  
     0.17,     0.2,    0.2,    0.2,    0.18,   0.15,   0.17,   0.2,    0.2,    0.2,    0.2,    0.2,    0.2,    
     0.2,    0.2,    0.2,    0.2,    0.2,    0.2,    0.17,   0.15,   0.18,   0.2,    0.2,    0.2,    0.17,   
     0.3,    0.4,    1.7
   };
	
   double phiFor2011_tmp[nphiBinsFor2011]={
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
   };
	
   double phiBinWidthFor2011_tmp[nphiBinsFor2011]={
     m_2pi, m_2pi, m_2pi, 
     m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, 
     m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi,
     m_2pi, m_2pi, m_2pi
   };
	



    double alphaFor2011_Prel_Jan2012[netaBinsFor2011]= {

      // current central and forward scales are wrt mc11c
      // commented numbers are wrt mc11a

       0.02250430,
       -0.00374902,
       0.02535370,
       0.0037675,         //   0.00464523,   
       0.00490447,        //   0.0057822,    
       0.005687,          //   0.00656473,   
       0.00802105,        //   0.00889878,   
       -0.00528993,       //  -0.0044122,  
       -0.0116496,        //  -0.0107719,   
       0.00592598,        //   0.00680371,   
       0.00247059,        //   0.00334832,   
       -0.000855526,      //   2.22039e-05,  
       0.00509505,        //   0.00597278,   
       0.00583526,        //   0.00671299,   
       -0.00340543,       //  -0.0025277,  
       -0.00348403,       //  -0.0026063,  
       -0.00196953,       //  -0.0010918,  
       -0.00182113,       //  -0.0009434, 
       0.00389051,        //   0.00476824,   
       0.00538199,        //   0.00625972,    
       0.00098167,        //   0.0018594,   
       0.00467596,        //   0.00555369,   
       0.00733492,        //   0.00821265,    
       -0.00631553,       //  -0.0054378,    
       -0.0114566,        //  -0.0105789,    
       -4.104e-05,        //   0.00083669,  
       -0.00356843,       //  -0.0026907,  
       -0.00392083,       //  -0.0030431,  
       -0.00479783,       //  -0.0039201,  
       0.00541310,
       -0.00480631,
       0.01122000
       
    };
   

    double alphaErrorFor2011_Prel_Jan2012[netaBinsFor2011]={ 

      0.002444877,
      0.004809635,
      0.003156490,
      0.000470342,        
      0.000374852,       
      0.000359052,          
      0.000374378,         
      0.00048418,          
      0.000597172,           
      0.000350214,         
      0.000294693,         
      0.000276543,        
      0.000253397,        
      0.000242044,         
      0.000241598,         
      0.00025457,           
      0.000242443,           
      0.00024981,           
      0.000243194,           
      0.000263781,        
      0.000281633,         
      0.000302674,          
      0.000370169,         
      0.000620942,         
      0.000500065,          
      0.000378803,          
      0.000365644,          
      0.0003695,            
      0.000474544,            
      0.006143817,
      0.005388449,
      0.003915748

   };



   //  double alphaFor2011_Prel_May2012[netaBinsFor2011]= { // 40 BINS HERE

   //    0.02250430,
   //    -0.00374902,
   //    0.02535370,
   //    -0.0147019,
   //    0.0163883,
   //    0.00461368,
   //    0.00628725,
   //    0.00526275,
   //    0.0182814,
   //    -0.00859598,
   //    0.0109223,
   //    -0.0414589,
   //    -0.00238195,
   //    0.00614168,
   //    0.00339483,
   //    6.33105e-05,
   //    0.00561867,
   //    0.00652951,
   //    -0.00160831,
   //    -0.0024357,
   //    -0.00158406,
   //    -0.0013337,
   //    0.00440734,
   //    0.00602478,
   //    0.00158447,
   //    0.00579444,
   //    0.00800333,
   //    0.00132747,
   //    -0.0323968,
   //    0.00262434,
   //    -0.0152283,
   //    0.0110142,
   //    -0.00427106,
   //    -0.00227786,
   //    -0.00354066,
   //    0.00636169,
   //    -0.0251212,
   //    0.00541310,
   //    -0.00480631,
   //    0.01122000
       
   //  };
   

   //  double alphaErrorFor2011_Prel_May2012[netaBinsFor2011]={  // 40 BINS HERE

   //    0.002444877,
   //    0.004809635,
   //    0.003156490,
   //    0.000852751,
   //    0.000623794,
   //    0.00038859,
   //    0.000534573,
   //    0.000398359,
   //    0.000591631,
   //    0.000597459,
   //    0.000683449,
   //    0.00091066,
   //    0.000706578,
   //    0.000360275,
   //    0.000307039,
   //    0.000284897,
   //    0.000262409,
   //    0.000248118,
   //    0.000245534,
   //    0.000250436,
   //    0.000253467,
   //    0.000245397,
   //    0.000249462,
   //    0.000261962,
   //    0.000287884,
   //    0.00030839,
   //    0.000364282,
   //    0.000693336,
   //    0.00109101,
   //    0.000681563,
   //    0.000612748,
   //    0.000587799,
   //    0.000400253,
   //    0.000537004,
   //    0.000388996,
   //    0.000625801,
   //    0.000849176,
   //    0.006143817,
   //    0.005388449,
   //    0.003915748

   // };



   ////// NUMBERS FOR 2012 DATA

   const int netaBinsFor2012=40;
   const int nphiBinsFor2012=40;

   double etaFor2012_tmp[netaBinsFor2012]={ 
     -4.05,	-3.00,	-2.65,	
     -2.435, -2.35, -2.2, -2.05, -1.91, -1.78, -1.685, -1.59, -1.51, -1.42, -1.285, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1, 
     0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.285, 1.42, 1.51, 1.59, 1.685, 1.78, 1.91, 2.05, 2.2, 2.35, 2.435,
     2.65,	3.0,	4.05
   };
   
    double etaBinWidthFor2012_tmp[netaBinsFor2012]= {
      1.7,	0.4,	0.3,  
      0.07, 0.1, 0.2, 0.1, 0.18, 0.08, 0.11, 0.08, 0.08, 0.1, 0.17, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2,
      0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.17, 0.1, 0.08, 0.08, 0.11, 0.08, 0.18, 0.1, 0.2, 0.1, 0.07,
      0.3,	0.4,	1.7
    };

    double phiFor2012_tmp[nphiBinsFor2012]={
      0, 0, 0, 
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0
    };

    double phiBinWidthFor2012_tmp[nphiBinsFor2012]={
      m_2pi, m_2pi, m_2pi, 
      m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi,
      m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi, m_2pi,
      m_2pi, m_2pi, m_2pi
    };


    double alphaFor2012_Prel_May2012[netaBinsFor2012]= {

      0.02250430,
      -0.00374902,
      0.02535370,
      -0.0068463,   // //-0.0066378,   //	-0.0103205,
      0.0125296,    // //0.0125153,    //	0.0123651,
      0.00424258,   // //0.00427124,   //	0.00498448,
      0.00694939,   // //0.00616575,   //	0.00630754,
      0.00211035,   // //0.00211782,   //	0.00138981,
      0.0171655,    // //0.0171367,    //	0.017964,
      -0.0125889,   // //-0.0124447,   //	-0.0126705,
      0.00698708,   // //0.00690754,   //	0.00523333,
      -0.0429252,   // //-0.0428683,   //	-0.0433733,
      -0.00888626,  // //-0.00893573,  //	-0.00830459,
      0.00410461,   // //0.00404838,   //	0.00238184,
      0.00111227,   // //0.00109154,   //	0.000823772,
      -0.00219195,  // //-0.00215747,  //	-0.00152048,
      0.00401976,   // //0.00411705,   //	0.00470086,
      0.00475353,   // //0.00477888,   //	0.00511869,
      -0.00482295,  // //-0.00485873,  //	-0.00543027,
      -0.00482355,  // //-0.00474785,  //	-0.00480945,
      -0.00345603,  // //-0.00341408,  //	-0.00377731,
      -0.00265316,  // //-0.00264677,  //	-0.00149513,
      0.00351433,   // //0.00342807,   //	0.00378859,
      0.00467427,   // //0.00491098,   //	0.00515336,
      -0.000494865, // //-0.000326698, //-0.000545532,
      0.00177284,   // //0.00169479,   //	0.00242343,
      0.00574647,   // //0.00568978,   //	0.00672731,
      -0.00922279,  // //-0.00922903,  //	-0.00997527,
      -0.0304822,   // //-0.0308524,   //	-0.0263691,
      -0.00491104,  // //-0.0049559,   //	-0.00432935,
      -0.0201955,   // //-0.0202624,   //	-0.0197593,
      0.00889966,   // //0.00898453,   //	0.00907997,
      -0.00735878,  // //-0.00722692,  //	-0.0075024,
      -0.00322852,  // //-0.00321507,  //	-0.00235722,
      -0.00542245,  // //-0.00546923,  //	-0.00555714,
      0.00225772,   // //0.00223877,   //	0.00385795,
      -0.018385,    // //-0.0184939,   //  -0.021102,
      0.00541310,
      -0.00480631,
      0.01122000
       
    };

    double alphaErrorFor2012_Prel_May2012[netaBinsFor2012]={ 

      0.002444877,
      0.004809635,
      0.003156490,
      0.00215173,  //0.00219706, //	0.00424077,
      0.000878076, //0.000880354, //	0.00121341,
      0.000482064, //0.000480182, //	0.00070409,
      0.000643453, //0.000673926, //	0.000992458,
      0.000507243, //0.000504225, //	0.000692405,
      0.000783798, //0.000782447, //	0.00112586,
      0.000793928, //0.000794224, //	0.00113855,
      0.00082132,  //0.000819359, //	0.00122294,
      0.00129539,  //0.00129918, //	0.00190648,
      0.000772966, //0.000773671, //	0.00110393,
      0.000425016, //0.00045473, //	0.000648396,
      0.000377662, //0.000377452, //	0.00056016,
      0.00036721,  //0.000366862, //	0.000525696,
      0.000339747, //0.0003376, //	0.000481844,
      0.000318085, //0.000316058, //	0.000445528,
      0.000307969, //0.00031033, //	0.00044413,
      0.000312218, //0.000307729, //	0.000441377,
      0.000311352, //0.000310442, //	0.000447734,
      0.000303208, //0.000309018, //	0.000447901,
      0.000316238, //0.000317767, //	0.000366621,
      0.000332868, //0.000297678, //	0.000374013,
      0.000359659, //0.00036149, //	0.000528825,
      0.000374383, //0.000376057, //	0.000530049,
      0.000453634, //0.000453116, //	0.000657645,
      0.000762685, //0.00076507, //	0.00111284,
      0.00120956,  //0.00122165, //	0.00176914,
      0.000823844, //0.000828228, //	0.00118927,
      0.000792373, //0.000796053, //	0.00112935,
      0.000764645, //0.000762326, //	0.00113188,
      0.000479664, //0.000495339, //	0.000713627,
      0.000686952, //0.00069044, //	0.000964883,
      0.000489275, //0.000488512, //	0.000692802,
      0.000927753, //0.000909586, //	0.00138773,
      0.00298658,  //0.00298355, //  0.00788777,
      0.006143817,
      0.005388449,
      0.003915748

   };


    calibMap myMap;
    
    if( corr_version=="2011" ){
       
       for(int i=0; i<netaBinsFor2011; i++) {

          myMap.eta = etaFor2011_tmp[i];
          myMap.etaBinSize = etaBinWidthFor2011_tmp[i];

          myMap.phi = phiFor2011_tmp[i];
          myMap.phiBinSize = phiBinWidthFor2011_tmp[i];

          myMap.alpha = alphaFor2011_Prel_Jan2012[i];
          myMap.alphaErr = alphaErrorFor2011_Prel_Jan2012[i];

          m_corrVec.push_back(myMap);

       }
        
    } else if( corr_version=="2012" ) {


       for(int i=0; i<netaBinsFor2012; i++) {

          myMap.eta = etaFor2012_tmp[i];
          myMap.etaBinSize = etaBinWidthFor2012_tmp[i];

          myMap.phi = phiFor2012_tmp[i];
          myMap.phiBinSize = phiBinWidthFor2012_tmp[i];

          myMap.alpha = alphaFor2012_Prel_May2012[i];
          myMap.alphaErr = alphaErrorFor2012_Prel_May2012[i];

          m_corrVec.push_back(myMap);

       }

    } else {
      
       for(int i=0; i<netaBins; i++){

          myMap.eta= eta_tmp[i];
          myMap.etaBinSize =etaBinWidth_tmp[i];

          myMap.phi = phi_tmp[i];
          myMap.phiBinSize = phiBinWidth_tmp[i];

          myMap.alpha = alpha_tmp[i];
          myMap.alphaErr = alphaError_tmp[i];

          m_corrVec.push_back(myMap);

       }


    }


   return true;
}



bool EnergyRescaler::printMap() const
{

   for (unsigned int i=0; i< m_corrVec.size(); i++)
   {
      std::cout<<"eta :  "<< m_corrVec.at(i).eta <<
         " etaErr : " <<m_corrVec.at(i).etaBinSize<<
         " phi    :  "<<m_corrVec.at(i).phi<<
         " phiErr :  "<<m_corrVec.at(i).phiBinSize<<
         " alpha  :  "<<m_corrVec.at(i).alpha<<
         " alphaErr : "<<m_corrVec.at(i).alphaErr<<endl;
   }


   return true;

}


// NEW FUNCTIONS START HERE (MB)


// sampling term inMC, parametrization from Iro

double EnergyRescaler::mcSamplingTerm(double cl_eta) {

  double aeta = fabs( cl_eta );
  double sampling = 0.;

  if ( aeta<0.8 )
    sampling = 0.091;

  else if ( aeta<1.37 )
    sampling = 0.036 + 0.130 * aeta;

  else if ( aeta<1.52 )
    sampling = 0.27;

  else if ( aeta<2.0 )
    sampling = 0.85 - 0.36 * aeta;

  else if ( aeta<2.3 )
    sampling = 0.16;
  
  else if ( aeta<2.5 )
    sampling = -1.05 + 0.52 * aeta;

  return sampling;

}


// sampling term uncertainty

double EnergyRescaler::mcSamplingTermRelError( double cl_eta ) {

  cl_eta = cl_eta*1.;
  double relerr = 0.1;

  return relerr;

}


// noise term in MC (from Iro)

double EnergyRescaler::mcNoiseTerm( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double noise = 0.;

  double noise37[25]={ 0.27, 0.27, 0.27, 0.27, 0.27, 
		       0.26, 0.25, 0.23, 0.21, 0.19, 
		       0.17, 0.16, 0.15, 0.14, 0.27, 
		       0.23, 0.17, 0.15, 0.13, 0.10, 
		       0.07, 0.06, 0.05, 0.04, 0.03 };  

  int ieta = (int) (aeta/0.1);

  if ( ieta<25 )
    noise =  noise37[ieta];

  return noise;

}


// constant term in MC (local)

double EnergyRescaler::mcConstantTerm( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double cst = 0.;

  if ( aeta<0.6 )
    cst = 0.005;

  else if ( aeta<1.75 )
    cst = 0.003;

  else if ( aeta<2.5 )
    cst = 0.0055 * (2.69 - aeta);

  //cst = 0.005;

  return cst;

}


// constant term fitted in data (long range)

double EnergyRescaler::dataConstantTerm( double cl_eta ) {

  double cst = 0.;

  if ( cl_eta<-3.2  )
    cst = 0.0285 ;

  else if ( cl_eta<-2.8  )
    cst = 0.0363 ;

  else if ( cl_eta<-2.47  )
    cst = 0.0198 ;

  else if ( cl_eta<-1.8  )
    cst = 0.01241325;

  else if ( cl_eta<-1.52  )
    cst = 0.0211967;

  else if ( cl_eta<-1.37 )
    cst = 0.0295445;

  else if ( cl_eta<-0.8 )
    cst = 0.00609462;

  else if ( cl_eta<-0.4 )
    cst = 0.0110266;

  else if ( cl_eta<0 )
    cst = 0.009217095;

  else if ( cl_eta<0.4 )
    cst = 0.009217095;

  else if ( cl_eta<0.8 )
    cst = 0.0110266;

  else if ( cl_eta<1.37 )
    cst = 0.00609462;

  else if ( cl_eta<1.52  )
    cst = 0.0243573;

  else if ( cl_eta<1.8  )
    cst = 0.0211967;

  else if ( cl_eta<2.47 )
    cst = 0.01241325;

  else if ( cl_eta<2.8 )
    cst = 0.0258 ;

  else if ( cl_eta<3.2 )
    cst = 0.0374 ;

  else if ( cl_eta<4.9 )
    cst = 0.0289 ;

  return cst;

}


double EnergyRescaler::dataConstantTermError( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double err = 0.;

  if ( aeta<1.37 )
    err = 0.006;

  else if ( aeta<2.47 )
    err = 0.006;
  
  else if ( aeta<3.2 )
    err = 0.011;
  
  else if ( aeta<4.9 )
    err = 0.010;
  
  return err;
}


double EnergyRescaler::dataConstantTermUpError( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double err = 0.;

  if ( aeta<1.37 )
    err = 0.005;

  else if ( aeta<2.47 )
    err = 0.006;
  
  else if ( aeta<3.2 )
    err = 0.011;
  
  else if ( aeta<4.9 )
    err = 0.010;
  
  return err;
}


double EnergyRescaler::dataConstantTermDownError( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double err = 0.;

  if ( aeta<1.37 )
    err = -0.006;

  else if ( aeta<2.5 )
    err = -0.006;
  
  else if ( aeta<3.2 )
    err = -0.011;
  
  else if ( aeta<4.9 )
    err = -0.015;
  
  return err;
}


// fitted Z peak resolution, data, in GeV

double EnergyRescaler::dataZPeakResolution( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double res = 0.;

  if ( aeta<1.37 )
    res = 1.62;

  else if ( aeta<2.5 )
    res = 1.99;

  return res;

}


// fitted Z peak resolution, MC, in GeV

double EnergyRescaler::mcZPeakResolution( double cl_eta ) {

  double aeta = fabs( cl_eta );
  double res = 0.;

  if ( aeta<1.37 )
    res = 1.45;

  else if ( aeta<2.5 )
    res = 1.63;

  return res;

}


// correlated part of constant term uncertainty, in data (approx.)

double EnergyRescaler::dataConstantTermCorError( double cl_eta ) {

  double mz = 91.2;
  
  double resData = dataZPeakResolution( cl_eta );
  double resMC   = mcZPeakResolution( cl_eta );
  double cmc     = mcConstantTerm( cl_eta );

  double smpup = 1. + mcSamplingTermRelError( cl_eta );
  double smpdo = 1. - mcSamplingTermRelError( cl_eta );

  double central = sqrt( 2*(resData*resData - resMC*resMC)/mz/mz + cmc*cmc );
  double vardown = sqrt( 2*(resData*resData - resMC*resMC*smpup*smpup)/mz/mz + cmc*cmc );
  double varup   = sqrt( 2*(resData*resData - resMC*resMC*smpdo*smpdo)/mz/mz + cmc*cmc );

  double errdown = fabs( central - vardown );
  double errup   = fabs( central - varup );

  return max( errup, errdown );

}


// total resolution uncertainty (fractional)

void EnergyRescaler::resolutionError( double energy, double cl_eta, double& errUp, double& errDown ) {

  double Cdata     = dataConstantTerm( cl_eta );
  double Cdata_cor = dataConstantTermCorError( cl_eta );
  double Cdata_err = dataConstantTermError( cl_eta );
  
  double Cdata_unc = 0.;
  if( Cdata_err > Cdata_cor )
    Cdata_unc = sqrt( Cdata_err*Cdata_err - Cdata_cor*Cdata_cor );
  if( Cdata_unc<0.001 )
    Cdata_unc = 0.001; // preserve at least the stat error

  double Smc       = mcSamplingTerm( cl_eta );
  double Smc_err   = mcSamplingTermRelError( cl_eta );

  double central = fcn_sigma( energy,  Cdata,  0.,  Smc,  0.);

  double err1 = fcn_sigma( energy, Cdata,  Cdata_unc, Smc,  0.    ) - central; 
  double err2 = fcn_sigma( energy, Cdata, -Cdata_unc, Smc,  0.    ) - central; 
  double err3 = fcn_sigma( energy, Cdata, -Cdata_cor, Smc, Smc_err) - central;
  double err4 = -err3;

  errUp = 0;
  if( err1>0 ) errUp = sqrt( errUp*errUp + err1*err1);
  if( err2>0 ) errUp = sqrt( errUp*errUp + err2*err2);
  if( err3>0 ) errUp = sqrt( errUp*errUp + err3*err3);
  if( err4>0 ) errUp = sqrt( errUp*errUp + err4*err4);

  errDown   = -errUp;
}


// total resolution (fractional)

double EnergyRescaler::resolution( double energy, double cl_eta, bool withCT ) {

  double a = mcSamplingTerm( cl_eta );
  double b = mcNoiseTerm( cl_eta );
  double c = mcConstantTerm( cl_eta );
  double d = dataConstantTerm( cl_eta );

  double sig2 = a*a/energy + b*b/energy/energy + c*c;
  if( withCT )
    sig2 += d*d;

  return sqrt(sig2);

}


// internal use only

double EnergyRescaler::fcn_sigma(double energy, double Cdata, double Cdata_er, double S, double S_er) {

  double sigma2 = std::pow((Cdata+Cdata_er)*energy,2) + std::pow(S*(1+S_er)*std::sqrt(energy),2);
  
  double sigma=0; 
  if (sigma2>0) 
    sigma=sqrt(sigma2);
  
  return sigma/energy;

}


// derive smearing correction

double EnergyRescaler::getSmearingCorrectionGeV(double eta, double energy, int value, bool mc_withCT, std::string /*corr_version*/) 
{
  double resMC, resData, /* resVar, */ errUp, errDown;
  resMC   = resolution( energy, eta, false );
  resData = resolution( energy, eta, true );
  resolutionError( energy, eta, errUp, errDown );

  double Cmc = 0.007;

  //=====================================
  //choose which value to use
  //=====================================

  if( value == 1 )
    resData += errDown;
  else if( value == 2 )
    resData += errUp;
  else if( value != 0 ) {
    std::cout << "getSmearingCorrection : wrong value, return 1" << endl;
    return 1;
  }
  
  //=====================================
  //Smearing procedure
  //=====================================

  double sigma2 = std::pow( resData*energy, 2 ) - std::pow( resMC*energy, 2 );
  if (mc_withCT==true) 
    sigma2 = sigma2 - std::pow( Cmc*energy, 2 );
  
  if (sigma2<=0) return 1;
  
  double sigma = sqrt(sigma2);
  double DeltaE0 = m_random3.Gaus(0,sigma);

  double cor0=(energy+DeltaE0)/energy;
  
  return cor0;
  
}

// smearing: using std::vectors from D3PD
std::vector<float> EnergyRescaler::getSmearingCorrectionMeV(
   std::vector<float> eta_s2, std::vector<float> cl_E, 
   int value, bool mc_withCT,std::string corr_version )
{
   std::vector<float> result;
   for (unsigned int i = 0; i < eta_s2.size(); ++i)
      result.push_back( getSmearingCorrectionMeV(eta_s2[i], cl_E[i], value, mc_withCT, corr_version) );
   return result;
}


// a calibration correction for crack electrons, to be applied to both data and MC

double EnergyRescaler::applyMCCalibrationGeV(double eta, double ET, std::string ptype) {
  
  if( ptype != "ELECTRON" || m_corr_version != "2011" )
    return 1.;
	
  double aeta = fabs(eta);
	
  if( aeta<1.42 || aeta>1.55 )
    return 1.;
	
  const int nBoundaries = 18;
  double ETBoundaries[nBoundaries]   = { 0., 5.4, 8.5, 12.9, 16., 20., 
					 25., 30., 35., 40., 45., 50., 
					 55., 60., 65., 70., 75., 99999. };
	 
  double CalibFactors[nBoundaries-1] = { 0.884845, 0.898526, 0.902439, 0.91899, 0.925868, 0.929440, 
					 0.948080, 0.943788, 0.96026, 0.955709, 0.964285, 0.95762, 
					 0.970385, 0.963489, 0.968149, 0.970799, 0.961617 };
	
  int i0 = -1;
	
  for ( int i=0; i<nBoundaries-1; i++)
    if ( ET>ETBoundaries[i] && ET<=ETBoundaries[i+1] )
      i0 = i;
	
  if( i0>=0 && i0<nBoundaries-1 )
    return 1./CalibFactors[i0];
	
  return 1.;
	
}
	


// AF -> G4 correction

double EnergyRescaler::applyAFtoG4(double eta) {

  double aeta = fabs(eta);
  if( aeta>2.47 )
    return 1.;
  
  const int nBoundaries2011 = 27;
  double EtaBoundaries2011[nBoundaries2011] = { -2.47, -2.3, -2.1, -1.9, -1.7, -1.52, -1.37, -1.2, 
						-1.0, -0.8, -0.6, -0.4, -0.2,  0.0,   0.2,   0.4, 
						0.6,  0.8,  1.0,  1.2,  1.37, 1.52, 1.7, 1.9, 2.1, 2.3, 2.47 };  
  double CalibFactors2011[nBoundaries2011-1] = { 1.01228, 1.00454, 1.00427, 1.00744, 1.00244, 1.00424, 
						 0.993664, 1.00191, 1.00366, 0.999175, 0.997696, 0.997001, 
						 0.999441, 0.999655, 0.996957, 0.997899, 0.999005, 1.00341, 
						 1.00156, 0.993236, 0.996897, 1.00218, 1.00679, 1.00396,
						 1.00353, 1.01224 };
  
  const int nBoundaries2012 = 35;
  double EtaBoundaries2012[nBoundaries2012] = { -2.47, -2.4, -2.3, -2.1, -2.0, -1.82, -1.74, -1.63, 
						-1.55, -1.47, -1.37, -1.2, -1.0, -0.8, -0.6, -0.4, -0.2,
						0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.37, 1.47, 1.55, 1.63,
						1.74, 1.82, 2.0, 2.1, 2.3, 2.4, 2.47 };  
  double CalibFactors2012[nBoundaries2012-1] = { 1.01944, 1.01074, 1.00574, 1.00702, 1.00506, 1.01151,
						 0.999852, 1.00278, 0.976932, 0.980372, 0.993411, 1.00212,
						 1.00375, 0.998893, 0.998349, 0.9973, 0.999556, 0.99938,
						 0.997396, 0.998218, 0.999082, 1.0031, 1.00175, 0.992942,
						 0.980641, 0.969807, 1.00268, 0.99945, 1.01133, 1.00486,
						 1.00619, 1.00516, 1.011, 1.01842 };
  
  int nBoundaries=0;
  double* EtaBoundaries=0;
  double* CalibFactors=0;
  if( m_corr_version != "2012" ) {
    nBoundaries = nBoundaries2011;
    EtaBoundaries = EtaBoundaries2011;
    CalibFactors = CalibFactors2011;
  } else if( m_corr_version == "2012" ) {
    nBoundaries = nBoundaries2012;
    EtaBoundaries = EtaBoundaries2012;
    CalibFactors = CalibFactors2012;
  }

  int i0 = -1;
  
  for ( int i=0; i<nBoundaries-1; i++)
    if ( eta>EtaBoundaries[i] && eta<=EtaBoundaries[i+1] )
      i0 = i;
  
  if( i0>=0 && i0<nBoundaries-1 )
    return CalibFactors[i0];
  
  return 1.;
  
}  


}
