// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingToolSA.h 513171 2012-08-10 09:41:46Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOLSA_H
#define TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOLSA_H

// STL include(s):
#include <string>
#include <map>
#include <vector>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "BunchCrossingToolBase.h"

// Forward declaration(s):
class TTree;
class TBranch;

namespace Trig {

   /**
    *  @short Stand-alone bunch crossing tool for processing D3PD files
    *
    *         This implementation was designed to be used for processing D3PD
    *         files which have the bunch configuration stored in them as
    *         metadata.
    *
    *         Each time you open a new D3PD file you need to call the
    *         <code>cacheConfig(...)</code> function, and for each event you
    *         need to call the <code>loadConfig(...)</code> function. This way
    *         the tool is always configured correctly in your analysis.
    *         Unfortunately unlike in Athena, there's no way of doing this
    *         automatically behing the scenes in this tool...
    *
    *         This tool has no Athena front-end, as it would make no sense.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 513171 $
    * $Date: 2012-08-10 11:41:46 +0200 (Fri, 10 Aug 2012) $
    */
   class D3PDBunchCrossingToolSA : public virtual BunchCrossingToolBase {

   public:
      /// Constructor that can receive both trees needed
      D3PDBunchCrossingToolSA( ::TTree* metaTree = 0 );

      /// Function caching the information out of the metadata tree
      bool cacheConfig( ::TTree* metaTree, bool clearCache = true );

      /// Function loading a the configuration corresponding to a given configuration ID
      bool loadConfig( UInt_t configID );
      /// Function checking if a given configuration key is known
      bool isAvailable( UInt_t configID ) const;

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
      /// Function returning whether the previous bunches were filled, and how
      virtual std::vector< bool > bunchesInFront( bcid_type bcid,
                                                  int bunches = 10 ) const;
      /// Function returning whether the following bunches were filled, and how
      virtual std::vector< bool > bunchesAfter( bcid_type bcid,
                                                int bunches = 10 ) const;
      /// Function returning the intensities of the bunch crossings before the reference
      virtual std::vector< float > bunchIntInFront( bcid_type bcid,
                                                    int bunches = 10,
                                                    BeamType type = Crossing ) const;
      /// Function returning the intensities of the bunch crossings after the reference
      virtual std::vector< float > bunchIntAfter( bcid_type bcid,
                                                  int bunches = 10,
                                                  BeamType type = Crossing ) const;
      /// Gap before a particular bunch
      virtual int gapBeforeBunch( bcid_type bcid,
                                  BunchDistanceType dtype = NanoSec,
                                  BunchFillType ftype = CollidingBunch ) const;
      /// Gap after a particular bunch
      virtual int gapAfterBunch( bcid_type bcid,
                                 BunchDistanceType dtype = NanoSec,
                                 BunchFillType ftype = CollidingBunch ) const;

      /// Get the number of filled bunches in the current configuration
      virtual unsigned int numberOfFilledBunches() const;

      /// Function setting the maximum bunch spacing parameter
      virtual void setMaxBunchSpacing( int spacing );
      /// Get the maximum bunch spacing parameter
      virtual int maxBunchSpacing() const;

      /// Function setting the length of the front of the bunch train
      virtual void setFrontLength( int length );
      /// Get the length of the front of the bunch train
      virtual int frontLength() const;
      /// Function setting the length of the tail of the bunch train
      virtual void setTailLength( int length );
      /// Get the length of the tail of the bunch train
      virtual int tailLength() const;

      /**
       *  @short Structure describing a bunch configuration
       *
       *         This custom structure is used just to avoid using either
       *         many simple, or just a few cryptic variables for caching
       *         the bunch configurations.
       *
       *         Unfortunately the rootcint generated dictionary can't
       *         handle this class if it's declared as a private member
       *         of the class. Since the main class doesn't inherit from
       *         TObject, it's also not possible to hide this class
       *         from the dictionary generator...
       *
       * $Revision: 513171 $
       * $Date: 2012-08-10 11:41:46 +0200 (Fri, 10 Aug 2012) $
       */
      struct BunchConfig {
         /// BCIDs of the colliding bunches
         std::vector< int > filledBCIDs;
         /// The beam 1 intensities in the colliding bunches
         std::vector< float > filledIntBeam1;
         /// The beam 2 intensities in the colliding bunches
         std::vector< float > filledIntBeam2;
         /// BCIDs of the unpaired bunches of beam 1
         std::vector< int > unpairedBCIDsBeam1;
         /// BCIDs of the unpaired bunches of beam 2
         std::vector< int > unpairedBCIDsBeam2;
         /// Intensities of the unpaired bunches in beam 1
         std::vector< float > unpairedIntBeam1;
         /// Intensities of the unpaired bunches in beam 2
         std::vector< float > unpairedIntBeam2;
      }; // struct BunchConfig

   private:
      /// Function reading the V1 metadata format
      bool cacheConfigV1( ::TTree* metaTree, bool clearCache );
      /// Function reading the V2 metadata format
      bool cacheConfigV2( ::TTree* metaTree, bool clearCache );

      /// Cached configurations
      std::map< UInt_t, BunchConfig > m_cache;

      bool m_configLoaded; ///< Flag specifying if any configuration is loaded already
      UInt_t m_currentID; ///< The currently loaded configuration ID
      bool m_validConfig; ///< Flag specifying whether we use a "valid" configuration

   }; // class D3PDBunchCrossingToolSA

}

#endif // TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOLSA_H
