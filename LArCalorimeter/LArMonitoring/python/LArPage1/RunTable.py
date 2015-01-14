#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import GetNumberFromDB as DB
import getopt,fileinput
import xmlrpclib
#config reading for default url
import ConfigParser 
config = ConfigParser.ConfigParser()
config.read('config.ini')
baseurl = config.get('url', 'base')
iconpath = config.get('url', 'iconpath')

########################################################################
f = cgi.FieldStorage()
run = f["run"].value
#run=str(248371)
text_for_next_steps = ""


table= DB.GetRunInfoFromDQM(run)
table.update(DB.GetRunInfoFromOracle(run))
table.update(DB.GetCAFInfo(run))



# table["TEMPEXPRESS"]=="notmp" must be
ESProcExpressDone = table["ESPROCEXPRESS"]!="0" and table["ESPROCEXPRESS"]!="" and (table["ESPROCEXPRESS"]==table["expressTot"] or (float(table["ESPROCEXPRESS"])>=float(table["expressTot"])*98.6/100. and table["TEMPEXPRESS"]=="notmp")) 


if table["DEFECTS"]=="NOTDONE" and not ESProcExpressDone:
    text_for_next_steps = '''Wait for Express Processing to finish'''

if table["ESPROCEXPRESS"]!="" and table["DEFECTS"]=="NOTDONE" and ESProcExpressDone:
    text_for_next_steps = '''<a href="'''+baseurl+'''WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=tier0&proc_ver=1&shifter=shifter&info=0&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check DCS errors, HV Trips, Noise Bursts, Data Corruption and upload the defects </a>(<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#EXPRESS_PROCESSING_run_report" target="_blank"><abbr>?</abbr></a>)'''


if table["DEFECTS"]=="DONE" and (table["ESCLEANING"]!="SUCCESS" or table["LCE"]!="SUCCESS" or "YES" not in table["LIST"]):
    text_for_next_steps = '''Wait for ES CAF Cleaning / LArCellEmpy to finish'''

if table["DEFECTS"]=="DONE" and table["ESCLEANING"]=="SUCCESS" and table["LCE"]=="SUCCESS" and table["ES"]!="DONE" and table["ESPROC"]==table["CosmicCaloTot"] and table["ESPROC"]!="0":
    text_for_next_steps = '''<a href="'''+baseurl+'''WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=larcomm&proc_ver=101&shifter=shifter&info=1&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check Noisy LAr Cells for UPD4</a> (<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#EXPRESS_PROCESSING_noisy_LAr_cel" target="_blank"><abbr>?</abbr></a>)<br />after expert\'s reply, set LAR_UNCHECKED defect absent in DB'''
    
if table["ES"]=="DONE" and ((table["BULKPROC"]!=table["CosmicCaloTot"] or table["BULKPROC"]=="0") or (table["ESPROCJETTAU"]!=table["JetTauEtmissTot"] or table["ESPROCJETTAU"]=="0")):
    text_for_next_steps = '''Wait for BULK Processing to finish'''

if table["ES"]=="DONE" and table["BULKPROC"]==table["CosmicCaloTot"] and table["BULKPROC"]!="0" and table["BULKCLEANING"]!="SUCCESS" and table["ESPROCJETTAU"]==table["JetTauEtmissTot"] and table["ESPROCJETTAU"]!="0" :
    text_for_next_steps = '''Wait for BULK CAF Cleaning to finish'''

if table["ES"]=="DONE" and table["BULKCLEANING"]=="SUCCESS":
    text_for_next_steps = '''<a href="'''+baseurl+'''WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=larcomm&proc_ver=102&shifter=shifter&info=1&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">Check Noisy LAr Cells for UPD3</a> (<a href="https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#BULK_checklist" target="_blank"><abbr>?</abbr></a>)<br />check HV NON NOMINAL Corrections<br />after expert\'s reply, set LAR_BULK_UNCHECKED defect absent in DB'''

if table["ES"]=="DONE" and table["BULK"]=="DONE":
    text_for_next_steps = "Do final signoff (<a href='https://atlasop.cern.ch/twiki/bin/view/Main/LArDQGuidelines#Data_period_signoff' target='_blank'><abbr>?</abbr></a>)<br />When finished, set LAR_UNCHECKED_FINAL defect absent in DB"

if table["FINAL"]=="DONE":
    text_for_next_steps = "<font color='green'>Run signed off: nothing else to do</font>"

 
img_ES_proc = iconpath
if table["ESPROC"]==table["CosmicCaloTot"] and table["ESPROC"]!="0" and table["CosmicCaloTot"]!="" and table["ESPROC"]!="": img_ES_proc += "tick.png"
else: img_ES_proc += "cross.png"
purcentage_ES_proc = 0
if table["CosmicCaloTot"]!="" and table["ESPROC"]!="":
    if float(table["CosmicCaloTot"])!=0: purcentage_ES_proc = float(table["ESPROC"])/float(table["CosmicCaloTot"])*100.
    else: purcentage_ES_proc = 0
else: purcentage_ES_proc = 0

img_ESEXPRESS_proc = iconpath
if ESProcExpressDone: img_ESEXPRESS_proc += "tick.png"
else: img_ESEXPRESS_proc += "cross.png"
purcentage_ESEXPRESS_proc = 0
if table["expressTot"]!="" and table["ESPROCEXPRESS"]!="":
    if float(table["expressTot"])!=0: purcentage_ESEXPRESS_proc = float(table["ESPROCEXPRESS"])/float(table["expressTot"])*100.
    else: purcentage_ESEXPRESS_proc = 0
else: purcentage_ESEXPRESS_proc = 0

img_BULK_proc = iconpath
if table["BULKPROC"]==table["CosmicCaloTot"] and table["BULKPROC"]!="0" and table["CosmicCaloTot"]!="" and table["BULKPROC"]!="": img_BULK_proc += "tick.png"
else: img_BULK_proc += "cross.png"
purcentage_BULK_proc = 0
if table["CosmicCaloTot"]!="" and table["BULKPROC"]!="":
    if float(table["CosmicCaloTot"])!=0: purcentage_BULK_proc = float(table["BULKPROC"])/float(table["CosmicCaloTot"])*100.
    else: purcentage_BULK_proc = 0
else: purcentage_BULK_proc = 0

img_BULKJETTAU_proc = iconpath
if table["ESPROCJETTAU"]==table["JetTauEtmissTot"] and table["ESPROCJETTAU"]!="0" and table["JetTauEtmissTot"]!="" and table["ESPROCJETTAU"]!="": img_BULKJETTAU_proc += "tick.png"
else: img_BULKJETTAU_proc += "cross.png"
purcentage_BULKJETTAU_proc = 0
if table["JetTauEtmissTot"]!="" and table["ESPROCJETTAU"]!="":
    if float(table["JetTauEtmissTot"])!=0: purcentage_BULKJETTAU_proc = float(table["ESPROCJETTAU"])/float(table["JetTauEtmissTot"])*100.
    else: purcentage_BULKJETTAU_proc = 0
else: purcentage_BULKJETTAU_proc = 0

img_defects_up = iconpath
if table["DEFECTS"]=="DONE": img_defects_up += "tick.png"
else: img_defects_up += "cross.png"

img_FINAL = iconpath
if table["FINAL"]=="DONE": img_FINAL += "tick.png"
else: img_FINAL += "cross.png"

img_LAr_Unchecked = iconpath
if table["ES"]=="DONE": img_LAr_Unchecked += "tick.png"
else: img_LAr_Unchecked += "cross.png"

img_LAr_BULK_Unchecked = iconpath
if table["BULK"]=="DONE": img_LAr_BULK_Unchecked += "tick.png"
else: img_LAr_BULK_Unchecked += "cross.png"

img_CAF_ES_cleaning = iconpath
color_ES_cleaning = "red"
if table["ESCLEANING"]=="SUCCESS":
    img_CAF_ES_cleaning += "tick.png"
    color_ES_cleaning = "green"
else: img_CAF_ES_cleaning += "cross.png"
if "NOT LAUNCHED YET" in table["ESCLEANING"]: color_ES_cleaning = "orange"
text_ES_cleaning = table["ESCLEANING"]

img_CAF_BULK_cleaning = iconpath
color_BULK_cleaning = "red"
if table["BULKCLEANING"]=="SUCCESS":
    img_CAF_BULK_cleaning += "tick.png"
    color_BULK_cleaning = "green"
else: img_CAF_BULK_cleaning += "cross.png"
if "NOT LAUNCHED YET" in table["BULKCLEANING"]: color_BULK_cleaning = "orange"
text_BULK_cleaning = table["BULKCLEANING"]

img_CAF_LCE = iconpath
color_LCE = "red"
if table["LCE"]=="SUCCESS" and "YES" in table["LIST"]:
    img_CAF_LCE += "tick.png"
    color_LCE = "green"
else: img_CAF_LCE += "cross.png"
if "NOT LAUNCHED YET" in table["LCE"]: color_LCE = "orange"
text_LCE = table["LCE"]
if "YES" in table["LIST"]: text_LCE += " and list produced"
else: text_LCE += " and list NOT produced"


text_for_defects = "Defects "
if table["DEFECTS"]=="DONE": text_for_defects+="uploaded"
else: text_for_defects+="NOT uploaded"

# html header
print '''
<html>
<head>
<meta name="author" content="'''+config.get('dev', 'email')+''' ['''+config.get('dev', 'email')+''']"/> 
<link rel="Stylesheet" href="'''+config.get('url','stylescss')+'''" type="text/css" />'''
# js script
print '''
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
          iframe.src = "'''+baseurl+'''LArPage1/iframe.py?run="+run
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

print '''<TABLE BORDER="1" cellspacing="0" cellpadding="0" width=99%>
<TR><TH align="center"><img src="'''+iconpath+'''update_button.png" ALT="update run" TITLE="update run" onClick="window.location.reload()" height=25 width=25>&nbsp;<br /></TD>
  <TH align="center" colspan=2>Tier 0 Processing</TD>
  <TH align="center" colspan=2>CAF jobs</TD>
  <TH align="center">LAr unchecked defects</TD>
  <TH align="center" width="300">Next steps</TD>
</TR>
<TR>
  <TD align="center" rowspan=3><b><a href="'''+baseurl+'''WebDisplayExtractor/index.py?low_run='''+str(run).replace(" ","")+'''&stream=CosmicCalo&source=tier0&proc_ver=1&shifter=shifter&info=0&rawchannels=on&calocells=on&sporadic=on&larcellempty=on&clusters=on" target="_blank">'''+str(run).replace(" ","")


print '''</a></b>'''
if table["ESPROCEXPRESS"]=="":
    print '''<font color="red">(run ongoing)</font>'''
    text_for_next_steps = '<blink>RUN CURRENTLY ONGOING</blink>'


print '''<!--<input type='checkbox' onclick="makeFrame('''+str(run).replace(" ","")+''');toggletext()" name='show_cb' value='none' id="hide" /><label for='hide'>(more...)</label><br />-->

    <br />'''+text_for_defects+'''<img src="'''+img_defects_up+'''" height=15 width=15><br />'''

if table["ESPROCEXPRESS"]!="": print '''<b>End of calib. loop</b><br />'''+str(table["ENDCALIBLOOP"])+'''<br />'''
else: print '''<b>End of calib. loop</b><br />Not defined: run ongoing<br />'''

print '''<b>Stable Beam:</b> '''+table["STABLEBEAM"]+'''<br />'''

if table["STABLEBEAM"]=="True":
    if table["ESPROCEXPRESS"]!="": 
        print '''<b>Approx. lumi</b>: '''+table["LUMI"]+'''<br />'''
        print '''<b>Average lum</b>: '''+table["LUMAVG"]+'''<br />'''
    else: 
        print '''<b>Approx. lumi</b>: run ongoing<br />'''

print '''<a href="'''+baseurl+'''HVTripsDB/index.py?query=select%20+trips.rowid,*+from+trips+INNER+JOIN+details+ON+trips.HVLINE+=+details.HVLINE+where+trips.TripRunNumber='''+str(run).replace(" ","")+'''+order+by%20trips.TripTimeStamp+desc" target="_blank">HV DB</a>, <a href="https://atlasdqm.cern.ch/defectentry/?run='''+str(run).replace(" ","")+'''&filter=LAr" target="_blank">Defect DB</a>, <a href="https://atlasdqm.cern.ch/dqsignoff/'''+str(run).replace(" ","")+'''">DQ Elogs</a></TD>
  <TD rowspan=2 align="center"><b>EXPRESS</b><br />
    
  </TD>'''


print '''<TD align="center" width="80" rowspan=2>
    <img src="'''+img_ES_proc+'''" height=25 width=25><br />
    <TABLE cellspacing="0" cellpadding="0" border=0 bgcolor=white width=100% height="10">
      <TR>
      CosmicCalo
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_ES_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_ES_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
    <img src="'''+img_ESEXPRESS_proc+'''" height=25 width=25>
    <TABLE cellspacing="0" cellpadding="0" border=0 bgcolor=white width=100% height="10">
      <TR><br />
      Express
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_ESEXPRESS_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_ESEXPRESS_proc-2.)+'''% height="10"></TD>'''

print '''
        <TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
  </TD>'''
print    '''<TD></TD>'''
#print '''<TD align="center">Cleaning:<br /><font color="'''+color_ES_cleaning+'''">'''+text_ES_cleaning+'''</font></TD>''' # obsolete box
print  '''<TD align="center" width=60><img src="'''+img_CAF_ES_cleaning+'''" height=25 width=25></TD>
  <TD rowspan=2 align="center"><img src="'''+img_LAr_Unchecked+'''" height=25 width=25></TD>
  <TD valign="center" align="center" rowspan="3">
  <font color="red">'''+text_for_next_steps+'''</font>
    <!--<ol>
      <!--<li><b>ES Checklist:</b><br /> Run Report<br /><br /></li>-->
      <!--<b>Check CAF jobs</b><br /><br />-->
      <!--<li><b>ES Checklist:</b><br /> Noisy LAr Cells<br /><br /></li>-->
      <!--<li><b>BULK Checklist:</b><br /> HV NON NOMINAL<br /><br /></li>-->
      <!--<li><b>BULK Checklist:</b><br /> Noisy LAr Cells</li>-->
    <!--</ol> -->'''
if table["HVNONNOMINAL"]=="PRESENT": print '''<br /><br /><font color="red">Caution: HV NON NOMINAL still present.</font>'''
if table["FINAL"]=="DONE": print '''<br /><br />Final Signoff <img src="'''+img_FINAL+'''" height=15 width=15>'''
print '''</TD>
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
        CosmicCalo
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_BULK_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_BULK_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
        <img src="'''+img_BULKJETTAU_proc+'''" height=25 width=25>
    <TABLE cellspacing="0" cellpadding="0" border=0 bgcolor=white width=100% height="10">
      <TR><br />
      JetTauEtmiss
	<TD bgcolor=white width=2% height="10"></TD>
	<TD bgcolor=green width='''+str(purcentage_BULKJETTAU_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=red width='''+str(100.-purcentage_BULKJETTAU_proc-2.)+'''% height="10"></TD>
	<TD bgcolor=white width=2% height="10"></TD>
      </TR>
    </TABLE>
  </TD>
  </TD>'''
print '''<TD></TD>'''
#print '''<TD width="150" align="center">Cleaning:<br /><font color="'''+color_BULK_cleaning+'''">'''+text_BULK_cleaning+'''</font></TD>''' # Obsolute box
print '''<TD align="center"><img src="'''+img_CAF_BULK_cleaning+'''" height=25 width=25></TD>
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

</body>'''

print '''</html>'''
