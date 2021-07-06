/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_RT_RELATION_DIGITOOL_H
#define MDT_DIGITIZATION_RT_RELATION_DIGITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Random/RandFlat.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"
#include "MDT_Digitization/MdtDigiToolOutput.h"
#include "MDT_Digitization/r_to_t_converter.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/*-----------------------------------------------

   Created 7-5-2004 by Niels van Eldik

 Digitization tool which uses an rt-relation + smearing to convert MDT digitization
 input quantities into the output

-----------------------------------------------*/
namespace MuonGM {
    class MuonDetectorManager;
}

class RT_Relation_DigiTool : public AthAlgTool, virtual public IMDT_DigitizationTool {
public:
    RT_Relation_DigiTool(const std::string& type, const std::string& name, const IInterface* parent);

    virtual MdtDigiToolOutput digitize(const MdtDigiToolInput& input, CLHEP::HepRandomEngine* rndmEngine) override final;

    virtual StatusCode initialize() override;

    bool initializeTube(const MuonGM::MuonDetectorManager* detMgr);

private:
    double getDriftTime(double radius) const;
    double getAdcResponse() const;
    bool isTubeEfficient(const double radius, CLHEP::HepRandomEngine* rndmEngine, const bool issMDT) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Gaudi::Property<double> m_effRadius{this, "EffectiveRadius", 14.4275};
    Gaudi::Property<double> m_smallEffRadius{this, "SmallEffectiveRadius", 7.5};  // for sMDTs // which value to put here?

    double m_maxRadius;
    double m_smallMaxRadius;  // for sMDTs

    std::vector<Rt_relation*> m_rt;
};

inline double RT_Relation_DigiTool::getDriftTime(double r) const {
    // Drift radius --> time converter
    R_to_t_converter RTC;

    // Get the time value and return it
    double time = RTC.convert(r, *m_rt[0]);
    if (time < 0) time = 0;
    return (time);
}

inline double RT_Relation_DigiTool::getAdcResponse() const { return 76.7; }

inline bool RT_Relation_DigiTool::isTubeEfficient(const double radius, CLHEP::HepRandomEngine* rndmEngine, const bool issMDT) const {
    if (radius < 0) return false;
    double eff = 0;
    if (issMDT) {
        if (radius > m_smallMaxRadius) return false;
        if (radius < m_smallEffRadius) return true;
        eff = 1.0 + (radius - m_smallEffRadius) / (m_smallEffRadius - m_smallMaxRadius);
    } else {
        if (radius > m_maxRadius) return false;
        if (radius < m_effRadius) return true;
        eff = 1.0 + (radius - m_effRadius) / (m_effRadius - m_maxRadius);
    }
    if (CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0) <= eff) return true;
    return false;
}

#endif
