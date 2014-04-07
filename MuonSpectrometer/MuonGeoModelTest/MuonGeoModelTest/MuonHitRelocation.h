/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_HIT_RELOCATION_H
#define MUON_HIT_RELOCATION_H

#include "GaudiKernel/Algorithm.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"

// Histograms quantities related to G4 Hits in the Atlas
// Detector.  Used to validate GeoModel + much more.


class MuonHitRelocation: public Algorithm {
public:

    MuonHitRelocation(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonHitRelocation();
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();
    long getIntStName(std::string stName) const;

private:

    bool _checkMdt;
    bool _checkRpc;
    bool _checkTgc;
    bool _checkCsc;
    bool _checksTgc;
    bool _checkMM;

    
    class Clockwork;
    Clockwork *c;
    RpcHitIdHelper* rmuonHelper;
    MdtHitIdHelper* mmuonHelper;
    TgcHitIdHelper* tmuonHelper;
    CscHitIdHelper* cmuonHelper;
    sTgcHitIdHelper* stmuonHelper;
    MicromegasHitIdHelper* mmmuonHelper;
    
};

#endif



