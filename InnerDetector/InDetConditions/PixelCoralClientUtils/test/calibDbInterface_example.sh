###############################################################################
#!/bin/sh
###############################################################################

export PATH=$PATH:$PWD/../i686-slc5-gcc43-opt/calibDbInterface.exe 

export PIXEL_CAL_KNOWLEDGE="oracle://INTR/ATLAS_MLIMPER"
#export PIXEL_CAL_KNOWLEDGE="oracle://DEVDB10/ATLAS_COOL_MLIMPER"
#export PIXEL_CAL_KNOWLEDGE="sqlite_file:pcd_knowledge.db"

#store the latest updated calibration strings (final argument) along with RunNumber (arg1) idTag (arg2) connTag (arg3) cfgTag (arg4) cfgModTag (arg5)
calibDbInterface.exe -s 183905 IDTAG1 CONNTAG1 CFGTAG1 CFGMODTAG1 PixCalib-BLKP-UPD4-000-01 CalibString2 CalibString3

#associate calibtag with specific run interval and tag=bla (cool is multi-tag)
#this step should be done by PixRCDCoolAgent, just here for demonstration...
# does not work anymore!
#calibDbInterface -c dummycalib_0 2 3 bla
