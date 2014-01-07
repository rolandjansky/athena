#/bin/bash

filename=$(echo "submit.sh")

rm $filename
touch $filename

echo "#!/bin/sh" >> $filename
echo "cd /afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011" >> $filename
echo "make clean;make " >> $filename
echo "root -b -q doCalibration.C" >> $filename
echo "echo done" >>$filename

chmod a+x $filename

bsub -J  "Prepare calibration" -q 1nd $filename