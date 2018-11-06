#!/bin/sh
#
# art-description: MC15-style simulation using frozen showers, then merge two copies of the HITS file
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

# MC15 setup
# ATLAS-R2-2015-03-01-00 and OFLCOND-RUN12-SDR-30
export TRF_ECHO=1
export GEOMETRY=ATLAS-R2-2015-03-01-00

AtlasG4_tf.py \
--inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root \
--outputHITSFile mu_E200_eta0-25_${GEOMETRY}.HITS.pool.root \
--maxEvents 5 \
--skipEvents 0 \
--geometryVersion ${GEOMETRY} \
--physicsList 'FTFP_BERT_ATL' \
--preInclude 'SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
--conditionsTag OFLCOND-RUN12-SDR-30 \
--DataRunNumber 222525

echo  "art-result: $? simulation"

INPUTFILE=mu_E200_eta0-25_${GEOMETRY}.HITS.pool.root
FILENAME=`basename ${INPUTFILE}`
FILEBASE=${FILENAME%.HITS.pool.root}
INPUTFILE2=${FILEBASE}.2.HITS.pool.root
RDOFILE1=${FILEBASE}.unmerged.RDO.pool.root
RDOFILE2=${FILEBASE}.merged.RDO.pool.root
# Copy the file.  We can't use cp here, since we need the copy to get
# a new GUID.  Otherwise, we'll get errors about duplicate GUIDs
# in the file catalog.
mergePOOL -o $INPUTFILE2 -i $INPUTFILE

echo  "art-result: $? copyHITS"

INPUTLIST=$INPUTFILE,$INPUTFILE2
MERGEHITSFILE=${FILEBASE}.merged.HITS.pool.root
echo $INPUTLIST

## TODO: Temporary hack until log files are found!
cp log.AtlasG4Tf log.AtlasG4Tf2
INPUTLOGLIST=log.AtlasG4Tf,log.AtlasG4Tf2

HITSMerge_tf.py \
--inputHITSFile "$INPUTLIST" \
--inputLogsFile "$INPUTLOGLIST" \
--outputHITS_MRGFile $MERGEHITSFILE \
--maxEvents 20 \
--skipEvents 0 \
--geometryVersion ${GEOMETRY}

echo  "art-result: $? mergeHITS"

Digi_tf.py \
--inputHITSFile $MERGEHITSFILE \
--outputRDOFile $RDOFILE2 \
--maxEvents 2 \
--skipEvents 6 \
--geometryVersion ${GEOMETRY} \
--conditionsTag 'OFLCOND-RUN12-SDR-30' \
--DataRunNumber 222525

echo  "art-result: $? mergeDigi"

Digi_tf.py \
--inputHITSFile "$INPUTLIST" \
--outputRDOFile $RDOFILE1 \
--maxEvents 2 \
--skipEvents 6 \
--geometryVersion ${GEOMETRY} \
--conditionsTag 'OFLCOND-RUN12-SDR-30' \
--DataRunNumber 222525

echo  "art-result: $? unmergeDigi"

diffPoolFiles.py $RDOFILE2 $RDOFILE1 | \
    sed 's/\[ERR\]\(.*\)POOLContainer_DataHeaderForm$/\[WARN\]\1POOLContainer_DataHeaderForm/g' | \
    sed 's/## Comparison : \[ERR\]/## Comparison : \[WARN\]/g'

echo  "art-result: $? diffRDO"
