#!/bin/bash
# test for CoraCoolExample, writing and reading from Oracle
export DBORACLE="oracle://devdb11;schema=ATLAS_COOLTEST;dbname=CTT$$;user=ATLAS_COOLTEST;password=Cool43devdb"
# temporarily set TNS_ADMIN to override incorrect default in release 19 (2/7/14)
export TNS_ADMIN=/afs/cern.ch/sw/lcg/external/oracle/11.2.0.3.0/admin

echo Start date is `date`
CoraCoolExample.exe $DBORACLE wrbpj /CoraCool/Test1 3 10 5 5 7 9
coolDropDB $DBORACLE
echo End date is `date`
exit
