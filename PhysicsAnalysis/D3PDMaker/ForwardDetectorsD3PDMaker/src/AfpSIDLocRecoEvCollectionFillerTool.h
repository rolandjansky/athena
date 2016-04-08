/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPSIDLORECOEVCOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPSIDLORECOEVCOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_SIDLocRecoEvCollection> AfpSIDLocRecoEvCollectionFillerBase;

    class AfpSIDLocRecoEvCollectionFillerTool : public AfpSIDLocRecoEvCollectionFillerBase
    {

    public:
        AfpSIDLocRecoEvCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_SIDLocRecoEvCollection& DataCollection);

    private:
        void ClearData(const AFP_SIDLocRecoEvCollection& DataCollection);

    private:
        int* m_pnTotTracksCnt;
        vector<int>* m_pvecTrackStationID;
        vector<float>* m_pvecTrackXPos;
        vector<float>* m_pvecTrackYPos;
        vector<float>* m_pvecTrackZPos;
        vector<float>* m_pvecTrackXSlope;
        vector<float>* m_pvecTrackYSlope;
        vector<int>* m_pvecTrackUsedPixels;
        vector<int>* m_pvecTrackHoles;
        vector<float>* m_pvecTrackQuality;
    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPSIDLORECOEVCOLLECTIONFILLERTOOL_H
