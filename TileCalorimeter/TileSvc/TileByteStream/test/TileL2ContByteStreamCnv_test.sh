#!/bin/bash
#
# Script running the TileL2ContByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileL2ContByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileL2ContByteStreamCnv_test.py
diff -ur TileL2Dumps-0 TileL2Dumps-4
