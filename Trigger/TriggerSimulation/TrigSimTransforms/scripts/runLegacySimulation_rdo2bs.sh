WORKDIR=${1:-rdo2bs}

echo "Digitization workdir: "$WORKDIR
rm -rf $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

export INPUTRDO="../digiRDO.pool.root"

export OUTPUTBS="digiBS.data"

echo "Input file : "$INPUTRDO
echo "Output     : "$OUTPUTBS
echo "Workdir    : "$WORKDIR 

TrigAndReco_tf.py --inputRDOFile $INPUTRDO --outputBS_OLDFile $OUTPUTBS \
--imf false \
--triggerConfig 'all=MCRECO:DB:TRIGGERDBMC:325,142,266' \
> log 2>&1 &

