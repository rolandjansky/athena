#!/bin/bash
# test for AtlCoolCopy, copying some data from Oracle INTR to SQLite
# rely on authentication file for authorisation
export DBORACLE="oracle://INTR;schema=ATLAS_COOL_GLOBAL;dbname=REFC2001"

AtlCoolCopy COOLOFL_INDET/OFLP200 "sqlite://X;schema=test0.db;dbname=OFLP200" -create -folder /Indet/Align
rm -f test0.db
exit
