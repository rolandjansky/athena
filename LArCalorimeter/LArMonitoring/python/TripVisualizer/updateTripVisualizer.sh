
basePath=/afs/cern.ch/user/l/larmon/public/TripVisualizer

pushd $basePath  #seems like the visualizer package was not written with absolute paths, temporary workaround



if [ "$AtlasSetup" = "" ]; then
    export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
    source $AtlasSetup/scripts/asetup.sh 16.6.2.4
fi

last=`ls -t ${basePath}/HVHistoryDat | head -1 |sed -e "s/trip_//" -e "s/.dat// " `
echo $last

next=$((last+1))
echo "generating dat files starting from TripId=$next"

python ${basePath}/GenerateHVDatFiles.py $next | tee .log

cat .log |
while read dummy dummy dummy dummy tripId dummy
  do
  if [ "$tripId" != "" ]; then
      echo "generating plots for trip $tripId"
      python ${basePath}/GeneratePlot.py $tripId
  fi 
  tripId=""
done

echo "Trip Visualizer has been updated!"

popd

