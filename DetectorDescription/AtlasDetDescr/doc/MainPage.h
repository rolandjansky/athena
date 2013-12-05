/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

This package contains an implentation base class for all identifier
helper classes - AtlasDetectorID. This class contains the information
of the "upper levels" of identifiers, i.e. which detector system
(InDet, LAr, Tile, Muon) and which subdetector. All concrete helpers
derive from AtlasDetectorID and thus each one can respond to the
generic queries on the upper levels of identifiers. 

AtlasDetectorID also provides facilities for printing out identifiers,
e.g. show_to_string, and maintains the version tags.


@section requirements requirements 

@include requirements

@section uses Packages used

@htmlinclude used_packages.html

*/
