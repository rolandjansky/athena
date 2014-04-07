/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage egammaEvent Package

This package provides Event Data Model of egamma objects.

Each egamma (or Electron, Photon) is made of 
  - link to CaloCluster, Track in the ID
  - arrays containing PID variables
  - ... 
  - objects called egDetail like  
      EMShower for EM cluster shower property, 
      EMShowerMinimal,
      EMTrackMatch for matching ID track with EM cluster, 
      EMConvert for photon conversion property, 
      EMBremFit for electron bremsstrahlung recovery property using calorimeter 
      EMTrackFit for electron bremsstrahlung recovery property using .inner detector. 

The class is made by several algorithms located in Reconstruction/egamma/egammaRec : egammaBuilder, a cluster-based reconstruction algorithm of (isolated) electrons and photons, 
softeBuilder, a track-based reconstruction algorithm of (low-pT or non-isolated) electrons, frwdBuilder, a cluster-based alorithm of forward electrons 
and egammaAODBuilder, an algorithm at AOD level which creates several collections based on the ESD collections build by the previous algorithms.

The properties class are made by tools located in Reconstruction/egamma/egammaTools.

EMShower property class data class is made by EMShowerBuilder. 
This class contains information about quantities usedful for electron/photon 
identification such as shower shape variables 
and leakage of showers into the hadronic calorimeters. 

EMTrackMatch is the track-cluster matching property data class. It is made by EMTrackMatchBuilder, which is an algorithm responsible for  the match of a calo cluster with the best matched track in front of the shower.

EMBremFit is made by EMBremsstrahlungBuilder. 
It is responsible for bremsstrahlung recovery using electromagnetic calorimeter. It re-calculates track pT 
accounting for energy of the photons emmitted in a hard bremsstrahlung. 

EMTrackFit is made by EMTrkRefitter.
It is responsible for bremsstrahlung recovery using inner detector track refitting. It re-calculates track pT accounting for energy of the photons emmitted in a hard bremsstrahlung. 

EMShowerMinimal is minimal EM shower property data class, as needed by LVL2. 
It is used in the Trigger/TrigT2Calo package. This class contains information 
about quantities useful for e/g identification in LVL2, such as shower shape variables.

EMErrorDetail gives error matrix associate to the 4-momentum

ElectronAssociation : for association of electron with jet

ElectronConstituent : implementation of the ElectronConstituent class for ParticleJet

ElectronContainer : class to define the container of electrons

Electron : class to define the Electron object

PhotonAssociation : for association of photon wih jet

PhotonContainer : class to define the container of photons

Photon : class to define the Photon object

egDetailContainer : container for detailed egamma information
egDetail : base class for detailed egamma information

egammaContainer : container of egamma objects
egamma : elec/gamma data class. It contains the different egDetails

egammaParamDefs : this file defines enums for egammaParameters.
egammaPIDdefs.h : This file defines the bit-occupation of the PID and OQ flags 
                  of egamma and the enum definition of the identification and 
                  object quality variables
egPID.h : class to hold PID vars egamma information.  


@author H. Ma <hma@bnl.gov>
@author A. Poppleton <Alan.Poppleton@cern.ch>
@author S. Rajagopalan <srinir@bnl.gov>
@author M. Wielers <Monika.Wielers.cern.ch>
@author A. Khodinov <khodinov@bnl.gov>

*/
