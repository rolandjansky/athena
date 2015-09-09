/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

   @mainpage xAODForward package

   @section rcRetrive Retriving object in Root Core

   @subsection rcContainer Retriving ALFADataContainer

   In the xAOD file ALFADataContainer object is stored. It has to be
   read first with the help of xAOD::TEvent object. When EventLoop
   is used the code will be the following

   @code{.cpp}
   xAOD::TEvent* event = wk()->xaodEvent();
   const xAOD::ALFADataContainer* alfaContainer = NULL;
   if( ! event->retrieve( alfaContainer, "ALFADataContainer").isSuccess() ){
     Error("execute", "Failed to retrieve ALFADataContainer. Exiting." );
     return EL::StatusCode::FAILURE;
   }
   @endcode

   In the code above it is checked if reading of the
   ALFADataContainer was successful. Without it RootCore will issue
   a warning.

   @note
   Objects read from xAOD::TEvent are const pointers.

   The retrived xAOD::ALFADataContainer has interface similar to
   std::vector and contains two ALFAData objects. *The first* object
   is fille with *TrackingData* tree and _the second_ with _EventHeader_
   tree. The trees are described in the twiki page
   https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ALFAntupleAlg

   @warning Both ALFAData object contain all the variables, but the
   variables that are not present in the corresponding tree contain
   default values.

   @subsection rcData Retriving an ALFAData object from ALFADataContainer

   ALFAData objects can be read from the ALFADataContainer with the
   same interface as is used in the std::vector. It means that
   e.g. at() mehtod chan be used.

   In order to read the get the first ALFAData object which contains
   TrackingData information the following code can be used:

   @code{.cpp}
   const xAOD::ALFAData* trackingData = alfaContainer->at(0);
   @endcode

   In order to read the get the first ALFAData object which contains
   TrackingData information the following code can be used:

   @code{.cpp}
   const xAOD::ALFAData* eventData = alfaContainer->at(1);
   @endcode

   Detailed descriptions of the functions providing access to the specific
   variables contain information in which tree the variable is
   stored.

   @section athena Retriving data in Athena

   In the xAOD file ALFADataContainer object is stored and it has to
   be retrived first. It is stored in eventStore. In the xAOD file
   there is only one such object, which means it can be retrived
   without specifing object name. Below an example code is
   presented.

   @code{.cpp}
   const xAOD::ALFADataContainer* alfaContainer = 0;
   CHECK( evtStore()->retrieve( alfaContainer) );
   @endcode

   Retriving ALFAData objects from the container object is common
   for Athena and RootCore, so please consult subsection @ref rcData

   @section array2D Decrypting vector index from serialised 2D array for tracks

   All arrays stored in xAOD are one dimensional. It means that data
   saved in 2D arrays has to be transformed into 1D array.

   In case of the 2D arrays storing information about tracks the
   index of a 1D array is constructed in the following way:

   @verbatim
   potIndex*maxTrackCnt() + trackIndex
   @endverbatim

   where:
    - @c potIndex is the index of the pot ranging from 0 to 7,
    - maxTrackCnt() is the number of maximal number of tracks in one station,
    - @c trackIndex is the index of the track in a station.

   @warning
   Number of entries in the vector does not correspond to number of
   tracks. To say if there is a track or not it has to be checked
   that the value of the variable is different from default one
   (usually -9999).

   In order to extract pot number from the array element index the
   following formula can be used

   @code{.cpp}
   int potIndex = index/maxTrackCnt();
   @endcode

   In order to extract track index from the array element index the
   following formula can be used

   @code{.cpp}
   int trackIndex = index%maxTrackCnt();
   @endcode

   @section array2DFibers Decrypting vector index from serialised 2D array for plates

   All arrays stored in xAOD are one dimensional. It means that data
   saved in 2D arrays has to be transformed into 1D array.

   In case of the 2D arrays storing information about plates the
   index of a 1D array is constructed in the following way:

   @verbatim
   potIndex*numberOfPlates + plateIndex
   @endverbatim

   where:
    - @c potIndex is the index of the pot ranging from 0 to 7,
    - @c numberOfPlates is the number of plates in a detector (20 for main detector and 3 for overlap detector),
    - @c plateIndex is the index of considered plate

   In order to extract pot number from the array element index the
   following formula can be used

   @code{.cpp}
   int potIndex = index/numberOfPlates;
   @endcode

   In order to extract plate index from the array element index the
   following formula can be used

   @code{.cpp}
   int trackIndex = index%numberOfPlates;
   @endcode

   @section array3Dtracks Decrypting vector index with track information from serialised 3D array

   All arrays stored in xAOD are one dimensional. It means that data
   saved in 3D arrays has to be transformed into 1D array.

   In case of the 3D arrays storing information about tracks and
   plates, an index of a 1D array is constructed in the following
   way:

   @verbatim
   potIndex*maxTrackCnt()*numberOfPlates + trackIndex*numberOfPlates + plateIndex
   @endverbatim

   where:
    - @c potIndex is the index of the pot ranging from 0 to 7, 
    - @c maxTrackCnt() is the number of maximal number of tracks in one station,
    - @c numberOfPlates is the maximal number of plates that can be used in track reconstruction - for main detector tracks it is 20, for overlap detectors it is 3,
    - @c trackIndex is the index of the track in a station,
    - @c plateIndex index of considered plate (from 0 to 19 for main detectors or from 0 to 2 for overlap detectors)\

   @warning
   Number of entries in the vector does not correspond to
   number of tracks. To say if there is a track or not it has to be
   checked that the track for this pot and this index was
   reconstructed in one of the detectors using getDetectorPartID().

   In order to extract pot number from the array element index the
   following formula can be used

   @code{.cpp}
   int potIndex = index/(maxTrackCnt()*numberOfPlates);
   @endcode

   In order to extract track index from the array element index the
   following formula can be used

   @code{.cpp}
   int trackIndex = (index/numberOfPlates)%maxTrackCnt();
   @endcode

   In order to extract plate index from the array element index the
   following formula can be used

   @code{.cpp}
   int plateIndex = index%numberOfPlates;
   @endcode

   @section array3DFibers Decrypting vector index with fibers information from serialised 3D array

   All arrays stored in xAOD are one dimensional. It means that data
   saved in 3D arrays has to be transformed into 1D array.
 
   In case of the 3D arrays storing information about fibers, an
   index of a 1D array is constructed in the following way:

   @verbatim
   potIndex*numberOfPlates*numberOfLayers + plateIndex*numberOfLayers + layerIndex
   @endverbatim

   where:
    - @c potIndex is the index of the pot ranging from 0 to 7, 
    - @c numberOfPlates is the maximal number of plates that can be used in track reconstruction - for main detector tracks it is 20, for overlap detectors it is 3,
    - @c numberLayers is the number of fiber layers in a plate, which is equal to 64 for main detectors and 30 for overlap detectors,
    - @c plateIndex is the index of the considered plate of fibers (runs from 0 to 20 for main detectors and from 0 to 2 for overlap detectors)
    - @c layerIndex is the index of considered layer (runs from 0 to 63 for main detectors and from 0 to 29 for overlap detectors)

   In order to extract pot number from the array element index the
   following formula can be used

   @code{.cpp}
   int potIndex = index/(numberOfPlates*numberOfLayers);
   @endcode

   In order to plate index from the array element index the
   following formula can be used

   @code{.cpp}
   int plateIndex = (index/numberOfLayers)%numberOfPlates;
   @endcode

   In order to extract layer index from the array element index the
   following formula can be used

   @code{.cpp}
   int layerIndex = index%numberOfLayers;
   @endcode

   @htmlinclude used_packages.html

   @include requirements
*/
