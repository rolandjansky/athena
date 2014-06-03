/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

This package contains one Service, LArRegionSelectorTable, which generates the tables of element extents
 and ROBids used by the region selector.<br>

 The code loops over each LAr TT
 and fills the class RegionSelectorLUT which includes for each element: <br>
 hash Id, <br>
 sampling (EM or had), <br>
 eta min, <br>
 eta max, <br>
 phi min, <br>
 phi max, <br>
 rob Ids  <br>

 Once filled, the tables are stored in StoreGate. <br>

There is an algorithm as well, LArRegionSelectorCheckOL,
meant to be run in standalone, for test purposes.<br>



@htmlinclude used_packages.html

@include requirements

*/
