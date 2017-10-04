// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGTOOLBASE_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGTOOLBASE_H

// STL include(s):
#include <set>
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AsgTools/AsgMetadataTool.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossing.h"
#include "TrigBunchCrossingTool/BunchTrain.h"

namespace Trig {

   /**
    * @short Base class for all BunchCrossingTool implementations
    *
    * This class holds the logic of the bunch crossing tool. The concrete
    * implementations have to supply it with a vector of the filled bunches
    * and optionally their intensities, which they can get from any location
    * they want.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 618331 $
    * $Date: 2014-09-24 13:55:26 +0200 (Wed, 24 Sep 2014) $
    */
   class BunchCrossingToolBase : public virtual IBunchCrossingTool,
                                 public asg::AsgMetadataTool {

   public:
      /// Default constructor
      BunchCrossingToolBase( const std::string& name = "BunchCrossingToolBase" );

      /// @name Functions implementing the IBunchCrossingTool interface
      /// @{

      /// The simplest query: Is the bunch crossing filled or not?
      virtual bool isFilled( bcid_type bcid ) const;

      /// Function deciding if a given bunch crossing is in a filled train
      virtual bool isInTrain( bcid_type bcid ) const;

      /// Function deciding if a given bunch crossing has an unpaired bunch
      virtual bool isUnpaired( bcid_type bcid ) const;

      /// Function deciding if there was a bunch from "beam 1" in this bunch crossing
      virtual bool isBeam1( bcid_type bcid ) const;

      /// Function deciding if there was a bunch from "beam 2" in this bunch crossing
      virtual bool isBeam2( bcid_type bcid ) const;

      /// Function returning the "intensity" of a given bunch crossing
      virtual float bcIntensity( bcid_type bcid,
                                 BeamType type = Crossing ) const;

      /// Get the type of the specific bunch crossing
      virtual BunchCrossingType bcType( bcid_type bcid ) const;

      /// The distance of the specific bunch crossing from the front of the train
      virtual int distanceFromFront( bcid_type bcid,
                                     BunchDistanceType type = NanoSec ) const;
      /// The distance of the specific bunch crossing from the tail of the train
      virtual int distanceFromTail( bcid_type bcid,
                                    BunchDistanceType type = NanoSec ) const;

      /// Gap before the train this BCID is in
      virtual int gapBeforeTrain( bcid_type bcid,
                                  BunchDistanceType type = NanoSec ) const;
      /// Gap after the train this BCID is in
      virtual int gapAfterTrain( bcid_type bcid,
                                 BunchDistanceType type = NanoSec ) const;

      /// Gap before a particular bunch
      virtual int gapBeforeBunch( bcid_type bcid,
                                  BunchDistanceType dtype = NanoSec,
                                  BunchFillType ftype = CollidingBunch ) const;
      /// Gap after a particular bunch
      virtual int gapAfterBunch( bcid_type bcid,
                                 BunchDistanceType dtype = NanoSec,
                                 BunchFillType ftype = CollidingBunch ) const;

      /// Function returning whether the previous bunches were filled, and how
      virtual std::vector< bool > bunchesInFront( bcid_type bcid,
                                                  int bunches = 10 ) const;
      /// Function returning whether the following bunches were filled, and how
      virtual std::vector< bool > bunchesAfter( bcid_type bcid = 0,
                                                int bunches = 10 ) const;

      /// Function returning the intensities of the bunch crossings before the reference
      virtual std::vector< float > bunchIntInFront( bcid_type bcid,
                                                    int bunches = 10,
                                                    BeamType type = Crossing ) const;
      /// Function returning the intensities of the bunch crossings after the reference
      virtual std::vector< float > bunchIntAfter( bcid_type bcid,
                                                  int bunches = 10,
                                                  BeamType type = Crossing ) const;

      /// Get the number of filled bunches in the current configuration
      virtual unsigned int numberOfFilledBunches() const;
      /// Get the number of unpaired bunches in the current configuration
      virtual unsigned int numberOfUnpairedBunches() const;
      /// Get the number of the bunch trains in the current configuration
      virtual unsigned int numberOfBunchTrains() const;
      /// Get the bunch spacing in the trains
      virtual int bunchTrainSpacing( BunchDistanceType type = NanoSec ) const;

      /// @}

   protected:
      /// Interpret the configuration for single bunches
      StatusCode loadSingleBunches( const std::vector< int >& bunches,
                                    const std::vector< float >& bunch_int1 =
                                    std::vector< float >(),
                                    const std::vector< float >& bunch_int2 =
                                    std::vector< float >() );
      /// Interpret the configuration for bunch trains
      StatusCode loadBunchTrains( const std::vector< int >& bunches,
                                  const std::vector< float >& bunch_int1 =
                                  std::vector< float >(),
                                  const std::vector< float >& bunch_int2 =
                                  std::vector< float >() );
      /// Interpret the configuration for unpaired bunches
      StatusCode loadUnpairedBunches( const std::vector< int >& beam1,
                                      const std::vector< int >& beam2,
                                      const std::vector< float >& bunch_int1 =
                                      std::vector< float >(),
                                      const std::vector< float >& bunch_int2 =
                                      std::vector< float >() );

      /// Function printing the configuration of the tool
      void printConfig() const;

      /// @name Variables holding the decoded bunch structure
      /// @{

      /// List of colliding bunches
      std::set< Trig::BunchCrossing > m_filledBunches;
      /// Internal list of single bunches
      std::set< Trig::BunchCrossing > m_singleBunches;
      /// Internal list of unpaired bunches
      std::set< Trig::BunchCrossing > m_unpairedBunches;
      /// Internal list of bunch trains
      std::set< Trig::BunchTrain > m_bunchTrains;

      /// @}

      /// @name Configurable tool properties
      /// @{

      /// The maximum bunch spacing that the tool should consider
      int m_maxBunchSpacing;
      /// Length of the "front" of a bunch train
      int m_frontLength;
      /// Length of the "tail" of a bunch train
      int m_tailLength;

      /// @}

   private:
      /// Get the apparent bunch spacing in the current configuration
      int bunchSpacing( const std::vector< int >& bunches ) const;

   }; // class BunchCrossingToolBase

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGTOOLBASE_H
