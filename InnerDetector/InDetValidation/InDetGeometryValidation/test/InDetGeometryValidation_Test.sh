#!/bin/bash

# In the CMake build just skip this whole thing:
if [ -z "${CMTPATH}" ]; then
    exit
fi

echo "## Running in $PWD"
cd ../../../InnerDetector/InDetValidation/InDetGeometryValidation/cmt/
gmake
exit
