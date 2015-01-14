// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingTool.h 639543 2015-01-14 17:51:02Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H

// STL include(s):
#include <string>
#include <map>
#include <vector>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/BunchConfig.h"

// Forward declaration(s):
class TTree;

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
    * $Revision: 639543 $
    * $Date: 2015-01-14 18:51:02 +0100 (Wed, 14 Jan 2015) $
    */
   class D3PDBunchCrossingTool : public BunchCrossingToolBase {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( D3PDBunchCrossingTool, Trig::IBunchCrossingTool )

   public:
      /// Default constructor
      D3PDBunchCrossingTool( const std::string& name =
                             "D3PDBunchCrossingTool" );

      /// @name Function managing the configuration loaded by the tool
      /// @{

      /// Function caching the information out of the metadata tree
      StatusCode cacheConfig( ::TTree* metaTree, bool clearCache = true );

      /// Function loading a the configuration corresponding to a given
      /// configuration ID
      StatusCode loadConfig( UInt_t configID );
      /// Function checking if a given configuration key is known
      bool isAvailable( UInt_t configID ) const;

      /// @}

      /// @name Functions re-implemented from the IBunchCrossingTool interface
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

      /// @}

   private:
      /// Function reading the V1 metadata format
      StatusCode cacheConfigV1( ::TTree* metaTree, bool clearCache );
      /// Function reading the V2 metadata format
      StatusCode cacheConfigV2( ::TTree* metaTree, bool clearCache );

      /// Cached configurations
      /// We need to be explicit about the namespace of Trig::BunchConfig,
      /// otherwise the dictionary generator fails in ROOT 5.
      std::map< UInt_t, Trig::BunchConfig > m_cache;

      /// Flag specifying if any configuration is loaded already
      bool m_configLoaded;
      /// The currently loaded configuration ID
      UInt_t m_currentID;
      /// Flag specifying whether we use a "valid" configuration
      bool m_validConfig;

   }; // class D3PDBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_D3PDBUNCHCROSSINGTOOL_H
