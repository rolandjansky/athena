#!/bin/bash

#uncomment this to run over just one file and do a few events, as a test run
#test="maxEvents=3"; test2="--nFiles 1";

#uncomment this to make JiveXML files
#jive=",EventOverlayJobTransforms/dojive.py"; jive2="--extOutFile Jive*.xml";

#conditions
#https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolProdTags
dco="COMCOND-BLKPST-005-08"
#dco="COMCOND-BLKPST-002-00"
mcco1="OFLCOND-SIM-BS7T-02"
mcco2="OFLCOND-DR-BS7T-ANom-11"
#mcco1="OFLCOND-SDR-BS7T-05-HI"
#mcco2="OFLCOND-SDR-BS7T-05-HI"
#geo="ATLAS-GEO-18-01-00"
geo="ATLAS-GEO-16-00-00"

#trig="triggerConfig=NONE"

#intype="raw"
#intype="mcrdo"
intype="overlayrdo"
if [ $intype == "raw" ]; then
 #this is for running on RAW data
 inds="user.haastyle.BSmultipleselector.GRL.test9_EXT7.merge.test8/"
 outds="user.haastyle.BSmultipleselector.GRL.test9_EXT7.merge.test8.reco.test1"
 datamc="conditionsTag=$dco preInclude=EventOverlayJobTransforms/UseOracle.py$jive"
 input="inputBSFile=%IN"; input2="--inputType RAW";
elif [ $intype == "mcrdo" ]; then
 #this is for running on overlay RDO's with MC conditions
 inds="user.haastyle.overlaygrid.J4.*.193211.test20_EXT2/"
 outds="user.haastyle.overlaygrid.J4.193211.test20.mcreco.test1"
 datamc="conditionsTag=$mcco2 preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom_MC.py$jive"
 input="inputRDOFile=%IN";
elif [ $intype == "overlayrdo" ]; then
 #this is for running on overlay RDO's with data conditions (actual overlay)
 inds="user.haastyle.overlaygrid.J4.*.2011.stream7.test32_EXT2/"
 outds="user.haastyle.overlaygrid.J4.2011.stream7.test32.reco.test1"
 datamc="conditionsTag=$dco preInclude=EventOverlayJobTransforms/UseOracle.py,EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py$jive postInclude=EventOverlayJobTransforms/Rt_override.py,EventOverlayJobTransforms/muAlign_reco.py"
 input="inputRDOFile=%IN";
fi

condition="geometryVersion=$geo $datamc $trig"

#don't ask
rm -rf Event/EventOverlay/EventOverlayJobTransforms/test/
if [ -e LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml ]; then echo "Have LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml"; else get_files LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml ; fi
ln -fs Event/EventOverlay/EventOverlayJobTransforms/scripts/atlas_error_ignore.db

#lib="--libDS user.haastyle.0105143637.423039.lib._001537"

echo "pathena --trf \"Reco_trf.py $input $condition outputESDFile=%OUT.ESD.pool.root outputCBNTFile=%OUT.CBNT.root $test \" --inDS $inds --outDS $outds --individualOutDS --nFilesPerJob 1 $test2 $jive2 --extFile LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml,atlas_error_ignore.db --memory=3000 $lib $input2 "
pathena --trf "Reco_trf.py $input $condition outputESDFile=%OUT.ESD.pool.root outputCBNTFile=%OUT.CBNT.root $test " --inDS $inds --outDS $outds --individualOutDS --tmpDir /scratch/ahaas/ --nFilesPerJob 1 $test2 $jive2 --extFile LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml,atlas_error_ignore.db --memory=3000 $lib $input2 
#--memory=10000 --cmtConfig x86_64-slc5-gcc43-opt 
#--site ANALY_SLAC
#--excludedSite ANALY_SLAC
#--noSubmit

