/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCommonDetailsMerger.h
///Definition of TauCommonDetailsMerger
///
///Package: PhysicsAnalysis/TauID/TauTools
///
///@author Thomas Burgess <tburgess-at-cern-ch>
///
///@date 2009-03-30

#ifndef __TauCommonDetailsMerger_h__
#define __TauCommonDetailsMerger_h__

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

/// Athena algorithm that retrieves TauJets and merges old details 
/// (Tau1P3P/TauRec (extra)Details) into new details (Common(Extra)Details)
///
/// This algorithm has one parameter,
/// TauContainerName - name of tau container in storegate, 
/// it's default value is (string) TauRecContainer
class TauCommonDetailsMerger : public AthAlgorithm {

public:
    ///Constructor
    TauCommonDetailsMerger(
	const std::string& name,  
	ISvcLocator* svcLocator);

    ///Destructor
    virtual ~TauCommonDetailsMerger();

    ///Before event loop    
    ///@return StatusCode
    virtual StatusCode initialize();

    ///For each event
    ///@return StatusCode
    virtual StatusCode execute();

    ///Post event loop
    ///@return StatusCode
    virtual StatusCode finalize();

private:
    
    ///Name of tau container in StoreGate
    std::string m_tauContainerName;

    ///Name of tau details container in StoreGate
    std::string m_tauDetailsContainerName;

    ///Name of tau extra details container in StoreGate
    std::string m_tauExtraDetailsContainerName;
    
    bool m_skipExtraDetails;
};

#endif //__TauCommonDetailsMerger_h__
