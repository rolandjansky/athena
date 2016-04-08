// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IBunchCrossingTool.h 618301 2014-09-24 08:53:05Z krasznaa $
#ifndef TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H
#define TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "AsgTools/IAsgTool.h"

/**
 *  @short The common trigger namespace for trigger analysis tools
 *
 *         This is the common namespace that all trigger analysis tools use by
 *         convention.
 *
 * $Revision: 618301 $
 * $Date: 2014-09-24 10:53:05 +0200 (Wed, 24 Sep 2014) $
 */
namespace Trig {

   /**
    * @short Interface for all the bunch crossing tools
    *
    * This is the dual-use inteface of all the bunch crossing tools.
    * It no longer uses the framework independent IIBunchCrossingTool
    * class...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 618301 $
    * $Date: 2014-09-24 10:53:05 +0200 (Wed, 24 Sep 2014) $
    */
   class IBunchCrossingTool : public virtual asg::IAsgTool {

      /// Declare the interface that this class provides
      ASG_TOOL_INTERFACE( Trig::IBunchCrossingTool )

   public:
      /// Convenience type definition
      typedef unsigned int bcid_type;

      /// The simplest query: Is the bunch crossing filled or not?
      /**
       * This is the simplest question that one can ask the configuration:
       * Was the current BCID coming from a collision or not? If people
       * properly use the triggers to select their events, then this function
       * is not too useful, but for some special analyses it might be a good
       * idea to have it.
       *
       * @param bcid The bcid that should be checked
       * @returns <code>true</code> if the bcid was a collision bunch crossing,
       *          <code>false</code> otherwise
       */
      virtual bool isFilled( bcid_type bcid = 0 ) const = 0;

      /// Function deciding if a given bunch crossing is in a filled train
      /**
       * Most of the functionality of the interface only makes sense for bunches
       * which were in trains. So this function can be checked at the beginning
       * of the analysis code, to make sure that the BC in question is interesting.
       *
       * @param bcid The bcid that should be checked
       * @returns <code>true</code> if the bcid is part of a bunch train,
       *          <code>false</code> otherwise
       */
      virtual bool isInTrain( bcid_type bcid = 0 ) const = 0;

      /// Function deciding if a given bunch crossing has an unpaired bunch
      /**
       * This function can be used to quickly decide whether the current event
       * came from an unpaired bunch. Note that the function doesn't make a difference
       * between "isolated" and "non isolated" unpaired bunches. You have to use
       * the <code>gapBeforeBunch</code> function to decide how isolated an
       * unpaired bunch is.
       *
       * @param bcid The bcid that should be checked
       * @returns <code>true</code> if the bcid belongs to an unpaired bunch,
       *          <code>false</code> otherwise
       */
      virtual bool isUnpaired( bcid_type bcid = 0 ) const = 0;

      /// Function deciding if there was a bunch from "beam 1" in this bunch crossing
      /**
       * This function is useful for differentiating the unpaired bunch
       * crossings based on which beam provided the unpaired bunch.
       *
       * @param bcid The bcid that should be checked
       * @returns <code>true</code> if the bcid contained a bunch from beam 1,
       *          <code>false</code> otherwise
       */
      virtual bool isBeam1( bcid_type bcid = 0 ) const = 0;

      /// Function deciding if there was a bunch from "beam 2" in this bunch crossing
      /**
       * This function is useful for differentiating the unpaired bunch
       * crossings based on which beam provided the unpaired bunch.
       *
       * @param bcid The bcid that should be checked
       * @returns <code>true</code> if the bcid contained a bunch from beam 2,
       *          <code>false</code> otherwise
       */
      virtual bool isBeam2( bcid_type bcid = 0 ) const = 0;

      /// Types of the return values of the bcIntensity function
      /**
       * The different information sources provide the "bunch intensity"
       * information in quite different ways, and the information can mean
       * different things actually. This enumeration is used to specify what
       * exact type of information the user is looking for.
       */
      enum BeamType {
         Beam1    = 0, ///< The returned intensity should be for "beam 1"
         Beam2    = 1, ///< The returned intensity should be for "beam 2"
         Crossing = 2  ///< The returned intensity should describe the BC
      };

      /// Function returning the "intensity" of a given bunch(-crossing)
      /**
       * Some imeplementations are able to return an "intensity" per BCID.
       * The exact meaning of this intensity is actually context sensitive,
       * as it could mean quite different things for pile-up MC and data
       * samples.
       *
       * The tool implementations that don't know about the bunch intensities
       * will return 1.0 for filled bunch crossings and 0.0 for empty ones.
       *
       * @param bcid The bcid that should be checked
       * @returns The "intensity" of the bcid in question
       */
      virtual float bcIntensity( bcid_type bcid = 0,
                                 BeamType type = Crossing ) const = 0;

      /// Simplified type for a given bunch crossing
      /**
       * This enumeration can specify what kind of bunch crossing one BCID
       * belongs to. The types could easily be extended later on.
       */
      enum BunchCrossingType {
         Empty = 0, ///< An empty bunch far away from filled bunches
         FirstEmpty = 1, ///< The first empty bunch after a train
         MiddleEmpty = 2, ///< An empty BCID in the middle of a train
         Single = 100, ///< This is a filled, single bunch (not in a train)
         Front = 200, ///< The BCID belongs to the first few bunches in a train
         Middle = 201, ///< The BCID belongs to the middle bunches in a train
         Tail = 202, ///< The BCID belongs to the last few bunces in a train
         Unpaired = 300 ///< This is an unpaired bunch (either beam1 or beam2)
      };

      /// Get the type of the specific bunch crossing
      /**
       * This function could be used as a single entry point to this tool in
       * principle. It gives a summary about a specific BCID. Remember however
       * that the exact meaning of many of the return values of this function
       * are configuration dependent.
       *
       * @param bcid The bcid that should be checked
       * @returns The type of the bcid in question
       */
      virtual BunchCrossingType bcType( bcid_type bcid = 0 ) const = 0;

      /// Enumeration specifying the units in which to expect the bunch distance type
      /**
       * To make it clear for the following functions what units to interpret their
       * return values in, it is possible to request their return values in different
       * units.
       */
      enum BunchDistanceType {
         NanoSec, ///< Distance in nanoseconds
         BunchCrossings, ///< Distance in units of 25 nanoseconds
         /// Distance in units of filled bunches (depends on filling scheme)
         FilledBunches
      };

      /// The distance of the specific bunch crossing from the front of the train
      /**
       * Get the distance of the specified bunch crossing from the front of the
       * bunch train. If the specified bunch crossing is not part of a bunch
       * train, then the function returns -1.
       *
       * The distance can be returned in multiple units. By default it is
       * returned in nanoseconds. If one specifies BunchCrossings, then the
       * distance is returned in nanosec_distance / 25. Finally, if one
       * specifies FilledBunches, then the distance is returned in the units of
       * the bunch spacing within the train.
       *
       * @param bcid The bcid that should be checked
       * @param type The type of the requested return value
       * @returns The distance of the bcid in question from the front of its
       *          bunch train
       */
      virtual
      int distanceFromFront( bcid_type bcid = 0,
                             BunchDistanceType type = NanoSec ) const = 0;
      /// The distance of the specific bunch crossing from the tail of the train
      /**
       * Get the distance of the specified bunch crossing from the tail of the
       * bunch train. If the specified bunch crossing is not part of a bunch
       * train, then the function returns -1.
       *
       * The distance can be returned in multiple units. By default it is
       * returned in nanoseconds. If one specifies BunchCrossings, then the
       * distance is returned in nanosec_distance / 25. Finally, if one
       * specifies FilledBunches, then the distance is returned in the units of
       * the bunch spacing within the train.
       *
       * @param bcid The bcid that should be checked
       * @param type The type of the requested return value
       * @returns The distance of the bcid in question from the tail of its
       *          bunch train
       */
      virtual
      int distanceFromTail( bcid_type bcid = 0,
                            BunchDistanceType type = NanoSec ) const = 0;

      /// Gap before the train this BCID is in
      /**
       * Get the gap that's between the train that the specified BCID is in, and
       * the previous train. This is a useful number for some jet/MET studies.
       *
       * Note that the function doesn't work with the FilledBunches type, as the
       * size of the gaps doesn't have to be a multiple of the bunch distance
       * within the trains.
       *
       * Returns "-1" when there's no right answer to the question. (BCID not
       * part of a train.)
       *
       * @param bcid The bcid whose train should be investigated
       * @param type The type of the requested return value
       * @returns The gap before the train of the specified bcid
       */
      virtual int gapBeforeTrain( bcid_type bcid = 0,
                                  BunchDistanceType type = NanoSec ) const = 0;
      /// Gap after the train this BCID is in
      /**
       * Get the gap that's between the train that the specified BCID is in, and
       * the next train. This is a useful number for some jet/MET studies.
       *
       * Note that the function doesn't work with the FilledBunches type, as the
       * size of the gaps doesn't have to be a multiple of the bunch distance
       * within the trains.
       *
       * Returns "-1" when there's no right answer to the question. (BCID not
       * part of a train.)
       *
       * @param bcid The bcid whose train should be investigated
       * @param type The type of the requested return value
       * @returns The gap after the train of the specified bcid
       */
      virtual int gapAfterTrain( bcid_type bcid = 0,
                                 BunchDistanceType type = NanoSec ) const = 0;

      /// Enumeration specifying what kind of bunch to use in the gap functions
      /**
       * The following functions can be used to calculate the gap before and
       * after a specific BCID to some other bunch type. The gap can actually be
       * wrt. two different types of bunches. The user may be interested between
       * the space of two filled bunches, the space between an unpaired bunch
       * and the previous filled bunch, the space between two unpaired bunches,
       * or the space between a filled bunch and the previous unpaired bunch.
       *
       * The empty type is just put here for completeness. Maybe once we'll be
       * using 25 ns spacing in the bunch trains, this will be a useful
       * parameter as well.
       *
       * This enumeration helps in answeing all of these questions.
       */
      enum BunchFillType {
         /// The gap should be calculated wrt. the closest colling bunch
         CollidingBunch = 0,
         /// The gap should be calculated wrt. the closest unpaired bunch
         UnpairedBunch  = 1,
         /// The gap should be calculated wrt. the closest empty bunch
         EmptyBunch     = 2,
         /// The gap should be calculated wrt. the closest unpaired bunch from
         /// beam 1
         UnpairedBeam1  = 3,
         /// The gap should be calculated wrt. the closest unpaired bunch from
         /// beam 2
         UnpairedBeam2  = 4
      };

      /// Gap before a particular bunch
      /**
       * The function tells you how far the BCID in question is from the
       * previous colliding bunch crossing.
       *
       * @param bcid The bcid that should be investigated
       * @param dtype The type of the requested return value
       * @param ftype The type of the previous bunch to consider
       * @returns The gap before the specified bcid
       */
      virtual
      int gapBeforeBunch( bcid_type bcid = 0,
                          BunchDistanceType dtype = NanoSec,
                          BunchFillType ftype = CollidingBunch ) const = 0;

      /// Gap after a particular bunch
      /**
       * The function tells you how far the BCID in question is from the next
       * colliding bunch crossing.
       *
       * @param bcid The bcid that should be investigated
       * @param dtype The type of the requested return value
       * @param ftype The type of the previous bunch to consider
       * @returns The gap after the specified bcid
       */
      virtual
      int gapAfterBunch( bcid_type bcid = 0,
                         BunchDistanceType dtype = NanoSec,
                         BunchFillType ftype = CollidingBunch ) const = 0;

      /// Function returning whether the previous bunches were filled, and how
      /**
       * To get a concrete idea of how the bunch structure looks like in front
       * of the bcid under investigation, this function returns a vector
       * representing the previous bunches.
       *
       * The indexing is such that <code>return[0]</code> means the reference
       * bunch, <code>return[1]</code> means the first bunch before that one,
       * etc. The boolean value <code>true</code> means that the bunch was
       * filled, <code>false</code> means that is was empty.
       *
       * It's really just another way of getting the same information as what
       * isFilled(...) is providing...
       *
       * @param bcid The bcid that should be checked
       * @param bunches The number of bunches about which the return value
       *                should have info
       * @returns The bunch pattern before the reference bunch
       */
      virtual std::vector< bool > bunchesInFront( bcid_type bcid = 0,
                                                  int bunches = 10 ) const = 0;

      /// Function returning whether the following bunches were filled, and how
      /**
       * This function is here to provide uniformity between the python and C++
       * implementations. It works pretty much like the bunchesInFron(...)
       * function.
       *
       * The indexing is such that <code>return[0]</code> means the reference
       * bunch, <code>return[1]</code> means the first bunch after that one,
       * etc. The boolean value <code>true</code> means that the bunch was
       * filled, <code>false</code> means that is was empty.
       *
       * It's really just another way of getting the same information as what
       * isFilled(...) is providing...
       *
       * @param bcid The bcid that should be checked
       * @param bunches The number of bunches about which the return value
       *                should have info
       * @returns The bunch pattern after the reference bunch
       */
      virtual std::vector< bool > bunchesAfter( bcid_type bcid = 0,
                                                int bunches = 10 ) const = 0;

      /// Function returning the intensities of the bunch crossings before the
      /// reference
      /**
       * In some cases one does not only want to know if the previous bunches
       * were filled, but also their intensities. This intensity has a context
       * sensitive meaning, as the MC and data bunch intensities can't be
       * directly compared in general, but in both cases the intensities are
       * described by floating point numbers.
       *
       * The indexing is the same as with the bunchesInFront(...) function.
       *
       * @param bcid The bcid that should be checked
       * @param bunches The number of bunches about which the return value
       *                should have info
       * @param type The intensity "type" the user is interested in
       * @returns The intensities of the bunch crossings in front of the
       *          reference BC
       */
      virtual std::vector< float >
      bunchIntInFront( bcid_type bcid = 0,
                       int bunches = 10,
                       BeamType type = Crossing ) const = 0;

      /// Function returning the intensities of the bunch crossings after the
      /// reference
      /**
       * In some cases one does not only want to know if the following bunches
       * were filled, but also their intensities. This intensity has a context
       * sensitive meaning, as the MC and data bunch intensities can't be
       * directly compared in general, but in both cases the intensities are
       * described by floating point numbers.
       *
       * The indexing is the same as with the bunchesAfter(...) function.
       *
       * @param bcid The bcid that should be checked
       * @param bunches The number of bunches about which the return value
       *                should have info
       * @param type The intensity "type" the user is interested in
       * @returns The intensities of the bunch crossings after of the reference
       *          BC
       */
      virtual std::vector< float >
      bunchIntAfter( bcid_type bcid = 0,
                     int bunches = 10,
                     BeamType type = Crossing ) const = 0;

      /// Get the number of filled bunches in the current configuration
      /**
       * This function gives the total number of filled (colliding) bunches in
       * the current configuration. This could come in handy in some
       * circumstances.
       *
       * @returns The total number of filled (colliding) bunches in the machine
       */
      virtual unsigned int numberOfFilledBunches() const = 0;

      /// Get the number of unpaired bunches in the current configuration
      /**
       * This function gives the total number of unpaired bunches in the current
       * configuration. This could come in handy in some circumstances.
       *
       * @returns The total number of unpaired bunches in the machine
       */
      virtual unsigned int numberOfUnpairedBunches() const = 0;

      /// Get the number of the bunch trains in the current configuration
      /**
       * This function gives the total number of bunch trains that were
       * identified in the current configuration. This could come in handy in
       * some circumstances.
       *
       * @returns The total number of bunch trains in the machine
       */
      virtual unsigned int numberOfBunchTrains() const = 0;

      /// Get the bunch spacing in the trains
      /**
       * This function can be used to easily find out what was the bunch spacing
       * in the bunch trains. This could come in handy in some circumstances.
       *
       * Note that a return value type of <code>FilledBunches</code> makes no
       * sense, so the code will complain if you request the value with that
       * parameter.
       *
       * @param type The type of the requested return value
       * @returns The bunch spacing in the requested units
       */
      virtual
      int bunchTrainSpacing( BunchDistanceType type = NanoSec ) const = 0;

   }; // class IBunchCrossingTool

} // namespace Trig

#endif // TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H
