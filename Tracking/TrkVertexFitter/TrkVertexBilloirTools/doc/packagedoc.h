/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVertexBilloirTools_page trkVertexBilloirTools

@section TrkVertexBilloirTools_introduction Introduction:
TrkVertexBilloirTools is a package which provides basic vertex fitting algorithms based on the Billoir fitting
method.<br>

The vertex fit estimates the position of the interaction point and the momentum vectors of the outgoing tracks at this point and it allows one to check the association of tracks to this point. The least square method is one of the best methods to merge the full information. But the least square method involves the inversion of a matrix of order 3n (n = number of tracks). Since the number of arithmetical operations for the inversion increases with the third power of the order, this method becomes prohibitive. A method has been developed (Metcalf, Regler and Broll 1973; Billoir, Fruehwirth and Regler 1985) to allow the application of the least square method to very complex events also. <br>

The Billoir method is based on the local parametrisation of tracks in the neighborhood of the vertex point. Thereby we have to define two different coordinate systems: a global coordinate system and a local coordinate system. The origin of the global coordinate system is always the interaction point. The origin of the local coordinate system is always in the neighborhood of the vertex point. After the vertex fit this origin moves to the vertex point. Therefore the aim is to estimate the vertex position in the global ATLAS frame and to re-calculate the parameters of tracks in the local (vertex) frame. <br>

Using the general parametrisation of the helix above we can parametrise the trajectory in the local frame. This parametrisation describes the trajectory of the particle close to the local origin by neglecting curvature effects or by an approximation of the curvature effects at first order. The relevant parameters are Perigee). <br>

The Billoir vertex fitter requires an a priori selection of tracks. This selection of tracks is the task of a vertex finding algorithm. The next important point is to choose the starting point. In case of primary vertex this point is surely the interaction point, but for secondary tracks one has to make an educated guess of the vertex position before fitting. Therefore the fitting procedure is iteratively executed. By default the starting point is set to (0,0,0) and the vertex point will be calculated. If another starting point has been chosen, then the origin of the local coordinate system will be shifted to the chosen starting point. In the next iteration the calculated vertex point is taken as the starting point and the new vertex point is calculated and so on. <br>

@section TrkVertexBilloirTools_howitworks How it works:
Two billoir fitting methods (so called FastVertexFitter and FullVertexFitter) are implemented as Athena AlgTools. The user
can retrieve the tool(s) from the ToolSvc during runtime and use them to fit a vertex. The basic usage
is to provide it with some Trk::Track(s) plus a start vertex (and optionally a beam constraint) and call
its fit(...) method. Both tools inherit from a common baseclass and therefore have the same interface.
After the fit all results are stored in a VxCandidate object (see Tracking/TrkEvent/VxVertex) which
can be accessed via the pointer to the tool (which was also used to call its calculate method).
Be aware that neither tool rights anything to StoreGate.<br>
A very good usecase can be found in: InnerDetector/InDetRecAlgs/InDetPriVxFinder.

@section TrkVertexBilloirTools_packagecontent Package Contents:
VxBilloirTools contains the following files/classes:
- FastVertexFitter.cxx ... the fast version. Calculates only the vertex position. Track parameters are untouched.
- FullVertexFitter.cxx ... the slower version. Calculates both: vertex position and track parameters.



\namespace Trk
A namespace for all vertexing packages and related stuff.
*/
