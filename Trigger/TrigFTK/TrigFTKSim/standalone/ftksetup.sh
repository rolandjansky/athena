###HEADER###
echo "This script has to be placed before TrigFTKSim, if you did just remove this header"
return
###HEADER###
export FTKSIM=${PWD}/TrigFTKSim
export FTKANA=${PWD}/TrigFTKAna

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh # --quiet
#asetup 17.3.10.1.18,IBLProd,here,64
asetup 19.0.0,here
. $FTKSIM/grid/dependencies.sh
export LD_LIBRARY_PATH=$FTKSIM/standalone:$LD_LIBRARY_PATH
export PATH=$PATH:$FTKANA/plot:$FTKSIM/scripts:$FTKSIM/standalone_scripts
localSetupDQ2Wrappers # --quiet
localSetupPandaClient # --quiet

# make a a proxy (if needed)
timeleft=$(voms-proxy-info -timeleft)
if [ $? -eq 1 ]; then
    voms-proxy-init -voms atlas    
elif [ $timeleft -le 3600 ]; then
    voms-proxy-init -voms atlas
else
    echo "A proxy was found, valid for about: $((timeleft/3600)) hours"
fi
