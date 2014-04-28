/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArTools Package

This package provides tools for LAr identifier mapping. 



@author Hong Ma <hma@bnl.gov>
@author Fabienne Ledroit <ledroit@lpsc.in2p3.fr>
@author Arnaud Lucotte <arnaud.lucotte@lpsc.in2p3.fr>

@section SignalMap Signal channel cabling mapping

LArCablingService (an AlgTool) provides the cabling mapping for signal channels, between online and offline identifiers.
It also provide the definition of LArReadoutModuleID (defined in LArIdentifier package), and the mapping from FEB ID to 
the backend readout module.

the data classes LArOnOffIdMap and LArFebRodMap are conditions data objects, and can be filled from database.

LArFebRod_P and LArOnOffId_P are persistent data structure.

web page:  http://lpsc.in2p3.fr/atlas/fabienne/identifiers.html
  http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_online.html

@section HVMap HV channel mapping

LArHVCablingTool provides the mapping between the HV channel and the detector electrode.,
It is also responsbile for encoding and decoding the LArHVCellID and LArHVDetectorID and LArHVElectrodeID.

LArHVMap is a data class that contains the mapping. 

web page:
http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_highvoltage.html

@section OtherClasses Other classes

LArMCSymTool  is a tool that maps a LAr readout channel id to a channel that is considered good for MC symmetry. Typically it is first channel in phi.


*/
