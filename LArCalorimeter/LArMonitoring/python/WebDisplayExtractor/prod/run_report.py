# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from threading import Thread
import xmlrpclib

global server

passfile = open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
passwd = passfile.read().strip(); passfile.close()  
passurl = 'https://%s@atlasdqm.cern.ch' % passwd
server = xmlrpclib.ServerProxy(passurl)

import sqlite3

import os,re,time,sys,cgi,string,math
import cgitb; cgitb.enable()

Eventtype = {2:'Raw data',4:'Result',8:'Calib',12:'Pedestal',16:'All+Result'}
global ATLASREADY
ATLASREADY = []
global NBR
global NBY
NBR = {}; NBY = {};
global noisedefects
global sevnoisedefects
noisedefects = []; sevnoisedefects = []

########################################################################
def listify(l):
  if len(l)==0: return ''
  elif len(l)==1: return str(l[0]) 
  l.sort()
  interval=False
  a = ''
  for i in range(len(l)-1):
    if interval: 
      if l[i+1]-l[i]==1: pass
      else: a += str(l[i])+' '; interval=False
    else:
      a += str(l[i])
      if l[i+1]-l[i]==1: a += '-'; interval=True
      else: a += ' '
  a += str(l[-1])
  return a

########################################################################
class queryhvtripdb(Thread):
  def __init__(self,run_spec):
    Thread.__init__(self)
    self.run_spec = run_spec

  def run(self):
    dbName='/afs/cern.ch/user/l/larmon/public/HVTripsDB/trips.db'    
    querycmd='select details.Det,details.Side,details.HVLINE,trips.TripLB,trips.RampUp,trips.StableZero,trips.RecoveryLB,trips.RecoveryRunNumber,details.status from trips INNER JOIN details ON trips.HVLINE = details.HVLINE where trips.TripRunNumber=%s order by trips.TripTimeStamp desc limit 100;'%self.run_spec['low_run']
    hv_conn = sqlite3.connect(dbName,detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
    c = hv_conn.cursor()
    c.execute(querycmd)
    trip = c.fetchall()
    self.List = trip
    #print "DEBUG inside:",self.List,"<br>"

########################################################################
class queryatlasready(Thread):
  def __init__(self,run_spec):
    Thread.__init__(self)
    self.run_spec = run_spec

  def run(self):
    athena_version = "17.2.6"
    import subprocess as sp
    cmd="export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;"
    cmd+="source $AtlasSetup/scripts/asetup.sh %s,32,here;"%(athena_version)
    cmd+="python /afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/WebDisplayExtractor/aux/GetATLASReadyLBs.py %s"%(self.run_spec['low_run'])
    P = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    tmp =  str(P.communicate()[0]).split('\n')[:-1]
    self.List=eval(tmp[-1])
    
########################################################################
class queryeventsperlb(Thread):
  def __init__(self,run_spec):
    Thread.__init__(self)
    self.run_spec = run_spec

  def run(self):

    athena_version = "17.2.6"
    
    if 'tmp_' in self.run_spec['stream']:
      stream = self.run_spec['stream'][4:]
    else:
      stream = self.run_spec['stream']

    import subprocess as sp
    cmd="export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;"
    #cmd+="source $AtlasSetup/scripts/asetup.sh 16.6.5,32,here,builds;"
    cmd+="source $AtlasSetup/scripts/asetup.sh %s,32,here;"%(athena_version)
    cmd+="python /afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/WebDisplayExtractor/aux/GetNEventsLB.py --run %s --stream %s;"%(self.run_spec['low_run'],stream)

    P = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    tmp = eval(P.communicate()[0])
    self.List = dict( [(x[1],x[0]) for x in list(tmp)] )

########################################################################
_Part = {'EMBA':'BarrelA','EMBC':'BarrelC','EMECA':'EmecA','EMECC':'EmecC','HECA':'HecA','HECC':'HecC','FCALA':'FcalA','FCALC':'FcalC'}

class mc(Thread):
  def __init__(self,run_spec,P):
    Thread.__init__(self)
    self.P = P
    self.run_spec = run_spec
    #self.mixed = mixed
    self.multicall = xmlrpclib.MultiCall( server )
    self.multicall.get_dqmf_sub_assessments(self.run_spec,'LAr/'+self.P+'/Data_Integrity')
    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Detector_Status/NDeviatingChannels'+self.P)
    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Data_Integrity/NullDigit_'+_Part[self.P])
    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Data_Integrity/Saturation_'+_Part[self.P])
    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Data_Integrity/LArFEBMonErrors'+_Part[self.P])
    
    for j in ['_burst','_timeVetoBurst','_pedestal_evolution','_quality_nChannel_burst']:
      #if self.mixed:
       # self.run_spec['stream'] = 'physics_CosmicCalo'
       # self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Noise/Partition/'+self.P+j)
       # self.run_spec['stream'] = 'express_express'      
      #else:
      self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Noise/Partition/'+self.P+j)
    

    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Data_Integrity/Out_E_FT_vs_SLOT_'+_Part[self.P])  
    self.multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Data_Integrity/missingTriggerType'+_Part[self.P])


  def run(self):

    self.results = self.multicall()
    self.runs = self.results[0].keys()
    self.FEBErrors_list = {}
    self.FEBErrors_results = {}

    for n,N in enumerate(self.runs):
      run_spec1 = self.run_spec.copy()
      run_spec1['high_run'] = N
      run_spec1['low_run'] = N
      H = self.results[0][N]['hists']
      self.multicall._MultiCall__call_list = []
      self.FEBErrors_results[N] = []

      ## Retrieve number of errors per FEB
      for h in H:
        if not 'LArFEBMonErrors' in h: continue
        self.multicall.get_dqmf_all_results(run_spec1,'LAr/'+self.P+'/Data_Integrity/'+h)
        self.FEBErrors_results[N].append( h )

      ## Retrieve error type per FEB
      #for h in H:
      #  if not 'FT' in h: continue
      #  self.multicall.get_dqmf_results(run_spec1,'LAr/'+self.P+'/Data_Integrity/'+h+':Mean')
      #  self.FEBErrors_results[N].append( [h] )
      #  #print 'JL check FEB errors:',h,self.FEBErrors_results,'<br>'
          
      r = self.multicall()
      for i,I in enumerate(self.multicall._MultiCall__call_list): 
        self.FEBErrors_results[N].append( r[i][N] )
        #print 'JL        ',r[i][N],'<br>'
        #print 'JL        ',self.FEBErrors_results[N],'<br>'  


########################################################################
def RunReport(run_spec,multicall,url,DCSGET,defects,includenotready,esproc,cleanesproc,bulkproc,cleanbulkproc,isexpress):

  # Queries
  Part = ['EMBA','EMBC','EMECA','EMECC','HECA','HECC','FCALA','FCALC']
  Part2 = ['EMBA','EMBC','EMECA','EMECC']
  _Part2 = {'EMBA':'BarrelA','EMBC':'BarrelC','EMECA':'EMECA','EMECC':'EMECC'}
  Part3 = {'EMBA':['EMB1A','EMB2A','EMB3A','EMBPA'],'EMBC':['EMB1C','EMB2C','EMB3C','EMBPC'],'EMECA':['EMEC1A','EMEC2A','EMEC3A','EMECPA'],'EMECC':['EMEC1C','EMEC2C','EMEC3C','EMECPC'],'HECA':['HEC0A','HEC1A','HEC2A','HEC3A'],'HECC':['HEC0C','HEC1C','HEC2C','HEC3C'],'FCALA':['FCAL1A','FCAL2A','FCAL3A'],'FCALC':['FCAL1C','FCAL2C','FCAL3C']}
  calls = []

  EMECA_lb=[]
  EMECC_lb=[]
  EMECA_mb=[]
  EMECC_mb=[]


  for p,P in enumerate(Part):
    calls.append( mc(run_spec,P) )
    calls[p].start()
 
  multicall._MultiCall__call_list = [] 
  multicall.get_dqmf_results(run_spec,'Global/DataFlow/m_release_stage_lowStat:Average')
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Run_Parameters/Eventtype')
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Run_Parameters/NbOfSamples')
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Data_Integrity/EventsRejectedYield')
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Collisions/LArCollTimeLumiBlockTimeCut')
 
  Plots = ['Data_Integrity/YieldOfRejectedEventsVsLBout','Data_Integrity/YieldOfRejectedEventsVsLB','Noise_Bursts/NoisyEvent_TimeVeto','Noise_Bursts/SaturatedTightEvent_TimeVeto']  
  Plots_description = ['Data Integrity(TimeVeto)','Data Integrity','Noise Burst(NoisyRO)','Noise Burst(Sat.Tight.)']
  for p in Plots: multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/'+p)

  for p,P in enumerate(Part2):
    multicall.get_dqmf_all_results(run_spec,'LAr/'+P+'/Noise/Partition/NoisyEvent_TimeVeto_'+_Part2[P])


  for p,P in enumerate(Part):
    Layers = Part3[P]
    for j,J in enumerate(Layers):
      multicall.get_dqmf_all_results(run_spec,'LAr/'+P+'/Signal_Time/Single_Cells/CellEnergyVsTime_'+J)
  
  multicall.get_dqmf_all_results(run_spec,'LAr/LAR_GLOBAL/Data_Integrity/EErrorsPerLB')
   
  results = multicall()


  # Retrieve HV trip from hv trip db
  hvtrip=queryhvtripdb(run_spec)
  hvtrip.start()
  hvtrip.join()
  HVTRIPS=hvtrip.List


  # Retrieve events per LB in the current stream from run query
  evtstream=queryeventsperlb(run_spec)
  evtstream.start()
  evtstream.join()
  NevtPerLB=evtstream.List

    

  # Retrieve stable beam info
  readyinfo = queryatlasready(run_spec)
  readyinfo.start()
  readyinfo.join()
  ATLASREADY=readyinfo.List

  # Print header
  S = ['NullDigit','Saturation']
  color = ['#FF0000','#0000FF']
  h_names = ['_burst','_timeVetoBurst','_pedestal_evolution','_quality_nChannel_burst']
  RE0 = re.compile(r'(?P<P>\S+)FT(?P<FT>\S+)Slot(?P<SL>\S+)')
  RE1 = re.compile(r'\((?P<LB>\S+)\.0*\)')
  RE2 = re.compile(r'x-axis\s*\S*\((?P<SL>\S+)\.0*\)\s*y-axis\s*\((?P<FT>\S+)\.0*\)\S+')


  print '''
<tr class="row">
<td><b>Run Info</td>
<td title="LAr Global"><b>LAr Global</b></td>
<td><b>Partition</td>
<td><b>Data integrity</b></td>
<td><b>Non-nominal HV</b></td>
<td><b>NullDigit/Saturation</td>
<td><b>Noise Bursts (LArNoisyRO-q factor)</td>
<td><b>Noise Bursts (Energy)</td>
<td><b>DSP computation</td>
<td><b>Timing</td>
</tr>
<tr>
<td></td>
<td>LB: &#37;ev (n) </td>
<td></td>
<td>(FT,SL)</td>
<td>HV correction</td>
<td>FT,SL,CH,&#37;ev,error</td>
<td>LB: &#37;ev (n) </td>
<td>LB:&nbsp;<a class="lb0" title="Number of events with more than 1% of all channels reporting |E|>3&#963;">Y3&#963</a>&nbsp;&nbsp;&nbsp;<a class="lb1" title="Number of events with more than 1% of all channels reporting |E|>3&#963; With Time Veto">Y3&#963(TimeVeto)</a>&nbsp;\n &nbsp;<a class="lb2" title="Sum of the energy (MeV) in all channels">SumE</a>&nbsp;&nbsp;<a class="lb3" title="number of cells with saturated Q factor">Sat Q</td>
<td><a class="lb3" title="Is Eoffline != Eonline for more than 20 events (per FEB) and (per LB)"><font color = black> # of FEBs/LBs</td>
<td><a class="lb3" title="check whether |<timing>| < 1.0 ns for layer 2 or < 2.0 ns for other layers"><font color = black>(ns)</td>
</tr>
'''

  if run_spec['stream'] == "physics_CosmicCalo" or run_spec['stream'] == "tmp_physics_CosmicCalo": CosmicCalo = True
  else: CosmicCalo =False
  if CosmicCalo:

    print '''
<tr>
<td>Stream</td>
<td></td>
<td></td>
<td>%s</td>
<td>%s</td>
<td>%s</td>
<td>%s</td>
<td>%s</td>
<td><a class="lb1">Not Valid for CosmicCalo<a></td>
<td><a class="lb1">Not Valid for CosmicCalo<a></td>
</tr>
''' % ('CosmicCalo','CosmicCalo','CosmicCalo','CosmicCalo','CosmicCalo')

  else:   

    print '''
<tr>
<td>Stream</td>
<td></td>
<td></td>
<td>%s</td>
<td>%s</td>
<td>%s</td>
<td>%s</td>
<td><a class="lb1">Only Valid for CosmicCalo<a></td>
<td>%s</td>
<td>%s</td>
</tr>
''' % (run_spec['stream'].split('_')[1],run_spec['stream'].split('_')[1],run_spec['stream'].split('_')[1],run_spec['stream'].split('_')[1],run_spec['stream'].split('_')[1],run_spec['stream'].split('_')[1])

  #<td title="LB with defects"><b>LBs with Defects</b></td>
  #<td title="# of collision candidates per LB">Collisions</td>
  #<td>LB: n</td>

  runs = results[1].keys()
  runs.sort()


  # Loop over runs
  for n,N in enumerate(runs):
    
    #F = file('/afs/cern.ch/user/l/larmon/public/reports/%s.txt'%(N),'w')
    Nev = -1
    if results[0].has_key(N): Nev = int(results[0][N])

    print '''
<tr class="out0">
<td rowspan="8" style="vertical-align:top">
<a href="http://atlas-runquery.cern.ch/query.py?q=find+run+%s+/+show+all+and+duration+and+ptag+and+dq+lar/+nodef" >%s</a><br /><hr />
<a href="https://atlasdqm.cern.ch/dqsignoff/%s"> DQ logbook </a><br /><hr />
<a href="%s">Tier 0 <br /> Webdisplay</a><hr />
%d<br />
%s<br />
%d samples<br />
<hr />
rejection<br />
<a href="%s/LAr/LAR_GLOBAL/Data_Integrity/EventsRejectedYield">%.3f&#37;</a>
</td>
''' % (N,N,N,url,Nev,Eventtype[int(results[1][N]['Mean'])],int(results[2][N]['Mean']),url,results[3][N]['Total'])


    Data_Integrity = []
    Data_Integrity_TimeVeto = []
    MediumNoisy_Events = []
    TightNoisy_Events = []
    Collisions = []

    # Bad LBs Spotting
    out = '<td rowspan="8" onclick="showit2(this);" style="vertical-align:top">\n'

    # Stable beam
    if len(ATLASREADY)==0:
      out += '<pre style="font-family:Arial"><font color=green>ATLAS Ready</font>: None<br/>'
    else:
      out += '<pre style="font-family:Arial"><font color=green>ATLAS Ready</font>: %s<br/>'%listify(ATLASREADY)
    out += '</pre>'

    # Print Red DCS flags
    if len(DCSGET)>0:
      dcsurl="http://atlas-runquery.cern.ch/query.py?q=find+run+%s+&#37;2F+show++dq+lar+DCSOFL"%run_spec['low_run']
      out += '<pre style="font-family:Arial"><a href=%s><font color=red>DCS Errors</font></a><br/>'%dcsurl
      for item in DCSGET:
        out+="%s lb %s<br/>"%(item,listify(DCSGET[item]))
      out += '</pre>'

    # Print HV Trips & Fill Defects
    hvtripurl="https://atlas-larmon.cern.ch/HVTripsDB/index.py?query=select&#37;20+trips.rowid,*+from+trips+INNER+JOIN+details+ON+trips.HVLINE+=+details.HVLINE+where+trips.TripRunNumber=%s+order+by&#37;20trips.TripTimeStamp+desc"%run_spec['low_run']
    out += '<pre style="font-family:Arial"><a href=%s><font color=red>HV Trips</font></a><br/>'%hvtripurl

    
    if len(HVTRIPS)>0:
      for trip in HVTRIPS:
        loc=str(trip[0])+str(trip[1]);    
        # warning: remove "PS" in partition name
        if loc=="EMBPSC": loc="EMBC";
        if loc=="EMBPSA": loc="EMBA";
        if loc=="EMECPSC":loc="EMECC";
        if loc=="EMECPSA":loc="EMECA";
        ch=int(trip[2]);
        triplb=int(trip[3]);rampup=int(trip[4]);
        stablezero=int(trip[5]);recoverlb=int(trip[6]);
        recoverrun=int(trip[7]);
        moduletype=str(trip[8]);


        ##########################
        ## Trip in a new module ##
        ##########################
        if (moduletype == "new"):
          if (recoverrun==int(run_spec['low_run'])):
            out+="%s trip %i-%i (new module) <br/>"%(loc,triplb,recoverlb) 
            # set defects
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP_NEWMODULE <font color=yellow>Y</font> %i-%i # channel %i - New HV module"%(loc,triplb,recoverlb,ch)
              defects.append(df)
          #in case of a short   
          else:
             out+="%s trip %i-%i (new module short) <br/>"%(loc,triplb,stablezero)
             if isexpress: 
               df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,stablezero,ch)
               defects.append(df)
               df = "LAR_%s_HVNONNOMINAL <font color=yellow>Y</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
               defects.append(df)
             else:
               df = "LAR_%s_HVNONNOMINAL <font color=green>G</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
               defects.append(df)
               df = "LAR_%s_HVNONNOM_CORRECTED <font color=red>R</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
               defects.append(df)
               
                      
   
        #################################
        ## Recovered trip within one LB  ##
        #################################
        elif (triplb == recoverlb):
            out+="%s trip %i (recovered) <br/>"%(loc,triplb) 
            # set defects
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i # channel %i"%(loc,triplb,ch)
              defects.append(df)

         
        #######################################
        ## Recovered trip - More than one LB ##
        #######################################
        elif (recoverrun==int(run_spec['low_run'])):
  
          ## autorecovered
          if (stablezero<0):
            out+="%s trip %i ramp %i-%i (recovered) <br/>"%(loc,triplb,rampup,recoverlb)
            # set defects
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i #  channel %i"%(loc,triplb,rampup,ch)
              defects.append(df)
              df = "LAR_%s_HVRAMPUP <font color=red>R</font> %i-%i #  channel %i"%(loc,rampup,recoverlb,ch)
              defects.append(df)
          ## manually recovered
          else:
            out+="%s trip %i stable %i-%i ramp %i-%i (recovered) <br/>"%(loc,triplb,stablezero+1,rampup-1,rampup,recoverlb)
            # set defects for ES
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,stablezero,ch)
              defects.append(df)
              df = "LAR_%s_HVRAMPUP <font color=red>R</font> %i-%i # channel %i"%(loc,rampup,recoverlb,ch)
              defects.append(df)
              df = "LAR_%s_HVNONNOMINAL <font color=yellow>Y</font> %i-%s # channel %i"%(loc,stablezero+1,rampup-1,ch)
              defects.append(df)
            # set defects for bulk
            else:
              df = "LAR_%s_HVNONNOMINAL <font color=green>G</font> %i-%s # channel %i"%(loc,stablezero+1,rampup-1,ch)
              defects.append(df)
              df = "LAR_%s_HVNONNOM_CORRECTED <font color=red>R</font> %i-%s # channel %i"%(loc,stablezero+1,rampup-1,ch)
              defects.append(df)
       
        ########################
        ## Not Recovered trip ##
        ########################
        else:
          ## easiest case: no ramp-up, no auto recovery
          if (rampup<0 and stablezero>0):
            out+="%s trip %i-%i (not recovered) <br/>"%(loc,triplb,stablezero)
            # set defects for ES
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,stablezero,ch)
              defects.append(df)
              df = "LAR_%s_HVNONNOMINAL <font color=yellow>Y</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
              defects.append(df)
            # set defects for bulk
            else:
              df = "LAR_%s_HVNONNOMINAL <font color=green>G</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
              defects.append(df)
              df = "LAR_%s_HVNONNOM_CORRECTED <font color=red>R</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
              defects.append(df)  
          ## second case: single failed ramp-up, no stable non nominal HV
          elif (rampup>0 and stablezero<0):
            out+="%s trip %i ramp %i-%s (not recovered) <br/>"%(loc,triplb,rampup,"End")
            # set defects for ES
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress : 
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,rampup,ch)
              defects.append(df)
              df = "LAR_%s_HVRAMPUP <font color=red>R</font> %i-%s # channel %i"%(loc,rampup,"End",ch)
              defects.append(df)
          ## third case: failed ramp-up, with stable non nominal HV AFTER ramp-up
          elif (rampup>0 and stablezero>0):
            if (stablezero > rampup):
              out+="%s trip %i ramp %i-%i stable %i-%s (not recovered) <br/>"%(loc,triplb,rampup,stablezero,stablezero+1,"End")
              # set defects for ES
              #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
              if isexpress:  
                df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,rampup,ch)
                defects.append(df)
                df = "LAR_%s_HVRAMPUP <font color=red>R</font> %i-%i # channel %i"%(loc,rampup,stablezero,ch)
                defects.append(df)
                df = "LAR_%s_HVNONNOMINAL <font color=yellow>Y</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)
                # set defects for bulk
              else:
                df = "LAR_%s_HVNONNOMINAL <font color=green>G</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)
                df = "LAR_%s_HVNONNOM_CORRECTED <font color=red>R</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)
            else:

              out+="%s trip %i-%i (not recovered) <br/>"%(loc,triplb,stablezero)
              # set defects for ES
              #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
              if isexpress:  
                df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,stablezero,ch)
                defects.append(df)
                df = "LAR_%s_HVNONNOMINAL <font color=yellow>Y</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)
                # set defects for bulk
              else:
                df = "LAR_%s_HVNONNOMINAL <font color=green>G</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)
                df = "LAR_%s_HVNONNOM_CORRECTED <font color=red>R</font> %i-%s # channel %i"%(loc,stablezero+1,"End",ch)
                defects.append(df)  
              
          # fourth case (highly improbable but already seen) :ramping until the end of the run !
          else:  
            out+="%s trip %i ramp %i-%s (not recovered) <br/>"%(loc,triplb,rampup,"End")
            # set defects for ES
            #if (run_spec['proc_ver'] == esproc or run_spec['proc_ver'] == cleanesproc):
            if isexpress:  
              df = "LAR_%s_HVTRIP <font color=red>R</font> %i-%i # channel %i"%(loc,triplb,rampup,ch)
              defects.append(df)
              df = "LAR_%s_HVRAMPUP <font color=red>R</font> %i-%s # channel %i"%(loc,rampup,"End",ch)
              defects.append(df)
    
    # No Trip
    else:
      out+="no trip found<br>"
    out += '</pre>'



    # Print Data integrity, noise bursts
    for j,J in enumerate(Plots):
      try: r = results[j+5][N]
      except:
        out += '?'
        continue

      if j==3 and isexpress:
        if run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc or run_spec['proc_ver']==102:continue


      K = r.keys()
      if len(K)>0: K.remove('NBins')
      LB = {}
      # Thresholds to display bursts on %events
      # Data Integrity/NoisyRO/Sat.Tight]
      thresholds = [1.,1.,0.,1.]
      for k in K:
        m = RE1.search(k).groupdict()
        lb = int(m['LB'])
        p = '%5.4f'%(r[k])
        # Apply thresholds for display
        if r[k] < thresholds[j]: continue
        # store LB per output value
        if includenotready or (lb in ATLASREADY):
          if LB.has_key(p): LB[p].append(lb)
          else: LB[p] = [lb]
          #print "DEBUG",j,J,k,r[k],p,LB,"<br>"
          if j==0: # Data Integrity Errors Outside Time Veto
            Data_Integrity_TimeVeto.append(lb)
          if j==1: # Data Integrity Errors
            Data_Integrity.append(lb)
          if j==2: # LArNoisyRO
            if lb in TightNoisy_Events: continue # avoid double counting
            TightNoisy_Events.append(lb)
          ##if j==3: # Saturated Tight
          ##  if r[k]> 5:
          ##    if lb in TightNoisy_Events: continue # avoid double counting
          ##    TightNoisy_Events.append(lb)
       
    
      if len(LB)>0:
        NBINS = 0
        for item in LB: NBINS += len(LB[item])
        a = '<a class="lb%d" href="%s/LAr/LAR_GLOBAL/%s">%s</a>' % (j,url,Plots_description[j],p)
        out += '<pre style="font-family:Arial;"><a href="%s/LAr/LAR_GLOBAL/%s" >%i LBs: %s</a>\n' % (url,J,NBINS,Plots_description[j])
        #if "Data Integrity" in Plots_description[j]:
        #  print "DEBUG",LB,len(LB),NBINS,"<br>"
        #for val in LB.keys():
        #  for lb in LB[val]:
        #    out += '%s: %s (%i)<br />' %(lb,val,round(float(val)*NevtPerLB[int(lb)]*0.01))
        P = LB.keys()
        P.sort()
        P.reverse()
        counter = 0;
        morethanfour = False
        for val in P:
          for lb in LB[val]:
            counter = counter+1
            if counter < 5: out += '%s: %s (%i)<br />' %(lb,val,round(float(val)*NevtPerLB[int(lb)]*0.01))
            elif counter == 5:
                out += '<a> <p  onclick="showit2(this);"  style="font-family:times;color:blue;" ><u>show</u></p> <!--f <br />'
                morethanfour = True
                out += '%s: %s (%i)<br />' %(lb,val,round(float(val)*NevtPerLB[int(lb)]*0.01))
            else: out += '%s: %s (%i)<br />' %(lb,val,round(float(val)*NevtPerLB[int(lb)]*0.01))
        if morethanfour: out +=' f-->\n'
        #if morethanfour: out +=' <p  onclick="showit3(this);"  style="font-family:times;color:blue;" ><u>hide</u></p> f-->\n'
        out += '</a></pre>'

        ### use this instead to compactify the list of LBs 
        #P = LB.keys()
        #P.sort()
        #P.reverse()
        #for p in P: 
        #   if float(p)>0.015: out += '%s: %s<br />' % (listify(LB[p]),p)
        #   else: out += '<!--%s: %s<br />-->' % (listify(LB[p]),p)
        #out += '</pre>'     
    
    #out += '</td>\n'
    Flagged_LBs = Data_Integrity_TimeVeto+TightNoisy_Events

    ###print "DEBUG - list of TIGHT bursts:",TightNoisy_Events,"<br>"
    ###print "DEBUG - list of MEDIUM bursts:",MediumNoisy_Events,"<br>"

    #########################################################################
    # May 2011:Store data integrity defects DUE to missing FEBs - ES only #
    #########################################################################
    ##if run_spec['proc_ver']==esproc or run_spec['proc_ver']==cleanesproc:
      ##df = "LAR_EMBA_COVERAGE <font color=red>R</font> # Dead controller for 2 FEBs"
      ##defects.append(df)

    #########################################################################
    # October 2012:Store data integrity defects DUE to missing FEBs - ES only #
    #########################################################################
    #if isexpress:
    #  df = "LAR_HECC_COVERAGE <font color=red>R</font> # FEB FT 16 Slot 5 powered off (cooling)"
    #  defects.append(df)
  
    ###################################################
    # Store data integrity defects - GLOBAL - ES only #
    ###################################################
    #######if len(Data_Integrity)>0 and (run_spec['proc_ver']==esproc or run_spec['proc_ver']==cleanesproc):
    #if len(Data_Integrity)>0 and (isexpress):  
    #    df = "LAR_DATACORRUPT <font color=red>R</font> "+listify(Data_Integrity)+" # data integrity error"
    #    defects.append(df)
    #if len(Data_Integrity)>0 and (run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc):
    #if len(Data_Integrity)>0 and (not isexpress):
    #    df = "LAR_DATACORRUPT <font color=green>G</font> "+listify(Data_Integrity)+" # data integrity error - Cleaned with Time Veto"
    #    defects.append(df)
    #if len(Data_Integrity_TimeVeto)>0 and (run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc):
    if len(Data_Integrity_TimeVeto)>0 and (not isexpress):
        df = "LAR_DATACORRUPT <font color=red>R</font> "+listify(Data_Integrity_TimeVeto)+" # data integrity error - NOT cleaned with Time Veto"
        defects.append(df)
      
    # Collisions
    #out += '<td rowspan="8" onclick="showit2(this);" style="vertical-align:top">\n'
    Collisions = []
    LB = {}
    try:
      r = results[4][N]
      K = r.keys()
      K.remove('NBins')
      for k in K:
        m = RE1.search(k).groupdict()
        lb = int(m['LB'])                                                                              
        p = int(r[k])
        if includenotready or (lb in ATLASREADY):
          if LB.has_key(p): LB[p].append(lb)
          else: LB[p] = [lb]
          Collisions.append(lb)

        #out += '<a href="%s/LAr/LAR_GLOBAL/Collisions/LArCollTimeLumiBlockTimeCut" >%d LBs</a><br />\n' % (url,r['NBins'])
        #out += '<a href="%s/LAr/LAR_GLOBAL/Collisions/LArCollTimeLumiBlockTimeCut" >%i LBs</a><br />\n' % (url,len(LB))
      out += '<pre style="font-family:Arial;"><a href="%s/LAr/LAR_GLOBAL/Collisions/LArCollTimeLumiBlockTimeCut" > Collisions: <br> %i LBs</a><br />\n' % (url,len(Collisions))
      if len(LB)>0:
        P = LB.keys()
        P.sort()
        P.reverse()
        #if run_spec['stream'] == 'physics_CosmicCalo':
        if CosmicCalo:  
          for p in P: 
            #if float(p)>1: out += '%s: <font color=red>%s</font><br />' %(listify(LB[p]),p)
            #else: out += '<!--%s: <font color=red>%s</font><br />-->' %(listify(LB[p]),p)
            out += '%s: <font color=red>%s</font><br />' %(listify(LB[p]),p)
    except: out += '?'
    out += '</td>\n'
    print out


    # Sum everything
    All_Bad_LBs = Data_Integrity+TightNoisy_Events+Collisions

    # Loop over partitions
    for p,P in enumerate(Part):
      calls[p].join()
      p_results = calls[p].results
      out = ''
      if p>0: out += '<tr class="out0">\n'
      out += '<td>'+P+'</td>\n'
      Partition_NoisyEvents = []
      Partition_MediumEvents = []
      Check_lb =[];Check_mb = []; Check_tb = []


      # FEB errors
      FEBErrors = ''
      error = ''
      FEBErrors_results = calls[p].FEBErrors_results[N]
      if len(FEBErrors_results)>0:
        h = FEBErrors_results[0] 
        r = FEBErrors_results[1] 
        K = r.keys()
        K.remove('NBins')
        fe = {}
        for k in K:
          m = RE2.search(k).groupdict()
          FT = int(m['FT'])
          SL = int(m['SL'])
          if fe.has_key(FT): fe[FT].append(SL)
          else: fe[FT] = [SL]
        if len(fe) > 0:  
          out += '<td style="width:100px" onclick="showit2(this);"><font color=red>%i reg. FEB errors<!--<br ><br />\n'%len(fe)
          for i in fe.keys():
            FEBErrors += '<a style="font-size:9px;" href="%s/LAr/%s/Data_Integrity/%s">(%d,%s)</a>\n' % (url,P,h,i,listify(fe[i])) 
          out += FEBErrors+'-->\n'
        else:
          out += '<td> %i reg. FEB errors <br ><br />'%len(fe)
          
      else: out += '<td>'
      

      try:
        FEB_trig = p_results[10][N]
        num_feb_trig = int(FEB_trig['NBins'])
        if len(FEBErrors_results)>0:
          if num_feb_trig > 0:
            out += '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Data_Integrity/missingTriggerType%s"><font color=red> <br>  %i PU trigger errors \n'%(url,P,_Part[P],FEB_trig['NBins'])
          else:
            out += ' %i PU trigger errors  <br ><br />' %(num_feb_trig)
      except: out += "Trig. check unavailable"       
        
      out += '</td>\n'
  
      # Partition with non-nominal HV
      HV = []
      r = p_results[1][N]
      if r['NBins']>0:
        K = r.keys(); K.remove('NBins')
        l = []                 
        for k in K: 
          m = RE1.search(k).groupdict()
          HV.append(int(m['LB']))

      if len(HV)>0:
        out += '<td style="width:80px"><a href="%s/LAr/%s/Detector_Status/NDeviatingChannels%s">%s</a></td>\n' % (url,P,P,listify(HV)) 
      else: out += '<td></td>\n'

      # NullDigit, Saturation
      prob=''
      for s in range(2):
        r = p_results[2+s][N]
        if r['NBins']>0:
          K = r.keys(); K.remove('NBins')
          for k in K:
            m = RE2.search(k).groupdict()
            SL = int(m['SL'])
            CH = int(m['FT'])
            if p<2:
              FT = (SL-1)/14
              SL = SL-FT*14
            else:                    
              FT = (SL-1)/15
              SL = SL-FT*15
            prob += '<a style="font-size:9px;" href="%s/LAr/%s/Data_Integrity/%sChan_%s">(%d,%d,%d,<font style="color:%s">%.1f</font>,%s)</a><br />' % (url,P,S[s],P,FT,SL,CH,color[s],100*r[k],S[s])
      if len(prob)>0: 
        out += '<td style="width:30px" onclick="showit2(this);"><a href="%s/LAr/%s/Data_Integrity/">%d FEB</a><br /><!--%s--></td>' % (url,P,prob.count('<br />'),prob)
      else: out += '<td style="width:30px"></td>\n'


      #####################################
      #NoiseBursts LArNoisyRO q-factor per partition
      #####################################
      out += '<td>'

      fail = False
      if p < 4:
        try: results[9+p][N]
        except: fail = True
      else:
        try: p_results[5][N]
        except: fail =True

      #EMB,EMEC        
      if p < 4 and not fail:    
        r = results[9+p][N]        
     
        K = r.keys()
        if len(K)>0: K.remove('NBins')
        LB = {}
 
        for k in K:
          m = RE1.search(k).groupdict()
          lb = int(m['LB'])
          x = '%5.4f'%(r[k])
          # store LB per output value
          if includenotready or (lb in ATLASREADY):
            if LB.has_key(x): LB[x].append(lb)
            else: LB[x] = [lb]
 
        ##Organize and display LBs (ordered by LB)
        #if len(LB)>0:
        #  LBs = [];LBmap = {}
        #  for val in LB.keys():
        #    for lb in LB[val]:
        #      LBs.append(lb)
        #      LBmap[lb]=[val]
        #  LBs.sort()   
        #  NBINS = 0
        #  for item in LB: NBINS += len(LB[item])
        #  out += '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Noise/Partition/NoisyEvent_TimeVeto_%s" >%i LBs: </a>\n' % (url,P,_Part2[P],NBINS)
        #  for lb in LBs:
        #  
        #    for val in LBmap[lb]:
        #
        #      num_evt = round(float(val)*NevtPerLB[int(lb)]*0.01)
        #      if isexpress:
        #        Partition_NoisyEvents.append(lb)
        #        if p == 2: EMECA_lb.append(lb) 
        #        if p == 3: EMECC_lb.append(lb) 
        #      else:
        #        if num_evt > 1.0:
        #          Partition_NoisyEvents.append(lb)
        #          if p == 2: EMECA_lb.append(lb) 
        #          if p == 3: EMECC_lb.append(lb) 
        #        else:
        #          Partition_MediumEvents.append(lb)
        #          if p == 2: EMECA_mb.append(lb) 
        #          if p == 3: EMECC_mb.append(lb) 
        #      out += '%s: %s (%i)<br />' %(lb,val,num_evt)


        #Organize and display LBs  (ordered by val)
        if len(LB)>0:
          NBINS = 0
          for item in LB: NBINS += len(LB[item])
          out += '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Noise/Partition/NoisyEvent_TimeVeto_%s" >%i LBs: </a>\n' % (url,P,_Part2[P],NBINS)
          Q = LB.keys()
          Q.sort()
          Q.reverse()
          counter = 0;
          morethanfour = False
          for val in Q:
            for lb in LB[val]:
              num_evt = round(float(val)*NevtPerLB[int(lb)]*0.01)
              if isexpress:
                Partition_NoisyEvents.append(lb)
                if p == 2: EMECA_lb.append(lb) 
                if p == 3: EMECC_lb.append(lb) 
              else:
                if num_evt > 1.0:
                  Partition_NoisyEvents.append(lb)
                  if p == 2: EMECA_lb.append(lb) 
                  if p == 3: EMECC_lb.append(lb) 
                else:
                  Partition_MediumEvents.append(lb)
                  if p == 2: EMECA_mb.append(lb) 
                  if p == 3: EMECC_mb.append(lb) 
              counter = counter+1
              if counter < 5: out += '%s: %s (%i)<br />' %(lb,val,num_evt)
              elif counter == 5:
                #out += '<p  onclick="showit2(this);"> ....<!-- <br />'
                out += '<p  onclick="showit2(this);"><u style="font-family:times;color:blue;">show</u> <!--f <br />'
                morethanfour = True
                out += '%s: %s (%i)<br />' %(lb,val,num_evt)
              else: out += '%s: %s (%i)<br />' %(lb,val,num_evt)
          if morethanfour: out +=' f--></p>\n'
      
      #HEC/FCAL          
      elif  not fail:
        r = p_results[5][N]
        if p == 4:
          Check_lb = EMECA_lb
          if not isexpress: Check_mb = EMECA_mb
        if p == 5:
          Check_lb = EMECC_lb
          if not isexpress: Check_mb = EMECC_mb
        if p == 6:
          Check_lb = EMECA_lb
          if not isexpress: Check_mb = EMECA_mb
        if p == 7:
          Check_lb = EMECC_lb
          if not isexpress: Check_mb = EMECC_mb
 
        K = r.keys()
        if len(K)>0: K.remove('NBins')
        LB = {} 
 
        for k in K:
          m = RE1.search(k).groupdict()
          lb = int(m['LB'])
          x = '%5.2f'%(r[k])
          # store LB per output value
          if includenotready or (lb in ATLASREADY):
            if lb in Check_lb:
              if LB.has_key(x): LB[x].append(lb)
              else: LB[x] = [lb]
              
        #Organize and display LBs
        if len(LB)>0:
          LBs = [];LBmap = {}
          for val in LB.keys():
            for lb in LB[val]:
              LBs.append(lb)
              LBmap[lb]=[val]
          LBs.sort()
          NBINS = 0
          for item in LB: NBINS += len(LB[item])
          #out += '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Noise/Partition/%s_burst" >%i LBs: </a>\n' % (url,P,P,NBINS)
          for lb in LBs:
            num_evt = round(float(val)*NevtPerLB[int(lb)]*0.01)
            if isexpress: Partition_NoisyEvents.append(lb)
            else:
              if num_evt > 1.0:
                Partition_NoisyEvents.append(lb)
                Check_tb.append(lb)
              else:
                Partition_MediumEvents.append(lb)
                Check_mb.append(lb)




                
            #Partition_NoisyEvents.append(lb)
            #for val in LBmap[lb]:
              #out += '%s: %s (%i)<br />' %(lb,val,math.ceil(float(val)*NevtPerLB[int(lb)]*0.01))

      else:  out += "Not available"        

    
      out += '</td>\n'

      #############################################################
      # Noise Bursts (Energy) per partition
      # Add thresholds O:N3sig>0 O:N3sigTV>0 1:E>10000 2:NQfactor>1
      #############################################################
      MinBias = False
      if run_spec['stream'] == "physics_MinBias": MinBias = True

      
      threshold=[0,0,50000,1]
      out += '<td onclick="showit2(this);">\n'
      LB = {}
      for j in range(4):
        try: p_results[5+j][N]#try: p_results[5][N]
        except:
          out += "Not available"
          continue
        r = p_results[5+j][N]
        K = r.keys()
        for k in K:
          if 'NBins' in k: continue
          m = RE1.search(k).groupdict()

          # store only lbs above threshold
          if int(r[k])>threshold[j]:
            lb = int(m['LB'])
            if includenotready or (lb in ATLASREADY):
              if not LB.has_key(lb): LB[lb] = [0,0,0,0]
              LB[lb][j] = int(r[k])
     
      # now flag noise bursts in LBs
    
      if len(LB)>0:
        LBs = LB.keys()
        LBs.sort()
        for i,lb in enumerate(LBs):
          if LB[lb][0]+LB[lb][1]+LB[lb][2]+LB[lb][3]==0: continue
          # Flag Red bursts per partition if Y3sigma TimeVeto > 0
          if LB[lb][1]>0:
            if NBR.has_key(P): NBR[P].append(lb)
            else:  NBR[P] = [lb]
          ## Flag Red bursts per partition if nQfactor > 10
          ##if LB[lb][3]>10:
          ##  if NBR.has_key(P): NBR[P].append(lb)
          ##  else:  NBR[P] = [lb]            
          # do not flag Yellow burst anymore
          #elif  LB[lb][2]>=5:
          #  if NBY.has_key(P): NBY[P].append(lb)
          #  else:  NBY[P]=[lb]
          mask = False
          #if lb in NBR or lb in NBY: mask = False 
          if mask: out += '<!--\n'
          burst = '' 
          for j,strength in enumerate(LB[lb]):
            if abs(strength)<1: continue         
            if j==1:burst += '<a class="lb%d" href="%s/LAr/%s/Noise/Partition/%s%s">(%d)</a>&nbsp;' % (j,url,P,P,h_names[j],strength)
            else: burst += '<a class="lb%d" href="%s/LAr/%s/Noise/Partition/%s%s">%d</a>&nbsp;' % (j,url,P,P,h_names[j],strength)       
                
          #if lb in TightNoisy_Events:out += '<font style="font-weight:bold;">%d</font>:%s &nbsp;' % (lb,burst.rstrip('&nbsp;'))
          if lb in Partition_NoisyEvents:out += '<font style="font-weight:bold;">%d</font>:%s &nbsp;' % (lb,burst.rstrip('&nbsp;'))
              
          else:out += '%d:%s &nbsp;' % (lb,burst.rstrip('&nbsp;'))
          if mask: out += '-->\n'   
    
      if run_spec['stream'].split('_')[1] == 'express':

        fail = False
        p_LB_err = []
        try: p_results[9][N]
        except: fail = True
        try: results[43][N]
        except: fail = True

        if not fail:
          r = p_results[9][N]          
          cells = r['NBins']
          bins = r.keys()
          bins.remove('NBins')
          FEBs=0

          LB_bins = results[43][N]
          LB_keys = LB_bins.keys()
          LB_keys.remove('NBins')
          #if p == 1: print LB_keys,"<br>"
          if (LB_bins['NBins'] > 0):
            for x,X in enumerate(LB_keys):
              if LB_bins[X] > 20.0:
                #if p==0: print "Value: ", LB_bins[X], "  Key: ", X, "  partition: ", X.split('(')[2].split('.')[0] ,"  LB: ",X.split('(')[1].split('.')[0],"<br>"
                found_p = X.split('(')[2].split('.')[0]
                found_LB = X.split('(')[1].split('.')[0]
                if p == 0 and found_p == '1': p_LB_err.append(found_LB)
                if p == 1 and found_p == '0': p_LB_err.append(found_LB)
                if p == 2 and found_p == '3': p_LB_err.append(found_LB)
                if p == 3 and found_p == '2': p_LB_err.append(found_LB)
                if p == 4 and found_p == '5': p_LB_err.append(found_LB)
                if p == 5 and found_p == '4': p_LB_err.append(found_LB)
                if p == 6 and found_p == '7': p_LB_err.append(found_LB)
                if p == 7 and found_p == '6': p_LB_err.append(found_LB)
          
          out +=  '<td>'
          #print "Partition: ", p, " LB: ",p_LB_err, "<br>"
          if cells > 0:
            for i,I in enumerate(bins):
              if r[I] > 150.0: FEBs+=1
              #print r[I],'<br>'
            if FEBs > 0:    
              #out +=  '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Data_Integrity/Out_E_FT_vs_SLOT_%s"><font color=red> <br>  %i FEBs with E difference</a><br />\n' % (url,P,_Part[P],FEBs)
              out +=  '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Data_Integrity/Out_E_FT_vs_SLOT_%s"><font color=red> <br>  %i FEBs with > 150 \n errors in E computation </a>' % (url,P,_Part[P],FEBs)

          if p_LB_err > 0:
              num_LB = 0
              for i in p_LB_err: num_LB = num_LB+1
              if num_LB > 0: out +=  '<pre style="font-family:Arial;"><a href="%s/LAr/LAR_GLOBAL/Data_Integrity/EErrorsPerLB"><font color=red> <br> %d LBs with > 20 \n errors in E computation </font> </a><br>' % (url,num_LB)
              for i in p_LB_err: out += '%s <br />' %(i)
              
          out +=  '</td>'
          
        else:    out +=  '<td> Not available </td>\n'

      else: out +=  '<td></td>\n'

      #####timing
      if CosmicCalo == False and  MinBias == False:
        
        fail = False
        Layers = Part3[P]
        out += '<td>'
        for j,J in enumerate(Layers):
          try: results[13+j][N]
          except: fail = True


          if not fail:
            if not p==7: r = results[13+j+p*4][N]
            else:   r = results[13+j+p*4-1][N]
            mean = r['Mean']
            if j==1:
              if mean > 1.0 or mean < -1.0:
                out +=  '<pre style="font-family:Arial;"><a href="%s/LAr/%s/Signal_Time/Single_Cells/CellEnergyVsTime_%s"><font color=red> %s mean=%.2f </a> \n' % (url,P,J,J,mean)
            elif mean > 2.0 or mean < -2.0:
              out +=  '<pre style="font-family:Arial;"><font color=red><a href="%s/LAr/%s/Signal_Time/Single_Cells/CellEnergyVsTime_%s"><font color=red> %s mean=%.2f </a> \n' % (url,P,J,J,mean)
                    
          else:  out +=  'Not available'
            
        out +=  '</td>\n'  

      else:out +=  '<td></td>\n'

      print out+'</td>\n</tr>\n'
      #Flagged_LBs += NBR+NBY
      

      ###############################################print "Noisy: ", NBR,Partition_NoisyEvents, "<br>"

      ##################################
      # Find noise bursts in partition #
      ##################################
      tb = []; mb = []
      ###if P in NBR: print "DEBUG RED",P,NBR[P],"<br>"
      ###if P in NBY: print "DEBUG YELLOW",P,NBY[P],"<br>"
      #for lb in TightNoisy_Events:
      #print Partition_NoisyEvents
      for lb in Partition_NoisyEvents:
        tb.append(lb)  ########################### NEW WAY ######################
      for lb in Partition_MediumEvents:
        mb.append(lb) 
        
        ###############################
        ##### OLD WAY #################
        ###############################
        ## add red burst always
        #if P in NBR and lb in NBR[P]:
        #  tb.append(lb)
        #  ###print "......DEBUG TIGHT partial tb (from NBR):",P,tb,"<br>" 
        #  continue
        ## add yellow bursts only if in more than 1 partition
        #elif P in NBY and lb in NBY[P]:
        #  ###print "***",P,lb,"<br>"
        #  occured = 0
        #  for part in NBR:
        #    for item in NBR[part]:
        #      if item == lb:
        #        occured+=1
        #        ###print "------------ found in NBR:",P,part,item,lb,occured,"<br>"
        #  for part in NBY:
        #    for item in NBY[part]:
        #      if item == lb:
        #        occured+=1
        #        ###print "------------ found in NBY:",P,part,item,lb,occured,"<br>"
        #  if occured > 1: tb.append(lb)
        #  ###print "......DEBUG partial tb (from NBY):",tb,"<br>"
          
      #for lb in MediumNoisy_Events:
      #  if lb in TightNoisy_Events: continue
      #  # print "DEBUG %s checking MEDIUM lb %i to be assigned <br>"%(P,lb)
      #  # add red burst always
      #  if P in NBR and lb in NBR[P]:
      #    mb.append(lb)
      #    ###print "......DEBUG MEDIUM partial mb (from NBR):",P,mb,"<br>" 
      #    continue
      #  # add yellow bursts only if in more than 1 partition
      #  elif P in NBY and lb in NBY[P]:
      #    occured = 0
      #    for part in NBR:
      #      for item in NBR[part]:
      #        if item == lb:
      #          occured+=1
      #    for part in NBY:
      #      for item in NBY[part]:
      #        if item == lb:
      #          occured+=1
      #    if occured > 1: mb.append(lb)

 
      ###########################  
      # Now fill bursts defects #
      ###########################
      #print "mb: ",mb," tb: ",tb," Check_mb: ",Check_mb," Check_lb: ",Check_lb, "<br>"
      
      if p > 3 and len(Check_lb) > 0:
        tb = Check_lb
        #if run_spec['proc_ver']==esproc or run_spec['proc_ver']==cleanesproc or run_spec['proc_ver']==101:
        if isexpress:  
          df = "LAR_"+P+"_NOISEBURST <font color=yellow>Y</font> "+listify(tb)+" # Noise burst in the LB spotted in %s - Cleaned by time veto"%(run_spec['stream'])
          noisedefects.append(df)
 
        # severe noise burst defect for BULK
        #if run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc or run_spec['proc_ver']==102:
        if not isexpress:  
          df = "LAR_"+P+"_SEVNOISEBURST <font color=red>R</font> "+listify(tb)+" # noise burst - NOT cleaned with Time Veto (v3)"
          sevnoisedefects.append(df)
          df = "LAR_"+P+"_NOISEBURST <font color=green>G</font> "+listify(tb)+" # noise bursts - NOT cleaned  with Time Veto (v3)"
          noisedefects.append(df)
      
      elif len(tb)>0 :
        # soft noise burst for ES
        #if run_spec['proc_ver']==esproc or run_spec['proc_ver']==cleanesproc:
        #if run_spec['proc_ver']==esproc or run_spec['proc_ver']==cleanesproc or run_spec['proc_ver']==101:
        if isexpress:  
          #df = "LAR_"+P+"_NOISEBURST <font color=yellow>Y</font> "+listify(tb)+" # noise bursts - Cleaned with Time Veto (v3)"
          df = "LAR_"+P+"_NOISEBURST <font color=yellow>Y</font> "+listify(tb)+" # Noise burst in the LB spotted in %s - Cleaned by time veto"%(run_spec['stream'])
          noisedefects.append(df)
 
        # severe noise burst defect for BULK
        #if run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc or run_spec['proc_ver']==102:
        if not isexpress:  
          df = "LAR_"+P+"_SEVNOISEBURST <font color=red>R</font> "+listify(tb)+" # noise burst - NOT cleaned with Time Veto (v3)"
          sevnoisedefects.append(df)
          df = "LAR_"+P+"_NOISEBURST <font color=green>G</font> "+listify(tb)+" # noise bursts - NOT cleaned  with Time Veto (v3)"
          noisedefects.append(df)

      if not isexpress:    

        if p > 3 and len(Check_mb) > 0:
          mb = Check_mb
          df = "LAR_"+P+"_NOISEBURST <font color=yellow>Y</font> "+listify(mb)+" # single isolated event flagged as LArNoisyRO_Std in JetTauEtmiss"
          noisedefects.append(df)
        elif len(mb)>0 :
          df = "LAR_"+P+"_NOISEBURST <font color=yellow>Y</font> "+listify(mb)+" # single isolated event flagged as LArNoisyRO_Std in JetTauEtmiss"
          noisedefects.append(df)  
 
      
    # include partition defects in global defects
    for item in noisedefects:defects.append(item)
    for item in sevnoisedefects:defects.append(item)
      
    if n<len(runs)-1: print '\n<tr><td colspan="8"><hr/></td></tr>\n'

 
