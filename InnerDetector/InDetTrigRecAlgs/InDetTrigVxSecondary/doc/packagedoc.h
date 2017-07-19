/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrigVxSecondary_page InDetTrigVxSecondary package

@author Andrew John Lowe

@section InDetTrigVxSecondary_Introduction Introduction
InDetTrigVxSecondary makes secondary vertex reconstruction
available at the EF. This has been done primarily for the b-jet
trigger slice, but may also have applications in triggers for
long-lived particles with displaced vertices as a signature.

@section InDetTrigVxSecondary_Details Functional details
The package contains a feature extraction (FEX) algorithm, which
iteratively calls the offline vertexing tool(s) defined in a
ToolHandleArray and records the vertices found in a way that enables
downstream hypothesis algorithms to access the vertices and apply cuts
on their attributes.

TrigVxSecondary runs in the High-Level Trigger (HLT) sequence table
after the primary vertex finder TrigVxPrimary. The offline tool(s) are
called from the algorithm via an abstract vertex tool interface. This
enables the fitter to be loaded and configured at runtime via the
jobOptions file InDetTrigVxSecondary_LoadTools.py.

The inputs to the vertex tool(s) are:
- The reconstructed primary vertex from TrigVxPrimary
- The tracks in the Region of Interest (RoI)
- The jet direction

The jet direction is derived from a subset of the tracks using a
simple cone algorithm (for details, see TrackJetFinder in this
package).

TrigVxSecondary records in StoreGate the secondary vertices in two forms:
- VxCandidate objects, to conform with the present output type of
  TrigVxPrimary
- VxSecVertexInfo objects, to enable common output irrespective of the
  vertex tool used

Moreover, VxSecVertexInfo objects contain the following additional
information not available in VxCandidate:
- Invariant mass
- Energy fraction
- Number of two-track vertices

In the default running mode, a single vertex finder is run:
InDetVKalVxInJetTool.

@section InDetTrigVxSecondary_InDetVKalVxInJetTool InDetVKalVxInJetTool: a short description

InDetVKalVxInJetTool is a vertex finder that uses VKalVrt as the
vertex fitting engine. VKalVrt is a universal package for any vertex
fitting problem, developed by Vadim Kostyukhin. It is a Kalman filter
using the Billoir method. Vertices may be fitted using either the
standard @f$\chi^{2}@f$ approach or adaptive functionals that
down-weight poorly-fitting tracks. The exact decay topology is not
assumed. By default, a single geometrical vertex is fitted. The single
vertex approximation allows high detection efficiency at the cost of
an imprecise kinematical reconstruction in cases where the b-and
c-hadron decay vertex distance is large. The basic idea employed by
VKalVrt is to iteratively remove tracks that are least compatible with
a given vertex candidate and fit again. Please refer to the
documentation for InDetVKalVxInJetTool for further details specific to
the default offline vertex finder.


*/
