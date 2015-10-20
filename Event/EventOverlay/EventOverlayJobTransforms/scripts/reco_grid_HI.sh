#!/bin/bash

#uncomment this to run over just one file and do a few events, as a test run
#test="maxEvents=3"; test2="--nFiles 2 --nSkipFiles 0";

#uncomment this to make JiveXML files
#jive=",EventOverlayJobTransforms/dojive.py"; jive2="--extOutFile Jive*.xml";

#Library to use. Comment out to build new one
#lib="--libDS "

#conditions
#https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolProdTags
dco="COMCOND-BLKPST-005-08"
#dco="COMCOND-BLKPST-002-00"
#mcco1="OFLCOND-SIM-BS7T-02"
#mcco2="OFLCOND-DR-BS7T-ANom-11"
mcco1="OFLCOND-SDR-BS7T-05-25-HI"
mcco2="OFLCOND-SDR-BS7T-05-25-HI"
#geo="ATLAS-GEO-16-00-00"
geo="ATLAS-GEO-18-01-00"

trig="triggerConfig=MCRECO:MC_HI_v1_no_prescale"
#trig="triggerConfig=NONE"

#Loop over Jx samples
for i in 4; do echo "Loop value for Jx is $i"

#Data types to run
for inputchoice in MC MCdigi Data; do 
#MC MCdigi Data RAW ESDData

#inputstring1="group.dataprep.overlaygrid.J${i}"
inputstring1="group.dataprep.overlaygrid.PythiaPhotonJet_Unbinned35"
#inputstring1="group.dataprep.overlaygrid.PythiaZeeJet_0Ptcut"
#inputstring1="group.dataprep.overlaygrid.Pythia_directJpsie3e3"

#inputstring2="193825_.test34"
inputstring2="193825_.geo18_MC12.test2"

outputstring="test3"

echo "inputchoice is ${inputchoice}"
if [ "$inputchoice" == "MC" ]; then
 #this is for running on overlay RDO's with MC conditions
 inds="${inputstring1}.*.${inputstring2}_EXT2/"
 outds="${inputstring1}.${inputstring2}.mcreco.${outputstring}/"
 datamc="conditionsTag=$mcco2 preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom_MC.py,RecJobTransforms/HeavyIonDataRecConfig.py$jive "
 input="inputRDOFile=%IN"
 outputs="outputESDFile=%OUT.ESD.pool.root outputTAGFile=%OUT.TAG.root outputNTUP_HIFile=%OUT.NTUP_HI.root"
 inputf="inputRDOFile=/u/at/ahaas/nfs3/user.haastyle.overlaygrid.J4.0.193211.test16.111123071929_EXT2/user.haastyle.000291.EXT2._00004.MCcond.RDO.pool.root"
elif [ "$inputchoice" == "Data" ]; then
 #this is for running on overlay RDO's with data conditions (actual overlay)
 inds="${inputstring1}.*.${inputstring2}_EXT4/"
 outds="${inputstring1}.${inputstring2}.reco.${outputstring}/"
 datamc="conditionsTag=$dco preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py,RecJobTransforms/HeavyIonDataRecConfig.py$jive postInclude=EventOverlayJobTransforms/Rt_override.py,EventOverlayJobTransforms/muAlign_reco.py"
 input="inputRDOFile=%IN"
 outputs="outputESDFile=%OUT.ESD.pool.root outputTAGFile=%OUT.TAG.root outputNTUP_HIFile=%OUT.NTUP_HI.root"
 inputf="inputRDOFile=XXX"
elif [ "$inputchoice" == "MCdigi" ]; then
 #this is for running on overlay RDO's made with MC conditions from data HITS
 inds="${inputstring1}.*.${inputstring2}_EXT5/"
 outds="${inputstring1}.${inputstring2}.mcdigireco.${outputstring}/"
 datamc="conditionsTag=$mcco2 preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom_MC.py,RecJobTransforms/HeavyIonDataRecConfig.py$jive "
 input="inputRDOFile=%IN"
 outputs="outputESDFile=%OUT.ESD.pool.root outputTAGFile=%OUT.TAG.root outputNTUP_HIFile=%OUT.NTUP_HI.root"
 inputf="inputRDOFile=XXX"
elif [ "$inputchoice" == "ESDData" ]; then
 #this is for re-running TAG and NTUP_HI on overlay ESD's with data conditions (actual overlay)
 inds="group.dataprep.overlaygrid.Pythia_directJpsie3e3.193825.test34.reco.test1_EXT0/"
 outds="group.dataprep.overlaygrid.Pythia_directJpsie3e3.193825.test34.recoESD.${outputstring}/"
 datamc="conditionsTag=$dco preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py,RecJobTransforms/HeavyIonDataRecConfig.py$jive postInclude=EventOverlayJobTransforms/Rt_override.py,EventOverlayJobTransforms/muAlign_reco.py"
 input="inputESDFile=%IN"
 outputs="outputTAGFile=%OUT.TAG.root outputNTUP_HIFile=%OUT.NTUP_HI.root"
 inputf="inputESDFile=/u/at/ahaas/nfs3/group.dataprep.overlaygrid.Pythia_directJpsie3e3.193825.test34.reco.test1.120120122022_EXT0/group.dataprep.57068_002072.EXT0._00118.ESD.pool.root"
elif [ "$inputchoice" == "RAW" ]; then
 #this is for running on RAW data
 inds="user.haastyle.HImerge.193825p.test12/"
 outds="user.haastyle.HImerge.193825p.test12.reco.${outputstring}"
 #inds="data11_hi.00193211.physics_MinBiasOverlay.merge.RAW"
 #outds="user.haastyle.193211.MinBiasOverlay.reco.test29"
 datamc="conditionsTag=$dco preInclude=RecJobTransforms/HeavyIonDataRecConfig.py$jive"
 input="inputBSFile=%IN"; input2="--inputType RAW";
 outputs="outputESDFile=%OUT.ESD.pool.root outputTAGFile=%OUT.TAG.root outputNTUP_HIFile=%OUT.NTUP_HI.root"
 inputf="inputBSFile=/u/at/ahaas/nfs3/data11_hi.00193211.physics_MinBiasOverlay.merge.RAW/data11_hi.00193211.physics_MinBiasOverlay.merge.RAW._lb0820._SFO-ALL._0001.1"
else echo "dont know inputchoice $inputchoice"; exit
fi

beamconstraint="InDetFlags.useBeamConstraint.set_Value_and_Lock(True);"
dozdc="jobproperties.HeavyIonD3PDMakerFlags.DoZDC.set_Value_and_Lock(False);"
condition="geometryVersion=$geo $datamc preExec='from HIJetRec.HIJetRecFlags import jobproperties; jobproperties.HIJetRecFlags.doHIJetMergeEvent=True; $beamconstraint InDetFlags.cutLevel=2; from HeavyIonD3PDMaker.HeavyIonD3PDMakerFlags import jobproperties; jobproperties.HeavyIonD3PDMakerFlags.DoHijingPars.set_Value_and_Lock(False); $dozdc jobproperties.HeavyIonD3PDMakerFlags.DoTruth.set_Value_and_Lock(True); ' $trig preInclude_e2d=HeavyIonD3PDMaker/HeavyIonD3PDMaker_PbPb2011_conf.py "

local="false"
if [ "$local" == "true" ]; then

 tempoodir="/scratch/ahaas/run21/"
 mkdir -p $tempoodir
 cp atlas_error_ignore.db $tempoodir
 cd $tempoodir ; rm *.root ; rm JiveXML* ; pwd ; 
 command="Reco_trf.py $inputf $condition outputESDFile=OUT.ESD.pool.root outputTAGFile=OUT.TAG.root outputNTUP_HIFile=OUT.NTUP_HI.root $test >& log.txt &" ;
 echo $command ; eval $command ; 

else

 rm -rf Event/EventOverlay/EventOverlayJobTransforms/test/
 cp Event/EventOverlay/EventOverlayJobTransforms/cmt/grid.requirements Event/EventOverlay/EventOverlayJobTransforms/cmt/requirements

 tmp6dir="dummy"
 if [ -e /scratch ]; then mkdir /scratch/ahaas; tmp6dir="/scratch/ahaas/"; fi
 if [ -e /tmp ]; then mkdir /tmp/ahaas; tmp6dir="/tmp/ahaas/"; fi

 ddo="--dbRelease ddo.000001.Atlas.Ideal.DBRelease.v170601:DBRelease-17.6.1.tar.gz"

 echo "pathena --trf \"Reco_trf.py $input $condition $outputs $test \" --inDS $inds --outDS $outds --individualOutDS --tmpDir $tmp6dir --nFilesPerJob 1 $test2 $jive2 --extFile atlas_error_ignore.db $lib $input2 $ddo --official "--voms=atlas:/atlas/dataprep/Role=production" "

 pathena --trf "Reco_trf.py $input $condition $outputs $test " --inDS $inds --outDS $outds --individualOutDS --tmpDir $tmp6dir --nFilesPerJob 1 $test2 $jive2 --extFile atlas_error_ignore.db $lib $input2 $ddo --official "--voms=atlas:/atlas/dataprep/Role=production" 
 #--memory=10000 --cmtConfig x86_64-slc5-gcc43-opt
 #--site ANALY_SLAC
 #--excludedSite ANALY_SLAC
 #--noSubmit
 echo
 echo "--------------------------------------------------------------"

fi

done
done; #loop over "i"

