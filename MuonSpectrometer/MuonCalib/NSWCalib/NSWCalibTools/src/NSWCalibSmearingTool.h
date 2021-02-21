/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef NSWCalibSmearingTool_h
#define NSWCalibSmearingTool_h


#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonPrepRawData/MuonCluster.h"

#include "TRandom3.h"

#include <map>

namespace Muon {

  class MuonIdHelperTool;

  class NSWCalibSmearingTool : virtual public INSWCalibSmearingTool, public AthAlgTool {

  public:

    NSWCalibSmearingTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibSmearingTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    StatusCode isAccepted(const Identifier id, bool& accepted);

    double getHighVoltage(Identifier id) const;

    StatusCode smearTimeAndCharge(const Identifier id, float& time, float& charge, bool& accepted);
    StatusCode smearCharge(const Identifier id, float& charge, bool& accepted);

    StatusCode getGainFraction(const Identifier id, float& charge);

  private:
    
    bool getIdFields(const Identifier id, int& etaSector, int& phiSector,
		     int& gasGap);

    bool getPCBIdentifier(const Identifier id, Identifier& pcb_id) const;

    double getMMEfficiencyFromHV(double hv) const;
    double getMMGainFractionFromHV(double hv) const;

    StatusCode readHighVoltagesStatus();

    ToolHandle<MuonIdHelperTool> m_idHelperTool;

    DoubleArrayProperty m_timeSmear;
    DoubleArrayProperty m_chargeSmear;

    DoubleArrayProperty m_channelEfficiency;
    DoubleArrayProperty m_clusterEfficiency;

    DoubleArrayProperty m_gainFraction;

    BooleanArrayProperty m_phiSectors;
    BooleanArrayProperty m_etaSectors;

    BooleanProperty m_readEfficiencyFromFile;
    BooleanProperty m_readGainFractionFromFile;
    StringProperty m_fileName;

    TRandom3 m_random;

    // HV maps ( for MM efficiencies )
    std::map<Identifier,float> m_hvMap; 

  };

}

#endif
