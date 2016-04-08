/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @mainpage xAODCaloRings package

  @author Werner Freund <wsfreund@cern.ch>

  $Revision: 670601 $
  $Date: 2015-05-28 16:22:49 +0200 (Thu, 28 May 2015) $

  @section xAODCaloRingsOverview Overview

  This package holds the Athena/ROOT-readable implementation of the reconstructed
  ringed calorimeter information of the detector. It also holds the
  ROOT-readable information of the configuration used to extract this
  information, which may be saved in the file as metadata.

  @section xAODCaloRingsClasses Main Classes

  The main class(es) of the package are the following:
     - xAOD::CaloRings: Typedef to the latest <code>CaloRings</code>
       class version.

     - xAOD::CaloRings_v1: The current latest calorimeter rings 
       holder implementation. It contains a vector of ElementLinks into its
       holden xAOD::RingSet_v1's.

     - xAOD::RingSet: Typedef to the latest <code>CaloRings</code>
       class version.

     - xAOD::RingSet_v1: The current latest calorimeter ring set implementation.
       It holds the energy (Et) for a set of concentric calorimeter rings. 
       This set of rings is within the same layer depth of the Calorimeter,
       here denominated as Calorimeter Joint Layer
       (<code>CalJointLayer</code>). Check CaloRingsDefs.h for more details.

     - xAOD::RingSetConf: Typedef to the latest <code>RingSetConf</code> class
     version.

     - xAOD::RingSetConf_v1: The current latest configuration information at the
       RingSet level. It contains each ring eta/phi step width, the number of rings,
       the calorimeter layers used to build the RingSet, its
       CalJoint[Layer/Section] and its [start/end][Layer/Section]Idx on a
       non-segmented representation. Its information can be copied into a simpler
       structure <code>RawConf</code>, which allows caching its information.

  @htmlinclude used_packages.html

  @include requirements
*/
