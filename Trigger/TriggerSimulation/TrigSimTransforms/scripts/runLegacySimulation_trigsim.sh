#!/bin/sh

WORKDIR=${1:-bs2trigbs}

echo "Simulation workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

INPUTBS="../rdo2bs/digiBS.data"

OUTPUTBS="trigBS.data"

GEOMTAG="ATLAS-GEO-21-02-02"
CONDTAG="OFLCOND-MC12b-SDR-01"
#CONDTAG="OFLCOND-RUN12-SDR-25"
#GEOMTAG="ATLAS-R1-2012-02-00-00"  # crashes
TRIGCONFIG='all=MCRECO:DB:TRIGGERDBMC_RUN1:325,142,266'
ASETUP="b2tb:17.2.11.2,slc5"

echo "Input file : "$INPUTBS
echo "Output     : "$OUTPUTBS
echo
echo "Conditions : "$CONDTAG
echo "Geometry   : "$GEOMTAG
echo "Trigger    : "$TRIGCONFIG
echo

test -n "$ASETUP" && ASETUP="--asetup \"$ASETUP\""

eval TrigAndReco_tf.py --inputBS_OLDFile $INPUTBS --outputBS_TRIG_OLDFile $OUTPUTBS \
    --imf false \
    --triggerConfig $TRIGCONFIG \
    --geometryVersion "b2tb:$GEOMTAG" --conditionsTag "b2tb:$CONDTAG" \
    --copyJO "b2tb:TrigSimTransforms/skeleton.BStoTRIGBS.py" \
    $ASETUP \
    > log  2>&1
