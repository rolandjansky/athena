#!/bin/bash
#
# Script running the TileDigitsContByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileDigitsContByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileDigitsContByteStreamCnv_test.py
diff -ur TileDigitDumps-0 TileDigitDumps-4
