WORKDIR=${1:-digitization}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

INPUTDIR="/afs/cern.ch/work/s/stelzer/LegacySimWorkareaForWernerJoerg/hitfiles"
INPUTHITS="HITS.00857308._000585.pool.root.1"
INPUTMBLOWPT="HITS.743321._[001996,001997,001998,001999,002000].pool.root"
INPUTMBHIGHPT="HITS.743322._[005996,005997,005998,005999,006000].pool.root"

OUTPUT="digiRDO.pool.root"

DIGICON=OFLCOND-MC12-SDR-06
DIGIGEO=ATLAS-GEO-20-00-01

echo "Input files dir: "$INPUTDIR
echo "Input hits     : "$INPUTHITS
echo "Low pt minbias : "$INPUTMBLOWPT
echo "High pt minbias: "$INPUTMBHIGHPT
echo
echo "Output: "$OUTPUT
echo
echo "Conditions: "$DIGICON
echo "Geometry  : "$DIGIGEO
echo


DigiMReco_trf.py \
    inputHitsFile="$INPUTDIR/$INPUTHITS" \
    maxEvents="50" \
    skipEvents="50" \
    preInclude_h2r="Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py,RunDependentSimData/configLumi_mc12_v1.py" \
    preExec_h2r="from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.UseDelta.set_Value_and_Lock(3);from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=[\"SimLayout\",\"PhysicsList\"]" \
    RunNumber="126940" \
    autoConfiguration="everything" \
    conditionsTag="$DIGICON" \
    geometryVersion="$DIGIGEO" \
    outputRDOFile="$OUTPUT" \
    numberOfLowPtMinBias="39.954664" \
    numberOfHighPtMinBias="0.045336" \
    LowPtMinbiasHitsFile="$INPUTDIR/$INPUTMBLOWPT" \
    DataRunNumber="-1" \
    jobNumber="1200" \
    digiSeedOffset1="1200" \
    digiSeedOffset2="1200" \
    HighPtMinbiasHitsFile="$INPUTDIR/$INPUTMBHIGHPT" \
    digiRndmSvc="AtDSFMTGenSvc"
