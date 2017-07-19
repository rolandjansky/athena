/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigMissingETHypo_page TrigMissingETHypo Package

The package provides the MissingET HYPO algorithm for 
both LVL2 and EF.

@author  Kyle Cranmer <cranmer@cern.ch> 
         Diego Casadeii <Diego.Casadei@cern.ch>
         Rashid Djilkibaev <rashid@physics.nyu.edu>
         Till Eifert <till.eifert@cern.ch>  
         Xiaowen Lei  <xiaowen@physics.arizona.edu>
         Allen Mincer <allen.mincer@nyu.edu>

@section TrigMissingETHypo_TrigMissingETHypoIntro Introduction

The package provides the hypothesis algorithm for both LVL2 and 
EF MissingET. The FEX algorithms currently return 
etx and ety, the x and y components of missing Et, 
Et, equal to the sqrt(metx^2+mety^2), and sumEt, the scalar 
sum of the energy projected onto the transverse plane.
They may eventually also return sigmaEt, an estimate of the 
uncertainty in et.

The algorithm passes an event based on the values of missing Et
and sumEt as presented in the properties description below.

@section TrigMissingETHypo_TrigEFMissingETAlgorithm Algorithm properties

Hypothesis thresholds are defined via MissingETCut and SumETCut.
The algorithm cuts on both missing transverse energy
(MET), the sqrt(METx^2+METy^2), and total scalar transverse
energy sum (SET) via MET > MissingETCut  and 
SET > SumETCut, both of which are in MeV.

The CutType flag is used to apply logic to combine these cuts. 
If CutType<-1.5, the algorithm passes the event if either the 
Missing ET OR Sum ET cuts are passed. If -1.5< CutType< 0.5, the 
passes the event only if both the Missing Et AND Sum Et cuts are 
passed.  Currently, the event fails if CutType > -0.5.  If the 
FEX algorithm is eventually modified to return sigmaEt, setting 
CutType > 0.0 will allow the Hypo algorithm to pass an event 
if et > Cutype * sigmaEt. 

doMuonCorrection is used to determine whether or not the calorimeter
quantities are corrected by muons.  The L2 and EF fex algorithms
adds the muon contribution if muons are defined as part of the chain.  
However, the muon information is stored as the last component in the 
TrigMissingEtComponent vector. If doMuonCorrection is set to False,  
TrigMissingETHypo will subtract the muon contribution giving back the
value without the muon.  

Two properties, Forward and Cenral, are provided to give some flexibility
should fake MET arise in particular regions of the calorimeter.  Summary 
of properties:

property                   [default]               meaning

"METLabel"           "TrigEFMissingET"       Name of the input feature 
"MissingETCut"       Depends on signature    Threshold for MissingEt (MeV)
"SumETCut"           Depends on signature    Threshold for the SumEt (MeV)
"CutType"            -3.0                    Select logical condition 
"doMuonCorrection"   true                    Retain muon correction  
"doMonitoring"       false                   Switch on/off monitoring (not in use)
"doTimers"           true                    Switch on/off internal timers 
"Central"            0.                      If <0 subtract FCAL contributions 
                                             If >0 require FCAL_MET<Central" 
"Forward"            0.                      If <0 require calo_MET<|Forward|
                                             If >0 require calo_MET<Forward AND muon_MET<Forward

@section TrigMissingETHypo_TrigMissingETHypoOverview Class Overview

Currently TrigEFMissingETHypo::hltExecute does the full event by
event work of the algorithm.

Information computed by the HLT MET Fex is stored into objects 
of the TrigMissingET class, containing two kinds of information:

  * basic information (global quantities):
        o the 3 components (Ex, Ey, Ez) of the missing ET vector (float)
        o the scalar sums SumE and SumET of the total and transverse energy (float)
        o one event flag (int) 
  * auxiliary information (from all components):
        o a vector of objects of the TrigMissingEtComponent class 

Both TrigMissingET and TrigMissingEtComponent are defined and implemented in 
the  Trigger/TrigEvent/TrigMissingEtEvent package.
*/
