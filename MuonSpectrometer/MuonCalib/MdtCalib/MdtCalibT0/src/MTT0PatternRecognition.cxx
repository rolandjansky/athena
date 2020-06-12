/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/MTT0PatternRecognition.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TLine.h"
#include "TGraph.h"
#include "TH1.h"

namespace MuonCalib{

//////////////////////////////////////////////////////////////////
// estimate_background(const TH1 &hist, double scale_min)	//
//////////////////////////////////////////////////////////////////

bool MTT0PatternRecognition::estimate_background(TH1F* hist, double scale_min)
	{
//Get first bin in input histogram which is not empty. 	This avoids underestimation of the background rate if the range of the input histogram exceeds the TDC-range
	int min(-1);
	for(int i=1; i<=hist->GetNbinsX(); i++)
		{
		if(hist->GetBinContent(i)>0)
			{
			min=i;
			break;
			}
		}
	if(min==-1) {
		MsgStream log(Athena::getMessageSvc(), "MTT0PattternRecognition");
		log<<MSG::WARNING<< "estimate_background() - No hits in input histogram!"<<endmsg;
		m_error=true;
		return false;
	}
	double maxx=scale_min-40;
	int max=hist->FindBin(maxx);
//we want 10 bins minimum for the background estimate
	if(max-min<m_settings->MinBackgroundBins())
		{
		min=max-128;
		if (min<0)
			min=0;
		}
//if there are still not enough bins for the background estimate we have a problem
	if(max-min<m_settings->MinBackgroundBins()) {
		MsgStream log(Athena::getMessageSvc(), "MTT0PattternRecognition");
		log<<MSG::WARNING<< "estimate_background() - Rising edge is to glose to lower histogram range!"<<endmsg;
		m_error=true;
		return false;
	}
//calculate average bin content
	m_background=0.0;
	double back_squared=0.0;
	double n_bins=0.0;
	double referece_chi2=0.0;
	for(int i=min; i<max; i++)
		{
		n_bins++;
		m_background+=hist->GetBinContent(i);
		back_squared+=hist->GetBinContent(i) * hist->GetBinContent(i);
		if (n_bins==m_settings->MinBackgroundBins())
			{
			double bac=m_background/n_bins;
			referece_chi2 = 2 * (back_squared/n_bins - bac*bac);
			}
		if (n_bins>m_settings->MinBackgroundBins())
			{
			double bac=m_background/n_bins;
			double chi2=2 * (back_squared/n_bins - bac*bac);
			if (chi2>5*referece_chi2) break;
			}
		}
	m_background/=n_bins;
//store lower edge of fit range
	m_fit_min=hist->GetBinCenter(min);
	m_t0_est=0.5*(scale_min + hist->GetBinCenter(max));
//mark selected range
	if(m_draw_debug_graph)
		{
		(new TLine(hist->GetBinCenter(min), 0, hist->GetBinCenter(min), hist->GetMaximum()))->Write("t0_back_left");
		(new TLine(hist->GetBinCenter(max), 0, hist->GetBinCenter(max), hist->GetMaximum()))->Write("t0_back_right");
		}

	return true;
	}


//////////////////////////////////////////
// estimate_height(const TH1 &hist)	//
//////////////////////////////////////////

double MTT0PatternRecognition::estimate_height(TH1F* hist)
	{
//smooth histogram
	if(!m_vbh.Smooth(hist->GetBinWidth(1)))
		{
		m_error=true;
		return -1;
		}
	if(!m_vbh.Smooth(hist->GetBinWidth(1)/2))
		{
		m_error=true;
		return -1;
		}
//draw debug graphs
	if(m_draw_debug_graph)
		{
		m_vbh.DenistyGraph()->Write("t0_density");
		m_vbh.BinWidthGraph()->Write("t0_binwidth");
		m_vbh.BinContentGraph()->Write("t0_bin_content");
		m_vbh.DiffDensityGraph()->Write("t0_diff_density");
		m_vbh.DiffDensityGraph()->Write("t0_diffbinwidth");
		}
//get the range in which the smallest, and second smallest bins are
	double lower=m_vbh.GetSortedBin(0).Width();
	double left(9e9), right(-9e9);
//	bool smallest_found(false);
	for(unsigned int i=0; i<m_vbh.GetNumberOfBins(); i++)
		{
		if(m_vbh.GetSortedBin(i).Left()<left) left = m_vbh.GetSortedBin(i).Left();
		if(m_vbh.GetSortedBin(i).Right()>right) right = m_vbh.GetSortedBin(i).Right();
//NOTE: For greater numerical stability m_vbh.GetSortedBin(i).Width() is considered greater only if it is greater by hist->GetBinWidth(1) / 100.0. I have seen differences in Binwidth in the order of 1e-13. On the other hand, due to the binning of the input histogram, e real difference in the binwidth of the VBH n*hist->GetBinWidth(1).
		if((m_vbh.GetSortedBin(i).Width() - lower) > hist->GetBinWidth(1) / 100.0) 
			{
		//reqire minimum distance between bins, and minimum number if significant bins	
			if(right-left>50 && i>20) break;
			lower=m_vbh.GetSortedBin(i).Width();
			}
		}
//mark selected range
	if(m_draw_debug_graph)
		{
		(new TLine(left, 0, left, hist->GetMaximum()))->Write("t0_height_left");
		(new TLine(right, 0, right, hist->GetMaximum()))->Write("t0_height_right");
		}
//calcularte mean bin content of input histogram in range
	int bleft(hist->FindBin(left)), bright(hist->FindBin(right));
	double nbins=0.0;
	m_height=0.0;
	for(int i=bleft; i<=bright; i++)
		{
		m_height+=hist->GetBinContent(i);
		nbins++;
		}
	if(nbins<1) {
		MsgStream log(Athena::getMessageSvc(), "MTT0PattternRecognition");
		log<<MSG::WARNING<< "estimate_height() - top region is too small! left="<<left<<" right="<<right<<endmsg;
		m_error=true;
		return -1;
	}
	m_height/=nbins;
	m_fit_max=right;
	return left;
	}
	
}
