#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string,math
import cgitb; cgitb.enable()
import sqlite3
import xmlrpclib
global server
#server = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch')

passfile = open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
passwd = passfile.read().strip(); passfile.close()  
passurl = 'https://%s@atlasdqm.cern.ch' % passwd
server = xmlrpclib.ServerProxy(passurl)

from threading import Thread
dbfile = 'LArBad.db'
global cursor
global conn

global info
global beambkg
global opts
global expert
global run_spec
global esproc
global bulkproc
global reproc
global cleanesproc
global cleanbulkproc
global lce_inputfile
global cluster_threshold
global includenotready
global ladies
global isexpress
global pass_letter
global cluster_E

global DCSGET  

global defects

DCSGET={};

defects = [];hvdefects = []

global Sides
Sides = []
global Times
Times = [time.time()]
Duplicated = [];Clusters = []
Clusters_found = 200*[0]
ClusterTypes = ['EMTopoClusters','CaloTopoClusters']
 
global multicall
multicall = xmlrpclib.MultiCall( server )

# set defaults
isexpress =False
info = False
beambkg = False
includenotready = False
ladies = False
expert = 10
opts = {}
run_spec = {}
the_url = 'https://atlas-larmon.cern.ch/WebDisplayExtractor/index.py?'
esproc=1;bulkproc=2;reproc=3;
cleanesproc=101;cleanbulkproc=102;
cluster_threshold = 50;
cluster_E = 'Et>10GeV'

global rawchannels
global calocells
global sporadic
global larcellempty
global clusters

c = cgi.FieldStorage()
########################print c
for i in c.keys():
  v = c.getvalue(i)
  if i=='info': info = int(v)
  elif i=='ladies': ladies = int(v)
  elif i=='beambkg': beambkg = True
  elif i=='expert': expert = 14
  elif i=='rawchannels': rawchannels = True
  elif i=='calocells': calocells = True
  elif i=='sporadic': sporadic = True
  elif i=='larcellempty': larcellempty = True
  elif i=='clusters': clusters = True
  elif i in ['DET','AC','SAM']: opts[i] = v
  elif i=='cluster_threshold': cluster_threshold = int(v)
  elif i=='includenotready': includenotready = True
  elif i=='cluster_E': cluster_E = v
  else: run_spec[i] = v
  the_url += i+'='+v+'&'
  lce_inputfile="LCE/BadCellList_run%s.txt"%run_spec['low_run']

########################################################################################
print '''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LAr WebDisplay Extractor</title>
<meta name="author" content="Mathieu Plamondon [Mathieu.Plamondon@cern.ch]"/>
<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />
<script type="text/javascript" src="scripts_ref.js"></script>
<base target="_blank">
</head>
<body>
<div class="wrapper"><br />
<a href="https://atlasop.cern.ch/atlas-point1/twiki/bin/view/Main/LArWebDisplayExtractor" style="color:#000000;font-size:20pt;font-weight:bold" title="click to access the twiki">LAr WebDisplay Extractor <font color=red>Dev Version</font>  </a>'''
#print '<font style="color:#FF0000">WARNING: Stuff might break unexpectedly... Development and tests in progress</font><br>'
#<img src="twiki.gif" style="width:50px" alt="twiki"/></a><br />

#########################################################################################

if not 'rawchannels' in dir() and not 'calocells' in dir() and not 'sporadic' in dir() and not 'larcellempty' in dir() and not 'clusters' in dir(): 
  rawchannels = True
  calocells = True
  sporadic = True
  larcellempty = True
  clusters = True
else:
  if not 'rawchannels' in dir(): rawchannels = False
  if not 'calocells' in dir(): calocells = False
  if not 'sporadic' in dir(): sporadic = False
  if not 'larcellempty' in dir(): larcellempty = False
  if not 'clusters' in dir(): clusters = False

the_url = the_url.rstrip('?').rstrip('&')
if not run_spec.has_key('proc_ver'): run_spec['proc_ver'] = 1
else:  run_spec['proc_ver'] = int(run_spec['proc_ver']) 
if run_spec.has_key('stream'):
  if run_spec['stream']=='express': run_spec['stream']='express_express'
  else: run_spec['stream'] = 'physics_'+run_spec['stream']
else: run_spec['stream'] = 'physics_CosmicCalo'
if not run_spec.has_key('source'): run_spec['source'] = 'tier0'

if run_spec.has_key('low_run'):
  run_spec['high_run'] = run_spec['low_run']
  multicall.get_runs_streams(run_spec)
  results = multicall()
  if len(results[0])==0:
    print "<h3><font color=red>&#8224 RUN '%s' DOES NOT EXIST</font></h3>"%(run_spec['low_run'])
    sys.exit()



if info == 0:  #RunReport
  if ladies == 1 :  #ladies-bulk
    run_spec['stream'] = 'physics_JetTauEtmiss'
    run_spec['proc_ver'] = 1
    isexpress = False
    run_spec['source'] = 'tier0'
    pass_letter = 'f'
    if run_spec.has_key('low_run'):
      run_info = server.get_run_information(run_spec)
      run_tag  = run_info[run_spec['low_run']]
      run_tag_string = run_tag[1]
      if 'data13' in run_tag_string:
        run_spec['stream'] = 'physics_HardProbes'
  
  elif ladies == 0:  #ladies-express
    run_spec['stream']='express_express'
    run_spec['proc_ver'] = 1
    isexpress = True
    run_spec['source'] = 'tier0'
    pass_letter = 'x'

  #expert options
  elif ladies == 2:
    isexpress = True
    pass_letter = 'x'
  else:
    isexpress = False
    pass_letter = 'f'
    
  #else:
  #  if run_spec['stream'] == 'physics_CosmicCalo':
  #    if  run_spec['proc_ver'] == 1: isexpress = True
  #    #elif run_spec['proc_ver'] == 2: isexpress = True
  #    else: isexpress = False
  #  elif run_spec['stream'] == 'express_express':
  #    if  run_spec['proc_ver'] == 1: isexpress = True
  #    else: isexpress = False
  #  else:  isexpress = False
      
    
else:  #NoisyCells
  if ladies == 1 : #ladies-bulk
    run_spec['stream'] = 'physics_CosmicCalo'
    run_spec['proc_ver'] = 102
    isexpress = False
    run_spec['source'] = 'test'
    pass_letter ='f'
    multicall.get_runs_streams(run_spec)
    results = multicall()
    if len(results[0])==0:
        run_spec['proc_ver'] = 1
        run_spec['source'] = 'tier0'
        print "<font color=red>You are using the basic tier0 output. If more than 10-20 UPD3 channels are proposed, you should perform a cleaning to confirm this diagnostic.</font>"

      
  
  elif ladies == 0: #ladies-express
    run_spec['stream']='physics_CosmicCalo'
    run_spec['proc_ver'] = 111
    isexpress = True
    run_spec['source'] = 'larcomm'
    pass_letter = 'x'
    multicall.get_runs_streams(run_spec)
    results = multicall()
    if len(results[0])==0:
      run_spec['proc_ver'] = 101
      run_spec['source'] = 'test'
      multicall.get_runs_streams(run_spec)
      results = multicall()
      if len(results[0])==0:
        run_spec['proc_ver'] = 1
        run_spec['source'] = 'tier0'
 

  #expert options
  elif ladies == 2:
    isexpress = True
    pass_letter = 'x'
  else:
    isexpress = False
    pass_letter ='f'

if run_spec.has_key('low_run') and run_spec['proc_ver'] == 1:
  mapping = server.get_procpass_amitag_mapping(run_spec)
  Proc_ver = []
  run_list = mapping[run_spec['low_run']]
  for i,item in enumerate(run_list):
    #    print item,"<br>"
    check_tag = False
    for f,field in enumerate(item):
      if f == 0:
        proc = field
      if f == 1:
        if field == run_spec['stream']:
          check_tag = True
      if (f == 2 and check_tag == True):
        tag = field[:1]
        if tag == pass_letter: Proc_ver.append(proc)        
  #print Proc_ver,"<br>"
  if not Proc_ver :
    print "<h3><font color=red>&#8224 BULK FOR RUN '%s' NOT YET FINISHED</font></h3>"%(run_spec['low_run'])
    sys.exit()
  else: run_spec['proc_ver'] = max(Proc_ver)

    

#if not opts.has_key('shifter'): opts['shifter'] = 'shifter'
f0 = os.stat(dbfile)
f1 = os.stat('index.py')  


########################################################################
class dcs(Thread):
  def __init__(self):
    Thread.__init__(self)

  def run(self):

    athena_version = "17.2.6"
    import subprocess as sp
    cmd="export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;"
    cmd+="source $AtlasSetup/scripts/asetup.sh %s,32,here;"%(athena_version)
    cmd+='detStatus_query.py --folder /GLOBAL/DETSTATUS/DCSOFL --r=%s "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_GLOBAL;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO" HEAD EM HEC FCAL;'%(run_spec['low_run'])
    P = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    self.DCSflags = P.communicate()


########################################################################
# i[12:16] = [[-3s,+3s,LB,Sat.Q],[Eth,5s,DB,Q],[E>20,LB,Q],[<E>,E>1,LB,Q],DR]
# R[0:4] = [[-3s,+3s,LB,Sat.Q],[Eth,5s,DB,Q],[E>20,LB,Q],[<E>,E>1,LB,Q],DR]
########################################################################

#proposed_flag
#import prop_flag

#print top 
import print_top

#print rows
#import print_rows

#Print Selection
#Add report button
import selection

#Listify
#RunReport
import run_report

#noisycells
import noisycells

########################################################################
def ShowPlots():
  size = 300 
  if len(Sides)>1: size = 250 
  print '<div id="plots">\n'
  print '<br />\n'
  for i in ClusterTypes:
    if info==1: print "<br><b>",i,"</b><br>"
    for j in Sides:
      if i == "EMTopoClusters":
        if cluster_E == 'Et>5GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EMclus_etaphi_Et_thresh1@JJJ'.replace('III',i).replace('JJJ',j)
        elif cluster_E == 'E>5GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EMclus_EtavsPhi1@JJJ'.replace('III',i).replace('JJJ',j)
        elif cluster_E == 'E>10GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EMclus_EtavsPhi2@JJJ'.replace('III',i).replace('JJJ',j)
        else: tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EMclus_etaphi_Et_thresh2@JJJ'.replace('III',i).replace('JJJ',j)
      if i == "CaloTopoClusters":
        if cluster_E == 'Et>5GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_clus_etaphi_Et_thresh1@JJJ'.replace('III',i).replace('JJJ',j)
        elif cluster_E == 'E>5GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EtavsPhi1@JJJ'.replace('III',i).replace('JJJ',j)
        elif cluster_E == 'E>10GeV': tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_EtavsPhi2@JJJ'.replace('III',i).replace('JJJ',j)
        else: tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_clus_etaphi_Et_thresh2@JJJ'.replace('III',i).replace('JJJ',j)
        #tmp_url = url+'/CaloMonitoring/CaloMonShift/CaloMonJJJ/IIIJJJ/m_clus_etaphi_Et_thresh2@JJJ'.replace('III',i).replace('JJJ',j)
      print ('<a href="AA"><img style="width:%dpx" src="AA.png" /></a>\n'%(size)).replace('AA',tmp_url)
    if len(Sides)>1: print '<br />'                                                                                             
  print '<br />\n</div>\n'
   
#########################################################################################
#print '''
#<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
#<html xmlns="http://www.w3.org/1999/xhtml">
#<head>
#<title>LAr WebDisplay Extractor</title>
#<meta name="author" content="Mathieu Plamondon [Mathieu.Plamondon@cern.ch]"/>
#<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
#<link rel="Stylesheet" href="styles.css" type="text/css" />
#<script type="text/javascript" src="scripts_dev.js"></script>
#<base target="_blank">
#</head>
#<body>
#<div class="wrapper"><br />
#<a href="https://atlasop.cern.ch/atlas-point1/twiki/bin/view/Main/LArWebDisplayExtractor" style="color:#000000;font-size:20pt;font-weight:bold" title="click to access the twiki">LAr WebDisplay Extractor <font color=red>Version 3.8</font>  </a>'''
##print '<font style="color:#FF0000">WARNING: Stuff might break unexpectedly... Development and tests in progress</font><br>'
##<img src="twiki.gif" style="width:50px" alt="twiki"/></a><br />
#########################################################################################


#if ladies == 1 and not run_spec['proc_ver'] == 102 :
    #procpass = server.get_next_proc_pass(run_spec['low_run'],run_spec['stream'],'tier0')
    #print procpass
    #proc_ver = procpass-1
    #run_spec['proc_ver'] = proc_ver 

selection.PrintSelection(run_spec,info,cluster_threshold,beambkg,expert,includenotready,ladies,cluster_E)

if run_spec.has_key('low_run'):
  import sqlite3
  conn = sqlite3.connect('LArId.db')
  cursor = conn.cursor()

  run_string = (run_spec['low_run']).strip(); run_spec['low_run'] = run_string

  # first line of upload defect text file
  defects.append("@%s"%run_spec['low_run'])

  #if run_spec['proc_ver'] == 1: defects.append("LAR_FIRSTDEFECTS_UNCHECKED G # Express processing defects uploaded")
  if isexpress: defects.append("LAR_FIRSTDEFECTS_UNCHECKED G # Express processing defects uploaded")

  # set web display links
  global url
  url = 'https://atlasdqm.cern.ch/webdisplay/%s/%d/%s/run_%s/run/' % (run_spec['source'],run_spec['proc_ver'],run_spec['stream'],run_spec['low_run'])

  if not run_spec.has_key('high_run'):# or info!=0: 
    run_spec['high_run'] = run_spec['low_run']
    #multicall.get_runs_streams(run_spec)
      
    #results = multicall()
    #if len(results[0])==0:
    #  print "<h3><font color=red>&#8224 RUN '%s' DOES NOT EXIST</font></h3>"%(run_spec['low_run'])
    #  sys.exit()

    _stream = run_spec['stream']
    for i in results[0][run_spec['low_run']]: 
      if _stream in i: _stream=i   
    if 'tmp_' in _stream:
      url = url.replace(run_spec['stream'],_stream) 
      run_spec['stream'] = _stream
      print "<font color=red>WARNING</font>: <b>DO NOT FILL the DEFECT DB YET</b>. The processing of this run is still ongoing. You will get partal info. Some LBs might be missing.<br><br>"
  ## Display info depending on the menu:
  ## 0: Run Report
  ## 1: Noisy LAr Cells
  try:
    ## Run Report Menu
    if info==0:
      
      print '</form>\n'
      
      DataIntegrityProblem = ['Parity','BCID','Sample Header','EVTID','SCAC status','SCA out of range','Gain mismatch','Type mismatch','# of samples','Empty data block','Checksum/block size','Missing header','Bad gain']
      ## Retrieve RED LBs from DCS
      DCS = dcs()
      DCS.start()
      DCS.join()
      d = DCS.DCSflags[0].replace('\n','<br>')
      # if RED, store partition and affected LBs
      # EMBA : [ 174693 , 1 ] - [ 174693 , 19 ]
      RE = re.compile(r'(?P<PART>\S+)\s*\:\s*\[\s*(?P<RUN1>\S+)\s*\,\s*(?P<LB1>\S+)\s*\]\s*\-\s*\[\s*(?P<RUN2>\S+)\s*\,\s*(?P<LB2>\S+)\s*\]')
      test = d.split('HEAD')[1].split(')')
      for item in test:
        if not 'RED' in item: continue
        st = item.split('RED')[0].replace('<br>','')
        m = RE.search(st).groupdict()
        for lb in range(int(m['LB1']),int(m['LB2'])):
          if DCSGET.has_key(m['PART']): DCSGET[m['PART']].append(lb)
          else: DCSGET[m['PART']] = [lb]
   

      #print DCSGET
      #print "Mark 2 <br>"
      #print m['PART']
      #get_dqmf_summary_flags_lb(run_spec, PART, COMP200_DCS, HEAD)

      ## Link to HV Trips DB & DCS
      #print "DQ flags from DCS automatically retrieved"
      #print '<input type="button" value="Check DCS flags" onclick=DCSQuery(%s) />'%(run_spec['low_run'])
      #print '<input type="button" value="Check HV Trips DB" onclick="HVTripsDB()"/>'

      Part = ['EMBA','EMBC','EMECA','EMECC','HECA','HECC','FCALA','FCALC']
      for P in Part:
        new_dcs = server.get_dqmf_summary_flags_lb(run_spec,P,"DCSOFL","HEAD")
      

      ## Now get run report
      print '<br><table class="report">\n'

      run_report.RunReport(run_spec,multicall,url,DCSGET,defects,includenotready,esproc,cleanesproc,bulkproc,cleanbulkproc,isexpress)
 
      print '</table>\n'

      # Finally, add report button
      selection.AddReportButton(defects,run_spec,info,beambkg)

    ## Noisy LAr Cells Menu
    else:
      run_spec['high_run'] = run_spec['low_run']
      #print "<font color=red><h3>Warning: DQ checks based on TopoClusters only !</h3></font><br>"
      
      clusters=True
      print '<table class="list">\n' 
      print_top.PrintFields(expert)
      print_top.PrintTopRow(expert,opts,rawchannels,calocells,sporadic,larcellempty,clusters)
      noisycells.NoisyLArCells(run_spec,opts,Sides,rawchannels,calocells,sporadic,larcellempty,clusters,Clusters,Duplicated,cluster_threshold,Clusters_found,lce_inputfile,expert,cursor,url,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,beambkg,isexpress,cluster_E)
      print '</table>\n</form>\n'
      # Add report button
      selection.AddReportButton(defects,run_spec,info,beambkg)

      
  except ValueError:
    print 'ValueError:',ValueError
  except:
    print 'Unexpected error:', sys.exc_info()
    print '<br />Problem encountered: a bug report has been sent<br />'
    #os.system('echo "%s\n%s" | mail -s "WebExtractor problem" christopher.marino@cern.ch' % (the_url,sys.exc_info()))
    
  ShowPlots()
  conn.close()
else:
  print '<table class="list">\n' 
  print_top.PrintFields(expert)
  print_top.PrintTopRow(expert,opts,rawchannels,calocells,sporadic,larcellempty,clusters)
  print '</table>\n</form>\n'

## here is the section to print non-matched clusters
Times.append(time.time())
print '<div id="help" style="background:#CCCCCC">\n'
if clusters and info!=0:
  #print '<br>Non-matched clusters:<hr><pre>'
  first = 1
  for cl in range(len(Clusters)):
    CL = Clusters[cl]
    if Clusters_found[cl]==0:
      type = CL[3].split("/")[3].split("Topo")[0]
      if type=="EM" or CL[3].split('@')[1] != "BAR":
        if first: print '<br>Non-matched clusters:<hr><pre>';first=0;
        print '%s %s <a href="%s/%s" class="det2">%s</a> Nentries:%i'%(type,CL[3].split('@')[1],url,CL[3],CL[4],CL[5])
  print '</pre>'
  # And here, print duplicated clusters
  if len(Duplicated)>0:
    print 'Duplicated clusters:<hr><pre>'
    for cl in  range(len(Duplicated)):
      CL = Duplicated[cl]
      type = CL[3].split("/")[3].split("Topo")[0]
      print '%s %s <a href="%s/%s" class="det2">%s</a> Nentries:%i'%(type,CL[3].split('@')[1],url,CL[3],CL[4],CL[5])
    print '</pre>'
      
## Footer
print '''</div>
<div class="push"></div>
</div>
<div class="footer">
<hr>
%6.1f seconds (total time) <a href="%s">Link to this query</a>, <a href="https://atlas-larmon.cern.ch/WebDisplayExtractor/logs/">logs server</a><br />
Developer: <a href="mailto:Mathieu.Plamondon@cern.ch">Mathieu Plamondon</a>&nbsp;
DQ contacts: <a href="mailto:trocme@lpsc.in2p3.fr,christopher.marino@cern.ch">Benjamin Trocm&eacute;, Christopher Marino &nbsp;<br></a>
Last updates: DB file (%s), <a href="https://svnweb.cern.ch/trac/atlasusr/browser/mplamond/projects/LArIdtranslator/trunk/www" >script file</a> (%s)<br>
<br />
</div>
</body>
</html>''' % (Times[-1]-Times[0],the_url,time.ctime(f0.st_ctime),time.ctime(f1.st_ctime))

#if info: os.system('echo "%s" | mail -s "LArWebExtractor query %s" Mathieu.Plamondon@cern.ch' % (the_url,os.environ['HTTP_X_FORWARDED_FOR']))
