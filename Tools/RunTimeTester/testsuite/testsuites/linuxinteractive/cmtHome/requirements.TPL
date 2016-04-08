set CMTSITE CERN
set SITEROOT /afs/cern.ch
macro ATLAS_DIST_AREA $(SITEROOT)/atlas/software/dist
macro ATLAS_TEST_AREA $_BASE_$/testsuites/$_TESTNAME_$
use AtlasLogin AtlasLogin-* $(ATLAS_DIST_AREA)
