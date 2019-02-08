/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerCommonSelector.cxx 668868 2015-05-20 20:26:18Z wsfreund $
#ifndef RINGER_STANDALONE

//#undef NDEBUG

// Local includes:
#include "RingerSelectorTools/ElectronTAccept.h"
#include "RingerSelectorTools/tools/RingerCommonSelector.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/procedures/RingerProcedureWrapper.icc"
#include "RingerSelectorTools/AsgElectronRingerSelector.h"

// STL includes:
#include <vector>

// xAOD includes:
#include "xAODCaloRings/CaloRings.h"
#include "xAODTracking/TrackParticle.h"

namespace Ringer {

//==============================================================================
RingerCommonSelector::RingerCommonSelector(
    const Ringer::IDiscrWrapperCollection *discrWrapperCol,
    const Ringer::IThresWrapper *thresWrapper,
    const RingerConfStruct &fileConf)
  : m_discrWrapperCol(discrWrapperCol),
    m_thresWrapper(thresWrapper),
    m_useTrackPat(fileConf.useTrackPat),
    m_useRawTrackPat(fileConf.useRawTrackPat),
    m_useCaloCommittee(fileConf.useCaloCommittee),
    m_fstDiscrLayer(0),
    m_lastDiscrLayer(0)
{
  if ( !m_discrWrapperCol ) {
    throw std::runtime_error("Cannot create RingerCommonSelector"
        "without discrimination wrapper collection.");
  }

  if ( !m_thresWrapper ) {
    throw std::runtime_error("Cannot create RingerCommonSelector"
        "without threshold wrapper.");
  }

  m_discrWrapperColSize = discrWrapperCol->size();

  m_fstDiscrLayer  = (*m_discrWrapperCol)[0];
  m_lastDiscrLayer = (*m_discrWrapperCol)[ m_discrWrapperColSize - 1 ];

  m_nonSegmentedDiscr = m_fstDiscrLayer->getSegType() ==
    SegmentationType::NoSegmentation;
}

//==============================================================================
StatusCode RingerCommonSelector::execute(
    const DepVarStruct &depVar,
    const xAOD::CaloRings* clRings,
    const TrackPatternsHolder* trackPat,
    asg::AcceptData& acceptData)
{
  ATH_MSG_DEBUG( "Starting executing RingerCommonSelector.");

  if ( !m_discrWrapperColSize ){
    ATH_MSG_ERROR("Cannot run with empty discrimination "
        "wrapper collection.");
    return StatusCode::FAILURE;
  }

  if ( m_useTrackPat || m_useRawTrackPat ) {
#ifndef NDEBUG
    ATH_MSG_DEBUG( "Using track information.");
#endif
    if (!trackPat){
      ATH_MSG_ERROR("Requested to add track patterns, "
          "but it isn't available.");
      return StatusCode::FAILURE;
    }

    // We now check if we want to add run discriminator specialized for track
    // patterns:
    const TrackPatternsHolder *localTrackPat = 0;
    if ( !m_useRawTrackPat ) {
      localTrackPat = trackPat;
    }

    if ( m_useCaloCommittee && !m_useRawTrackPat && !m_nonSegmentedDiscr )
    {
#ifndef NDEBUG
      ATH_MSG_DEBUG( "Using CaloCommite with nonSegmented discriminator.");
#endif
      // If we are doing calorimeter committee and have a discriminator
      // specialized for the track patterns, we need to distinguish the calorimeter
      // output from the track discriminator output, thus we execute the
      // discrimination layer two times, one for each type of input and get the
      // output space representation within two vectors:
      m_fstDiscrLayer->execute( depVar,
          clRings,
          nullptr,
          m_output);
      m_fstDiscrLayer->execute( depVar,
          nullptr,
          localTrackPat,
          m_trackDiscr_output);
    } else {
#ifndef NDEBUG
      ATH_MSG_DEBUG( "Not using CaloCommitee or using without segmented "
          "discriminator or using raw pattern.");
#endif
      // Otherwise we simply propagate
      m_fstDiscrLayer->execute( depVar, clRings, localTrackPat, m_output);
    }
  } else {
#ifndef NDEBUG
    ATH_MSG_DEBUG( "Not using track information. Propagating first layer.");
#endif
    // Do not input track patterns into discrimination layer:
    m_fstDiscrLayer->execute( depVar, clRings, nullptr, m_output );
  }

  // Check if we have more than one discrimination layer:
  if ( m_discrWrapperColSize > 1 ){

    // If we are using raw track pattern and haven't a dedicated committee for
    // the Calorimeter information, then we add the raw track pattern into the next
    // classification layer input space:
    if ( m_useRawTrackPat && !m_useCaloCommittee ) {
      trackPat->exportPatternsTo(m_input);
    }

    // Set input to be same as last layer output, so that we can continue
    // applying our chain:
    m_input = m_output;

    // If we have intermediary layers:
    if ( m_discrWrapperColSize > 2 ){
      // We use the output from first discrimination layer to work with the
      // sequential layers (except the last):
      for (size_t discrIdx = 1; discrIdx < m_discrWrapperColSize - 1;
          ++discrIdx ){

        (*m_discrWrapperCol)[discrIdx]->execute(
            depVar,
            m_input,
            m_output );
        // Set input to be same as last layer output, so that we can continue
        // applying our chain:
        m_input = m_output;
      }
    }

    // If we did calorimeter only committee, then we need to add track
    // information to last discrimination layer:
    if ( m_useCaloCommittee ) {
      // We check if we want to add raw track information to the last
      // discrimination layer input:
      if ( m_useRawTrackPat ) {
        trackPat->exportPatternsTo( m_input );
      } else {
        // otherwise we need to add track discrimination output, executed by
        // first layer into the last layer input space:
        // Copy trackDiscr output to input space:
        m_input.insert( m_input.end(),
            m_trackDiscr_output.begin(),
            m_trackDiscr_output.end());
      }
    }

    // Apply our last discriminator
    m_lastDiscrLayer->execute(
        depVar,
        m_input,
        m_output);
#ifndef NDEBUG
            ATH_MSG_DEBUG("Final output space is :" << m_output);
#endif
  }

  // With the discrimination chain applied, we now move into obtaining its
  // decision:
  m_thresWrapper->getOutput(depVar, m_output, m_decVec);

  if ( m_decVec.empty() ){
    ATH_MSG_ERROR("There was no decision returned "
        "by discrimination chain.");
    return StatusCode::FAILURE;
  }

  // Save discrimination into particle decision mask. We only use the first
  // decision vector output:
  acceptData.setCutResult( BitdefElectron_v1::RingerChainDec, m_decVec[0] );

  return StatusCode::SUCCESS;

}

} // Namespace Ringer

#endif // RINGER_STANDALONE
