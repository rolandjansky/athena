/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODCoreCnv Package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 583869 $
   $Date: 2014-02-18 12:31:09 +0100 (Tue, 18 Feb 2014) $

   @section xAODCoreCnvOverview Overview

   This package houses the code that runs as a first step of an xAOD slimming
   job. The code reads in an auxiliary store object of any type (implementing
   the SG::IAuxStore interface), and "wrapps" it into either an
   xAOD::AuxContainerBase or an xAOD::AuxInfoBase object.

   Neither of these wrapper objects have any persistent variables of their own,
   they just report all variables of the wrapped object as dynamic variables,
   which Athena I/O then writes out into individual branches in the output
   file.

   @section xAODCoreCnvAlgorithms Algorithms

   There is a single useful algorithm in the package at the moment,
   xAODMaker::AuxStoreWrapper. It can be configured to prepare just some
   selected, or all auxiliary store objects for slimming. Needs to be run
   as the first algorithm, before any of the auxiliary store objects would
   be read into StoreGate by some other algorithm.

   @section xAODCoreCnvUsage Usage

   After the algorithm is finished, the event store can no longer retrieve the
   auxiliary store objects as their original, specific type
   (e.g. xAOD::CaloClusterAuxContainer_v1), the event store will just hold
   proxies to generic xAOD::AuxContainerBase and xAOD::AuxInfoBase objects.

   This means that while the output item list looks like the following after
   reconstruction:

   <code>
     StreamAOD.ItemList += [ "xAOD::CaloClusterContainer_v1#Clusters" ]<br/>
     StreamAOD.ItemList += [ "xAOD::CaloClusterAuxContainer_v1#ClustersAux." ]
   </code>

   , after running this algorithm, one needs to instead write something like:

   <code>
     StreamAOD.ItemList += [ "xAOD::CaloClusterContainer_v1#Clusters" ]<br/>
     StreamAOD.ItemList += [ "xAOD::AuxContainerBase#ClustersAux." ]
   </code>

   Adding the auxiliary variable selection at the end of the item string,
   following the latest formalism, now allows us to slim away variables
   which were read in as "static" variables.

   @htmlinclude used_packages.html

   @include requirements
*/
