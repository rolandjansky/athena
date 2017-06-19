#!/bin/bash
# test for AtlCoolCopy, copying some data from Oracle INTR to SQLite
# rely on authentication file for authorisation
export DBORACLE="oracle://INTR;schema=ATLAS_COOL_GLOBAL;dbname=REFC2001"

echo Start date is `date`

AtlCoolCopy COOLOFL_INDET/OFLP200 "sqlite://X;schema=test0.db;dbname=OFLP200" -create -folder /Indet/Align
ls -l
rm test0.db
echo End date is `date`
exit
