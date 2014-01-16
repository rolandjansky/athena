/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class allows easy Efficiency Calculations for up to
//		three dimensions
///////////////////////////////////////////////////////////////////////////

#include "MuonValUtils/MSEfficiency.h"


MSEfficiency::MSEfficiency()
{
	clear();
}


// Copy constructor
MSEfficiency::MSEfficiency(const MSEfficiency& ob)
{
	nDim1Bins	= ob.nDim1Bins;
	nDim2Bins	= ob.nDim2Bins;
	nDim3Bins	= ob.nDim3Bins;
	vecDim1Bins	= ob.vecDim1Bins;
	vecDim2Bins	= ob.vecDim2Bins;
	vecDim3Bins	= ob.vecDim3Bins;	
	nTotalEvents	= ob.nTotalEvents;
	nPassedEvents	= ob.nPassedEvents;
}


// Overload =
void MSEfficiency::operator = (const MSEfficiency& ob)
{
	nDim1Bins	= ob.nDim1Bins;
	nDim2Bins	= ob.nDim2Bins;
	nDim3Bins	= ob.nDim3Bins;
	vecDim1Bins	= ob.vecDim1Bins;
	vecDim2Bins	= ob.vecDim2Bins;
	vecDim3Bins	= ob.vecDim3Bins;	
	nTotalEvents	= ob.nTotalEvents;
	nPassedEvents	= ob.nPassedEvents;
}


void MSEfficiency::clear()
{
	nDim1Bins=0;
	nDim2Bins=0;
	nDim3Bins=0;
	vecDim1Bins.clear();
	vecDim2Bins.clear();
	vecDim3Bins.clear();
	nTotalEvents.clear();
	nPassedEvents.clear();

}

bool MSEfficiency::setupDim(int dim, double start, double end, int bins)
{
	if (dim==1)
	{
		nDim2Bins=1;
		nDim3Bins=1;
		vecDim1Bins.clear();
		vecDim1Bins.push_back(start);
		double step=(end-start)/((double)(bins));
		double s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim1Bins.push_back(s);
		}
		nDim1Bins=(int)vecDim1Bins.size();
		return setupDim(dim, vecDim1Bins);
	}
	if (dim==2)
	{
		if (nDim1Bins<=1) return false;
		vecDim2Bins.clear();
		vecDim2Bins.push_back(start);
		double step=(end-start)/((double)(bins));
		double s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim2Bins.push_back(s);
		}
		nDim2Bins=(int)vecDim2Bins.size();
		return setupDim(dim, vecDim2Bins);
	}
	if (dim==3)
	{
		if (nDim2Bins<=1) return false;
//		nDim3Bins=bins;
		vecDim3Bins.clear();
		vecDim3Bins.push_back(start);
		double step=(end-start)/((double)(bins));
		double s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim3Bins.push_back(s);
		}
		nDim3Bins=(int)vecDim3Bins.size();
		return setupDim(dim, vecDim3Bins);
	}

	return false;
}



bool MSEfficiency::setupDim(int dim, vector <double> &bins)
{
	vector<int> 		dummyInt1D;
	vector< vector<int> >	dummyInt2D;
	int i;

	if (dim==1)
	{
		vecDim1Bins=bins;
		if ((vecDim1Bins.size()==0) ) return false;

		dummyInt1D.push_back(0);
		dummyInt2D.push_back(dummyInt1D);

		nTotalEvents.clear();
		nPassedEvents.clear();
		nDim1Bins=(int)vecDim1Bins.size();
		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nPassedEvents.push_back(dummyInt2D);
		}
	}


	if (dim==2)
	{
		vecDim2Bins=bins;
		if ((vecDim1Bins.size()==0) || (vecDim2Bins.size()==0)) return false;

		dummyInt1D.push_back(0);

		nDim2Bins=(int)vecDim2Bins.size();
		for (i=0; i<(int)vecDim2Bins.size(); i++)
		{
			dummyInt2D.push_back(dummyInt1D);
		}
		nTotalEvents.clear();
		nPassedEvents.clear();

		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nPassedEvents.push_back(dummyInt2D);
		}
	}

	if (dim==3)
	{
		vecDim3Bins=bins;
		if ((vecDim1Bins.size()==0) || (vecDim2Bins.size()==0) || (vecDim3Bins.size()==0)) return false;

		nDim3Bins=(int)vecDim3Bins.size();
		for (i=0; i<(int)vecDim3Bins.size(); i++)
		{
			dummyInt1D.push_back(0);
		}
		for (i=0; i<(int)vecDim2Bins.size(); i++)
		{
			dummyInt2D.push_back(dummyInt1D);
		}
		nTotalEvents.clear();
		nPassedEvents.clear();
		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nPassedEvents.push_back(dummyInt2D);
		}
	}
	return true;
}


int MSEfficiency::getNumberOfBins(int dim)
{
	if (dim==1) return nDim1Bins-1;
	if (dim==2) return nDim2Bins-1;
	if (dim==3) return nDim3Bins-1;

	return dim;
}

bool MSEfficiency::getDimBinInfo(int dim, int id, double &meanOfBin, double &widthOfBin)
{
	if (dim==1)
	{
		if ((id<0) || (nDim1Bins<=id)) return false;
		meanOfBin  = vecDim1Bins[id]+(vecDim1Bins[id+1]-vecDim1Bins[id])/2.0;
		widthOfBin = (vecDim1Bins[id+1]-vecDim1Bins[id])/2.0;
	}
	if (dim==2)
	{
		if ((id<0) || (nDim2Bins<=id)) return false;
		meanOfBin  = vecDim2Bins[id]+(vecDim2Bins[id+1]-vecDim2Bins[id])/2.0;
		widthOfBin = (vecDim2Bins[id+1]-vecDim2Bins[id])/2.0;
	}
	if (dim==3)
	{
		if ((id<0) || (nDim3Bins<=id)) return false;
		meanOfBin  = vecDim3Bins[id]+(vecDim3Bins[id+1]-vecDim3Bins[id])/2.0;
		widthOfBin = (vecDim3Bins[id+1]-vecDim3Bins[id])/2.0;
	}
	return true;
}


//pt dim1
//eta dim2
//phi dim3
void MSEfficiency::setEfficiencyForEvent(bool pass, double dim1, double dim2, double dim3)
{
	int x=0;
	int y=0;
	int z=0;
	int i=0;

	if ((vecDim1Bins.size()==0) && (vecDim2Bins.size()==0) && (vecDim3Bins.size()==0)) return;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return;
	if ((dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<dim3)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++)	if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++)	if ((vecDim3Bins[i]<dim3) && (dim3<=vecDim3Bins[i+1])) z=i;

	nTotalEvents[x][y][z]++;
	if (pass==true) nPassedEvents[x][y][z]++;
}

void MSEfficiency::setEfficiencyForEvent(bool pass, double dim1, double dim2)
{
	int x=0;
	int y=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++)	if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;


	nTotalEvents[x][y][0]++;
	if (pass==true) nPassedEvents[x][y][0]++;
}

void MSEfficiency::setEfficiencyForEvent(bool pass, double dim1)
{
	int x=0;
	int i=0;

	if ((dim1<vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<=dim1) && (dim1<vecDim1Bins[i+1])) x=i;

	nTotalEvents[x][0][0]++;
	if (pass==true) nPassedEvents[x][0][0]++;
}


bool MSEfficiency::getOverallEfficiency(double &Eff, double &EffError)
{
	int i=0;
	int j=0;
	int k=0;
	int nTotal=0;
	int nPeak=0;

	for (i=0; i<nDim1Bins; i++)
		for (j=0; j<nDim2Bins; j++)
			for (k=0; k<nDim3Bins; k++)
			{
				nPeak+=nPassedEvents[i][j][k];
				nTotal+=nTotalEvents[i][j][k];
			}
	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}

bool MSEfficiency::getEfficiency123(double dim1, double dim2, double dim3, double &Eff, double &EffError)
{
	int x=0;
	int y=0;
	int z=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return false;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return false;
	if ((dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<dim3)) return false;


	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<dim3) && (dim3<=vecDim3Bins[i+1])) z=i;

	int nTot=nTotalEvents[x][y][z];
	if (nTot==0) return false;

	Eff	= ((double)(nPassedEvents[x][y][z]))/((double)(nTot));
	EffError= getError(nPassedEvents[x][y][z], nTot);
	return true;
}


bool MSEfficiency::getEfficiency1(double Dim1, double &Eff, double &EffError)
{
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	Eff=-1.0;
	EffError=-1.0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return false;

	for (i=0; i<nDim1Bins; i++)
	{
		if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1]))
		{
			for (j=0; j<nDim2Bins; j++)
				for (k=0; k<nDim3Bins; k++)
				{
					nPeak+=nPassedEvents[i][j][k];
					nTotal+=nTotalEvents[i][j][k];
				}
		}
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}


bool MSEfficiency::getEfficiency2(double Dim2, double &Eff, double &EffError)
{
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	Eff=-1.0;
	EffError=-1.0;

	if ((Dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<Dim2)) return false;

	for (i=0; i<nDim2Bins; i++)
	{
		if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1]))
		{
			for (j=0; j<nDim1Bins; j++)
				for (k=0; k<nDim3Bins; k++)
				{
					nPeak+=nPassedEvents[j][i][k];
					nTotal+=nTotalEvents[j][i][k];
				}
		}
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}


bool MSEfficiency::getEfficiency3(double Dim3, double &Eff, double &EffError)
{
	//int x=0;
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	Eff=-1.0;
	EffError=-1.0;

	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return false;

	for (i=0; i<nDim3Bins; i++)
	{
		if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1]))
		{
			//x=i;
			for (j=0; j<nDim1Bins; j++)
				for (k=0; k<nDim2Bins; k++)
				{
					nPeak+=nPassedEvents[j][k][i];
					nTotal+=nTotalEvents[j][k][i];
				}
		}
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}

bool MSEfficiency::getEfficiency13(double Dim1, double Dim3, double &Eff, double &EffError)
{
	int x=0;
	int z=0;
	int i=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	Eff=-1.0;
	EffError=-1.0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return false;
	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return false;

	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1])) z=i;

	for (k=0; k<nDim2Bins; k++)
	{
		nPeak+=nPassedEvents[x][k][z];
		nTotal+=nTotalEvents[x][k][z];
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}


bool MSEfficiency::getEfficiency12(double Dim1, double Dim2, double &Eff, double &EffError)
{
	int x=0;
	int y=0;
	int i=0;
	int k=0;

	Eff=-1.0;
	EffError=-1.0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return false;
	if ((Dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<Dim2)) return false;

	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1])) y=i;

	for (k=0; k<nDim3Bins; k++)
	{
		nPeak+=nPassedEvents[x][y][k];
		nTotal+=nTotalEvents[x][y][k];
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}


bool MSEfficiency::getEfficiency23(double Dim2, double Dim3, double &Eff, double &EffError)
{
	int y=0;
	int z=0;
	int i=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	Eff=-1.0;
	EffError=-1.0;

	if ((Dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<Dim2)) return false;
	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return false;

	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1])) z=i;

	for (k=0; k<nDim1Bins; k++)
	{
		nPeak+=nPassedEvents[k][y][z];
		nTotal+=nTotalEvents[k][y][z];
	}

	if (nTotal==0) return false;
	Eff	= ((double)(nPeak))/((double)(nTotal));
	EffError= getError(nPeak, nTotal);
	return true;
}

double  MSEfficiency::getError(int ok, int total)
{
	double q,p;
	p=((double)ok)/((double)total);
	q=1.0-p;
	if (ok==total) return getError(ok-1, total);
	return sqrt((p*q)/((double)total));
}


void  MSEfficiency::getTransformInformation(	vector<int>	&vecBinInfo, 
						vector<double>	&vecDimensionInfo,
						vector<int>	&vecStoredInformation)
{
	vecBinInfo.clear();
	vecDimensionInfo.clear();
	vecStoredInformation.clear();

	vecBinInfo.push_back(nDim1Bins);
	vecBinInfo.push_back(nDim2Bins);
	vecBinInfo.push_back(nDim3Bins);

	for (int i=0; i<nDim1Bins; i++)	vecDimensionInfo.push_back(vecDim1Bins[i]);
	for (int i=0; i<nDim2Bins; i++)	vecDimensionInfo.push_back(vecDim2Bins[i]);
	for (int i=0; i<nDim3Bins; i++)	vecDimensionInfo.push_back(vecDim3Bins[i]);

	for (int i1=0; i1<nDim1Bins; i1++)
		for (int i2=0; i2<nDim2Bins; i2++)
			for (int i3=0; i3<nDim3Bins; i3++)
			{
				vecStoredInformation.push_back(nPassedEvents[i1][i2][i3]);
				vecStoredInformation.push_back(nTotalEvents[i1][i2][i3]);
			}
}


void  MSEfficiency::setTransformInformation(	vector<int>	&vecBinInfo, 
						vector<double>	&vecDimensionInfo,
						vector<int>	&vecStoredInformation)
{
	nPassedEvents.clear();
	nTotalEvents.clear();

	if (vecBinInfo.size()!=3) return;
	
	nDim1Bins = vecBinInfo[0];
	nDim2Bins = vecBinInfo[1];
	nDim3Bins = vecBinInfo[2];

	vecDim1Bins.clear();
	vecDim2Bins.clear();
	vecDim3Bins.clear();
	for (int i=0; i<nDim1Bins; i++)	vecDim1Bins.push_back(vecDimensionInfo[i]);
	for (int i=0; i<nDim2Bins; i++)	vecDim2Bins.push_back(vecDimensionInfo[i+nDim1Bins]);
	for (int i=0; i<nDim3Bins; i++)	vecDim3Bins.push_back(vecDimensionInfo[i+nDim1Bins+nDim2Bins]);

	setupDim(1, vecDim1Bins);
	setupDim(2, vecDim2Bins);
	setupDim(3, vecDim3Bins);

	long n=0;
	for (int i1=0; i1<nDim1Bins; i1++)
		for (int i2=0; i2<nDim2Bins; i2++)
			for (int i3=0; i3<nDim3Bins; i3++)
			{
				nPassedEvents[i1][i2][i3]	= vecStoredInformation[n];
				n++;
				nTotalEvents[i1][i2][i3]	= vecStoredInformation[n];
				n++;
			}
}
