/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TRandom.h>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):

#include "CPAnalysisExamples/JetCalibrationToolExample.h"

#include "PATInterfaces/CorrectionTool.h"
#include "PATInterfaces/SystematicCode.h"     
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 

namespace CP {

   JetCalibrationToolExample::JetCalibrationToolExample( const std::string& name )
     : asg::AsgTool( name ),
       m_currentCalib (0){

   }

   StatusCode JetCalibrationToolExample::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );

      // set up for default running without systematics
      applySystematicVariation (SystematicSet ()).ignore();

      // Add the affecting systematics to the global registry
      SystematicRegistry& registry = SystematicRegistry::getInstance();
      if (registry.registerSystematics(*this) != SystematicCode::Ok)
         return StatusCode::FAILURE;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   CorrectionCode JetCalibrationToolExample::applyCorrection( xAOD::Jet& jet ) {

      // Retrieve the event information:
      const xAOD::EventInfo* ei = 0;
      if( evtStore()->retrieve( ei, "EventInfo" ).isFailure() ) {
         ATH_MSG_WARNING( "No EventInfo object could be retrieved" );
         ATH_MSG_WARNING( "Random number generation not configured correctly" );
      }

      if (!ei) {
         ATH_MSG_ERROR( "Cannot retrieve the EventInfo" );
         return CorrectionCode::Error;
      }

      // In real life, the jet correction factors will depend on pileup related
      // quantities, on jet moments and jet kinematics. The correction factors
      // will be possibly retrieved from some root file to be decided at 
      // configuration stage. Due to the lack of real jet tools at the time of
      // code writing, we just mock an example. Here simply assuming a linear
      // correction with mu, just to illustrate the mechanics.
      // WARNING! This is NOT a jet/EtMiss recommendation!

      // Access mu

      const float mu = ei->averageInteractionsPerCrossing();

      // compute a simple correction factor depending on mu. Subtract 200 MeV per unit mu

      float correctionOffset =  200*mu;

      // Redefine the jet 4vector by scaling both pt and mass

      float newJetPt = jet.pt() - correctionOffset;
      if (newJetPt < 5000) newJetPt = 5000;

      xAOD::JetFourMom_t newp4(m_currentCalib->m_calibrationFactor*(newJetPt),
                               jet.eta(), jet.phi(),
                               m_currentCalib->m_calibrationFactor*jet.m());
      
      // Set the jet 4vector:
      jet.setJetP4( newp4);

      ATH_MSG_DEBUG("correction applied");

      // Return gracefully:
      return CorrectionCode::Ok;
   }

   CorrectionCode JetCalibrationToolExample::correctedCopy( const xAOD::Jet& input,
                                                            xAOD::Jet*& output ) {

      // A sanity check:
      if( output ) {
         ATH_MSG_WARNING( "Non-null pointer received. "
                          "There's a possible memory leak!" );
      }

      ATH_MSG_DEBUG("making the copy");

      return CP::CorrectionTool< xAOD::JetContainer >:: correctedCopy( input,output );
   }

   bool JetCalibrationToolExample::
   isAffectedBySystematic( const SystematicVariation& systematic ) const
   {
      SystematicSet sys = affectingSystematics();
      return sys.find (systematic) != sys.end ();
   }

   SystematicSet JetCalibrationToolExample::
   affectingSystematics() const
   {
      SystematicSet result;
      // the first systematic is a continuous one
      result.insert (SystematicVariation ("JETNP1", SystematicVariation::CONTINUOUS));
      // for the second systematic we only support two points:
      result.insert (SystematicVariation ("JETNP2", 1));
      result.insert (SystematicVariation ("JETNP2", -1));
      return result;
   }

   SystematicSet JetCalibrationToolExample ::
   recommendedSystematics() const
   {
      /// rationale: In the simple case that we recommend that all our
      ///   affecting systematics be used we can do this.  if we
      ///   recommend to use only a subset, we have to create that
      ///   separately.
      return affectingSystematics();
   }

   SystematicCode JetCalibrationToolExample ::
   applySystematicVariation ( const SystematicSet& systConfig ) {

      // first check if we already know this systematic configuration
      boost::unordered_map<SystematicSet,Calib>::iterator calibIter
         = m_calib.find (systConfig);
      if (calibIter != m_calib.end())
      {
         m_currentCalib = &calibIter->second;
         return SystematicCode::Ok;
      }

      // this is a systematic configuration that we haven't seen yet,
      // let's create a configuration for it.

      Calib calib;
      calib.m_calibrationFactor = 1;

      // the first systematic is a simple continuous systematic, where 1
      // sigma corresponds to a 5% effect on the calibration factor
      calib.m_calibrationFactor *= 1 + 0.05 * systConfig.getParameterByBaseName ("JETNP1");

      // for the second parameter we know that it is a +3% variation at
      // +1 sigma and a -7% variation at -1 sigma.  anything else is an
      // unsupported systematic
      SystematicVariation syst = systConfig.getSystematicByBaseName ("JETNP2");
      if (syst == SystematicVariation ("JETNP2", 1))
         calib.m_calibrationFactor *= 1.03;
      else if (syst == SystematicVariation ("JETNP2", -1))
         calib.m_calibrationFactor *= 0.93;
      else if (!syst.empty())
         return SystematicCode::Unsupported;

      // store this calibration for future use, and make it current
      m_currentCalib
         = &m_calib.insert (std::make_pair (systConfig, calib)).first->second;
      return SystematicCode::Ok;
   }

} // namespace CP
