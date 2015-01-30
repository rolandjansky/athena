#!/bin/bash
if [[ ${#} != 2 ]] ; then
    echo "usage:" $0 "inputdataset outputpath"
    exit
fi

for ((reg=0;reg<64;reg++)) ; do
    file=${1}${reg}.pcache.root
    filenopath=${file##*/}
    dest=$2/${filenopath%%.pcache.root}
    echo $file "->" $dest
    steer=makecompressedbank.reg${reg}.steer
    cat <<EOF > $steer
LOGGING_PRINT_LEVEL 5
NINPUTS 1 ${file}
PATTERNSBYSECTOR ${dest}.bysector.root

BINARYFILEOUT ${dest}.compressed.binary
ROOTFILEOUT ${dest}.compressed.root
CACHEDBANKFILEOUT ${dest}.ordered.pcache.root
# specify FTK geometry -- has to match geometry in pcache.root file
PMAP ../config/map_file/raw_8LcIbl123.pmap
RMAP ../config/map_file/raw_12Libl.tmap
SSMAP ../config/ss_file/raw_30x32x72Ibl.ss
TSP_SSMAP ../config/ss_file/raw_15x16x36Ibl.ss
EOF
    ../standalone/makecompressedbank $steer
    rm $steer
done
