/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVertexKinematicFitterUtils_page The TrkKinematicVertexFitterUtils package

TrkKinematicVertexFitterUtils provides classes for constraints in vertex reconstruction.
&nbsp;
For the time being it is meant to host the concrete types of kinematic constraints
implemented under the Trk::IKinematicConstraint base class interface.

Each constraint class calculates a vector of values and a matrix of derivatives
of the corresponding constraint equation at the expansion point provided by
the user.

@section TrkVertexKinematicFitterUtils_OverviewTrkKinUtils Class Overview
  The TrkVertexKinematicFitterUtils package contains:

  - Trk::MassConstraint - formulates the mass constraint in kinematic vertex fitting

The classes in this package (and any future additions) shall not extend any
of the athena components, therefore the package follows the installed_library pattern.
      

@author <a href="http://cern.ch/xwho">Maaike Limper</a>

@section TrkVertexKinematicFitterUtils_ExtrasTrkKinUtils Extra Pages

*/
