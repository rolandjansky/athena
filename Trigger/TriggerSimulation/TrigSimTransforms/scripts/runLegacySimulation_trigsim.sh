WORKDIR=${1:-bs2trigbs}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

export INPUTBS="../tmpfiles/tmp.BS_OLD"

export OUTPUTBS="trigBS.data"

export DIGICON="OFLCOND-MC12b-SDR-01"
export DIGIGEO="ATLAS-GEO-21-02-02"

echo "Input file : "$INPUTBS
echo "Output     : "$OUTPUTBS
echo
echo "Conditions : "$DIGICON
echo "Geometry   : "$DIGIGEO
echo

#get_files -jo TrigSimTransforms/athena_BStoBS_Example.py

#athena.py athena_BStoBS_Example.py 2>&1 > BStoTrigBS.log

TrigAndReco_tf.py --inputBS_OLDFile $INPUTBS --outputBS_TRIGFile $OUTPUTBS \
--imf false \
--asetup "b2tb:--testarea=/afs/cern.ch/user/s/stelzer/legacySimulation/SimRelease AtlasProduction,17.2.11.2,slc5"  --triggerConfig 'all=MCRECO:DB:TRIGGERDBMC:325,142,266' \
--geometryVersion "b2tb:ATLAS-GEO-21-02-02" --conditionsTag "b2tb:OFLCOND-MC12b-SDR-01" \
> log  2>&1




