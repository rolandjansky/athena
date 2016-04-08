/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

The CaloIdCnv package contains the converters which are necessary
to initialize the Calorimeter helpers and identifier managers. <br>
There is one converter per helper or manager. <br>
And there are 3 LAr offline helpers, 2 Tile offline helpers, 3 Calo-wide
offline helpers, 1 Trigger Tower online helper and 1 id manager. <br>
TileTBID helper is a bit special - it is not used for normal calorimeter cells,
but used for MBTS cells in full ATLAS reconstruction as
well as for ancillary detectors at the testbeam (like Muon Wall or beam chambers)

@htmlinclude used_packages.html

@include requirements

*/
