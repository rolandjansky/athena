/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundClusterFillerTool.h 589851 2014-03-27 20:01:30Z ssnyder $
#ifndef D3PDMAKER_BEAMBACKGROUNDCLUSTERFILLERTOOL_H
#define D3PDMAKER_BEAMBACKGROUNDCLUSTERFILLERTOOL_H

#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/errorcheck.h"
#include <string>


namespace D3PD {

/**
 * @brief Block filler tool for the cluster results of the Beam Background Identification Method
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 589851 $
 * $Date: 2014-03-27 21:01:30 +0100 (Thu, 27 Mar 2014) $
 */
class BeamBackgroundClusterFillerTool
  : public BlockFillerTool<xAOD::CaloCluster>
{
public:
  BeamBackgroundClusterFillerTool (const std::string& type,
      const std::string& name,
      const IInterface* parent);

  // Book variables for this block.
  virtual StatusCode book();

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::CaloCluster& p);

private:
  /// Property: SG key for the beam background object.
  std::string m_beamBackgroundKey;

  BeamBackgroundData* m_beamBackgroundData;

  int* m_resultClus;
  float* m_drdzClus;
};

} // namespace D3PD

#endif
