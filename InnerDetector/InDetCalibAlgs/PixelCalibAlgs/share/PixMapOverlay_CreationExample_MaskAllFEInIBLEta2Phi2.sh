#############################################
#
# Authors : Sam Meehan <samuel.meehan@cern.ch>   
#
#############################################

#go into the share directory where the jobOptions exists and remove the old output if it exists
rm PixMapOverlay_Run2_MaskAllFEInEta2Phi2OfIBL_NoMaskedModules.db

##############
#set up your arguments for 
# outputlabel
# iov
# list of modules to mask
# list of FE's to mask
##############

export RUNARGS=""
RUNARGS="$RUNARGS OUTFILENAME='PixMapOverlay_Run2_MaskAllFEInEta2Phi2OfIBL_NoMaskedModules.db';"
RUNARGS="$RUNARGS DATABASE='OFLP200';"
RUNARGS="$RUNARGS TAGNAME='PixMapOverlay-EXAMPLE';"
RUNARGS="$RUNARGS RUN1=222222; LB1=0; "
RUNARGS="$RUNARGS RUN2=999999; LB2=0; "
RUNARGS="$RUNARGS MASKMODULES=''; "
RUNARGS="$RUNARGS MASKFRONTENDS=' "   
RUNARGS="$RUNARGS 33710080:[0,1,2] ' "


echo $RUNARGS

#run the job options
athena -c "$RUNARGS" PixMapOverlay_Run2_CommandLineArguments.py

cd ..