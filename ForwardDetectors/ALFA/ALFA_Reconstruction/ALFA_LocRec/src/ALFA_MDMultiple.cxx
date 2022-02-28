/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_MDMultiple.h"
#include <algorithm>    // std::copy
//using namespace std;

ALFA_MDMultiple::ALFA_MDMultiple()
{
	memset(&m_iNumHitsLayer, 0.0, sizeof(m_iNumHitsLayer));

	m_fOverlapCut      = 0.0;
	m_iMultiplicityCut = 0;
	m_iNumLayerCut     = 0;
	m_iRPot            = 0;
	m_iUVCut           = 0;


	m_fRecXPos = NULL;
	m_fRecYPos = NULL;
	m_fOvU     = NULL;
	m_fOvV     = NULL;
	m_iNU      = NULL;
	m_iNV      = NULL;
	m_fRecXPos = new std::vector<Float_t>();
	m_fRecYPos = new std::vector<Float_t>();
	m_fOvU     = new std::vector<Float_t>();
	m_fOvV     = new std::vector<Float_t>();
	m_iNU      = new std::vector<Int_t>();
	m_iNV      = new std::vector<Int_t>();

	for (int iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		m_iFibSel[iLayer] = NULL;
		m_iFibSel[iLayer] = new std::vector<Int_t>();
	}

	m_iTrackMatch[0] = NULL;
	m_iTrackMatch[1] = NULL;
	m_iTrackMatch[0] = new std::vector<Int_t>();
	m_iTrackMatch[1] = new std::vector<Int_t>();
}

ALFA_MDMultiple::ALFA_MDMultiple(const ALFA_MDMultiple &obj)
{
//	std::copy(obj.m_iNumHitsLayer, obj.m_iNumHitsLayer + sizeof(obj.m_iNumHitsLayer)/sizeof(Int_t), m_iNumHitsLayer);
	std::copy(obj.m_iNumHitsLayer, obj.m_iNumHitsLayer + ALFALAYERSCNT*ALFAPLATESCNT, m_iNumHitsLayer);

	m_fOverlapCut      = obj.m_fOverlapCut;
	m_iMultiplicityCut = obj.m_iMultiplicityCut;
	m_iNumLayerCut     = obj.m_iNumLayerCut;
	m_iRPot            = obj.m_iRPot;
	m_iUVCut           = obj.m_iUVCut;

	m_fRecXPos = NULL;
	m_fRecYPos = NULL;
	m_fOvU     = NULL;
	m_fOvV     = NULL;
	m_iNU      = NULL;
	m_iNV      = NULL;
	m_fRecXPos = new std::vector<Float_t>();
	m_fRecYPos = new std::vector<Float_t>();
	m_fOvU     = new std::vector<Float_t>();
	m_fOvV     = new std::vector<Float_t>();
	m_iNU      = new std::vector<Int_t>();
	m_iNV      = new std::vector<Int_t>();

	std::copy(obj.m_fRecXPos->begin(), obj.m_fRecXPos->end(), m_fRecXPos->begin());
	std::copy(obj.m_fRecYPos->begin(), obj.m_fRecYPos->end(), m_fRecYPos->begin());
	std::copy(obj.m_fOvU->begin(),     obj.m_fOvU->end(),     m_fOvU->begin());
	std::copy(obj.m_fOvV->begin(),     obj.m_fOvV->end(),     m_fOvV->begin());
	std::copy(obj.m_iNU->begin(),      obj.m_iNU->end(),      m_iNU->begin());
	std::copy(obj.m_iNV->begin(),      obj.m_iNV->end(),      m_iNV->begin());

	for (int iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		m_iFibSel[iLayer] = new std::vector<Int_t>(*obj.m_iFibSel[iLayer]);
	}

	m_iTrackMatch[0] = NULL;
	m_iTrackMatch[1] = NULL;
	m_iTrackMatch[0] = new std::vector<Int_t>();
	m_iTrackMatch[1] = new std::vector<Int_t>();
	std::copy(obj.m_iTrackMatch[0]->begin(), obj.m_iTrackMatch[0]->end(), m_iTrackMatch[0]->begin());
	std::copy(obj.m_iTrackMatch[1]->begin(), obj.m_iTrackMatch[1]->end(), m_iTrackMatch[1]->begin());
}

ALFA_MDMultiple& ALFA_MDMultiple::operator=(const ALFA_MDMultiple &obj)
{
	if (this != &obj)
	{
//		std::swap(m_iNumHitsLayer, obj.m_iNumHitsLayer);
		Int_t nLayers = ALFALAYERSCNT*ALFAPLATESCNT;

		for (int i = 0; i < nLayers; i++)
			m_iNumHitsLayer[i] = obj.m_iNumHitsLayer[i];

		m_fOverlapCut      = obj.m_fOverlapCut;
		m_iMultiplicityCut = obj.m_iMultiplicityCut;
		m_iNumLayerCut     = obj.m_iNumLayerCut;
		m_iRPot            = obj.m_iRPot;
		m_iUVCut           = obj.m_iUVCut;

//		std::swap(m_fRecXPos, obj.m_fRecXPos);
//		std::swap(m_fRecYPos, obj.m_fRecYPos);
//		std::swap(m_fOvU    , obj.m_fOvU);
//		std::swap(m_fOvV    , obj.m_fOvV);
//		std::swap(m_iNU     , obj.m_iNU);
//		std::swap(m_iNV     , obj.m_iNV);
//		std::swap(m_iFibSel, obj.m_iFibSel);
//		std::swap(m_iTrackMatch, obj.m_iTrackMatch);

		std::copy(m_fRecXPos->begin(), m_fRecXPos->end(), obj.m_fRecXPos->begin());
		std::copy(m_fRecYPos->begin(), m_fRecYPos->end(), obj.m_fRecYPos->begin());
		std::copy(m_fOvU->begin(),     m_fOvU->end(),     obj.m_fOvU->begin());
		std::copy(m_fOvV->begin(),     m_fOvV->end(),     obj.m_fOvV->begin());
		std::copy(m_iNU->begin(),      m_iNU->end(),      obj.m_iNU->begin());
		std::copy(m_iNV->begin(),      m_iNV->end(),      obj.m_iNV->begin());

		for (int iLayer = 0; iLayer < nLayers; iLayer++)
		{
			m_iFibSel[iLayer] = NULL;
			m_iFibSel[iLayer] = new std::vector<Int_t>();
			std::copy(m_iFibSel[iLayer]->begin(), m_iFibSel[iLayer]->end(), obj.m_iFibSel[iLayer]->begin());
		}

		m_iTrackMatch[0] = NULL;
		m_iTrackMatch[1] = NULL;
		m_iTrackMatch[0] = new std::vector<Int_t>();
		m_iTrackMatch[1] = new std::vector<Int_t>();
		std::copy(m_iTrackMatch[0]->begin(), m_iTrackMatch[0]->end(), obj.m_iTrackMatch[0]->begin());
		std::copy(m_iTrackMatch[1]->begin(), m_iTrackMatch[1]->end(), obj.m_iTrackMatch[1]->begin());
	}

	return *this;
}

ALFA_MDMultiple::~ALFA_MDMultiple()
{
	if (m_fRecXPos!=NULL) {delete m_fRecXPos; m_fRecXPos=NULL;}
	if (m_fRecYPos!=NULL) {delete m_fRecYPos; m_fRecYPos=NULL;}
	if (m_fOvU!=NULL) {delete m_fOvU; m_fOvU=NULL;}
	if (m_fOvV!=NULL) {delete m_fOvV; m_fOvV=NULL;}
	if (m_iNU!=NULL) {delete m_iNU; m_iNU=NULL;}
	if (m_iNV!=NULL) {delete m_iNV; m_iNV=NULL;}

	for (int iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		if (m_iFibSel[iLayer]!=NULL) {delete m_iFibSel[iLayer]; m_iFibSel[iLayer]=NULL;}
	}
	if (m_iTrackMatch[0]!=NULL) {delete m_iTrackMatch[0]; m_iTrackMatch[0]=NULL;}
	if (m_iTrackMatch[1]!=NULL) {delete m_iTrackMatch[1]; m_iTrackMatch[1]=NULL;}
}

StatusCode ALFA_MDMultiple::Initialize(Int_t iRPot, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iMultiplicityCut, Int_t iNumLayerCut, Int_t iUVCut, Float_t fOverlapCut)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Initialize()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Initialize()");

	m_iRPot            = iRPot;
	m_iUVCut           = iUVCut;
	m_iNumLayerCut     = iNumLayerCut;
	m_iMultiplicityCut = iMultiplicityCut;
	m_fOverlapCut      = fOverlapCut;

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

//	std::cout << "m_iMultiplicityCut, m_iUVCut = " << m_iMultiplicityCut << ", " << m_iUVCut << std::endl;

	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDMultiple::Execute(const std::list<MDHIT> &ListMDHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Execute()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Execute()");

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
	std::vector<double> b_p, b_n;
	std::vector<double> Ov_p, Ov_n;
	std::vector<int> Num_p, Num_n;
	std::vector<int> FSel_n[ALFAPLATESCNT], FSel_p[ALFAPLATESCNT];
	std::vector<int> iTrackMatch[2];


	//Checking that the multiplicity cut conditions are satisfied
	//At least more than UV_cut layers have a multiplicity lower than multi_cut
// 	Int_t iNumUFiberHits=0, iNumVFiberHits=0;
// 	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
// 	{
// 		if ((Int_t)m_MapLayers[iLayer].ListFibers.size()<=m_iMultiplicityCut && (Int_t)m_MapLayers[iLayer].ListFibers.size()>0)
// 		{
// 			// prohozen U/V?
//  //			if (fmod(double(iLayer),double(2)) == 1) iNumUFiberHits++;
//  //			else iNumVFiberHits++;
// 
// 			if (fmod(double(iLayer),double(2)) == 0) iNumUFiberHits++;
// 			else iNumVFiberHits++;
// 		}
// 	}

// 	if (iNumUFiberHits>=m_iMultiplicityCut && iNumVFiberHits>=m_iMultiplicityCut)
	{
		Reco_Track(b_p, b_n, Ov_p, Ov_n, Num_p, Num_n, FSel_n, FSel_p, iTrackMatch);

		//Now sorting the tracks using NumU+NumV criteria -------------------------------
		Int_t iCntSort=0;
		Int_t iMaxSum=0;
		std::vector<Int_t> MaxTrackID;
		Int_t iTmpMaxTrackID = -1;

//		std::cout << "b_p.size = " << b_p.size() << std::endl;
		while (iCntSort<(Int_t)b_p.size())
		{
			iMaxSum=0;
			for (Int_t i=0; i<(Int_t)b_p.size(); i++)
			{
				//Checking that the maximum was not already used
				Bool_t MaxUsed=false;

				for (UInt_t j=0; j<MaxTrackID.size(); j++)
				{
					if (i==MaxTrackID[j]) {MaxUsed = true; break;}
				}

				if (((Num_p[i]+Num_n[i])>iMaxSum) && (!MaxUsed))
				{
					iMaxSum = Num_p[i]+Num_n[i];
					iTmpMaxTrackID=i;
				}
			}
			MaxTrackID.push_back(iTmpMaxTrackID);
			iCntSort++;
		}

//		std::cout << "MaxTrackID size = " << MaxTrackID.size() << std::endl;

//		for (Int_t i=0; i<(Int_t)b_p.size();i++)
		for (Int_t i=0; i<(Int_t)MaxTrackID.size();i++)
		{
//			std::cout << "   [" << i << "] = " << MaxTrackID[i] << std::endl;
//			if (fabs((b_p[i]+b_n[i])/2.0) < 135.5)
			{
				m_fRecXPos->push_back((b_p[MaxTrackID[i]]-b_n[MaxTrackID[i]])/2.0);
				m_fRecYPos->push_back((b_p[MaxTrackID[i]]+b_n[MaxTrackID[i]])/2.0);

				m_fOvU->push_back(Ov_p[MaxTrackID[i]]);
				m_fOvV->push_back(Ov_n[MaxTrackID[i]]);
				m_iNU->push_back(Num_p[MaxTrackID[i]]);
				m_iNV->push_back(Num_n[MaxTrackID[i]]);

				//the U and V are somehow interchanged, therefore they are here interchanged back.
//				m_fOvU->push_back(Ov_n[MaxTrackID[i]]);   //just for test
//				m_fOvV->push_back(Ov_p[MaxTrackID[i]]);   //just for test
//				m_iNU->push_back(Num_n[MaxTrackID[i]]);   //just for test
//				m_iNV->push_back(Num_p[MaxTrackID[i]]);   //just for test

				m_iTrackMatch[0]->push_back(iTrackMatch[0][MaxTrackID[i]]);
				m_iTrackMatch[1]->push_back(iTrackMatch[1][MaxTrackID[i]]);

//				std::cout << "iTrackMatch[0][" << MaxTrackID[i] << "] = " << iTrackMatch[0][MaxTrackID[i]] << std::endl;
//				std::cout << "iTrackMatch[1][" << MaxTrackID[i] << "] = " << iTrackMatch[1][MaxTrackID[i]] << std::endl;
//				std::cout << "x, y = " << (b_p[MaxTrackID[i]]-b_n[MaxTrackID[i]])/2.0 << ", " << (b_p[MaxTrackID[i]]+b_n[MaxTrackID[i]])/2.0 << std::endl;

				for (Int_t iPlate=0; iPlate<ALFAPLATESCNT; iPlate++)
				{
					m_iFibSel[2*iPlate]->push_back(FSel_p[iPlate][MaxTrackID[i]]);
					m_iFibSel[2*iPlate+1]->push_back(FSel_n[iPlate][MaxTrackID[i]]);
//					m_iFibSel[2*iPlate+1]->push_back(FSel_p[iPlate][MaxTrackID[i]]);   //just for test
//					m_iFibSel[2*iPlate]->push_back(FSel_n[iPlate][MaxTrackID[i]]);     //just for test
				}
			}
		}
//		{
////			if (fabs((b_p[i]+b_n[i])/2.0) < 135.5)
//			{
//				m_fRecXPos->push_back((b_p[i]-b_n[i])/2.0);
//				m_fRecYPos->push_back((b_p[i]+b_n[i])/2.0);

////				m_fOvU->push_back(Ov_p[i]);
////				m_fOvV->push_back(Ov_n[i]);
////				m_iNU->push_back(Num_p[i]);
////				m_iNV->push_back(Num_n[i]);

//				//the U and V are somehow interchanged, therefore they are here interchanged back.
//				m_fOvU->push_back(Ov_n[i]);
//				m_fOvV->push_back(Ov_p[i]);
//				m_iNU->push_back(Num_n[i]);
//				m_iNV->push_back(Num_p[i]);

//				m_iTrackMatch[0]->push_back(iTrackMatch[0][i]);
//				m_iTrackMatch[1]->push_back(iTrackMatch[1][i]);

//				std::cout << "iTrackMatch[0][" << i << "] = " << iTrackMatch[0][i] << std::endl;
//				std::cout << "iTrackMatch[1][" << i << "] = " << iTrackMatch[1][i] << std::endl;
//				std::cout << "x, y = " << (b_p[i]-b_n[i])/2.0 << ", " << (b_p[i]+b_n[i])/2.0 << std::endl;

//				for (Int_t iPlate=0; iPlate<ALFAPLATESCNT; iPlate++)
//				{
////					m_iFibSel[2*iPlate]->push_back(FSel_p[iPlate][i]);
////					m_iFibSel[2*iPlate+1]->push_back(FSel_n[iPlate][i]);
//					m_iFibSel[2*iPlate+1]->push_back(FSel_p[iPlate][i]);
//					m_iFibSel[2*iPlate]->push_back(FSel_n[iPlate][i]);
//				}
//			}

//		}

// 		std::cout << "------------------------------------------------------------------" << std::endl;
	}


	return StatusCode::SUCCESS;
}

StatusCode ALFA_MDMultiple::Finalize(Float_t (&fRecXPos)[MAXTRACKNUM], Float_t (&fRecYPos)[MAXTRACKNUM])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Finalize()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Finalize()");

	Int_t iTrackNum=0, iSize=0;
	std::fill_n(&fRecXPos[0], sizeof(fRecXPos)/sizeof(Float_t), -9999.0);
	std::fill_n(&fRecYPos[0], sizeof(fRecYPos)/sizeof(Float_t), -9999.0);

	iSize = (m_fRecXPos->size() <= m_fRecYPos->size())? m_fRecXPos->size() : m_fRecYPos->size();
	iTrackNum = (iSize < MAXTRACKNUM)? iSize : MAXTRACKNUM;

	for (Int_t i=0; i<iTrackNum; i++)
	{
		fRecXPos[i] = m_fRecXPos->at(i);
		fRecYPos[i] = m_fRecYPos->at(i);
	}

	return StatusCode::SUCCESS;
}

/************************************************/
/*  Making projection and storing in an array   */
/************************************************/
void ALFA_MDMultiple::Proj_Store(Int_t iFiberSide, Int_t (&iOver)[72000], Float_t fbRef, Int_t iSideFlag)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Proj_Store()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Pro_Store()");

	Float_t fSign;
	Float_t fXInter, fYInter;
	Float_t FibCen;

	if (m_faMD[m_iRPot][iSideFlag][0]>0) fSign=1.0;
	else fSign=-1.0;

	for (Int_t iBin=0; iBin<72000; iBin++)
	{
		iOver[iBin]=0;
	}

	// coverity bug 13955 fixed bellow
//	std::list<int>::iterator intIter;
//	for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
//	{
//		for (intIter=m_MapLayers[2*iLayer+iFiberSide].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer+iFiberSide].ListFibers.end(); intIter++)
//		{
//			if (*intIter!=9999)
//			{
//				//Depending on layer orientation, computing the projection of the hit fiber
//				fXInter= fSign*(fbRef-m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]);
//				fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]*fbRef+m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter])-fbRef;
//				FibCen = (fYInter-fSign*fXInter)/sqrt(2.0);

//				//Filling the table with the hit fiber
//				for (Int_t iBin=0; iBin<480; iBin++)
//				{
//					iOver[(int)((FibCen-0.24)*1000)+iBin+36000]++;
//				}
//			}
//		}
//	}
	
	for (UInt_t iLayer=0; iLayer!=ALFAPLATESCNT; ++iLayer)
	{
		const unsigned int thisSideLayer = iLayer*2+iSideFlag;
		const unsigned int thisLayer = 2*iLayer+iFiberSide;
		const std::list<int> & thisFiberContainer = m_MapLayers[thisLayer].ListFibers;
		for (const auto & thisFiber:thisFiberContainer)
		{
			if (thisFiber!=9999)
			{
				//Depending on layer orientation, computing the projection of the hit fiber
				fXInter= fSign*(fbRef-m_fbMD[m_iRPot][thisSideLayer][thisFiber])/(1+fSign*m_faMD[m_iRPot][thisSideLayer][thisFiber]);
				fYInter= (fSign*m_faMD[m_iRPot][thisSideLayer][thisFiber]*fbRef+m_fbMD[m_iRPot][thisSideLayer][thisFiber])/(1+fSign*m_faMD[m_iRPot][thisSideLayer][thisFiber])-fbRef;
				FibCen = (fYInter-fSign*fXInter)/sqrt(2.0);

				//Filling the table with the hit fiber
				for (Int_t iBin=0; iBin<480; iBin++)
				{
					iOver[(int)((FibCen-0.24)*1000)+iBin+36000]++;
				}
			}
		}
	}
}

/************************************************/
/*  Making projection and storing in an array   */
/************************************************/
void ALFA_MDMultiple::Proj_Store(std::vector<Int_t> FiberHit[ALFAPLATESCNT], Int_t (&iOver)[72000], Float_t fbRef, Int_t iSideFlag)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Proj_Store()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Pro_Store()");

	Float_t fSign;
	Float_t fXInter, fYInter;
	Float_t FibCen;

	if (m_faMD[m_iRPot][iSideFlag][0]>0) fSign=1.0;
	else fSign=-1.0;

	for (Int_t iBin=0; iBin<72000; iBin++)
	{
		iOver[iBin]=0;
	}

	int iHit = 9999;
	for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		for (UInt_t j=0; j<FiberHit[iLayer].size(); j++)
		{
			iHit = FiberHit[iLayer][j];
			if (iHit!=9999)
			{
				//Depending on layer orientation, computing the projection of the hit fiber
				fXInter= fSign*(fbRef-m_fbMD[m_iRPot][iLayer*2+iSideFlag][iHit])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iHit]);
				fYInter= (fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iHit]*fbRef+m_fbMD[m_iRPot][iLayer*2+iSideFlag][iHit])/(1+fSign*m_faMD[m_iRPot][iLayer*2+iSideFlag][iHit])-fbRef;
				FibCen = (fYInter-fSign*fXInter)/sqrt(2.0);

				//Filling the table with the hit fiber
				for (Int_t iBin=0; iBin<480; iBin++)
				{
					iOver[(int)((FibCen-0.24)*1000)+iBin+36000]++;
				}
			}
		}
	}
}

/************************************************/
/*   Identifying plateau in projection array	*/
/************************************************/
void ALFA_MDMultiple::Find_Proj(Int_t iOver[72000], Float_t fbRef, Float_t &fb, Float_t &fOv, Int_t &iNum)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Find_Proj()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Find_Proj()");

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
		p_min = -36.0 + double(iSizePlateau.front())*1e-3;
		p_tmp_min = iSizePlateau.front();

		p_max = -36.0 + double(iSizePlateau.back())*1e-3;
//		p_tmp_max = iSizePlateau.back();

//		Making sure that the plateau belongs to the same track
		Int_t full_width = iSizePlateau.size();

		for (Int_t i=0; i<full_width; i++)
		{
			if (iSizePlateau[full_width-i-1]-p_tmp_min < 500)
			{
//				p_tmp_max = iSizePlateau[full_width-i-1];
				p_max = -36.0 + double(iSizePlateau[full_width-i-1])*1e-3;

				break;
			}
		}

		if ((p_max-p_min)<m_fOverlapCut)
		{
//			Storing the coordinate of the maximum
			fb = fbRef+(p_min+p_max)/sqrt(2.0);
			fOv = p_max-p_min;
		}
	}

	iNum = iNumFib;
}

/************************************************/
/************************************************/
/*       Identifying fibers on the track        */
/************************************************/
/************************************************/

void ALFA_MDMultiple::Finding_Fib(Int_t iFiberSide, Float_t fbRef, Float_t fbRec, Int_t (&iFSel)[10], Int_t iSideFlag)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Finding_Fib()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Finding_Fib()");

	Float_t b_pos, b_neg;
	Float_t x, y, x_tmp, y_tmp;
	Float_t min_dist;
	Float_t dist_x, dist_y;
	Float_t dist_full;
	Float_t fib_dist;
//	Int_t gFib;

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
	for (Int_t iLayer = 0; iLayer<ALFAPLATESCNT; iLayer++)
	{
		min_dist=0.24;
//		gFib = 9999;

		// coverity bug 13956 fixed
//		for (intIter=m_MapLayers[2*iLayer+iFiberSide].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer+iFiberSide].ListFibers.end(); intIter++)
//		{
//			if (*intIter!=9999)
//			{
//				x_tmp = (y-m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter])/m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter];
//				y_tmp = m_faMD[m_iRPot][iLayer*2+iSideFlag][*intIter]*x+m_fbMD[m_iRPot][iLayer*2+iSideFlag][*intIter];

//				dist_x = TMath::Abs(x-x_tmp);
//				dist_y = TMath::Abs(y-y_tmp);

//				dist_full = sqrt(dist_x*dist_x+dist_y*dist_y);
//				fib_dist = sqrt(TMath::Power((dist_x+dist_y)/2.0,2)-TMath::Power(dist_full/2.0,2));

//				if (fib_dist <= min_dist)
//				{
//					min_dist = fib_dist;
//					gFib = *intIter;
//					iFSel[iLayer] = gFib;
//				}
//			}
//		}

		const unsigned int thisSideLayer = iLayer*2+iSideFlag;
		const unsigned int thisLayer = 2*iLayer+iFiberSide;
		const std::list<int> & thisFiberContainer = m_MapLayers[thisLayer].ListFibers;
		for (const auto & thisFiber:thisFiberContainer)
		{
			if (thisFiber != 9999)
			{
				x_tmp = (y-m_fbMD[m_iRPot][thisSideLayer][thisFiber])/m_faMD[m_iRPot][thisSideLayer][thisFiber];
				y_tmp = m_faMD[m_iRPot][thisSideLayer][thisFiber]*x+m_fbMD[m_iRPot][thisSideLayer][thisFiber];

				dist_x = TMath::Abs(x-x_tmp);
				dist_y = TMath::Abs(y-y_tmp);

				dist_full = sqrt(dist_x*dist_x+dist_y*dist_y);
				fib_dist = sqrt(TMath::Power((dist_x+dist_y)/2.0,2)-TMath::Power(dist_full/2.0,2));

				if (fib_dist <= min_dist)
				{
					min_dist = fib_dist;
					iFSel[iLayer] = thisFiber;
				}
			}
		}


/*
		std::list<int> listFibersTmp = m_MapLayers[2*iLayer+iFiberSide].ListFibers;
		std::list<int>::const_iterator itBeg = listFibersTmp.begin();
		std::list<int>::const_iterator itEnd = listFibersTmp.end();
		for (; itBeg!=itEnd; ++itBeg)
		{
			if (*itBeg!=9999)
			{
				x_tmp = (y-m_fbMD[m_iRPot][iLayer*2+iSideFlag][*itBeg])/m_faMD[m_iRPot][iLayer*2+iSideFlag][*itBeg];
				y_tmp = m_faMD[m_iRPot][iLayer*2+iSideFlag][*itBeg]*x+m_fbMD[m_iRPot][iLayer*2+iSideFlag][*itBeg];

				dist_x = TMath::Abs(x-x_tmp);
				dist_y = TMath::Abs(y-y_tmp);

				dist_full = sqrt(dist_x*dist_x+dist_y*dist_y);
				fib_dist = sqrt(TMath::Power((dist_x+dist_y)/2.0,2)-TMath::Power(dist_full/2.0,2));

				if (fib_dist <= min_dist)
				{
					min_dist = fib_dist;
					gFib = *itBeg;

					iFSel[iLayer] = gFib;
				}
			}
		}
*/

	}
}

/************************************************/
/*				Finding all tracks				*/
/************************************************/

void ALFA_MDMultiple::Reco_Track(std::vector<double> &b_p, std::vector<double> &b_n,
								std::vector<double> &Ov_p, std::vector<double> &Ov_n,
								std::vector<int> &Num_p, std::vector<int> &Num_n,
								std::vector<int> (&FSel_n)[ALFAPLATESCNT], std::vector<int> (&FSel_p)[ALFAPLATESCNT],
								std::vector<int> (&iTrackMatch)[2])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::Reco_Track()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::Reco_Track()");

//	Int_t FSel_pos[ALFAPLATESCNT];
	Int_t FSel_neg[ALFAPLATESCNT];
	Int_t FSel_pos_tmp[ALFAPLATESCNT];
	Int_t Over_p[72000];
	Int_t Over_n[72000];
	Int_t cnt_step_U=0;
	Int_t cnt_step_V=0;
	Int_t NumU=0;
	Int_t NumV=0;
	Float_t b_ref_p;
	Float_t b_ref_n;
	Float_t OvU;
	Float_t OvV;
	Float_t b_pos;
	Float_t b_neg;

 	std::list<int>::iterator intIter;


	//clear
	for (Int_t i=0; i<ALFAPLATESCNT; i++)
	{
		FSel_n[i].clear();
		FSel_p[i].clear();
	}
	b_n.clear();
	b_p.clear();
	Ov_n.clear();
	Ov_p.clear();
	Num_n.clear();
	Num_p.clear();

	int iNumUFiberHits = 0;
	int iNumVFiberHits = 0;

	std::vector<Int_t> Fiber_MB_tmp[ALFAPLATESCNT];
	std::vector<Int_t> Fiber_MB_n[ALFAPLATESCNT];

	//fix coverity 13959 - following code replaced by next for loop
//	std::list<int>::iterator it;
//	for (UInt_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
//	{
//		Fiber_MB_n[iLayer].clear();
//		for (it=m_MapLayers[2*iLayer+1].ListFibers.begin(); it!=m_MapLayers[2*iLayer+1].ListFibers.end(); it++)
////		for (it=m_MapLayers[2*iLayer].ListFibers.begin(); it!=m_MapLayers[2*iLayer].ListFibers.end(); it++)   //just for test
//		{
//			Fiber_MB_n[iLayer].push_back(*it);
//		}
//	}

	for (UInt_t iLayer=0; iLayer!=ALFAPLATESCNT; ++iLayer)
	{
		Fiber_MB_n[iLayer].clear();
		const unsigned int thisLayer=2*iLayer+1;
		const std::list<int> & thisFiberContainer = m_MapLayers[thisLayer].ListFibers;
		for (const auto & thisFiber:thisFiberContainer)
		{
//			std::cout << "thisFiber: " << thisFiber << std::endl;
			Fiber_MB_n[iLayer].push_back(thisFiber);
		}
	}
	do
	{
		b_ref_n=-127.0;
		b_ref_p=-127.0;

//		First projection step on U side
//		-------------------------------
//		filling the array for U side with reference value
		Proj_Store(0, Over_p, b_ref_n, 0);
//		Proj_Store(1, Over_p, b_ref_n, 0);   //just for test
//		Proj_Store(1, Over_p, b_ref_n, 1);   //just for test

//		Find first maxium
		Find_Proj(Over_p, b_ref_n, b_pos, OvU, NumU);

		Finding_Fib(0, b_ref_n, b_pos, FSel_pos_tmp, 0);
//		Finding_Fib(1, b_ref_n, b_pos, FSel_pos_tmp, 0);   //just for test
//		Finding_Fib(1, b_ref_n, b_pos, FSel_pos_tmp, 1);   //just for test
		for (int i=0; i<ALFAPLATESCNT; i++)
		{
			Fiber_MB_tmp[i].clear();
			Fiber_MB_tmp[i].push_back(FSel_pos_tmp[i]);
		}

//		// added ----------
//		std::cout << "FSel_pos after 1st U proj: ";
//		for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
//		{
//			std::cout << " " << FSel_pos[iLayer];
//		}
//		std::cout << std::endl;

		//Then reconstruction all tracks possible using the second side
		for (UInt_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
		{
			m_MapLayers[2*iLayer+1].ListFibers.clear();
//			m_MapLayers[2*iLayer].ListFibers.clear();   //just for test
			for (unsigned int i=0; i<Fiber_MB_n[iLayer].size(); i++)
			{
				m_MapLayers[2*iLayer+1].ListFibers.push_back(Fiber_MB_n[iLayer][i]);
//				m_MapLayers[2*iLayer].ListFibers.push_back(Fiber_MB_n[iLayer][i]);   //just for test
			}
		}

//		std::cout << "NumU " << NumU << std::endl;

//		Then reconstruct all tracks possible using the second side
		if (NumU>=m_iUVCut)
		{
			cnt_step_V=0;
			do
			{
				// New Part to apply the multiplicity cut at each iteration
				iNumUFiberHits=0;
				iNumVFiberHits=0;
				for (UInt_t iLayer=0;iLayer<ALFAPLATESCNT;iLayer++)
				{
					if ((Int_t)m_MapLayers[2*iLayer].ListFibers.size()<=m_iMultiplicityCut && m_MapLayers[2*iLayer].ListFibers.size()>0) iNumUFiberHits++;
					if ((Int_t)m_MapLayers[2*iLayer+1].ListFibers.size()<=m_iMultiplicityCut && m_MapLayers[2*iLayer+1].ListFibers.size()>0) iNumVFiberHits++;
//					if ((Int_t)m_MapLayers[2*iLayer+1].ListFibers.size()<=m_iMultiplicityCut && m_MapLayers[2*iLayer+1].ListFibers.size()>0) iNumUFiberHits++;   //just for test
//					if ((Int_t)m_MapLayers[2*iLayer].ListFibers.size()<=m_iMultiplicityCut && m_MapLayers[2*iLayer].ListFibers.size()>0) iNumVFiberHits++;       //just for test
				}

//				std::cout << "2nd multiplicity cut " << iNumUFiberHits << " " << iNumVFiberHits << std::endl;

				if (iNumUFiberHits>=m_iNumLayerCut && iNumVFiberHits>=m_iNumLayerCut)
				{
					//First projection on V side
					//-------------------------------
					//filling the array for V side with reference value
					Proj_Store(1, Over_n, b_ref_p, 1);
//					Proj_Store(0, Over_n, b_ref_p, 1);   //just for test
//					Proj_Store(0, Over_n, b_ref_p, 0);   //just for test
					Find_Proj(Over_n, b_ref_p, b_neg, OvV, NumV);

//					std::cout << "NumV " << NumV << std::endl;

					if (NumV>=m_iUVCut)
					{
						//Now make the second projection step
						//-----------------------------------
						//U side
//						Proj_Store(0, Over_p, b_neg, 0);
	//					Proj_Store(1, Over_p, b_neg, 0);   //just for test
						Proj_Store(Fiber_MB_tmp, Over_p, b_neg, 0);
//						Proj_Store(Fiber_MB_tmp, Over_p, b_neg, 1);   //just for test
						Find_Proj(Over_p, b_neg, b_pos, OvU, NumU);

						//V side
						Proj_Store(1, Over_n, b_pos, 1);
//						Proj_Store(0, Over_n, b_pos, 1);   //just for test
//						Proj_Store(0, Over_n, b_pos, 0);   //just for test
						Find_Proj(Over_n, b_pos, b_neg, OvV, NumV);

						//Third projection steps
						//----------------------
						//U side
//						Proj_Store(0, Over_p, b_neg, 0);
	//					Proj_Store(1, Over_p, b_neg, 0);   //just for test
						Proj_Store(Fiber_MB_tmp, Over_p, b_neg, 0);
//						Proj_Store(Fiber_MB_tmp, Over_p, b_neg, 1);   //just for test
						Find_Proj(Over_p, b_neg, b_pos, OvU, NumU);

						//V side
						Proj_Store(1, Over_n, b_pos, 1);
//						Proj_Store(0, Over_n, b_pos, 1);   //just for test
//						Proj_Store(0, Over_n, b_pos, 0);   //just for test
						Find_Proj(Over_n, b_pos, b_neg, OvV, NumV);

	//					//We store the information in the vector
						b_p.push_back(b_pos);
						Ov_p.push_back(OvU);
						Num_p.push_back(NumU);

						b_n.push_back(b_neg);
						Ov_n.push_back(OvV);
						Num_n.push_back(NumV);

						iTrackMatch[0].push_back(cnt_step_U);
						iTrackMatch[1].push_back(cnt_step_V);

						//Once done we want to remove the hit belonging to the first track on side V
						//We first find the corresponding fibers
//						Finding_Fib(0,b_neg, b_pos, FSel_pos, 0);
						Finding_Fib(1,b_pos, b_neg, FSel_neg, 1);
//						Finding_Fib(1,b_neg, b_pos, FSel_pos, 0);   //just for test
//						Finding_Fib(0,b_pos, b_neg, FSel_neg, 1);   //just for test
//						Finding_Fib(1,b_neg, b_pos, FSel_pos, 1);   //just for test
//						Finding_Fib(0,b_pos, b_neg, FSel_neg, 0);   //just for test

						for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
						{
							FSel_n[iLayer].push_back(FSel_neg[iLayer]);
//							FSel_p[iLayer].push_back(FSel_pos[iLayer]);
							FSel_p[iLayer].push_back(FSel_pos_tmp[iLayer]);
						}

						//added -----------------------------------------------------
//						std::cout << "FSel_n ";
//						for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
//						{
//							std::cout << " " << FSel_neg[iLayer];
//						}
//						std::cout << std::endl;
//						std::cout << "FSel_p ";
//						for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
//						{
//							std::cout << " " << FSel_pos[iLayer];
//						}
//						std::cout << std::endl;

						//Removing fibers used for the first track for V Side
						for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; ++iLayer)
						{
							// coverity bug 30038 fixed bellow
//							std::list<int>::iterator itBeg = m_MapLayers[2*iLayer+1].ListFibers.begin();
//							std::list<int>::iterator itEnd = m_MapLayers[2*iLayer+1].ListFibers.end();
//							for (; itBeg != itEnd; itBeg++)
//							{
//								if (*itBeg == (int)FSel_neg[iLayer])
//								{
//									m_MapLayers[2*iLayer+1].ListFibers.erase(itBeg);
//									break;
//								}
//							}

							const unsigned int thisLayer=2*iLayer+1;
							const std::list<int> & thisFiberContainer = m_MapLayers[thisLayer].ListFibers;
							for (const auto & thisFiber:thisFiberContainer)
							{
								if (thisFiber == (int)FSel_neg[iLayer])
								{
									auto it = std::find(begin(thisFiberContainer), end(thisFiberContainer), thisFiber);
									m_MapLayers[2*iLayer+1].ListFibers.erase(it);
									break;
								}
							}


//							for (intIter=m_MapLayers[2*iLayer+1].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer+1].ListFibers.end(); intIter++)
////							for (intIter=m_MapLayers[2*iLayer].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer].ListFibers.end(); intIter++)   //just for test
//							{
//// 								if (*intIter==(int)FSel_neg[iLayer])
//								if (*intIter==(int)FSel_neg[iLayer])
//								{
////									*intIter = 9999;
// 									m_MapLayers[2*iLayer+1].ListFibers.erase(intIter);
////									m_MapLayers[2*iLayer].ListFibers.erase(intIter);   //just for test
//									break;
//								}
//							}
						}
						cnt_step_V++;
					}
				}
				else NumV = 0;
			}
			while (NumV>=m_iUVCut);

//			When we cannot find tracks anymore for the V side, and that all combinations with the U side has been done, we start again with the U side
//			But first we remove the fibers belonging to this track

//			std::cout << "cnt_step_V " << cnt_step_V << std::endl;

//			Int_t iNumErasedFibs=0;
			if (cnt_step_V>0)
			{
				for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
				{
					std::list<int>::iterator itBeg = m_MapLayers[2*iLayer].ListFibers.begin();
					std::list<int>::iterator itEnd = m_MapLayers[2*iLayer].ListFibers.end();
					for (; itBeg != itEnd; itBeg++)
					{
						if (*itBeg == (int)FSel_pos_tmp[iLayer])
						{
							m_MapLayers[2*iLayer].ListFibers.erase(itBeg);
							break;
						}
					}


//					for (intIter=m_MapLayers[2*iLayer].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer].ListFibers.end(); intIter++)
////					for (intIter=m_MapLayers[2*iLayer+1].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer+1].ListFibers.end(); intIter++)   //just for test
//					{
////						if (*intIter==(int)FSel_pos[iLayer])
//						if (*intIter==(int)FSel_pos_tmp[iLayer])
//						{
////							*intIter = 9999;
//							m_MapLayers[2*iLayer].ListFibers.erase(intIter);
////							m_MapLayers[2*iLayer+1].ListFibers.erase(intIter);   //just for test
////							iNumErasedFibs++;
//							break;
//						}

//					}
				}
			}
			else
			{
				if (NumV>0)
				{
//					Finding_Fib(0, b_ref_n, b_pos, FSel_pos, 0);
//					Finding_Fib(1, b_ref_n, b_pos, FSel_pos, 0);   //just for test

					for (Int_t iLayer=0; iLayer<ALFAPLATESCNT; iLayer++)
					{
						std::list<int>::iterator itBeg = m_MapLayers[2*iLayer].ListFibers.begin();
						std::list<int>::iterator itEnd = m_MapLayers[2*iLayer].ListFibers.end();
						for (; itBeg != itEnd; itBeg++)
						{
							if (*itBeg == (int)FSel_pos_tmp[iLayer])
							{
								m_MapLayers[2*iLayer].ListFibers.erase(itBeg);
								break;
							}
						}


//						for (intIter=m_MapLayers[2*iLayer].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer].ListFibers.end(); intIter++)
////						for (intIter=m_MapLayers[2*iLayer+1].ListFibers.begin(); intIter!=m_MapLayers[2*iLayer+1].ListFibers.end(); intIter++)   //just for test
//						{
////							if (*intIter==(int)FSel_pos[iLayer])
//							if (*intIter==(int)FSel_pos_tmp[iLayer])
//							{
//	//							*intIter = 9999;
//								m_MapLayers[2*iLayer].ListFibers.erase(intIter);
////								m_MapLayers[2*iLayer+1].ListFibers.erase(intIter);   //just for test
////								iNumErasedFibs++;
//								break;
//							}
//						}
					}
				}
				else break;
			}
			cnt_step_U++;
//			if (iNumErasedFibs==0) NumU=0;
		}
	}
	while (NumU>=m_iUVCut);
}

void ALFA_MDMultiple::GetData(Int_t (&iNumU)[MAXTRACKNUM], Int_t (&iNumV)[MAXTRACKNUM], Float_t (&fOvU)[MAXTRACKNUM], Float_t (&fOvV)[MAXTRACKNUM], Int_t (&iFibSel)[MAXTRACKNUM][ALFALAYERSCNT*ALFAPLATESCNT])
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDMultiple::GetData()");
	ATH_MSG_DEBUG("ALFA_MDMultiple::GetData()");

	Int_t iTrackNum;
	Int_t iSize;
	std::fill_n(&fOvU[0], sizeof(fOvU)/sizeof(Float_t), -9999.0);
	std::fill_n(&fOvV[0], sizeof(fOvV)/sizeof(Float_t), -9999.0);
	std::fill_n(&iNumU[0], sizeof(iNumU)/sizeof(Int_t), -9999);
	std::fill_n(&iNumV[0], sizeof(iNumV)/sizeof(Int_t), -9999);
	std::fill_n(&iFibSel[0][0], sizeof(iFibSel)/sizeof(Int_t), -9999);

	iTrackNum=0;
	iSize=0;
	for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
	{
		iSize = ((Int_t)m_iFibSel[iLayer]->size() > iSize)? m_iFibSel[iLayer]->size() : iSize;
	}
	iTrackNum = (iSize < MAXTRACKNUM)? iSize : MAXTRACKNUM;
	for (Int_t iTrack=0; iTrack<iTrackNum; iTrack++)
	{
		for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			iFibSel[iTrack][iLayer] = m_iFibSel[iLayer]->at(iTrack);
		}
	}

	iTrackNum=0;
	iSize=0;
	iSize = (m_fOvU->size() <= m_fOvV->size())? m_fOvU->size() : m_fOvV->size();
	iTrackNum = (iSize < MAXTRACKNUM)? iSize : MAXTRACKNUM;
	for (Int_t iTrack=0; iTrack<iTrackNum; iTrack++)
	{
		fOvU[iTrack] = m_fOvU->at(iTrack);
		fOvV[iTrack] = m_fOvV->at(iTrack);
	}

	iTrackNum=0;
	iSize=0;
	iSize = (m_iNU->size() <= m_iNV->size())? m_iNU->size() : m_iNV->size();
	iTrackNum = (iSize < MAXTRACKNUM)? iSize : MAXTRACKNUM;
	for (Int_t iTrack=0; iTrack<iTrackNum; iTrack++)
	{
		iNumU[iTrack] = m_iNU->at(iTrack);
		iNumV[iTrack] = m_iNV->at(iTrack);
	}
}
