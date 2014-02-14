/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODTriggerCnv package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 575635 $
   $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $

   @section xAODTriggerCnvOverview Overview

   This package holds the components (algorithms and tools) that can be used
   to translate the LVL1_ROI object that was used in AOD files, into the
   xAOD RoI objects/containers.

   Most users will only be interested in the python helper function
   xAODRoICreator. But it is also possible to make use of the converter tools
   of the package from the outside if necessary.

   @section xAODTriggerCnvTools Tool Interfaces

   It is possible to use the following tool interfaces to create xAOD objects
   out of AOD ones, from other packages as well:
     - xAODMaker::IEmTauRoICnvTool: Tool creating xAOD::EmTauRoIContainer.
     - xAODMaker::IJetRoICnvTool: Tool creating xAOD::JetRoIContainer.
     - xAODMaker::IMuonRoICnvTool: Tool creating xAOD::MuonRoIContainer.
     - xAODMaker::IEnergySumRoICnvTool: Tool creating xAOD::EnergySumRoI.
     - xAODMaker::IJetEtRoICnvTool: Tool creating xAOD::JetEtRoI.

   @htmlinclude used_packages.html

   @include requirements
*/
