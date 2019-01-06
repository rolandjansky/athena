/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigT1TGC_page TrigT1TGC Package

  @author Chihiro Omachi<omati@phys.sci.kobe-u.ac.jp>
  @author Masaya Ishino<Masaya.Ishino@cern.ch>
  @author Naoko Kanaya<Naoko.Kanaya@cern.ch>   

@section TrigT1TGC_TrigT1TGCIntro Introduction

  This package is dedicated to trigger electronic simulation for the endcap
  muon trigger system.
  TGC digits are fed into the simulation where logics of each electronic 
  module is emulated in detail.
  Trigger decision, RoI, pt threshold and charge, from the sector logic
  is given to the Muon CTP Interface.
  The readout line is also emulated and it creats bare-RDO for digits and
  coincidences.

@section TrigT1TGC_TrigT1TGCOverview Class Overview
  
  The package contains the following algorithms:

  - LVL1TGCTrigger::LVL1TGCTrigger: Main Algorithm
  - LVL1TGCTrigger::CBNTAA_TGCMakeCoincidenceOut: Algorithm to create AANT (digits/coincidence)
  - LVL1TGCTrigger::MakeCoincidenceOut: Algorithm to create CWN (almost equivalent to the above)


  The algorithms of the TrigT1TGC package can be configured using configurables.  See TrigT1TGC_jobOptions.py and TrigT1TGCConfig.py:
  @include TrigT1TGC_jobOptions.py

  The properties of LVl1TGCTrigger can be changed by users:
  - @link LVL1TGCTrigger::LVL1TGCTrigger::LVL1TGCTrigger @endlink
  - @link LVL1TGCTrigger::CBNTAA_TGCMakeCoincidenceOut::CBNTAA_TGCMakeCoincidenceOut @endlink

@section TrigT1TGC_TrigT1TGC Special configuration for cosmic run
  User can insert dummy hits to satisfy 2-station coincidence for cosmic muon trigger
  by the endcap muon trigger system. She/he can use CosmicM2.msk to make such a special configuration by
  LVL1TGCTrigger::LVL1TGCTrigger::MaskFileName="CosmicM2.msk"

@section TrigT1TGC_TrigT1TGC Special configuration for a slow charged particle.
  User can use all hits(i.e. associated to next bunch) by LVL1TGCTrigger::CurrentBunchTag=0   

@section TrigT1TGC_TrigT1TGC Extra Packages

*/
