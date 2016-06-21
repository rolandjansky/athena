#!/bin/sh

WORKDIR=${1:-rdo2bs}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

#INPUTRDO='root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2579_r6164_tid05062245_00/RDO.05062245._000003.pool.root.1'
#INPUTRDO='root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc12_8TeV.208002.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6mu6.recon.RDO.e1331_a159_a190_tid01180650_00/RDO.01180650._000002.pool.root.1'
INPUTRDO='../digitization/digiRDO.pool.root'
OUTPUTBS="digiBS.data"

TRIGCONFIG='all=MCRECO:MC_pp_v6'
#TRIGCONFIG='all=MCRECO:DB:TRIGGERDBMC:325,142,266'
CONDTAG="OFLCOND-MC12-SDR-06"
GEOMTAG="ATLAS-GEO-18-01-01"

echo "Input file : "$INPUTRDO
echo "Output     : "$OUTPUTBS
echo "Workdir    : "$WORKDIR
echo
echo "Conditions : "$CONDTAG
echo "Geometry   : "$GEOMTAG

TrigAndReco_tf.py --inputRDOFile $INPUTRDO --outputBS_OLDFile $OUTPUTBS \
    --imf false \
    --maxEvents 100 \
    --triggerConfig $TRIGCONFIG \
    --geometryVersion "r2bo:$GEOMTAG" \
    --conditionsTag "r2bo:$CONDTAG" \
    --outputBSEformatVersion "r2bo:run1" \
    --outputBSEventStorageVersion "r2bo:run1" \
    > log 2>&1
