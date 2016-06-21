#!/bin/sh

WORKDIR=${1:-bs2trigbs}

echo "Simulation workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

INPUTBS="../rdo2bs/digiBS.data"

OUTPUTBS="trigBS.data"

DIGICON="OFLCOND-MC12b-SDR-01"
DIGIGEO="ATLAS-GEO-21-02-02"
#DIGICON="OFLCOND-RUN12-SDR-25"
#DIGIGEO="ATLAS-R2-2015-02-01-00"
TRIGCONFIG='all=MCRECO:DB:TRIGGERDBMC:325,142,266'
#ASETUP="b2tb:--testarea=/afs/cern.ch/user/s/salnikov/work/public/LegacySimulation/17.2.11.2 AtlasProduction,17.2.11.2,slc5"
ASETUP="b2tb:17.2.11.2,slc5"

echo "Input file : "$INPUTBS
echo "Output     : "$OUTPUTBS
echo
echo "Conditions : "$DIGICON
echo "Geometry   : "$DIGIGEO
echo "Trigger    : "$TRIGCONFIG
echo

test -n "$ASETUP" && ASETUP="--asetup \"$ASETUP\""

eval TrigAndReco_tf.py --inputBS_OLDFile $INPUTBS --outputBS_TRIG_OLDFile $OUTPUTBS \
    --imf false \
    --triggerConfig $TRIGCONFIG \
    --geometryVersion "b2tb:$DIGIGEO" --conditionsTag "b2tb:$DIGICON" \
    --copyJO "b2tb:TrigSimTransforms/skeleton.BStoTRIGBS.py" \
    $ASETUP \
    > log  2>&1
