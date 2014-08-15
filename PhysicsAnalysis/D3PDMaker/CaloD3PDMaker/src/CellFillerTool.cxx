/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/CellFillerTool.cxx
 * @author Jet Goodson <JGoodson@cern.ch>
 * @date Sep, 2009
 * @brief Block filler tool for cells.
 */


#include "CellFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CellFillerTool::CellFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<CaloCell> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode CellFillerTool::book()
{
  CHECK( addVariable ("E", m_cellEnergy));
  CHECK( addVariable ("eta", m_cellEta));
  CHECK( addVariable ("phi", m_cellPhi));
  CHECK( addVariable ("sinTh", m_cellSinTh));
  CHECK( addVariable ("cosTh", m_cellCosTh));
  CHECK( addVariable ("cotTh", m_cellCotTh));
  CHECK( addVariable ("x", m_cellX));
  CHECK( addVariable ("y", m_cellY));
  CHECK( addVariable ("z", m_cellZ));
  CHECK( addVariable ("Et", m_cellEt));
  CHECK( addVariable ("gain", m_cellGain));
  CHECK( addVariable ("badCell", m_badCell));
  CHECK( addVariable ("samplingID", m_samplingID));
  CHECK( addVariable ("time", m_time));
  CHECK( addVariable ("provenance", m_provenance));
  CHECK( addVariable ("quality", m_quality));
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode CellFillerTool::fill (const CaloCell& p)
{
 

  //for (std::vector<CaloCell *>::const_iterator it = p.begin(); it != p.end(); ++it) {
  *m_cellEnergy = p.energy();
  *m_cellEta = p.eta();
  *m_cellPhi = p.phi();
  *m_cellSinTh = p.sinTh();
  *m_cellCosTh = p.cosTh();
  *m_cellCotTh = p.cotTh();
  *m_cellX = p.x();
  *m_cellY = p.y();
  *m_cellZ = p.z();
  *m_cellEt = p.et();
  *m_cellGain = p.gain();
  *m_badCell =  p.badcell();
  const CaloDetDescrElement *dde = p.caloDDE();
  *m_samplingID = dde->getSampling();
  *m_time = p.time();
  *m_provenance = p.provenance();
  *m_quality = p.quality();


  //}//end of cells loop

  return StatusCode::SUCCESS;
}


} // namespace D3PD
