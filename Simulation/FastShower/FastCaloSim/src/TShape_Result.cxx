/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/TShape_Result.h"
#include "FastCaloSim/TLateralShapeCorrection.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TVector2.h"
#include "TSpline.h"
#include "TH1F.h"
#include "TBuffer.h"
#include <cmath>

#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include <exception>
#include <stdexcept> // out_of_range exception

using namespace std;

class TSplineAccess:public TSpline {
public:
  Int_t   GetN() const {return fNp;};
  void    ResetHistGraph() {if(fHistogram) delete fHistogram;fHistogram=0; if(fGraph) delete fGraph;fGraph=0;};
  TH1F*   Histogram() const {return fHistogram;};
  TGraph* Graph() const {return fGraph;};
};  

TShape_Result::TShape_Result(const char* name, const char* title):TNamed(name,title),
								  m_fitsplines_EnergyDistribution_maxx(0),m_fitsplines_EtaPhiAspectRatio_minx(0),m_fitsplines_EtaPhiAspectRatio_maxx(0), 
								  m_deta(0),m_reldeta(0),m_reldeta_dist(0),m_dphi(0),m_fitsplines_EnergyDistribution(0),m_fitsplines_EtaPhiAspectRatio(0),m_pdgid(0),m_energy(0),m_eta(0),m_calosample(0), m_distmin(0), m_distmax(0),m_integral_eta_scale(0),m_integral_phi_scale(0),m_cutoff_eta(0),m_cutoff_phi(0),
								  m_cellbordersmearing(1),m_reletascale(0),
								  m_cell_width_eta(0),m_cell_width_eta1(0),m_cell_width_eta2(0),m_cell_width_eta3(0),
								  m_chi2(-1),m_errdef(0),m_flags(0),m_nevents(100),m_cputime(0),m_correction(0)
{
}

//______________________________________________________________________________
void TShape_Result::Streamer(TBuffer &R__b)
{
   // Stream an object of class TShape_Result.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> m_deta;
      if(R__v>=7){               // added this lines (25.5.2009)
        R__b >> m_reldeta;
      }
      else{
        m_reldeta=0;
      }
      if(R__v>=9){
        R__b >> m_reldeta_dist;
        if(R__v<10){
            m_reldeta_dist =0;

        }
      }else{                                                     // added this line (12.5.2009)
        m_reldeta_dist=0;
      }
      R__b >> m_dphi;
      R__b >> m_fitsplines_EnergyDistribution;
      R__b >> m_fitsplines_EtaPhiAspectRatio;
      R__b >> m_pdgid;
      R__b >> m_energy;
      R__b >> m_eta;
      R__b >> m_calosample;
      R__b >> m_distmin;
      R__b >> m_distmax;
      m_params.Streamer(R__b);
      R__b >> m_integral_eta_scale;
      R__b >> m_integral_phi_scale;
      R__b >> m_cutoff_eta;
      R__b >> m_cutoff_phi;
      R__b >> m_cellbordersmearing;
      if(R__v<5 && m_cellbordersmearing==0) m_cellbordersmearing=1.0;
      if(R__v>=3) R__b >> m_reletascale;
      if(R__v>=11){
       R__b >> m_cell_width_eta;
       R__b >> m_cell_width_eta1;
       R__b >> m_cell_width_eta2;
       R__b >> m_cell_width_eta3;
      }else{
       m_cell_width_eta =0;
       m_cell_width_eta1=0;
       m_cell_width_eta2=0;
       m_cell_width_eta3=0;
      }
      if(R__v>= 5) {
        R__b >> m_chi2;
        R__b >> m_errdef;
        R__b >> m_flags;
      }  
      if(R__v>= 6) {
        R__b >> m_nevents;
        R__b >> m_cputime;
      } else {
        if(m_flags>0) m_nevents=200;
         else m_nevents=100;
      }
      R__b.CheckByteCount(R__s, R__c, TShape_Result::IsA());

      if(m_fitsplines_EnergyDistribution) {
        Int_t np=m_fitsplines_EnergyDistribution->GetNp();
        double fY;
        m_fitsplines_EnergyDistribution->GetKnot(np-1,m_fitsplines_EnergyDistribution_maxx,fY);
      } else {
        m_fitsplines_EnergyDistribution_maxx=0;
      }  

      if(m_fitsplines_EtaPhiAspectRatio) {
        Int_t np=m_fitsplines_EtaPhiAspectRatio->GetNp();
        double fY;
        m_fitsplines_EtaPhiAspectRatio->GetKnot(0   ,m_fitsplines_EtaPhiAspectRatio_minx,fY);
        m_fitsplines_EtaPhiAspectRatio->GetKnot(np-1,m_fitsplines_EtaPhiAspectRatio_maxx,fY);
      } else {
        m_fitsplines_EtaPhiAspectRatio_minx=0;
        m_fitsplines_EtaPhiAspectRatio_maxx=0;
      }  
   } else {
      R__c = R__b.WriteVersion(TShape_Result::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      R__b << m_deta;
      R__b << m_reldeta;
      R__b << m_reldeta_dist;
      R__b << m_dphi;
      R__b << m_fitsplines_EnergyDistribution;
      R__b << m_fitsplines_EtaPhiAspectRatio;
      R__b << m_pdgid;
      R__b << m_energy;
      R__b << m_eta;
      R__b << m_calosample;
      R__b << m_distmin;
      R__b << m_distmax;
      m_params.Streamer(R__b);
      R__b << m_integral_eta_scale;
      R__b << m_integral_phi_scale;
      R__b << m_cutoff_eta;
      R__b << m_cutoff_phi;
      R__b << m_cellbordersmearing;
      R__b << m_reletascale;
      R__b << m_cell_width_eta;
      R__b << m_cell_width_eta1;
      R__b << m_cell_width_eta2;
      R__b << m_cell_width_eta3;
      R__b << m_chi2;
      R__b << m_errdef;
      R__b << m_flags;
      R__b << m_nevents;
      R__b << m_cputime;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

Double_t TShape_Result::f_2DSpline_getd(double dxfcx_mm,double dyfcy_mm,int maxiter,double max_aspect)
{
  double dxfcx_mm2=dxfcx_mm*dxfcx_mm;
  double dyfcy_mm2=dyfcy_mm*dyfcy_mm;
  double d2=dxfcx_mm2 + dyfcy_mm2;
  double d=TMath::Sqrt(d2);
  
  if(!m_fitsplines_EtaPhiAspectRatio) return d;
  
  Int_t np=((TSplineAccess*)m_fitsplines_EtaPhiAspectRatio)->GetN();
  double fX,fY,fB,fC,fD;
  ((TSpline3*)m_fitsplines_EtaPhiAspectRatio)->GetCoeff(np-1,fX,fY,fB,fC,fD);

  int niter=0;
  double oldd=d;
  do {
    double da;
    double a;
    bool dobreak=false;
    if(d>fX) {
      a=fY;
      da=0;
/*
      double dx=d-fX;
      a=fY+dx*fB;
      da=fB;
*/      
    } else {
      a=m_fitsplines_EtaPhiAspectRatio->Eval(d);
      da=((TSpline3*)m_fitsplines_EtaPhiAspectRatio)->Derivative(d);
    }
    if(a> max_aspect) {
      a=max_aspect;
      da=0;
      dobreak=true;
    }  
    if(a<-max_aspect) {
      a=-max_aspect;
      da=0;
      dobreak=true;
    }  
    
    double aspect2=TMath::Exp(2*a);
    
    oldd=d;
    double newd=TMath::Sqrt(dxfcx_mm2*aspect2 + dyfcy_mm2/aspect2);
    d=newd;
    if(dobreak) break;
    
    double delta =d-oldd;
    double absdelta=TMath::Abs(delta);
    if(absdelta     <0.001 ) break;
    if(absdelta/oldd<0.0001) break;

//    cout<<"iter="<<niter<<" d="<<d<<" dinter="<<dinter;
    double ddelta=-1 + (a*da)/(d) * (dxfcx_mm2*aspect2 - dyfcy_mm2/aspect2);
    
    d=oldd-delta/ddelta;
    if(d/newd<0.9) d=0.9*newd;
    if(d/newd>1.1) d=1.1*newd;
    if(std::isnan(d)) {
      cout<<"=================================="<<endl;
      cout<<"=================================="<<endl;
      cout<<"=================================="<<endl;
      cout<<"d is nan"<<endl;
      cout<<"dxfcx_mm="<<dxfcx_mm<<endl;
      cout<<"dyfcy_mm="<<dyfcy_mm<<endl;
      cout<<"oldd="<<oldd<<endl;
      cout<<"newd="<<newd<<endl;
      cout<<"delta="<<delta<<endl;
      cout<<"ddelta="<<ddelta<<endl;
      cout<<"a="<<a<<endl;
      cout<<"da="<<da<<endl;
      cout<<"aspect="<<TMath::Sqrt(aspect2)<<endl;
      cout<<"iter="<<niter<<endl;
      cout<<"=================================="<<endl;
      cout<<"=================================="<<endl;
      cout<<"=================================="<<endl;
      d=newd;
      niter=99;
      break;
    }
    ++niter;
  } while (niter<maxiter);

  return d;
}

Double_t TShape_Result::SplineEval(TSpline3* sp,Double_t x,Double_t xmin,Double_t xmax)
{
  double res;
  if(x>xmax) {
    Int_t np=sp->GetNp();
    double fX,fY,fB,fC,fD;
    sp->GetCoeff(np-1,fX,fY,fB,fC,fD);

    double dx=x-fX;
    res=fY+dx*fB;
  } else {
    if(x<xmin) {
      double fX,fY,fB,fC,fD;
      sp->GetCoeff(0,fX,fY,fB,fC,fD);
      double dx=x-fX;
      res=fY+dx*fB;
    } else {
      res=sp->Eval(x);
    }
  }
  return res;
}

Double_t TShape_Result::f_2DSpline(double dxfcx_mm,double dyfcy_mm)
{
  double dxfcx_mm2=dxfcx_mm*dxfcx_mm;
  if(m_fitsplines_EtaPhiAspectRatio) {
    double etascale=SplineEval( (TSpline3*)(m_fitsplines_EtaPhiAspectRatio) , dxfcx_mm , m_fitsplines_EtaPhiAspectRatio_minx , m_fitsplines_EtaPhiAspectRatio_maxx);
    dxfcx_mm2*=TMath::Exp(2*etascale);
  }
  
  double dyfcy_mm2=dyfcy_mm*dyfcy_mm;
  double d2=dxfcx_mm2 + dyfcy_mm2;
  double d=TMath::Sqrt(d2);
  
/*
  double max_aspect=2;
  double d=f_2DSpline_getd(dxfcx_mm,dyfcy_mm,40,max_aspect);
*/  
  double exp=SplineEval( (TSpline3*)m_fitsplines_EnergyDistribution , d , 0 , m_fitsplines_EnergyDistribution_maxx);
  if(exp<-20) exp=-20;
  if(exp>3) exp=3;

//  double rs=reletascale()*dxfcx_mm/cutoff_eta();

//  double val=TMath::Exp( exp + rs );
  double val=TMath::Exp( exp );
  
  return val;
}

void TShape_Result::AddShapeCorrection(TLateralShapeCorrectionBase* shape) 
{
  m_correction.push_back(shape);
  if(shape->HasShapeFactor()) SetBit(kHasShapeFactor);
  if(shape->HasCellFactor())  SetBit(kHasCellFactor);
}

Double_t TShape_Result::CellIntegralEtaPhi(const CaloDetDescrElement& cell,const double eta, const double phi,double& ein_wide,double fcx, double fcy,double direction_factor)
{
  double cx=cell.eta();
  double cy=cell.phi();

//  int calosample=calosample();

  double dy_border_mod=cellbordersmearing();
  double dy_border=0;
  if(calosample()>=CaloCell_ID_FCS::EMB1 && calosample()<=CaloCell_ID_FCS::EMB3) dy_border=0.0245437;
  if(calosample()>=CaloCell_ID_FCS::EME1 && calosample()<=CaloCell_ID_FCS::EME3) {
    if(TMath::Abs(cx)<2.5) dy_border=0.0245437;
     else dy_border=0.0981748;
  }  
  dy_border*=dy_border_mod;

  double dy_slopedist=(cell.dphi()-dy_border)/2;
  if(dy_slopedist<0) dy_slopedist=0;
  double slope_weight;
//  double dxh = cell.deta()/2;

  double cell_width_factor=1+cell_width_eta()+fabs(direction_factor)*cell_width_eta1()+fabs(direction_factor)*fabs(direction_factor)*cell_width_eta2();
  if(cell_width_factor<0.02) cell_width_factor=0.02;
  if(cell_width_factor>10) cell_width_factor=10;

  double dxh=(cell.deta()*cell_width_factor)/2;
  
  
//   if(dxh*1000000 < 0. ){
//     cout << "dxh  = " << dxh << endl;
//     cout << "dxh*10^6  = " << dxh*1000000<< endl;
//   }
//   if(dxh !=0){
//     dxh *= (1+reldeta_width()*direction_factor);
//       if(dxh < 0.0){
//         cout << "dxh_mod  = " << dxh << endl;
//         cout << "dxh_mod*10^6  = " << dxh*1000000 << endl;
//         cout << "direction_factor*100 = " << direction_factor*100 << endl;
//         cout << "reldeta_width = " << reldeta_width() << endl;
//       }
//   }
  double dyh=(cell.dphi()+dy_border)/2;
  
//   double fcx=eta_center(eta,distfactor,direction_factor*sign);
//   double fcy=phi_center(phi);
  // Jakobi factor : dtheta/deta=-2*exp(-eta)/(1+exp(-2eta))
  double eta_jakobi=eta_jakobi_factor(fcx);
//  double phi_dist2r=1.0/TMath::CosH(fcx);
  double phi_dist2r=1.0;
  double dist000=TMath::Sqrt(cell.r()*cell.r()+cell.z()*cell.z());
  double celldx=cell.deta()*eta_jakobi*dist000;
  double celldy=cell.dphi()*phi_dist2r*cell.r();
  
  double deta=TMath::Abs( cx-fcx );
  double dphi=TMath::Abs( TVector2::Phi_mpi_pi(cy-fcy) );
  double deta_mm=deta*eta_jakobi*dist000;
  double dphi_mm=dphi*phi_dist2r*cell.r();

  if( Sqr(deta_mm/(m_cutoff_eta+celldx/2)) + Sqr(dphi_mm/(m_cutoff_phi+celldy/2))>1 ) {
    ein_wide=0;
    return 0;
  }
  
  const int nx=TMath::Max(TMath::Nint(celldx/m_integral_eta_scale),3);
  //if(calosample() ==1 ) nx=TMath::Max(TMath::Nint(celldx/m_integral_eta_scale),10);
  const int ny=TMath::Max(TMath::Nint(celldy/m_integral_phi_scale),2);
  double norm=nx*ny;
  norm/=4*dxh*dyh;
  double sxh=dxh/nx;
  double syh=dyh/ny;
  double sx=sxh*2;
  double sy=syh*2;

  /*
  if(cell_width_factor>3 && TMath::Abs(cx-fcx)<cell.deta()/2 && TMath::Abs(cy-fcy)<cell.dphi()/2) {
    cout << "cx = " << cx << " cy = " << cy << endl;
    cout << "cdeta/2 = " << cell.deta()/2 << "cdphi/2 = " << cell.dphi()/2<< endl;
    cout << "dxh_mod  = " << dxh << endl;
    cout << "direction_factor*100 = " << direction_factor*100 << endl;
    cout << "cell_width_eta = " << cell_width_eta() << endl;
    cout << "cell_width_eta1 = " << cell_width_eta1() << endl;
    cout << "cell_width_eta2 = " << cell_width_eta2() << endl;
  }
  */

  if(std::isnan(fcx) || std::isnan(fcy)) {
    cout<<"=================================="<<endl;
    cout<<"=================================="<<endl;
    cout<<"isnan in CellIntegralEtaPhi"<<endl;
    cout<<"fcx="<<fcx<<endl;
    cout<<"fcy="<<fcy<<endl;
    cout<<"truth.Eta()="<<eta<<endl;
    cout<<"truth.Phi()="<<phi<<endl;
    cout<<"=================================="<<endl;
    cout<<"=================================="<<endl;
  }
  double integ=0;
  ein_wide=0;
  double x=cx-dxh+sxh;
  for(int ix=0;ix<nx;++ix) {
    double dxfcx=x-fcx;

    double dxfcx_mm=dxfcx*eta_jakobi*dist000;
    if(x<0) dxfcx_mm*=-1.0;
    double y=cy-dyh+syh;
    for(int iy=0;iy<ny;++iy) {
      if(std::isnan(x) || std::isnan(y)) {
        cout<<"=================================="<<endl;
        cout<<"=================================="<<endl;
        cout<<"isnan in CellIntegralEtaPhi"<<endl;
        cout<<"fcx="<<fcx<<endl;
        cout<<"fcy="<<fcy<<endl;
        cout<<"x="<<x<<endl;
        cout<<"y="<<y<<endl;
        cout<<"cx="<<cx<<endl;
        cout<<"cy="<<cy<<endl;
        cout<<"dxh="<<dxh<<endl;
        cout<<"dyh="<<dyh<<endl;
        cout<<"=================================="<<endl;
        cout<<"=================================="<<endl;
      }

      double dyfcy=TVector2::Phi_mpi_pi((y-fcy));
      double dyfcy_mm=dyfcy*phi_dist2r*cell.r();
      
      double val=1;
      
      val*=f_2DSpline(dxfcx_mm,dyfcy_mm);
      
      if(dy_border>0) {
        slope_weight=1-(TMath::Abs(y-cy)-dy_slopedist)/dy_border;
        if(slope_weight<0) slope_weight=0;
        if(slope_weight>1) slope_weight=1;
        val*=slope_weight;
      } else slope_weight=1;
      
      if(HasShapeFactor()) {
        float deta_cellunits=dxfcx_mm/celldx;
        float dphi_cellunits=dyfcy_mm/celldy;
        for(unsigned int icorr=0;icorr<m_correction.size();++icorr) if(m_correction[icorr]->HasShapeFactor()) {
          val*=m_correction[icorr]->shapefactor(dxfcx_mm,dyfcy_mm,deta_cellunits,dphi_cellunits);
        }
      }

      integ+=val;
      
//      if(CellIntegralEtaPhi_doprint) cout<<"    sp "<<ix<<","<<iy<<" dxfcx="<<dxfcx<<" dyfcy="<<dyfcy<<" val="<<val;
      
      if( Sqr(dxfcx_mm/m_cutoff_eta) + Sqr(dyfcy_mm/m_cutoff_phi) < 1) {
        ein_wide  +=val;
      }  
      y+=sy;
    }
    x+=sx;
  }

  integ/=norm; 
  ein_wide/=norm;
//  if(integ < 0.00001) cout <<" integ very small:  integ*10^8  = "<< integ*100000000 << endl;
  return integ;
}

   
void TShape_Result::SetDebugInfo( const int& sample, FastShowerInfo* fsi )
{
	if(!fsi)
	{
		cout << "TShapeResult \tERROR \tSetDebugInfo called with NULL pointer for FastShowerInfo object" << endl;
		return;
	}
	
	try
	{
		fsi->m_shaperes_calosample.at(sample)= calosample();
		fsi->m_shaperes_id.at(sample) = id();
		fsi->m_shaperes_e.at(sample) = (float) energy();
		fsi->m_shaperes_eta.at(sample) = (float) eta();
	}
	catch(std::out_of_range& e) 
	{
    	std::cerr << e.what( ) << " in TShape_Result::SetDebugInfo. Please check your index (" << sample << ") and if FastShowerInfo::Initialize has correctly been called and all vecors have been initialized. \n";
  	}
}









ClassImp(TShape_Result)
