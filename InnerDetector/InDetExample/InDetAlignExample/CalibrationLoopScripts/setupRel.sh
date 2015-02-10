export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'
cd ../../../..
asetup 19.3.0.3,AtlasProduction,here,builds
cd InnerDetector/InDetExample/InDetAlignExample/CalibrationLoopScripts
export STAGE_SVCCLASS='atlcal'
