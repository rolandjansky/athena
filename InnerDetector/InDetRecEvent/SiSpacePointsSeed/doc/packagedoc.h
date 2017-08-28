/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiSpacePointsSeed_page SiSpacePointsSeed
@author Igor.Gavrilenko@cern.ch
@section SiSpacePointsSeed_SiSpacePointsSeedIntro Introduction

This class represent possible track candidate using only space points information.

Maind data member are:

1. m_spacepoints - is list of pointers to space points sorted in increasing radius order. Fist space points
                   in the list should has mimimal radius.

2. m_zvertex     - is estimation possible track position for radius = 0.

This class contains information after first step of pattern recognition using space points
information and is input for local track finding in the road.


*/
