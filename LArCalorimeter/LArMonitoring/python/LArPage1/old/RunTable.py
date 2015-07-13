#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import sys,os,getopt,fileinput
import subprocess as sp
import test
import xmlrpclib
global server
server = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch') 
from threading import Thread
from time import *
from threading import Thread
import os
#import GetRunInfo
#from GetRunInfo import *

########################################################################
f = cgi.FieldStorage()


########################################################################
def GetRunNumber():
    #f = cgi.FieldStorage()
    run = 0
    for i in f.keys(): run = f[i].value

    return run

########################################################################
def ReturnBasicRunInfo(run):
    cmd="export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;"
    cmd+="source $AtlasSetup/scripts/asetup.sh 16.6.5,32,here,builds;"
    cmd+="python /afs/cern.ch/user/l/larmon/public/LArPage1/scripts/ReturnBasicRunInfo.py "+run
    output_sp = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    output =  output_sp.communicate()[0]
    output = output.replace("['ATLF', 'ATLAS_COOLPROD', 'atlas_dd']","")
    output = output.replace("__Warning: use COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP__","")
    output = output.replace("Data source lookup using /afs/cern.ch/atlas/software/builds/AtlasCore/16.6.5/InstallArea/XML/AtlasAuthentication/dblookup.xml","")
    output = output.replace('''file CORAL/Services/XMLAuthenticationService Error Did not find authentication.xml in /afs/cern.ch/atlas/software/builds/AtlasCore/16.6.5/InstallArea/XML/AtlasAuthentication''',"")
    output = output.replace('''The problem was: boost::filesystem::exists: Permission denied: "/afs/cern.ch/atlas/software/builds/AtlasCore/16.6.5/InstallArea/XML/AtlasAuthentication/authentication.xml''',"")
    output = output.replace('''Error Did not find authentication.xml in /afs/cern.ch/atlas/software/builds/AtlasCore/16.6.5/InstallArea/XML/AtlasAuthentication''',"")
    output = output.replace('''Data source lookup using /afs/cern.ch/atlas/software/builds/AtlasCore/16.6.5/InstallArea/XML/AtlasAuthentication/dblookup.xml''',"")
    output = output.replace('''file CORAL/Services/XMLAuthenticationService''',"")
    output = output.replace('''CORAL/Services/XMLAuthenticationService Error Could not open "authentication.xml" for reading CORAL/Services/XMLAuthenticationService''',"")
    output = output.replace('''file CORAL/Services/XMLAuthenticationService''',"")
    output = output.replace('''CORAL/Services/XMLAuthenticationService''',"")
    output = output.replace('''Error Could not open "authentication.xml"''',"")
    output = output.replace('''for reading''',"")
    output = output.replace('''file''',"")
    output = output.replace('''"''',"")

    return str(output)

##########################################################################
def ParseBasicRunInfo(run):
     infos={}
     tmp = ReturnBasicRunInfo(run)

     #print tmp

     separators={}
     separators[0]=",SIGNOFF:"
     separators[1]=",BULK:"
     separators[2]=",ES:"
     separators[3]=", Defects:UP:"
     separators[4]=", LCE:"
     separators[5]=", BULKCleaning:"
     separators[6]=", ESCleaning:"
     separators[7]=", TotalEvents:"
     separators[8]=", BULKProcEvents:"
     separators[9]=", ESProcEvents:"
     separators[10]=", IntegratedLumi:"
     separators[11]=", EndCalibLoop:"
     separators[12]="ListOfCells:"

     dictext={}
     dictext[0]="SIGNOFF"
     dictext[1]="BULK"
     dictext[2]="ES"
     dictext[3]="DefectsUP"
     dictext[4]="LCE"
     dictext[5]="BULKCleaning"
     dictext[6]="ESCleaning"
     dictext[7]="TotalEvents"
     dictext[8]="BULKProcEvents"
     dictext[9]="ESProcEvents"
     dictext[10]="IntegratedLumi"
     dictext[11]="EndCalibLoop"
     dictext[12]="ListOfCells"

     for i in range(0,len(separators)):
         tmp2 = tmp.partition(separators[i])
         tmpstring = dictext[i]
         infos[tmpstring]=tmp2[2].replace("\n","")
         tmp = tmp2[0]
         
     #print infos
     
     return infos

run = str(GetRunNumber())
table = ParseBasicRunInfo(run)
text_for_next_steps = ""


#tests
## table["TotalEvents"]=5
## table["ESProcEvents"]=table["TotalEvents"]
## table["DefectsUP"]=str(1)
## table["ESCleaning"]="SUCCESS"
## table["LCE"]="SUCCESS"
## table["ES"]=str(1)
## table["BULKProcEvents"]=table["TotalEvents"]
## table["BULKCleaning"]="SUCCESS"
## table["BULK"]=str(1)
## table["SIGNOFF"]=str(1)
#end tests

if table["DefectsUP"]==str(0) and (table["ESProcEvents"]!=table["TotalEvents"] or table["ESProcEvents"]=="0"):
    text_for_next_steps = '''Wait for Express Processing to finish'''

if table["DefectsUP"]==str(0) and table["ESProcEvents"]==table["TotalEvents"] and table["ESProcEvents"]!="0":
    text_for_next_steps = '''<a href="https://atlas-larmon.cern.ch/WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=tier0&proc_ver=1&shifter=shifter&info=0&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check DCS errors, HV Trips, Noise Bursts, Data Corruption and upload the defects </a>(<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#EXPRESS_PROCESSING_run_report"><abbr>?</abbr></a>)'''

if table["DefectsUP"]==str(1) and (table["ESCleaning"]!="SUCCESS" or table["LCE"]!="SUCCESS" or "YES" not in table["ListOfCells"]):
    text_for_next_steps = '''Wait for ES CAF Cleaning / LArCellEmpy to finish'''

if table["DefectsUP"]==str(1) and table["ESCleaning"]=="SUCCESS" and table["LCE"]=="SUCCESS" and table["ES"]!=str(1):
    text_for_next_steps = '''<a href="https://atlas-larmon.cern.ch/WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=larcomm&proc_ver=101&shifter=shifter&info=1&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check Noisy LAr Cells for UPD4</a> (<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#EXPRESS_PROCESSING_noisy_LAr_cel" target="_blank"><abbr>?</abbr></a>)<br />after expert\'s reply, set LAR_UNCHECKED defect absent in DB'''
    
if table["ES"]==str(1) and (table["BULKProcEvents"]!=table["TotalEvents"] or table["BULKProcEvents"]=="0"):
    text_for_next_steps = '''Wait for BULK Processing to finish'''

if table["ES"]==str(1) and table["BULKProcEvents"]==table["TotalEvents"] and table["BULKProcEvents"]!="0" and table["BULKCleaning"]!="SUCCESS":
    text_for_next_steps = '''Wait for BULK CAF Cleaning to finish'''

if table["ES"]==str(1) and table["BULKCleaning"]=="SUCCESS":
    text_for_next_steps = '''<a href="https://atlas-larmon.cern.ch/WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=larcomm&proc_ver=102&shifter=shifter&info=1&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check Noisy LAr Cells for UPD3</a> (<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#BULK_checklist" target="_blank"><abbr>?</abbr></a>)<br />check HV NON NOMINAL Corrections<br />after expert\'s reply, set LAR_BULK_UNCHECKED defect absent in DB'''

if table["ES"]==str(1) and table["BULK"]==str(1):
    text_for_next_steps = "Do final signoff (<a href='https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#Data_period_signoff' target='_blank'><abbr>?</abbr></a>)<br />When finished, set LAR_UNCHECKED_FINAL defect absent in DB"

if table["SIGNOFF"]==str(1):
    text_for_next_steps = "<font color='green'>Run signed off: nothing else to do</font>"
 
img_ES_proc = "../icons/"
if table["ESProcEvents"]==table["TotalEvents"] and table["ESProcEvents"]!="0": img_ES_proc += "tick.png"
else: img_ES_proc += "cross.png"
purcentage_ES_proc = 0
if float(table["TotalEvents"])!=0: purcentage_ES_proc = float(table["ESProcEvents"])/float(table["TotalEvents"])*100.
else: purcentage_ES_proc = 0

img_BULK_proc = "../icons/"
if table["BULKProcEvents"]==table["TotalEvents"] and table["BULKProcEvents"]!="0": img_BULK_proc += "tick.png"
else: img_BULK_proc += "cross.png"
purcentage_BULK_proc = 0
if float(table["TotalEvents"])!=0: purcentage_BULK_proc = float(table["BULKProcEvents"])/float(table["TotalEvents"])*100.
else: purcentage_BULK_proc = 0

img_defects_up = "../icons/"
if table["DefectsUP"]==str(1): img_defects_up += "tick.png"
else: img_defects_up += "cross.png"

img_SIGNOFF = "../icons/"
if table["SIGNOFF"]==str(1): img_SIGNOFF += "tick.png"
else: img_SIGNOFF += "cross.png"

img_LAr_Unchecked = "../icons/"
if table["ES"]==str(1): img_LAr_Unchecked += "tick.png"
else: img_LAr_Unchecked += "cross.png"

img_LAr_BULK_Unchecked = "../icons/"
if table["BULK"]==str(1): img_LAr_BULK_Unchecked += "tick.png"
else: img_LAr_BULK_Unchecked += "cross.png"

img_CAF_ES_cleaning = "../icons/"
color_ES_cleaning = "red"
if table["ESCleaning"]=="SUCCESS":
    img_CAF_ES_cleaning += "tick.png"
    color_ES_cleaning = "green"
else: img_CAF_ES_cleaning += "cross.png"
if "NOT LAUNCHED YET" in table["ESCleaning"]: color_ES_cleaning = "orange"
text_ES_cleaning = table["ESCleaning"]

img_CAF_BULK_cleaning = "../icons/"
color_BULK_cleaning = "red"
if table["BULKCleaning"]=="SUCCESS":
    img_CAF_BULK_cleaning += "tick.png"
    color_BULK_cleaning = "green"
else: img_CAF_BULK_cleaning += "cross.png"
if "NOT LAUNCHED YET" in table["BULKCleaning"]: color_BULK_cleaning = "orange"
text_BULK_cleaning = table["BULKCleaning"]

img_CAF_LCE = "../icons/"
color_LCE = "red"
if table["LCE"]=="SUCCESS" and "YES" in table["ListOfCells"]:
    img_CAF_LCE += "tick.png"
    color_LCE = "green"
else: img_CAF_LCE += "cross.png"
if "NOT LAUNCHED YET" in table["LCE"]: color_LCE = "orange"
text_LCE = table["LCE"]
if "YES" in table["ListOfCells"]: text_LCE += " and list produced"
else: text_LCE += " and list NOT produced"

## img_CAF_BULK_cleaning = "../icons/"
## if table["BULKCleaning"]=="SUCCESS": img_CAF_BULK_cleaning += "tick.png"
## else: img_CAF_BULK_cleaning += "cross.png"
## text_BULK_cleaning = table["BULKCleaning"]

## img_CAF_LCE = "../icons/"
## if table["LCE"]=="SUCCESS": img_CAF_LCE += "tick.png"
## else: img_CAF_LCE += "cross.png"
## text_LCE = table["LCE"]

text_for_defects = "Defects "
if table["DefectsUP"]==str(1): text_for_defects+="uploaded"
else: text_for_defects+="NOT uploaded"

print '''
<html>
<head>
<meta name="author" content="Olivier Davignon [davignon@cern.ch]"/>
<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />
<script type="text/javascript" src="scripts.js"></script>

<script type="text/javascript">
function hide_loading(){
if(Obj3.value=="hidden"){
Obj3=document.getElementById("loading")
Obj3.style.display='none'
Obj3.style.visibility='hidden'
Obj3.value='hidden'
}
Obj3=document.getElementById("loading")
Obj3.style.display='none'
Obj3.style.visibility='hidden'
Obj3.value='hidden'
}
</script>

<script language="JavaScript" type="text/javascript">
function makeFrame(run) {
          Obj2=document.getElementById("test")
          var iframe;
          if (document.all){
          iframe = document.createElement('<iframe onload="hide_loading()"></iframe>');
          }
          else if (document.getElementById) {
          iframe = document.createElement('iframe');
          iframe.onload=function(){hide_loading()};
          }
          if (!iframe) return;
          iframe.frameBorder = 0;
          iframe.width = 380;
          iframe.height = 200;
          iframe.src = "https://atlas-larmon.cern.ch/LArPage1/scripts/iframe2.py?run="+run
          iframe.setAttribute ("id", "testframe");
          if(Obj2.value!='already') document.getElementById ("test").appendChild(iframe);
          Obj2.value='already'
  }
   
</script> 
<script type="text/javascript">
function uncheck(){
var allInputs = document.getElementsByTagName('input');
var input, i = allInputs.length;
while (i--){
input = allInputs[i];
if ('checkbox' == input.type){
input.checked = false;
}
}
}
</script>

<script type="text/javascript">
function toggletext(){
Obj=document.getElementById("test")
Obj2=document.getElementById("hide")
var test2 = 0
Obj3=document.getElementById("loading")
if(Obj2.value=='none'){
     if(Obj3.value=='hidden') hide_loading()
     Obj.style.display='block'
     Obj.style.visibility='visible'
     if(Obj3.value!='hidden') Obj3.style.display='block'
     if(Obj3.value!='hidden') Obj3.style.visibility='visible'
     Obj2.value='block'
     test2 = 1
}
if(Obj2.value=='block' && test2==0){
     Obj.style.display='none'
     Obj.style.visibility='hidden'
     Obj3.style.display='none'
     Obj3.style.visibility='hidden'
     Obj2.value='none'
}
}
</script>

<script type="text/javascript">
function test(){
        document.write("Hello world!");
        }
 </script>
 
<base target="_blank">

</head>
<body onload="uncheck()">'''
#print table
print '''<TABLE BORDER="1" cellspacing="0" cellpadding="0" width=99%>
<TR>
  <TH align="center"><img src="../icons/update_button.png" ALT="update run" TITLE="update run" onClick="window.location.reload()" height=25 width=25>&nbsp;<br /></TD>
  <TH align="center" colspan=2>Tier 0 Processing</TD>
  <TH align="center" colspan=2>CAF jobs</TD>
  <TH align="center">LAr unchecked defects</TD>
  <TH align="center" width="300">Next steps</TD>
</TR>
<TR>
  <TD align="center" rowspan=3><b><a href="https://atlas-larmon.cern.ch/WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=tier0&proc_ver=1&shifter=shifter&info=0&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">'''+str(run).replace(" ","")+'''</a></b>
  <!--<input type='checkbox' onclick="makeFrame('''+str(run).replace(" ","")+''');toggletext()" name='show_cb' value='none' id="hide" /><label for='hide'>(more...)</label><br />-->

    <br />'''+text_for_defects+'''<img src="'''+img_defects_up+'''" height=15 width=15><br />
    <b>End of calib. loop</b><br />'''+str(table["EndCalibLoop"])+'''<br />
    <b>Approx. lumi</b>: '''+str(table["IntegratedLumi"])+''' pb-1<br />
    <a href="https://atlas-larmon.cern.ch/HVTripsDB/index.py?query=select%20+trips.rowid,*+from+trips+INNER+JOIN+details+ON+trips.HVLINE+=+details.HVLINE+where+trips.TripRunNumber='''+str(run).replace(" ","")+'''+order+by%20trips.TripTimeStamp+desc" target="_blank">HV DB</a>, <a href="https://atlasdqm.cern.ch/defectentry/?run='''+str(run).replace(" ","")+'''&filter=LAr" target="_blank">Defect DB</a>, <a href="https://atlas-larmon.cern.ch/LArTimeLine/?run='''+str(run).replace(" ","")+'''" target="_blank">Timeline</a>, <a href="https://atlasop.cern.ch/elog/ATLAS/ATLAS/?mode=summary&reverse=0&reverse=1&npp=20&last=31&Message+Type=Data+Quality&Subject='''+str(run).replace(" ","")+'''&sall=0" target="_blank">DQ Elogs</a>, <a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines" target="_blank">Guidelines</a>, <a href="https://atlasdqm.cern.ch/webdisplay/tier0" target="_blank">Tier 0
  </TD>
  <TD rowspan=2 align="center"><b>EXPRESS</b><br />
    
  </TD>'''

print '''<TD align="center" width="80" rowspan=2><img src="'''+img_ES_proc+'''" height=25 width=25><br />
    <TABLE cellspacing="0" cellpadding="0" border=0 bgcolor=white width=100% height="10">
      <TR>
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_ES_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_ES_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
  </TD>
  <TD align="center">Cleaning:<br /><font color="'''+color_ES_cleaning+'''">'''+text_ES_cleaning+'''</font></TD>
  <TD align="center" width=60><img src="'''+img_CAF_ES_cleaning+'''" height=25 width=25></TD>
  <TD rowspan=2 align="center"><img src="'''+img_LAr_Unchecked+'''" height=25 width=25></TD>
  <TD valign="center" align="center" rowspan="3">
  <font color="red">'''+text_for_next_steps+'''</font>
    <!--<ol>
      <!--<li><b>ES Checklist:</b><br /> Run Report<br /><br /></li>-->
      <!--<b>Check CAF jobs</b><br /><br />-->
      <!--<li><b>ES Checklist:</b><br /> Noisy LAr Cells<br /><br /></li>-->
      <!--<li><b>BULK Checklist:</b><br /> HV NON NOMINAL<br /><br /></li>-->
      <!--<li><b>BULK Checklist:</b><br /> Noisy LAr Cells</li>-->
    <!--</ol> -->
    <br /><br />Final Signoff <img src="'''+img_SIGNOFF+'''" height=15 width=15>
  </TD>
</TR>

<TR>
  <TD align="center">LArCellEmpy:<br /><font color="'''+color_LCE+'''">'''+text_LCE+'''</font></TD>
  <TD align="center"><img src="'''+img_CAF_LCE+'''" height=25 width=25></TD>
</TR>

<TR>
  <TD align="center"><b>BULK</b></TD>
  <TD align="center"><img src="'''+img_BULK_proc+'''" height=25 width=25>
    <TABLE cellspacing="0" cellpadding="0" border=0 bgcolor=white width=100% height="10">
      <TR>
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_BULK_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_BULK_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
  </TD>
  <TD width="150" align="center">Cleaning:<br /><font color="'''+color_BULK_cleaning+'''">'''+text_BULK_cleaning+'''</font></TD>
  <TD align="center"><img src="'''+img_CAF_BULK_cleaning+'''" height=25 width=25></TD>
  <TD align="center"><img src="'''+img_LAr_BULK_Unchecked+'''" height=25 width=25></TD>
</TR>
<TR>
  <TD colspan="7">
    <div id="test" style="visibility:hidden;display:none" value="not">
      <div id="loading" style="" value="hidden"><b><blink>Loading information from database...</blink></b></div>
    </div>
  </TD>
</TR>

</TABLE>

</body>
</html>'''
