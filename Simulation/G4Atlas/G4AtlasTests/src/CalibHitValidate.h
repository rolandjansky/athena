/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_CALIBHITVALIDATE_H
#define G4ATLASTESTS_CALIBHITVALIDATE_H
/**
 * @class CalibHitValidate
 * @author Robert Harrington <roberth@cern.ch>
 * @date 11-March-2014
 * @brief Add energy in CaloCalibrationHitContainers
 *
 *  */

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>

class CalibHitValidate: public AthAlgorithm
{
 public:

  CalibHitValidate(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode execute() override;
  virtual StatusCode initialize() override;

 private:

  /**
   * @brief vector of calibration hit container names to use.
   *
   * The containers specified in this property should hold calibration
   * hits inside the calorimeter systems. */
  std::vector<std::string> m_CalibrationHitContainerNames;

  /**
   * @brief vector of dead material calibration hit container names to use.
   *
   * The containers specified in this property should hold calibration
   * hits outside the calorimeter systems - i.e. dead material hits ... */
  std::vector<std::string> m_DMCalibrationHitContainerNames;

  /**
   * @brief name of truth particle container
   */
  std::string m_truthParticleCollectionName;
};

#endif // CALIBHITVALIDATE_H
