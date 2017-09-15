/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkSurfaces_page The TrkSurfaces package

Package for surface descriptions in the Tracking package. 

@author Andreas.Salzburger@cern.ch

@section TrkSurfaces_TrkSurfaceIntro Introduction

The different <b>Trk::Surface</b> types define the track parameterisations available.

<table>
<tr>
    <td><b>Trk::ConeSurface</b></td>
    <td> Conical Surface, ownes/defined by Trk::ConeBounds</td>
</tr>
<tr>
    <td><b>Trk::CylinderSurface</b></td>
    <td> Cylindrical Surface, ownes/defined by Trk::CylinderBounds</td>
</tr>
<tr>
    <td><b>Trk::DiscSurface</b></td>
    <td> Surface describing a disc, ownes/defined by Trk::DiscBounds </td>
</tr>
<tr>    
    <td><b>Trk::PerigeeSurface</b></td>
    <td> Surface for Perigee representation, placeholder for IP point, the orientation is fixed to be aligned along the tracking z-Axis. </td>
</tr>
<tr>
    <td><b>Trk::PlaneSurface</b></td>
    <td> Planar Surface representation, can either have Trk::RectanguleBounds, 
         Trk::TrapezoidBounds, Trk::RotatedTrapezoidBounds, Trk::DiamondBounds and Trk::EllipseBounds </td>    
</tr>
<tr>
    <td><b>Trk::StraightLineSurface</b></td>
    <td> Surface for straight line representation, differently to the Trk::PerigeeSurface it can be oriented arbitrarily </td>
</tr>
</table>


@section TrkSurfaces_TrkSurfaceUsage Usage of 3 and 2-dim objects

In this package the following classes for 3 dimensional and 2 dimensional points of vectors
are used from the GeoPrimitives package
- Amg::Vector3D for 3D points and vectors
- Amg::Vector2D for 2D points on a surface or the special local direction 


*/
