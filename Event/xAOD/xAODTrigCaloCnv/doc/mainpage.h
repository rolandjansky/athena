/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODTrigCaloCnv package

   @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>

   $Revision: 575635 $
   $Date: 2014-03-18 11:58:41 +0100 (Tue, 18 Mar 2014) $

   @section xAODTrigCaloCnvOverview Overview

   This package holds the components (algorithms and tools) that can be used
   to translate the TrigCaloCluster and TrigEMCluster objects that were used in AOD files, into the
   xAOD TrigCaloCluster and TrigEMCluster objects/containers.

   Most users will only be interested in the python helper function
   xAODTrigCaloClusterCreator and xAODTrigEMClusterCreator. But it is also possible to make use of the converter tools
   of the package from the outside if necessary.

   @section xAODTrigCaloCnvTools Tool Interfaces

   It is possible to use the following tool interfaces to create xAOD objects
   out of AOD ones, from other packages as well:
     - xAODMaker::ITrigCaloClusterCnvTool: Tool creating xAOD::TrigCaloClusterContainer.
     - xAODMaker::ITrigEMClusterCnvTool: Tool creating xAOD::TrigEMClusterContainer.

   @htmlinclude used_packages.html

   @include requirements
*/
