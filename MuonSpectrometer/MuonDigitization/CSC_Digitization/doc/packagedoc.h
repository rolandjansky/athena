/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page CSC_Digitization_page Short Description of CSC Digitization

 - This is a package to simulate the charge (in number of equivalent electrons) induced on the segmented 
CSC cathode strips by the avalanche on the anode wires given a Geant4 hit in the sensitive gas. For a 
given hit, the number primary and secondary ionisations electrons are simulated and the charge collected 
at the anode is obtained, given the gas gain, from empirical parameterization. This charge is divided 
equally between the 2 cathodes (precision and non-precision coordinates) and the empirical charge 
distribution is foled in given the charge on each strip. This process is repeated for all the hits in the 
events, integrating the total charge on each strip. The amplifier noise is added at the end.

 - Note the simulation of the electronic response, that is, the ADC samplings for each strip is done 
elsewhere by taking the charge on each strip and folding in the signal shape. 

 - CSC_Digitizer.h does the digitization, that is the charge finding on the strips. This is called by 
CscDigitBuilder.h, the top algorithms that runs the digitization.

 - For more detailed information, see the note on the Muon Digitization.

 - for questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov

*/
