/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPTDLOCRECOEVCOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPTDLOCRECOEVCOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_TDLocRecoEvCollection> AfpTDLocRecoEvCollectionFillerBase;

    class AfpTDLocRecoEvCollectionFillerTool : public AfpTDLocRecoEvCollectionFillerBase
    {

    public:
        AfpTDLocRecoEvCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_TDLocRecoEvCollection& DataCollection);

    private:
        void ClearData(const AFP_TDLocRecoEvCollection& DataCollection);

    private:
        int* m_pnTotTracksCnt;
        std::vector<int>* m_pvecTrackStationID;
        std::vector<int>* m_pvecTrackQuarticID;
        std::vector<int>* m_pvecTrackTrainID;
        std::vector<float>* m_pvecTrackTrainTime;
        std::vector<float>* m_pvecTrackTrainAmplitude;
        std::vector<int>* m_pvecTrackSaturatedBars;
    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPTDLOCRECOEVCOLLECTIONFILLERTOOL_H
