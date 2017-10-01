/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/VariableBinwidthHistogram.h"

// ROOT
#include "TH1.h"
#include "TGraph.h"
#include <cmath>

using namespace std;

namespace MuonCalib{


//////////////////////////////////////////////////////////////////////////
// Initialize(const &TH1 hist, double binc_rate, double max_bin_width)	//
//////////////////////////////////////////////////////////////////////////

bool VariableBinwidthHistogram :: Initialize(TH1F * hist, double binc_rate, double max_bin_width, double min_x, double max_x)
	{
	if(binc_rate<1.0)
		{
		cerr<<"ERROR in VariableBinwidthHistogram :: Initialize!"<<endl;
		cerr<<"binc_rate must be greater than 1!"<<endl;
		m_error=true;
		return false;
		}
//get maximum, firest bin and last bin
	double max;
	int first_bin(hist->FindBin(min_x)), last_bin(hist->FindBin(max_x));
	bool max_valid(true);
	if(first_bin==0) first_bin=1;
	else max_valid=false;
	if(last_bin>hist->GetNbinsX())last_bin=hist->GetNbinsX();
	else max_valid=false;
	if(max_valid) max=hist->GetMaximum();
	else
		{
		max=-9e9;
		for(int i=first_bin; i<=last_bin; i++)
			{
			if(max<hist->GetBinContent(i)) max=hist->GetBinContent(i);
			}
		}
//get number of entries per bin
	m_binc=static_cast<unsigned int>(ceil(max*binc_rate));
	m_max_bin_width=max_bin_width;
//create first bin
	m_bins.clear();
	m_bins.push_back(new VariableBinwidthHistogramBin(hist->GetBinLowEdge(first_bin) + 0.5 * max_bin_width, max_bin_width, 0));
	VariableBinwidthHistogramBin *current_bin(m_bins[0]);
//loop on histogram bins
	for(int i=first_bin; i<=last_bin; i++)
		{
	//maximum binwidth reached
		if(hist->GetBinCenter(i) > current_bin->Right())
			{
			m_bins. push_back(new VariableBinwidthHistogramBin(current_bin->Right() + 0.5 * m_max_bin_width, max_bin_width, 0));
			current_bin=m_bins[m_bins.size()-1];
			}
	//will the bin be full
		if(current_bin->Entries() + static_cast<unsigned int>(hist->GetBinContent(i)) > m_binc)
			{
		//this will be filled in the next bin
			unsigned int overflow=current_bin->Entries() + static_cast<unsigned int>(hist->GetBinContent(i)) - m_binc;	
		//the current bin ends here.
			current_bin->MoveRight(hist->GetBinLowEdge(i));
			current_bin->SetContent(m_binc);
		//create new bin
			m_bins. push_back(new VariableBinwidthHistogramBin(current_bin->Right() + 0.5 * m_max_bin_width, m_max_bin_width, overflow));
			current_bin=m_bins[m_bins.size()-1];
			continue;
			}
	//add to content of current bin
		(*current_bin)+=static_cast<unsigned int>(hist->GetBinContent(i));	
		}
//sort bins by content
	for(unsigned int i=0; i<m_bins.size(); i++)
		{
		m_sort_bins.push_back(VBHBinPtrSrt(m_bins[i]));
		}
	m_sorted=false;
	m_error=false;
	return true;
	}


//////////////////////////
// Smooth(double width)	//
//////////////////////////

bool  VariableBinwidthHistogram :: Smooth(double width)
	{
//needs at last 3 bins to smooth
	if(m_bins.size()<3) 
		{
		cerr<<"VariableBinwidthHistogram :: Smooth: VBH has less than 3 bins!"<<endl;
		return false;
		}
	for(unsigned int i=0; i<m_bins.size()-3; i++)
		{
		Double_t sl1=m_bins[i+1]->Width()-m_bins[i]->Width();
		Double_t sl2=m_bins[i+2]->Width()-m_bins[i+1]->Width();
	//one slopes must be smaller or equal to bw
//		if(fabs(sl1)>width && fabs(sl2)>width) continue;
	//slopes must be oposit sign
		if(sign(sl1)==sign(sl2)) continue;
	//prevents numerical effects
		if(fabs(sl1)<width/2 || fabs(sl2)<width/2) continue;
	//move bin boarder
		m_bins[i]->MoveRight(m_bins[i]->Right()-width/2*sign(sl2));
		m_bins[i+1]->MoveLeft(m_bins[i]->Right()-width/2*sign(sl2));
		}
	m_sorted=false;
	return true;
	}

//////////////////////////
// DenistyGraph()	//
//////////////////////////

TGraph *VariableBinwidthHistogram :: DenistyGraph() const
	{
	Double_t *x = new Double_t[m_bins.size()],
	         *y = new Double_t[m_bins.size()];
	for(unsigned int i=0; i<m_bins.size(); i++)
		{
		x[i]=m_bins[i]->Center();
		y[i]=m_bins[i]->Density();
		}	 
	TGraph *gr=new TGraph(m_bins.size(), x, y); 
//	cout<<"delete"<<endl;
//	delete [] x;
//	delete [] y;
//	cout<<"return"<<endl;
	return gr;
	}


//////////////////////////////////
// BinWidthGraph() const	//
//////////////////////////////////

TGraph *VariableBinwidthHistogram :: BinWidthGraph() const
	{
	Double_t *x = new Double_t[m_bins.size()],
	         *y = new Double_t[m_bins.size()];
	for(unsigned int i=0; i<m_bins.size(); i++)
		{
		x[i]=m_bins[i]->Center();
		y[i]=m_bins[i]->Width();
		}	 
	TGraph *gr=new TGraph(m_bins.size(), x, y); 
//	delete [] x;
//	delete [] y;
	return gr;
	}


//////////////////////////////////
// BinContentGraph() const	//
//////////////////////////////////

TGraph *VariableBinwidthHistogram :: BinContentGraph() const
	{
	Double_t *x = new Double_t[m_bins.size()],
	         *y = new Double_t[m_bins.size()];
	for(unsigned int i=0; i<m_bins.size(); i++)
		{
		x[i]=m_bins[i]->Center();
		y[i]=m_bins[i]->Entries();
		}	 
	TGraph *gr=new TGraph(m_bins.size(), x, y); 
//	delete [] x;
//	delete [] y;
	return gr;
	}


TGraph *VariableBinwidthHistogram :: DiffDensityGraph() const
	{
	if(m_bins.size()<2)
		{
		cerr<<"VariableBinwidthHistogram :: DiffDensity(): Need at alst 2 bins for differential density!"<<endl;
		return new TGraph();
		}
	Double_t *x = new Double_t[m_bins.size()-1],
	         *y = new Double_t[m_bins.size()-1];
	for(unsigned int i=0; i<m_bins.size()-1; i++)
		{
		x[i]=m_bins[i]->Center();
		y[i]=m_bins[i+1]->Density() - m_bins[i]->Density();
		}	 
	TGraph *gr=new TGraph(m_bins.size()-1, x, y); 
	return gr;
	}



TGraph *VariableBinwidthHistogram :: DiffBinwidthGraph() const	
	{
	if(m_bins.size()<2)
		{
		cerr<<"VariableBinwidthHistogram :: DiffBinwidth(): Need at alst 2 bins for differential density!"<<endl;
		return new TGraph();
		}
	Double_t *x = new Double_t[m_bins.size()-1],
	         *y = new Double_t[m_bins.size()-1];
	for(unsigned int i=0; i<m_bins.size()-1; i++)
		{
		x[i]=m_bins[i]->Center();
		y[i]=m_bins[i+1]->Width() - m_bins[i]->Width();
		}	 
	TGraph *gr=new TGraph(m_bins.size()-1, x, y); 
	return gr;
	}

}
