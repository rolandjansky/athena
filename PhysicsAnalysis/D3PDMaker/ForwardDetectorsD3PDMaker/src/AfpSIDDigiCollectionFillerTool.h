/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPSIDDIGICOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPSIDDIGICOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_SiDigiCollection> AfpSIDDigiCollectionFillerBase;

    class AfpSIDDigiCollectionFillerTool : public AfpSIDDigiCollectionFillerBase
    {

    public:
        AfpSIDDigiCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent );

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_SiDigiCollection& DataCollection);

    private:
        int GetNumOfItems(const AFP_SiDigiCollection& DataCollection, const enum eSIDCountingLevel eCntLevel);
        void ClearData(const AFP_SiDigiCollection& DataCollection);

    private:
        //items per hit pixels
        int* m_pnTotPixelsCnt;
        std::vector<int>* m_pvecPixStationIDs;
        std::vector<int>* m_pvecPixLayerIDs;
        std::vector<int>* m_pvecPixRow;
        std::vector<int>* m_pvecPixColumn;
        std::vector<float>* m_pvecPixEAmp;

        //items per hit layers
        int* m_pnTotLayersCnt;
        std::vector<int>* m_pvecLarStationIDs;
        std::vector<int>* m_pvecLarLayerIDs;
        std::vector<int>* m_pvecLarHitsPerLayer;
        std::vector<float>* m_pvecLarEAmpPerLayer;

        //items per hit stations
        int* m_pnTotStationsCnt;
        std::vector<int>* m_pvecStaStationIDs;
        std::vector<int>* m_pvecStaHitsPerStation;
        std::vector<float>* m_pvecStaEAmpPerStation;
    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPSIDDIGICOLLECTIONFILLERTOOL_H
