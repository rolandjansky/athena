/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class allows easy Resoltuion Calculations
///////////////////////////////////////////////////////////////////////////

#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MuonPTResolution.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"

MSResolution::MSResolution()
{
	clear();
}


// Copy constructor
MSResolution::MSResolution(const MSResolution& ob)
{
	m_1D_StackInfo	= ob.m_1D_StackInfo;
	vecBins		=	 ob.vecBins;
	vecTrue		=	 ob.vecTrue;
	vecReco		=	 ob.vecReco;
}


// Overload =
void MSResolution::operator = (const MSResolution& ob)
{
	m_1D_StackInfo	= ob.m_1D_StackInfo;
	vecBins		=	 ob.vecBins;
	vecTrue		=	 ob.vecTrue;
	vecReco		=	 ob.vecReco;
}


MSResolution::~MSResolution()
{
}

void MSResolution::clear()
{
	m_1D_StackInfo.clear();
	vector<double> vDummy;
	vecBins.clear();
	vecTrue.clear();
	vecReco.clear();

	m_1D_StackInfo["Overall"][0]=vDummy;
	vecBins["Overall"].push_back(-3.0);
	vecBins["Overall"].push_back(+3.0);
}


void MSResolution::setupDim(string id, double start, double end, int bins)
{
	vector<double> vDummy;

	vecTrue[id].clear();
	vecReco[id].clear();

	vecBins[id].clear();
	vecBins[id].push_back(start);
	double step=(end-start)/((double)(bins));
	double s=start;
	for (int i=0; i<bins; i++)
	{
		s+=step;
		vecBins[id].push_back(s);
		m_1D_StackInfo[id][i]=vDummy;
	}
}

void MSResolution::add(string id, double value, double insert_value)
{
	if (vecBins.find(id) == vecBins.end())	return;
	vecTrue[id].push_back(insert_value);
	vecReco[id].push_back(insert_value);
	
	int x=0;
	int i=0;
	int nBins=getNumberOfBins(id);

	if ((value<vecBins[id][0]) || (vecBins[id][vecBins[id].size()-1]<value)) return;

	for (i=0; i<nBins; i++)	if ((vecBins[id][i]<=value) && (value<vecBins[id][i+1])) x=i;

	m_1D_StackInfo[id][x].push_back(insert_value);
	m_1D_StackInfo["Overall"][0].push_back(insert_value);
}

void MSResolution::add(string id, double value, double ptTrue, double ptReco)
{
	if (vecBins.find(id) == vecBins.end())	return;

	double ptRes	=	((1.0/ptTrue)-(1.0/ptReco))/(1.0/ptTrue);

	vecTrue[id].push_back(ptTrue);
	vecReco[id].push_back(ptReco);

	int x=0;
	int i=0;
	int nBins=getNumberOfBins(id);

	if ((value<vecBins[id][0]) || (vecBins[id][vecBins[id].size()-1]<value)) return;

	for (i=0; i<nBins; i++)	if ((vecBins[id][i]<=value) && (value<vecBins[id][i+1])) x=i;

	m_1D_StackInfo[id][x].push_back(ptRes);
	m_1D_StackInfo["Overall"][0].push_back(ptRes);
}

int   MSResolution::getBinInfo(string id, double value)
{
	if (vecBins.find(id) == vecBins.end())	return -1;
	int nBins=getNumberOfBins(id);
	for (int i=0; i<=nBins; i++)
	{
		if (value<vecBins[id][i]) return i;
	}
	return -1;
}

bool MSResolution::getDimBinInfo(string id, int i, double &meanOfBin, double &widthOfBin)
{
	meanOfBin=0.0;
	widthOfBin=0.0;
	if (vecBins.find(id) == vecBins.end())	return false;
	int nBins=getNumberOfBins(id);
	if ((i<0) || (nBins<=i)) return false;
	meanOfBin  = vecBins[id][i]+(vecBins[id][i+1]-vecBins[id][i])/2.0;
	widthOfBin = (vecBins[id][i+1]-vecBins[id][i])/2.0;
	return true;
}


int  MSResolution::getNumberOfBins(string id)
{
	if (vecBins.find(id) == vecBins.end())	return -1;
	return (int)vecBins[id].size()-1;
}



bool MSResolution::getResolution(string id, double value, double &PTResMean, double &ErrMean, double &PT68Width, double &PTSigma, double &ErrPTSigma)
{
	if (vecBins.find(id) == vecBins.end())	return false;

	int nBins=getNumberOfBins(id);
	int x=0;

	PTResMean	= 0.0;
	ErrMean		= 0.0;
	PT68Width	= 0.0;
	PTSigma		= 0.0;
	ErrPTSigma	= 0.0;

	if ((value<vecBins[id][0]) || (vecBins[id][vecBins[id].size()-1]<value)) return false;
	for (int i=0; i<nBins; i++)	if ((vecBins[id][i]<=value) && (value<vecBins[id][i+1])) x=i;

	//double mean;
	//double distance;
	//distance=0.25;
	//mean=0.0;

	TF1	gaussian("g1","gaus", -0.3, 0.3);		// -0.3 , 0,3
	TH1F	mHistoPTRes("PT_Res_Distribution","PT_Res_Distribution",40, -0.3, 0.3);

	for (int i=0; i<(int)m_1D_StackInfo[id][x].size(); i++)
		mHistoPTRes.Fill(m_1D_StackInfo[id][x][i]);

	double TailFraction;

	getMuonPTResolution(	&mHistoPTRes,
				3,
				PTResMean,
				ErrMean,
				PTSigma,
				ErrPTSigma,
				TailFraction,
				0.68,
				PT68Width);

	std::cout<<" TM-Resolution: "<<id<<" mean "<<PTResMean<<"  sigma "<<PTSigma<<"  tail "<<TailFraction<<"  68w "<<PT68Width<<std::endl;

/*	mHistoPTRes.Fit(&gaussian);


	Double_t par[8];
	gaussian.GetParameters(&par[0]);
	mean	= fabs(par[1]);
	distance= 3.0*fabs(par[2]);

	// Getting Mean
	
	TF1	gaussian2("g2","[3]+[2]*(1/(sqrt(2.0*3.14152)*[1]))*exp(-0.5*(((x[0]-[0])*(x[0]-[0]))/([1]*[1])))", mean-distance, mean+distance);

	gaussian2.SetParameter(0,mean);
	gaussian2.SetParameter(1,distance/3.0);
	gaussian2.SetParameter(2,100.);
	gaussian2.SetParameter(3,0.1);

	mHistoPTRes.Fit(&gaussian2);

	gaussian2.GetParameters(&par[0]);

	PTResMean	= (gaussian2.GetParameter(0));
	ErrMean		= (gaussian2.GetParError(0));
	

	// Getting 95.4% Width

	PTSigma		= fabs((gaussian2.GetParameter(1)));
	get95PercentRange(id, value, PTResMean, PT68Width);
	ErrPTSigma	= fabs((gaussian2.GetParError(1)));
*/
	return true;
}

double fit_bw(double *x, double *par)
{
	Double_t bw[120]=
	{
	0.001277970,    0.001377110,    0.001425620,    0.001423670,   0.001380080,    0.001372230,
	0.001397640,    0.001408260,    0.001423740,    0.001444980,   0.001476560,    0.001528750,
	0.001592960,    0.001654420,    0.001696820,    0.001719870,   0.001742580,    0.001784960,
	0.001847450,    0.001911020,    0.001957340,    0.001986040,   0.002013240,    0.002058030,
	0.002129450,    0.002219610,    0.002306760,    0.002371600,   0.002417540,    0.002474520,
	0.002577540,    0.002737010,    0.002928630,    0.003111500,   0.003255970,    0.003357930,
	0.003437240,    0.003530780,    0.003679360,    0.003901910,   0.004173820,    0.004440310,
	0.004662170,    0.004850390,    0.005060920,    0.005368100,   0.005842670,    0.006529310,
	0.007418300,    0.008443590,    0.009541480,    0.010736700,   0.012165000,    0.013960200,       
	0.016506200,    0.019937300,    0.024904800,    0.032257500,   0.042564900,    0.058774600,
	0.079834700,    0.099173100,    0.103405000,    0.085914000,   0.062607400,    0.043508300,
	0.028367400,    0.019137400,    0.013947100,    0.010777600,   0.008795630,    0.007268770,
	0.006050300,    0.005074930,    0.004301300,    0.003704810,   0.003249280,    0.002875490,
	0.002534550,    0.002222110,    0.001965540,    0.001787910,   0.001667750,    0.001540670,
	0.001399510,    0.001288360,    0.001196170,    0.001096960,   0.000985893,    0.000873227,
	0.000785877,    0.000714149,    0.000643602,    0.000590844,   0.000555048,    0.000520910,
	0.000490724,    0.000455894,    0.000415860,    0.000387901,   0.000362100,    0.000343127,
	0.000333103,    0.000308865,    0.000275397,    0.000254986,   0.000243638,    0.000227633,
	0.000211036,    0.000199103,    0.000192421,    0.000188789,   0.000183909,    0.000176311,
	0.000167005,    0.000158099,    0.000151712,    0.000147755,   0.000144633,    0.000141605
	};

	Double_t xMean = x[0]-91187.0;
	Double_t fitval=0.0;

	for (int il=0;il<120;il++)
	{
		Double_t bwb=60000.25+500.0*il;
		Double_t gc=(bwb-par[0]);
		Double_t arg = 0;
		if (par[2] != 0) arg = (xMean - gc)/par[1];
		fitval += bw[il]*par[2]*TMath::Exp(-0.5*arg*arg);
	}

	return fitval;
}


bool MSResolution::getZBosonProperties(string id, double value, double &ZBosonMean, double &ErrMean,double &ZBosonSigma, double &ErrSigma)
{
	if (vecBins.find(id) == vecBins.end())	return false;

	int nBins=getNumberOfBins(id);
	int x=0;

	ZBosonMean	= 0.0;
	ErrMean		= 0.0;
	ZBosonSigma	= 0.0;
	ErrSigma	= 0.0;

	if ((value<vecBins[id][0]) || (vecBins[id][vecBins[id].size()-1]<value)) return false;
	for (int i=0; i<nBins; i++)	if ((vecBins[id][i]<=value) && (value<vecBins[id][i+1])) x=i;

	TH1F	mHistoPTRes("PT_Res_Distribution","PT_Res_Distribution",60, 60000., 120000.);

	for (int i=0; i<(int)m_1D_StackInfo[id][x].size(); i++)
		mHistoPTRes.Fill(m_1D_StackInfo[id][x][i]);

	/*TF1	fitFunction("g1","gaus", 60000., 120000.);
	mHistoPTRes.Fit(&fitFunction);

	ZBosonMean	= fabs((fitFunction.GetParameter(1)));
	ErrMean		= fabs((fitFunction.GetParError(1)));
	ZBosonSigma	= fabs((fitFunction.GetParameter(2)));
	ErrSigma	= fabs((fitFunction.GetParError(2)));*/
	
	TF1 *fitFunction = new TF1("fit_bwg",fit_bw,60,110, 3);

	fitFunction->SetParameter(0,90000.);
	fitFunction->SetParameter(1,2000.);
	fitFunction->SetParameter(2,2000.);

	mHistoPTRes.Fit(fitFunction);
	
	ZBosonMean	= fabs((fitFunction->GetParameter(0)));
	ErrMean		= fabs((fitFunction->GetParError(0)));
	ZBosonSigma	= fabs((fitFunction->GetParameter(1)));
	ErrSigma	= fabs((fitFunction->GetParError(1)));

	return true;
}

bool MSResolution::get95PercentRange(std::string id, double value, double mean, double &width)
{
	if ((value<vecBins[id][0]) || (vecBins[id][vecBins[id].size()-1]<value)) return false;
	int nBins=getNumberOfBins(id);
	int x;
	for (int i=0; i<nBins; i++)	if ((vecBins[id][i]<=value) && (value<vecBins[id][i+1])) x=i;

	double	total;
	double	sum;
	
	
	total=0.0;
	sum=0.0;
	for (int j=0; j<(int)m_1D_StackInfo[id][x].size(); j++)
	{
		sum+=m_1D_StackInfo[id][x][j];
	}

	double	percentage	= 0.0;
	double	step_width	= 0.1;
	double	distance	= 0.0;
	
	percentage=0.0;
	do
	{
		percentage=0.0;
		while (percentage<0.683)
		{
			total	= 0.0;
			sum	= 0.0;
			distance+=step_width;
			for (int j=0; j<(int)m_1D_StackInfo[id][x].size(); j++)
			{
				if 	(((mean-distance)<(m_1D_StackInfo[id][x][j])) && 
					((m_1D_StackInfo[id][x][j])<(mean+distance)))	sum+=1.0;
				total+=1.0;
			}
			percentage = sum/total;
		}
		distance-=step_width;
		step_width*=0.75;
	} while (step_width>(1.0/total));
	width=distance;
	return true;
}

