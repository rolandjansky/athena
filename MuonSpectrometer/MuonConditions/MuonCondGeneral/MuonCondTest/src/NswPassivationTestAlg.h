/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NswPassivationTestAlg_H
#define NswPassivationTestAlg_H

// STL
#include <chrono>
#include <string>

// Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonReadoutGeometry/NswPassivationDbData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

// Forward declarations
class ISvcLocator;
class StatusCode;

class NswPassivationTestAlg : public AthReentrantAlgorithm {
public:
    NswPassivationTestAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~NswPassivationTestAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &) const override;

private:
    StatusCode retrieve(const EventContext &, std::chrono::duration<double> &) const;
    std::string timestamp() const;

    SG::ReadCondHandleKey<NswPassivationDbData> m_readKey{
        this, "ReadKey", "NswPassivationDbData",
        "Key of NswPassivationDbData object containing passivation data"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};  // end of class

#endif
