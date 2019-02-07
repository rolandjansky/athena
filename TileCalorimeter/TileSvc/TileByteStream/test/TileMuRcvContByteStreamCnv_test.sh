#!/bin/bash
#
# Script running the TileMuRcvContByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileMuRcvContByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileMuRcvContByteStreamCnv_test.py
diff -ur TileMuRcvDumps-0 TileMuRcvDumps-4
