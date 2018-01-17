WORKDIR=${1:-digitization}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

#INPUTDIR="/afs/cern.ch/work/s/stelzer/LegacySimWorkareaForWernerJoerg/hitfiles"
INPUTDIR="root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/LegacySim/hitfiles"
INPUTHITS="mc12_8TeV.126940.PowhegPythia8_AU2CT10_ZZ_4mu_mll4_2pt5.merge.HITS.e1280_s1469_s1470_tid00857308_00/HITS.00857308._000585.pool.root.1"

# globbing does not work well with EOS, need to specify each individual file
#INPUTMBLOWPT="HITS.743321._[001996,001997,001998,001999,002000].pool.root"
INPUTMBLOWPT="\
mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471_tid743321_00/HITS.743321._001996.pool.root.5 \
mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471_tid743321_00/HITS.743321._001997.pool.root.6 \
mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471_tid743321_00/HITS.743321._001998.pool.root.5 \
mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471_tid743321_00/HITS.743321._001999.pool.root.4 \
mc12_8TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1119_s1469_s1471_tid743321_00/HITS.743321._002000.pool.root.4 \
"
#INPUTMBHIGHPT="HITS.743322._[005996,005997,005998,005999,006000].pool.root"
INPUTMBHIGHPT="\
mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471_tid743322_00/HITS.743322._005996.pool.root.1 \
mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471_tid743322_00/HITS.743322._005997.pool.root.1 \
mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471_tid743322_00/HITS.743322._005998.pool.root.1 \
mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471_tid743322_00/HITS.743322._005999.pool.root.1 \
mc12_8TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e1119_s1469_s1471_tid743322_00/HITS.743322._006000.pool.root.1 \
"

OUTPUT="digiRDO.pool.root"

CONDTAG=OFLCOND-MC12-SDR-06
GEOMTAG=ATLAS-R1-2012-02-00-00

echo "Input files dir: "$INPUTDIR
echo "Input hits     : "$INPUTHITS
echo "Low pt minbias : "$INPUTMBLOWPT
echo "High pt minbias: "$INPUTMBHIGHPT
echo
echo "Output: "$OUTPUT
echo
echo "Conditions: "$CONDTAG
echo "Geometry  : "$GEOMTAG
echo


Reco_tf.py \
    --inputHITSFile="$INPUTDIR/$INPUTHITS" \
    --maxEvents="50" \
    --skipEvents="50" \
    --runNumber="126940" \
    --preInclude="h2r:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py,RunDependentSimData/configLumi_mc12_v1.py" \
    --preExec="h2r:from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.UseDelta.set_Value_and_Lock(3);from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=[\"SimLayout\",\"PhysicsList\"];from GeoModelSvc.GeoModelSvcConf import GeoModelSvc;GeoModelSvc = GeoModelSvc();GeoModelSvc.IgnoreTagDifference = True" \
    --autoConfiguration="everything" \
    --conditionsTag="$CONDTAG" \
    --geometryVersion="$GEOMTAG" \
    --outputRDOFile="$OUTPUT" \
    --numberOfLowPtMinBias="39.954664" \
    --numberOfHighPtMinBias="0.045336" \
    $(for path in $INPUTMBLOWPT; do echo --inputLowPtMinbiasHitsFile=$INPUTDIR/$path; done) \
    --DataRunNumber="-1" \
    --jobNumber="1200" \
    --digiSeedOffset1="1200" \
    --digiSeedOffset2="1200" \
    $(for path in $INPUTMBHIGHPT; do echo --inputHighPtMinbiasHitsFile=$INPUTDIR/$path; done) \
    --digiRndmSvc="AtDSFMTGenSvc" \
    > log 2>&1
