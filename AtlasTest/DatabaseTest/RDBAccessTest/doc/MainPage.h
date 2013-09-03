/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

RDBAccessTest is a simple test demonstrating the usage of RDBAccessSvc.
At the initialization phase the algorithm is connecting to the database
and getting the contents of TIFG structure (from TileCal) corresponding
to the ATLAS version ATLAS-00 (two records). Then the records of the
recordset are navigated using two different ways - by index and by iterator,
and the field values are printed out.

--------------------------------
  JOB OPTIONS 
--------------------------------

@include RDBAccessTest.py

--------------------------------
  REQUIREMENTS 
--------------------------------

@include requirements
@htmlinclude used_packages.html 

*/
