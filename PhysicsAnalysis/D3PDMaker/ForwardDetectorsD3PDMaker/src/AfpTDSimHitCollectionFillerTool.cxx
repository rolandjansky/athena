/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpTDSimHitCollectionFillerTool.h"

namespace D3PD {

	AfpTDSimHitCollectionFillerTool::AfpTDSimHitCollectionFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
		: AfpTDSimHitCollectionFillerBase( type, name, parent )
    {
		ATH_MSG_INFO( "AfpTDSimHitCollectionFillerTool---> Constructor" );

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
		m_pvecStationID=NULL;
		m_pvecDetectorID=NULL;
		m_pvecSensitiveElementID=NULL;
    }

	StatusCode AfpTDSimHitCollectionFillerTool::initialize()
    {
        // Initialize the base class:
		CHECK( AfpTDSimHitCollectionFillerBase::initialize() );
		ATH_MSG_INFO( "AfpTDSimHitCollectionFillerTool---> Initialized" );

        return StatusCode::SUCCESS;
    }

	StatusCode AfpTDSimHitCollectionFillerTool::book()
    {
		ATH_MSG_DEBUG( "AfpTDSimHitCollectionFillerTool---> Booking Histograms" );

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
		CHECK(addVariable("stID",m_pvecStationID));
		CHECK(addVariable("layerID",m_pvecDetectorID));
		CHECK(addVariable("senselID",m_pvecSensitiveElementID));

        return StatusCode::SUCCESS;
    }

	StatusCode AfpTDSimHitCollectionFillerTool::fill(const AFP_TDSimHitCollection& DataCollection)
    {
        ClearData(DataCollection);

		int i;
		AFP_TDSimHitCollection::const_iterator iterColl;

		ClearData(DataCollection);

		*m_pnTotHitsCnt=DataCollection.size();
		for(i=0,iterColl=DataCollection.begin();iterColl!=DataCollection.end();i++,iterColl++)
		{
			(*m_pvecHitID)[i]=iterColl->m_nHitID;
			(*m_pvecTrackID)[i]=iterColl->m_nTrackID;
			(*m_pvecParticleEncoding)[i]=iterColl->m_nParticleEncoding;
			(*m_pvecKineticEnergy)[i]=iterColl->m_fKineticEnergy;
			(*m_pvecEnergyDeposit)[i]=iterColl->m_fEnergyDeposit;
			(*m_pvecPreStepX)[i]=iterColl->m_fPreStepX;
			(*m_pvecPreStepY)[i]=iterColl->m_fPreStepY;
			(*m_pvecPreStepZ)[i]=iterColl->m_fPreStepZ;
			(*m_pvecPostStepX)[i]=iterColl->m_fPostStepX;
			(*m_pvecPostStepY)[i]=iterColl->m_fPostStepY;
			(*m_pvecPostStepZ)[i]=iterColl->m_fPostStepZ;
			(*m_pvecGlobalTime)[i]=iterColl->m_fGlobalTime;
			(*m_pvecStationID)[i]=iterColl->m_nStationID;
			(*m_pvecDetectorID)[i]=iterColl->m_nDetectorID;
			(*m_pvecSensitiveElementID)[i]=iterColl->m_nSensitiveElementID;
		}

        return StatusCode::SUCCESS;
    }

	void AfpTDSimHitCollectionFillerTool::ClearData(const AFP_TDSimHitCollection& DataCollection)
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
		m_pvecStationID->resize(nItemsCnt);
		fill_n(m_pvecStationID->begin(),m_pvecStationID->size(),-1);
		m_pvecDetectorID->resize(nItemsCnt);
		fill_n(m_pvecDetectorID->begin(),m_pvecDetectorID->size(),-1);
		m_pvecSensitiveElementID->resize(nItemsCnt);
		fill_n(m_pvecSensitiveElementID->begin(),m_pvecSensitiveElementID->size(),-1);

    }


} // namespace D3PD

