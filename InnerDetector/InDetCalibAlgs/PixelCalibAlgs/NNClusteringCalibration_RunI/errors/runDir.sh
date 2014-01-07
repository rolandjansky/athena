#!/bin/bash

#./runDir.sh $(pwd)/noTrack_200_1_X false 200 0.3 0.99 0.1 1 true 30 200
#./runDir.sh $(pwd)/noTrack_200_1_Y false 200 0.3 0.99 0.1 1 false 30 201
#./runDir.sh $(pwd)/noTrack_200_2_X false 200 0.3 0.99 0.1 2 true 25 202
#./runDir.sh $(pwd)/noTrack_200_2_Y false 200 0.3 0.99 0.1 2 false 25 203
#./runDir.sh $(pwd)/noTrack_200_3_X false 200 0.3 0.99 0.1 3 true 20 204
#./runDir.sh $(pwd)/noTrack_200_3_Y false 200 0.3 0.99 0.1 3 false 20 205

#./runDir.sh $(pwd)/track_200_1_X true 200 0.3 0.99 0.1 1 true 30 206
#./runDir.sh $(pwd)/track_200_1_Y true 200 0.3 0.99 0.1 1 false 30 207
#./runDir.sh $(pwd)/track_200_2_X true 200 0.3 0.99 0.1 2 true 25 208
#./runDir.sh $(pwd)/track_200_2_Y true 200 0.3 0.99 0.1 2 false 25 209
#./runDir.sh $(pwd)/track_200_3_X true 200 0.3 0.99 0.1 3 true 20 210
#./runDir.sh $(pwd)/track_200_3_Y true 200 0.3 0.99 0.1 3 false 20 211

#./runDir.sh $(pwd)/noTrack_20_1_X false 20 0.3 0.99 0.1 1 true 30 212
#./runDir.sh $(pwd)/noTrack_20_1_Y false 20 0.3 0.99 0.1 1 false 30 213
#./runDir.sh $(pwd)/noTrack_20_2_X false 20 0.3 0.99 0.1 2 true 25 214
#./runDir.sh $(pwd)/noTrack_20_2_Y false 20 0.3 0.99 0.1 2 false 25 215
#./runDir.sh $(pwd)/noTrack_20_3_X false 20 0.3 0.99 0.1 3 true 20 216
#./runDir.sh $(pwd)/noTrack_20_3_Y false 20 0.3 0.99 0.1 3 false 20 217

#./runDir.sh $(pwd)/track_20_1_X true 20 0.3 0.99 0.1 1 true 30 218
#./runDir.sh $(pwd)/track_20_1_Y true 20 0.3 0.99 0.1 1 false 30 219
#./runDir.sh $(pwd)/track_20_2_X true 20 0.3 0.99 0.1 2 true 25 220
#./runDir.sh $(pwd)/track_20_2_Y true 20 0.3 0.99 0.1 2 false 25 221
#./runDir.sh $(pwd)/track_20_3_X true 20 0.3 0.99 0.1 3 true 20 222
#./runDir.sh $(pwd)/track_20_3_Y true 20 0.3 0.99 0.1 3 false 20 223


#jobNumber=1

useTrackEstimate="false"
nPatternsPerUpdate=200
learningRate=0.3
learningRateDecrease=0.99
learningRateMomentum=0.1
nParticles=0
planeX=true
nBins=30


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

if (test ! -z $8); then
    planeX=$8
fi

if (test ! -z $9); then
    nBins=$9
fi

if ( test ! -z $10); then
    jobNumber=${10}
fi

echo "Directory is: " $directory
echo "UseTrackEstimate: "$useTrackEstimate
echo "nPatternsPerUpdate: "$nPatternsPerUpdate
echo "learningRate: "$learningRate
echo "learningRateDecrease: "$learningRateDecrease
echo "learningRateMomentum: "$learningRateMomentum
echo "nParticles: "$nParticles
echo "planeX: "$planeX
echo "nBins: "$nBins
echo "jobNumber: "$jobNumber


#jobNumber=$startFromNumber
#let lastJobNumber=$jobNumber+$numberJobsToProcess

echo "Creating directories if missing"
if $(test ! -e $directory);then mkdir -p $directory;fi


#mkdir $directory
cd $directory

echo "Linking necessary macros..."
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/libTJetNet.so 
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsOneTracks.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsTwoTracks.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsThreeTracks.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsOneTracks_noTrack.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsTwoTracks_noTrack.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/WeightsThreeTracks_noTrack.root
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/trainNN_cxx.so
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/errors/doTraining.C

    
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
echo "root -b -q doTraining.C\("$nParticles","$planeX","$nBins","$useTrackEstimate","$nPatternsPerUpdate","$learningRate","$learningRateDecrease","$learningRateMomentum"\)" >> $filename
#echo "echo Copying back files" >> $filename
#echo "rm *.so" >> $filename
#echo "cp -rv * "$actualdir"/" >> $filename
#echo "rm -r /tmp/giacinto/\$dirName" >> $filename
echo "echo \"Finished\" " >>$filename
    
chmod a+x $filename
    

echo "Job name: NN-"$jobNumber

bsub -J $(echo "NN-"$jobNumber) -q atlasb1 $filename
#bsub -J $(echo "NN-"$jobNumber) -q 1nw $filename

cd $directory


    
