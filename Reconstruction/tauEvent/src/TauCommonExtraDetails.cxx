/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCommonExtraDetails.cxx
///
///Implementation of details class for tau event
///
///Package: Reconstruction/TauEvent
///
///@author Thomas Burgess
///
/// Created by Thomas Burgess on 2008-10-20.

//TauEvent includes
#include "tauEvent/TauCmp.h"
#include "tauEvent/TauCommonExtraDetails.h"
//Other includes
#include "CaloEvent/CaloCellContainer.h"
#include "DataModel/ClassName.h"
#include "DataModel/ElementLink.h"
//C/C++ includes
#include <string>
#include <vector>

namespace Analysis {

    //Default constructor
    //Initializes members to default values where applicable
    TauCommonExtraDetails :: TauCommonExtraDetails():
        TauDetails(),
        //Applicable for track & calo seeded tau reconstruction
        m_sumPtLooseTrk(),
        m_sumPtTrk(),
        //Applicable for calo seeded tau reconstruction
        m_seedCalo_nEMCell(),
        m_seedCalo_stripEt(),
        m_seedCalo_EMCentFrac(),
        m_seedCalo_sumCellEnergy(),
        m_seedCalo_sumEMCellEnergy()
    {
        allocTracksAndSamplings( 10, 4 );
    }

    //Copy constructor
    TauCommonExtraDetails :: TauCommonExtraDetails(
            const TauCommonExtraDetails &rhs ):
        TauDetails(),
        //Applicable for track & calo seeded tau reconstruction
        m_sumPtLooseTrk(rhs.m_sumPtLooseTrk),
        m_sumPtTrk(rhs.m_sumPtTrk),
        m_closestEtaTrkVertCell(rhs.m_closestEtaTrkVertCell),
        m_closestPhiTrkVertCell(rhs.m_closestPhiTrkVertCell),
        m_closestEtaTrkCell(rhs.m_closestEtaTrkCell),
        m_closestPhiTrkCell(rhs.m_closestPhiTrkCell),
        m_etaTrkCaloSamp(rhs.m_etaTrkCaloSamp),
        m_phiTrkCaloSamp(rhs.m_phiTrkCaloSamp),
        m_etaLooseTrkCaloSamp(rhs.m_etaLooseTrkCaloSamp),
        m_phiLooseTrkCaloSamp(rhs.m_phiLooseTrkCaloSamp),
        //Applicable for calo seeded tau reconstruction
        m_seedCalo_nEMCell(rhs.m_seedCalo_nEMCell),
        m_seedCalo_stripEt(rhs.m_seedCalo_stripEt),
        m_seedCalo_EMCentFrac(rhs.m_seedCalo_EMCentFrac),
        m_seedCalo_sumCellEnergy(rhs.m_seedCalo_sumCellEnergy),
        m_seedCalo_sumEMCellEnergy(rhs.m_seedCalo_sumEMCellEnergy)
    {
    }

    //Destructor
    TauCommonExtraDetails :: ~TauCommonExtraDetails()
    {
    }

    //Assignment operator
    TauCommonExtraDetails &TauCommonExtraDetails :: operator = (
            const TauCommonExtraDetails &rhs )
    {
        if( this != &rhs )
        {
            m_sumPtLooseTrk=rhs.m_sumPtLooseTrk;
            m_sumPtTrk=rhs.m_sumPtTrk;
            m_closestEtaTrkVertCell=rhs.m_closestEtaTrkVertCell;
            m_closestPhiTrkVertCell=rhs.m_closestPhiTrkVertCell;
            m_closestEtaTrkCell=rhs.m_closestEtaTrkCell;
            m_closestPhiTrkCell=rhs.m_closestPhiTrkCell;
            m_etaTrkCaloSamp=rhs.m_etaTrkCaloSamp;
            m_phiTrkCaloSamp=rhs.m_phiTrkCaloSamp;
            m_etaLooseTrkCaloSamp=rhs.m_etaLooseTrkCaloSamp;
            m_phiLooseTrkCaloSamp=rhs.m_phiLooseTrkCaloSamp;
            //Applicable for calo seeded tau reconstruction
            m_seedCalo_nEMCell=rhs.m_seedCalo_nEMCell;
            m_seedCalo_stripEt=rhs.m_seedCalo_stripEt;
            m_seedCalo_EMCentFrac=rhs.m_seedCalo_EMCentFrac;
            m_seedCalo_sumCellEnergy=rhs.m_seedCalo_sumCellEnergy;
            m_seedCalo_sumEMCellEnergy=rhs.m_seedCalo_sumEMCellEnergy;
        }
        return *this;
    }

    bool TauCommonExtraDetails::operator==(
            const TauCommonExtraDetails& rhs ) const
    {
        //Double comparisons
        if (!TauCmp::cmp(m_sumPtLooseTrk,rhs.m_sumPtLooseTrk))
            return false;
        if (!TauCmp::cmp(m_sumPtTrk,rhs.m_sumPtTrk))
            return false;
        if (!TauCmp::cmp(m_seedCalo_stripEt,rhs.m_seedCalo_stripEt))
            return false;
        if (!TauCmp::cmp(m_seedCalo_EMCentFrac,rhs.m_seedCalo_EMCentFrac))
            return false;
        if (!TauCmp::cmp(m_seedCalo_sumCellEnergy,rhs.m_seedCalo_sumCellEnergy))
            return false;
        if (!TauCmp::cmp(m_seedCalo_sumEMCellEnergy,
                    rhs.m_seedCalo_sumEMCellEnergy))
            return false;
        //Interger comparisons
        if ( m_seedCalo_nEMCell != rhs.m_seedCalo_nEMCell) return false;
        //Vector vector double comparisons
        if (!TauCmp::vvcmp(m_etaTrkCaloSamp,rhs.m_etaTrkCaloSamp))
            return false;
        if (!TauCmp::vvcmp(m_phiTrkCaloSamp,rhs.m_phiTrkCaloSamp))
            return false;
        if (!TauCmp::vvcmp(m_etaLooseTrkCaloSamp,rhs.m_etaLooseTrkCaloSamp))
            return false;
        if (!TauCmp::vvcmp(m_phiLooseTrkCaloSamp,rhs.m_phiLooseTrkCaloSamp))
            return false;
        //Vector vector Element links
        if (!TauCmp::vvcmp(m_closestEtaTrkVertCell,rhs.m_closestEtaTrkVertCell))
            return false;
        if (!TauCmp::vvcmp(m_closestPhiTrkVertCell,rhs.m_closestPhiTrkVertCell))
            return false;
        if (!TauCmp::vvcmp(m_closestEtaTrkCell,rhs.m_closestEtaTrkCell))
            return false;
        if (!TauCmp::vvcmp(m_closestPhiTrkCell,rhs.m_closestPhiTrkCell))
            return false;
        return true;
    }

    void TauCommonExtraDetails::allocTracksAndSamplings(
            const unsigned int tracks,
            const unsigned int samplings )
    {
        if ( tracks == 0 || samplings == 0 )
            return;
        std::vector<ElementLink<CaloCellContainer> > initEL( samplings );
        m_closestEtaTrkVertCell.clear();
        m_closestEtaTrkVertCell.resize( tracks, initEL );
        m_closestEtaTrkCell.clear();
        m_closestEtaTrkCell.resize( tracks, initEL );
        m_closestPhiTrkVertCell.clear();
        m_closestPhiTrkVertCell.resize( tracks, initEL );
        m_closestPhiTrkCell.clear();
        m_closestPhiTrkCell.resize( tracks, initEL );
        std::vector<double> initD( samplings, TauDetails::DEFAULT );
        m_etaTrkCaloSamp.clear();
        m_etaTrkCaloSamp.resize( tracks, initD );
        m_phiTrkCaloSamp.clear();
        m_phiTrkCaloSamp.resize( tracks, initD );
        m_etaLooseTrkCaloSamp.clear();
        m_etaLooseTrkCaloSamp.resize( tracks, initD );
        m_phiLooseTrkCaloSamp.clear();
        m_phiLooseTrkCaloSamp.resize( tracks, initD );
    }

    //Class name
    std::string TauCommonExtraDetails::s_className =
        ClassName<TauCommonExtraDetails>::name();

    const std::string& TauCommonExtraDetails::className() const
    {
        return s_className;
    }

} //Namespace analysis
