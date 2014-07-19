###############################################################################
#!/bin/sh
###############################################################################

export PATH=$PATH:$PWD/../i686-slc5-gcc43-opt/calibDbInterface.exe 

export PIXEL_CAL_KNOWLEDGE="oracle://INTR/ATLAS_MLIMPER"
#export PIXEL_CAL_KNOWLEDGE="oracle://DEVDB10/ATLAS_COOL_MLIMPER"
#export PIXEL_CAL_KNOWLEDGE="sqlite_file:/detpriv/pix/data/PixCalibDataOffline/pcd_knowledge.db"

#store the latest updated calibration strings
calibDbInterface.exe -s PixCalib-BLKP-UPD4-000-01 string2 string3
