export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

D='date "+%u"'
DATE=`eval $D`
printf "Day of week as number: %s \n" "$DATE"

source $AtlasSetup/scripts/asetup.sh 20.7.X.Y-VAL,rel_${DATE},AtlasDerivation,here --nightliesarea=/afs/cern.ch/atlas/software/builds/nightlies/

#Note 0 is Sunday, 1 is Monday etc until 6 is Saturday