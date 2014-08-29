#!/bin/bash
# test for AtlCoolCopy, copying some data from Oracle INTR to SQLite
# rely on authentication file for authorisation
export DBORACLE="oracle://INTR;schema=ATLAS_COOL_GLOBAL;dbname=REFC2001"

echo Start date is `date`

AtlCoolCopy.exe $DBORACLE "sqlite://X;schema=test0.db;dbname=REFC2001" -create -folder /REFCOND1/LAR/FOLDER_0 
ls -l
rm test0.db
echo End date is `date`
exit
