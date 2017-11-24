#!/bin/sh

WORKDIR=${1:-rdo2bs}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

#INPUTRDO='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2579_r6164_tid05062245_00/RDO.05062245._000003.pool.root.1'
#INPUTRDO='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/LegacySim/digitizedRDO/digiRDO.pool.root'
INPUTRDO='../digitization/digiRDO.pool.root'
OUTPUTBS="digiBS.data"

TRIGCONFIG='all=MCRECO:MC_pp_v6'
#TRIGCONFIG='all=MCRECO:DB:TRIGGERDBMC_RUN1:325,142,266'
CONDTAG="OFLCOND-MC12-SDR-06"
GEOMTAG=ATLAS-R1-2012-02-00-00

echo "Input file : "$INPUTRDO
echo "Output     : "$OUTPUTBS
echo "Workdir    : "$WORKDIR
echo
echo "Conditions : "$CONDTAG
echo "Geometry   : "$GEOMTAG
echo "Trigger    : "$TRIGCONFIG

TrigAndReco_tf.py --inputRDOFile $INPUTRDO --outputBS_OLDFile $OUTPUTBS \
    --imf false \
    --maxEvents 100 \
    --triggerConfig $TRIGCONFIG \
    --geometryVersion "r2bo:$GEOMTAG" \
    --conditionsTag "r2bo:$CONDTAG" \
    --outputBSEformatVersion "r2bo:run1" \
    --outputBSEventStorageVersion "r2bo:run1" \
    > log 2>&1
