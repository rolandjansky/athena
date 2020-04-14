/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWCalibSmearingTool_h
#define NSWCalibSmearingTool_h


#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonPrepRawData/MuonCluster.h"

#include "TRandom3.h"


namespace Muon {

  class NSWCalibSmearingTool : virtual public INSWCalibSmearingTool, public AthAlgTool {

  public:

    NSWCalibSmearingTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibSmearingTool() {};

    virtual StatusCode initialize();

    StatusCode isAccepted(const Identifier id, bool& accepted);

    StatusCode smearTimeAndCharge(const Identifier id, float& time, float& charge, bool& accepted);
    StatusCode smearCharge(const Identifier id, float& charge, bool& accepted);

    StatusCode getGainFraction(const Identifier id, float& charge);

  private:
    
    bool getIdFields(const Identifier id, int& etaSector, int& phiSector, int& gasGap);

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    DoubleArrayProperty m_timeSmear;
    DoubleArrayProperty m_chargeSmear;

    DoubleArrayProperty m_channelEfficiency;
    DoubleArrayProperty m_clusterEfficiency;

    DoubleArrayProperty m_gainFraction;


    BooleanArrayProperty m_phiSectors;
    BooleanArrayProperty m_etaSectors;

    TRandom3 m_random;

  };

}

#endif
