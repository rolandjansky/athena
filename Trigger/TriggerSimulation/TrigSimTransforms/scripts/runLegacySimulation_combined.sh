WORKDIR=${1:-rdo2trigger2esd}

echo "Combined workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

export INPUTRDO="../digiRDO.pool.root"

export OUTPUTRDO="trigRDO.pool.root"

echo "Input file     : "$INPUTRDO
echo
echo "Output: "$OUTPUTRDO
echo

TrigAndReco_tf.py --inputRDOFile $INPUTRDO --outputRDO_TRIGFile $OUTPUTRDO \
--imf false \
--asetup "b2tb:--testarea=/afs/cern.ch/user/s/stelzer/legacySimulation/SimRelease AtlasProduction,17.2.11.2,slc5"  --triggerConfig 'all=MCRECO:DB:TRIGGERDBMC:325,142,266' \
--geometryVersion "b2tb:ATLAS-GEO-21-02-02" --conditionsTag "b2tb:OFLCOND-MC12b-SDR-01" \
2>&1 | tee log 

