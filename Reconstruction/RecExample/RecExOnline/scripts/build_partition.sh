#!/bin/sh

if [ $# -ne 2 ] ; then
    echo "Usage: ${0##*/} PARTITION DATAFILE" 1>&2
    exit 1
fi
if [ ! -f "$2" ] ; then
    echo "${0##*/}: ${2}: No such file" 1>&2
    exit 1
fi

PARTITION="$1"
DATAFILE="$2"

# Generate ROB list (only needed when data file changes)
if [ "${DATAFILE}" -nt robhit.py ] ; then
    rosconf-from-data.py -p "${DATAFILE}" > robhit.py
fi

# Generate monitoring segment
export PYTHONPATH=".:${PYTHONPATH}"
#pm_part_l2ef.py -p "${PARTITION}" --data="['${DATAFILE}']" --ros-farm=RecExOnline.ros_farm --pts-per-efd=1 --l2pus-per-node=1
# pm_part_l2ef is outdated and can be replaced by 
pm_part_hlt.py -p "${PARTITION}" --data="['${DATAFILE}']" --ros-farm=RecExOnline.ros_farm 

# Run this script once for each new tdaq release.
# You can then start the partition with
# setup_daq -p "${PARTITION}" -d "${PARTITION}.data.xml"
