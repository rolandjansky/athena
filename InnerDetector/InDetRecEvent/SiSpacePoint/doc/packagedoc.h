/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SiSpacePoint_page SiSpacePoint Package
@author Martin.Siebel@cern.ch

@section SiSpacePoint_SiSpacePointIntro Introduction

This package contains the definition and implementation for the
InDet::PixelSpacePoint and InDet::SCT_SpacePoint classes. Both classes are derived from
Trk::TrkSpacePoint.

@section SiSpacePoint_SiSpacePoint Class Overview
  The SiSpacePoint package contains the following classes:

   - InDet::PixelSpacePoint: The SpacePoints for the PixelDetector.
     Most information including local errors is taken from PrepRawData, which is
     given to the constructor.
     Global errors are derived from them local errors.

   - SCT_SpacePoint: The SpacePoints for the SCT. Most
     information is derived from the GlobalPosition, which
     is given to the constructor. The LocalPosition is assumed
     on the DetectorElement corresponding to the first element
     of the PRD-pair given to the constructor.  Local errors are
     assumed constant for the time being. They are only initialized when
     inquired and buffered after that.

@section SiSpacePoint_SCT_Errors Estimation of the SCT_SpacePoint error matrix
   <br>
   \image html SctSpacePointError.gif
   <br>
   The local coordinates  \f$  x  \f$  and  \f$  y  \f$  represent the combination of
   two one-dimensional measurements (two dimensional, if also the constraint due
   to the position of the SCT module is counted) from two different sensor surfaces.
   The calculation of the error matrix of a SCT_SpacePoint is therefore not straightforward,
   especially the correlation between  \f$  x  \f$  and  \f$  y  \f$  has to be taken into account.
   The figure shows an idealised case in which two sensor surfaces cross each other with an
   angle  \f$  \alpha  \f$  which is around 40mrad in the SCT geometry. One sensor gives a measurement in
   the local coordinate   \f$   u   \f$  , the other in the local coordinate   \f$   v  \f$ . The local coordinates 
    \f$  x  \f$  and  \f$  y  \f$  refer to the orientation of the first sensor. With the given orientation this
   leads to the identity  \f$  x=u  \f$ . As shown in the figure,  \f$  y  \f$  is related to  \f$  u  \f$  and  \f$  v  \f$  via
   <br> <br>
      \f$  u = x_0 + ( y + y_0 ) \tan\alpha  \f$ 
   <br> <br>
   with
   <br> <br>
     \f$  x_0 = v \cos\alpha  \f$ 
   <br> <br>
   and
   <br> <br>
      \f$  y_0 = v \sin\alpha \f$  .
   <br> <br>
   From this  \f$ y \f$  is found to be
   <br> <br>
      \f$  y = u \cot\alpha - ( \cot\alpha \cos\alpha + \sin\alpha ) v \f$ 
   <br> <br>
   Therefore  \f$ (u,v) \f$  transforms to  \f$ (x,y) \f$  via
   <br> <br>
      \f$  \left(\begin{array}{c} x \\ y  \end{array}\right) = T \left(\begin{array}{c} u \\ v  \end{array}\right)  \f$   
   <br> <br>
   with
   <br> <br>
     \f$  T = \left(\begin{array}{cc}  1 & 0 \\ a & b \end{array}\right)  \f$  .
   <br> <br>
   where  \f$  a  \f$  and  \f$  b  \f$  are given by
   <br> <br>
      \f$  a = \cot\alpha    \simeq 25.0 \f$ 
   <br> <br> 
     \f$  b = -(\cot\alpha \cos\alpha + \sin\alpha)  \simeq -25.0 \f$ 
   <br> <br>     
   From this the covariance matrix of  \f$  x  \f$  and  \f$  y  \f$  is calculated via
   <br> <br>
       \f$  C_{x,y} = T C_{u,v} T^t  \f$ 
   <br> <br>
   with
   <br> <br>
       \f$  C_{u,v} = \left(\begin{array}{cc} \Delta u^2 & 0 \\ 0 & \Delta v^2 \end{array}\right)  \f$  .
   <br> <br>
  This results in the error matrix
   <br> <br>
      \f$  C_{x,y} = \left(\begin{array}{cc} \Delta u^2 & a \Delta u^2 \\ a \Delta u^2  & a^2\Delta u^2 + b^2\Delta v^2  \end{array}\right)  \f$  .
   <br> <br>
  With \f$ \Delta u = \Delta v \f$ and inserting \f$ a \f$ and \f$ b \f$ this can be simplified to
  <br> <br>
      \f$ C_{x,y} =  \left(\begin{array}{cc} 1 & 25  \\ 25  &  1250  \end{array}\right) \Delta u^2  \f$  .
  <br> <br>
  With \f$ \Delta u = 20\mu m\f$ this yields \f$ \Delta y = 707\mu m \f$. This value is slightly below the value of \f$ 800 \mu m\f$, which is realistically
  expected due to the ideal assumptions made in this derivation. In order to maintain the proper errors in \f$ x \f$ and \f$ y \f$, the bottom right entry of 
  \f$ C_{x,y} \f$ is adjusted to 1600, while the rest of the matrix is kept.





*/
