/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBHITVALIDATE_H
#define CALIBHITVALIDATE_H
/**
 * @class CalibHitValidate
 * @author Robert Harrington <roberth@cern.ch>
 * @date 11-March-2014
 * @brief Add energy in CaloCalibrationHitContainers
 *
 *  */

class CaloDetDescrManager;
class CaloDetDescrElement;
class CaloCell_ID;
class CaloDM_ID;
class CaloDmDescrManager;
class McEventCollection;
//class TruthParticleContainer;

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"

//#include "CaloRec/CaloClusterCollectionProcessor.h"
//#include "CaloEvent/CaloClusterMoment.h"
//#include "CaloEvent/CaloSampling.h"
//#include "CaloIdentifier/CaloCell_ID.h"
//#include "CaloDmDetDescr/CaloDmDescrManager.h"

#include <string>
#include <vector>
#include <set>
#include <map>


class CalibHitValidate: public AthAlgorithm
{
 public:

  CalibHitValidate(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~CalibHitValidate();

  StatusCode execute();
  StatusCode initialize();

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

  //const CaloDetDescrManager* m_calo_dd_man;

  //const CaloCell_ID* m_calo_id;

  //const CaloDM_ID*    m_caloDM_ID;

  //const CaloDmDescrManager* m_caloDmDescrManager;

  //const TruthParticleContainer* m_truthParticles;

  //bool m_isInitialized;

};

#endif // CALIBHITVALIDATE_H
