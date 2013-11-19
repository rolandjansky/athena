/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott (LMU)  
//            N. Benekos (MPI)
// July 2006
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "MuonEvtValidator/MSEfficiency.h"

MSEfficiency::MSEfficiency()
{
	clear();
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
	nTotalPassedEvents.clear();

}

bool MSEfficiency::setupDim(int dim, float start, float end, int bins)
{
	if (dim==1)
	{
		nDim1Bins=bins;
		vecDim1Bins.clear();
		vecDim1Bins.push_back(start);
		float step=(end-start)/((float)(bins));
		float s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim1Bins.push_back(s);
		}
		return setupDim(dim, vecDim1Bins);
	}
	if (dim==2)
	{
		if (nDim1Bins<=1) return false;
		nDim2Bins=bins;
		vecDim2Bins.clear();
		vecDim2Bins.push_back(start);
		float step=(end-start)/((float)(bins));
		float s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim2Bins.push_back(s);
		}
		return setupDim(dim, vecDim2Bins);
	}
	if (dim==3)
	{
		if (nDim2Bins<=1) return false;
		nDim3Bins=bins;
		vecDim3Bins.clear();
		vecDim3Bins.push_back(start);
		float step=(end-start)/((float)(bins));
		float s=start;
		for (int i=0; i<bins; i++)
		{
			s+=step;
			vecDim3Bins.push_back(s);
		}
		return setupDim(dim, vecDim3Bins);
	}

	return false;
}



bool MSEfficiency::setupDim(int dim, vector <float> &bins)
{
	vector<int> 		dummyInt1D;
	vector< vector<int> >	dummyInt2D;
	int i;

	if (dim==1)
	{
		if ((vecDim1Bins.size()==0) ) return false;
		vecDim3Bins=bins;

		dummyInt1D.push_back(0);
		dummyInt2D.push_back(dummyInt1D);

		nTotalEvents.clear();
		nTotalPassedEvents.clear();
		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nTotalPassedEvents.push_back(dummyInt2D);
		}
	}


	if (dim==2)
	{
		if ((vecDim1Bins.size()==0) || (vecDim2Bins.size()==0)) return false;
		vecDim3Bins=bins;

		dummyInt1D.push_back(0);

		for (i=0; i<(int)vecDim2Bins.size(); i++)
		{
			dummyInt2D.push_back(dummyInt1D);
		}
		nTotalEvents.clear();
		nTotalPassedEvents.clear();
		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nTotalPassedEvents.push_back(dummyInt2D);
		}
	}

	if (dim==3)
	{
		if ((vecDim1Bins.size()==0) || (vecDim2Bins.size()==0) || (vecDim3Bins.size()==0)) return false;
		vecDim3Bins=bins;

		for (i=0; i<(int)vecDim3Bins.size(); i++)
		{
			dummyInt1D.push_back(0);
		}
		for (i=0; i<(int)vecDim2Bins.size(); i++)
		{
			dummyInt2D.push_back(dummyInt1D);
		}
		nTotalEvents.clear();
		nTotalPassedEvents.clear();
		for (i=0; i<(int)vecDim1Bins.size(); i++)
		{
			nTotalEvents.push_back(dummyInt2D);
			nTotalPassedEvents.push_back(dummyInt2D);
		}
	}
	return true;
}


int MSEfficiency::getNumberOfBins(int dim)
{
	if (dim==1) return nDim1Bins;
	if (dim==2) return nDim2Bins;
	if (dim==3) return nDim3Bins;

	return dim;
}

bool MSEfficiency::getDimBinInfo(int dim, int id, float &meanOfBin, float &widthOfBin)
{
	if (dim==1)
	{
		if ((id<0) || (nDim1Bins<id)) return false;
		meanOfBin  = vecDim1Bins[id]+(vecDim1Bins[id+1]-vecDim1Bins[id])/2.0;
		widthOfBin = (vecDim1Bins[id+1]-vecDim1Bins[id])/2.0;
	}
	if (dim==2)
	{
		if ((id<0) || (nDim2Bins<id)) return false;
		meanOfBin  = vecDim2Bins[id]+(vecDim2Bins[id+1]-vecDim2Bins[id])/2.0;
		widthOfBin = (vecDim2Bins[id+1]-vecDim2Bins[id])/2.0;
	}
	if (dim==3)
	{
		if ((id<0) || (nDim3Bins<id)) return false;
		meanOfBin  = vecDim3Bins[id]+(vecDim3Bins[id+1]-vecDim3Bins[id])/2.0;
		widthOfBin = (vecDim3Bins[id+1]-vecDim3Bins[id])/2.0;
	}
	return false;
}


//pt dim1
//eta dim2
//phi dim3
void MSEfficiency::setEfficiencyForEvent(bool pass, float dim1, float dim2, float dim3)
{
	int x=0;
	int y=0;
	int z=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return;
	if ((dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<dim3)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++)	if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++)	if ((vecDim3Bins[i]<dim3) && (dim3<=vecDim3Bins[i+1])) z=i;

	nTotalEvents[x][y][z]++;
	if (pass==true) nTotalPassedEvents[x][y][z]++;
}

void MSEfficiency::setEfficiencyForEvent(bool pass, float dim1, float dim2)
{
	int x=0;
	int y=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++)	if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;


	nTotalEvents[x][y][0]++;
	if (pass==true) nTotalPassedEvents[x][y][0]++;
}

void MSEfficiency::setEfficiencyForEvent(bool pass, float dim1)
{
	int x=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;

	for (i=0; i<nDim1Bins; i++)	if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;

	nTotalEvents[x][0][0]++;
	if (pass==true) nTotalPassedEvents[x][0][0]++;
}


void MSEfficiency::getOverallEfficiency(float &Eff, float &EffError)
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
				nPeak+=nTotalPassedEvents[i][j][k];
				nTotal+=nTotalEvents[i][j][k];
			}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}

void MSEfficiency::getEfficiency123(float dim1, float dim2, float dim3, float &Eff, float &EffError)
{
	int x=0;
	int y=0;
	int z=0;
	int i=0;

	if ((dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<dim1)) return;
	if ((dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<dim2)) return;
	if ((dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<dim3)) return;


	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<dim1) && (dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<dim2) && (dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<dim3) && (dim3<=vecDim3Bins[i+1])) z=i;

	int nTot=nTotalEvents[x][y][z];
	if (nTot==0) nTot=1;

	Eff	= ((float)(nTotalPassedEvents[x][y][z]))/((float)(nTot));
	EffError= getError(nTotalPassedEvents[x][y][z], nTot);
}


void MSEfficiency::getEfficiency1(float Dim1, float &Eff, float &EffError)
{
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return;

	for (i=0; i<nDim1Bins; i++)
	{
		if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1]))
		{
			for (j=0; j<nDim2Bins; j++)
				for (k=0; k<nDim3Bins; k++)
				{
					nPeak+=nTotalPassedEvents[i][j][k];
					nTotal+=nTotalEvents[i][j][k];
				}
		}
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}


void MSEfficiency::getEfficiency2(float Dim2, float &Eff, float &EffError)
{
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<Dim2)) return;

	for (i=0; i<nDim2Bins; i++)
	{
		if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1]))
		{
			for (j=0; j<nDim1Bins; j++)
				for (k=0; k<nDim3Bins; k++)
				{
					nPeak+=nTotalPassedEvents[j][i][k];
					nTotal+=nTotalEvents[j][i][k];
				}
		}
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}


void MSEfficiency::getEfficiency3(float Dim3, float &Eff, float &EffError)
{
	int __attribute__((__unused__)) x=0; 
	int i=0;
	int j=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return;

	for (i=0; i<nDim3Bins; i++)
	{
		if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1]))
		{
			x=i;
			for (j=0; j<nDim1Bins; j++)
				for (k=0; k<nDim2Bins; k++)
				{
					nPeak+=nTotalPassedEvents[j][k][i];
					nTotal+=nTotalEvents[j][k][i];
				}
		}
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}

void MSEfficiency::getEfficiency13(float Dim1, float Dim3, float &Eff, float &EffError)
{
	int x=0;
	int z=0;
	int i=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return;
	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return;

	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1])) z=i;

	for (k=0; k<nDim2Bins; k++)
	{
		nPeak+=nTotalPassedEvents[x][k][z];
		nTotal+=nTotalEvents[x][k][z];
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}


void MSEfficiency::getEfficiency12(float Dim1, float Dim2, float &Eff, float &EffError)
{
	int x=0;
	int y=0;
	int i=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim1<=vecDim1Bins[0]) || (vecDim1Bins[vecDim1Bins.size()-1]<Dim1)) return;
	if ((Dim2<=vecDim2Bins[0]) || (vecDim3Bins[vecDim2Bins.size()-1]<Dim2)) return;

	for (i=0; i<nDim1Bins; i++) if ((vecDim1Bins[i]<Dim1) && (Dim1<=vecDim1Bins[i+1])) x=i;
	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1])) y=i;

	for (k=0; k<nDim3Bins; k++)
	{
		nPeak+=nTotalPassedEvents[x][y][k];
		nTotal+=nTotalEvents[x][y][k];
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}


void MSEfficiency::getEfficiency23(float Dim2, float Dim3, float &Eff, float &EffError)
{
	int y=0;
	int z=0;
	int i=0;
	int k=0;

	int nTotal=0;
	int nPeak=0;

	if ((Dim2<=vecDim2Bins[0]) || (vecDim2Bins[vecDim2Bins.size()-1]<Dim2)) return;
	if ((Dim3<=vecDim3Bins[0]) || (vecDim3Bins[vecDim3Bins.size()-1]<Dim3)) return;

	for (i=0; i<nDim2Bins; i++) if ((vecDim2Bins[i]<Dim2) && (Dim2<=vecDim2Bins[i+1])) y=i;
	for (i=0; i<nDim3Bins; i++) if ((vecDim3Bins[i]<Dim3) && (Dim3<=vecDim3Bins[i+1])) z=i;

	for (k=0; k<nDim1Bins; k++)
	{
		nPeak+=nTotalPassedEvents[k][y][z];
		nTotal+=nTotalEvents[k][y][z];
	}

	if (nTotal==0) nTotal=1;
	Eff	= ((float)(nPeak))/((float)(nTotal));
	EffError= getError(nPeak, nTotal);
}


double  MSEfficiency::getError(int ok, int total)
{
	double q,p;
	p=((double)ok)/((double)total);
	q=1.0-p;
	// Achtung!!!
	if ((total<20.0)) return -1.0;
	if (ok==total) return getError(ok-1, total);
	return sqrt((p*q)/((double)total));
}

