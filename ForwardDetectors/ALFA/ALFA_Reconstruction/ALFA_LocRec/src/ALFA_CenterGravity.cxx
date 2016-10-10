/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_CenterGravity.h"
using namespace std;

ALFA_CenterGravity::ALFA_CenterGravity()
{
	histU_PT = NULL;
	histV_PT = NULL;



	m_fFiberD = 0.0;
	m_fHitBU  = 0.0;
	m_fHitBV  = 0.0;
	m_fPrivPI = 0.0;
	m_fRHigh  = 0.0;
	m_fRLow   = 0.0;
	m_fRecXPos= 0.0;
	m_fRecYPos= 0.0;
	m_fTHigh  = 0.0;
	m_fTLow   = 0.0;
	m_fbMinU  = 0.0;
	m_fbMinV  = 0.0;
	m_fzStep  = 0.0;
	m_iRBins  = 0;
	m_iRPot   = 0;
	m_iTBins  = 0;
	memset(&m_iFHits, 0, sizeof(m_iFHits));
}

ALFA_CenterGravity::~ALFA_CenterGravity()
{

}

StatusCode ALFA_CenterGravity::Initialize(const eRPotName &eRPName, const std::list<MDHIT> &ListMDHits, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fzMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT])
{
	m_iRPot = eRPName - 1;
	m_ListMDHits = ListMDHits;

	for (Int_t iLayer = 0; iLayer < ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		for (Int_t iFiber = 0; iFiber < ALFAFIBERSCNT; iFiber++)
		{
			m_faMD[iLayer][iFiber] = faMD[m_iRPot][iLayer][iFiber];
			m_fbMD[iLayer][iFiber] = fbMD[m_iRPot][iLayer][iFiber];
			m_fzMD[iLayer][iFiber] = fzMD[m_iRPot][iLayer][iFiber];
		}
	}

	Float_t fzBase = m_fzMD[0][0];
	m_fbMinU = 1111.0;
	m_fbMinV = 1111.0;

	for (Int_t iLayer = 0; iLayer < ALFAPLATESCNT*ALFALAYERSCNT; iLayer++)
	{
		for (Int_t iFiber = 0; iFiber < ALFAFIBERSCNT; iFiber++)
		{
			if ((m_fbMD[iLayer][iFiber] < m_fbMinU) && m_faMD[iLayer][iFiber] < 0)
			{
				m_fbMinU = m_fbMD[iLayer][iFiber];
			}

			if ((m_fbMD[iLayer][iFiber] < m_fbMinV) && m_faMD[iLayer][iFiber] > 0)
			{
				m_fbMinV = m_fbMD[iLayer][iFiber];
			}

			// rescale zPos
			m_fzMD[iLayer][iFiber] -= fzBase;
		}
	}

	m_fFiberD = 0.7071;
	m_fzStep  = 2.0;

	m_fPrivPI= 3.14159265359;
	m_fTLow  = (1.0/4.0)*m_fPrivPI;
	m_fTHigh = (3.0/4.0)*m_fPrivPI;
	m_fRLow  = 0.0;
	m_fRHigh = 64.0;
	m_iTBins = 100;
	m_iRBins = 64;


	HistInitialize();

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CenterGravity::Execute()
{
	StatusCode sc;
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CenterGravity::Execute()");

  // SELECT CANDIDATE HITS
	sc = SelectHitInLayer();
	if(sc.isFailure())
	{
		ATH_MSG_ERROR(" hit selection failure ");
		return sc;
	}

	sc = CenterGravity();
	if(sc.isFailure())
	{
		ATH_MSG_ERROR(" CenterGravity failure ");
		return sc;
	}


	return StatusCode::SUCCESS;
}

StatusCode ALFA_CenterGravity::Finalize(Float_t &fRecXPos, Float_t &fRecYPos)
{
	fRecXPos = m_fRecXPos;
	fRecYPos = m_fRecYPos;

	HistFinalize();

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CenterGravity::SelectHitInLayer()
{
	// Reset Histograms
	histU_PT->Reset();
	histV_PT->Reset();

	// U (V) vs Z and Hough Transformed 2D histograms
	Float_t fU = 0;
	Float_t fZ = 0;
	Float_t fRadius = 0;
	Float_t fTheta  = 0;

	FIBERS structFibers;
	std::map<int, FIBERS> mapLayers;
	mapLayers.clear();
	std::list<int>::iterator iterFiber;

	std::list<MDHIT>::const_iterator iter;
	for (iter=m_ListMDHits.begin(); iter!=m_ListMDHits.end(); iter++)
	{
		if (m_iRPot == (*iter).iRPot)
		{
			if(mapLayers.find((*iter).iPlate)==mapLayers.end())
			{
				structFibers.ListFibers.clear();

				mapLayers.insert(std::pair<int, FIBERS>((*iter).iPlate, structFibers));
				mapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
			}
			else
			{
				mapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
			}
		}
	}

	for (Int_t iLayer = 0; iLayer < ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		for (iterFiber=mapLayers[iLayer].ListFibers.begin(); iterFiber!=mapLayers[iLayer].ListFibers.end(); iterFiber++)
		{
			if (fmod((double)iLayer,(double)2) == 0)	// for U-fibers
			{
				fU = (m_fbMD[iLayer][*iterFiber]-m_fbMinU)/m_fFiberD;
				fZ = m_fzMD[iLayer][*iterFiber]/m_fzStep + 0.5;

				for(Int_t k=0; k < m_iTBins; k++)
				{
					fTheta  = m_fTLow + (k+0.5)*(m_fTHigh-m_fTLow)/m_iTBins;
					fRadius = fZ*cos(fTheta)+fU*sin(fTheta);

					histU_PT->Fill(fTheta, fRadius, 1.0);
				}
			}
			else	// for V-fibers
			{
				fU = (m_fbMD[iLayer][*iterFiber]-m_fbMinV)/m_fFiberD;
				fZ = (m_fzMD[iLayer][*iterFiber]+1.0)/m_fzStep + 0.5;

				for(Int_t k=0; k < m_iTBins; k++)
				{
					fTheta  = m_fTLow + (k+0.5)*(m_fTHigh-m_fTLow)/m_iTBins;
					fRadius = fZ*cos(fTheta)+fU*sin(fTheta);

					histV_PT->Fill(fTheta, fRadius, 1.0);
				}
			}
		}
	}

	// Get maximumbin from PT histograms and determine seed track
	Int_t iULocMax, iULocMay, iULocMaz;
	Int_t iVLocMax, iVLocMay, iVLocMaz;

	histU_PT->GetMaximumBin(iULocMax, iULocMay, iULocMaz);
	histV_PT->GetMaximumBin(iVLocMax, iVLocMay, iVLocMaz);

	// Select Hits closest to seed track ( dr < 2.0 )
	Float_t fRMinU, fRMinV, fRTmp, fRdr;

	Float_t fRLineU = m_fRLow + (iULocMay-0.5)*(m_fRHigh-m_fRLow)/m_iRBins; // -0.5 since bins start with 1 (first bin, second...)
	Float_t fRLineV = m_fRLow + (iVLocMay-0.5)*(m_fRHigh-m_fRLow)/m_iRBins;

	Double_t fTLineU = m_fTLow + (iULocMax-0.5)*(m_fTHigh-m_fTLow)/m_iTBins;
	Double_t fTLineV = m_fTLow + (iVLocMax-0.5)*(m_fTHigh-m_fTLow)/m_iTBins;


	Float_t fAlphaU = -cos(fTLineU)/sin(fTLineU);
	Float_t fAlphaV = -cos(fTLineV)/sin(fTLineV);

	Float_t fBetaU = fRLineU/sin(fTLineU);
	Float_t fBetaV = fRLineV/sin(fTLineV);

	Float_t fBUMin = fAlphaU*0.0 + fBetaU;
	Float_t fBUMax = fAlphaU*19.0 + fBetaU;

	Float_t fBVMin = fAlphaV*1.0 + fBetaV;
	Float_t fBVMax = fAlphaV*20.0 + fBetaV;

	m_fHitBU = (fBUMax-fBUMin)/2.0 + fBUMin;
	m_fHitBV = (fBVMax-fBVMin)/2.0 + fBVMin;

	for (Int_t iLayer = 0; iLayer < ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		//fRMinU = fRMinV = 2.0;
		fRMinU = fRMinV = 0.5;
		m_iFHits[iLayer] = -9999;

		for (iterFiber=mapLayers[iLayer].ListFibers.begin(); iterFiber!=mapLayers[iLayer].ListFibers.end(); iterFiber++)
		{
			if (fmod((double)iLayer,(double)2) == 0)	// for U-fibers
			{
				fU = (m_fbMD[iLayer][*iterFiber]-m_fbMinU)/m_fFiberD;
				fZ = m_fzMD[iLayer][*iterFiber]/m_fzStep + 0.5;

				fRTmp = fZ*cos(fTLineU) + fU*sin(fTLineU);
				fRdr = fabs(fRLineU - fRTmp);

				if (fRdr < fRMinU)
				{
					fRMinU = fRdr;
					m_iFHits[iLayer] = *iterFiber;
				}
			}
			else	// for V-fibers
			{
				fU = (m_fbMD[iLayer][*iterFiber]-m_fbMinV)/m_fFiberD;
				fZ = (m_fzMD[iLayer][*iterFiber]+1.0)/m_fzStep + 0.5;

				fRTmp = fZ*cos(fTLineV) + fU*sin(fTLineV);
				fRdr = fabs(fRLineV - fRTmp);

				if (fRdr < fRMinV)
				{
					fRMinV = fRdr;
					m_iFHits[iLayer] = *iterFiber;
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CenterGravity::CenterGravity()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CenterGravity::CenterGravity()");

	Int_t n_p = 0;
	Int_t n_n = 0;
	Float_t faMeanP = 0;
	Float_t fbMeanP = 0;
	Float_t faMeanN = 0;
	Float_t fbMeanN = 0;

	// Layer loop
	for(Int_t iLayer = 0; iLayer < ALFAPLATESCNT*ALFALAYERSCNT; iLayer++)
	{
		if(m_iFHits[iLayer]!= -9999)
		{
			// Separate pos. and neg. slope layers
			if(m_faMD[iLayer][1] >= 0)
			{
				faMeanP = faMeanP + m_faMD[iLayer][m_iFHits[iLayer]];
				fbMeanP = fbMeanP + m_fbMD[iLayer][m_iFHits[iLayer]];		// + 0.07071; // due to the staggering - see my ALFA talk from 18.02.2008
				n_p++;
			}
			else
			{
				faMeanN = faMeanN + m_faMD[iLayer][m_iFHits[iLayer]];
				fbMeanN = fbMeanN + m_fbMD[iLayer][m_iFHits[iLayer]];		// - 0.07071;
				n_n++;
			}
		}
	}

	// At least one hit in pos. and neg. slope layer
	if((n_p>=1)&&(n_n>=1))
	{
		faMeanP = faMeanP/n_p;
		fbMeanP = fbMeanP/n_p;
		faMeanN = faMeanN/n_n;
		fbMeanN = fbMeanN/n_n;

		if(faMeanP == faMeanN)
		{
			ATH_MSG_DEBUG("something goes wrong, faMeanP = faMeanN ");
		}

		m_fRecXPos = (fbMeanN - fbMeanP)/(faMeanP - faMeanN);
		m_fRecYPos = faMeanP * m_fRecXPos + fbMeanP;
	}
	else
	{
		m_fRecXPos = -9999;
		m_fRecYPos = -9999;
	}

	return StatusCode::SUCCESS;
}


void ALFA_CenterGravity::HistInitialize()
{
	histU_PT = new TH2D("histU_PT", "Hough Trans. of U", m_iTBins, m_fTLow, m_fTHigh, m_iRBins, m_fRLow, m_fRHigh);
	histV_PT = new TH2D("histV_PT", "Hough Trans. of V", m_iTBins, m_fTLow, m_fTHigh, m_iRBins, m_fRLow, m_fRHigh);
}

void ALFA_CenterGravity::HistFinalize()
{
	if (histU_PT!=NULL) delete histU_PT;
	if (histV_PT!=NULL) delete histV_PT;
	histU_PT = NULL;
	histV_PT = NULL;
}

void ALFA_CenterGravity::GetData(Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT])
{
	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		iFibSel[iLayer] = m_iFHits[iLayer];
	}
}
