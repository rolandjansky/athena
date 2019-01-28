#!/bin/bash
#
# Script running the TileLaserObjByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileLaserObjByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileLaserObjByteStreamCnv_test.py
diff -ur TileLaserDumps-0 TileLaserDumps-4
