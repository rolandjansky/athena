###############################################################################
#!/bin/sh
###############################################################################

rm -f testdb.sql

export CAN_CALIB_DB="sqlite_file:testdb.sql"
#export CAN_CALIB_DB=oracle://devdb10/atlas_pixel_khanov

./pcd.sh init mytag pixelcalibsvc_dump_assembly_02.txt 
./pcd.sh update mytag 5620 -t
./pcd.sh copy mytag1 mytag
