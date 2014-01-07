#/bin/bash

#./runDir.sh $(pwd)/noTrack_200_1 false 200 0.3 0.99 0.1 1 1
#./runDir.sh $(pwd)/noTrack_200_2 false 200 0.3 0.99 0.1 2 2
#./runDir.sh $(pwd)/noTrack_200_3 false 200 0.3 0.99 0.1 3 3
#./runDir.sh $(pwd)/track_200_1 true 200 0.3 0.99 0.1 1 4
#./runDir.sh $(pwd)/track_200_2 true 200 0.3 0.99 0.1 2 5
#./runDir.sh $(pwd)/track_200_3 true 200 0.3 0.99 0.1 3 6
#./runDir.sh $(pwd)/noTrack_20_1 false 20 0.03 0.95 0.03 1 7
#./runDir.sh $(pwd)/noTrack_20_2 false 20 0.03 0.95 0.03 2 8
#./runDir.sh $(pwd)/noTrack_20_3 false 20 0.03 0.95 0.03 3 9
#./runDir.sh $(pwd)/track_20_1 true 20 0.03 0.95 0.03 1 10
#./runDir.sh $(pwd)/track_20_2 true 20 0.03 0.95 0.03 2 11
#./runDir.sh $(pwd)/track_20_3 true 20 0.03 0.95 0.03 3 12


jobNumber=1

useTrackEstimate="false"
nPatternsPerUpdate=200
learningRate=0.3
learningRateDecrease=0.99
learningRateMomentum=0.1

directory=$(pwd)/myDir$jobNumber

if ( test ! -z $1); then 
    directory=$1
fi

if ( test ! -z $2); then 
    useTrackEstimate=$2
fi

if ( test ! -z $3); then 
    nPatternsPerUpdate=$3
fi

if ( test ! -z $4);then 
    learningRate=$4
fi

if ( test ! -z $5); then 
    learningRateDecrease=$5
fi

if ( test ! -z $6); then
    learningRateMomentum=$6
fi

if ( test ! -z $7); then
    nParticles=$7
fi

if ( test ! -z $8); then
    jobNumber=$8
fi

echo "Directory is: " $directory

#jobNumber=$startFromNumber
#let lastJobNumber=$jobNumber+$numberJobsToProcess

echo "Creating directories if missing"
if $(test ! -e $directory);then mkdir -p $directory;fi


#mkdir $directory
cd $directory

echo "Linking necessary macros..."
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/libTJetNet.so 
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/positions/trainNN_cxx.so
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/positions/doTraining.C

    
actualdir=$(pwd)

echo "Create batch standalone file for " $a
    
filename=$(echo $directory'/submitJob.sh')

echo '================='
echo $filename
echo '================='
    
if (test -e $filename);then
    rm $filename
fi
    
touch $filename

echo "#!/bin/zsh" >> $filename
echo "" >> $filename
echo "unset DISPLAY" >> $filename
echo "echo Runnin on host \`hostname\`" >> $filename
#    echo "export LD_LIBRARY_PATH=/afs/cern.ch/user/g/giacinto/scratch0/MCFM/lhapdf-5.8.4/lib/.libs/:$LD_LIBRARY_PATH">>$filename 
echo "cd  " $actualdir " " >> $filename
#echo "cd /tmp/" >> $filename
#echo "mkdir giacinto" >> $filename
#echo "cd /tmp/giacinto" >> $filename
#echo "dirName=\$(uuidgen)" >> $filename
#echo "echo Creating dir name" >> $filename
#echo "mkdir \$dirName" >> $filename
#echo "cd /tmp/giacinto/\$dirName" >> $filename
#echo "ln -s "$actualdir"/* ." >> $filename
#echo "./mcfm input.DAT >& logfile.txt" >> $filename
#    echo "echo Copying root file to castor" >> $filename
#    echo "echo Creating directory if not there " >> $filename
#    echo "nsmkdir /castor/cern.ch/user/g/giacinto/MCFM/"$directoryShort >> $filename
#    echo "nsmkdir /castor/cern.ch/user/g/giacinto/MCFM/"$directoryShort"/job-"$jobNumber >> $filename
#    echo "for a in \$(ls|egrep root\$);do xrdcp \$a root://castoratlas//castor/cern.ch/user/g/giacinto/MCFM/"$directoryShort"/job-"$jobNumber"/"$a";done" >> $filename
#    echo "echo Removing local file" >> $filename
#echo "for a in \$(ls|egrep root\$);do rm \$a;done" >> $filename
echo "root -b -q doTraining.C\("$nParticles","$useTrackEstimate","$nPatternsPerUpdate","$learningRate","$learningRateDecrease","$learningRateMomentum"\)" >> $filename
#echo "echo Copying back files" >> $filename
#echo "rm *.so" >> $filename
#echo "cp -rv * "$actualdir"/" >> $filename
#echo "rm -r /tmp/giacinto/\$dirName" >> $filename
echo "echo \"Finished\" " >>$filename
echo "echo \"$PWD\" ">>$filename     
chmod a+x $filename
    
#####bsub -J $(echo "NN-"$jobNumber) -q atlascatlong $filename
bsub -J $(echo "NN-"$jobNumber) -q atlasb1 $filename
#bsub -J $(echo "NN-"$jobNumber) -q 1nw $filename
cd $directory

#source $filename

    
