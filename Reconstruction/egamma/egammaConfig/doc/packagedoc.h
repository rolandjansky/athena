/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaConfig_page egammaConfig Package

This package is responsible for configuring the electron and photon reconstruction and identification algorithms in the new style.

The following reconstruction algorithms are implemented (Text below needs updating)

 - The old one, implemented in egammaBuilder and managed through the python script egammaGetter, which is seeded from the electromagnetic (EM) calorimeters, starts from clusters reconstructed in the calorimeters by the SlidingWindowFinder (https://twiki.cern.ch/twiki/bin/view/Atlas/SlidingWindowClustering#Sliding_Window_Clustering). For each cluster, with ET(seed)>3GeV in the cluster container the best inner detector track is searched for within a given E/p range. The complexity of the extrapolation (coordinate systems, etc) is hidden in ExtrapolateToCaloTool. It then builds identification variables. As of release 21 this is no longer run.

 - An alternative algorithm, implemented by topoEgammaBuilder and also managed by the python script egammaGetter, is based on topoclusters. It attempts to recover electrons that emit brem photons by creating "superclusters." Superclusters can also be used in photons to especially help converted photons. This is the default egamma algorithm for central electrons and photons as of release 21.

 - An algorithm, implemented in egammaForwardBuilder and managed through the python script egammaForwardGetter, is dedicated to the reconstruction and identification of electrons in the forward region of ATLAS (2.5<|eta|<4.9). In contrast to the egamma and topoEgamma builders, this algorithm can use only the information from the calorimeters, as the tracking system is limited to |eta|<2.5, and the topological clusters (instead of SW clusters). The pre-selection and ID are done in the same algorithm. The variables used to discriminant between electron and hadrons are defined as the topo cluster moments or combination of them. This is done separately in two eta bins: the EMEC IW and the FCal using a cut based technic.

The EDM for the electrons and photons is defined in Event/xAOD/xAODEgamma. Three collections are produced in the xAOD, Electrons, Photons, and ForwardElectrons.

@author H. Ma <hma@bnl.gov>
@author A. Poppleton <Alan.Poppleton@cern.ch>
@author S. Rajagopalan <srinir@bnl.gov>
@author M. Wielers <Monika.Wielers@cern.ch>
@author A. Khodinov <khodinov@bnl.gov>
@author F. Derue <derue@lpnhe.in2p3.fr>
@author T. Koffas  <Thomas.Koffas@cern.ch>
@author J. Mitrevski <Jovan.Mitrevski@cern.ch>
@author C. Anastopoulos <Christos.Anastopoulos@cern.ch>
@author D. Divalentino <david.di.valentino@cern.ch>
@author R. Naranjo <Roger.Naranjo@cern.ch>
*/
