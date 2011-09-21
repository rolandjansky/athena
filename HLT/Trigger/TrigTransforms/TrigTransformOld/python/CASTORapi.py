# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### CASTORapi.py
###
### last changed by $Author: hegbi $
### $Date$
### $Revision#
###
### author: Luc Goossens
### 

# 2011-07-11 Graeme Stewart <graeme.andrew.stewart@cern.ch>
#  * Improved exception handling to print errors

import sys, time, logging, logging.handlers
from commands import getstatusoutput as gso


castor_log = logging.getLogger("CASTORapi")
handler    = logging.handlers.RotatingFileHandler('castorApi.log','w',0)
handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s %(levelname)s %(message)s'))
castor_log.addHandler(handler)
###########

def do(cmd,exit=False,prcmd=True,prout=True) :
 if prcmd : castor_log.info(cmd)
 s,o = gso(cmd)
 if prout and len(o) < 1000 : castor_log.debug(o)
 if s : 
   castor_log.error( "(gso) %s returned non-zero exit code %d" % (cmd,s) )
   castor_log.error( "(s,o) = ", s , o)
   if exit : 
     castor_log.info( "-> exiting")
     sys.exit(s)
 return s,o


###########                      CASTOR interface

def castor_setcomment(fullpath,comment) :
  s,o = do("nssetcomment %s %s" % (fullpath,comment),exit=False,prcmd=True,prout=True)
  return s
  
  
def castor_getcomment(fullpath) :
  s,o = do("nsls -d --comment %s" % fullpath,exit=False,prcmd=True,prout=True)
  words = o.split()
  if len(words) < 2 :
    return ""
  comment = words[1]
  if not (comment.isalnum()) :
    return ""
  else :
    return comment
  
  
def castor_listdir(fullpath) :
  s,o = do("nsls %s" % fullpath,exit=False,prcmd=True,prout=False)
  if s == 0 :
    return o.split()
  else :
    return 'error'


def castor_mkdir(fullpath) :
  cmd = "rfmkdir -p %s" % fullpath
  castor_log.info(cmd)
  s,o = gso(cmd)
  if s == 256 and o.find('File exists') > 0 :
    # intercept no-op
    castor_log.debug( "rfmkdir: dir exists, nothing was done" )
    return 0
  else :
    if s != 0 : 
      castor_log.error( "(gso) %s returned non-zero exit code %d" % (cmd,s) )
      castor_log.error("(s,o) = ", s , o )
    return s


def castor_stagein(svcclass,fullpath,localname) :
  cmd="export STAGE_SVCCLASS=%s; rfcp %s %s" % (svcclass,fullpath,localname)
  s,o = do(cmd,exit=False,prcmd=True,prout=True)
  return s
  
  
def castor_stageout(svcclass,localname,fullpath) :
  cmd="export STAGE_SVCCLASS=%s; rfcp %s %s" % (svcclass,localname,fullpath)
  s,o = do(cmd,exit=False,prcmd=True,prout=True)
  return s
  
  
def castor_ismigrated(fullpath):
  cmd="nsls -l %s" % fullpath
  s,o = do(cmd,exit=False,prcmd=True,prout=True)
  if s == 0 and o[0]=='m' :
    return True
  elif s == 0 and o[0]=='-' :
    return False
  else :
    return 'error'
  
  
def castor_rmemptydir(fullpath,prout=True) :
  # currently there is no easy way to make this fail if the dir is not empty
  # nsrmdir is in the pipe-line
  s,o = do("nsrm -R %s " % fullpath,exit=False,prcmd=True,prout=prout)
  return s


def castor_rmdir(fullpath,prout=True) :
  s,o = do("nsrm -R %s " % fullpath,exit=False,prcmd=True,prout=prout)
  return s


def castor_rmfile(fullpath,prout=True) :
  s,o = do("nsrm %s " % fullpath,exit=False,prcmd=True,prout=prout)
  return s


def castor_info(fullpath,prout=True):
  # returns (age in days , elts, comment)
  s,o = do("nsls -l -d --comment %s" % fullpath,exit=False,prcmd=True,prout=prout)
  if s == 0 :
    tokens=o.split()
    elts=int(tokens[1])
    try :
        cmt=str(tokens[9])
        if not (cmt.isalnum()) :
            cmt=''
    except Exception, x:
        print 'Exception parsing nsls in castor_info: %s' % x
        cmt=''
    month=tokens[5]
    day=tokens[6]
    yearOrTime=tokens[7]
    months={'Jan':1,'Feb':2,'Mar':3,'Apr':4,'May':5,'Jun':6,'Jul':7,'Aug':8,'Sep':9,'Oct':10,'Nov':11,'Dec':12}
    month=months[month]
    day=int(day)
    
    if yearOrTime.count(':') > 0 :
      # it's a time, figure out correct year
      thisyear=time.localtime()[0]
      if time.mktime((thisyear,month,day, 0,0,0, 0,0,0)) > time.time() :
        year = thisyear - 1
      else :
        year = thisyear
    else :
      year = int(yearOrTime)
    
    epoch = time.mktime((year,month,day, 0,0,0, 0,0,0))
    deltadays = int(time.time()) - epoch
    return int(deltadays / (24 * 3600)), elts, cmt

  else :
    return None,None,None  

  
#### from ps_REPLICATE

#def castor_stagerGet(pfns,sourcesvcclass,destsvcclass) :
def castor_stagerGet(pfns,destsvcclass) :
  # returns exitcode, successes 
  
  # assemble CASTOR command
  comstr  = 'export STAGE_HOST=castoratlas ; '
#  comstr += 'export STAGE_SVCCLASS=%s ; ' % sourcesvcclass
  comstr += 'stager_get -S ' + destsvcclass
  for pfn in pfns :
    comstr += ' -M %s ' % pfn    
    
  castor_log.info(comstr)
  # run CASTOR command
  (s,o) = gso(comstr)
    
  successes = []
  for line in o.split('\n') :
    if line.find('SUBREQUEST_READY') > -1 :
      pfn = line.split()[0]
      lfn = pfn.split('/')[-1]
      successes.append(lfn)
      
  return s,successes 
  
  
#### from ps_DELTMPDS

def castor_onTape(fullname):
  # return boolean
  cmd = 'nsls -l ' + fullname
  s,o = gso(cmd)
  return o and (o[0] == 'm')


def castor_stagerrm_files(pfnames) :
  # return ok | error
  r = 'ok'
  # assemble CASTOR command
  cmd  = 'export STAGE_HOST=castoratlas ; '
  cmd += 'stager_rm -S t0merge '
  for pfn in pfnames :
    cmd += '-M %s ' % pfn    
  s,o = gso(cmd)
  if s : r = 'error'  
  return r

  
  
#### from updateMon


def castor_poolInfo(pool):
  #returns tot,used,hosts,pend,run,recallq,migrq,taperate,tapefrate
  
  # occupancy
  s,o = gso("staggso " + pool + " -si")
  castor_log.debug(o)
  try :  
    # e.g. "POOL t0atlas          CAPACITY 186.42T    FREE  39.61T(21%)"
    tot = int(float(o.split()[3].replace('T','')))
    used = tot - int(float((o.split()[5].replace('T',' ')).split()[0]))
  except : 
    tot = 0
    used = 0 

  # nr of hosts
  s,o = gso("lemon-cli -s --script-mode -m 6254 -n c2atlas.%s_data" % pool)
  castor_log.debug(o)
  try :    hosts = int(float(o.split()[-4]))
  except : hosts = 0

  # active/queued requests
  s,o = gso("lemon-cli -s --script-mode -m 6259 -n c2atlas_%s_data" % pool)
  castor_log.debug(o)
  try :    pend = int(float(o.split()[-2]))
  except : pend = 0
  try :    run = int(float(o.split()[-3]))
  except : run = 0
  
  # tape recall queue
  s,o = gso("lemon-cli -s --script-mode -m 6270 -n c2atlas.%s_data" % pool)
  castor_log.debug(o)
  try :    recallq = int(float(o.split()[-1]))
  except : recallq = 0

  # tape migration queue
  s,o1 = gso("lemon-cli -s --script-mode -m 6247 -n c2atlas.%s_data" % pool)
  castor_log.debug(o1)
  s,o2 = gso("lemon-cli -s --script-mode -m 6248 -n c2atlas.%s_data" % pool)
  castor_log.debug(o2)
  try :    migrq = int(float(o1.split()[-1])+float(o2.split()[-1]))
  except : migrq = 0

  # tape writing rate
  s,o = gso("lemon-cli -s --script-mode -m 6266 -n c2atlas.%s_data" % pool)
  castor_log.debug(o)
  try :    taperate = int(float(o.split()[-1]))
  except : taperate = 0
  try :    tapefrate = int(float(o.split()[-2]))
  except : tapefrate = 0
  
  return tot,used,hosts,pend,run,recallq,migrq,taperate,tapefrate


###########                      relrfcp interface


import os

try :
 import cx_Oracle
except ImportError:
 castor_log.error( "relrfcp : FAILED LOADING cx_Oracle")


from subprocess import * 

def gsowt (cmd, timeout=None):
  p1 = Popen(args=cmd,shell=True,stdout=PIPE,stderr=STDOUT, preexec_fn = lambda : os.setpgid(0,0))
  # os.setpgid(0,0) -> create new process group
  pid1 = p1.pid
  #print pid1
  if timeout :
    killcmd="sleep %d ; kill -9 -%d" % (timeout,pid1)
    # note negative pid to kill process group !
    #print killcmd
    p2 = Popen(killcmd,shell=True,stdout=PIPE,stderr=STDOUT, preexec_fn = lambda : os.setpgid(0,0))
    pid2=p2.pid
    
  stdout,stderr = p1.communicate()
  # need to kill sleep + kill process
  if timeout :
    killcmd="kill -9 -%d" % pid2
    Popen(killcmd,shell=True,stdout=PIPE,stderr=STDOUT)
    
  return p1.returncode,stdout


 
 
def getConnection() :
  try :
    pdbpw=os.environ['PDBPW']
    pdbuser=os.environ['PDBUSER']
    pdbservice=os.environ['PDBSERVICE']
  except Exception, x:
    castor_log.warnings( "relrfcp : could not set up connection env vars: %s" % x)
    return None
  try:  
    return cx_Oracle.connect(pdbuser+'/'+pdbpw+'@'+pdbservice)
  except Exception, x:
    castor_log.error("relrfcp : could not connect to DB: %s" % x)
    return None


def log(source,dest,retrynr,exitcode,startepoch, endepoch,stdboth, filesize, server, 
        transfertime, conn=None, eacro='' ,type = '',filetype='') :
 
  if len(stdboth) > 990 :
      stdboth = stdboth[:495]+'.....'+stdboth[-495:]
  sql ="insert into cpattempt (source,dest,retrynr,exitcode,erroracronym,startepoch, \
          endepoch,stdboth,filesize,server,transfertime,type,filetype)"
  sql+=" values (:source,:dest,:retrynr,:exitcode,:eacro,:startepoch, :endepoch,:stdboth,\
          :filesize, :server, :transfertime, :type, :filetype)"
  varmap = {'source':source,'dest':dest,'retrynr':retrynr,'exitcode':exitcode,'startepoch':startepoch,
              'eacro':eacro, 'type':type, 'endepoch':endepoch,'stdboth':stdboth, 'filesize':filesize, 
              'server':server,'transfertime':transfertime, 'filetype':filetype}
  try :
    
    if not conn :
      conn=getConnection()
      if conn :
        castor_log.info("connection succeeded")
        cur=conn.cursor()
        castor_log.debug("executing...")
        castor_log.debug(sql)
        castor_log.debug(varmap)
        cur.execute(sql,varmap)
        conn.commit()
        conn.close()
        castor_log.info("connection closed")
      else :
        print sql
        print varmap
        
    else :
      cur=conn.cursor()
      castor_log.debug("executing...")
      castor_log.debug(sql)
      castor_log.debug(varmap)
      cur.execute(sql,varmap)
      conn.commit()
  except Exception, x:
    castor_log.error("relrfcp : could not log rfcp result in DB: %s" % x)
    castor_log.error(sql)
    castor_log.error(varmap)
     
 

def rfcp(src,dest,maxattempt=30,maxtime=20000,minp=True,perm='750',sleepmul=5,
         db=True,usesafe=False,safeto=900,
         svcclass='t0atlas',fileclass=None,type='UNKNOWN',filetype='UNKNOWN',extractServer=False) :
  # maxtime in secs
  # minp = create parent dirs 
  # type in (TOTMP, FROMTMP, TOPERM, FROMPERM)
  # filetype in (RAW, ESD, ...)
  # returns (erroracronym,tottime,lastcptime,allattempts)
 
  # usesafe keyword is obsolete/deprecated

  start=time.time()
  allattempts=[]
  lastacro=''
  lastcptm=0
  s=0 ; o =''
  eacro=''
  if extractServer :
    setenvstr='export RFIO_TRACE=3 ; '
  else :
    setenvstr=''
  #setenvstr+='export STAGE_SVCCLASS=%s ; ' % svcclass 
  setenvstr+='export STAGE_SVCCLASS=%s ; umask 7 ; ' % svcclass 
     
  # castor source and/or dest ?
  srcOnCastor =  (src[:8] == '/castor/')
  destOnCastor = (dest[:8] == '/castor/') 
   
  # if minp rfmkdir
  if minp and dest.find('/') > -1 :
    destdir=dest[:dest.rfind('/')]
    #com='rfmkdir -m '+perm+' -p '+destdir
    com='rfmkdir -p '+destdir
    if fileclass :
      com += ' ; nschclass '+fileclass+' '+destdir
    castor_log.info(com)
    #print time.ctime()
    (s,o)=gsowt(com)
    castor_log.debug(s)
    castor_log.error(o)
    #totout+=o+'\n'
    
  # if source is castor test for staged in
  if srcOnCastor :
    com=setenvstr + "stager_qry -M "+src
    castor_log.info(com)
    #print time.ctime()
    (s,o)=gsowt(com )
    castor_log.debug(s)
    castor_log.error(o)
    #classify
    if o.find('STAGED') > -1 or o.find('STAGEOUT') > -1 or o.find('CANBEMIGR') > -1 :
      stageStatus = 'ONSTAGER'
    elif o.find('No such file or directory') > -1 :
      stageStatus = 'NOTONSTAGER'
    else :
      stageStatus = 'UNKNOWN'
  
#    if stageStatus == 'NOTONSTAGER' :
#      while retries < maxattempt and time.time() - start + sleeptime < maxtime :
#        if db : log(src,dest,0,100000,0,0,o,0,'',0,None,'NOTONSTAGER',type,filetype)
#        tottime=int(1000*(time.time()-start))
#        allattempts.append((s,0,o,0,0))
#      return ('NOTONSTAGER',tottime,0,allattempts)
    
  sleeptime=0
  retries=0
  while retries < maxattempt and time.time() - start + sleeptime < maxtime :
    if sleeptime : 
      castor_log.info( "sleeping %d secs ..." % sleeptime )
      time.sleep(sleeptime)
     
    #attempt copy ...
    cpstart=time.time()
    rfcpcom= setenvstr + ' rfcp '+src+' '+dest   
     
    castor_log.info(rfcpcom)
    (s,o)=gsowt(rfcpcom,timeout=safeto)
    castor_log.debug(s)
    castor_log.error(o)
    cpend=time.time()
    lastcptm=int(1000*(cpend-cpstart))
    # classify
    if s == 0 :
      # TODO check here if it really was a success ? e.g. compare file size
      eacro='OK'
    elif s == 35072 or s == -9 :
      eacro='SAFETIMEOUT'
    elif (o.find('ermission denied') != -1)  :
      eacro='NOPERMISSION'
    elif (o.find('o such file or directory') != -1) :
      eacro='NOTEXIST'
    elif (o.find('Device or resource busy') != -1) :
      eacro='BUSY'   
    elif (o.find('Communication error') != -1) :
      eacro='COMERROR'   
    elif (o.find('instead of 0 bytes') != -1) :
      eacro='ZEROBYTES'   
    elif (o.find('close target') != -1) :
      eacro='CLOSETARGET'   
    elif (o.find('Timed out') != -1) :
      eacro='TIMEOUT'   
    elif (o.find('Internal error') != -1) and retries >= 2 :
      eacro='INTERNAL3'   
    elif (o.find('Internal error') != -1) :
      eacro='INTERNAL'   
    elif (o.find('Unable to receive header') != -1) :
      eacro='HEADER'   
    elif (o.find('Address already in use') != -1) :
      eacro='ADDRESS'   
    elif (o.find('Host not known') != -1) :
      eacro='HOST'   
    elif (o.find('Connection closed by remote end') != -1) :
      eacro='REMOTECLOSE'   
    else :
      eacro='ERROR'
        
    #extract server
    server=''
    if extractServer :
      p1=o.find('lxfs')
      if p1 != -1 : server=o[p1:o.find('ch',p1)+2] 
      # filter out debug garbage again and replace o
      out=''
      for line in o.split('\n') :
        if line.find('rfio:') == -1  and line.find('getifnam') == -1 and line.find('trace') == -1 : 
          out += line + '\n'
      o=out
    
    # print s,o after filtering
    castor_log.debug(s)
    castor_log.error(o)
       
    # filter out killMe/saferfcp logging and replace o
    out=''
    for line in o.split('\n') :
      if line.find('killMe') == -1  and line.find('saferfcp') == -1 and line.find('kill') == -1 :
        out += line + '\n'
    o=out
     
    # if success recover filesize and transfertime from stdout
    sz=0 ; xtime=0
    if s==0 :
      try : sz=int(o.split()[0])
      except :  sz=0
      try : 
          xtime=int(o[o.find('bytes in')+8:o.find('seconds')])
          if xtime == 0 : xtime = 1
      except Exception, x:
          print 'Exception parsing rate from command output: %s'% x
          xtime=0
        
    allattempts.append((s,retries+1,o,int(cpstart),int(cpend)))
        
    # if db is set log into db
    if db : 
      log(src,dest,retries+1,s,int(cpstart),int(cpend),o,sz,server,xtime,None,eacro,type,filetype)
    
    # return for success and fatal errors
    if eacro=='OK' or eacro=='NOPERMISSION' or eacro=='NOTEXIST' or eacro=='ZEROBYTES' or eacro=='INTERNAL3' :
      return (eacro,int(1000*(time.time()-start)),lastcptm,allattempts) 
    
    # if dest castor and BUSY error attempt stager_rm
    if eacro=='BUSY' :
      com= setenvstr + 'stager_rm -M '+dest + ' ; nsrm ' + dest
      castor_log.info(com)
      (s,o)=gsowt(com)
      castor_log.debug(s)
      castor_log.error(o)
        
    # prepare for retry  
    retries += 1
    if sleeptime : sleeptime = min(10000,sleeptime * sleepmul)
    else : sleeptime = 60
  
  return ('ABORTED',int(1000*(time.time()-start)),lastcptm,allattempts)
     
    
    
if __name__ == '__main__':
  if sys.argv[1] == 'rfcp' :
    src=sys.argv[2]
    dst=sys.argv[3]
    svcclass=sys.argv[4]
    type=sys.argv[5]
    filetype=sys.argv[6]
    safeto=int(sys.argv[7])
    maxattempt=int(sys.argv[8])
    print src,dst
    r = rfcp(src,dst,svcclass=svcclass,type=type,filetype=filetype,safeto=safeto,maxattempt=maxattempt)
    print r
  else :
    print "usage CASTORapi.py rfcp src dst svcclass type filetype safeto maxattempt"
    

