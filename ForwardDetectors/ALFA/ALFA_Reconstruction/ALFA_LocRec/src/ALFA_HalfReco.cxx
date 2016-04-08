/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_HalfReco.h"

ALFA_HalfReco::ALFA_HalfReco()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::ALFA_HalfReco()");
	ATH_MSG_DEBUG("begin ALFA_HalfReco::ALFA_HalfReco");

	m_fOvU = 0.0;
	m_fOvV = 0.0;
	m_fOverlapCut = 0.0;
	m_fRecXPos = -9999.0;
	m_fRecYPos = -9999.0;
	m_iHalf = 0;
	m_iMultiplicityCut = 0;

//	memset(&m_iNumHitsLayer, 0, sizeof(m_iNumHitsLayer));

	memset(&m_fhits, 0, sizeof(m_fhits));
	std::fill_n(m_fhits, sizeof(m_fhits)/sizeof(Int_t), -9999);

	m_iNumU = 0;
	m_iNumV = 0;
	m_iRPot = 0;
	m_iUVCut = 0;

	ATH_MSG_DEBUG("end ALFA_HalfReco::ALFA_HalfReco");
}

ALFA_HalfReco::~ALFA_HalfReco()
{
}

StatusCode ALFA_HalfReco::Initialize(Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iHalf, Int_t fMultiplicityCut, Int_t iUVCut, Float_t fOverlapCut)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::Initialize()");
	ATH_MSG_DEBUG("begin ALFA_HalfReco::Initialize()");

	m_iMultiplicityCut = fMultiplicityCut;
	m_iUVCut = iUVCut;
	m_fOverlapCut = fOverlapCut;
	m_iHalf = iHalf;

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

	ATH_MSG_DEBUG("end ALFA_HalfReco::Initialize()");

	return StatusCode::SUCCESS;
}

StatusCode ALFA_HalfReco::Execute(Int_t iRPot, const std::list<MDHIT> &ListMDHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::Execute()");
	ATH_MSG_DEBUG("ALFA_HalfReco::Execute()");

	FIBERS structFibers;
	Int_t iNumUFiberHits = 0;        //u_hits, v_hits (a condition for the UVCut 3)
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
//	for (Int_t i=half_ch*10;i<half_ch*10+10;i++)
	for (Int_t i=ALFAPLATESCNT*m_iHalf; i<m_iHalf*ALFAPLATESCNT+ALFAPLATESCNT; i++)
	{
//		if (m_iNumHitsLayer[i] <= m_iMultiplicityCut && m_iNumHitsLayer[i] > 0)
		if ((Int_t)m_MapLayers[i].ListFibers.size()<=m_iMultiplicityCut && (Int_t)m_MapLayers[i].ListFibers.size()>0)
		{
			if (fmod(double(i),double(2)) == 1) iNumUFiberHits++;
			else iNumVFiberHits++;
		}
	}


	// RECONSTRUCTION
	m_fRecXPos = -9999.0;
	m_fRecYPos = -9999.0;
	m_fOvU     = -9999.0;
	m_fOvV      = -9999.0;

	memset(&m_fhits, 0, sizeof(m_fhits));
	std::fill_n(m_fhits, sizeof(m_fhits)/sizeof(Int_t), -9999);

	if (iNumUFiberHits>=m_iUVCut && iNumVFiberHits>=m_iUVCut) OverLap();

	ATH_MSG_DEBUG("end ALFA_HalfReco::Execute()");
	return StatusCode::SUCCESS;
}

StatusCode ALFA_HalfReco::Finalize(Float_t &fRecXPos, Float_t &fRecYPos)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::Finalize()");
	ATH_MSG_DEBUG("begin ALFA_HalfReco::Finalize()");

	fRecXPos = m_fRecXPos;
	fRecYPos = m_fRecYPos;

	ATH_MSG_DEBUG("end ALFA_HalfReco::Execute()");
	return StatusCode::SUCCESS;
}

void ALFA_HalfReco::HistFill(Float_t &b_p, Float_t &b_n, Float_t &Ov_p, Float_t &Ov_n, Int_t &NumU, Int_t &NumV, Int_t iFlag)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::HistFill()");
	ATH_MSG_DEBUG("ALFA_HalfReco::HistFill()");

	Int_t iHit;
	Float_t fFibCen;
	Float_t b_ref[2] = {-127.0, -127.0};
	Float_t fXInter, fYInter;
	Float_t p_min, p_max;
	Float_t n_min, n_max;
	Int_t iMinP_tmp, iMaxP_tmp;
	Int_t iMinN_tmp, iMaxN_tmp;
	Int_t iFullWidth;
	const Int_t NBINTOT = 72000;
	Int_t Over_p[NBINTOT], Over_n[NBINTOT];

	if (iFlag==0) //First call, Event not reconstructed for U and V
	{
		b_ref[0]=-127.0;
		b_ref[1]=-127.0;
	}

	if (iFlag==1) //Event reconstructed for U but not for V so let's try to refine
	{
		b_ref[0]=b_p;
		b_ref[1]=-127.0;

	}

	if (iFlag==2) //Event reconstructed for V but not for U so let's try to refine
	{
		b_ref[0]=-127.0;
		b_ref[1]=b_n;
	}

	if (iFlag==3) //Event reconstructed for U and V, ultime refinement
	{
		b_ref[0]=b_p;
		b_ref[1]=b_n;
	}

	for (Int_t i=0;i<72000;i++)
	{
		Over_p[i]=0;
		Over_n[i]=0;
	}

	std::list<int>::iterator intIter;
//for(Int_t i=half_ch*10;i<half_ch*10+10;i++)
	for (Int_t iLayer = ALFAPLATESCNT*m_iHalf; iLayer < m_iHalf*ALFAPLATESCNT+ALFAPLATESCNT; iLayer++)
	{
		for (intIter=m_MapLayers[iLayer].ListFibers.begin(); intIter!=m_MapLayers[iLayer].ListFibers.end(); intIter++)
		{
			iHit = *intIter;

			if (m_faMD[m_iRPot][iLayer][iHit] >= 0)
			{
				fXInter = (b_ref[1]-m_fbMD[m_iRPot][iLayer][iHit])/(1+m_faMD[m_iRPot][iLayer][iHit]);
				fYInter = (m_faMD[m_iRPot][iLayer][iHit]*b_ref[1]+m_fbMD[m_iRPot][iLayer][iHit])/(1+m_faMD[m_iRPot][iLayer][iHit])-b_ref[1];
				fFibCen = (fYInter-fXInter)/sqrt(2.0);

				for (Int_t bin=0;bin<480;bin++)
				{
					Over_p[(int)((fFibCen-0.24)*1000)+bin+36000]++;
				}
			}
			else
			{
				fXInter = (m_fbMD[m_iRPot][iLayer][iHit]-b_ref[0])/(1-m_faMD[m_iRPot][iLayer][iHit]);
				fYInter = (m_fbMD[m_iRPot][iLayer][iHit]-m_faMD[m_iRPot][iLayer][iHit]*b_ref[0])/(1-m_faMD[m_iRPot][iLayer][iHit])-b_ref[0];
				fFibCen = (fYInter+fXInter)/sqrt(2.0);

				for (Int_t bin=0;bin<480;bin++)
				{
					Over_n[(int)((fFibCen-0.24)*1000)+bin+36000]++;
				}
			}
		}
	}

	//Determine maximum
	NumU=0;
	NumV=0;

	std::vector<int> *max_p = new std::vector<int>;
	std::vector<int> *max_n = new std::vector<int>;

	for (Int_t i=0;i<72000;i++)
	{
		if (Over_p[i]>NumU) NumU = Over_p[i];
		if (Over_n[i]>NumV) NumV = Over_n[i];
	}

	if (NumU >= m_iUVCut) // If more than m_iUVCut fibers in the maximum we do the reconstruction
	{
		for (Int_t i=0;i<72000;i++)
		{
			if (Over_p[i]==NumU)
			{
				max_p->push_back(i);
			}
		}

		p_min = -36.0 + double(max_p->front())*1e-3;
		p_max = -36.0 + double(max_p->back())*1e-3;
		iMinP_tmp = max_p->front();
		iMaxP_tmp = max_p->back();

//		Making sure that the plateau belongs to the same track
		iFullWidth = max_p->size();

		for (Int_t i=0; i<iFullWidth; i++)
		{
			if (max_p->at(iFullWidth-i-1)-iMinP_tmp < 500)
			{
				iMaxP_tmp = max_p->at(iFullWidth-i-1);
				p_max     = -36.0 + double(iMaxP_tmp)*1e-3;
				break;
			}
		}

		if ((p_max-p_min)<m_fOverlapCut) //Check for multiple tracks
		{
			b_p = b_ref[1]+(p_min+p_max)/sqrt(2.);
			Ov_p = p_max-p_min;
		}
		else
		{
			b_p = -9999.0;
			Ov_p = -9999.0;
		}
	}
	else
	{
		b_p = -9999.0;
		Ov_p = -9999.0;
	}

	if (NumV >= m_iUVCut) // If more than m_iUVCut fibers in the maximum we do the reconstruction
	{
		for (Int_t i=0;i<72000;i++)
		{
			if (Over_n[i]==NumV)
			{
				max_n->push_back(i);
			}
		}

		n_min = -36.0 + double(max_n->front())*1e-3;
		n_max = -36.0 + double(max_n->back())*1e-3;
		iMinN_tmp = max_n->front();
		iMaxN_tmp = max_n->back();

//		Making sure that the plateau belongs to the same track
		iFullWidth = max_n->size();

		for (Int_t i=0; i<iFullWidth; i++)
		{
			if (max_n->at(iFullWidth-i-1)-iMinN_tmp < 500)
			{
				iMaxN_tmp = max_n->at(iFullWidth-i-1);
				n_max     = -36.0 + double(iMaxN_tmp)*1e-3;
				break;
			}
		}

		if ((n_max-n_min)<m_fOverlapCut) //Check for multiple tracks
		{
			b_n = b_ref[0]+(n_min+n_max)/sqrt(2.);
			Ov_n = n_max-n_min;
		}
		else
		{
			b_n = -9999.0;
			Ov_n = -9999.0;
		}
	}
	else
	{
		b_n = -9999.0;
		Ov_n = -9999.0;
	}

	delete max_p;
	delete max_n;

	ATH_MSG_DEBUG("endl ALFA_HalfReco::HistFill()");
}

void ALFA_HalfReco::OverLap()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::OverLap()");
	ATH_MSG_DEBUG("ALFA_HalfReco::OverLap()");

	Float_t b_mean_n=-9999.0;
	Float_t b_mean_p=-9999.0;
	Float_t overReg_p;
	Float_t overReg_n;
	Int_t NumU, NumV;

	//First call, projection on middle fiber for U and V
	HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 0);

	//If first projection worked, we do the ultimate one
	if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0)
	{
		HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
		if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0) HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
	}
	else
	{
		//If first projection worked for p side but not for n, we try to refine n projection using the information from p side
		if (b_mean_p!=-9999.0 && b_mean_n==-9999.0)
		{
			HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 1);

			if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0)
			{
				//if it as worked for both we do the ultimate one
				HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
				if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0) HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
			}
		}

		//If first projection worked for p side but not for n, we try to refine n projection using the information from p side
		if (b_mean_p==-9999.0 && b_mean_n!=-9999.0)
		{
			HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 2);

			if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0)
			{
				//if it as worked for both we do the ultimate one
				HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
				if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0) HistFill(b_mean_p, b_mean_n, overReg_p, overReg_n, NumU, NumV, 3);
			}
		}
	}

	Float_t x_tmp, y_tmp;
	Float_t min_x,min_y;
	Float_t dist_x,dist_y;
//	Float_t dist_min_x,dist_min_y;
	Int_t gFib_x, gFib_y;
	Int_t iHit;

	Float_t OL_U, OL_V;

	std::list<int>::iterator intIter;
	if (b_mean_p!=-9999.0 && b_mean_n!=-9999.0)
	{
		m_fRecXPos = (b_mean_n-b_mean_p)/2.0;
		m_fRecYPos = (b_mean_n+b_mean_p)/2.0;
		OL_U = overReg_p;
		OL_V = overReg_n;

		//for (Int_t i=half_ch*10;i<half_ch*10+10;i++)
		for (Int_t iLayer = ALFAPLATESCNT*m_iHalf; iLayer<m_iHalf*ALFAPLATESCNT+ALFAPLATESCNT; iLayer++)
		{
			min_x = 0.24/cos(TMath::Pi()/4.);
			min_y = 0.24/cos(TMath::Pi()/4.);

			gFib_x = 9999;
			gFib_y = 9999;

			for (intIter=m_MapLayers[iLayer].ListFibers.begin(); intIter!=m_MapLayers[iLayer].ListFibers.end(); intIter++)
			{
				iHit = *intIter;
				x_tmp = (m_fRecYPos-m_fbMD[m_iRPot][iLayer][iHit])/m_faMD[m_iRPot][iLayer][iHit];
				y_tmp = m_faMD[m_iRPot][iLayer][iHit]*m_fRecXPos+m_fbMD[m_iRPot][iLayer][iHit];
				dist_x = TMath::Abs(m_fRecXPos-x_tmp);
				dist_y = TMath::Abs(m_fRecYPos-y_tmp);

				if (dist_x <= min_x)
				{
//					dist_min_x = m_fRecXPos-x_tmp;
					min_x = dist_x;
					gFib_x = iHit;
				}

				if (dist_y <= min_y)
				{
//					dist_min_y = m_fRecYPos-y_tmp;
					min_y = dist_y;
					gFib_y = iHit;
				}
			}

			if (gFib_x==gFib_y) m_fhits[iLayer]=gFib_x;
		}
	}
	else
	{
		m_fRecXPos = -9999.0;
		m_fRecYPos = -9999.0;
		NumU = 0;
		NumV = 0;
		OL_U = -9999.0;
		OL_V = -9999.0;
	}

	SetData(NumU, NumV, OL_U, OL_V);

	ATH_MSG_DEBUG("end ALFA_HalfReco::OverLap()");
}

void ALFA_HalfReco::SetData(Int_t iNumU, Int_t iNumV, Float_t fOvU, Float_t fOvV)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::SetData()");
	ATH_MSG_DEBUG("begin ALFA_HalfReco::SetData()");

	m_iNumU = iNumU;
	m_iNumV = iNumV;
	m_fOvU = fOvU;
	m_fOvV = fOvV;

	ATH_MSG_DEBUG("end ALFA_HalfReco::SetData()");
}

void ALFA_HalfReco::GetData(Int_t &iNumU, Int_t &iNumV, Float_t &fOvU, Float_t &fOvV, Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_HalfReco::GetData()");
	ATH_MSG_DEBUG("begin ALFA_HalfReco::GetData()");

	iNumU = m_iNumU;
	iNumV = m_iNumV;
	fOvU = m_fOvU;
	fOvV = m_fOvV;

	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		iFibSel[iLayer] = m_fhits[iLayer];
	}

	ATH_MSG_DEBUG("end ALFA_HalfReco::GetData()");
}
