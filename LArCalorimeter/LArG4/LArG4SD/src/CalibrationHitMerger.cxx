/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibrationHitMerger.h"

/** Constructor */
LArG4::CalibrationHitMerger::CalibrationHitMerger( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_inputHitsSGKeys(),
  m_inputHits(),
  m_outputHits()
{
  // Input collections StoreGate keys
  declareProperty( "InputHits",    m_inputHitsSGKeys   );

  // Output collections
  declareProperty( "OutputHits",   m_outputHits        );
}


/** Destructor */
LArG4::CalibrationHitMerger::~CalibrationHitMerger()
{}


/** Athena Algorithm initialize */
StatusCode LArG4::CalibrationHitMerger::initialize()
{
  ATH_CHECK(setupReadHandleKeyVector(m_inputHitsSGKeys,   m_inputHits  ));

  ATH_CHECK(initializeVarHandleKey(m_outputHits  ));

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode LArG4::CalibrationHitMerger::execute()
{
  // skip if not input collection
  if ( m_inputHits.empty() ) {
    return StatusCode::SUCCESS;
  }
  // TODO: is there a way to conveniently get the total number of hits in all m_inputHits
  //       and reserve the corresponding size in the outputHandle
  SG::WriteHandle<CaloCalibrationHitContainer> outputHandle{m_outputHits};
  ATH_CHECK( outputHandle.record(std::make_unique<CaloCalibrationHitContainer>()) );
  m_calibrationHits_t calibrationHits;
  unsigned int nHitsMerged(0);
  for ( const auto& collKey: m_inputHits ) {
    SG::ReadHandle<CaloCalibrationHitContainer> inputHandle{collKey};
    ATH_MSG_DEBUG(inputHandle.name() << " has " << inputHandle->size() << " hits.");
    for ( const auto& hit: *inputHandle ) {
      // If we haven't had a hit in this cell before, create one and add
      // it to the hit collection.

      // If we've had a hit in this cell before, then add the energy to
      // the existing hit.

      // Look for the key in the hitCollection (this is a binary search).
      auto bookmark = calibrationHits.lower_bound(hit);

      // The lower_bound method of a map finds the first element
      // whose key is not less than the identifier.  If this element
      // == our hit, we've found a match.

      // Reminders:
      // bookmark = iterator (pointer) into the hitCollection set.
      // (*bookmark) = a member of the set, which is a LArG4Hit*.

      // Equals() is a function defined in LArG4Hit.h; it has the value of
      // "true" when a LArG4Hit* points to the same identifier.

      if (bookmark == calibrationHits.end() ||
          !(*bookmark)->Equals(hit)) {
        auto&& hitCopy = std::make_unique<CaloCalibrationHit>(*hit);
        // We haven't had a hit in this readout cell before.  Add it
        // to our set.
        if (calibrationHits.empty() ||
            bookmark == calibrationHits.begin()) {
          // Insert the hit before the first entry in the map.
          calibrationHits.insert(hitCopy.release());
        } else {
          // We'just done a binary search of hitCollection, so we should use
          // the results of that search to speed up the insertion of a new
          // hit into the map.  The "insert" method is faster if the new
          // entry is right _after_ the bookmark.  If we left bookmark
          // unchanged, the new entry would go right _before_ the
          // bookmark.  We therefore want to decrement the bookmark from
          // the lower_bound search.
          calibrationHits.insert(--bookmark, hitCopy.release());
        }
      } else {
        ++nHitsMerged;
        // Update the existing hit.
        (*bookmark)->Add(hit);
      }
    }
  }

  // Loop through the hits...
  for(auto hit : calibrationHits)
  {
    // Can we actually do this with move?
    outputHandle->push_back(hit);
  } // End of loop over hits
  // Clean up
  calibrationHits.clear();
  ATH_MSG_DEBUG(outputHandle.name() << " has " << outputHandle->size() << " hits. Merged " << nHitsMerged << " hits.");
  return StatusCode::SUCCESS;
}


/** Athena Algorithm finalize */
StatusCode LArG4::CalibrationHitMerger::finalize()
{
  return StatusCode::SUCCESS;
}

/** Initialize the given VarHandleKey */
StatusCode LArG4::CalibrationHitMerger::initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const {
  if ( varHandleKey.key().empty() )
    return StatusCode::SUCCESS;

  return varHandleKey.initialize();
}
/** Setup a vector of ReadHandleKeys for the given vector of string StoreGate Keys */
StatusCode LArG4::CalibrationHitMerger::setupReadHandleKeyVector( const SGKeyVector_t&      sgKeyVec,
                                                                ReadHandleKeyVector_t& readHandleVec ) const {
  readHandleVec.reserve( sgKeyVec.size() );

  // convert string StoreGate key to ReadHandleKey
  for ( const auto& sgKey: sgKeyVec ) {
    readHandleVec.emplace_back( sgKey );
  }

  // initialize all ReadHandleKeys
  for ( auto& readHandleKey: readHandleVec ) {
    if ( readHandleKey.initialize().isFailure() ) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

