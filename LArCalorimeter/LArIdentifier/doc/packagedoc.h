/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArIdentifier_page LArIdentifier Package
@author arnaud.lucotte@lpsc.in2p3.fr

@section LArIdentifier_LArIdentifierIntro Introduction

This package implements a certain number of classes of identifiers used to describe the calorimeter 
cells. <br>
<br>
Several descriptions of a calorimeter cell are available. A cell can be described by its 
"geometric" components (detector,z-side,region,sampling,eta,phi) or by its "hardware" 
components (detector,z-side,feedthrough, FEC slot,FEB channel). A cell can also 
be described according to the High Voltage line characteristics that powers it. To Each of these description 
corresponds an identifier, which implements univoquely the corresponding characteristics. 

An Identifier is a 32-bit integer in which all the information regarding the object is encoded. 
To every cell is associated a geometric identifier called "offline identifier" and an hardware 
identifier also called "online identifier". Only the "online identifier" associated to the cell 
is a persistent data.

An Helper provides the framework of the identifier description. In our case there are 
"online helpers" and "offline Helpers". Helpers contains the complete description of the 
identifiers in terms of "Fields" defined in a "Dictionary" (which, in Atlas, is an xml file). 
Helpers also provides all functionalities needed to access the identifier or a set of identifiers 
identifiers of a specific sub-system, per eta, etc...)

This package contains three main classes of identifiers: one for the hardware description 
of a cell, and two used in the HighVoltage description of a cell.

@section LArIdentifier_LArIdentifierOverview Class Overview
  The LArIdentifier package contains of following classes:
  - LArOnlineID: base identifiers to describe calorimeter 
                 cells from the hardware standpoint 
                 ie: in terms of the following fields:<br>
                 [Detector,barrel/Endcap,Side,Feedthrough,Febslot,channel]<br>
                 A complete description can be found in:<br>
                 http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_online.html  
  - LArElectrodeID: base identifiers describing the electrode
                 in terms of the following fields
		 [Detector,side,Module,HV_Phi_sector,HV_eta_sector,gap,plane]
  - LArHVLineID  : identifiers describing the High Voltage lines
                  in terms of the following fields:
		  [HV partition, HV CAN line, HV CAN node,HV line]
                 A complete description can be found in:<br>
                 http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_highvoltage.html  

  The following classes are no longer used from release 11 on and are kept  
  for backward compatibility reasons only (comissionning analyses):
  - LArHVCellID 
  - LArHVDetectorID
  - LArHVElectrodeID
  - LArHVSpecialElectrodeID



*/
