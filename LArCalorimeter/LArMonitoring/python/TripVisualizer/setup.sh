#export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
#source $AtlasSetup/scripts/asetup.sh 16.6.2.4

last=`ls -t HVHistoryDat | head -1 |sed -e "s/trip_//" -e "s/.dat// " `
echo $last

next=$((last+1))
echo $next

#python GenerateHVDatFiles.py $next
#python GenerateAllPlots.py  #should be a way to do this only for new ones...
