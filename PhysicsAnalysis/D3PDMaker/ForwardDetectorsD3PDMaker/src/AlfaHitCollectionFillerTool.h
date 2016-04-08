/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_ALFAHITCOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ALFAHITCOLLECTIONFILLERTOOL_H

#include "AlfaCommonHdr.h"
#include "ALFA_SimEv/ALFA_HitCollection.h"

namespace D3PD
{
	typedef D3PD::BlockFillerTool<ALFA_HitCollection> AlfaHitCollectionFillerBase;

	class AlfaHitCollectionFillerTool : public AlfaHitCollectionFillerBase
	{

	public:
		AlfaHitCollectionFillerTool(const std::string& type, const std::string& name, const IInterface* parent);

		virtual StatusCode initialize();
		virtual StatusCode book();
		virtual StatusCode fill(const ALFA_HitCollection& DataCollection);

	private:
		void ClearData(const ALFA_HitCollection& DataCollection);

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

		vector<int>* m_pvecFiberSign;
		vector<int>* m_pvecPlateID;
		vector<int>* m_pvecFiberID;
		vector<int>* m_pvecStationID;
	};
}

#endif // FORWARDDETECTORSD3PDMAKER_ALFAHITCOLLECTIONFILLERTOOL_H
