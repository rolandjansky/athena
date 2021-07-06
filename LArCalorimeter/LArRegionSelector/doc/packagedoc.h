/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

@page RegSelCondAlg_LAr

This package contains one algorithm, RegSelCondAlg_LAr, which generates the tables of element extents
 and ROBids used by the region selector tools.<br>

 The code loops over each LAr TT
 and fills the class RegionSelectorLUT which includes for each element: <br>
 hash Id, <br>
 sampling (EM or had), <br>
 eta min, <br>
 eta max, <br>
 phi min, <br>
 phi max, <br>
 rob Ids  <br>

 Once filled, the tables are stored in DetectoGate. <br>





*/
