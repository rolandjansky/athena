/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_ODTracking.h"

using namespace std;

ALFA_ODTracking::ALFA_ODTracking()
{
	m_listResults.clear();

	m_iMultiplicityCut = 0;
	m_fDistanceCut = 0.0;
	m_iLayerCut = 0;
	m_iDataType = 0;

	memset(&m_iFibSel, 0, sizeof(m_iFibSel));
	std::fill_n(&m_iFibSel[0][0], sizeof(m_iFibSel)/sizeof(Int_t), 9999);
}

ALFA_ODTracking::~ALFA_ODTracking()
{
	m_listResults.clear();
}

StatusCode ALFA_ODTracking::Initialize(Int_t iMultiplicityCut, Float_t fDistanceCut, Int_t iLayerCut, Int_t iDataType)
{
	m_iMultiplicityCut = (int)iMultiplicityCut;
	m_fDistanceCut = fDistanceCut;
	m_iLayerCut = iLayerCut;
	m_iDataType = iDataType;
	
	return StatusCode::SUCCESS;
}

StatusCode ALFA_ODTracking::Execute(Int_t iRPot, const std::list<ODHIT> &ListODHits, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT])
{
	Int_t iLayer;
	FIBERS structFibers;
	std::map<int, FIBERS> MapLayers;
	MapLayers.clear();
	std::list<ODHIT>::const_iterator iter;
	for (iter=ListODHits.begin(); iter!=ListODHits.end(); iter++)
	{
		if (iRPot == (*iter).iRPot)
		{
			iLayer = 2*(*iter).iPlate + (*iter).iSide;
			if(MapLayers.find(iLayer)==MapLayers.end())
			{
				structFibers.ListFibers.clear();
				MapLayers.insert(std::pair<int, FIBERS>(iLayer, structFibers));
				MapLayers[iLayer].ListFibers.push_back((*iter).iFiber);
			}
			else
			{
				MapLayers[iLayer].ListFibers.push_back((*iter).iFiber);
			}
		}
	}

	memset(&m_iMulti, 0, sizeof(m_iMulti));

	Int_t iSide, iPlate;
	Int_t iMaxLay = ODSIDESCNT*ODPLATESCNT;
	for (Int_t iLayer=0; iLayer<iMaxLay; iLayer++)
	{
		iPlate = iLayer/2;
		iSide = iLayer%2;
		m_iMulti[iSide][iPlate] = MapLayers[iLayer].ListFibers.size();
	}

	FiberProjection(iRPot, MapLayers, faOD, fbOD);
//	FindingPosition(iRPot, MapLayers, faOD, fbOD);

	return StatusCode::SUCCESS;
}

StatusCode ALFA_ODTracking::Finalize(std::list<ODRESULT>* pListResults)
{
	*pListResults = m_listResults;

	return StatusCode::SUCCESS;
}

void ALFA_ODTracking::FiberProjection(Int_t iRPot, std::map<int, FIBERS> &MapLayers, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT])
{
	const Int_t NBINTOT = 17000;
	Int_t iSign;
	Int_t iHitLayer[ODSIDESCNT][NBINTOT];
	Float_t fPosition;
	Float_t fFiberXPos = 23;

	fFiberXPos = (m_iDataType==1)? 22 : 23;

	std::list<int>::const_iterator iIter;

	ODRESULT Results;

	for (Int_t iSide=0; iSide<ODSIDESCNT; iSide++)
	{
		Results.iSide	= -1;
		Results.fRecPos	= -9999.0;
		Results.fOverY  = -9999.0;
		Results.iNumY   = -1;

		iSign = (iSide==0)? -1 : 1;

		for (Int_t iBin=0; iBin<NBINTOT; iBin++)
		{
			iHitLayer[iSide][iBin] = 0;
		}

		for (Int_t iPlate=0; iPlate<ODPLATESCNT; iPlate++)
		{
			//multiplicity cut
			if (MapLayers[2*iPlate+iSide].ListFibers.size()<=(UInt_t)m_iMultiplicityCut && MapLayers[2*iPlate+iSide].ListFibers.size() > 0)
			{
				//Making the projections
				for (iIter=MapLayers[2*iPlate+iSide].ListFibers.begin(); iIter!=MapLayers[2*iPlate+iSide].ListFibers.end(); iIter++)
				{
//					cout << "iPot, iPlate, iSide, iFiber = " << iRPot << ", " << iPlate << ", " << iSide << ", " << *iIter << endl;
					fPosition = -1*(iSign*fFiberXPos*faOD[iRPot][iPlate][iSide][*iIter] + fbOD[iRPot][iPlate][iSide][*iIter]);
					for (Int_t iBin=0; iBin<480; iBin++)
					{
						iHitLayer[iSide][(int)(((fPosition-0.24)*1000)+iBin-128000)]++;
					}
				}
			}
		}

		Int_t iNum = 0;
		std::vector<int> *vecMaxHit = new std::vector<int>;

		//Determining the maximum overlap between fibers
		for (Int_t iBin=0; iBin<NBINTOT; iBin++)
		{
			if (iHitLayer[iSide][iBin]>iNum) iNum = iHitLayer[iSide][iBin];
		}

		//Determining the position and the width of the maximum
		//iNum = <0;3> depends on the count of hit layers
		if (iNum>=m_iLayerCut)
		{
			for (Int_t iBin=0; iBin<NBINTOT; iBin++)
			{
				if (iHitLayer[iSide][iBin]==iNum)
				{
					vecMaxHit->push_back(iBin);
				}
			}

			Float_t fHitMinPos, fHitMaxPos;
//			Int_t iHitMinPos_tmp, iHitMaxPos_tmp;
//			Int_t iFullWidth;

			fHitMinPos = -128.0 - (double)(vecMaxHit->front())*1e-3;
			fHitMaxPos = -128.0 - (double)(vecMaxHit->back())*1e-3;

//			iHitMinPos_tmp = vecMaxHit->front();
//			iHitMaxPos_tmp = vecMaxHit->back();

			// Making sure that the plateau belongs to the same track
//			iFullWidth = vecMaxHit->size();

//			for (Int_t i=0; i<iFullWidth; i++)
//			{
//				if (vecMaxHit->at(iFullWidth-i-1)-iHitMinPos_tmp < 500)
//				{
//					iHitMaxPos_tmp = vecMaxHit->at(iFullWidth-i-1);
//					fHitMaxPos = -128.0 - (double)(iHitMaxPos_tmp)*1e-3;
//					break;
//				}
//			}

			if ((fHitMinPos-fHitMaxPos)<m_fDistanceCut)
			{
				fPosition       = (fHitMinPos + fHitMaxPos)/2.0;
				Results.iSide	= iSide;
				Results.fRecPos	= fPosition;
				Results.fOverY  = fHitMinPos-fHitMaxPos;
				Results.iNumY   = iNum;
				m_listResults.push_back(Results);
			}

			// m_iFibSelOD ------------------------------------------------------------------
			Float_t fMin = 0.0;
			Float_t fDist = 0.0/*, fDistMin = 0.0*/;
			Int_t iFibSel = 9999;

			if (Results.fRecPos !=-9999.0)
			{
				for (Int_t iPlate=0; iPlate<ODPLATESCNT; iPlate++)
				{
					iFibSel = 9999;
					fMin = 0.24/cos(TMath::Pi()/4.0);

					for (iIter=MapLayers[2*iPlate+iSide].ListFibers.begin(); iIter!=MapLayers[2*iPlate+iSide].ListFibers.end(); iIter++)
					{
						fPosition = iSign*fFiberXPos*faOD[iRPot][iPlate][iSide][*iIter] + fbOD[iRPot][iPlate][iSide][*iIter];
						fDist = TMath::Abs(Results.fRecPos-fPosition);

						if (fDist<=fMin)
						{
//							fDistMin = Results.fRecPos-fPosition;
							fMin = fDist;
							iFibSel = *iIter;
						}
					}

					m_iFibSel[iSide][iPlate] = iFibSel;
				}
			}
		}


		delete vecMaxHit;
	}
}

void ALFA_ODTracking::FindingPosition(Int_t iRPot, std::map<int, FIBERS> &MapLayers, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT])
{
	Int_t iSign;
	Float_t fDistanceA;
	Float_t fDistanceB;
	Float_t fPosition;

	Float_t fFiberXPos = 23;
	fFiberXPos = (m_iDataType==1)? 22 : 23;

	ODRESULT Results;
	Results.clear();

//	std::list<int>::iterator intIter0;
//	std::list<int>::iterator intIter1;
//	std::list<int>::iterator intIter2;
	std::list<int>::const_iterator intIter0;
	std::list<int>::const_iterator intIter1;
	std::list<int>::const_iterator intIter2;

	for (Int_t iSide=0; iSide<ODSIDESCNT; iSide++)
	{
		iSign = (iSide==0) ? -1 : 1;

		if (((Int_t)MapLayers[0+iSide].ListFibers.size()<=m_iMultiplicityCut) && ((Int_t)MapLayers[2+iSide].ListFibers.size()<=m_iMultiplicityCut) && ((Int_t)MapLayers[4+iSide].ListFibers.size()<=m_iMultiplicityCut))
		{
			if (MapLayers[0+iSide].ListFibers.size()>0)
			{
				for (intIter0=MapLayers[0+iSide].ListFibers.begin(); intIter0!=MapLayers[0+iSide].ListFibers.end(); intIter0++)
				{
					if (MapLayers[2+iSide].ListFibers.size()>0)
					{
						for (intIter1=MapLayers[2+iSide].ListFibers.begin(); intIter1!=MapLayers[2+iSide].ListFibers.end(); intIter1++)
						{
							if (MapLayers[4+iSide].ListFibers.size()>0)
							{
								for (intIter2=MapLayers[4+iSide].ListFibers.begin(); intIter2!=MapLayers[4+iSide].ListFibers.end(); intIter2++)
								{
									fDistanceA = TMath::Abs(iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0] + fbOD[iRPot][0][iSide][*intIter0] - iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1] - fbOD[iRPot][1][iSide][*intIter1]);
									fDistanceB = TMath::Abs(iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1] + fbOD[iRPot][1][iSide][*intIter1] - iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2] - fbOD[iRPot][2][iSide][*intIter2]);

									if (fDistanceA<m_fDistanceCut && fDistanceB<m_fDistanceCut)
									{
										fPosition = (iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0]+fbOD[iRPot][0][iSide][*intIter0] + iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1]+fbOD[iRPot][1][iSide][*intIter1] + iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2]+fbOD[iRPot][2][iSide][*intIter2])/3.0;
										Results.iSide   = iSide;
										Results.fRecPos	= fPosition;
										m_listResults.push_back(Results);
									}
								}
							}
							else
							{
								fDistanceA = TMath::Abs(iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0]+fbOD[iRPot][0][iSide][*intIter0] - iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1]-fbOD[iRPot][1][iSide][*intIter1]);
								if (fDistanceA < m_fDistanceCut)
								{
									fPosition = (iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0]+fbOD[iRPot][0][iSide][*intIter0] + iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1]+fbOD[iRPot][1][iSide][*intIter1])/2.0;
									Results.iSide   = iSide;
									Results.fRecPos	= fPosition;
									m_listResults.push_back(Results);
								}
							}
						}
					}
					else
					{
						if (MapLayers[4+iSide].ListFibers.size()>0)
						{
							for (intIter2=MapLayers[2*2+iSide].ListFibers.begin(); intIter2!=MapLayers[2*2+iSide].ListFibers.end(); intIter2++)
							{
								fDistanceA = TMath::Abs(iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0]+fbOD[iRPot][0][iSide][*intIter0] - iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2]-fbOD[iRPot][2][iSide][*intIter2]);
								if (fDistanceA < m_fDistanceCut)
								{
									fPosition = (iSign*fFiberXPos*faOD[iRPot][0][iSide][*intIter0]+fbOD[iRPot][0][iSide][*intIter0] + iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2]+fbOD[iRPot][2][iSide][*intIter2])/2.0;
									Results.iSide   = iSide;
									Results.fRecPos	= fPosition;
									m_listResults.push_back(Results);
								}
							}
						}
					}
				}
			}
			else
			{
				if (MapLayers[2+iSide].ListFibers.size()>0)
				{
					for (intIter1=MapLayers[2*1+iSide].ListFibers.begin(); intIter1!=MapLayers[2*1+iSide].ListFibers.end(); intIter1++)
					{
						if (MapLayers[4+iSide].ListFibers.size()>0)
						{
							for (intIter2=MapLayers[2*2+iSide].ListFibers.begin(); intIter2!=MapLayers[2*2+iSide].ListFibers.end(); intIter2++)
							{
								fDistanceB = TMath::Abs(iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1]+fbOD[iRPot][1][iSide][*intIter1] - iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2]-fbOD[iRPot][2][iSide][*intIter2]);
								if (fDistanceB<m_fDistanceCut)
								{
									fPosition = (iSign*fFiberXPos*faOD[iRPot][1][iSide][*intIter1]+fbOD[iRPot][1][iSide][*intIter1] + iSign*fFiberXPos*faOD[iRPot][2][iSide][*intIter2]+fbOD[iRPot][2][iSide][*intIter2])/2.0;
									Results.iSide	= iSide;
									Results.fRecPos	= fPosition;
									m_listResults.push_back(Results);
								}
							}
						}
					}
				}
			}
		}
	}
}

void ALFA_ODTracking::GetData(Int_t (&iFibSel)[ODSIDESCNT][ODPLATESCNT])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_ODTracking::GetData()");
	ATH_MSG_DEBUG("begin ALFA_ODTracking::GetData()");


	for (int iSide=0; iSide<ODSIDESCNT; iSide++)
	{
		for (int iPlate=0; iPlate<ODPLATESCNT; iPlate++)
		{
			iFibSel[iSide][iPlate] = m_iFibSel[iSide][iPlate];
		}
	}

	ATH_MSG_DEBUG("end ALFA_ODTracking::GetData()");
}
