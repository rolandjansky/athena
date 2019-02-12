// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArDumpShapes.h,v 1.1 2006-06-27 18:31:59 ssnyder Exp $
/**
 * @file  LArCalibUtils/LArDumpShapes.h
 * @author scott snyder
 * @date Feb 2006
 * @brief Dump out shape information to a ROOT file.
 *
 * The information in the ROOT file consists of a set of 1-D histograms,
 * with names of the form shap%.9d_%.1d%.1d%.1d%.1d%.3d%.3d.
 * The % fields are as follows:
 *
 *   - CHID (as an integer)
 *   - gain
 *   - barrel/EC flag + 3
 *     (so it's always positive, and barrel is 3)
 *   - sampling
 *   - region
 *   - phi index
 *   - eta index
 *
 * The first half of the values in the histograms are the actual shape values.
 * The second half are the derivatives.
 */

#ifndef LARDUMPSHAPES_H
#define LARDUMPSHAPES_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <string>

/**
 * @brief Dump out shape information to a ROOT file.
 *
 * On the first event, this algorithm will retrieve the
 * LAr pulse shape information from the detector store
 * and dump it to a ROOT file for analysis.
 *
 * On subsequent events, this algorithm does nothing.
 */
class LArDumpShapes
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Algorithm constructor.
   * @brief name The name of the algorithm.
   * @brief svcloc The service locator object.
   */
  LArDumpShapes (const std::string& name, ISvcLocator* svcloc);

  /// Standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}


private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /// JO property: The nmae of the ROOT file to create.
  std::string m_filename;

  /// JO property: The desired gain.
  int m_gain;

  /// True for the first event.
  bool m_first;
};

#endif // not LARDUMPSHAPES_H
