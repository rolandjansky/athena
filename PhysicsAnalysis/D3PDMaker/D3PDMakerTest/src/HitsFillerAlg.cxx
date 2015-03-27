/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/HitsFillerAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Put objects into StoreGate to test D3PD maker.
 *
 * Makes a dummy MDTSimHitsCollection.
 */


#include "HitsFillerAlg.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrackRecord/TrackRecord.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
HitsFillerAlg::HitsFillerAlg (const std::string& name,
                              ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_count (0)
{
  declareProperty ("MDTSimHitName",   m_MDTSimHitName   = "MDTSimHits");
  declareProperty ("TrackRecordName", m_TrackRecordName = "TrackRecord");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode HitsFillerAlg::initialize()
{
  m_count = 0;
  return AthAlgorithm::initialize();
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode HitsFillerAlg::finalize()
{
  return AthAlgorithm::finalize();
}


/**
 * @brief Fill a MDTSimHitCollection.
 */
StatusCode HitsFillerAlg::fillMDTSimHitCollection()
{
  MDTSimHitCollection* c = new MDTSimHitCollection (m_MDTSimHitName);

  for (int i = 0; i < 10; i++) {
    int j = i * 10 + m_count;
    MDTSimHit h (j,
                 j+10, // time
                 j+11, // radius
                 Amg::Vector3D(j+12,j+13,j+14), //position
                 j+15, // trackNumber
                 j+16, // stepLength
                 j+17, // energyDeposit
                 j+18, // particleEncoding,
                 j+19  // kineticEnergy
                 );
    c->push_back (h);
  }

  CHECK( this->evtStore()->record (c, m_MDTSimHitName) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill a TrackRecordCollection.
 */
StatusCode HitsFillerAlg::fillTrackRecordCollection()
{
  TrackRecordCollection* c = new TrackRecordCollection (m_TrackRecordName);

  for (int i = 0; i < 10; i++) {
    int j = i * 10 + m_count;
    CLHEP::Hep3Vector p(j+11,j+12,j+13);
    CLHEP::Hep3Vector x(j+14,j+15,j+16);
    c->Emplace(j, // PDG
               j+10, // energy
               p, //position
               x, //momentum
               j+17, // time
               j+18, // barcode
               "volname" // volume name
               );
  }

  CHECK( this->evtStore()->record (c, m_TrackRecordName) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode HitsFillerAlg::execute()
{
  CHECK( fillMDTSimHitCollection() );
  CHECK( fillTrackRecordCollection() );

  ++m_count;
  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
