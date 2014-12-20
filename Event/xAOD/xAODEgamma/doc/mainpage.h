/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODEgamma package

   @author Jovan Mitrevski <Jovan.Mitrevski@cern.h>
   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   @author Christos Anastopoulos <Christos.Anastopoulos@cern.h>
   @author Anthony Morley <Anthony.Morley@cern.ch>
   @author Bruno Lenzi <Bruno.Lenzi@cern.ch>

   $Revision: 636327 $
   $Date: 2014-12-16 17:34:24 +0100 (Tue, 16 Dec 2014) $

   @section xAODEgammaOverivew Overview

   This package holds the EDM classes describing the output of ATLAS's
   egamma reconstruction.

   @section xAODEgammaClasses Main Classes

   The main class(es) of the package are the following:
      - xAOD::Egamma: Typedef to the latest <code>Egamma_vX</code> class
        version.
      - xAOD::EgammaContainer: Typedef to the latest egamma container
        version.
      - xAOD::EgammaAuxContainer: Typedef to the latest egamma auxiliray 
        container version.
      - xAOD::Egamma_v1: The current latest egamma implementation.

      - xAOD::Electron: Typedef to the latest <code>Electron_vX</code> class
        version.
      - xAOD::ElectronContainer: Typedef to the latest egamma container
        version.
      - xAOD::ElectronAuxContainer: Typedef to the latest egamma auxiliray 
        container version.
      - xAOD::Electron_v1: The current latest egamma implementation.

      - xAOD::Photon: Typedef to the latest <code>Photon_vX</code> class
        version.
      - xAOD::PhotonContainer: Typedef to the latest egamma container
        version.
      - xAOD::PhotonAuxContainer: Typedef to the latest egamma auxiliray 
        container version.
      - xAOD::Photon_v1: The current latest egamma implementation.

   @section EgammaHelpers Helpers
   
   Helper functions are provided for the most common routines and used in reconstruction
   and CP tools. Users should request additions in case of need.
   
   The functions are split in 3 files: 
   EgammaxAODHelpers, ElectronxAODHelpers, PhotonxAODHelpers, EgammaTruthxAODHelpers
   but the users only need:
   <code> #include "xAODEgamma/EgammaxAODHelpers.h" </code>
   



   @htmlinclude used_packages.html

   @include requirements
*/
