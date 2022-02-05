/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterPositionInCaloFillerTool.cxx
 * @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
 * @date May, 2012
 * @brief Block filler tool for eta/phi positions in calo coordinates from a CaloCluster.
 */


#include "ClusterPositionInCaloFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ClusterPositionInCaloFillerTool::ClusterPositionInCaloFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : Base (type, name, parent)
{  
  declareProperty("FillSeedCoordinates", m_FillSeedCoordinates=false);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode ClusterPositionInCaloFillerTool::initialize()
{
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());
  return  StatusCode::SUCCESS;
}



/**
 * @brief Book variables for this block.
 */
StatusCode ClusterPositionInCaloFillerTool::book()
{

  if(m_FillSeedCoordinates){
    CHECK( addVariable ("eta0Calo", m_eta0Calo,
                        "Cluster seed eta position in local calorimeter coordinates") );
    CHECK( addVariable ("phi0Calo", m_phi0Calo,
                        "Cluster seed phi position in local calorimeter coordinates") );
  }
  else{
    CHECK( addVariable ("etaCalo", m_etaCalo,
                        "Cluster eta position in local calorimeter coordinates") );
    CHECK( addVariable ("phiCalo", m_phiCalo,
                        "Cluster phi position in local calorimeter coordinates") );
  }

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
StatusCode ClusterPositionInCaloFillerTool::fill (const CaloCluster& p)
{
  CHECK( fillT (p) );
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
StatusCode ClusterPositionInCaloFillerTool::fill (const xAOD::CaloCluster& p)
{
  CHECK( fillT (p) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 * @param p The input object.
 *
 * This handles either cluster type.
 */
template <class T>
StatusCode ClusterPositionInCaloFillerTool::fillT (const T& p)
{

  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle { m_caloDetDescrMgrKey };
  CaloCell_ID::CaloSample sam = CaloCell_ID::EMB2;
  if (p.inBarrel() && !p.inEndcap())  
    sam = CaloCell_ID::EMB2;
  else if (!p.inBarrel() && p.inEndcap())
    sam = CaloCell_ID::EME2;
  else { 
    if (p.eSample(CaloSampling::EMB2) > p.eSample(CaloSampling::EME2)) sam=CaloCell_ID::EMB2;
    else sam = CaloCell_ID::EME2;
  }

  if (m_FillSeedCoordinates) {
    CaloCellDetPos::getDetPosition(**caloDetDescrMgrHandle,
                                   sam,
                                   p.eta0(),
                                   p.phi0(),
                                   *m_eta0Calo,
                                   *m_phi0Calo);
  } else {
    CaloCellDetPos::getDetPosition(
      **caloDetDescrMgrHandle, sam, p.eta(), p.phi(), *m_etaCalo, *m_phiCalo);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
