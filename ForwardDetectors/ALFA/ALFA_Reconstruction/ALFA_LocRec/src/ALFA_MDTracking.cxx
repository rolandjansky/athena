/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_MDTracking.h"

ALFA_MDTracking::ALFA_MDTracking()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::ALFA_MDTracking()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::ALFA_MDTracking");

	m_iNumU = 0;
	m_iNumV = 0;
	m_fOverlapU = -9999.0;
	m_fOverlapV = -9999.0;


	m_iMultiplicityCut = 0;
	m_iUVCut = 0;
	m_fOverlapCut = 0.0;
	m_fRecXPos = 0.0;
	m_fRecYPos = 0.0;
	m_iRPot = 0;

	memset(&m_iFibSel, 0, sizeof(m_iFibSel));
	std::fill_n(m_iFibSel, sizeof(m_iFibSel)/sizeof(Int_t), -9999);

	ATH_MSG_DEBUG("end ALFA_MDTracking::ALFA_MDTracking");
}

ALFA_MDTracking::~ALFA_MDTracking()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::~ALFA_MDTracking()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::~ALFA_MDTracking");
	ATH_MSG_DEBUG("end ALFA_MDTracking::~ALFA_MDTracking");
}

StatusCode ALFA_MDTracking::Initialize(Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iMultiplicityCut, Int_t iUVCut, Float_t fOverlapCut)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::Initialize()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::Initialize()");

	m_iMultiplicityCut = iMultiplicityCut;
	m_iUVCut = iUVCut;
	m_fOverlapCut = fOverlapCut;

	for (Int_t iRPot = 0; iRPot < RPOTSCNT; iRPot++)
	{
		for (Int_t iLayer = 0; iLayer < ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			for (Int_t iFiber = 0; iFiber < ALFAFIBERSCNT; iFiber++)
			{
				m_faMD[iRPot][iLayer][iFiber] = faMD[iRPot][iLayer][iFiber];
				m_fbMD[iRPot][iLayer][iFiber] = fbMD[iRPot][iLayer][iFiber];
			}
		}
	}

	ATH_MSG_DEBUG("end ALFA_MDTracking::Initialize()");

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDTracking::Execute(Int_t iRPot, const std::list<MDHIT> &ListMDHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::Execute()");
	ATH_MSG_DEBUG("ALFA_MDTracking::Execute()");

	FIBERS structFibers;
	Int_t iNumUFiberHits = 0;
	Int_t iNumVFiberHits = 0;

	m_iRPot = iRPot;
	m_MapLayers.clear();

	std::list<MDHIT>::const_iterator iter;
	for (iter=ListMDHits.begin(); iter!=ListMDHits.end(); iter++)
	{
		if (iRPot == (*iter).iRPot)
		{
			if(m_MapLayers.find((*iter).iPlate)==m_MapLayers.end())
			{
				structFibers.ListFibers.clear();
				m_MapLayers.insert(std::pair<int, FIBERS>((*iter).iPlate, structFibers));
				m_MapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
			}
			else
			{
				m_MapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
			}
		}
	}

	//Checking that the multiplicity cut conditions are satisfied
	//At least more than UV_cut layers have a multiplicity lower than multi_cut
	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		if ((Int_t)m_MapLayers[iLayer].ListFibers.size()<=m_iMultiplicityCut && (Int_t)m_MapLayers[iLayer].ListFibers.size()>0)
		{
			if (fmod(double(iLayer),double(2)) == 1) iNumUFiberHits++;
			else iNumVFiberHits++;
		}
	}

	m_fRecXPos = -9999.0;
	m_fRecYPos = -9999.0;
	m_fOverlapU = -9999.0;
	m_fOverlapV = -9999.0;

	memset(&m_iFibSel, 0, sizeof(m_iFibSel));
	std::fill_n(m_iFibSel, sizeof(m_iFibSel)/sizeof(Int_t), -9999);

	if (iNumUFiberHits>=m_iUVCut && iNumVFiberHits>=m_iUVCut) OverLap();

	ATH_MSG_DEBUG("end ALFA_MDTracking::Execute()");
	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDTracking::Finalize(Float_t &fRecXPos, Float_t &fRecYPos)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::Finalize()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::Finalize()");

	fRecXPos = m_fRecXPos;
	fRecYPos = m_fRecYPos;

	ATH_MSG_DEBUG("end ALFA_MDTracking::Execute()");

	return StatusCode::SUCCESS;
}

void ALFA_MDTracking::HistFill(Float_t &b_p, Float_t &b_n, Float_t &fOverP, Float_t &fOverN, Int_t &iNumU, Int_t &iNumV, Int_t iFlag)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::HistFill()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::HistFill()");

	Int_t iHit;
	Float_t fFibCen;
	Float_t fbRef[2] = {-127.0, -127.0};
	Float_t fXInter, fYInter;
	Float_t fMinP, fMaxP;
	Float_t fMinN, fMaxN;
	Int_t iMinP_tmp, iMaxP_tmp;
	Int_t iMinN_tmp, iMaxN_tmp;
	Int_t iFullWidth;
	const Int_t NBINTOT = 72000;
	Int_t Over_p[NBINTOT], Over_n[NBINTOT];

	if (iFlag==0) //First call, Event not reconstructed for U and V
	{
		fbRef[0]=-127.0;
		fbRef[1]=-127.0;
	}

	if (iFlag==1) //Event reconstructed for U but not for V so let's try to refine
	{
		fbRef[0]=b_p;
		fbRef[1]=-127.0;
	}

	if (iFlag==2) //Event reconstructed for V but not for U so let's try to refine
	{
		fbRef[0]=-127.0;
		fbRef[1]=b_n;
	}

	if (iFlag==3) //Event reconstructed for U and V, ultime refinement
	{
		fbRef[0]=b_p;
		fbRef[1]=b_n;
	}

	for (Int_t iBin=0;iBin<72000;iBin++)
	{
		Over_p[iBin]=0;
		Over_n[iBin]=0;
	}

	std::list<int>::iterator iter;
	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		for (iter=m_MapLayers[iLayer].ListFibers.begin(); iter!=m_MapLayers[iLayer].ListFibers.end(); iter++)
		{
			iHit = *iter;

			if (m_faMD[m_iRPot][iLayer][iHit]>=0)
			{
				fXInter = (fbRef[1]-m_fbMD[m_iRPot][iLayer][iHit])/(1+m_faMD[m_iRPot][iLayer][iHit]);
				fYInter = (m_faMD[m_iRPot][iLayer][iHit]*fbRef[1]+m_fbMD[m_iRPot][iLayer][iHit])/(1+m_faMD[m_iRPot][iLayer][iHit])-fbRef[1];
				fFibCen = (fYInter-fXInter)/sqrt(2.0);

				for (Int_t bin=0;bin<480;bin++)
				{
					Over_p[(int)((fFibCen-0.24)*1000)+bin+36000]++;
				}
			}
			else
			{
				fXInter = (m_fbMD[m_iRPot][iLayer][iHit]-fbRef[0])/(1-m_faMD[m_iRPot][iLayer][iHit]);
				fYInter = (m_fbMD[m_iRPot][iLayer][iHit]-m_faMD[m_iRPot][iLayer][iHit]*fbRef[0])/(1-m_faMD[m_iRPot][iLayer][iHit])-fbRef[0];
				fFibCen = (fYInter+fXInter)/sqrt(2.0);

				for (Int_t bin=0;bin<480;bin++)
				{
					Over_n[(int)((fFibCen-0.24)*1000)+bin+36000]++;
				}
			}
		}
	}

	//Determine maximum
	iNumU=0;
	iNumV=0;

	std::vector<int> *vecMaxP = new std::vector<int>;
	std::vector<int> *vecMaxN = new std::vector<int>;

	for (Int_t i=0;i<72000;i++)
	{
		if (Over_p[i]>iNumU) iNumU = Over_p[i];
		if (Over_n[i]>iNumV) iNumV = Over_n[i];
	}

	if (iNumU>=m_iUVCut) // If more than m_iUVCut fibers in the maximum we do the reconstruction
	{
		for (Int_t i=0; i<NBINTOT; i++)
		{
			if (Over_p[i]==iNumU)
			{
				vecMaxP->push_back(i);
			}
		}

		fMinP     = -36.0 + double(vecMaxP->front())*1e-3;
		fMaxP     = -36.0 + double(vecMaxP->back())*1e-3;
		iMinP_tmp = vecMaxP->front();
		iMaxP_tmp = vecMaxP->back();

//		Making sure that the plateau belongs to the same track
		iFullWidth = vecMaxP->size();

		for (Int_t i=0; i<iFullWidth; i++)
		{
			if (vecMaxP->at(iFullWidth-i-1)-iMinP_tmp < 500)
			{
				iMaxP_tmp = vecMaxP->at(iFullWidth-i-1);
				fMaxP     = -36.0 + double(iMaxP_tmp)*1e-3;
				break;
			}
		}

		if ((fMaxP-fMinP)<m_fOverlapCut) //Check for multiple tracks
		{
			b_p = fbRef[1]+(fMinP+fMaxP)/sqrt(2.);
			fOverP = fMaxP-fMinP;
		}
		else
		{
			b_p = -9999.0;
			fOverP = -9999.0;
		}
	}
	else
	{
		b_p = -9999.0;
		fOverP = -9999.0;
	}

	if (iNumV>=m_iUVCut) // If more than m_iUVCut fibers in the maximum we do the reconstruction
	{
		for (Int_t i=0; i<NBINTOT; i++)
		{
			if (Over_n[i]==iNumV)
			{
				vecMaxN->push_back(i);
			}
		}

		fMinN     = -36.0 + double(vecMaxN->front())*1e-3;
		fMaxN     = -36.0 + double(vecMaxN->back())*1e-3;
		iMinN_tmp = vecMaxN->front();
		iMaxN_tmp = vecMaxN->back();

//		Making sure that the plateau belongs to the same track
		iFullWidth = vecMaxN->size();

		for (Int_t i=0; i<iFullWidth; i++)
		{
			if (vecMaxN->at(iFullWidth-i-1)-iMinN_tmp < 500)
			{
				iMaxN_tmp = vecMaxN->at(iFullWidth-i-1);
				fMaxN     = -36.0 + double(iMaxN_tmp)*1e-3;
				break;
			}
		}

		if ((fMaxN-fMinN)<m_fOverlapCut) //Check for multiple tracks
		{
			b_n = fbRef[0]+(fMinN+fMaxN)/sqrt(2.);
			fOverN = fMaxN-fMinN;
		}
		else
		{
			b_n = -9999.0;
			fOverN = -9999.0;
		}
	}
	else
	{
		b_n = -9999.0;
		fOverN = -9999.0;
	}

//	if (b_p!=-9999.0 && b_n!=-9999.0)
//	{
//		if ((b_n+b_p)/2.0<=-135.5 && (iNumU>=m_iUVCut && iNumV>=m_iUVCut))
//		{
//			b_n = -9999.0;
//			b_p = -9999.0;
//		}
//	}

	delete vecMaxP;
	delete vecMaxN;

	ATH_MSG_DEBUG("endl ALFA_MDTracking::HistFill()");
}

void ALFA_MDTracking::OverLap()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::OverLap()");
	ATH_MSG_DEBUG("ALFA_MDTracking::OverLap()");

	Float_t fBMeanN = -9999.0;
	Float_t fBMeanP = -9999.0;
	Float_t fOverRegP;
	Float_t fOverRegN;
	Int_t iNumU, iNumV;

	//First call, projection on middle fiber for U and V
	HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 0);

	//If first projection worked, we do the ultimate one
	if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0)
	{
		HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
		if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0)
		{
			HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
		}
	}
	else
	{
		//If first projection worked for p side but not for n, we try to refine n projection using the information from p side
		if (fBMeanP!=-9999.0 && fBMeanN==-9999.0)
		{
			HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 1);

			if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0)
			{
				//if it as worked for both we do the ultimate one
				HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
				if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0) HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
			}
		}

		//If first projection worked for p side but not for n, we try to refine n projection using the information from p side
		if (fBMeanP==-9999.0 && fBMeanN!=-9999.0)
		{
			HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 2);

			if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0)
			{
				//if it as worked for both we do the ultimate one
				HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
				if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0) HistFill(fBMeanP, fBMeanN, fOverRegP, fOverRegN, iNumU, iNumV, 3);
			}
		}
	}

	Float_t fxTmp, fyTmp;
	Float_t fMinX,fMinY;
	Float_t fDistX,fDistY;
//	Float_t fDistMinX,fDistMinY;
	Int_t iHit;
	Float_t fOverlapU, fOverlapV;
	Int_t iFibSelX, iFibSelY;

	std::list<int>::iterator iter;
	if (fBMeanP!=-9999.0 && fBMeanN!=-9999.0)
	{
		m_fRecXPos = (fBMeanN-fBMeanP)/2.0;
		m_fRecYPos = (fBMeanN+fBMeanP)/2.0;
		fOverlapU = fOverRegP;
		fOverlapV = fOverRegN;

		for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			fMinX = 0.24/cos(TMath::Pi()/4.0);
			fMinY = 0.24/cos(TMath::Pi()/4.0);

			iFibSelX = -9999, iFibSelY = -9999;
			for (iter=m_MapLayers[iLayer].ListFibers.begin(); iter!=m_MapLayers[iLayer].ListFibers.end(); iter++)
			{
				iHit = *iter;
				fxTmp = (m_fRecYPos-m_fbMD[m_iRPot][iLayer][iHit])/m_faMD[m_iRPot][iLayer][iHit];
				fyTmp = m_faMD[m_iRPot][iLayer][iHit]*m_fRecXPos+m_fbMD[m_iRPot][iLayer][iHit];
				fDistX = TMath::Abs(m_fRecXPos-fxTmp);
				fDistY = TMath::Abs(m_fRecYPos-fyTmp);

				if (fDistX<=fMinX)
				{
//					fDistMinX = m_fRecXPos-fxTmp;
					fMinX = fDistX;
					iFibSelX = iHit;
				}

				if (fDistY<=fMinY)
				{
//					fDistMinY = m_fRecYPos-fyTmp;
					fMinY = fDistY;
					iFibSelY = iHit;
				}
			}

			if (iFibSelX==iFibSelY) m_iFibSel[iLayer] = iFibSelX;
		}
	}
	else
	{
		m_fRecXPos = -9999.0;
		m_fRecYPos = -9999.0;
		iNumU = 0;
		iNumV = 0;
		fOverlapU = -9999.0;
		fOverlapV = -9999.0;
	}

	SetData(iNumU, iNumV, fOverlapU, fOverlapV);

	ATH_MSG_DEBUG("end ALFA_MDTracking::OverLap()");
}

void ALFA_MDTracking::SetData(Int_t iNumU, Int_t iNumV, Float_t fOverlapU, Float_t fOverlapV)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::SetData()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::SetData()");

	m_iNumU = iNumU;
	m_iNumV = iNumV;
	m_fOverlapU = fOverlapU;
	m_fOverlapV = fOverlapV;

	ATH_MSG_DEBUG("end ALFA_MDTracking::SetData()");
}

void ALFA_MDTracking::GetData(Int_t &iNumU, Int_t &iNumV, Float_t &fOverlapU, Float_t &fOverlapV, Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::GetData()");
	ATH_MSG_DEBUG("begin ALFA_MDTracking::GetData()");

	iNumU = m_iNumU;
	iNumV = m_iNumV;
	fOverlapU = m_fOverlapU;
	fOverlapV = m_fOverlapV;

	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		iFibSel[iLayer] = m_iFibSel[iLayer];
	}

	ATH_MSG_DEBUG("end ALFA_MDTracking::GetData()");
}
