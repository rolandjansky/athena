#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import getopt,fileinput
import subprocess as sp
from time import *

#config reading for default url
import ConfigParser 
config = ConfigParser.ConfigParser()
config.read('config.ini')
baseurl = config.get('url', 'base')
LatestRunUrl = config.get('url', 'LatestRun')



###############rank 1
def ShowListOfRuns(list_of_runs,number_limit):
  print'''<br /> '''
  
  limit = number_limit
  if len(list_of_runs) < limit: limit = len(list_of_runs)

  for i in range(0,limit):
    #print "run",i
    #print list_of_runs
    
    this_run_number = list_of_runs[i]
    #print this_run_number

    if str(this_run_number)=="": continue
    
    print '''<div id="loading2'''+str(i)+'''" style="" value="hidden"><b><blink>Loading summary information from database...</blink></b></div>
    <iframe onload="hide_loading2('''+str(i)+''')" src="'''+baseurl+'''LArPage1/RunTable.py?run='''+str(this_run_number)+'''" height="240" width="100%" frameborder="0"></iframe>'''

    print '''&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;more about run '''+str(this_run_number)+'''&nbsp;&nbsp;<input type='checkbox' onclick="makeFrame('''+str(i)+''','''+str(this_run_number)+''');toggletext('''+str(i)+''')" name='show_cb' value='none' id="hide'''+str(i)+'''" /><label for='hide'></label><br />
    
    <div id="test'''+str(i)+'''" style="visibility:hidden;display:none" value="not" width=100%">
      <div id="loading'''+str(i)+'''" style="" value="hidden"><b><blink>Loading information from database...</blink></b></div>
    </div><br />'''


########MAIN FUNCTION########
# html header
print '''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LADIeS Page 1</title>
<meta name="author" content="'''+config.get('dev', 'email')+''' ['''+config.get('dev', 'email')+''']"/> 
<link rel="Stylesheet" href="'''+config.get('url','stylescss')+'''" type="text/css" />'''
# js script
print '''
<script type="text/javascript">
function hide_loading(i){
if(Obj3.value=="hidden"){
Obj3=document.getElementById("loading"+i)
Obj3.style.display='none'
Obj3.style.visibility='hidden'
Obj3.value='hidden'
}
Obj3=document.getElementById("loading"+i)
Obj3.style.display='none'
Obj3.style.visibility='hidden'
Obj3.value='hidden'
}
</script>

<script type="text/javascript">
function hide_loading2(i){
Obj3=document.getElementById("loading2"+i)
Obj3.style.display='none'
Obj3.style.visibility='hidden'
Obj3.value='hidden'
}
</script>

<script language="JavaScript" type="text/javascript">
function makeFrame(i,run) {
          Obj2=document.getElementById("test"+i)
          var iframe;
          if (document.all){
          iframe = document.createElement(\'<iframe onload="hide_loading()"></iframe>\');
          }
          else if (document.getElementById) {
          iframe = document.createElement('iframe');
          iframe.onload=function(){hide_loading(i)};
          }
          if (!iframe) return;
          iframe.frameBorder = 0;
          iframe.width = 380;
          iframe.height = 200;
          iframe.src = "'''+baseurl+'''LArPage1/iframe.py?run="+run
          iframe.setAttribute ("id", "testframe"+i);
          if(Obj2.value!='already') document.getElementById ("test"+i).appendChild(iframe);
          Obj2.value='already'
  }
   '''
#document.body.appendChild(ifrm);
print'''
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
function toggletext(i){
Obj=document.getElementById("test"+i)
Obj2=document.getElementById("hide"+i)
var test2 = 0
Obj3=document.getElementById("loading"+i)
if(Obj2.value=='none'){
     if(Obj3.value=='hidden') hide_loading(i)
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
 
'''


# get shifter names
import ShifterNames
infos_shifters=ShifterNames.GetCurrentShifterNames()

#print infos_shifters

print '''</head>
<body onload="uncheck()">'''

print '''<div class="wrapper"><br />'''
print '''<a href="" style="color:#000000;font-size:20pt;font-weight:bold">LADIeS Page 1</a><br><br>'''

if infos_shifters["FIRSTNAMESHIFTER2"]=="":
  infos_shifters["FIRSTNAMESHIFTER2"]="NONE"

if infos_shifters["FIRSTNAMESHIFTER1"]=="":
  infos_shifters["FIRSTNAMESHIFTER1"]="NONE"

if infos_shifters["FIRSTNAMESHIFTER2"]!="NONE" and infos_shifters["FIRSTNAMESHIFTER1"]=="NONE":
  print '''<b>Current shifter:</b> <A HREF="mailto:'''+infos_shifters["EMAILSHIFTER2"]+'''">'''+infos_shifters["SURNAMESHIFTER2"]+''' '''+infos_shifters["FIRSTNAMESHIFTER2"]+'''</A>, no main shifter (only support)<br /><br />'''

if infos_shifters["FIRSTNAMESHIFTER2"]=="NONE" and infos_shifters["FIRSTNAMESHIFTER1"]!="NONE":
  print '''<b>Current shifter:</b> <A HREF="mailto:'''+infos_shifters["EMAILSHIFTER1"]+'''">'''+infos_shifters["SURNAMESHIFTER1"]+''' '''+infos_shifters["FIRSTNAMESHIFTER1"]+'''</A>, no support shifter<br /><br />'''

if infos_shifters["FIRSTNAMESHIFTER2"]!="NONE" and infos_shifters["FIRSTNAMESHIFTER1"]!="NONE":
  print '''<b>Current shifters:</b> <A HREF="mailto:'''+infos_shifters["EMAILSHIFTER1"]+'''">'''+infos_shifters["SURNAMESHIFTER1"]+''' '''+infos_shifters["FIRSTNAMESHIFTER1"]+'''</A>,  <A HREF="mailto:'''+infos_shifters["EMAILSHIFTER2"]+'''">'''+infos_shifters["SURNAMESHIFTER2"]+''' '''+infos_shifters["FIRSTNAMESHIFTER2"]+'''</a><br /><br />'''

if infos_shifters["FIRSTNAMESHIFTER2"]=="NONE" and infos_shifters["FIRSTNAMESHIFTER1"]=="NONE":
  print '''<b>Current shifter:</b> no shifter!<br /><br />'''


print '<b>To update info for a given run:</b> click on the "reload" button above the corresponding run number (No need to reload the full page)<br>'
print '<b>Warning:</b> the CAF job status is not working yet.<br/>'

# cgi input data purser 
cgifields = cgi.FieldStorage()
cgidata = {}
for i in cgifields.keys():
  cgidata[i] = str(cgifields[i].value)


# check for lastrun
if (not cgidata.has_key('lastrun')) or  int(cgidata['lastrun']) <1:
  import urllib
  f = urllib.urlopen(LatestRunUrl)
  last_run_bytes = f.read()
  last_run_str = last_run_bytes.decode("utf8")
  cgidata['lastrun'] = last_run_str
  cgidata['nolastrun'] = 1
  # for the "Last Run" nruns has a default value in config.ini
  if (not cgidata.has_key('nruns')) or cgidata.has_key('nruns') < 1:
    cgidata['nruns'] = str(config.get('run', 'nruns'))

# check for firstrun
defdelta=int(config.get('run', 'defdelta'))
if (not cgidata.has_key('firstrun')) or  int(cgidata['firstrun']) <1:
  cgidata['firstrun'] = str(int(cgidata['lastrun'])-defdelta)
  cgidata['nofirstrun'] = 1

# existancve of minrun (could be some problem with data retreving)
minfirstrun=str(config.get('run', 'minfirstrun'))
if int(cgidata['firstrun'])<int(minfirstrun):
  cgidata['firstrun']=minfirstrun
if int(cgidata['lastrun'])<int(minfirstrun):
  cgidata['lastrun']=minfirstrun

# check for maxdelta lastrun-firstrun
maxdelta=int(config.get('run', 'maxdelta'))
if int(cgidata['lastrun'])-int(cgidata['firstrun'])>maxdelta:
  cgidata['firstrun']=str(int(cgidata['lastrun'])-defdelta)

# check for firstrun<lastrun
if int(cgidata['firstrun']) > int(cgidata['lastrun']): 
  #swap firstrun lastrun
  tmp=cgidata['firstrun']
  cgidata['firstrun']=cgidata['lastrun']
  cgidata['lastrun']=tmp

# check for maxrun
if not cgidata.has_key('nruns'):
  cgidata['nruns']=str(-1)

#check for datatag
if not cgidata.has_key('datatag'):
  cgidata['datatag']=config.get('run', 'defdatatag')


#check for beamcondition
if not cgidata.has_key('beamcondition'):
  cgidata['beamcondition']=config.get('run', 'defbeamcondition')

#check for dqstatus
if not cgidata.has_key('dqstatus'):
  cgidata['dqstatus']=config.get('run', 'defdqstatus')

#check for minlum (integral luminosity)
if not cgidata.has_key('minlum'):
  cgidata['minlum']=str(config.get('run', 'defminlum'))

#check for minnevent (number of events)
if not cgidata.has_key('minnevent'):
  cgidata['minnevent']=str(config.get('run', 'defminnevent'))

#check for minnlb (number of luminosity blocks)
if not cgidata.has_key('minnlb'):
  cgidata['minnlb']=str(config.get('run', 'defminnlb'))

  
# options input
print '''<FORM METHOD=GET ACTION="'''+baseurl+'''LArPage1/index.py" TARGET=_BLANK>
<table border="0">
 <tr>
   <th>First run</th>
   <th>Last run</th>
   <th>Data tag</th>
   <th>Beam condition</th>
   <th>DQ status</th>
   <th>Min Lum.</th> <!-- int luminocity-->
   <th>Min N event</th> <!--Number of events -->
   <th>Min NLB</th><!--Number of luminosity blocks (-1 if run ongoing)  -->
   <th>Max runs/page</th>
   <th><input type="reset" value="Reset"></th>
 </tr>
 <tr>
   <td><INPUT type=text name="firstrun" size="10" '''
if not cgidata.has_key('nofirstrun'): 
  print '''value="'''+str(cgidata['firstrun'])
print '''"></td>
 <td><INPUT type=text name="lastrun" size="10"'''
if not cgidata.has_key('nolastrun'): 
  print '''value="'''+str(cgidata['lastrun'])
print '''"></td>
   <td><INPUT type=text list="datatagtype" value="'''+cgidata['datatag']+'''" name="datatag">
       <datalist id="datatagtype"  size="20">'''
datataglist=config.get('run', 'datataglist')
for datatag in datataglist.split():
  print '''<option value="'''+datatag+'''">'''
print '''</datalist> 
   </td>
   <td> <select name="beamcondition">
         <option value="0"'''
if int(cgidata['beamcondition'])==0:
  print " selected"
print      '''>No cond.</option>
         <option value="1"'''
if int(cgidata['beamcondition'])==1:
  print " selected"
print      '''>stable beam</option>
       </select> 
   </td>
   <td> <select name="dqstatus">
         <option value="0"'''
if int(cgidata['dqstatus'])==0:
  print " selected"
print      '''>No filter</option>
         <option value="1"'''
if int(cgidata['dqstatus'])==1:
  print " selected"
print      '''>Express</option>
         <option value="2"'''
if int(cgidata['dqstatus'])==2:
  print " selected"
print      '''>Bulk</option>
       </select> 
   </td>
   <td><INPUT type=text name="minlum" value="'''+str(cgidata['minlum'])+'''" size="10"></td>
   <td><INPUT type=text name="minnevent" value="'''+str(cgidata['minnevent'])+'''" size="10"></td>
   <td><INPUT type=text name="minnlb"  value="'''+str(cgidata['minnlb'])+'''" size="10"></td>
   <td><INPUT type=text name="nruns" value="'''+str(cgidata['nruns'])+'''" size="10"></td>
   <td>&nbsp;&nbsp;<INPUT type="submit" value="Submit">&nbsp;&nbsp;</td>
 </tr>
</table>
</FORM>
'''



#making runlist
import GetListOfRuns
runlist= GetListOfRuns.makerunlist(cgidata)
# presenting runs on the web page
ShowListOfRuns(runlist,len(runlist))


## Footer
print '''</div>

<div class="push"></div>
</div>
<div class="footer">
<hr>'''
print '''Developer: <a href="mailto:'''+config.get('dev', 'email')+'''">'''+config.get('dev', 'name')+'''</a>&nbsp;
DQ contact: <a href="mailto:'''+config.get('DQ', 'email')+'''">'''+config.get('DQ', 'name')+'''</a> &nbsp;<br />
Last update: '''+config.get('dev', 'lastupdate')
print'''
</div>'''
print '''</body>
</html>''' 
