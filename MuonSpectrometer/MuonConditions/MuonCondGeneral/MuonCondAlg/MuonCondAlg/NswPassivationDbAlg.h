/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_NSWPASSIVATIONDBALG_H
#define MUONCONDALG_NSWPASSIVATIONDBALG_H

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Muon includes
#include "MuonReadoutGeometry/NswPassivationDbData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


// Forward declarations
class CondAttrListCollection;


class NswPassivationDbAlg: public AthReentrantAlgorithm{

public:

    using AthReentrantAlgorithm::AthReentrantAlgorithm;
    virtual ~NswPassivationDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext&) const override;

 
private:

    typedef SG::WriteCondHandleKey<NswPassivationDbData> writeKey_t;
    typedef SG::ReadCondHandleKey<CondAttrListCollection> readKey_t;

	StatusCode loadData(const EventContext& ctx) const;

	Identifier buildChannelId(const int station, const int stationEta, const int stationPhi, const int multiLayer, const int gasGap, const int pcbPos) const;
    
    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
    writeKey_t m_writeKey{this, "WriteKey", "NswPassivationDbData", "Key of output passivation data" };

    readKey_t m_readKey_data_mm{this, "ReadKey_MM", "/MDT/MM/PASSIVATION", "Key of input MM condition data"};
 
};


#endif
