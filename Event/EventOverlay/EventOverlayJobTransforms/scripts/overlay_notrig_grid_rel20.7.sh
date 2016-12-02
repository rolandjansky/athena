#how to send to grid:
#asetup 20.7.X.Y-VAL,here
#localSetupPandaClient
#source overlay_ch_grid_rel20.7.sh

#!/bin/bash

#test version
test="01"
#The evgen dataset should have files with 5000 events each
#Make sure it's local using: dq2-list-dataset-site -n mc*EVNT* SLACXRD_DATADISK (or LOCALGROUPDISK ... )
# mc15_13TeV.422011.ParticleGun_single_mu_Pt100.evgen.EVNT.e4459 dataset contains 10 files
project="mc15";process="ParticleGun_single_mu_Pt100";evgen="${project}_13TeV.422011.${process}.evgen.EVNT.e4459";slash="/"

#The zerobias dataset should have 500 files with 100 RAW zerobias events each = 50000 events total
run="2015";zerobias="mc15_valid.00200010.overlay_stream2_2015_3.skim.DRAW.r7677"

#conditions
dco="CONDBR2-BLKPA-2015-12"
geo="ATLAS-R2-2015-03-01-00"

#The number of 100 event blocks to skip of evgen events in each job, usually goes from 0 to <50
for (( i=0; i<50; i+=1 )) ; do
inputevgenfile=$i
echo
echo "inputevgenfile is $inputevgenfile "

#The output dataset to create with the HITS and RDO files
output="user.tkharlam.overlaygrid.${process}.${inputevgenfile}.${run}_triggerOff_LorentzID_test${test}"

#skipfiles is the number of input zerobias files to use (jobs to run) for each round of input evgen files
#can't be bigger than the number of evgen files in the input dataset, but should be 50 if possible
skipfiles=10; #500 if possible

#events to do
testr=$skipfiles; teste=100; # $skipfiles blocks of 100 events each (for inputfile 1-10..., so total of 50*100*10=50000 events, for skipfiles=50)
#testr=2; teste=2; let i=1000000 ; #to run just a small test

########################################

ffile="%IN"
filterstring="--triggerBit 240"

#The number of input evgen files for each zerobias file
#If this is 1, then you can do 50 blocks of 100 evgen events before running out (assuming 5000 events/evgenfile), 
# i.e. inputevgenfile could go up to <50, and you could do 250k events total.
#Basicallly, you should set this to (Nfiles in EVNT dataset / 50 ), assuming again 5000 events/evgenfile.
ninputmin=1

#for skipping the right number of input RAW files
let skipfiles*=inputevgenfile; 

echo "Will use evgen dataset $evgen$slash $nfilesevgen and do up to $teste events per job"
echo "Will use $testr files of zerobias dataset $zerobias from run ${run}, skipping the first $skipfiles"
echo "Will create output dataset $output"

let inputevgenfile*=100
let maxinputevgenfile=$ninputmin*1000-100
echo "Will skip $inputevgenfile (out of $maxinputevgenfile) evgen events in each job"
echo

fsamp="LARElecCalibMCfSampl-G496-19213-"

OverlayChain="OverlayChain_tf.py --inputZeroBiasBSFile $ffile --outputRDOFile %OUT.RDO.pool.root $filterstring  --skipEvents 0 --maxEvents $teste --triggerBit 240 --DataRunNumber $run --inputEVNTFile %MININ --randomSeed %RNDM:${inputevgenfile} --geometryVersion $geo --conditionsTag $dco --digiSeedOffset1 211 --digiSeedOffset2 122 --conditionsTag $dco --samplingFractionDbTag FTFP_BERT_BIRK --fSampltag $fsamp --preInclude 'sim:EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/magfield.py' 'overlayBS:EventOverlayJobTransforms/custom.py' --preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock(\"4samples1phase\")' --postInclude 'sim:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign.py,EventOverlayJobTransforms/g4runnumber.py' 'overlayBS:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' --ignorePatterns 'L1TopoMenuLoader.+ERROR.'"

Reco="; Reco_tf.py --autoConfiguration 'everything' --inputRDOFile %OUT.RDO.pool.root --outputESDFile %OUT.ESD.pool.root --outputAODFile %OUT.AOD.pool.root --maxEvents $teste --skipEvents 0 --preInclude 'EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py' --postInclude 'r2e:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign_reco.py' --preExec 'from LArConditionsCommon.LArCondFlags import larCondFlags;larCondFlags.OFCShapeFolder.set_Value_and_Lock(\"4samples1phase\");rec.doTrigger=False' --postExec 'r2e:from AthenaCommon import CfgGetter;CfgGetter.getPublicTool(\"LArCellBuilderFromLArRawChannelTool\").RawChannelsName=\"LArRawChannels_FromDigits\"' --ignorePatterns 'L1TopoMenuLoader.+ERROR.'"

tmp6dir="/tmp/tkharlam/"

pathena --trf "$OverlayChain $Reco" --individualOutDS --tmpDir $tmp6dir --inDS $zerobias --inputType RAW --minDS $evgen$slash --nMin $ninputmin --outDS $output --nFiles $testr --nSkipFiles $skipfiles $ddo --nFilesPerJob 1 --skipScan --memory=3000

#--noSubmit
#--nEventsPerJob 100 --nEventsPerFile 100
#--excludedSite ANALY_SLAC 
#--extOutFile "athfile*.log.txt" 
#--site ANALY_SLAC

done
