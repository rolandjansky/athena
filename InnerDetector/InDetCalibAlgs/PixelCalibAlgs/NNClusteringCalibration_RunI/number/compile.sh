#/bin/bash

filename=$(echo "submit.sh")

rm $filename
touch $filename

echo "#!/bin/sh" >> $filename
echo "cd  $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration" >> $filename
echo "make clean;make " >> $filename
echo "cd $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/number" >> $filename
echo "root -b -q compile.C" >> $filename
echo "cd $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/positions" >> $filename
echo "root -b -q compile.C" >> $filename
echo "cd $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors" >> $filename
echo "root -b -q compile.C" >> $filename
echo "echo done" >>$filename

chmod a+x $filename

#bsub -J  "compile JetNet" -q atlascatlong $filename
bsub -J  "compile JetNet" -q atlasb1 $filename
