#!/bin/bash
#
# Script running the TileRawChannelContByteStreamCnv_test.py test with CTest.
#

# Run the job:
athena.py TileByteStream/TileRawChannelContByteStreamCnv_test.py
athena.py --thread=4 TileByteStream/TileRawChannelContByteStreamCnv_test.py
diff -ur TileRawChannelDumps-0 TileRawChannelDumps-4
