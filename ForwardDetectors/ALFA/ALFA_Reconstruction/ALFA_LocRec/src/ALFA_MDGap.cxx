/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_MDGap.h"

ALFA_MDGap::ALFA_MDGap()
{
	memset(&m_faMD, 0, sizeof(m_faMD));
	memset(&m_fbMD, 0, sizeof(m_fbMD));
	memset(&m_fRecXPos, 0, sizeof(m_fRecXPos));
	memset(&m_fRecYPos, 0, sizeof(m_fRecYPos));
	memset(&m_fOvU, 0, sizeof(m_fOvU));
	memset(&m_fOvV, 0, sizeof(m_fOvV));
	memset(&m_iNU, 0, sizeof(m_iNU));
	memset(&m_iNV, 0, sizeof(m_iNV));
	memset(&m_iFibSel, 0, sizeof(m_iFibSel));
	memset(&m_iNumHitsLayer, 0, sizeof(m_iNumHitsLayer));

	m_fOverlapCut   = 0.0;
	m_iRPot         = 0;
	m_iUVCut        = 0;
}

ALFA_MDGap::~ALFA_MDGap()
{
}

StatusCode ALFA_MDGap::Initialize(Int_t iRPot, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iUVCut, Float_t fOverlapCut)
{
	m_iRPot = iRPot;
	m_iUVCut = iUVCut;
	m_fOverlapCut = fOverlapCut;

	for (Int_t iPot=0; iPot<RPOTSCNT; iPot++)
	{
		for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			for (Int_t iFiber=0; iFiber<ALFAFIBERSCNT; iFiber++)
			{
				m_faMD[iPot][iLayer][iFiber] = faMD[iPot][iLayer][iFiber];
				m_fbMD[iPot][iLayer][iFiber] = fbMD[iPot][iLayer][iFiber];
			}
		}
	}

	memset(&m_fRecXPos, 0, sizeof(m_fRecXPos));
	memset(&m_fRecYPos, 0, sizeof(m_fRecYPos));
	memset(&m_fOvU, 0, sizeof(m_fOvU));
	memset(&m_fOvV, 0, sizeof(m_fOvV));
	memset(&m_iNU, 0, sizeof(m_iNU));
	memset(&m_iNV, 0, sizeof(m_iNV));
	memset(&m_iFibSel, 0, sizeof(m_iFibSel));
	std::fill_n(&m_fRecXPos[0], sizeof(m_fRecXPos)/sizeof(Float_t), -9999.0);
	std::fill_n(&m_fRecYPos[0], sizeof(m_fRecYPos)/sizeof(Float_t), -9999.0);
	std::fill_n(&m_fOvU[0], sizeof(m_fOvU)/sizeof(Float_t), -9999.0);
	std::fill_n(&m_fOvV[0], sizeof(m_fOvV)/sizeof(Float_t), -9999.0);
	std::fill_n(&m_iFibSel[0], sizeof(m_iFibSel)/sizeof(Int_t), 9999);

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDGap::Execute(const std::list<MDHIT> &ListMDHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDGap::Execute()");
	ATH_MSG_DEBUG("ALFA_MDGap::Execute()");

	FIBERS structFibers;
	m_MapLayers.clear();

	std::list<MDHIT>::const_iterator iter;
	for (iter=ListMDHits.begin(); iter!=ListMDHits.end(); iter++)
	{
		if (m_iRPot == (*iter).iRPot)
		{
			if(m_MapLayers.find((*iter).iPlate)==m_MapLayers.end())
			{
				structFibers.ListFibers.clear();
				m_MapLayers.insert(std::pair<int, FIBERS>((*iter).iPlate, structFibers));
				m_MapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
			}
			else m_MapLayers[(*iter).iPlate].ListFibers.push_back((*iter).iFiber);
		}
	}

	// RECONSTRUCTION
	std::vector<Float_t> b_p, b_n;
	std::vector<Float_t> Ov_p, Ov_n;
	std::vector<int> Num_p, Num_n;
	Int_t FSel_n[ALFAPLATESCNT], FSel_p[ALFAPLATESCNT];
	Bool_t FGaps_n[ALFAPLATESCNT], FGaps_p[ALFAPLATESCNT];
	Int_t Gap_Fib_p[ALFAPLATESCNT][2], Gap_Fib_n[ALFAPLATESCNT][2];
	std::vector<Int_t> GapsID;

	Int_t Gaps[2][2];
//	Int_t Gaps_ID[MAXTRACKNUM];
//	Bool_t FGaps[20];
//	Int_t GapFib[20][2];

	Reco_Track(b_p, b_n, Ov_p, Ov_n, Num_p, Num_n, FSel_n, FSel_p, Gaps, GapsID, FGaps_p, FGaps_n, Gap_Fib_p, Gap_Fib_n);

	Int_t cnt_tr=0;
	for (UInt_t i=0;i< b_p.size();i++)
	{
		m_fRecXPos[cnt_tr]=(b_p[i]-b_n[i])/2.;
		m_fRecYPos[cnt_tr]=(b_n[i]+b_p[i])/2.;

		m_fOvU[cnt_tr]= Ov_p[i];
		m_fOvV[cnt_tr]= Ov_n[i];

		m_iNU[cnt_tr]= Num_p[i];
		m_iNV[cnt_tr]= Num_n[i];

//		Gaps_ID[cnt_tr]=GapsID[i];

		if(cnt_tr==0)
		{
			for (Int_t j=0; j<ALFAPLATESCNT; j++)
			{
				m_iFibSel[j*2] = FSel_p[j];
				m_iFibSel[j*2+1] = FSel_n[j];
			}
		}

//		if (GapsID[i]==1)
//		{
//			for (Int_t j=0; j<ALFAPLATESCNT; j++)
//			{
//				FGaps[j*2]=FGaps_p[j];
//				FGaps[j*2+1]=FGaps_n[j];

//				GapFib[j*2][0]=Gap_Fib_p[j][0];
//				GapFib[j*2][1]=Gap_Fib_p[j][1];
//				GapFib[j*2+1][0]=Gap_Fib_n[j][0];
//				GapFib[j*2+1][1]=Gap_Fib_n[j][1];
//			}
//		}
		cnt_tr++;
	}

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDGap::Finalize(Float_t (&fRecXPos)[MAXTRACKNUM], Float_t (&fRecYPos)[MAXTRACKNUM])
{
		for (Int_t i=0; i<MAXTRACKNUM; i++)
		{
			fRecXPos[i] = m_fRecXPos[i];
			fRecYPos[i] = m_fRecYPos[i];
		}

	return StatusCode::SUCCESS;
}

/************************************************/
/*  Making projection and storing in an array   */
/************************************************/
void ALFA_MDGap::Proj_Store(Int_t iFiberSide, Int_t (&iOver)[72000], Float_t fbRef, Int_t iSideFlag)
{
	Float_t fSign;
	Float_t fXInter, fYInter;
	Float_t fFibCen;

	if (m_faMD[m_iRPot][iSideFlag][0]>0) fSign=1.0;
	else fSign=-1.0;

	for (Int_t iBin=0; iBin<72000; iBin++)
	{
		iOver[iBin]=0;
	}

	std::list<int>::iterator intIter;
	int iIndex = 0;
	for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		iIndex = 2*iLayer+iFiberSide;
		for (intIter=m_MapLayers[iIndex].ListFibers.begin(); intIter!=m_MapLayers[iIndex].ListFibers.end(); intIter++)
		{
			if (*intIter!=9999)
			{
				//Depending on layer orientation, computing the projection of the hit fiber
				fXInter= fSign*(fbRef-m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]);
				fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]*fbRef+m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter])-fbRef;
				fFibCen = (fYInter-fSign*fXInter)/sqrt(2.0);

				//Filling the table with the hit fiber
				for (Int_t iBin=0; iBin<480; iBin++)
				{
					iOver[(int)((fFibCen-0.24)*1000)+iBin+36000]++;
				}
			}
		}
	}
}

/************************************************/
/*   Identifying plateau in projection array	*/
/************************************************/
void ALFA_MDGap::Find_Proj(Int_t iOver[72000], Float_t fbRef, Float_t &fb, Float_t &fOv, Int_t&iNum)
{
	std::vector<int> iSizePlateau;
	Int_t iNumFib=0;
	Int_t p_tmp_min;
//	Int_t p_tmp_max;
	Float_t p_min;
	Float_t p_max;

	//Determine the maximum number of overlapping fibers in both directions
	for (Int_t i=0;i<72000;i++)
	{
		if (iOver[i]>iNumFib) iNumFib=iOver[i];
	}

	//	Filling array for all values equal to the maximum
	if (iNumFib>=m_iUVCut)
	{
		for (Int_t i=0;i<72000;i++)
		{
			if (iOver[i]==iNumFib)
			{
				iSizePlateau.push_back(i);
			}
		}

//		Finding first and last position where the maximum is found
		p_min = -36.0 + Float_t(iSizePlateau.front())*1e-3;
		p_tmp_min = iSizePlateau.front();

		p_max = -36.0 + Float_t(iSizePlateau.back())*1e-3;
//		p_tmp_max = iSizePlateau.back();

//		Making sure that the plateau belongs to the same track
		Int_t full_width = iSizePlateau.size();

		for (Int_t i=0; i<full_width; i++)
		{
			if (iSizePlateau[full_width-i-1]-p_tmp_min < 500)
			{
//				p_tmp_max = iSizePlateau[full_width-i-1];
				p_max = -36.0 + Float_t(iSizePlateau[full_width-i-1])*1e-3;
				break;
			}
		}

		if ((p_max-p_min)<m_fOverlapCut)
		{
//			Storing the coordinate of the maximum
			fb = fbRef+(p_min+p_max)/sqrt(2.0);
			fOv = p_max-p_min;
//			iNum = iNumFib;
		}
	}

	iNum = iNumFib;
}

/************************************************/
/************************************************/
/*			Identifying fibers on the track		*/
/************************************************/
/************************************************/

void ALFA_MDGap::Finding_Fib(Int_t iFiberSide, Float_t fbRef, Float_t fbRec, Int_t (&iFSel)[10], Int_t iSideFlag)
{
	Float_t b_pos, b_neg;
	Float_t x, y, x_tmp, y_tmp;
	Float_t min_dist;
	Float_t dist_x, dist_y;
	Float_t dist_full;
	Float_t fib_dist;
	Int_t gFib;

	if (iSideFlag==0)
	{
		b_neg= fbRef;
		b_pos= fbRec;
	}
	else
	{
		b_neg= fbRec;
		b_pos= fbRef;
	}

	x= (b_pos-b_neg)/2.0;
	y= (b_neg+b_pos)/2.0;

	for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++) iFSel[iLayer] = 9999;

	//For each layer, we determine the hit fiber which is closest to the track
	std::list<int>::iterator intIter;
	int iIndex = 0;
	for (Int_t iLayer = 0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		min_dist=0.24;
		gFib = 9999;

		iIndex = 2*iLayer+iFiberSide;
		for (intIter=m_MapLayers[iIndex].ListFibers.begin(); intIter!=m_MapLayers[iIndex].ListFibers.end(); intIter++)
		{
			if (*intIter!=9999)
			{
				x_tmp = (y-m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter];
				y_tmp = m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]*x+m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter];

				dist_x = TMath::Abs(x-x_tmp);
				dist_y = TMath::Abs(y-y_tmp);

				dist_full = sqrt(dist_x*dist_x+dist_y*dist_y);
				fib_dist = sqrt(TMath::Power((dist_x+dist_y)/2.0,2)-TMath::Power(dist_full/2.0,2));

				if (fib_dist <= min_dist)
				{
					min_dist = fib_dist;
					gFib = *intIter;

					iFSel[iLayer] = gFib;
				}
			}
		}
	}
}

/************************************************/
/*				Finding all tracks				*/
/************************************************/

void ALFA_MDGap::Reco_Track(std::vector<Float_t> &b_p, std::vector<Float_t> &b_n,
								std::vector<Float_t> &Ov_p, std::vector<Float_t> &Ov_n,
								std::vector<int> &Num_p, std::vector<int> &Num_n,
								Int_t (&FSel_n)[ALFAPLATESCNT], Int_t (&FSel_p)[ALFAPLATESCNT],
								Int_t (&Gaps)[2][2], std::vector<Int_t> &GapsID, Bool_t (&FGaps_p)[ALFAPLATESCNT], Bool_t (&FGaps_n)[ALFAPLATESCNT],
								Int_t (&Gap_Fib_p)[ALFAPLATESCNT][2], Int_t (&Gap_Fib_n)[ALFAPLATESCNT][2])
{
	Int_t Over_p[72000];
	Int_t Over_n[72000];
	Int_t Over_copy[72000];
	Int_t NumU=5;
	Int_t NumV=5;
	Float_t b_ref_p = -127.0;
	Float_t b_ref_n = -127.0;
	Float_t OvU;
	Float_t OvV;
	Float_t b_pos;
	Float_t b_neg;
	Bool_t FG_p[ALFAPLATESCNT];	//asi k nicemu - vstup do silentgap
	Bool_t FG_n[ALFAPLATESCNT];	//asi k nicemu - vstup do silentgap
	Int_t G_F_p[ALFAPLATESCNT][2];	//asi k nicemu - vstup do silentgap
	Int_t G_F_n[ALFAPLATESCNT][2];	//asi k nicemu - vstup do silentgap
	Float_t b_pos_silent;
	Float_t b_neg_silent;
	Float_t b_pos_active;
	Float_t b_neg_active;

	//clear
	memset(&FSel_n, 0, sizeof(Int_t));
	memset(&FSel_p, 0, sizeof(Int_t));
	memset(&FGaps_n, false, sizeof(Bool_t));
	memset(&FGaps_p, false, sizeof(Bool_t));
	memset(&Gap_Fib_n, 0, sizeof(Int_t));
	memset(&Gap_Fib_p, 0, sizeof(Int_t));
	std::fill_n(&Gap_Fib_n[0][0], sizeof(Gap_Fib_n)/sizeof(Int_t), 9999);
	std::fill_n(&Gap_Fib_p[0][0], sizeof(Gap_Fib_p)/sizeof(Int_t), 9999);

	b_n.clear();
	b_p.clear();
	Ov_n.clear();
	Ov_p.clear();
	Num_n.clear();
	Num_p.clear();

	b_ref_n=-127.0;
	b_ref_p=-127.0;

//	First projection step on U side
//	-------------------------------
//	filling the array for U side with reference value
	Proj_Store(0, Over_p, b_ref_n, 0);

//	Find first maxium
	Find_Proj(Over_p, b_ref_n, b_pos, OvU, NumU);

//	Then reconstruction all tracks possible using the second side
	if (NumU>=m_iUVCut)
	{
		//First projection on V side
		//-------------------------------
		//filling the array for V side with reference value
		Proj_Store(1, Over_n, b_ref_p, 1);
		Find_Proj(Over_n, b_ref_p, b_neg, OvV, NumV);

		if (NumV>=m_iUVCut)
		{
			//Now make the second projection step
			//-----------------------------------
			//U side
			Proj_Store(0, Over_p, b_neg, 0);
			Find_Proj(Over_p, b_neg, b_pos, OvU, NumU);

			//V side
			Proj_Store(1, Over_n, b_pos, 1);
			Find_Proj(Over_n, b_pos, b_neg, OvV, NumV);

			//Third projection steps
			//----------------------
			//U side
			Proj_Store(0, Over_p, b_neg, 0);
			Find_Proj(Over_p, b_neg, b_pos, OvU, NumU);

			//V side
			Proj_Store(1, Over_n, b_pos, 1);
			Find_Proj(Over_n, b_pos, b_neg, OvV, NumV);

			//We store the information in the vector
			b_p.push_back(b_pos);
			Ov_p.push_back(OvU);
			Num_p.push_back(NumU);

			b_n.push_back(b_neg);
			Ov_n.push_back(OvV);
			Num_n.push_back(NumV);

			GapsID.push_back(0);

			//Once done we want to remove the hit belonging to the first track on side V
			//We first find the corresponding fibers
			Finding_Fib(0,b_neg, b_pos, FSel_p, 0);
			Finding_Fib(1,b_pos, b_neg, FSel_n, 1);

			//Then looking for silent gaps in the plateau
			//Filling the array with the gap information

			for (Int_t bin=0;bin<72000;bin++)
			{
				Over_copy[bin]=Over_p[bin];
			}
			Gaps[0][0] = Silent_Gap(Over_copy, b_neg, b_pos, FSel_p, FGaps_p, Gap_Fib_p, OvU, (Int_t)0);
			Find_Proj(Over_copy, b_neg, b_pos_silent, OvU, NumU);

			for (Int_t bin=0;bin<72000;bin++)
			{
				Over_copy[bin]=Over_n[bin];
			}
			Gaps[1][0] = Silent_Gap(Over_copy, b_pos, b_neg, FSel_n, FGaps_n, Gap_Fib_n, OvV, 1);
			Find_Proj(Over_copy, b_pos, b_neg_silent, OvV, NumV);

			if (Gaps[0][0]>0 || Gaps[1][0]>0)
			{
				b_p.push_back(b_pos_silent);
				Ov_p.push_back(OvU);
				Num_p.push_back(NumU);

				b_n.push_back(b_neg_silent);
				Ov_n.push_back(OvV);
				Num_n.push_back(NumV);

				GapsID.push_back(1);
			}

			//then looking for active gaps in the plateau
			for (Int_t bin=0;bin<72000;bin++)
			{
				Over_copy[bin]=Over_p[bin];
			}
			Gaps[0][1] = Active_Gap(0,Over_copy, b_neg, b_pos, FSel_p, OvU, 0);
			Find_Proj(Over_copy, b_neg, b_pos_active, OvU, NumU);

			for (Int_t bin=0;bin<72000;bin++)
			{
				Over_copy[bin]=Over_n[bin];
			}
			Gaps[1][1] = Active_Gap(1,Over_copy, b_pos, b_neg, FSel_n, OvV, 1);
			Find_Proj(Over_copy, b_pos, b_neg_active, OvV, NumV);

			if (Gaps[0][1]>0 || Gaps[1][1]>0)
			{
				b_p.push_back(b_pos_active);
				Ov_p.push_back(OvU);
				Num_p.push_back(NumU);

				b_n.push_back(b_neg_active);
				Ov_n.push_back(OvV);
				Num_n.push_back(NumV);

				GapsID.push_back(2);
			}

			//Then looking for both gaps in the plateau
			if ((Gaps[0][0]>0 || Gaps[1][0]>0) && (Gaps[0][1]>0 || Gaps[1][1]>0))
			{
				for (Int_t bin=0;bin<72000;bin++)
				{
					Over_copy[bin]=Over_p[bin];
				}
				Silent_Gap(Over_copy, b_neg, b_pos, FSel_p, FG_p, G_F_p, OvU, 0);
				Active_Gap(0,Over_copy, b_neg, b_pos, FSel_p, OvU, 0);
				Find_Proj(Over_copy, b_neg, b_pos_active, OvU, NumU);

				for (Int_t bin=0;bin<72000;bin++)
				{
					Over_copy[bin]=Over_n[bin];
				}
				Silent_Gap(Over_copy, b_pos, b_neg, FSel_n, FG_n, G_F_n, OvV, 1);
				Active_Gap(1,Over_copy, b_pos, b_neg, FSel_n, OvV, 1);
				Find_Proj(Over_copy, b_pos, b_neg_active, OvV, NumV);

				b_p.push_back(b_pos_active);
				Ov_p.push_back(OvU);
				Num_p.push_back(NumU);

				b_n.push_back(b_neg_active);
				Ov_n.push_back(OvV);
				Num_n.push_back(NumV);

				GapsID.push_back(3);
			}
		}
	}
}

void ALFA_MDGap::GetData(Int_t (&iNumU)[MAXTRACKNUM], Int_t (&iNumV)[MAXTRACKNUM], Float_t (&fOvU)[MAXTRACKNUM], Float_t (&fOvV)[MAXTRACKNUM], Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT])
{
	for (Int_t iTrack=0; iTrack<MAXTRACKNUM; iTrack++)
	{
		iNumU[iTrack] = m_iNU[iTrack];
		iNumV[iTrack] = m_iNV[iTrack];
		fOvU[iTrack]  = m_fOvU[iTrack];
		fOvV[iTrack]  = m_fOvV[iTrack];

		for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			iFibSel[iLayer] = m_iFibSel[iLayer];
		}
	}
}


Int_t ALFA_MDGap::Silent_Gap(Int_t (&Over)[72000], Float_t b_ref, Float_t b_rec, Int_t FSel[ALFAPLATESCNT], Bool_t (&FGap)[ALFAPLATESCNT], Int_t (&Gap_Fib)[ALFAPLATESCNT][2], Float_t OverLap, Int_t iSideFlag)
{
	Float_t fSign;
	Float_t fXInter, fYInter;
	Float_t fFibCen, fFibCen0, fFibCen1;

	Float_t b_center, gap_size;
	Int_t Gap_Width;

	Int_t Found_Gap=0;

	if (m_faMD[m_iRPot][iSideFlag][0]>0) fSign=1.0;
	else fSign=-1.0;

	for(UInt_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		if (FSel[iLayer]==9999)
		{
			if (iSideFlag==0 || (iSideFlag==1 && iLayer!=4))
			{
				for (Int_t iFiber=0; iFiber<ALFAFIBERSCNT-1; iFiber++)
				{
					fXInter= fSign*(b_ref-m_fbMD[m_iRPot][iLayer*2+iSideFlag][iFiber])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber]);
					fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber]*b_ref+m_fbMD[m_iRPot][iLayer*2+iSideFlag][iFiber])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber])-b_ref;
					fFibCen0 = (fYInter-fSign*fXInter)/sqrt(2.);

					fXInter= fSign*(b_ref-m_fbMD[m_iRPot][iLayer*2+iSideFlag][iFiber+1])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber+1]);
					fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber+1]*b_ref+m_fbMD[m_iRPot][iLayer*2+iSideFlag][iFiber+1])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iFiber+1])-b_ref;
					fFibCen1 = (fYInter-fSign*fXInter)/sqrt(2.);

					b_center = (fFibCen1+fFibCen0)/2.;

					fXInter= fSign*(b_ref-b_rec)/2.;
					fYInter= (b_ref+b_rec)/2.-b_ref;
					fFibCen = (fYInter-fSign*fXInter)/sqrt(2.);

					gap_size = TMath::Abs(fFibCen1-fFibCen0)-0.480;
					Gap_Width = int(gap_size*1000.);

					if (((b_center+gap_size/2.)<fFibCen+OverLap/2. && (b_center+gap_size/2.)> fFibCen-OverLap/2.) || ((b_center-gap_size/2.)>fFibCen-OverLap/2. && (b_center-gap_size/2.)<fFibCen+OverLap/2.))
					{
						Found_Gap++;
						FGap[iLayer]=true;

						Gap_Fib[iLayer][0] = iFiber;
						Gap_Fib[iLayer][1] = iFiber+1;

						for (Int_t bin=0;bin<Gap_Width;bin++)
						{
							Over[(Int_t)((b_center-gap_size/2.)*1000)+bin+36000]++;
						}
					}
				}
			}
		}
	}

	return Found_Gap;
}

Int_t ALFA_MDGap::Active_Gap(Int_t iFiberSide, Int_t (&Over)[72000], Float_t b_ref, Float_t b_rec, Int_t FSel[ALFAPLATESCNT], Float_t OverLap, Int_t iSideFlag)
{
	Float_t fSign;
	Float_t fXInter, fYInter;
	Float_t fFibCen, fFibCen0, fFibCen1;

	Float_t b_center, gap_size;
	Int_t Gap_Width;

	Int_t Found_Gap=0;

	if (m_faMD[m_iRPot][iSideFlag][0]>0) fSign=1.0;
	else fSign=-1.0;

	Bool_t bHit[2];
	Int_t shift;

	std::list<int>::iterator iHit;
	int iIndex = 0;
	for(UInt_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		iIndex = 2*iLayer+iFiberSide;
		if (FSel[iLayer]!=9999)
		{
			bHit[0]=false;
			bHit[1]=false;

			for (iHit=m_MapLayers[iIndex].ListFibers.begin(); iHit!=m_MapLayers[iIndex].ListFibers.end(); iHit++)
			{
				if (*iHit==FSel[iLayer]+1) bHit[0]=true;
				if (*iHit==FSel[iLayer]-1) bHit[1]=true;
			}

			for (UInt_t j=0;j<2;j++)
			{
				if (j==0) shift=1;
				else shift=-1;

				if(bHit[j])
				{
					fXInter= fSign*(b_ref-m_fbMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]]);
					fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]]*b_ref+m_fbMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]])-b_ref;
					fFibCen0 = (fYInter-fSign*fXInter)/sqrt(2.);

					fXInter= fSign*(b_ref-m_fbMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]+shift])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]+shift]);
					fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]+shift]*b_ref+m_fbMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]+shift])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][FSel[iLayer]+shift])-b_ref;
					fFibCen1 = (fYInter-fSign*fXInter)/sqrt(2.);

					b_center = (fFibCen1+fFibCen0)/2.;

					fXInter= fSign*(b_ref-b_rec)/2.;
					fYInter= (b_ref+b_rec)/2.-b_ref;
					fFibCen = (fYInter-fSign*fXInter)/sqrt(2.);

					gap_size = TMath::Abs(fFibCen1-fFibCen0)-0.480;
					Gap_Width = int(gap_size*1000.);

					if (((b_center+gap_size/2.)<fFibCen+OverLap/2. && (b_center+gap_size/2.)> fFibCen-OverLap/2.) || ((b_center-gap_size/2.)>fFibCen-OverLap/2. && (b_center-gap_size/2.)<fFibCen+OverLap/2.))
					{
						Found_Gap++;

						for (Int_t bin=0;bin<Gap_Width;bin++)
						{
							Over[(Int_t)((b_center-gap_size/2.)*1000)+bin+36000]++;
						}
					}
				}
			}
		}
	}

	return Found_Gap;
}
