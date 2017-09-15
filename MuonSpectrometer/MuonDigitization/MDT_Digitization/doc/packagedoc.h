/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MDT_Digitization_page MDT_Digitization package
	@author Daniela.Rebuzzi <daniela.rebuzzi@pv.infn.it>
	@author Niels Van Eldik <nveldik@nikhef.nl>
	
@section MDT_Digitization_MDT_DigitizationIntro Introduction

Package to generate MDT digits starting from Geant4 hits.
The MDT Digit object consist of an Offline ID, a TDC count and an ADC count.

@section MDT_Digitization_MDT_DigitizationOverview Class Overview

This package contains the MDT_Digitizer algorithm. 
It takes care of all the steps necessary to simulate the MDT tube response and
to generate MDT Digits out of MDT Hits (the Digits -> RDOs conversion is
performed by the given converter - RDOs are the persistified objects):

      - Conversion from SimID to OfflineID
      - Conversion from impact parameter to drift time
      - Calculation of the time structure of the event 
      - Trigger match for the digits
      - Total time -> TDC counts conversion for the "good" digits

@section MDT_Digitization_MDT_DigitizationDetails Working principles

In the MDT_Digitizer, the following effects taken into account:

       1.  Drift time + charge by detailed simulation of tube response
       
       2.  Time of flight (Tof) of the particle from its generation vertex to
           the tube (G4 global time)
	   
       3.  The bunch time relative to the current bunch (for pile-up)
       
       4.  Propagation delay of the signal along the tube
       
       4a. Additional delays due to cables/electronics
       
       5.  Matching and masked window
                                                     
             |____masked_____|bunch crossing offset|______matching ______|     
	     
          - Digits coming from hits in the masked window are marked as
            "masked" and will have TDC and ADC counts set to zero
          - Hits in the matching window will produce normal digits   
          - If a hit lies outside of both windows no Digit is created
	  
       6. Dead time
          For a given tube the hits are sorted on time (drift + tof + prop + bunch)
          From first hit we initialize the deadtime = time_hit1 + adc_hit1 + deadwindow
          No additional hits will be created if the time of a second hit is smaller than the 
          deadtime, if a hit with t > deadtime is found the deadtime is reset to: 
           deadtime = time_hit2 + adc_hit2 + deadwindow
      
       7. Optionally the digitizer can check whether the MDTSimHits are ok,
          corrupted hits (i.e. hits with invalid tube radius, or invalid
	  position in the tube or with tof > 10*minTof) are discarted

@section MDT_Digitization_MDT_DigitizationTools Digitization Tools

For the r->t conversion, two different tools are available and selectable via
jobOptions, setting the property DigitizationTool of the MDT_Digitizer:

	1. MDT_Response_DigiTool (default): detailed simulation of MDT tube
	response, including cluster size fluctuation, diffusion and the ADC
	response (for the slewing corrections)
	
	2. RT_Relation_DigiTool: which provides a r->t conversion using an external
	rt relation, coming from data or from Garfiled simulation, for example
	(here the ADC count is set to a fixed value)
	
@section MDT_Digitization_ExtrasMDT_Digitization Extra Pages

 - For more detailed information, see the note on the Muon Digitization.

 - for questions or comments: 
 		daniela.rebuzzi@pv.infn.it
		nveldik@nikhef.nl


*/
