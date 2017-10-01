/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/MTTmaxPatternRecognition.h"

//root
#include "TF1.h"
#include "TLine.h"
#include "TGraph.h"
#include "TH1.h"
#include <cmath>

using namespace std;

namespace MuonCalib{



inline Double_t myexp(Double_t *x, Double_t *p)
	{
	return exp(p[0] + p[1] * (x[0] - p[3])) + p[2];
	}

bool MTTmaxPatternRecognition :: Initialize( TH1F *hist, double t0, const T0MTSettings * settings)
	{
	m_settings=settings->TMaxSettings();
	m_draw_debug_graph=settings->DrawDebugGraphs();
	m_error=false;
	m_t0=t0;
	m_error=!m_vbh.Initialize(hist, m_settings->VBHBinContent(), m_settings->MaxBinwidth(), t0 + m_settings->VBHLow());
	if(m_error || m_vbh.GetNumberOfBins()<2)
		{
		cerr<<"MTTmaxPatternRecognition :: Initialize: INitialization of VBH failed!"<<endl;
		}
	if(!m_error) estimate_background(hist);
	if(!m_error) estimate_height(hist);
	return !m_error;
	}


//////////////////////////////////////////
// estimate_background( TH1 *hist)	//
//////////////////////////////////////////

bool MTTmaxPatternRecognition :: estimate_background( TH1F *hist)
	{
//smooth VBH in several steps
	for(double i=1.0; i<=4.0; i*=2.0)
		{
		for(int j=0; j<2; j++)
			if(!m_vbh.Smooth(hist->GetBinWidth(1)/i))
				{
				m_error=true;
				return false;
				}
		}
	if(m_draw_debug_graph)
		{
		m_vbh.DenistyGraph()->Write("tmax_density");
		m_vbh.BinWidthGraph()->Write("tmax_binwidth");
		m_vbh.DiffDensityGraph()->Write("tmax_diffdensity");
		m_vbh.DiffBinwidthGraph()->Write("tmax_diffbinwidth");
		}
//find falling edge - search for steepenst slope in a range of 600 to 800 ns after the t0
	double peak_falling(0.0), peak_falling_slope(-9e9);
	for(unsigned int i=0; i<m_vbh.GetNumberOfBins()-1; i++)
		{
		const VariableBinwidthHistogramBin &bin1(m_vbh.GetBin(i)), &bin2(m_vbh.GetBin(i+1));
	//select region
		if(bin1.Center()<m_t0+m_settings->EndMin() || bin1.Center()>m_t0+m_settings->EndMax()) continue;
		if(bin2.Width() - bin1.Width() > peak_falling_slope && bin2.Right() < hist->GetBinLowEdge(hist->GetNbinsX()) - 10)
			{
			peak_falling=bin1.Right();
			peak_falling_slope=bin2.Width() - bin1.Width();
			}
		}
//	cout<<"Peak falling="<<peak_falling<<endl;
//check region size
	int firstbin=hist->FindBin(peak_falling+m_settings->DistBackground());
	int lastbin=hist->FindBin(m_vbh.GetBin(m_vbh.GetNumberOfBins() - 1).Right());
	if(lastbin-firstbin<m_settings->MinBackgroundBins()) lastbin=hist->GetNbinsX();
	if(lastbin-firstbin<m_settings->MinBackgroundBins()) 
		{
		cerr<<"MTTMaxPattternRecognition :: estimate_background: Falling edge is to glose to upper histogram range!"<<endl;
		m_error=true;
		return false;		
		}
//calcul;ate mean
	m_background=0.0;
	double n_bins=0.0;
	if(lastbin>hist->GetNbinsX())lastbin=hist->GetNbinsX();
	for(int i=firstbin; i<=lastbin; i++)
		{
		m_background += hist->GetBinContent(i);
		n_bins++;
		}
//mark selected range
	if(m_draw_debug_graph)
		{
		(new TLine(hist->GetBinCenter(firstbin), 0, hist->GetBinCenter(firstbin), hist->GetMaximum()))->Write("tmax_back_left");
		(new TLine(hist->GetBinCenter(lastbin), 0, hist->GetBinCenter(lastbin), hist->GetMaximum()))->Write("tmax_back_right");
		}

	m_background/=n_bins;
	m_tmax_est=peak_falling;
	m_fit_max=hist->GetBinCenter(lastbin);
	return true;
	}


//////////////////////////////////////////////////////////
// estimate_height( TH1 *hist,  double background_min)	//
//////////////////////////////////////////////////////////

bool  MTTmaxPatternRecognition :: estimate_height( TH1F *hist)
	{
//get start value for fit
	Double_t left = m_tmax_est - m_settings->DistAB() - m_settings->WidthAB(),
	         right = m_tmax_est - m_settings->DistAB();
//fit
	TF1 *fun=new TF1("myexp", myexp, left, right, 4);
	fun->FixParameter(2, m_background);
	fun->FixParameter(3, m_t0);
	hist->Fit("myexp", "Q0+", "", left, right);
//store parameters
//	TF1 *fun=hist->GetFunction("expo");
	m_a=fun->GetParameter(0);
	m_b=fun->GetParameter(1);
	m_fit_min=m_tmax_est - m_settings->DistAB() - m_settings->WidthAB();
//mark selected range
	if(m_draw_debug_graph)
		{
		(new TLine(left, 0, left, hist->GetMaximum()))->Write("tmax_height_left");
		(new TLine(right, 0, right, hist->GetMaximum()))->Write("tmax_height_right");
		}
	return true;
	}

}	
