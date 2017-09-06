/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4PhysicsLists_page G4PhysicsLists
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4PhysicsLists_G4PhysicsLists Introduction

This package provides an interface to all the Geant4 physics lists to be included in the simulation.  The lists currently supported are:
  
        - Fast_Physics
         Only transportation is activated. 
        
        - ExN01
         ExN01PhysicsList.cc defines only a geantino and a transportation
        process. Though cutoff will not be applied onto geantino, a dummy
        value must be set.

        - ExN02
         In this example, all the so called 'electromagnetic processes' are
        introduced for gamma, charged leptons, and charged hadrons (see the
        method PhysicsList::ConstructEM())
        
        - ExN03
         gamma: conversion, compton scattering, photoelectric effect
        e+-: msc, ionization, bremsstrahlung, annihilation
        mu+-: msc, ionization, bremsstrahlung, pair production
        plus decay processes for all particles.

        - QGSP
         Up to G4.7.1 QGSP is like QGSP_GN, without gamma- and electro-nuclear
        reactions.  Starting from g4.8.1, QGSP is basically QGSP_GN, with the 
        new multiple scattering implementation.

        - QGSP_BERT
         as QGSP_GN for energies above 3GeV. Below 3 GeV the Bertini cascade is 
        used for nucleon and pion induced reactions. The price to pay is reduced 
        CPU performance.  It does not include gamma and electro nuclear physics.

        - QGSP_EMX
         An experimental physics list introduced with G4.8.1. It uses a slightly
        different EM physics, activating the subcutoff option to produce more
        secondaries near volume boudaries. Coupled with revised range cuts, this 
        should allow some CPU time gain

        - QGSP_EMV
         Another CPU time optimized physics list. Starting from g4.8.1, it uses 
        the same EM physics as g4.7.1.

        - QGSP_BERT_EMV
         A combination of the hadronic physics models in QGSP_BERT and the 
        electromagnetic physics models of QGSP_EMV.  

        -QGSP_BERT_HP
         is just like the QGSP_BERT physics list, with the addition of high-precision
        neutron physics models (particularly useful for modeling low energy neutrons
        in cavern background and radiation).  Only neutrons below 20 MeV are affected.

        - QGSP_BIC, QGSP_BIC_HP
         Adds to the standard QGSP physics list the Binary Cascade model for protons
        and neutrons below 10 GeV (ie no LEP model).  _HP again denotes high-precision 
        neutron physics models.

        - QGSP_BERT_TRV, QGSP_BERT_NQE
         Additional physics lists based on the QGSP_BERT models.  NQE for no quasi-
        elastic scattering, and TRV for a 5 (5.4) GeV cut for the LHEP (BERT) 
        hadronic physics models.

        - LHEP
         is the most CPU performent variant. It uses the LEP and HEP parametrized
        models for inelastic scattering. These models parametrize the final states 
        individual inelastic reactions, so you will not see resonances, and detailed 
        angular distributions for O(100MeV) reactions may not be perfect. The average 
        quantities will be well described. Gamma- and electro- nuclear reactions 
        are modeled using chiral invariant phase-space decay for energy transfers 
        below ~3 GeV in the case of incident electrons and positrons, and for gamma 
        energies below ~3 GeV. For higher energies quark-gluon string model is used,
        with the string excitation cross-sections being calculated in single 
        interaction approximation.

        - LHEP_BERT
         is as LHEP for energies above 3GeV. Below 3 GeV the Bertini cascade is 
        used for nucleon and pion induced reactions. The price to pay is reduced CPU
        performance. It does not include gamma and electro nuclear physics.
 
        - QGSC, QGSC_BERT
         Just like QGSP, except applying the CHIPS model for nuclear de-excitation.

        - QGSC_EFLOW
         based on QGSC, but with a different set up for the excited nucleus created 
        by the high energy interaction.  It tends to give a good description of 
        target fragmentation products.

        - FTFP, FTFP_BERT, FTTF_BIC, FTFC
         These lists use an alternate string model, the FRITIOF model.  C again is
        for CHIPS, BERT for the Bertini cascade, and BIC for the binary cascade.

@section G4PhysicsLists_G4PhysicsLists Class Overview

This package is only an interface, so it has no classes!



*/
