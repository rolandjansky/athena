/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetV0Finder_page The InDetV0FinderInterfaces package

@author Evelina.Bouhova@cern.ch

@section InDetV0Finder_introductionInDetV0Finder Introduction:
InDetV0Finder is a package which contains the algorithm for finding V0 vertices in
the Inner Detector. It retrieves a V0Container and writes all info to StoreGate.

@section InDetV0Finder_howitworksInDetV0Finder How it works:
InDetV0Finder gets its input tracks from StoreGate and selects tracks pairs with
opposite charge. If an acceptable starting point is found by <b></i>m_getStartingPoint</i></b>
a 3D vertex fit without any mass or pointing constraints is performed using <b></i>TrkV0Fitter</i></b>.

If the unconstrained fit satisfies a number of conditions, mass constrained fits,
assuming Kshort, Lambda, LambdaBar and Gamma hypotheses, are performed.

If at least one of these is successful and the imposed conditions are met,
a Trk::V0Candidate is stored, containing all accepted Trk::V0Hypothesis

<br>
<b></i>m_getStartingPoint</i></b> calculates the initial approximation to the vertex position.
Based on CTVMFT, it tries to intersect circles that are (R,Phi) projections of the helical
trajectories of the two tracks. If the circles intersect, the intersection with smaller z difference
between the two helices extrapolated to the intersection is chosen. If the circles do not intersect,
the vertex approximation is taken as the point of closest approach of the two circles. The first
track in the argument list should be the track with the smaller radius.

@section InDetV0Finder_jobOptionsInDetV0Finder The jobOptions file:
The algorithm is automatically configured via genConf. Most cuts are controlled by parameters,
which can be changed through jobOptions:
  - int <b></i>m_masses</i></b> = 1 if using PDG values for the masses of the two tracks and the V0,
                                = 2 if user set;
    - double <b></i>m_masspi</i></b> : pion mass (139.57 MeV)
    - double <b></i>m_massp</i></b> : proton mass (938.272 MeV)
    - double <b></i>m_masse</i></b> : electron mass (0.510999 MeV)
    - double <b></i>m_massK0S</i></b> : Kshort mass (497.672 MeV)
    - double <b></i>m_massLambda</i></b> : Lambda mass (1115.68 MeV)
    - double <b></i>m_massGamma</i></b> : Gamma mass (0.0 MeV)

  - double <b></i>m_d0min</i></b> : Minimum d0 of charged tracks
  - double <b></i>m_z0min</i></b> : Minimum z0 of charged tracks
  - double <b></i>m_ptmin</i></b> : Minimumpt of charged tracks

  - double <b></i>m_minsxy</i></b> : Minimum Rxy of starting point (1. mm)
  - double <b></i>m_minRxy</i></b> : Minimum Rxy of vertex (10. mm)

  - int <b></i>m_lcuts</i></b>  = 1 if using vertex prob cuts, = 2 if using chi2 cuts
  - int <b></i>m_mcuts</i></b>  = 1 if using mass prob cuts, = 2 if using mass window cuts
  - int <b></i>m_mfcuts</i></b> = 1 if using mass prob cuts, = 2 if using mass window cuts (final selection)
  - int <b></i>m_point</i></b>  = 1 if no pointing for conversions, = 2 if pointing to PV

  - double <b></i>m_maxChi2</i></b> : Maximum chi2/dof of vertex fit (6.)
  - double <b></i>m_minVertProb</i></b> : Minimum vertex probability (0.001)
  - double <b></i>m_minMassProb</i></b> : Minimum probability for a mass hypothesis (0.001)
  - double <b></i>m_maxMassErr</i></b> : Maximum mass error (100.)

  - double <b></i>m_sigmassK0S</i></b> : Kshort mass resolution (8.5 MeV)
  - double <b></i>m_sigmassLambda</i></b> : Lambda mass resolution (3.5 MeV)
  - double <b></i>m_sigmassGamma</i></b> : Gamma invariant mass cut (10.0 MeV)
  - double <b></i>m_nsig</i></b> : mass windows set to m_nsig times the mass resolution (5.)




*/
