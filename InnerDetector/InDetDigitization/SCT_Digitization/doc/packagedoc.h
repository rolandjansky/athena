/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_Digitization_page SCT_Digitization package
@author Szymon.Gadomski@cern.ch,
        Awatif.Belymam@cern.ch,
        Davide.Costanzo@cern.ch,
        tgcornel@nikhef.nl,
        Grant.Gorfine@cern.ch,
        Paul.Bell@cern.ch,
        Jorgen.Dalmau@cern.ch,
        Kondo.Gnanvo@cern.ch,
        and others


@section SCT_Digitization_SCT_DigitizationIntro Introduction
The SCT_Digitization package processes the GEANT4 hits in each SCT sensitive detector element (wafer),
simulates the detector response, and generates the SCT RDO's.
The model used is documented in the note ATL-SOFT-2001-005 by Szymon Gadomski.

The entry and exit points of a track passing through a wafer, and the energy deposited, are obtained from the G4 hit.
The path through the wafer is divided into a number of steps, and the total charge deposited is divided equally amongst the steps.
This charge is then drifted to the wafer readout surface, taking into account the Lorentz angle and random diffusion.
The list of surface charges with their associated drift times is then input to a model of the SCT front-end electronics.
The response from the model for each strip (diode) is compared with a fixed readout threshold,
and an RDO created and stored for each strip above threshold.
Detector noise is included in the model, and pure noise hits are also generated.
Crosstalk between neighbouring channels is also taken into account.

@section SCT_Digitization_SCT_DigitizationOverview Class overview
The SCT_Digitization package contains the following classes:
 - SCT_Amp: Models an SCT amplifier and shaper, giving CRRC response to a list of charges with times.
 - SCT_Digitization: Athena algorithm inheriting from SiDigitization.
 - SCT_FrontEnd: Models response of ABCD chip amplifiers.
 - SCT_SurfaceChargesGenerator: Drifts charge from hits to SCT wafer surface.

@section SCT_Digitization_SCT_DigitizationRefs Reference pages
 - @ref notes_SCT_Digitization
*/



/**
@page notes_SCT_Digitization Change notes
@htmlinclude notes.html
*/
