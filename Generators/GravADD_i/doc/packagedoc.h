/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page GravADD_i_page GravADD_i
@author Nele Boelaert  (at_cern.ch)

@section GravADD_i_introductionGravADD_i Introduction

GravADD is a generator for gravitational scattering (GS) and black hole (BH) production in large extra dimensions. It has been implemented in the ATLAS software framework and has become available from release 15.6 onwards.

References:
- More info about the physics: hep-ph/0608210
- An internal note about the implementation: ATL-PHYS-INT-2010-012
- The twiki GravADDForAtlas: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/GravADDForAtlas

The physics model used is the ADD model (hep-ph/9803315, hep-ph/9807344), but with the extra requirement that the 4-dimensional membrane has a finite width, which is motivated by physical arguments. The width of the membrane is parameterised by a parameter Ms, which has the dimension of mass; 1/Ms is a measure of the width (see the references listed above for more info). 

GravADD generates three different types of events: QCD, gravitational scattering (GS) and black hole (BH) events. The generator assigns a flag, gravev, to indicate if the event is a QCD (gravev=0), a GS (gravev=1) or a BH (gravev=2) event. In the Athena implementation, this flag is stored in the StoreGate's weight container, allowing the user to retrieve this information afterwards in the analysis stage. 

In a GS event, quantized Kaluza-Klein (KK) modes occur as intermediate states and, in order to calculate the scattering amplitude, the sum of all modes has to be made. But due to the finite width of the membrane, KK states with a mass above Ms are effectively cut off. So only KK states with a mass below Ms are included in the propagator. 

In hadron collisions, BHs are expected to form when the interacting partons come closer than twice the Schwarzschild radius. This sets a minimum on the BH mass. Another limit on the black hole mass comes from the existence of the finite width of the membrane, because, in the approximation of a narrow width, the membrane cannot be more extended than the black hole itself, meaning that the Schwarzschild radius should not be smaller than 1/Ms 

The BH generation is done with the Charybdis generator. The current version of Charybdis is 1.003, but an upgrade to Charybdis 2 is foreseen (soon). There is the possibility to switch of BH production in GravADD. 


@section GravADD_i_paramsGravADD_i Parameters

- MP = the fundamental Planck Scale defined as in (hep-ph/0608210). This is the default definition in GravADD, but the user can change via the jobOption to the definition used by the Particle Data Group.
- Ms = Kaluza Klein mass cutoff (1/Ms = measure of the width of the membrane)
- n = number of extra dimensions, with (n+4) the total number of dimensions. 
- Meff = the effective Planck Scale which is used for the calculation of the gravitational scattering amplitude, which is a function of MP, Ms and n. 

Orthogonal set of parameters: Meff, Ms/MP (relative KK mass cutoff) and total number of dimensions.


@section GravADD_i_implementationGravADD_i Implementation in Athena

The code is split over two separate packages, namely Generators/PythiaExo_i and Generators/GravADD_i. 

The GS part is implemented in PYTHIA 6.4 by actual changes in the code. A PythiaGS library was created for this purpose in the Generators/PythiaExo_i package. Basically, the event weight for a non BH event is reweighted with a factor sigma(QCD+GS)/sigma(QCD) in order to take into account GS. In order to be able to access the high-pT phase space, PythiaGS uses a modified version of the PYEVWT(WTXS) routine, and runs by default with the option mstp(142) =1. 

The BH generator is connected to PythiaGS as an external process using the UPINIT and UPEVNT routines that come with PYTHIA, and are called after initialisation with PYINIT USER. 

The QCD processes are selected in the normal way, using the MSUB array in PYTHIA. 

The package Generators/GravADD_i has been created to contain the code for the BH production and for reading in the parameters. 


@section GravADD_i_ExampleGravADD_i Example JobOption

jobOption file to initialize ("pyinit user GRAVADD") and set model parameters (a list with all options will be given later on)

- from PythiaExo_i.PythiaGS_iConf import PythiaGS
- job +=PythiaGS()
- job.PythiaGS.PythiaCommand =["pyinit user GRAVADD","gravADD 1 8"]

Warning: GravADD only generates weighted events, so you should always fill your analysis histograms with pari(10), which is stored in the weight container.


@section GravADD_i_ModelparamsGravADD_i JobOption Parameters

Model parameters are set in the JobOption file, GravADD_i contains code to read them in

Use: "gravADD <index> <value>"

All complete list of all parameters: index, name, type, default, description:
- 1, TOTDIM, int, 10, total number of dimensions: (n+4)
- 2, CMEN, double, 7000., center of mass energy in GeV
- 3, MSPRAT, double, 1., Ms/MP
- 4, MEFF, double, 1000., Meff
- 5, PLDEF, int, 1, Planck mass definition, 1:  as defined in hep-ph/0608210; 2:  as defined by the Particle Data Group
- 6, GSONLY, int, 0, 0: GS and BH; 1: only GS but no BH 



@section GravADD_i_TipsGravADD_i Tips for analysis

Note that since GravADD generates weighted events, the user should always fill analysis histograms with the event weight pari(10), which is stored as third entry in the event weight container. At the end of the event loop, the histograms need to be divided by the bin width and the sum of weights of all events, and multiplied with the total integrated cross section, in order to convert the results to nb/(dimension of the horizontal axis). The total integrated cross section of event samples generated in Athena, is stored as MetaData in the file. 

The flag gravev that GravADD uses to distinguish the three types of events (QCD, GS and BH) is stored as 4th entry in the event weight container. 




*/
