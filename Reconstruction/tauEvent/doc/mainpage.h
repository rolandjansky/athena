/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage tauEvent

@author Thomas Burgess <thomas.burgess[]cern.ch>
@author Anna Kaczmarska,
@author Stan Lai
@author Lukasz Janyst
@author Nico Meyer
@author Noel Dawe

@section Introduction

This is the Reconstruction/tauEvent package which purpose is to enable 
access to quantities for reconstructed tau candidates.
The main storage class is Analysis::TauJet which keeps links to
the details classes Analysis::TauCommonDetails and 
Analysis::TauCommonExtraDetails.
The common are saved in AODs and ESDs, 
whereas the extra details are only saved in ESDs. 
TauJet and TauPID are allways saved.

@section Deprecated classes
The following classes have been deprecated and further usage should be avoided
 - Analysis::Tau1P3PDetails
 - Analysis::TauRecDetails 
 - Analysis::Tau1P3PExtraDetails 
 - Analysis::TauRecExtraDetails 
 - tauAnalysisHelperObject
 - Analysis::TauGenericDetails (removed from cvs)

Packages used by tauEvent are listed here:

@htmlinclude used_packages.html

The requirements file for tauEvent is shown here:

@include requirements

*/
