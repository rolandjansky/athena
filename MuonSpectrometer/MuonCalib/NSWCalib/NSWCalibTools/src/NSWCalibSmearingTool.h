/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWCalibSmearingTool_h
#define NSWCalibSmearingTool_h


#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"



#include "MuonPrepRawData/MuonCluster.h"



#include <map>

namespace Muon {

  class NSWCalibSmearingTool : virtual public INSWCalibSmearingTool, public AthAlgTool {

  public:

    NSWCalibSmearingTool(const std::string&, const std::string&, const IInterface*);

    virtual ~NSWCalibSmearingTool() {};

    virtual StatusCode initialize() override;

    virtual StatusCode isAccepted(const Identifier id, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const override;

    double getHighVoltage(Identifier id) const;

    StatusCode smearTimeAndCharge(const Identifier id, float& time, float& charge, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const override;
    StatusCode smearCharge(const Identifier id, float& charge, bool& accepted, CLHEP::HepRandomEngine* rndmEngine) const override;

    virtual StatusCode getGainFraction(const Identifier id, float& charge) override;
 
  private:
    
    bool getIdFields(const Identifier id, int& etaSector, int& phiSector,
		     int& gasGap) const;

    bool getPCBIdentifier(const Identifier id, Identifier& pcb_id) const;

    double getMMEfficiencyFromHV(double hv) const;
    double getMMGainFractionFromHV(double hv) const;

    StatusCode readHighVoltagesStatus();

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Gaudi::Property<std::vector<double>> m_timeSmear {this, "TimeSmear" ,{0.,0.,0.,0.,0.,0.,0.,0.}};
    Gaudi::Property<std::vector<double>> m_chargeSmear {this, "ChargeSmear" ,{0.,0.,0.,0.,0.,0.,0.,0.}};

    Gaudi::Property<std::vector<double>> m_channelEfficiency {this, "ChannelEfficiency", {1.,1.,1.,1.,1.,1.,1.,1.}};
    Gaudi::Property<std::vector<double>> m_clusterEfficiency {this, "ClusterEfficiency", {1.,1.,1.,1.,1.,1.,1.,1.}};

    Gaudi::Property<std::vector<double>> m_gainFraction {this, "GainFraction", {1.,1.,1.,1.,1.,1.,1.,1.}};

    Gaudi::Property<std::vector<bool>> m_phiSectors {this, "PhiSectors", {true,true,true,true,true,true,true,true}};
    Gaudi::Property<std::vector<bool>> m_etaSectors {this, "EtaSectors", {}}; // needs to be set via config since it differs for MMs and sTGCs

    Gaudi::Property<bool> m_readEfficiencyFromFile {this, "ReadEfficiencyFromFile", false};
    Gaudi::Property<bool> m_readGainFractionFromFile {this, "ReadGainFractionFromFile", false};
    Gaudi::Property<std::string> m_fileName{this, "FileName", ""};

    // HV maps ( for MM efficiencies )
    std::map<Identifier,float> m_hvMap;

  };

}

#endif
