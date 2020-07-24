#
#  File specifying the location of Pythia 8 to use.
#

message( INFO "Setting Pythia8 env. variables from AthGeneration" )
set( PYTHIA8_LCGVERSION 244p3 )
set( PYTHIA8_LCGROOT
   ${LCG_RELEASE_DIR}/MCGenerators/pythia8/${PYTHIA8_LCGVERSION}/${LCG_PLATFORM} )
