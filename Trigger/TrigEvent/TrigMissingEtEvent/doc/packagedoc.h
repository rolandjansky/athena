/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigMissingEtEvent_page TrigMissingEtEvent
@author   Diego Casadei


@section TrigMissingEtEvent_TrigMissingEtEventIntro Introduction

This package defines the transient class needed to keep info about the
missing energy vector and the total (scalar) energy sums computed by
the ATLAS HLT.  More details are provided by the <a
href="https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasHltMetSlice">AtlasHltMetSlice</a>
twiki page and in the <a
href="http://cdsweb.cern.ch/record/1129146">ATL-COM-DAQ-2008-008</a>
note.


@section TrigMissingEtEvent_TrigMissingEtEventOverview Class Overview

The TrigMissingEtEvent package contains of following classes:

  - TrigMissingET : contains global quantities [the missing energy vector
                    components, scalar sums of total and transverse energy,
                    the event flag and the (unused) RoIword] and local 
                    quantities corresponding to individual contributions
                    [e.g. calo samplings, muons].
                    Each individual component is an instance of the
                    TrigMissingEtComponent class.

  - TrigMissingEtComponent : contains local contributions to the missing
                             energy vector and to the scalar energy sums,
                             together with calibration constants and a
                             component-level status flag.

  - TrigMissingETContainer : used to save things on disk, via transient /
                             persistent separation implemented in the
                             TrigMissingEtEventTPCnv package.





*/
