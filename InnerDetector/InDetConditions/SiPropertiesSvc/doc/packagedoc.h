/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiPropertiesSvc_page SiPropertiesSvc Package

@authors Grant Gorfine

@section SiPropertiesSvc_SiPropertiesSvcIntro Introduction

Service for providing silicon properties for each detector element.
The properties are available via the SiliconProperties class from
which one can obtain hall and drift mobility, diffusion constant and
number of electron hole pairs per deposited energy.  Some of these
are dependent on temperature and voltages which are obtained via the
appropriate service and updated via a call back.



*/
