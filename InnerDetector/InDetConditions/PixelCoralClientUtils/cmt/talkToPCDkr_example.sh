###############################################################################
#!/bin/sh
###############################################################################

rm -f testdb.sql

export CAN_CALIB_DB="sqlite_file:testdb.sql"
#export CAN_CALIB_DB=oracle://devdb10/atlas_pixel_khanov

talkToPCDkr -s hello/world mytag
talkToPCDkr -s hello/again mytag1
talkToPCDkr -s hello/oncemore mytag2
talkToPCDkr -r
talkToPCDkr -v hello/again 
talkToPCDkr -r
