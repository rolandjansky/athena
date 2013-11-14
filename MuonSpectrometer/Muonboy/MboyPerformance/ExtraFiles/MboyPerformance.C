/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  // Compile and run cbntana, which makes histogram file
  gSystem->Exec("/bin/sh -c \"(printenv; date; gmake -k -f Makefile) > ntreader.log 2>&1\"");
  gSystem->Exec("/bin/sh -c \"(rm -f out.root; /bin/echo '==============='; ./ntreader ntuple.root)>> ntreader.log 2>&1\"");
}

