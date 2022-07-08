//#include "stdafx.h"
#include "McTrack.h"
#include "SiCluster.h"
#include "PixelCluster.h"
#include "SCT_Cluster.h"
#include "SCT_BarrelCluster.h"
#include "SCT_EndCapCluster.h"
#include "SpacePoint.h"
#include "Surface.h"
#include "RecTrack.h"
#include "EventData.h"
#include "FileReader.h"
#include <map>
#include <iostream>
#include <fstream>

int FileReader::readEvents(std::ifstream& input, std::vector<EventData*>& events)
{
    int dataTag, eventNumber, Np, Np2;
    float par[6];
    EventData* pEvent=NULL;
    //events.clear();

    while(input.good())
    {
        input >> dataTag;
        if (dataTag==11)
        {
            // MC info
            input >> eventNumber;
            input >> par[0] >> par[1] >> par[2] >> par[3] >> par[4] >> par[5] >> Np >> Np2;
            McTrack* pMcTrack = new McTrack(par);
            for(int i=0;i<Np;i++)
            {
                input >> par[0] >> par[1] >> par[2] >> par[3];
                pMcTrack->addBremPoint(par[0], par[1], par[2], par[3]);
            }
            if(!pEvent) pEvent = new EventData();
            pEvent->setEventNumber(eventNumber);
            pEvent->setMcTrack(pMcTrack);
            events.push_back(pEvent);
            //if(events.size()==5) break;
            pEvent=NULL;
        }
        else if (dataTag==22)
        {
            // track candidate
            if(!pEvent) pEvent = new EventData();
            int nTracks;
            input >> nTracks;
            for(int k=0;k<nTracks;k++)
            {
                int AlgoId, nSP;
                input >> AlgoId;
                if(AlgoId==0) continue;
                RecTrack* pTrack = new RecTrack(AlgoId);
                input >> nSP;
                input >> par[0] >> par[1] >> par[2] >> par[3] >> par[4];
                pTrack->setInitialParameters(par);
                float x0,y0,z0=par[1];
                input >> par[0] >> par[1] >> par[2];
                pTrack->addBFieldPoint(0.0,0.0,z0,par);
                for(int iSP=0;iSP<nSP;iSP++)
                {
                    input >> x0 >> y0 >> z0;
                    input >> par[0] >> par[1] >> par[2];
                    pTrack->addBFieldPoint(x0,y0,z0,par);
                    int nCL;
                    input >> nCL;
                    SpacePoint* pSP = new SpacePoint(x0,y0,z0);
                    for(int iCL=0;iCL<nCL;iCL++)
                    {
                        input >> par[0] >> par[1] >> par[2] >> par[3] >> par[4] >> par[5];
                        Surface* pS = new Surface(par);
                        for(int nRow=0;nRow<3;nRow++)
                        {
                            input >> par[0] >> par[1] >> par[2];
                            pS->addRow(nRow,par);
                        }
						
                        if(nCL==1)
                        {
                            SiCluster* pCL = new PixelCluster(pS);
                            input >> par[0] >> par[1] >> par[2] >> par[3];
                            pCL->setParameters(par);
                            pSP->addCluster(pCL);
                        }
                        else
                        {
                            int sctType;
                            input >> sctType;
                            if(sctType==0)
                            {
                                SiCluster* pCL = new SCT_BarrelCluster(pS);
                                input >> par[0] >> par[1];
                                pCL->setParameters(par);
                                pSP->addCluster(pCL);
                            }
                            else if(sctType==1)
                            {
                                SiCluster* pCL = new SCT_EndCapCluster(pS);
                                input >> par[0] >> par[1] >> par[2];
                                pCL->setParameters(par);
                                pSP->addCluster(pCL);
                            }
                        }
                    }
                    pTrack->addSpacePoint(pSP);
                }
                //pTrack->sortSpacePoints();
                pEvent->m_tracks.push_back(pTrack);
            }
        }
    }
	
    return (int)events.size();
}



