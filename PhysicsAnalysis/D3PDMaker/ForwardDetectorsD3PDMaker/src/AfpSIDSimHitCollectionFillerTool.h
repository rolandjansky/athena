/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_SIDSimHitCollection> AfpSIDSimHitCollectionFillerBase;

    class AfpSIDSimHitCollectionFillerTool : public AfpSIDSimHitCollectionFillerBase
    {

    public:
        AfpSIDSimHitCollectionFillerTool(const std::string& type, const std::string& name, const IInterface* parent);

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_SIDSimHitCollection& DataCollection);

    private:
        void ClearData(const AFP_SIDSimHitCollection& DataCollection);

    private:
		int* m_pnTotHitsCnt;
		vector<int>* m_pvecHitID;
		vector<int>* m_pvecTrackID;
		vector<int>* m_pvecParticleEncoding;
		vector<float>* m_pvecKineticEnergy;
		vector<float>* m_pvecEnergyDeposit;
		vector<float>* m_pvecPreStepX;
		vector<float>* m_pvecPreStepY;
		vector<float>* m_pvecPreStepZ;
		vector<float>* m_pvecPostStepX;
		vector<float>* m_pvecPostStepY;
		vector<float>* m_pvecPostStepZ;
		vector<float>* m_pvecGlobalTime;
		vector<int>* m_pvecStationID;
		vector<int>* m_pvecDetectorID;
		vector<int>* m_pvecIsAuxVSID;
		vector<int>* m_pvecPixelRow;
		vector<int>* m_pvecPixelCol;
    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H
