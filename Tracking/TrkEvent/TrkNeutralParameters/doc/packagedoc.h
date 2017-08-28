/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkNeutralParameters_page The TrkNeutralParameters package

@author Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch

@section TrkNeutralParameters_introTkPara Introduction

The TrkParameters package contains classes to represent the Track's parameters at all points on the track, from the Trk::Perigee parameters, to parameters at  various parts of the 
detector. The parameter classes must also include ErrorMatrices, must reproduce all current functionality, be lightweight and be fast.<p>
For convenience, a list of the (currently available) parameters is reproduced below:
- Trk::NeutralAtaCone            : contains the parameters on a cone surfaces, defined as \f$(locphi, locZ, \phi, \theta, q/p)\f$
- Trk::NeutralAtaCylinder        : contains the parameters on a cylinder, defined as \f$(locR\phi, locZ, \phi, \theta, q/p)\f$
- Trk::NeutralAtaDisc            : contains the parameters on a disc, defined as \f$(locR, locPhi, \phi, \theta, q/p)\f$
- Trk::NeutralAtaPlane           : contains the parameters on an arbitrary oriented plane, defined as \f$(R\phi, z, \phi, \theta, q/p)\f$
- Trk::NeutralAtaStraightLine    : contains the parameters at a straight line, defined as \f$(locR, locZ, \phi, \theta, q/p)\f$
- Trk::NeutralPerigee            : contains the perigee parameters, defined as \f$(d_0, z_0, \phi_0, \theta, q/p)\f$

The internal representation of the five parameters for all inherited classes follows:
- the first two elements are the position on the plane expressed in local coordinates (natural frame of the surface)
- the last represent the momentum by \f$ (\phi, \theta, q/p) \f$, where<br>
\f$ \phi \f$ is the azimutal angle measured from the x-axis so that the positive x-axis has \f$ \phi = 0 \f$,
fullfilling that \f$ \phi \in [-\pi, \pi)\f$ <br>
\f$ \theta \f$ is the polar angle measured from the z-axis so that the positive z-axis has \f$ \theta = 0 \f$,
fullfilling that \f$ \theta \in [0,\pi] \f$<br>
\f$ q/p \f$ is the charge diveded by the magnitude of the momentum

Click on <a href="annotated.html">Composite List</a> to see all the classes available in this package.<br>
Outstanding issues are shown in the <a href="todo.html">ToDo</a> list.

@section TrkNeutralParameters_designTkPara Design

In this section, we explain some of the design decisions we've made (which might help to explain the code a bit)
- often an object might have the capability to return other objects, but if these objects are time-consuming to create (for example, matrix inversions),
it makes sense to only create them as necessary, and to cache the result.
The approach we have taken is to use mutable pointers to these subobjects, which are NULL by default.
 - If an object needs to be returned, it is created and assigned to the pointer.
 - If it is needed again, no further processing is required ... and the pointer is just reused.
 - The best way to understand this is to look at some code: in Trk::AtaCylinder, there is a Trk::AtaCylinder::localPosition method, which returns a Trk::LocalPosition.
If you click on the link, you will be able to see that the Trk::LocalPosition is only created once, and only if it is needed.
- When a class returns access to an object it owns it should do so by reference (to make the ownership explicit) UNLESS the object could often be undefined. In which case it 
should return a const pointer, set to NULL (i.e. 0) whenever the object is void.
- At the moment not much is inlined - this will change when we start work on optimisation
- Trk::LocalParameters are defined in TrkEventPrimitives

@author Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
*/
