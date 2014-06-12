# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

DETlist = ['EMB','EMEC','HEC','FCAL']
dbfile = 'LArBad.db'
from threading import Thread
import sqlite3
import os,re,time,sys,cgi,string,math
import xmlrpclib

passfile = open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
passwd = passfile.read().strip(); passfile.close()  
passurl = 'https://%s@atlasdqm.cern.ch' % passwd
server = xmlrpclib.ServerProxy(passurl)

global Parts
Parts = []

import print_rows
import print_top

ACmap = {1:'A+','1':'A+','A+':'A+',-1:'C-','-1':'C-','C-':'C-'}
ACmap2 = {'A':1,'C':-1}
inv_ACmap2 = {1:'A+','1':'A+','A+':'A+',-1:'C-','-1':'C-','C-':'C-'}
dd=[0.,0.,0.,0.]
## CaloCell bins variations to match real cells: [EMB[sampling],EMEC[sampling],HEC[sampling],FCAL[sampling]]
de=[[0.01,0.001,0.005,0.01],[0.01,0.001,0.01,0.01],[0.001,0.001,0.001,0.001],[0.,0.02,0.1,0.1]]
dp=[[0.01,0.003,0.005,0.02],[0.01,0.005,0.01,0.01],[0.05,0.05,0.05,0.05],[0.,0.05,0.03,0.02]]
cols = ['OFF_ID','ONL_ID','DET','AC','FT','SL','CH','SAM','ETA','PHI','HVLINES','HVCORR','CL','TT_COOL_ID']

ClusterTypes = ['EMTopoClusters','CaloTopoClusters']
BadChannels = ['deadReadout','deadCalib','deadPhys','almostDead','short','unstable','distorted','lowNoiseHG','highNoiseHG','unstableNoiseHG','lowNoiseMG','highNoiseMG','unstableNoiseMG','lowNoiseLG','highNoiseLG','unstableNoiseLG','missingFEB','peculiarCL','problematicFor?','sporadicBurstNoise']

########################################################################
mp = ['noise_acceptance_AVG','acceptance_AVG','signal_AVG','quality_AVG','fractionOverQ','DB','5Sigma','hiEth','Quality','ENLB','EN','Warning','Error','Cluster','LCE0','LCE1','LCE2','LCE3']
# 0:GREEN 1:RED 2:BLUE 3:PINK 4:YELLOW 5:BLACK  
col = [1,0,2,3,3,2,1,0,3,2,1,4,1,5,0,1,2,3]
def GetClass(plot):
  for a in range(18):
    if mp[a] in plot:
     return col[a]


########################################################################
def get_id(c):
  a = 7<<27
  be=0
  if c[0]>0: be=1
  a |= be<<25
  ac=0
  if c[1]>0: ac=1
  a |= ac<<24
  a |= c[2]<<19
  a |= (c[3]-1)<<15
  a |= c[4]<<8
  return a


########################################################################
class db(Thread):
  def __init__(self,cmd,plot,par,V):
    Thread.__init__(self)
    self.cmd = cmd
    self.plot = plot
    self.par = par
    self.V = V
  def run(self):
    con = sqlite3.connect('LArId.db')
    cur = con.cursor()
    cur.execute(self.cmd)
    self.R = cur.fetchall()
    con.close()

########################################################################
class mc2(Thread):
  def __init__(self):
    Thread.__init__(self)
    self.multicall = xmlrpclib.MultiCall( server )
  def run(self):
    self.results = self.multicall()
########################################################################
class mc3(Thread):
  def __init__(self,P,run_spec):
    Thread.__init__(self)
    self.multicall = xmlrpclib.MultiCall( server )
    self.P = P
    self.run_spec = run_spec
  def run(self):
    self.results = self.multicall()
    ##print "DEBUG - mc3 - 1:",self.P,len(self.multicall._MultiCall__call_list),time.time(),"<br>"  
    ##print "DEBUG - mc3 - 2:",self.multicall._MultiCall__call_list,"<br>" 
    ##print "DEBUG - mc3 - 3:",self.results,"<br>" 
    N = self.run_spec['low_run']
    H = self.results[0][N]['hists']
    calls = []
    i=0
    for h in H:
      ##print "DEBUG", h,"<br>"
      if not 'Sl' in h: continue
      if i%50==0: calls.append( mc2() )
      calls[-1].multicall.get_dqmf_all_results(self.run_spec,'LAr/'+self.P+'/Noise/Single_Cells/Non_Gaussian_Tails/'+h)
      i+=1

    self.h2p = []
    for c in calls: c.start()
    for c in calls:
      c.join()
      for i,I in enumerate(c.multicall._MultiCall__call_list):
        plot = I[1][-1].split('/')[-1]
        self.h2p.append( [plot,c.results[i][N]] )


########################################################################
class GetLArCellEmpty(Thread):
  def __init__(self,cluster_threshold,lce_inputfile):
    Thread.__init__(self)
    self.cluster_threshold = cluster_threshold
    self.lce_inputfile = lce_inputfile
  def run(self):

    self.Chans = {}
    f = open(self.lce_inputfile,"r")
    for line in f:
      fields=string.split(line)
      
      if fields[0]=='onlid':continue
      # skip cells found only with algo 1
      algo=int(fields[11])

      if algo == 1: continue
      # retrieve all cells
      onlid = fields[0]
      part=fields[1][0:-2]
      side=fields[1][-1]
      FT=int(fields[2])
      SL=int(fields[3])
      CH=int(fields[4])
      evt=int(fields[6])
      emean=float(fields[7])
      lb=float(fields[8])
      qf=float(fields[9])
      lblist=fields[11].split(";")
      comment="LB:"
      for item in lblist:comment+=" %s"%item
     
      d=DETlist.index(part)
      id = get_id((d,ACmap2[side],FT,SL,CH))
      # store the channels
      # cut to match the cluster threshold
      if evt < self.cluster_threshold: continue
      # now store the channel
      if not self.Chans.has_key(id): self.Chans[id] = [ [],[],[],[] ]
      self.Chans[id][0].append([int(emean),'&lt;E&gt;GeV','LCE0'])
      self.Chans[id][1].append([evt,'Nevts&gt;1GeV','LCE1'])
      self.Chans[id][2].append([int(lb),comment,'LCE2'])
      self.Chans[id][3].append([int(qf*100),'%qfac&gt;4000','LCE3'])
       
########################################################################
class GetRawChannels(Thread):
  def __init__(self,run_spec,Parts):
    Thread.__init__(self)
    self.run_spec = run_spec
    self.Parts = Parts
  def run(self):

    RE = re.compile(r'\S*\((?P<SSLN>\S+)\.0*\)\s*y-axis\s*\((?P<CH>\S+)\.0*\)\S+')
    N = self.run_spec['low_run']
    calls = []
    Plots = ['acceptance_AVG','noise_acceptance_AVG','signal_AVG','quality_AVG']
    for P in self.Parts:
      calls.append( mc2() )
      for p in Plots: calls[-1].multicall.get_dqmf_all_results(self.run_spec,'LAr/'+P+'/Noise/Single_Cells/'+P+'_'+p)
      calls[-1].start()

    self.Chans = {}
    #print "DEBUG %i <br>"%(len(calls))
    for c in calls:
      c.join()
      for i,I in enumerate(c.multicall._MultiCall__call_list):
        plot = I[1][-1]
        part = plot.split('/')[1]
        d = DETlist.index(part[:-1])
        for p,P in enumerate(Plots): 
          if P in plot: pi=p; break
        r = c.results[i][N]
        K = r.keys()
        for k in K:
          if not 'x-axis' in k: continue
          m = RE.search(k).groupdict()
          SSLN = int(m['SSLN'])
          if d: FT = (SSLN-1)/15; SL = SSLN-FT*15
          else: FT = (SSLN-1)/14; SL = SSLN-FT*14
          CH = int(m['CH'])
          id = get_id((d,ACmap2[part[-1]],FT,SL,CH))
          if not self.Chans.has_key(id): self.Chans[id] = [ [],[],[],[] ]
          if 'noise_acceptance' in plot: self.Chans[id][1].append( [r[k],k,plot] )
          elif 'acceptance' in plot:  self.Chans[id][0].append( [r[k],k,plot] )
          elif 'signal' in plot: self.Chans[id][2].append( [r[k],k,plot] )
          elif 'quality' in plot: self.Chans[id][3].append( [r[k],k,plot] )

########################################################################
class GetClusters(Thread):
  def __init__(self,run_spec,Sides,Clusters,Duplicated,cluster_threshold,cluster_E):
    Thread.__init__(self)
    self.run_spec = run_spec
    self.Sides = Sides
    self.Clusters = Clusters
    self.Duplicated = Duplicated
    self.cluster_threshold = cluster_threshold
    self.cluster_E = cluster_E
  def run(self):

    # Distance cells/clusters; 
    DR_match = 0.2;
    # Decoding the DQ bin output
    RE = re.compile(r'(?P<cat>\S+)-\(eta,phi\)\[OSRatio\]=\((?P<eta>\S+),(?P<phi>\S+)\)\[(?P<sigma>\S+)\]')
    N = self.run_spec['low_run']
    multicall = xmlrpclib.MultiCall( server )

    print "<br> Energy setting",self.cluster_E

    for j in self.Sides:
      for i in ClusterTypes:
        if i=='EMTopoClusters':
          if self.cluster_E == 'E>5GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EMclus_EtavsPhi1@%s' % (j,i,j,j) )
          elif self.cluster_E == 'E>10GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EMclus_EtavsPhi2@%s' % (j,i,j,j) )
          elif self.cluster_E == 'Et>5GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EMclus_etaphi_Et_thresh1@%s' % (j,i,j,j) )
          else: multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EMclus_etaphi_Et_thresh2@%s' % (j,i,j,j) )
        if i=='CaloTopoClusters':
          if self.cluster_E == 'E>5GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EtavsPhi1@%s' % (j,i,j,j) )
          elif self.cluster_E == 'E>10GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_EtavsPhi2@%s' % (j,i,j,j) )
          elif self.cluster_E == 'Et>5GeV': multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_clus_etaphi_Et_thresh1@%s' % (j,i,j,j) )
          else: multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_clus_etaphi_Et_thresh2@%s' % (j,i,j,j) )
          #multicall.get_dqmf_all_results(self.run_spec,'CaloMonitoring/CaloMonShift/CaloMon%s/%s%s/m_clus_etaphi_Et_thresh2@%s' % (j,i,j,j) )
    
    #print "DEBUG - clusters :",len(multicall._MultiCall__call_list),time.time(),"<br>"
    R = multicall()

    for i,I in enumerate(multicall._MultiCall__call_list):
      r = R[i][N]
      K = r.keys()
      #print "DEBUG",r,K,"<br>"
      for k in K:
        if k[0] == "Y" or k[0] == "R":
          # select clusters above threshold
          #print "DEBUG",k,r[k],"<br>"
          if int(r[k])<self.cluster_threshold: continue
          m = RE.search(k).groupdict()
          # Clusters can be both in EC and BARREL plots. Avoid double counting.
          is_duplicate = False
          for cl in self.Clusters:
            #print "DEBUG CLUSTERS",cl[4],k,"<br>"
            if k.split("=")[1] == cl[4].split("=")[1]: is_duplicate = True
          if is_duplicate:
            self.Duplicated.append( [float(m['eta']),float(m['phi']),DR_match,I[1][-1],k,int(r[k])] )
          else:
            self.Clusters.append( [float(m['eta']),float(m['phi']),DR_match,I[1][-1],k,int(r[k])] )
            
########################################################################
class GetCaloCells(Thread):
  def __init__(self,run_spec,Parts):
    Thread.__init__(self)
    self.run_spec = run_spec
    self.Parts = Parts
  def run(self):

    N = self.run_spec['low_run'] 
    RE = re.compile(r'.*=\((?P<eta>\S+),(?P<phi>\S+)\).*')
    RE1 = re.compile(r'(?P<cat>\S+)-\(eta,phi\)\[OSRatio\]=\((?P<eta>\S+),(?P<phi>\S+)\)\[(?P<sigma>\S+)\]')
    RE2 = re.compile(r'x-axis\s*\S*\((?P<eta>\S+)\)\s*y-axis\s*\((?P<phi>\S+)\)\)')
    S  = {0:'P',1:'1',2:'2',3:'3'}
    Si = {'r':0,'0':0,'1':1,'2':2,'3':3}
    calls = []

    for P in self.Parts:
      d = DETlist.index(P[:-1])
      calls.append( mc2() )
      plot = 'LAr/'+P+'/Noise/Single_Cells/'
      for s in range(4):
        sam = 'Sampling'+str(s)
        s2 = S[s]
        if s==0:
          if d==3: continue
          if d==2: s2='0'
          else: sam='Presampler'
  
        # change in plot name after run 182161
        suffix='_hiEth'
        comment1='Nevts&gt;Eth'
        comment2='%qfac>4000(E&gt;Eth)'
        if int(self.run_spec['low_run']) < 182161:
          suffix='_4Sigma'
          comment1='%evts&gt;4&#963'
          comment2='%qfac>4000(E&gt;4&#963)'
 
        calls[-1].multicall.get_dqmf_all_results(self.run_spec,plot+'%s/fractionOverQthVsEtaPhi_%s'%(sam,P[:-1]+s2+P[-1])+suffix )
        calls[-1].multicall.get_dqmf_all_results(self.run_spec,plot+'%s/EnergyRMSvsDB_%s_noEth'%(sam,P[:-1]+s2+P[-1]) )
        calls[-1].multicall.get_dqmf_all_results(self.run_spec,plot+'%s/CellOccupancyVsEtaPhi_%s_hiEth'%(sam,P[:-1]+s2+P[-1]) )
        calls[-1].multicall.get_dqmf_all_results(self.run_spec,plot+'%s/CellOccupancyVsEtaPhi_%s_5Sigma'%(sam,P[:-1]+s2+P[-1]) )
                    
      calls[-1].start()

    DB = []


    for p,P in enumerate(self.Parts):
      c = calls[p] 
      c.join()
      d = DETlist.index(P[:-1])
      AC = P[-1]
      for i,I in enumerate(c.multicall._MultiCall__call_list):
        plot = I[1][-1]
        sam = Si[plot.split('/')[4][-1]] 
        par = (d,AC,sam)      
        V = [] 
        cmd = 'select ONL_ID,ETA,PHI from LARID where DET=%d and AC=%d and SAM=%d and (' % (d,ACmap2[AC],sam)
        r = c.results[i][N]
        K = r.keys()
        yellow_bins = 0
        red_bins = 0
        bad_cells = 0
   
        #print "DEBUG     --->",K,"<br>"
        for k in K:
          #print k, r[k],"<br>"
          if 'NRedBins' in k:
            red_bins = r[k]
            bad_cells += red_bins
            continue
          elif 'NYellowBins' in k:
            yellow_bins = r[k]
            bad_cells += yellow_bins
            continue
          elif 'NBins' in k: 
            continue
          #elif 'eta' in k:
          elif 'OSRatio' in k:  
            m = RE.search(k).groupdict()
          elif 'axis' in k: 
            m = RE2.search(k).groupdict()
          else:
            continue  
          if 'CellOccupancy' in plot and r[k]<50:
            bad_cells -= 1
            continue              
          elif 'DB' in plot and r[k]<2000:
            bad_cells -= 1
            continue
        
          eta = float(m['eta'])
          if d==1 and sam in [1,2]: eta+=0.0005
          phi = float(m['phi'])
          if AC=='A': phi-=dd[sam]
          else:       phi+=dd[sam] 
          limits = (eta-de[d][sam],eta+de[d][sam],phi-dp[d][sam],phi+dp[d][sam])
          V.append([limits,r[k],k])
          cmd += '(ETA between %f and %f and PHI between %f and %f) or ' % limits
          #print P[:-1]+AC,sam,k,r[k],"<br>"
          
        #if 'CellOccupancy' in plot and bad_cells != 0:
        #  print "<br>", plot, "<br>"
        #  print "Bad:", bad_cells, "<br>"
        #  print len(V), "<br>"
        if ('CellOccupancy' in plot or 'DB' in plot) and len(V) != bad_cells:
          print "<br> WARNING!  there is a discrepancy between the number of red and yellow bins (",int(bad_cells),") and the cells downloaded to be examined (",len(V),") for the plot:", plot
          
        # this I am not sure what it does yet
        if 'or' in cmd:
          DB.append( db(cmd.rstrip(' or ')+')',plot,par,V) )
          DB[-1].start()

      
    self.Chans = {}
    for s in DB:
      s.join()
      for r in s.R:
        id = r[0]
        com = []
        for v in s.V:
          if r[1]>v[0][0] and r[1]<v[0][1]:
            if r[2]>v[0][2] and r[2]<v[0][3]: com.append([v[1],v[2]]) 
        if not self.Chans.has_key(id): self.Chans[id] = [[],[],[],[]]
        for a in com:
          if 'CellOccupancy' in s.plot:
            if 'hiEth' in s.plot:
              self.Chans[id][0].append( [a[0],comment1,s.plot] )
            if '5S' in s.plot:
              self.Chans[id][1].append( [a[0],'Nevts&gt;5&#963;',s.plot] )
          if 'DB' in s.plot:
            self.Chans[id][2].append( [a[0],'DB noise deviation',s.plot] )
          if 'Qth' in s.plot:
            self.Chans[id][3].append( [a[0]*100,comment2,s.plot] )
 
########################################################################
class GetSporadic(Thread):
  def __init__(self,run_spec,Parts):
    Thread.__init__(self)
    self.run_spec = run_spec
    self.Parts = Parts
  def run(self):

    N = self.run_spec['low_run'] 
    RE0 = re.compile(r'FT(?P<FT>\S+)Sl(?P<SL>\S+)Ch(?P<CH>\S+)')
    RE1 = re.compile(r'\((?P<LB>\S+)\.0*\)')
    calls = []
    for P in self.Parts:
      calls.append( mc3(P,self.run_spec) )
      calls[-1].multicall.get_dqmf_sub_assessments(self.run_spec,'LAr/'+P+'/Noise/Single_Cells/Non_Gaussian_Tails')
      calls[-1].start()

    self.Chans = {}
    LBs = {}

    for p,P in enumerate(self.Parts):
      calls[p].join()
      plot = 'LAr/'+P+'/Noise/Single_Cells/Non_Gaussian_Tails/'
      for h in calls[p].h2p:
        m = RE0.search(h[0].split('_')[0]).groupdict()
        SL = int(m['SL'])
        FT = int(m['FT'])
        CH = int(m['CH'])
        d = DETlist.index(P[:-1])
        id = get_id((d,ACmap2[P[-1]],FT,SL,CH))
        dev = 0.
        if h[1].has_key('SideBands'):
       
          if 'EN' in plot+h[0]:
            title = "E>5-15GeV"
          else:
            title = "Qfac>4000"            
          dev = float(h[1]['SideBands'])
          if str(dev)=='nan': dev=-0.01
        elif h[1].has_key('NBins'): 
          title = 'LB: '
          dev = int(h[1]['NBins'])
          LB = []
          for lb in h[1].keys():
            if 'NBins' in lb: continue
            m = RE1.search(lb).groupdict()
            LB.append( int(m['LB']) )
            title+=m['LB']+' '
          LBs[id] = LB
        if not self.Chans.has_key(id): self.Chans[id] = [ [],[],[] ]
        if 'ENLB' in h[0]: self.Chans[id][1].append( [dev,title,plot+h[0]] )
        elif 'EN' in h[0]: self.Chans[id][0].append( [dev,title,plot+h[0]] )
        else: self.Chans[id][2].append( [100*dev,title,plot+h[0]] )

    """
    f = file('reports/%s.txt'%(N),'r')
    while not 'Remove' in f.readline(): pass
    removed_LBs = []
    b = f.readline().split()
    for i in b:
      if '-' in i:
        a = i.split('-')
        for j in range(int(a[0]),int(a[1])+1): removed_LBs.append( int(j) )
      else: removed_LBs.append( int(i) )

    from copy import copy
    for id in LBs.keys():
      LB = LBs[id]
      LB.sort()
      LB1 = copy(LB)
      #for lb in LBs[id]:
      #  if lb in removed_LBs: LB1.remove(lb)
      #if len(LB1)==0: self.Chans.pop(id) 
      #else:
      for ch in self.Chans[id]: ch.append(str(LB1))
    """          

########################################################################
def NoisyLArCells(run_spec,opts,Sides,rawchannels,calocells,sporadic,larcellempty,clusters,Clusters,Duplicated,cluster_threshold,Clusters_found,lce_inputfile,expert,cursor,url,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,beambkg,isexpress,cluster_E):

 
  # Queries 
  DETs = range(4)
  if opts.has_key('DET'):
    if clusters: 
      if int(opts['DET'])==0: DETs = [0]
      else: DETs = [1,2,3]
    else: DETs = [int(opts['DET'])]
  sides = ['A','C']
  if opts.has_key('AC'): 
    if clusters and 0 in DETs: pass
    else: sides = [ACmap[opts['AC']][0]]

  lce_exists = True
  lce_message = False
  try:open(lce_inputfile)
  except IOError:
    print '<h3><font color=red>WARNING:  LArCellEmpty file not found!'
    lce_exists = False
    lce_message = True
  if lce_exists:  
    size = os.path.getsize(lce_inputfile)
    if (size < 1000):
      print '<h3><font color=red>WARNING:  LArCellEmpty file is very small!'
      lce_message = True
  if not larcellempty:
    print '<h3><font color=red>WARNING:  LArCellEmpty channels were not retrieved!'
    lce_message = True
  if lce_message and (not isexpress): print 'This is however not problematic for the bulk, as this output is not used for flagging proposal.</font></h3>'
  else: print '</font></h3>'

  for d in DETs:    
    for AC in sides:      
      Parts.append( DETlist[d]+AC )      
  if 'EMECC' in Parts or 'HECC' in Parts or 'FCALC' in Parts: Sides.append('ECC')
  if 'EMBA'  in Parts or 'EMBC' in Parts: Sides.append('BAR')
  if 'EMECA' in Parts or 'HECA' in Parts or 'FCALA' in Parts: Sides.append('ECA')

  # fetch DQ results
  jobs = [0,0,0,0]
  if rawchannels: raw = GetRawChannels(run_spec,Parts); raw.start(); jobs[0] = raw
  if calocells: calo = GetCaloCells(run_spec,Parts); calo.start(); jobs[1] = calo
  if sporadic: spo = GetSporadic(run_spec,Parts); spo.start(); jobs[2] = spo
  if larcellempty: lce = GetLArCellEmpty(cluster_threshold,lce_inputfile); lce.start();jobs[3] = lce
  if clusters: clus = GetClusters(run_spec,Sides,Clusters,Duplicated,cluster_threshold,cluster_E); clus.start()


  # fetch badchannels
  Bad = [{},{},{},{}]
  run = int(run_spec['low_run'])
  if run>0:
    b_conn = sqlite3.connect(dbfile)
    b_cursor = b_conn.cursor()
    for u,upd in enumerate(['1','4']):
      for b,bad in enumerate(['BadChannels','MissingFEBs']):
        b_cursor.execute('select status from %s where %d>=since and %d<=until'%(bad+'UPD'+upd,run,run))
        for d in b_cursor.fetchall(): Bad[2*u+b].update(eval(str(d[0])))
    b_conn.close()

  # Get results
  R = []
  Chans = set([])
  for i,j in enumerate(jobs):
 
    if j==0: continue
    j.join()
    try: j.Chans.keys()
    except:
      print '<h3><font color=red> Histograms are not found.  Please verify that the plots in the webdisplay exist. </font></h3>'
      sys.exit()
    C = j.Chans.keys()
    for ch in C:Chans.add(ch)

  R1 = []
  if len(Chans)>0:
    cmd = 'select '+','.join(cols[:expert])+',FEB_ID from LARID where ONL_ID in ('    
    for ch in Chans: cmd += '%d,'%(ch)   
    cursor.execute(cmd.rstrip(',')+')')   
    R1 = cursor.fetchall()
 
  R2 = []
  for r in R1:
    id = r[1]
    feb_id = r[-1]
    upd1=[0,0] 
    if Bad[0].has_key(id): upd1[0] = Bad[0][id] 
    if Bad[1].has_key(feb_id): upd1[1] = Bad[1][feb_id]
    upd4=[0,0] 
    if Bad[2].has_key(id): upd4[0] = Bad[2][id] 
    if Bad[3].has_key(feb_id): upd4[1] = Bad[3][feb_id]
    i = list(r[:-1])+[ upd1,upd4, [[],[],[],[]] , [[],[],[],[]] , [[],[],[]] ,[[],[],[],[]] ,-1 ]
    for j,J in enumerate(jobs):
      if J:
        if J.Chans.has_key(id):
          i[expert+2+j] = J.Chans[id]
          #print "JL DEBUG",j,J,id,i,"<br><br>"
    R2.append(i)

  if not clusters:
    print_rows.PrintRows(beambkg,clusters,url,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,R2)

  else:

    clus.join()
    from math import pi,sqrt
    R_used = len(R2)*[0]

    for cl in Clusters:
      R3 = []
     
      clusterType=cl[3].split("/")[3].split("Topo")[0]
      for i in R2:
        eta = i[8]
        phi = i[9]
        det = i[2]
        sampling = i[7]
       
        #print "  DEBUG Cells",clusterType,i[1],i[2],i[3],i[7],"<br>"
        #print "DEBUG CLUSTERS",cl,"<br>"
        ##### WARNING: issue with EMTopo
        #: use CaloTopo Only ##### 
        #if clusterType=="EM":continue
        ##### Match EM clusters with EMB,EMEC cells #####
        ##### Match TopoClusters with HEC,FCAL cells #####
        if clusterType=="EM" and not(det==0 or det ==1):continue
        if clusterType=="Calo" and not(det==2 or det==3):continue
       
        if i[3]>0: phi-=dd[i[7]]
        else:      phi+=dd[i[7]]
        deta = abs( eta-cl[0] )
        dphi = abs( phi-cl[1] )
        if dphi>2*pi: dphi-=2*pi
        dR = sqrt(deta*deta+dphi*dphi)
       
        if dR<cl[2]:
          Clusters_found[ Clusters.index(cl) ] += 1
          R_used[ R2.index(i) ] += 1
          i[-1] = dR
          R3.append(i)
 
      if cl[4][0] == "Y":Class = GetClass("Warning")
      if cl[4][0] == "R":Class = GetClass("Error")

      if len(R3)>0:
        print '<tr><td colspan="%d"><font style="color:#FFFFFF">%s</font>&nbsp;:&nbsp;<a class="det%d" href="%s/%s">%s &nbsp; %d</a></td></tr>' % (expert+7,cl[4],Class,url,cl[3],cl[3].split('Clusters')[0].split("/")[3],cl[5])

        print_rows.PrintRows(beambkg,clusters,url,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,R3)

    print_top.PrintFields(expert)
    R4 = []

    for i in range(len(R2)): 
      if R_used[i]==0: R4.append( R2[i] )
    print_rows.PrintRows(beambkg,clusters,url,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,R4)

