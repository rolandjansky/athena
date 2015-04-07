#!/bin/bash

#The evgen dataset should have files with 5000 events each
#Make sure it's local using: dq2-list-dataset-site -n mc*EVNT* SLACXRD_DATADISK (or LOCALGROUPDISK ... )
#process="mu100";evgen="user09.AndrewHaas477621.${process}.EVGEN.test1"
#process="nu10";evgen="user09.AndrewHaas477621.${process}.EVGEN.v1";
#project="mc11";process="J4";evgen="${project}_7TeV.105013.${process}_pythia_jetjet.evgen.EVNT.e815";slash="/"
#HI evnt below
#project="mc10";process="J3";evgen="${project}_2TeV.105012.${process}_pythia_jetjet.evgen.EVNT.e733";slash="/"
#project="mc10";process="J4";evgen="${project}_2TeV.105013.${process}_pythia_jetjet.evgen.EVNT.e733";slash="/"
#project="mc10";process="Pythia_directJpsie3e3";evgen="${project}_2TeV.105751.${process}.evgen.EVNT.*";slash="/"
#project="mc10";process="PythiaPhotonJet_Unbinned35";evgen="${project}_2TeV.108081.${process}.evgen.EVNT.e890";slash="/"
project="mc10";process="PythiaPhotonJet_Unbinned35";evgen="${project}_2TeV.108081.${process}.evgen.EVNT.e890";slash="/"
#project="mc11";process="PythiaZeeJet_0Ptcut";evgen="${project}_2TeV.119104.${process}.evgen.EVNT.e1046";slash="/"

#The zerobias dataset should have 500 files with 100 RAW zerobias events each = 50000 events total
#run="180241";zerobias="user09.AndrewHaas477621.zerobiasDS.${run}.RAW.test3";
#stream="7";run="2011";zerobias="user.haastyle.BSmultipleselector.GRL.test9_EXT${stream}.merge.test8/";
#run="193211";zerobias="data11_hi.00${run}.physics_MinBiasOverlay.merge.RAW";
run="193825";zerobias="user.haastyle.HImerge.193825p.90308.test12/"; #for small tests
#run="193825";zerobias="user.haastyle.HImerge.${run}p.test12/"; #merged 193825,193826

#conditions
#https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolProdTags
dco="COMCOND-BLKPST-005-08"
mcco1="OFLCOND-SIM-BS7T-02"
mcco2="OFLCOND-DR-BS7T-ANom-11"
#geo="ATLAS-GEO-16-00-00"
geo="ATLAS-GEO-18-01-00"

#Can use the build job of a previous run
#lib="--libDS "

#Override vertex position, for heavy-ion
doG4vertexoverride=true

#The number of 100 event blocks to skip of evgen events in each job, usually goes from 0 to <50
for (( i=0; i<10; i+=1 )) ; do
inputevgenfile=$i
echo
echo "inputevgenfile is $inputevgenfile "

#The output dataset to create with the HITS and RDO files
#output="user.haastyle.overlaygrid. "
output="group.dataprep.overlaygrid.${process}.${inputevgenfile}.${run}_${stream}.geo18_MC12.test3"

#skipfiles is the number of input zerobias files to use (jobs to run) for each round of input evgen files
#can't be bigger than the number of evgen files in the input dataset, but should be 50 if possible
skipfiles=10; #50 if possible

#events to do
testr=$skipfiles; teste=100; # $skipfiles blocks of 100 events each (for inputfile 1-10..., so total of 50*100*10=50000 events, for skipfiles=50)
#testr=2; teste=2; let i=1000000 ; #to run just a small test

########################################

ffile="%IN"
filterstring="TriggerBit=240"
if [ "$doG4vertexoverride" == true ]; then 
  extraG4preinclude=",EventOverlayJobTransforms/vertex_override.py" ; 
  extfile="--extFile vtx.txt,filter.txt"
  filterstring="filterfile=filter.txt"

  mcco1="OFLCOND-SDR-BS7T-05-25-HI"
  mcco2="OFLCOND-SDR-BS7T-05-25-HI"

  #filter events
  ffile="simplefiltered.RAW"; DoFilter="BSFilter_trf.py inputBSFile=%IN OutputBSFilterFile=$ffile EventIdFile=events_orig.txt $filterstring skipEvents=%SKIPEVENTS ; "
fi

#The number of input evgen files for each zerobias file
#If this is 1, then you can do 50 blocks of 100 evgen events before running out (assuming 5000 events/evgenfile), 
# i.e. inputevgenfile could go up to <50, and you could do 250k events total.
#Basicallly, you should set this to (Nfiles in EVNT dataset / 50 ), assuming again 5000 events/evgenfile.
ninputmin=1

#for skipping the right number of input RAW files
let skipfiles*=inputevgenfile; 

#nfilesevgen=`python /afs/slac.stanford.edu/g/atlas/c/AtlasCore/16.6.7/Database/Bookkeeping/AMIClients/pyAMI/python/pyAMI.py ListDataset logicalDatasetName=$evgen project=$project processingStep=production | grep NFILES`

echo "Will use evgen dataset $evgen$slash $nfilesevgen and do up to $teste events per job"
echo "Will use $testr files of zerobias dataset $zerobias from run ${run}, skipping the first $skipfiles"
echo "Will create output dataset $output"
if [ -z "$lib" ]; then echo "Building library"; else echo "Using library $lib"; fi

let inputevgenfile*=100
let maxinputevgenfile=$ninputmin*5000-100
if [ "$process" == "nu10" ]; then let inputevgenfile=0; fi; #reuse the same neutrino events, since there's only 5k, and they're all the same
echo "Will skip $inputevgenfile (out of $maxinputevgenfile) evgen events in each job"
echo "extraG4preinclude is $extraG4preinclude and extfile is $extfile and filterstring is $filterstring "
echo

#AtlCoolConsole.py "COOLONL_LAR/COMP200;readoracle"
#listtags /LAR/ElecCalib/fSampl/Symmetry
fsamp="fSampltag=LARElecCalibMCfSampl-CSC02-D-"

#this messes up grid jobs... and fix the requirements file and remove the RTT garbage
rm -rf Event/EventOverlay/EventOverlayJobTransforms/test/
cp Event/EventOverlay/EventOverlayJobTransforms/cmt/grid.requirements Event/EventOverlay/EventOverlayJobTransforms/cmt/requirements

#verbose: preInclude="SimulationJobOptions/preInclude.VerboseVertexPositioner.py,"

DoEvents="BSFilter_trf.py inputBSFile=$ffile OutputBSFilterFile=simpledummy.RAW EventIdFile=events.txt TriggerBit=5 skipEvents=0 maxEvents=$teste"

MCcond="; AtlasG4_trf.py --omitvalidation=ALL inputEvgenFile=%MININ outputHitsFile=%OUT.MCcond.HITS.pool.root maxEvents=100 skipEvents=${inputevgenfile} randomSeed=%RNDM:${inputevgenfile} geometryVersion=$geo conditionsTag=$mcco1 physicsList=QGSP_BERT preInclude='EventOverlayJobTransforms/g4runnumber2.py${extraG4preinclude}' postInclude='EventOverlayJobTransforms/UseOracle.py' "
MCcond="${MCcond}; Digi_trf.py --omitvalidation=ALL inputHitsFile=%OUT.MCcond.HITS.pool.root outputRDOFile=%OUT.MCcond.RDO.pool.root maxEvents=$teste skipEvents=0 geometryVersion=$geo digiSeedOffset1=211 digiSeedOffset2=122 conditionsTag=$mcco2 samplingFractionDbTag=QGSP_BERT postInclude='EventOverlayJobTransforms/UseOracle.py' "

Datacond="; AtlasG4_trf.py --omitvalidation=ALL runNumber=$run inputEvgenFile=%MININ outputHitsFile=%OUT.HITS.pool.root maxEvents=100 skipEvents=${inputevgenfile} randomSeed=%RNDM:${inputevgenfile} geometryVersion=$geo conditionsTag=$dco physicsList=QGSP_BERT preInclude='EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/magfield.py,EventOverlayJobTransforms/g4runnumber.py${extraG4preinclude}' postInclude='EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/Rt_override.py,EventOverlayJobTransforms/muAlign.py' "
Datacond="${Datacond}; overlay_bs_trf.py --omitvalidation=ALL inputHitsFile=%OUT.HITS.pool.root outputRDOFile=%OUT.RDO.pool.root pileupBSFile=$ffile maxEvents=$teste skipEvents=0 geometryVersion=$geo digiSeedOffset1=211 digiSeedOffset2=122 conditionsTag=$dco jobConfig='[EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom.py]' samplingFractionDbTag=QGSP_BERT $fsamp postInclude='EventOverlayJobTransforms/Rt_override.py' "

MCdigidata="; Digi_trf.py --omitvalidation=ALL inputHitsFile=%OUT.HITS.pool.root outputRDOFile=%OUT.MCconddigi.RDO.pool.root maxEvents=$teste skipEvents=0 geometryVersion=$geo digiSeedOffset1=211 digiSeedOffset2=122 conditionsTag=$mcco2 samplingFractionDbTag=QGSP_BERT postInclude='EventOverlayJobTransforms/UseOracle.py' "

tmp6dir="dummy"
if [ -e /scratch/ahaas ]; then tmp6dir="/scratch/ahaas/"; fi
if [ -e /tmp/ahaas ]; then tmp6dir="/tmp/ahaas/"; fi

ddo="--dbRelease ddo.000001.Atlas.Ideal.DBRelease.v170601:DBRelease-17.6.1.tar.gz"

echo "pathena --trf \" $DoFilter $DoEvents $MCcond $Datacond $MCdigidata \" --individualOutDS --tmpDir $tmp6dir --inDS $zerobias --inputType RAW --minDS $evgen$slash --nMin $ninputmin --outDS $output --nFiles $testr --nSkipFiles $skipfiles --extOutFile \"events*.txt\" $ddo $extfile $lib --nFilesPerJob 1 --memory=3000 --skipScan --official \"--voms=atlas:/atlas/dataprep/Role=production\" " 
pathena --trf "$DoFilter $DoEvents $MCcond $Datacond $MCdigidata " --individualOutDS --tmpDir $tmp6dir --inDS $zerobias --inputType RAW --minDS $evgen$slash --nMin $ninputmin --outDS $output --nFiles $testr --nSkipFiles $skipfiles --extOutFile "events*.txt" $ddo $extfile $lib --nFilesPerJob 1 --memory=3000 --skipScan --official "--voms=atlas:/atlas/dataprep/Role=production" 
#--noSubmit
#--nEventsPerJob 100 --nEventsPerFile 100
#--excludedSite ANALY_SLAC 
#--extOutFile "athfile*.log.txt" 
#--site ANALY_SLAC

done
