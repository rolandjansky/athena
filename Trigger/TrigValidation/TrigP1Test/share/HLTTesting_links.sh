# Setup links to all the necessary files to run HLTTesting.
# To be executed once in a given run directory.
#
# Syntax: HLTTesting_links.sh [MT testdir]
#
# If 'MT testdir' is given and the athenaMT job was successfull and
# produced an output file this file will be used for athenaPT

echo "running HLTTesting_links.sh"

# This is created by RecExCommon_links.so but we cannot use the replicas
rm -rf sqlite200

# Pre-made files for athenaMT/PT (base name only)
mtFile="/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/daq.physicsMCppv2"
ptFile="/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/daq.physicsMCppv2.Lvl2Result"

# Get nightly (dev, devval, 15.2.X.Y, etc.)
nightly=`echo $CMTPATH | sed -r 's;.*/builds/nightlies/(.*?)/Atlas.*;\1;'`

# Setup link
makelink() {
    file=$1.${nightly}.data
    if [ -e $file ]; then
        ln -sf $file $2
    else
        file=$1.latest.data
        ln -sf $file $2
    fi
    echo $file
}

echo "Using `makelink $mtFile athenaMT.data` as input to athenaMT"

# Setup link to athenaPT ByteStream file
# Either use pre-made file or output from athenaMT job

# Make sure no previous link exists
rm -f athenaPT.data    

if [[ $# -gt 0 ]]; then
    mtDir=$1                                       # Run dir of athenaMT test
    mtOutput=`ls -t1 ${mtDir}/*.data | head -1`    # Output of athenaMT test

    # Check if athenaMT test successfull
    egrep ATHENA_BAD_EXIT\|ATN_TIME_LIMIT ${mtDir}/summary.log &> /dev/null
    okExit=$?
    if [[ ${okExit} -eq 1 && -e ${mtOutput} ]]; then
        echo "Using athenaMT output '${mtOutput}' as input to athenaPT"
        ln -sf ${mtOutput} athenaPT.data
    fi
fi

if [[ ! -e athenaPT.data ]]; then
    echo "Using pre-made file `makelink ${ptFile} athenaPT.data` as input to athenaPT"
    
fi

# Get some additional files
get_files -symlink -jo athenaPTafterMT.py

#