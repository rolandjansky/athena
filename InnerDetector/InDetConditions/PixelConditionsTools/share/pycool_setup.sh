# setup for COOL/pycool
# to be sourced not run directly i.e.
# source pycool_setup.sh
#
# COOL version and path to LCG installation
setenv COOLVER COOL_1_3_2
setenv COOLSRC /afs/cern.ch/sw/lcg/app/releases/COOL/${COOLVER}/src
setenv SCRAM_ARCH slc3_ia32_gcc323
setenv PATH /afs/cern.ch/sw/lcg/app/spi/scram:${PATH}
#
pushd $COOLSRC
eval `scram runtime -csh`
setenv SEAL_CONFIGURATION_FILE ${COOLSRC}/RelationalCool/tests/seal.opts.error
setenv POOL_AUTH_PATH ${HOME}/private 
setenv CORAL_AUTH_PATH ${HOME}/private
popd
