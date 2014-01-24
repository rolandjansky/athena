/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPTDDIGICOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPTDDIGICOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_TDDigiCollection> AfpTDDigiCollectionFillerBase;

    class AfpTDDigiCollectionFillerTool : public AfpTDDigiCollectionFillerBase
    {

    public:
        AfpTDDigiCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_TDDigiCollection& DataCollection);

    private:
        void ClearData();

    private:

        //SiDigiCollection
        //vector<float>* m_pvecXDetCS;
        //int* m_pnMaxTrackCnt;

    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPTDDIGICOLLECTIONFILLERTOOL_H
