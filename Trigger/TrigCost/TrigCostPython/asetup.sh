###############################################################################
#
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Athena setup for TrigCostPython/makeRoot.py
#
###############################################################################

# 2011: command archived here (previously called ./elliot.sh)
#source $AtlasSetup/scripts/asetup.sh AtlasProduction,17.0.4.2,runtime,opt,32 --testarea=`pwd`

# 2012: command -- expanded
#rel=17.0.4,builds,here                     # 2012 DataSummary : runs AtlRunQuery
#rel=17.0.4.2                               # 2011 xmon        : can't run AtlRunQuery
#rel=17.0.5.6                               # kunkle recommend : can't run AtlRunQuery
#rel=17.1.X.Y-VAL,rel0,builds,runtime,here  # aidan recommend  : runs AtlRunQuery
rel=17.0.4,builds                           # 2012 xmon

AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
TestArea=/afs/cern.ch/user/$(echo $USER|cut -c1)/$USER/public/cmthome
echo "Source $AtlasSetup/scripts/asetup.sh $rel --testarea=$TestArea"
source $AtlasSetup/scripts/asetup.sh $rel --testarea=$TestArea
echo "Done asetup"

#eof
