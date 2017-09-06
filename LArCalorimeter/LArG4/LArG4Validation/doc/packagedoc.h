/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page LArG4Validation_page The LArG4Validation Package

 - This class is for the validation of simulation, in particular for the liquid argon calorimetry

 - generate: a script to run a variety of simulation flavors with output in ntuple and histogram 
     formats.  generate keeps track of energy deposition in all calorimeter sections using the
     SingleTrackValidation class.

 - SingleTrackValidation: the class that takes care of ntuple building and histogramming for generate
     and the other similar scripts included


 - SingleTrackValidation-H62003.py: a validation script similar to generate for the 2003 test beam
 
 - SingleTrackValidation.py: a direct interface to SingleTrackValidation.   
 
 - ValidateRome.py: a validation script similar to generate for the Rome samples  
 
 - digitize: a script for standard digitization of the output from the generate script 
 
 - jobOptions.singleTrackValidation.py: a jobOptions level interface to the SingleTrackValidation object
 
 - validate-electrons: single electron validation, similar to a reduced generate script

 - share/generateRTT_jobOptions.py: dummy job options for RTT running

 - share/generateRTT_*.C: Post processing root job for running generate script in RTT jobs

 - test/LArG4Validation_TestConfiguration.xml: Configuraton XML file for RTT running

 - For questions or comments: Joe Boudreau 
   boudreau@pitt.edu

*/
