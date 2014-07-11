/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  TrkJetVxFitter
 
@author Nicola.Giacinto.Piacquadio@cern.ch
 
\section introTrkVertexFitterUtils Introduction
This package contains a new fitter application, which we 
called JetFitter, which permits to do a multi-vertex fit, 
where all the vertices are constrained to lie on a single  
flight axis and the exact position of the various 
vertices along the flight length is found during the fit.

In practice this fit is very usefull to be applied to 
fit the B-D hadron cascade inside a b-quark jet. 
The fit is in this case initialized with the primary vertex 
position, where the flight axis is constrained to pass through, 
and with a first estimate of the flight direction of the 
B-D hadron cascade which is tipically the jet direction 
as reconstructed in the calorimeter. Then all the tracks 
are fit, constraining them to lie on this flight axis, which 
is exact for B-hadrons and a good approximation for tracks 
coming from the D-hadron. A clustering procedure is following. 
For more information please give a look at the documentation 
of the JetFitter finder in the <i>InDetSecVxFinderTool</i> package.

\section designTrkJetVxFitter Design 

<b><i>Trk::JetFitterRoutines</i></b> This class contains the 
core functions for the "JetFitter" algorithm.

This is a new algorithm which uses a "Kalman filter" based approach 
to implement the "ghost track method" used for the first time by SLD 
(reference to be inserted here).
 
All the tracks inside a jet can be fitted with the constraint of lying on the same 
'flight axis' from the primary vertex towards the jet. This class provides all the methods 
which are necessary to perform this fit and to evaluate the probability of two or more 
tracks to build one cluster on the jet axis.

The real finding algorithm is then implemented in the "SV" finder algorithm. In this 
class only the methods for 'fitting' are implemented.

There is no clear input and clear output in a similar fit, so this works more like 
a continous interaction between the finder and the fitter, which is provided in the 
form in a set of routines to use.

<b><i>Trk::JetFitterHelper</i></b> This class provides 
utilities for the JetFitter algorithm, mainly to manipulate the 
JetVxCandidate object during the fit, but also to perform 
linearization and similar tasks...

One very interesting feature is the application of <i>exact</i> constraints 
through the Kalman Filter formalism, a possibility illustrated for 
the first time in Babar with TreeFitter by Wouter Hulsbergen. 
This helps to have a fast estimation of the probability of forming a common cluster 
between two vertices. (reference will be put here)

<b><i>Trk::JetFitterInitializationHelper</i></b> 
This class is used to initialize a VxJetCandidate, which is the class to be
provided to the 'JetFitter' algorithm in order to perform the fit of all the 
vertices on the JetAxis.
For using it, please look at the doxygen documentation of its methods.

<b><i>Trk::KalmanVertexOnJetAxisUpdator</i></b>  A concrete 
implementation of the Vertex Updator using the Kalman filter algorithm. 
This is a special implementation (part of the JetFitter algorithm) 
which is able to fit a vertex correcting the jet axis parameters 
to find the best overall agreement.
--> Method updating the fit along the flight axis 
with a track which is added to one of the vertices in the 
fit.

<b><i>Trk::KalmanVertexOnJetAxisSmoother</i></b> 
A concrete implementation of the Vertex Smoother for 
smoothing the vertex along the Jet Axis.
--> Update the vertex on the jet axis (chi2 and ndf) with the 
knowledge coming from the fitted cascade, updating 
also all the related track parameters of the tracks at 
the vertex.


For additional information or in case 
of problems please contact the authors...

@htmlinclude used_packages.html
@include requirements

*/
