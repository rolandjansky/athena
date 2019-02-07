/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonExtraDetails_p1.h
/// 
/// Declaration of persistent class
/// for Analysis::TauCommonExtraDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonExtraDetails_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAUCOMMONEXTRADETAILS_P1_H
#define TAUEVENTTPCNV_TAUCOMMONEXTRADETAILS_P1_H

//Includes
#include <vector>
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

//Forward declarations
class TauCommonExtraDetailsCnv_p1;

/// Persistent class for Analysis::TauCommonExtraDetails
class TauCommonExtraDetails_p1
{
    friend class TauCommonExtraDetailsCnv_p1;
public:
    /// Default constructor
    ///
    /// Make sure that members added to this class since the first version
    /// get initialized.  (Otherwise, when reading an old version, they'll
    /// be left uninitialized.)
    TauCommonExtraDetails_p1();

    /// Destructor
    ~TauCommonExtraDetails_p1() = default;
    
private:
    ///@copydoc Analysis::TauCommonExtraDetails::m_sumPtLooseTrk
    float m_sumPtLooseTrk;

    ///@copydoc Analysis::TauCommonExtraDetails::m_sumPtTrk
    float m_sumPtTrk;

    ///@copydoc Analysis::TauCommonExtraDetails::m_closestEtaTrkVertCell
    std::vector<ElementLinkInt_p2> m_closestEtaTrkVertCell;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_closestPhiTrkVertCell
    std::vector<ElementLinkInt_p2> m_closestPhiTrkVertCell;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_closestEtaTrkCell
    std::vector<ElementLinkInt_p2> m_closestEtaTrkCell;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_closestPhiTrkCell
    std::vector<ElementLinkInt_p2> m_closestPhiTrkCell;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_etaTrkCaloSamp
    std::vector<float> m_etaTrkCaloSamp;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_phiTrkCaloSamp
    std::vector<float> m_phiTrkCaloSamp;
        
    ///@copydoc Analysis::TauCommonExtraDetails::m_etaLooseTrkCaloSamp
    std::vector<float> m_etaLooseTrkCaloSamp;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_phiLooseTrkCaloSamp
    std::vector<float>  m_phiLooseTrkCaloSamp;

    ///@copydoc Analysis::TauCommonExtraDetails::m_seedCalo_nEMCell
    int m_seedCalo_nEMCell;

    ///@copydoc Analysis::TauCommonExtraDetails::m_seedCalo_stripEt
    float m_seedCalo_stripEt;

    ///@copydoc Analysis::TauCommonExtraDetails::m_seedCalo_EMCentFrac
    float m_seedCalo_EMCentFrac;

    ///@copydoc Analysis::TauCommonExtraDetails::m_seedCalo_sumCellEnergy
    float m_seedCalo_sumCellEnergy;
	
    ///@copydoc Analysis::TauCommonExtraDetails::m_seedCalo_sumEMCellEnergy
    float m_seedCalo_sumEMCellEnergy;

    ///Element link container name lookup table
    ElementLinkContNames_p2 m_linkNames;

    ///Element link container name lookup table
    unsigned short int m_tracks;
    
    ///Element link container name lookup table
    unsigned short int m_looseTracks;
}; 

#endif // TAUEVENTTPCNV_TAUCOMMONEXTRADETAILS_P1_H
