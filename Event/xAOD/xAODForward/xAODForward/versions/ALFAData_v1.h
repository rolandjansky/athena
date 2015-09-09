// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData_v1.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_VERSIONS_ALFADATA_V1_H
#define XAODFORWARD_VERSIONS_ALFADATA_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class containing ALFA information
   ///
   /// For information how to retrive, and to some level use this object,
   /// see the detailed package description.
   ///
   /// @author Libor Nozka <Libor.Nozka@cern.ch>
   ///
   /// $Revision: 693858 $
   /// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
   ///
   class ALFAData_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      ALFAData_v1();

      /// @name Information from LocRecEvCollection and LocRecODEvCollection
      /// @{

      /// Detector ID where a track was reconstructed
      ///
      /// * 0 --- Default value, i.e. no reconstructed track
      /// * 1 --- Main Detector (MD)
      /// * 2 --- Overlap Detector (OD) x > 0
      /// * 3 --- Overlap Detector (OD) x < 0
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& detectorPartID() const;
      /// Set the detector ID where a track was reconstructed
      void setDetectorPartID( const std::vector< int >& value );

      /// Maximum number of reconstructed tracks in a pot
      ///
      /// This value is used to set the size of arrays containing
      /// information about tracks.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      int maxTrackCnt() const;
      /// Set the maximum number of reconstructed tracks in a pot
      void setMaxTrackCnt( int value );

      /// An overlap size for a U projection of the main detector
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& overU() const;
      /// Set an overlap size for a U projection of the main detector
      void setOverU( const std::vector< float >& value );

      /// An overlap size for a V projection of the main detector
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& overV() const;
      /// Set an overlap size for a V projection of the main detector
      void setOverV( const std::vector< float >& value );

      /// An overlap size for a Y projection of the Overlap Detector
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& overY() const;
      /// Set an overlap size for a Y projection of the main detector
      void setOverY( const std::vector< float >& value );

      /// Number of Main Detector U plates used in a reconstruction
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& numU() const;
      /// Set the number of main detector U plates used in a reconstruction
      void setNumU( const std::vector< int >& value );

      /// Number of Main Detector V plates used in a reconstruction
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& numV() const;
      /// Set the number of main detector V plates used in a reconstruction
      void setNumV( const std::vector< int >& value );

      /// Number of Overlap Detector Y plates used in a reconstruction
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& numY() const;
      /// Set the number of overlap detector Y plates used in a reconstruction
      void setNumY( const std::vector< int >& value );

      /// Map of plates used in main detector track reconstruction
      ///
      /// The values can be of elements can be:
      /// * 0 - when plate was not used in reconstruction
      /// * 1 - when plate was used in reconstruction
      ///
      /// This is a 3D array (8 stations x maxTrackCnt() x 20 plates)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array3Dtracks.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& mdFibSel() const;
      /// Set the map of plates used in the main detector track reco
      void setMDFibSel( const std::vector< int >& value );

      /// Map of plates used in overlap detector track reconstruction
      ///
      /// The values can be of elements can be:
      /// * 0 - when plate was not used in reconstruction
      /// * 1 - when plate was used in reconstruction
      ///
      /// This is a 3D array (8 stations x maxTrackCnt() x 20 plates)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array3Dtracks.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& odFibSel() const;
      /// Set the map of plates used in the overlap detector track reco
      void setODFibSel( const std::vector< int >& value );

      /// Track X coordinate in ALFA detector coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& xDetCS() const;
      /// Set the track X coordinate in the ALFA detector coordinate system
      void setXDetCS( const std::vector< float >& value );

      /// Track Y coordinate in ALFA detector coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& yDetCS() const;
      /// Set the track Y coordinate in the ALFA detector coordinate system
      void setYDetCS( const std::vector< float >& value );

      /// @}

      /// @name Information from LocRecCorrEvCollection and
      ///       LocRecCorrODEvCollection
      /// @{

      /// Track X coordinate in the LHC coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& xLhcCS() const;
      /// Set the track X coordinate in the LHC coordinate system
      void setXLhcCS( const std::vector< float >& value );

      /// Track Y coordinate in LHC coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& yLhcCS() const;
      /// Set the track Y coordinate in the LHC coordinate system
      void setYLhcCS( const std::vector< float >& value);

      /// Track Z coordinate in LHC coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& zLhcCS() const;
      /// Set the track Z coordinate in the LHC coordinate system
      void setZLhcCS( const std::vector< float >& value );

      /// Track X coordinate in the roman pot coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& xRPotCS() const;
      /// Set the track X coordinate in the roman pot coordinate system
      void setXRPotCS(const std::vector<float>& value);

      /// Track Y coordinate in the roman pot coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& yRPotCS() const;
      /// Set the track Y coordinate in the roman pot coordinate system
      void setYRPotCS( const std::vector< float >& value);

      /// Track X coordinate in the station coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& xStatCS() const;
      /// Set the track X coordinate in the station coordinate system
      void setXStatCS( const std::vector< float >& value );

      /// Track Y coordinate in the station coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& yStatCS() const;
      /// Set the track Y coordinate in the station coordinate system
      void setYStatCS( const std::vector< float >& value );

      /// Track X coordinate in the beam coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& xBeamCS() const;
      /// Set the track X coordinate in the beam coordinate system
      void setXBeamCS( const std::vector< float >& value );

      /// Track Y coordinate in the beam coordinate system
      ///
      /// This is a 2D array (8 stations x maxTrackCnt() )
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2D.
      ///
      /// Variable stored in TrackingData tree i.e. first ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< float >& yBeamCS() const;
      /// Set the track Y coordinate in the beam coordinate system
      void setYBeamCS( const std::vector< float >& value );

      /// @}

      /// @name Information from RawDataContainer
      /// @{

      /// Trigger rate
      ///
      /// This is 1D array with 8 elements, one per pot.
      ///
      /// Variable stored in EventHeader tree i.e. second
      /// ALFAData object in the ALFADataContainer.
      ///
      const std::vector< int >& scaler() const;
      /// Set the trigger rate
      void setScaler( const std::vector< int >& value );

      /// Trigger pattern 
      ///
      /// It is a 2D array (8 stations x 16 bits)
      /// The values can be of elements can be:
      /// * 0 - false
      /// * 1 - true
      ///
      /// @warning
      /// This variable is most probably not filled.
      ///
      const std::vector< int >& trigPat() const;
      /// Set the trigger pattern
      void setTrigPat( const std::vector< int >& value );

      /// @}

      /// @name Information from DigitCollection
      /// @{

      /// Map of fiber hits in main detectors
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 3D array (8 stations x 20 plates x 64 layers)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array3DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& mdFiberHits() const;
      /// Set the map of fiber hits in the main detectors
      void setMDFiberHits( const std::vector< int >& value );

	   /// Total number of fibers fired in a layer in a main detector
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 2D array (8 stations x 20 plates)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& mdMultiplicity() const;
      /// Set the total number of fibers fired in a later of a main detector
      void setMDMultiplicity( const std::vector< int >& value);

      /// @}

      /// @name Information from ODDigitCollection
      /// @{

      /// Map of fiber hits in overlap detectors with the positive X coordinate
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 3D array (8 stations x 20 plates x 64 layers)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array3DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& odFiberHitsPos() const;
      /// Set the map of fiber hits in overlap detectors with positive X coord.
      void setODFiberHitsPos( const std::vector< int >& value );

      /// Map of fiber hits in overlap detectors with the negative X coordinate
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 3D array (8 stations x 20 plates x 64 layers)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array3DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& odFiberHitsNeg() const;
      /// Set the map of fiber hits in overlap detectors with negative X coord.
      void setODFiberHitsNeg( const std::vector< int >& value);

      /// Total number of fibers fired in a layer in an overlap detector with
      /// positive X coordiante
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 2D array (8 stations x 20 plates)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& odMultiplicityPos() const;
      /// Set the total number of fibers fired in a layer in an overlap detector
      /// positive X coordinate
      void setODMultiplicityPos( const std::vector< int >& value );

      /// Total number of fibers fired in a layer in an overlap detector with
      /// negative X coordiante
      ///
      /// The values can be of elements can be:
      /// * 0 - when fiber was empty
      /// * 1 - when fiber was hit
      ///
      /// This is a 2D array (8 stations x 20 plates)
      /// transformed to a 1D vector. In order to decipher the
      /// index see @ref array2DFibers.
      ///
      /// Variable stored in EventHeader tree i.e. second ALFAData
      /// object in the ALFADataContainer.
      ///
      const std::vector< int >& odMultiplicityNeg() const;
      /// Set the total number of fibers fired in a layer in an overlap detector
      /// negative X coordinate
      void setODMultiplicityNeg( const std::vector< int >& value );

      /// @}

   }; // class ALFAData_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::ALFAData_v1, SG::AuxElement);

#endif // XAODFORWARD_VERSIONS_ALFADATA_V1_H
