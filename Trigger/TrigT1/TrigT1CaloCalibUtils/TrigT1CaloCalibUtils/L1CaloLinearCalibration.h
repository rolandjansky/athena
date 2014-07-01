/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOLINEARCALIBRATION_H
#define L1CALOLINEARCALIBRATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <boost/scoped_ptr.hpp>

// forward declrations
class L1CaloEnergyScanResultsContainer;
class L1CaloEnergyScanRunInfoContainer;

/**
 * This algorithms creates calibration constants using a linear function.
 *
 * The input is a L1CaloRampDataContainer created by the L1CaloRampMaker
 * algorithm. For each trigger tower the ramp is fitted using a pol1.
 * The slope and a few parameters are stored into a sqlite file, which gets
 * validated later by a validation algorithm.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */

class L1CaloLinearCalibration : public AthAlgorithm
{
 public:
    L1CaloLinearCalibration(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~L1CaloLinearCalibration();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    std::string m_l1CaloRampDataContainerKey;

    boost::scoped_ptr<L1CaloEnergyScanResultsContainer> m_energyScanResultsContainer;
    boost::scoped_ptr<L1CaloEnergyScanRunInfoContainer> m_energyScanRunInfoContainer;
};

#endif // L1CALOLINEARCALIBRATION_H
