/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArDumpShapes.cxx,v 1.3 2007-12-03 10:02:56 wlampl Exp $
/**
 * @file  LArCalibUtils/LArDumpShapes.cxx
 * @author scott snyder
 * @date Feb 2006
 * @brief Dump out shape information to a ROOT file.
 */

#include "LArCalibUtils/LArDumpShapes.h"
#include "LArElecCalib/ILArShape.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloGain.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "TFile.h"
#include "TH1.h"
#include <cstdio>
#include <cmath>


/**
 * @brief Standard Algorithm constructor.
 * @brief name The name of the algorithm.
 * @brief svcloc The service locator object.
 */
LArDumpShapes::LArDumpShapes (const std::string& name, ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_first (true)
{
  // Set up properties.
  declareProperty ("filename", m_filename = "shapes.root");
  declareProperty ("gain", m_gain = CaloGain::LARHIGHGAIN);
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArDumpShapes::initialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode LArDumpShapes::execute()
{
  // Only do this for the first event.
  if (!m_first)
    return StatusCode::SUCCESS;

  // Get the shapes from the detector store.
  const DataHandle<ILArShape> dd_shape;
  CHECK( detStore()->retrieve (dd_shape) );

  // Get the cabling service.
  LArCablingService * cabling = 0;
  CHECK( toolSvc()->retrieveTool("LArCablingService",cabling) );

  // Get the calorimeter identifier helper.
  const LArEM_ID* em_id = CaloIdManager::instance()->getEM_ID();

  // Open the ROOT file.
  TFile f (m_filename.c_str(), "RECREATE");

  // Loop over all EM cells.
  typedef std::vector<Identifier>::const_iterator It;
  It beg = em_id->em_begin();
  It end = em_id->em_end();
  for (; beg != end; ++beg) {
    // Check that this is a valid cell.
    Identifier id = *beg;
    if (!id.is_valid()) continue;
    HWIdentifier chid = cabling->createSignalChannelID (id);
    if (!chid.is_valid()) continue;

    // Get the shape information for this cell.
    ILArShape::ShapeRef_t shape = dd_shape->Shape(chid, m_gain, 0);
    ILArShape::ShapeRef_t shapeder = dd_shape->ShapeDer(chid, m_gain, 0);

    // Construct the name to use for the histogram.
    char name[50];
    std::sprintf(name, "shap%.9d_%.1d%.1d%.1d%.1d%.3d%.3d",
                 (int)chid.get_identifier32().get_compact(),
                 m_gain,
                 em_id->barrel_ec (id)+3,
                 em_id->sampling(id),
                 em_id->region(id),
                 em_id->phi(id),
                 em_id->eta(id));

    // Create the histogram.
    unsigned sz1 = shape.size();
    unsigned sz2 = shapeder.size();
    TH1* h1 = new TH1F (name, name, sz1 + sz2, 0, sz1 + sz2);

    // Copy the data to the histogram.
    for (unsigned int i=0; i < sz1; i++)
      h1->SetBinContent (i+1, shape[i]);
    for (unsigned int i=0; i < sz2; i++) {
      double der = shapeder[i];
      // ShapeDer uses 1e100 as a flag, which is not
      // convertable to a float...
      if (std::abs(der) > 1e10) der = 1e10;
      h1->SetBinContent (i+sz1+1, der);
    }

    // Write the histogram to the ROOT file.
    h1->Write();
    delete h1;
  }

  // Close the ROOT file and remember that we did this.
  f.Close();
  m_first = false;
  return StatusCode::SUCCESS;
}



