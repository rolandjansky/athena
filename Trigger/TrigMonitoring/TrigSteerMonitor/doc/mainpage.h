/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrigSteerMonitor Package
@author Gordon.Fischer@desy.de
@author nedden@mail.desy.de
@author Till.Eifert@cern.ch
@author Tomasz.Bold@cern.ch
@author risler@mail.desy.de



@section TrigSteerMonitorIntroduction
The Package that monitors the Steering decision, Errorcode, RoI

@section TrigSteerMonitor Class Overview

Monitoring the Steering Decision:

TrigChainMoni.h

fills the following histograms:

chainAcceptance_L2_x         Raw acceptance of chains  (Number of events vs chain)

chainAcceptancePS_L2_x       acceptance of chains after Prescale   (Number of events vs chain)

chainAcceptancePT_L2_x       Final acceptance of chains (after Prescale and PassThrough )  (Number of events vs chain)


TrigSignatureMoni.h

chainLength_L2            Length of Chains  (#steps vs chaincounter )

signatureAcceptance_L2_x  Raw acceptance of signatures  (2D histo Number of events as fct of step and stram/group/chain)

			  x-axis: total rate(bin=1), streams (bin=2,...,NStreams+1),
                                  groups (bin=Nstreams+2,...,NStreams+NGroups+1), 
                                  chains (bin=NStreams+NGroups+2,...,NStreams+NGroups+NChains+2)

			  y-axis: input, step1..step_max, raw rate of this stream/group/chain, PS rate=prescaled rate, 

                                  PT rate= rate after passthrough and prescale 


eventsPassingStep_L2_x          Number of events passing (i.e. being executed) a step in L2 for Run Summary (N vs step)

totaleventsPassingStep_L2_x     Number of events passing  a step in L2 for Run Summary (N vs step including events accepted by already terminated chains)


TrigTEMoni.h

TEnumber_L2_x             TE abundance         Number of output TEs vs TE type, summing up all active TEs 

allTEnumber_L2_x          all TE abundance     Number of output TEs vs TE type, summing up all TEs



obsolete: TrigSteerMoni.h

other classes :

TrigRoIMoni.h - monitors the RoIs eta and phi


RoIs_Deta_L2  			RoIs_Deta_L2

RoIs_Dphi_L2  			RoIs_Dphi_L2

RoIs_eta0_L2  			RoIs_eta0_L2

RoIs_phi0_L2                    RoIs_phi0_L2

Initital_RoIs_phi_vs_etaL2      Initital_RoIs_phi_vs_etaL2



TrigErrorMon.h - how often did ErrorCodes occur

ErrorCodes_L2_reason	  		  ErrorCodes_L2_reason

ErrorCodes_L2_action 	  		  ErrorCodes_L2_action

ErrorCodes_L2_steeringInternalReason	  ErrorCodes_L2_steeringInternalReason

ErrorCodes_vs_Chains_L2_reason		  ErrorCodes_vs_Chains_L2_reason 2Dhisto

ErrorCodes_vs_Chains_L2_action 		  ErrorCodes_vs_Chains_L2_action 2Dhisto


*/
