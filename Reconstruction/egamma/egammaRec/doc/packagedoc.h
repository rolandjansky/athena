/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaRec_page egammaRec Package

This package is responsible for electron and photon reconstruction and identification algorithms.

At present, three electron and photon reconstruction and identication algorithms have been implemented in the ATLAS ofine reconstruction software, all integrated into one single package and a common event data model.
 - The standard one, implemented in egammaBuilder and managed through the python script egammaGetter, which is seeded from the electromagnetic (EM) calorimeters, starts from clusters reconstructed in the calorimeters by the SlidingWindowFinder (https://twiki.cern.ch/twiki/bin/view/Atlas/SlidingWindowClustering#Sliding_Window_Clustering). For each cluster, with ET(seed)>3GeV in the cluster container the best inner detector track is searched for within a given E/p range. The complexity of the extrapolation (coordinate systems, etc) is hidden in ExtrapolateToCaloTool. It then builds identification variables 
From release 13 egamma objects, having a track, which match a conversion (reconstructed by the conversion reconstruction algorithm) are considered as "Photon" and not "Electron".

 - A second algorithm, implemented in softeBuilder and managed through the python script softeGetter, which is seeded from the inner detector tracks, is optimized for electrons with energies as low as a few GeV, and selects good-quality tracks matching a relatively isolated deposition of energy in the EM calorimeters. The identication variables are then calculated in the same way as for the standard algorithm.

 - A third algorithm, implemented in egammaForwardBuilder and managed through the python script egammaForwardGetter,is dedicated to the reconstruction and identification of electrons in the forward region of ATLAS (2.5<|eta|<4.9). In contrast to the softe and egamma builders the algorithm can use only the information from the calorimeters, as the tracking system is limited to |eta|<2.5, and the topological clusters (instead of SW clusters). The pre-selection and ID are done in the same algorithm. The variables used to discriminant between electron and hadrons are defined as the topo cluster moments or combination of them. This is done separately in two eta bins: the EMEC IW and the FCal using a cut based technic. The forward electron AUTHOR is 8. 

The different algorithms are run independently during reconstruction.
There are three collections in the ESD (ElectronCollection, PhotonCollection 
and softeCollection) as well as two details containers (egDetailContainer and 
SofteDetailContainer), which are necessary to access the shower variables. 
Therefore a true electron may be found with the standard (cluster-based) and 
softe (track-based) collections.


At AOD level, Electrons and Photons are build by egammaAODBuilder.
They are differentiated by a loose track matching the electromagnetic cluster 
and to a conversion object. 
The egamma objects, having a track, 
which match a conversion are considered as "Photon" and not "Electron".
The overlap for electrons reconstructed by the cluster-based and the track-based algorithms is resolved when the AODs are constructed: if the track is common 
to a candidate soft-electron AND a standard egamma, then the objects are 
considered to be the same. 


There are two bits allocated for the "authorship" of each egamma object, see 
egammaEvent/egammaParamDefs.h for possible authors. 

For both electrons and photons the barycentre of the electromagnetic shower is 
estimated in each sampling.
The electromagnetic shower-shape variables are calculated with respect to 
these barycentres in EMShowerBuilder.
Note that there are also two algorithms responsible for bremsstrahlung 
recovery : EMBremsstrahlungBuilder re-calculates track pT accounting for 
energy of the photons emitted in a hard bremsstrahlung. 
EMTrkRefitter implements use od DNA and GSF fitters.

EMPIDBuilder tool is used for electron and photon identification. 
The PID variables can be accessed for both reconstruction algorithms : 
egammaID(egammaPID::MYFAVOURITEID):
identification information (including isem). For possible values of MYFAVOURITEID see egammaEvent/egammaPIDdefs.h.
As references loose, medium and tight selections are provided. 
The possible algorithms as well as discriminating variables (see for example EPiVariables for electron
vs jets discrimination variables, used by the log-likelihood and neural-network procedures),
which enter in the definition of these PID algorithms, are described in
https://twiki.cern.ch/twiki/bin/view/Atlas/ElectronReconstruction.
In addition to cut based (isem) identification, results of the following multivariate analysis tools are available: likelihood, neutral network, H-matrix and boosted decision trees.

Other algorithms are avaliable in the egammaRec package :

convAODRecVxCand : algorithm to recover photons conversions from the eelctron container

egammaAODRender : algorithm to change the clustering and redo pid at AOD level

egammaTrackSlimmer : algorithm to put out the slimmed track collection

CBNTAA_egamma : fill the CBNT for reconstructed objects.

CBNTAA_egTruth : fill the CBNT for truth particles


@author H. Ma <hma@bnl.gov>
@author A. Poppleton <Alan.Poppleton@cern.ch>
@author S. Rajagopalan <srinir@bnl.gov>
@author M. Wielers <Monika.Wielers@cern.ch>
@author A. Khodinov <khodinov@bnl.gov>
@author F. Derue <derue@lpnhe.in2p3.fr>
@author T. Koffas  <Thomas.Koffas@cern.ch>
*/
