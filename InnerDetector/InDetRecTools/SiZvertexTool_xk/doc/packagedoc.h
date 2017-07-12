/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiZvertexTool_xk_page SiZvertexTool_xk
@author Igor.Gavrilenko@cern.ch
@section SiZvertexTool_xk_SiZvertexTool_xkIntro Introduction

This tool is concrete implementation for ISiZvertexMaker
and is devoted for estimation position all primary vertices along beam direction.

 
This tool uses histogramming method. It takes all space point seeds with two pixel space points
and fill one dimension histogramm of Z-position this seeds extrapolated to radius = 0. Once this histogramm
has been filled primary vertices can be detected by simply scanning the contents of the cumulative array.
     
This algorithm uses ISiSpacePointsSeedMaker tool for space point seeds production.


*/
