/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Ian Hinchliffe (I_Hinchliffe@lbl.gov) Jan 29, 2003

        updated Ewelina Lobodzinska (ewelina@mail.desy.de) Jan 12, 2008

The installed library   TruthHelper in
Generators/GenAnalysisTools/TruthHelper
 provides a set of predicates that can be used in order to select
 partilces of certain types. It runs against the HepMC events that are
 produced either by GeneratorModules or GenzModule and hides the
 complex status codes from the user.

The available predicates are as follows

 IsGenerator  returns true on particles that were part of the
  event that came from the event generator. Partilces produced by
  Geant are returned false

 IsGenNonInteracting returns true on neutrinos and other non
  interacting particles such as those from supersymmetry. It returns a
complete list including any geant secondaries. Is has more partilces
  than results from the combination of IsGenStable and a subseqent
  selection on the IsGenNonInteracting PDG ID codes.

 IsGenStable returns  true on all stable particles. In the case of events
  from GenzModule these are all the partilces that are passed as input
  to Geant as either non-interacting or as particles that are
  subsequently interact and decay in Geant

 IsGenSimulStable returns true on all stable particles during generation an
d simulation

 IsGenInteractinging is a subset of IsGenStable that excludes non
  interacting particles. It includes electrons, photons, muons, and
  hadrons that are considered as imput to geant such as pions, kaons
  and K_L.

 IsGenType allows a list of PDG ID codes to be specified and
  returns true on all the partilces in the record (independent of
  their status codes) that have these the specifed code (or its
  negative) For example code 11 would return all electrons  and
positrons including
  those from Geant showers. Code 24 would find all the W bosons,
  (including any maked as documentaries which may not be physical)

 IsConversion returns true for the particles that convert into the e+e-
 pair

 IsBremsstrahlung returns true for the input particles in the Bremsstrahlun
g process



@htmlinclude used_packages.html

@include requirements

*/
