/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "SFHelpers.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "Messaging.h"
#include "PATInterfaces/MakeSystematicsVector.h"
#include "PATInterfaces/SystematicsUtil.h"
#include <cmath>
#include <xAODEgamma/Electron.h>

int
SFHelpers::result(AsgElectronEfficiencyCorrectionTool& tool,
                  const xAOD::Electron& el,
                  double& nominalSF,
                  double& totalPos,
                  double& totalNeg,
                  const bool isToys)
{

  CP::SystematicSet systs = tool.recommendedSystematics();
  if (!isToys) {
    /*
     * Split the variation in up and down
     * Do it before any loop
     * This is obviously more of a sanity check
     * as they are currently symmetric so is mainly
     * about inspecting them both
     */
    std::vector<CP::SystematicVariation> positiveVar{};
    std::vector<CP::SystematicVariation> negativeVar{};
    for (const auto& sys : systs) {
      float param = sys.parameter();
      if (param < 0) {
        negativeVar.push_back(sys);
      } else {
        positiveVar.push_back(sys);
      }
    }
    // Helper function as a lamda
    auto totalSyst =
      [&tool](xAOD::Electron el,
              const std::vector<CP::SystematicVariation>& variations,
              const double nominal,
              double& result) {
        double total2{};
        double systematic{};
        for (const auto& sys : variations) {
          if (tool.applySystematicVariation({ sys }) != StatusCode::SUCCESS ||
              tool.getEfficiencyScaleFactor(el, systematic) !=
                CP::CorrectionCode::Ok) {
            MSG_ERROR("Error in setting/getting " << sys.name());
            return CP::CorrectionCode::Error;
          }
          MSG_DEBUG(tool.appliedSystematics().name()
                    << " Result : " << systematic
                    << " Value :  " << systematic - nominal);
          total2 += (nominal - systematic) * (nominal - systematic);
        }
        result = std::sqrt(total2);
        return CP::CorrectionCode::Ok;
      };

    // Do the work
    // Empty variation is the nominal
    CHECK(tool.applySystematicVariation({}));
    CHECK(tool.getEfficiencyScaleFactor(el, nominalSF) ==
          CP::CorrectionCode::Ok);
    CHECK(totalSyst(el, negativeVar, nominalSF, totalNeg));
    CHECK(totalSyst(el, positiveVar, nominalSF, totalPos));
  } else {
    CP::MakeSystematicsVector sysVec;
    sysVec.addGroup("toys");
    sysVec.setToys(tool.getNumberOfToys());
    sysVec.calc(systs);
    std::vector<CP::SystematicSet> toys = sysVec.result("toys");
    std::vector<double> toysVal{};
    toysVal.reserve(toys.size());

    // Do the work
    for (const auto& sys : toys) {
      double systematic{};
      CHECK(tool.applySystematicVariation(sys) == StatusCode::SUCCESS &&
            tool.getEfficiencyScaleFactor(el, systematic) ==
              CP::CorrectionCode::Ok);
      MSG_DEBUG(tool.appliedSystematics().name()
                << " toy Result : " << systematic)
      toysVal.push_back(systematic);
    }
    /*
     *  B. P. Welford 1962
     *  Donald KnutArt of Computer Programming, Vol 2, page
     *  232, 3rd edition
     */
    // 1st element,initilize
    double meanK{ toysVal[0] };   // current mean
    double meanK_1{ toysVal[0] }; // next mean
    double s{ 0 };
    size_t k{ 1 };
    const size_t N = toysVal.size();
    // subsequent ones
    for (size_t i = 1; i != N; ++i) {
      const double x{ toysVal[i] };
      const double invk{ (1.0 / (++k)) };
      meanK_1 = meanK + (x - meanK) * invk;
      s += (x - meanK_1) * (x - meanK);
      meanK = meanK_1;
    }
    const double variance = s / (N - 1);
    nominalSF = meanK;
    totalNeg = sqrt(variance);
    totalPos = sqrt(variance);
  }
  return 0;
}
