/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AlfaHitCollectionFillerTool.h"

namespace D3PD {

	AlfaHitCollectionFillerTool::AlfaHitCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
		: AlfaHitCollectionFillerBase( type, name, parent )
	{
		ATH_MSG_INFO( "AlfaHitCollectionFillerTool---> Constructor" );

		m_pnTotHitsCnt=NULL;
		m_pvecHitID=NULL;
		m_pvecTrackID=NULL;
		m_pvecParticleEncoding=NULL;
		m_pvecKineticEnergy=NULL;
		m_pvecEnergyDeposit=NULL;
		m_pvecPreStepX=NULL;
		m_pvecPreStepY=NULL;
		m_pvecPreStepZ=NULL;
		m_pvecPostStepX=NULL;
		m_pvecPostStepY=NULL;
		m_pvecPostStepZ=NULL;
		m_pvecGlobalTime=NULL;
		m_pvecFiberSign=NULL;
		m_pvecPlateID=NULL;
		m_pvecFiberID=NULL;
		m_pvecStationID=NULL;
	}

	StatusCode AlfaHitCollectionFillerTool::initialize()
	{
		// Initialize the base class:
		CHECK( AlfaHitCollectionFillerBase::initialize() );
		ATH_MSG_INFO( "AlfaHitCollectionFillerTool---> Initialized" );

		return StatusCode::SUCCESS;
	}

	StatusCode AlfaHitCollectionFillerTool::book()
	{
		ATH_MSG_DEBUG( "AlfaHitCollectionFillerTool---> Booking Histograms" );

		CHECK(addVariable("tot_num_hits",m_pnTotHitsCnt));
		CHECK(addVariable("hitID",m_pvecHitID));
		CHECK(addVariable("trackID",m_pvecTrackID));
		CHECK(addVariable("encoding",m_pvecParticleEncoding));
		CHECK(addVariable("kineticE",m_pvecKineticEnergy));
		CHECK(addVariable("depE",m_pvecEnergyDeposit));
		CHECK(addVariable("prestep_x",m_pvecPreStepX));
		CHECK(addVariable("prestep_y",m_pvecPreStepY));
		CHECK(addVariable("prestep_z",m_pvecPreStepZ));
		CHECK(addVariable("poststep_x",m_pvecPostStepX));
		CHECK(addVariable("poststep_y",m_pvecPostStepY));
		CHECK(addVariable("poststep_z",m_pvecPostStepZ));
		CHECK(addVariable("time",m_pvecGlobalTime));
		CHECK(addVariable("fibersign",m_pvecFiberSign));
		CHECK(addVariable("plateID",m_pvecPlateID));
		CHECK(addVariable("fiberID",m_pvecFiberID));
		CHECK(addVariable("stationID",m_pvecStationID));

		return StatusCode::SUCCESS;
	}

	StatusCode AlfaHitCollectionFillerTool::fill(const ALFA_HitCollection& DataCollection)
	{
		ClearData(DataCollection);

		int i;
		ALFA_HitCollection::const_iterator iterColl;

		ClearData(DataCollection);

		*m_pnTotHitsCnt=DataCollection.size();
		for(i=0,iterColl=DataCollection.begin();iterColl!=DataCollection.end();i++,iterColl++)
		{
			(*m_pvecHitID)[i]=iterColl->GetHitID();
			(*m_pvecTrackID)[i]=iterColl->GetTrackID();
			(*m_pvecParticleEncoding)[i]=iterColl->GetParticleEncoding();
			(*m_pvecKineticEnergy)[i]=iterColl->GetKineticEnergy();
			(*m_pvecEnergyDeposit)[i]=iterColl->GetEnergyDeposit();
			(*m_pvecPreStepX)[i]=iterColl->GetPreStepX();
			(*m_pvecPreStepY)[i]=iterColl->GetPreStepY();
			(*m_pvecPreStepZ)[i]=iterColl->GetPreStepZ();
			(*m_pvecPostStepX)[i]=iterColl->GetPostStepX();
			(*m_pvecPostStepY)[i]=iterColl->GetPostStepY();
			(*m_pvecPostStepZ)[i]=iterColl->GetPostStepZ();
			(*m_pvecGlobalTime)[i]=iterColl->GetGlobalTime();
			(*m_pvecFiberSign)[i]=iterColl->GetSignFiber();
			(*m_pvecPlateID)[i]=iterColl->GetPlateNumber();
			(*m_pvecFiberID)[i]=iterColl->GetFiberNumber();
			(*m_pvecStationID)[i]=iterColl->GetStationNumber();
		}

		return StatusCode::SUCCESS;
	}

	void AlfaHitCollectionFillerTool::ClearData(const ALFA_HitCollection& DataCollection)
	{
		int nItemsCnt;

		nItemsCnt=DataCollection.size();
		*m_pnTotHitsCnt=-1;
		m_pvecHitID->resize(nItemsCnt);
		fill_n(m_pvecHitID->begin(),m_pvecHitID->size(),-1);
		m_pvecTrackID->resize(nItemsCnt);
		fill_n(m_pvecTrackID->begin(),m_pvecTrackID->size(),-1);
		m_pvecParticleEncoding->resize(nItemsCnt);
		fill_n(m_pvecParticleEncoding->begin(),m_pvecParticleEncoding->size(),UNDEFINTVALUE);
		m_pvecKineticEnergy->resize(nItemsCnt);
		fill_n(m_pvecKineticEnergy->begin(),m_pvecKineticEnergy->size(),UNDEFFLOATVALUE);
		m_pvecEnergyDeposit->resize(nItemsCnt);
		fill_n(m_pvecEnergyDeposit->begin(),m_pvecEnergyDeposit->size(),UNDEFFLOATVALUE);
		m_pvecPreStepX->resize(nItemsCnt);
		fill_n(m_pvecPreStepX->begin(),m_pvecPreStepX->size(),UNDEFFLOATVALUE);
		m_pvecPreStepY->resize(nItemsCnt);
		fill_n(m_pvecPreStepY->begin(),m_pvecPreStepY->size(),UNDEFFLOATVALUE);
		m_pvecPreStepZ->resize(nItemsCnt);
		fill_n(m_pvecPreStepZ->begin(),m_pvecPreStepZ->size(),UNDEFFLOATVALUE);
		m_pvecPostStepX->resize(nItemsCnt);
		fill_n(m_pvecPostStepX->begin(),m_pvecPostStepX->size(),UNDEFFLOATVALUE);
		m_pvecPostStepY->resize(nItemsCnt);
		fill_n(m_pvecPostStepY->begin(),m_pvecPostStepY->size(),UNDEFFLOATVALUE);
		m_pvecPostStepZ->resize(nItemsCnt);
		fill_n(m_pvecPostStepZ->begin(),m_pvecPostStepZ->size(),UNDEFFLOATVALUE);
		m_pvecGlobalTime->resize(nItemsCnt);
		fill_n(m_pvecGlobalTime->begin(),m_pvecGlobalTime->size(),UNDEFFLOATVALUE);
		m_pvecFiberSign->resize(nItemsCnt);
		fill_n(m_pvecFiberSign->begin(),m_pvecFiberSign->size(),-1);
		m_pvecPlateID->resize(nItemsCnt);
		fill_n(m_pvecPlateID->begin(),m_pvecPlateID->size(),-1);
		m_pvecFiberID->resize(nItemsCnt);
		fill_n(m_pvecFiberID->begin(),m_pvecFiberID->size(),-1);
		m_pvecStationID->resize(nItemsCnt);
		fill_n(m_pvecStationID->begin(),m_pvecStationID->size(),-1);
	}


} // namespace D3PD


