/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
@page TrigT1CaloSim_page TrigT1CaloSim Package
Author: Alan Watson, Ed Moyse, Pete Faulkner<P>

The Package that simulates the first level calorimeter trigger.

@section TrigT1CaloSim_TrigT1CaloSim Introduction

TrigT1CaloSim is composed of 11 core Athena Algorithms, used for simulating 
the trigger and producing RoIs, CTP inputs, CBNTAA and Bytestream data, plus 
3 others for testing or other specialised operations not used in normal 
simulation jobs. The core algorithms are:

TriggerTowerMaker.h <BR>
CPMTowerMaker.h <BR>
EmTauTrigger.h <BR>
CPCMMMaker.h <BR>
JetElementMaker.h <BR>
JetTrigger.h <BR>
EnergyTrigger.h <BR>
ROD.h <BR>
JEPCMMMaker.h <BR>
CBNTAA_LVL1calo.h <BR>
CBNTAA_LVL1CaloESD.h <BR>

The "Maker" algorithms construct data objects, e.g. Trigger Towers and Jet Elements, which are used by the "Trigger" algorithms to produce "result objects" (i.e. RoIs, CTP words, bytestream data). The CBNTAA routines are used to persistify (save) information to the Combined Ntuple.  TriggerTowers and JetElements are persistified to the ESD (outlined below).  The Class Overview will be presented linking the important Classes in one group, and then different "functionality" packages in seperate groups (ie the Classes needed to persistify/write-to the ESD will be presented in one block).

These algorithms started life in the TrigT1Calo package, which combined both algorithms and data objects in a single package. To allow more flexible use of the trigger algorithm code (e.g. for monitoring as well as simulation) and avoid dependency loops that package was splits, with the Athena Algorithms being separated from the tools and data objects in a new package, TrigT1CaloSim. 


@section TrigT1CaloSim_Class Class Overview

*Core Classes(9):<BR>
-----------------<BR>

TriggerTowerMaker.h - TriggerTowerMaker is responsible for producing the TriggerTower objects, which are used by EmTauTrigger and JetElementMaker, and are persistified to ESD.  TriggerTowerMaker takes as input either GEANT Cells or LAr/Tile TTL1s (simulations of the analogue signal).

EmTauTrigger.h - The algorithm responsible for simulating the Em/tau calo trigger.

JetElementMaker.h - JetElementMaker reads in TriggerTower objects and sums them together to produce JetElement objects.

JetTrigger.h - JetTrigger Simulates the hardware jet trigger.  It reads in JetElements and the TriggerMenu (produced by TrigT1Config).

EnergyTrigger.h - EnergyTrigger uses EnergyCrate and JetElement objects in order to closely follow the layout of the hardware.

ROD.h - The algorithm responsible for simulating the Read Out Drivers.  Creates S-link words and sends them to the RoI builder simulation (TrigT1RoIB), provides multiplicity counts to the CTP simulation (TrigT1CTP) and performs the "jet estimator energy trigger". 

CPMTowerMaker.h - Forms CPMTower objects (simplified version of TriggerTower) for CPM bytestream simulation.
CPCMMMaker.h - Forms data objects for CP CMM data bytestream simulation.
JEPCMMMaker.h - Forms data objects for JEP CMM bytestream simulation. 

*Other Algorithms (5):<BR>
----------------------<BR>

MergeSlices.h - Algorithm to merge multiple time slices of data to emulate multi-slice readout in bytestream. Note that normal Athena simulation does not provide a valid set of successive time slices, so this is a fix-up to allow technical tests only.

Tester.h -  The Tester algorithm checks the performance of the LVL1 simulation.

CBNTAA_LVL1calo.h - Fills the Athena ntuple with TrigT1Calo information. 

CBNTAA_LVL1CaloESD.h - Fills Athena ntuple from objects in TES.  This was a "temporary" fix for Rome data, but is still used in ESDtoCBNT job scripts.

DumpTriggerObjects.h -  The DumpTriggerObjects algorithm shows how to access EmTauROIs.

*Helpful External Links:<BR>
----------------------------

<A href="http://atlas-computing.web.cern.ch/atlas-computing/packages/lvl1Trigger/lvl1TriggerCalo.php"> TrigT1Calo Webpage at CERN </A>

<A href="http://www.ep.ph.bham.ac.uk/user/woehrling/T1CaloESD.html"> ESD TriggerTower and JetElement content </A>

<A href="http://atlas.web.cern.ch/Atlas/GROUPS/DAQTRIG/PESA/egamma/rome/ESDcontents.html"> ESD ROI and missing Et content </A>

<A href="https://uimon.cern.ch/twiki/bin/view/Atlas/JobOptions"> JobOptions interface (link to twiki) </A>

<A href=""> xml interface </A>

<A href=""> TriggerMenu </A>




*/
