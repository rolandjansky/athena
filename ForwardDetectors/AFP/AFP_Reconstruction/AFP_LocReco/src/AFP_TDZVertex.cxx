/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDZVertex.h"

AFP_TDZVertex::AFP_TDZVertex()
{
	m_listResults.clear();

	m_AmpThresh = 0.;
	m_iDataType = 0;

}

AFP_TDZVertex::~AFP_TDZVertex()
{
	m_listResults.clear();
}

StatusCode AFP_TDZVertex::Initialize(float fAmpThresh, int iDataType)
{
	m_AmpThresh = (float)fAmpThresh;
	m_iDataType = iDataType;
	
	return StatusCode::SUCCESS;
}

StatusCode AFP_TDZVertex::Execute(const std::list<TDHIT> &ListTDHits)
{

	
	const int NST = 4;
	const int NDET = 4;
	const int NTRAINS = 4;
	//const int NBARS = 32;
	
        std::vector<float> vecTDHit[NST][NDET][NTRAINS];
	
	for (int i=0; i<NST; i++)
	{
		for (int j=0; j<NDET; j++)
		{
			for (int k=0; k<NTRAINS; k++)
			{	
			 vecTDHit[i][j][k].clear();
			}
		}
	}

	
        std::list<TDHIT>::const_iterator iter;
	for (iter=ListTDHits.begin(); iter!=ListTDHits.end(); iter++)
	{
		if ((*iter).fADC > m_AmpThresh)
		{	
			//reco time + position correction + saturation
			//cout<<"qqq "<<(*iter).nStationID<<"\t"<<(*iter).nSensitiveElementID<<"\t"<<((*iter).nDetectorID)%4<<endl;
			if ((*iter).fADC < 100) vecTDHit[(*iter).nStationID][(*iter).nSensitiveElementID][((*iter).nDetectorID)%4].push_back( (*iter).fTDC + int(((*iter).nDetectorID)/4)*0.001 );
                        else { 
                              vecTDHit[(*iter).nStationID][(*iter).nSensitiveElementID][((*iter).nDetectorID)%4].push_back(0.0);
                             }
		}
	}



	for (int i=0; i<NST; i++)
	{
		for (int j=0; j<NDET; j++)
		{
			for (int k=0; k<NTRAINS; k++)
			{	
				int TrSize = vecTDHit[i][j][k].size();
				float TrTime = 0.;
				int TrSat = 0;
				if ( TrSize>0 )
				{
					for (int l=0; l<TrSize; l++)
					{
						TrTime += (vecTDHit[i][j][k].at(l));
                                                if ( vecTDHit[i][j][k].at(l)==0.0) TrSat++;
					}
					// time average
					if( TrSize!=TrSat) TrTime /= (TrSize-TrSat);					
					
					TDRESULT Results;
					if (1)
					{
						Results.nStationID  = i;
						Results.nDetectorID = j;
						Results.nTrainID    = k;
						Results.fTrainTime  = TrTime;
						Results.nTrainSize  = TrSize;
                                                Results.nTrainNSat  = TrSat;
   						m_listResults.push_back(Results);
					}
					
				}
			}
		}
	}




	return StatusCode::SUCCESS;
}

StatusCode AFP_TDZVertex::Finalize(std::list<TDRESULT>* pListResults)
{
	*pListResults = m_listResults;

	return StatusCode::SUCCESS;
}



void AFP_TDZVertex::GetData()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDZVertex::GetData()");
	LogStream << MSG::DEBUG << "begin AFP_TDZVertex::GetData()" << endmsg;

	/////
	/////

	LogStream << MSG::DEBUG << "end AFP_TDZVertex::GetData()" << endmsg;
}
