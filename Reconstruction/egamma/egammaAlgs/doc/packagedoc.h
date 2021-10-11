/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaAlgs_page egammaAlgs Package

This package collects the algorithms used by egamma reconstruction. More information about the main algorithms is provided in egammaRec.

The main reconstruction of electron and photons is made by the following algorithms:

- egammaTopoClusterCopier: select topoclusters
- egammaSelectedTrackCopy: select tracks that are matched to topocluster
- EMBremCollectionBuilder: refit tracks with GSF
- EMVertexBuilder: builds ID vertices
- egammaRecBuilder: build egammaRec objects from tracks, vertices and clusters
- photonSuperClusterBuilder: create supercluster for photons
- electronSuperClusterBuilder: create superclusters for electrons
- topoEgammaBuilder: create the final superclusters

<center>
@image html egammaAlgs_algos.svg width=80%
</center>

An algorithm to perform truth associations is also provided, as well as some utility algorithms.

@author H. Ma <hma@bnl.gov>
@author A. Poppleton <Alan.Poppleton@cern.ch>
@author S. Rajagopalan <srinir@bnl.gov>
@author M. Wielers <Monika.Wielers@cern.ch>
@author A. Khodinov <khodinov@bnl.gov>
@author F. Derue <derue@lpnhe.in2p3.fr>
@author T. Koffas <Thomas.Koffas@cern.ch>
@author J. Mitrevski <Jovan.Mitrevski@cern.ch>
@author C. Anastopoulos <Christos.Anastopoulos@cern.ch>
@author D. Divalentino <david.di.valentino@cern.ch>
*/

