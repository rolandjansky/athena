#/bin/bash


#./runDir.sh $(pwd)/track_200 true 200 0.3 0.99 0.1 2

#./runDir.sh $(pwd)/noTrack_20 false 20 0.03 0.95 0.03 3
#./runDir.sh $(pwd)/noTrack_200 false 200 0.3 0.99 0.1 1
#./runDir.sh $(pwd)/track_20 true 20 0.03 0.95 0.03 4



#./runDir.sh $(pwd)/track_200_useeta true 200 0.3 0.99 0.1 100
#./runDir.sh $(pwd)/track_20_useeta true 20 0.03 0.95 0.03 101



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
    jobNumber=$7
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
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/number/trainNN_cxx.so
ln -s $TestArea/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/number/doTraining.C
   
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
echo "root -b -q doTraining.C\("$useTrackEstimate","$nPatternsPerUpdate","$learningRate","$learningRateDecrease","$learningRateMomentum"\)" >> $filename
#echo "echo Copying back files" >> $filename
#echo "rm *.so" >> $filename
#echo "cp -rv * "$actualdir"/" >> $filename
#echo "rm -r /tmp/giacinto/\$dirName" >> $filename
echo "echo \"Finished\" " >>$filename
    
chmod a+x $filename
    
bsub -J $(echo "NN-"$jobNumber) -q atlasb1 $filename
#bsub -J $(echo "NN-"$jobNumber) -q 1nw $filename

cd $directory


    
