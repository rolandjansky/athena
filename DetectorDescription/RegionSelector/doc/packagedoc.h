/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page RegionSelector_page RegionSelector Package
@author Simon George <S.George@rhul.ac.uk>, Aline Gesualdi Mello, Steve Armstrong

@section RegionSelector_RegionSelectorIntro Introduction

The RegSelSvc translates physical regions within any subdetector system into detector identifiers or Read-Out Buffer (ROB) identifiers.

It has been designed and developed for use in the High Level Trigger, where it is used to implement the Region of Interest (RoI) mechanism. It is also intended to be of general use in the ATLAS reconstruction.

It is currently maintained by Simon George (general), Mark Sutton (indet), Denis Damazio (calo), Alessandro DiMattia (muon).

@section RegionSelector_RegionSelectorOverview Overview

  The RegionSelector package contains of following methods to retrieve identifiers:
 
  - RegionSelector::DetHashIDList translates a geometrical region into @c std::vector<IdentifierHash> 
    without repeated @c IdentifierHash elements.
  - RegionSelector::DetROBIDListUint: translates a geometrical region into @c std::vector<uint32_t> 
    without repeated @c ROBID elements.

@section RegionSelector_RegionSelectorUsage Usage

Region Selector execution is like any Service, so in order to be used in a 
standard Athena Algorithm, some additional lines must be added to the .h and 
.cxx files. It is (best) configured using the IRegSelSvc interface from the 
IRegionSelector to free your package from dependence on the actual RegionSelector
implementation.

In your requirements file, add:
@code  
  use IRegionSelector IRegionSelector-* DetectorDescription
@endcode

In the .h file, include the IRegSelSvc.h file (foward declaration 
is not sufficient as the header file includes some enumerations) and a 
declare a ServiceHandle for the RegSelSvc as private data member of your 
algorithm.
@code
  include "IRegionSelector/IRegSelSvc.h"
  //...
  ServiceHandle<IRegSelSvc> m_regSelSvc;
@endcode  
In the .cxx file, in the Algorithm's constructor, initialise the ServiceHandle.
@code
  m_regSelSvc("RegSelSvc")
@endcode
In the initialize method check it can retrieve the RegionSelector service.  
@code
  if (m_regSelSvc.retrieve()).isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve RegionSelector Service";
    return HLT::BAD_JOB_SETUP;
  }
@endcode

In your Algorithm's execute method, call the @c DetIDList method with the required detector, \f$ \eta \f$ and \f$ \phi \f$ range, and a pointer to a vector of identifiers. The vector will be filled with identifiers found in the region:
@code
  std::vector<IdentifierHash> detIdentifiers;
  m_regSelSvc->DetHashIDList(PIXEL, etaMin, etaMax, phiMin, phiMax, &detIdentifiers); 
  std::vector<uint32_t> robIdentifiers;
  m_regSelSvc->DetROBIDListUint(PIXEL, etaMin, etaMax, phiMin, phiMax, robIdentifiers);
@endcode
The above examples specify the @c PIXEL detector. Note that the detector is specified using @c DETID enumeration, whose values are: @c PIXEL, @c SCT, @c TRT, @c LAR, @c TTEM, @c TTHEC, @c TILE, @c MDT, @c RPC and @c TGC.
	 
There are some other methods that are only valid for some specific
detectors. For calorimeters (@c LAR, @c TTEM, @c TTHEC and @c TILE),
there are methods to retrieve identifiers and ROB IDs for a given
sampling. For muon detectors (@c MDT, @c RPC and @c TGC), there is an additional argument @c TYPEID, which is an enumeration whose values are: @c BIL, @c BIS, @c BML, @c BMS, @c BOL, @c BOS, @c BMF, @c BIR, @c BOF and @c BOG. 
Note that Muon detectors do not have methods to retrieve ROBID values. 

Note that for the Inner Detector there are additional methods which also take zMin and zMax 
values for the roi, eg
@code
  std::vector<IdentifierHash> detIdentifiers;
  m_regSelSvc->DetHashIDList(PIXEL, zMin, zMax, etaMin, etaMax, phiMin, phiMax, &detIdentifiers); 
@endcode
which allow the z extent along the beamline to be specified. There are also additional
 methods that allow the roi to be specified with respect to a shifted beamline in x and y 
@code
  std::vector<IdentifierHash> detIdentifiers;
  m_regSelSvc->DetHashIDList(PIXEL, zMin, zMax, etaMin, etaMax, phiMin, phiMax, xBeam, yBeam, &detIdentifiers); 
@endcode
but care must be taken in this case when specifying an roi from -pi to pi, since any 
shift in y will mean that both -pi and pi will intersect with any given layer at some 
other angle!=pi in the lab frame and the phi wrapping will render phiMin = phiMax so 
you will get an roi with deltaphi=0 which is almost certainly not what you want in this
case.  

@section RegionSelector_RegionSelectorConfig Configuration

RegionSelector is configured by the RegSelSvcDefault
configurable. Declaring the ServiceHandle as shown above should
generate the configuration you need automatically.

RegionSelector has the following properties:

  - @c @b RegionType (@c @b string): not used;
  - @c @b GeometricalLayout (@c @b string): specify which atlas detector description layout to use for static maps, e.g., @c Rome-Initial, @c DC2. Currently only the muon detector uses static maps. The other detectors are not influenced by this as they calculate their lookup tables on the fly from whichever detector description version has been loaded by GeoModel. You only need to set this if you wish to override the default value, which is taken from the GeoModelSvc at run time.
  - @c @b enableID, @c @b enableCalo or @c @b enableMuon (@c @b bool): these flags specify which detectors maps should be initialized for. They may be used in combination. By default they are all false (disabled).
  - @c @b WriteTables (@c @b bool): flag to turn on writing of maps back out to files at initialization time. This is just for expert debugging.
  - @c @b OutputFile (@c @b string): common prefix name for map files. Each map will be written to a separate file name by this prefix plus detector name. The default name is @c RSOutputTable.txt so for example the TRT table would be written to @c RSOutputTableTRT.txt.

Note that RegionSelector has a run-time dependency on InnerDetector and Calo tools which produce the lookup tables it needs. These tools will automatically be configured and invoked by the RegSelSvc if the relevant @c enable flags are on.



*/
