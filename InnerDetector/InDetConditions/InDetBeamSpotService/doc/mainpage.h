/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage The InDetBeamSpotService package

\section introductionInDetCondServicesIntroduction

Remark: This package used to be called InDetCondServices but was renamed to
InDetBeamSpotService shortly after release 14.0.0 was out.

This package provides services (in the Athena sense) related to inner
detector conditions data. So far, there is a single service, BeamCondSvc

\section BeamCondSvc

The BeamCondSvc (accessed through the abstract interface IBeamCondSvc.h) 
provides a service to return the beamspot position and error matrix, for
use in reconstruction and analysis algorithms. The information can 
be set from jobOptions or read from the COOL conditions
database (default behaviour), where it can potentially change event by event.

The beamspot centroid is represented by a position (the centre of the
distribution), RMS widths in x, y and z (the sigmas) and two tilt angles
giving the inclination from the z axis in the xz and yz planes. These
values can be set by joboptions, and are the representation used in the
conditons database, where they are stored in a folder /Indet/Beampos .

The service provides retrieve methods beamPos, beamSigma, beamTilt to 
get the current values of the parameters in this native representation,
and a method beamVertex() which returns the beamspot as a Trk::RecVertex
object ready for inclusion in track and vertex fitting. Note that the
error matrix is non-diagonal if the beam tilt is non-zero - then 
the best estimate of the (x,y) position will depend on z within the beamspot
envelope.

The service can be enabled by including the joboption file:

include("InDetCondServices/BeamCondSvc.py")

which automatically sets up IOVDbSvc to read from the conditions database.

The service provides the following joboption parameters (default in brackets)
 - useDB(true)  - read beamspot from COOL conditions database
 - posX(0.)      - x position of beamspot
 - posY(0.)      - y position of beamspot
 - posZ(0.)      - z position of beamspot
 - sigmaX(0.015) - x RMS width of beamspot
 - sigmaY(0.015) - y RMS width of beamspot
 - sigmaZ(53.)   - z RMS width of beamspot
 - tiltX(0.)     - x tilt of beamline
 - tiltY(0.)     - y tilt of beamline

\section Setting the beamspot position

A simple python script utils/beamSpot_set.py is provided to update the beamspot
position in the conditions database. Use --help for parameters.

*/
