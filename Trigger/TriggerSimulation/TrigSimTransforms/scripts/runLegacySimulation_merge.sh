#!/bin/sh
#
# Script which runs three-step legacy trigger simulation:
#   1. RDO to BS conversion
#   2. Trigger simulation on BS file using some legacy release
#      producing BS_OLD output file
#   3. Merging of RDO and BS_OLD file into output RDO
#

WORKDIR=${1:-trigbs2rdo}

echo "Merge workdir  : "$WORKDIR
rm -rf $WORKDIR && mkdir $WORKDIR && cd $WORKDIR

#INPUTRDO='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc12_8TeV.208002.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6mu6.recon.RDO.e1331_a159_a190_tid01180650_00/RDO.01180650._000002.pool.root.1'
INPUTRDO='../digitization/digiRDO.pool.root'
INPUTBS='../bs2trigbs/trigBS.data'

OUTPUTRDO="trigRDO.pool.root"

CONDTAG=OFLCOND-MC12-SDR-06
GEOMTAG=ATLAS-R1-2012-02-00-00

echo
echo "Input RDO file : "$INPUTRDO
echo "Input BS file  : "$INPUTBS
echo "Output         : "$OUTPUTRDO
echo

TrigAndReco_tf.py \
    --inputBS_TRIG_OLDFile $INPUTBS \
    --inputRDOFile $INPUTRDO \
    --outputRDO_TRIG_OLDFile $OUTPUTRDO \
    --geometryVersion "tb2tr:$GEOMTAG" \
    --conditionsTag "tb2tr:$CONDTAG" \
    > log  2>&1
