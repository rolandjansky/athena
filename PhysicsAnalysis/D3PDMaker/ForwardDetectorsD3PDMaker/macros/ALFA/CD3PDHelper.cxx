/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TList.h"
#include "TSeqCollection.h"
//#include "TH1F.h"

#include<list>

class CD3PDHelper
{

public:
	static void FilterByDetector(TSeqCollection& Coll, TSeqCollection& FilteredColl, const int nRPid, const int nDetector)
	{
		FilteredColl.Clear();
		TIter iter(&Coll);

		printf("Filter events hiting Detector=%i in RP=%i\r\n",nDetector,nRPid);
		while(iter.Next()){
			CTrackingData* pdata=(CTrackingData*)(*iter);
			if(pdata->HasTrackInDetector(nRPid,nDetector)){
				FilteredColl.Add(pdata);
			}
		}
	}

	static void FilterByDetectorInRPs(TSeqCollection& Coll, TSeqCollection& FilteredColl, const int arrRPid[], const int nRPcnt, const int nDetector)
	{
		bool bRes=true;
		int i;

		FilteredColl.Clear();
		TIter iter(&Coll);

		//printf("Filter events hiting Detector=%i.\r\n",nDetector);
		while(iter.Next()){
			bRes=true;
			CTrackingData* pdata=(CTrackingData*)(*iter);
			for(i=0;i<nRPcnt;i++){
				bRes&=pdata->HasTrackInDetector(arrRPid[i],nDetector);
			}

			if(bRes) FilteredColl.Add(pdata);
		}
	}

	static void GetDataInDetector(TSeqCollection& Coll, list<float>& DataColl, eDataItem eItem, const int nRPid, const int nDetector)
	{
		DataColl.clear();
		TIter iter(&Coll);

		//printf("Fill data hiting Detector=%i in RP=%i.\r\n",nDetector,nRPid);
		while(iter.Next()){
			CTrackingData* pdata=(CTrackingData*)(*iter);
			pdata->FillDataInDetector(DataColl,eItem,nRPid,nDetector);
		}
	}

	static TH1F* CreateHistogram(list<Float_t>& DataColl, const char szName[], const char szTitle[], int nBinCnt=-1, const bool bNormalize=false)
	{
		TH1F* pHist;
		Long64_t nEntries;
		Float_t fVal;
		Float_t fMin=9999, fMax=-9999;

		nEntries=DataColl.size();
		list<Float_t>::iterator iter;
		for(iter=DataColl.begin();iter!=DataColl.end();iter++)
		{
			fVal=(*iter);
			if(fVal>fMax) fMax=fVal;
			if(fVal<fMin) fMin=fVal;
		}

		if(nBinCnt<=0){
			if(nEntries<=100) nBinCnt=10;
			else if(nEntries<2500) nBinCnt=(Long64_t)(0.1*nEntries);
			else nBinCnt=250;
		}

		pHist=new TH1F(szName,szTitle,nBinCnt,fMin,fMax);

		for(iter=DataColl.begin();iter!=DataColl.end();iter++)
		{
			pHist->Fill(*iter);
		}

		if(bNormalize==true){
			pHist->Scale(1.0/pHist->Integral());
		}

		return pHist;
	}

	static void GEComputeDr(TSeqCollection& DataColl, list<float>& listDr, eGoldenEventsBranch eGEBranch)
	{
		const int arrB1RPids[4]={0, 2, 5, 7};
		const int arrB2RPids[4]={1, 3, 4, 6};
		const int* pnRPids=(eGEBranch==EGEB_B1)? arrB1RPids:arrB2RPids;

		listDr.clear();
		TList FDataColl;
		CAlgHelper::FilterByDetectorInRPs(DataColl, FDataColl, pnRPids, 4, 1);

		list<float> listX2, listY2, listX5, listY5;
		list<float>::iterator iterX2,iterY2, iterX5, iterY5;
		CAlgHelper::GetDataInDetector(FDataColl, listX2, EDI_XLHC, (eGEBranch==EGEB_B1)? 2:3, 1);
		CAlgHelper::GetDataInDetector(FDataColl, listY2, EDI_YLHC, (eGEBranch==EGEB_B1)? 2:3, 1);
		CAlgHelper::GetDataInDetector(FDataColl, listX5, EDI_XLHC, (eGEBranch==EGEB_B1)? 5:4, 1);
		CAlgHelper::GetDataInDetector(FDataColl, listY5, EDI_YLHC, (eGEBranch==EGEB_B1)? 5:4, 1);

		//printf("Entries X2: %i\r\n", listX2.size());
		//printf("Entries Y2: %i\r\n", listY2.size());
		//printf("Entries X5: %i\r\n", listX5.size());
		//printf("Entries Y5: %i\r\n", listY5.size());

		int ncnt=listX2.size();

		if(ncnt>0){
			iterX2=listX2.begin(); iterY2=listY2.begin();
			iterX5=listX5.begin(); iterY5=listY5.begin();
			for(int i=0;i<ncnt;i++){
				listDr.push_back(sqrt(((*iterX2)+(*iterX5))*((*iterX2)+(*iterX5))+((*iterY2)+(*iterY5))*((*iterY2)+(*iterY5))));
				iterX2++; iterY2++;
				iterX5++; iterY5++;
			}
		}
	}

	static void ComputeDiffs(TSeqCollection& DataColl, list<float>& listRes, const int nRPid1, const int nRPid2, const int nDetector, eDataItem eItem)
	{
		int* pnRPids=new int[2];
		pnRPids[0]=nRPid1; pnRPids[1]=nRPid2;

		listRes.clear();
		TList FDataColl;
		CAlgHelper::FilterByDetectorInRPs(DataColl, FDataColl, pnRPids, 2, nDetector);

		list<float> listX1, listX2;
		list<float>::iterator iterX1,iterX2;
		CAlgHelper::GetDataInDetector(FDataColl, listX1, eItem, nRPid1, nDetector);
		CAlgHelper::GetDataInDetector(FDataColl, listX2, eItem, nRPid1, nDetector);

		int ncnt=listX1.size();

		if(ncnt>0){
			iterX1=listX1.begin(); iterX2=listX2.begin();
			for(int i=0;i<ncnt;i++){
				listRes.push_back((*iterX2)-(*iterX1));
				iterX1++; iterX2++;
			}
		}

		if(pnRPids) delete [] pnRPids;
	}

	static void ComputeDiffs(TSeqCollection& DataColl1, TSeqCollection& DataColl2, list<float>& listRes, const int nRPid, const int nDetector, eDataItem eItem)
	{
		listRes.clear();
		TList FDataColl1, FDataColl2;
		CAlgHelper::FilterByDetector(DataColl1, FDataColl1, nRPid, nDetector);
		CAlgHelper::FilterByDetector(DataColl2, FDataColl2, nRPid, nDetector);

		list<float> listX1, listX2;
		list<float>::iterator iterX1,iterX2;
		CAlgHelper::GetDataInDetector(FDataColl1, listX1, eItem, nRPid, nDetector);
		CAlgHelper::GetDataInDetector(FDataColl2, listX2, eItem, nRPid, nDetector);

		if(listX1.size()!=listX2.size()){
			printf("Error in difference calculations, different collection size: Cnt1=%i vs. Cnt2=%i\r\n",listX1.size(),listX2.size());
			return;
		}

		int ncnt=listX1.size();

		if(ncnt>0){
			iterX1=listX1.begin(); iterX2=listX2.begin();
			for(int i=0;i<ncnt;i++){
				listRes.push_back((*iterX2)-(*iterX1));
				iterX1++; iterX2++;
			}
		}
	}
};
