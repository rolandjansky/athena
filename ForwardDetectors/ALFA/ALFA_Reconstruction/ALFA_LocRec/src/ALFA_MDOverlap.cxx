/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_MDOverlap.h"
using namespace std;

ALFA_MDOverlap::ALFA_MDOverlap()
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

ALFA_MDOverlap::~ALFA_MDOverlap()
{

}

StatusCode ALFA_MDOverlap::Initialize(const eRPotName &eRPName, const std::list<MDHIT> &ListMDHits, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fzMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT])
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

StatusCode ALFA_MDOverlap::Execute()
{
	StatusCode sc;
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDOverlap::Execute()");

  // SELECT CANDIDATE HITS
	sc = SelectHitInLayer();
	if(sc.isFailure())
	{
		ATH_MSG_ERROR("hit selection failure");
		return sc;
	}

	sc = Overlap();
	if(sc.isFailure())
	{
		ATH_MSG_ERROR("Overlap failure");
		return sc;
	}


	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDOverlap::Finalize(Float_t &fRecXPos, Float_t &fRecYPos)
{
	fRecXPos = m_fRecXPos;
	fRecYPos = m_fRecYPos;

	HistFinalize();

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDOverlap::SelectHitInLayer()
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

	std::list<int>::iterator iterFiber;
	Int_t iHitFiber;

	mapLayers.clear();

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
			iHitFiber = *iterFiber;

			if (fmod((double)iLayer,(double)2) == 0)	// for U-fibers
			{
				fU = (m_fbMD[iLayer][iHitFiber]-m_fbMinU)/m_fFiberD;
				fZ = m_fzMD[iLayer][iHitFiber]/m_fzStep + 0.5;

				for(Int_t k=0; k < m_iTBins; k++)
				{
					fTheta  = m_fTLow + (k+0.5)*(m_fTHigh-m_fTLow)/m_iTBins;
					fRadius = fZ*cos(fTheta)+fU*sin(fTheta);

					histU_PT->Fill(fTheta, fRadius, 1.0);
				}
			}
			else	// for V-fibers
			{
				fU = (m_fbMD[iLayer][iHitFiber]-m_fbMinV)/m_fFiberD;
				fZ = (m_fzMD[iLayer][iHitFiber]+1.0)/m_fzStep + 0.5;

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
		fRMinU = 2.0;
		fRMinV = 2.0;
		m_iFHits[iLayer] = -9999;

		for (iterFiber=mapLayers[iLayer].ListFibers.begin(); iterFiber!=mapLayers[iLayer].ListFibers.end(); iterFiber++)
		{
			iHitFiber = *iterFiber;

			if (fmod((double)iLayer,(double)2) == 0)	// for U-fibers
			{
				fU = (m_fbMD[iLayer][iHitFiber]-m_fbMinU)/m_fFiberD;
				fZ = m_fzMD[iLayer][iHitFiber]/m_fzStep + 0.5;

				fRTmp = fZ*cos(fTLineU) + fU*sin(fTLineU);
				fRdr = fabs(fRLineU - fRTmp);

				if (fRdr < fRMinU)
				{
					fRMinU = fRdr;
					m_iFHits[iLayer] = iHitFiber;
				}
			}
			else	// for V-fibers
			{
				fU = (m_fbMD[iLayer][iHitFiber]-m_fbMinV)/m_fFiberD;
				fZ = (m_fzMD[iLayer][iHitFiber]+1.0)/m_fzStep + 0.5;

				fRTmp = fZ*cos(fTLineV) + fU*sin(fTLineV);
				fRdr = fabs(fRLineV - fRTmp);

				if (fRdr < fRMinV)
				{
					fRMinV = fRdr;
					m_iFHits[iLayer] = iHitFiber;
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDOverlap::Overlap()
{
	Int_t n_p = 0;
	Int_t n_n = 0;
	Float_t faMeanP = 0;
	Float_t fbMeanP = 0;
	Float_t faMeanN = 0;
	Float_t fbMeanN = 0;

	Float_t fbMinP, fbMaxP, fbMinN, fbMaxN;
	Float_t fActive = 0.48;
	Float_t fHalfStep = 0;

	for(Int_t iLayer = 0; iLayer < ALFAPLATESCNT*ALFALAYERSCNT; iLayer++)
	{
		if(m_iFHits[iLayer]!= -9999)
		{
			if(m_faMD[iLayer][0] >= 0)
			{
				fbMeanP += m_fbMD[iLayer][m_iFHits[iLayer]];
				++n_p;
			}
			else
			{
				fbMeanN += m_fbMD[iLayer][m_iFHits[iLayer]];
				++n_n;
			}
		}
	}

	if ((n_p >= 1) && (n_n >= 1))
	{
		fbMeanP = fbMeanP / n_p + 0.07071;
		fbMeanN = fbMeanN / n_n - 0.07071;
	}

	// these mean b values are just used to generate reasonable start values
	// and to avoid that one starts from a bad (i.e. cross talk) fiber
	fbMinP = fbMeanP + fActive;
	fbMaxP = fbMeanP - fActive;
	fbMinN = fbMeanN + fActive;
	fbMaxN = fbMeanN - fActive;

	Int_t iLayMinU = -9999;
	Int_t iLayMaxU = -9999;
	Int_t iLayMinV = -9999;
	Int_t iLayMaxV = -9999;

	// now find minimum overlap in b values, separately for positive and
	// negative slopes

	n_p = 0;
	n_n = 0;
	for(Int_t iLayer = 0; iLayer < ALFAPLATESCNT*ALFALAYERSCNT; iLayer++)
	{
		if(iLayer>=0 && m_iFHits[iLayer]!= -9999)
		{
			if(m_faMD[iLayer][0] >= 0)
			{
				fHalfStep = fabs(0.5*fActive/sin(atan(m_faMD[iLayer][m_iFHits[iLayer]])));

				if (m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep > fbMaxP)
				{
					if (fbMinP > m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep)
					{
						fbMinP = m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep;
						iLayMinU = iLayer;
					}
					else
					{
						fbMinP = fbMinP;
					}
				}

				if (m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep < fbMinP)
				{
					if (fbMaxP > m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep)
					{
						fbMaxP = fbMaxP;
					}
					else
					{
						fbMaxP = m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep;
						iLayMaxU = iLayer;
					}
				}

				faMeanP = faMeanP + m_faMD[iLayer][m_iFHits[iLayer]];
				++n_p;
			}
			else
			{
				fHalfStep = fabs(0.5*fActive/sin(atan(m_faMD[iLayer][m_iFHits[iLayer]])));

				if (m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep > fbMaxN)
				{
					if (fbMinN > m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep)
					{
						fbMinN =m_fbMD[iLayer][m_iFHits[iLayer]] + fHalfStep;
						iLayMinV = iLayer;
					}
					else
					{
						fbMinN = fbMinN;
					}
				}

				if (m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep < fbMinN)
				{
					if (fbMaxN > m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep)
					{
						fbMaxN = fbMaxN;
					}
					else
					{
						fbMaxN = m_fbMD[iLayer][m_iFHits[iLayer]] - fHalfStep;
						iLayMaxV = iLayer;
					}
				}

				faMeanN = faMeanN + m_faMD[iLayer][m_iFHits[iLayer]];
				++n_n;
			}
		}
	}

	Double_t fPolyX[4];
	Double_t fPolyY[4];

//	Float_t OL_x;
//	Float_t OL_y;

	if (iLayMinU>=0 && iLayMaxU>=0 && iLayMinV>=0 && iLayMaxV>=0)
	{
		fPolyX[0] = (m_fbMD[iLayMinU][m_iFHits[iLayMinU]] - m_fbMD[iLayMinV][m_iFHits[iLayMinV]])/(m_faMD[iLayMinV][m_iFHits[iLayMinV]] - m_faMD[iLayMinU][m_iFHits[iLayMinU]]);
		fPolyY[0] = m_faMD[iLayMinU][m_iFHits[iLayMinU]]*fPolyX[0] + m_fbMD[iLayMinU][m_iFHits[iLayMinU]] + fActive*0.707/2.0;

		fPolyX[1] = (m_fbMD[iLayMinU][m_iFHits[iLayMinU]] - m_fbMD[iLayMaxV][m_iFHits[iLayMaxV]] + fActive*0.707)/(m_faMD[iLayMaxV][m_iFHits[iLayMaxV]] - m_faMD[iLayMinU][m_iFHits[iLayMinU]]);
		fPolyY[1] = m_faMD[iLayMinU][m_iFHits[iLayMinU]]*fPolyX[1] + m_fbMD[iLayMinU][m_iFHits[iLayMinU]] + fActive*0.707/2.0;

		fPolyX[2] = (m_fbMD[iLayMaxU][m_iFHits[iLayMaxU]] - m_fbMD[iLayMinV][m_iFHits[iLayMinV]] - fActive*0.707)/(m_faMD[iLayMinV][m_iFHits[iLayMinV]] - m_faMD[iLayMaxU][m_iFHits[iLayMaxU]]);
		fPolyY[2] =  m_faMD[iLayMaxU][m_iFHits[iLayMaxU]]*fPolyX[2] + m_fbMD[iLayMaxU][m_iFHits[iLayMaxU]] - fActive*0.707/2.0;

		fPolyX[3] = (m_fbMD[iLayMaxU][m_iFHits[iLayMaxU]] - m_fbMD[iLayMaxV][m_iFHits[iLayMaxV]])/(m_faMD[iLayMaxV][m_iFHits[iLayMaxV]] - m_faMD[iLayMaxU][m_iFHits[iLayMaxU]]);
		fPolyY[3] = m_faMD[iLayMaxU][m_iFHits[iLayMaxU]]*fPolyX[3] + m_fbMD[iLayMaxU][m_iFHits[iLayMaxU]] - fActive*0.707/2.0;

		m_fRecXPos = (fPolyX[0] + fPolyX[1] + fPolyX[2] + fPolyX[3])/4.0;
		m_fRecYPos = (fPolyY[0] + fPolyY[1] + fPolyY[2] + fPolyY[3])/4.0;

//		OL_x = (fabs(fPolyX[0] - m_fRecXPos) + fabs(fPolyX[1] - m_fRecXPos) + fabs(fPolyX[2] - m_fRecXPos) + fabs(fPolyX[3] - m_fRecXPos))/4.0;
//		OL_y = (fabs(fPolyY[0] - m_fRecYPos) + fabs(fPolyY[1] - m_fRecYPos) + fabs(fPolyY[2] - m_fRecYPos) + fabs(fPolyY[3] - m_fRecYPos))/4.0;
	}
	else
	{
		m_fRecXPos = -9999.0;
		m_fRecYPos = -9999.0;

//		OL_x = -9999.0;
//		OL_y = -9999.0;
	}

	return StatusCode::SUCCESS;
}


void ALFA_MDOverlap::HistInitialize()
{
	histU_PT = new TH2D("histU_PT", "Hough Trans. of U", m_iTBins, m_fTLow, m_fTHigh, m_iRBins, m_fRLow, m_fRHigh);
	histV_PT = new TH2D("histV_PT", "Hough Trans. of V", m_iTBins, m_fTLow, m_fTHigh, m_iRBins, m_fRLow, m_fRHigh);
}

void ALFA_MDOverlap::HistFinalize()
{
	delete histU_PT;
	delete histV_PT;
}

void ALFA_MDOverlap::GetData(Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT])
{
	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		iFibSel[iLayer] = m_iFHits[iLayer];
	}
}
