/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonG4SD_page 

   @package MuonG4SD 
   
   @author Daniela Rebuzzi <daniela.rebuzzi@pv.infn.it>
   @author Ketevi A. Assamagan <ketevi@bnl.gov>
   @author Andrea Di Simone <Andrea.DiSimone@cern.ch>	
   @author Yoji Hasegawa <hasegawa@azusa.shinshu-u.ac.jp>
   @author Niels Van Eldik <nveldik@nikhef.nl>

   Implementations of the SensitiveDetector for the Muon technologies
 
G4VSensitiveDetector is an abstract base class which represents a detector. 
The principal mandate of a sensitive detector is the construction of hit 
objects using information from steps along a particle track. 

The ProcessHits() method of G4VSensitiveDetector performs this task using 
G4Step objects as input. Objects of the G4TouchableHistory class may be used 
as an optional input.

The pointer to your sensitive detector must be set to one or more 
G4LogicalVolume objects to set the sensitivity of these volumes. 
The pointer should be also registered to G4SDManager.

G4VSensitiveDetector has three major virtual methods.

- ProcessHits()
    This method is invoked by G4SteppingManager when a step is composed in 
    the G4LogicalVolume which has the pointer to this sensitive detector. 
    The first argument of this method is a G4Step object of the current step. 
    The second argument is a G4TouchableHistory object for the ``Readout 
    geometry''. The second argument is NULL for the case ``Readout geometry'' 
    is not assigned to this sensitive detector. In this method, one or more 
    G4VHit objects should be constructed if the current step is meaningful 
    for your detector.

- Initialize()
    This method is invoked at the beginning of each event. The argument of 
    this method is an object of the G4HCofThisEvent class. Hits collections, 
    where hits produced in this particular event are stored, can be associated 
    to the G4HCofThisEvent object in this method. The hits collections 
    associated with the G4HCofThisEvent object during this method can be used 
    for ``during the event processing'' digitization.

EndOfEvent()
    This method is invoked at the end of each event. The argument of this 
    method is the same object as the previous method. Hits collections 
    occasionally created in your sensitive detector can be associated to the 
    G4HCofThisEvent object. 

@section MuonG4SD_ExtrasMuonG4SD Extra Pages

*/
