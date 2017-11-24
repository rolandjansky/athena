#!/bin/sh
#
# Script which runs three-step legacy trigger simulation:
#   1. RDO to BS conversion
#   2. Trigger simulation on BS file using some legacy release
#      producing BS_OLD output file
#   3. Merging of RDO and BS_OLD file into output RDO
#

WORKDIR=${1:-rdo2trigger2rdo}

echo "Combined workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

#INPUTRDO='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2579_r6164_tid05062245_00/RDO.05062245._000003.pool.root.1'
#INPUTRDO=/afs/cern.ch/work/s/stelzer/LegacySimWorkareaForWernerJoerg/digitizedRDO/digiRDO.pool.root
INPUTRDO='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/LegacySim/digitizedRDO/digiRDO.pool.root'

OUTPUTRDO="trigRDO.pool.root"

CONDTAG="all:OFLCOND-MC12b-SDR-01"

GEOMTAG="ATLAS-R1-2012-02-00-00 b2tb:ATLAS-GEO-21-02-02"

TRIGCONFIG='default=MCRECO:MC_pp_v6'
TRIGCONFIG_OLD='b2tb=MCRECO:DB:TRIGGERDBMC_RUN1:325,142,266'

ASETUP="b2tb:17.2.11.2,slc5"
COPYJO="b2tb:TrigSimTransforms/skeleton.BStoTRIGBS.py"

echo
echo "Input RDO file : "$INPUTRDO
echo "Output         : "$OUTPUTRDO
echo
echo "Conditions     : "$CONDTAG
echo "Geometry       : "$GEOMTAG
echo "Trigger        : "$TRIGCONFIG $TRIGCONFIG_OLD
echo

#
# Note --ignoreErrors option, some (or all steps) are still producing ERROR
# messages which are not fatal but need futher work on configuration to avoid
# them, once all ERROR messages are suppressed this otion can be removed
#
TrigAndReco_tf.py \
    --inputRDOFile $INPUTRDO \
    --outputRDO_TRIG_OLDFile $OUTPUTRDO \
    --triggerConfig $TRIGCONFIG $TRIGCONFIG_OLD \
    --geometryVersion $GEOMTAG \
    --conditionsTag $CONDTAG \
    --asetup "$ASETUP" \
    --copyJO "$COPYJO" \
    --outputBSEformatVersion "r2bo:run1" \
    --outputBSEventStorageVersion "r2bo:run1" \
    --imf false \
    > log  2>&1
