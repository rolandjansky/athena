#!/bin/bash
# A.Korn 2011
# simple script to run RTT plots standalone

#name of output summary page
summaryHtml=datasetsRTT.html

#set list of datasetdirs
#last dir is expected to be datasetname, no / at the end!
myDirs=(/afs/cern.ch/user/a/akorn/www/RTT/RTEST/group.det-slhc.mc11_14TeV.0107231.multi01_mu50.ESD.D3PD.NTUP.ibl02.test2.110722161856
)

#reference for comparison
myref=/afs/cern.ch/user/a/akorn/www/RTT/RTEST/group.det-slhc.mc11_14TeV.0107231.multi01_mu50.ESD.D3PD.NTUP.ibl02.test2.110722161856/group.det-slhc.26809_000709.EXT0._00001.InDetStandardPlots.root

echo "<html><head><Title>" >$summaryHtml
echo "Upgrade RTT test " >>$summaryHtml
echo "</Title></head><body>" >>$summaryHtml

for myDir in ${myDirs[@]}; do


  #create subdir with datasetname
  datasetDir=`echo $myDir|awk -F/ '{print $NF}'`
  echo "DatasetDir: "$datasetDir
  cp  ALL_IDPerformanceRTT_Plots.html $datasetDir

  #look InDetStandardPlots.root files
  myfiles=`ls $myDir|grep root|grep InDetStandardPlots`

  # can't merge histofiles easily together, if more than one, take first 
  myfile=${myfiles[0]}
  echo "processing: " $myDir " / " $myfile
  cd $datasetDir
  #root -b -q ../makeplots_fix.C\(\"/afs/cern.ch/user/a/akorn/www/RTT/RTEST/group.det-slhc.mc11_14TeV.0107231.multi01_mu50.ESD.D3PD.NTUP.ibl02.test2.110722161856/group.det-slhc.26809_000709.EXT0._00001.InDetStandardPlots.root\"\)
  root -b -q ../makeplots_fix.C\(\"$myfile\"\)

  #needs to start with DCube!!
  mkdir DCube-AK_DCubeConfig

  #run DCube for comparison with reference
  dcube.py -c ../AK_DCubeConfig.xml -s ../../dcubesrv/ -x DCube-AK_DCubeConfig/InDetStandardPlots.root.dcube.xml -p -r $myref $myDir/$myfile

  #link everythign together
  cp ../IDPerformanceRTT_dynamic_pagemaker.py .
  ./IDPerformanceRTT_dynamic_pagemaker.py
  cd ..
 
  echo "<LI><A HREF="$datasetDir"/ALL_IDPerformanceRTT_Plots_Dynamic.html>"$datasetDir"</A></LI>" >>$summaryHtml 

done;

echo "</body></html>">>$summaryHtml
