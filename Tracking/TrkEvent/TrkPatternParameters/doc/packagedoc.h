/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkPatternParameters_page TrkPatternParameters
@author Igor.Gavrilenko@cern.ch
@section TrkPatternParameters_TrkPatternParametersIntro Introduction

The TrkPatternParameters package contains class to represent the Track parameters at all
kind surface. This class optimazed for pattern recognition process. The main property
of this class is that his information can be rewrite. It is means that the same object of
this class can present information for any kind surface and during reconstruction process
don't need to use operators new and delete. Pointer to surface define kind of track
presentation. If this pointer is pointer to AtaPlane it is means that track parameters are
for AtaPlane if it points to Perigee surface track parameters are for Perigee.

The main data member of this class are: 

1. m_surface      - is pointer to surface.
2. m_parameters   - are standard track parameters for give nsurface.
3. m_covariance   - is covariance matrix of track parameters
4. m_iscovariance - bool parameters is true if covariance matrix exist

Given class contains set member functions which provide similar information like
standard TrackParameters class and contains set converters which provide
transformation information from TrackParameters to PatternParameteres and
from PatternParameteres to TrackParameters.


*/
