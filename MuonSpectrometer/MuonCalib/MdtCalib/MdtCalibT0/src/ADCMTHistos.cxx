/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/ADCMTHistos.h"

//root
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"
#include <cmath>

using namespace std;

namespace MuonCalib {

/** skew normal ADC fitting 
*/
    inline Double_t adcfitf_skewnormal(Double_t *x, Double_t *par){     
  //par[0] =  skew gauss norm 
  //par[1] = skew gauss mean (i.e. mu) 
  //par[2] = skew gauss sigma (i.e sigma) 
  //par[3] = skew factor  (i.e. alpha) 
  // Numeric constants 
  //Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2) 
  Double_t invsq2pi = 1.0/(TMath::Sqrt(2*3.14159265358979323846264338327950288419716939937510)); 
  Double_t twoPi = 6.2831853071795;//2Pi 

//  Double_t Landau_Value = par[0]*TMath::Landau(x[0],par[1],par[2],kFALSE); 

  Double_t delta_value = par[3]/(TMath::Sqrt(1.+par[3]*par[3]));
  Double_t omega_square = (par[2]*par[2])/(1. - 4.*delta_value*delta_value/(twoPi));
  Double_t omega_value = TMath::Sqrt(omega_square);
  Double_t xi_value = par[1] - delta_value*omega_value*2.*invsq2pi;
  Double_t Gauss_part= (invsq2pi/omega_value)*exp(-((x[0] - xi_value)*(x[0] - xi_value))/(2.0*omega_square));//phi(x) 

  Double_t Erf_part=0.5*( 1+TMath::Erf(par[3]*(x[0]-xi_value)/(omega_value)) );
  
  Double_t SkewNormal_Value = par[0]*2.*Gauss_part*Erf_part;

  Double_t MyGaussFuncValue = SkewNormal_Value;

  return(MyGaussFuncValue);
 
                                                                    }

		
void ADCMTHistos :: FillA(double a) 
	{m_adc->Fill(static_cast<Axis_t>(a));}	


//////////////////////////////////////////////////////////
// Initialize(int id, const T0MTSettings & settings)	//
//////////////////////////////////////////////////////////

void ADCMTHistos :: Initialize(int id, const T0MTSettings * settings, const char * hname)
	{
	m_settings=settings;
//	if(m_settings->VerboseLevel()>1)
//		{
		cout<<"ADCMTHistos :: Initialize: called"<<endl;
//		}
	char buf[100];
	std::cout<<gDirectory->GetName()<<std::endl;
	if(hname==NULL)
		snprintf(buf, 100, "adc_spec_%d", id);
	else
		snprintf(buf, 100, "adc_spec_%s", hname);
	m_adc=new TH1F(buf, "", settings->NBinsADC(), settings->ADCMin(), settings->ADCMax());
	m_id=id;
	m_adc_ok=false;
	}



bool ADCMTHistos :: FitAdc()
	{
/*	m_adc_fit = new TF1("adc_fun", "landau(0) + gaus(3)");
	m_adc->Fit("landau", "Q");
	TF1 *lan_fit=m_adc->GetFunction("landau");
	for(int i=0; i<3; i++)
		m_adc_fit->SetParameter(i, lan_fit->GetParameter(i));
	m_adc->Fit("adc_fun", "Q");
*/
        m_adc_fit = new TF1("adc_fun",adcfitf_skewnormal, 50, 320, 4); 
        Double_t average = m_adc->GetMean();
        Double_t max=m_adc->GetMaximum();
//      m_adc->SetAxisRange(50,350,"X");
	m_adc_fit->SetParameters(max,average,40,1);  // initialize value
        m_adc_fit->SetLineColor(kRed);  
        m_adc_fit->SetParNames("Max","Mean", "Sigma","Skew_factor"); 
        m_adc_fit->SetParLimits(0,0,1000000); 
        m_adc_fit->SetParLimits(1,100,200); 
        m_adc_fit->SetParLimits(2,0,200); 
	//gStyle->SetOptFit(1011);  

	m_adc->Fit("adc_fun","Q+","",50,320); 
	m_adc_ok = true;
	return true;
	}

} //namespace MuonCalib
