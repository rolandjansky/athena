#!/bin/bash
#
# Script running the TileBeamElemContByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileBeamElemContByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileBeamElemContByteStreamCnv_test.py
diff -ur TileBeamElemDumps-0 TileBeamElemDumps-4
