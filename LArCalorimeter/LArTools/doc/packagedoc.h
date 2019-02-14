/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**

@page LArTools_page LArTools Package

This package provides tools for LAr identifier mapping. 



@author Hong Ma <hma@bnl.gov>
@author Fabienne Ledroit <ledroit@lpsc.in2p3.fr>
@author Arnaud Lucotte <arnaud.lucotte@lpsc.in2p3.fr>

@section LArTools_SignalMap Signal channel cabling mapping

LArCablingService (an AlgTool) provides the cabling mapping for signal channels, between online and offline identifiers.
LArCablingService will be removed, LArCablingLegacyService will be provided for clients which should retire soon

It also provide the definition of LArReadoutModuleID (defined in LArIdentifier package), and the mapping from FEB ID to 
the backend readout module.

the data classes LArOnOffIdMap and LArFebRodMap are conditions data objects, and can be filled from database.

LArFebRod_P and LArOnOffId_P are persistent data structure.

web page:  http://lpsc.in2p3.fr/atlas/fabienne/identifiers.html
  http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_online.html

@section LArTools_HVMap HV channel mapping

LArHVCablingTool provides the mapping between the HV channel and the detector electrode.,
It is also responsbile for encoding and decoding the LArHVCellID and LArHVDetectorID and LArHVElectrodeID.

LArHVMap is a data class that contains the mapping. 

web page:
http://lpsc.in2p3.fr/atlas/lucotte/3.SOFTWARE/software_highvoltage.html

@section LArTools_OtherClasses Other classes

LArMCSymTool  is a tool that maps a LAr readout channel id to a channel that is considered good for MC symmetry. Typically it is first channel in phi.


*/
