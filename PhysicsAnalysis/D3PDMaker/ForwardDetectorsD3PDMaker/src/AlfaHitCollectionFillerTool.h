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
		std::vector<int>* m_pvecHitID;
		std::vector<int>* m_pvecTrackID;
		std::vector<int>* m_pvecParticleEncoding;
		std::vector<float>* m_pvecKineticEnergy;
		std::vector<float>* m_pvecEnergyDeposit;
		std::vector<float>* m_pvecPreStepX;
		std::vector<float>* m_pvecPreStepY;
		std::vector<float>* m_pvecPreStepZ;
		std::vector<float>* m_pvecPostStepX;
		std::vector<float>* m_pvecPostStepY;
		std::vector<float>* m_pvecPostStepZ;
		std::vector<float>* m_pvecGlobalTime;

		std::vector<int>* m_pvecFiberSign;
		std::vector<int>* m_pvecPlateID;
		std::vector<int>* m_pvecFiberID;
		std::vector<int>* m_pvecStationID;
	};
}

#endif // FORWARDDETECTORSD3PDMAKER_ALFAHITCOLLECTIONFILLERTOOL_H
