#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## CreateSQLiteJobDatabase.py - 
# $Id$

### INCLUDED MODULE 
from sqlalchemy import *

import sys
import datetime
import time
import re
import json
import subprocess
import logging
import os

### Setup global logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M',
                    filename='./CreateSQLiteJobDatabase.log',
                    filemode='w')
console = logging.StreamHandler()
console.setLevel(logging.INFO)
formatter = logging.Formatter('%(levelname)-8s %(message)s')
console.setFormatter(formatter)
logging.getLogger('').addHandler(console)

def pwd():
    Proc = subprocess.Popen('pwd', shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
    Out = (Proc.communicate()[0])[:-1]     
    return Out


def GetReleaseSetup():
    
    atlas_base_dir = os.environ['AtlasBaseDir']

    if 'AtlasPatchVersion' in os.environ:
        current_nightly = os.environ['AtlasPatchVersion']
    elif 'AtlasArea' in os.environ:
        current_nightly = os.environ['AtlasArea'].split('/')[-1]
    elif 'AtlasVersion' in os.environ:
        current_nightly = os.environ['AtlasVersion']
        
    if "rel" not in current_nightly:
        platform = os.environ['GEANT4'].split('/')[-1]
        setup="%s,%s"%(platform.replace("-", ","),current_nightly)
    else :
        latest_tag = "latest_copied_release"
        if atlas_base_dir.split('/')[1] == 'cvmfs':
            latest_tag = "latest"
            latest_nightly  = (os.environ['AtlasBaseDir'].split('rel')[:-1])[0]+latest_tag 
        elif atlas_base_dir.split('/')[1] == 'afs':  
            latest_nightly  = (os.environ['AtlasArea'].split('rel')[:-1])[0]+latest_tag 

        latest_nightly  = subprocess.Popen(['/bin/bash', '-c',"ls -l "+latest_nightly], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[-1]
    
        release  = os.environ['ATLAS_RELEASE_BASE']
        if 'afs' in release.split('/'):
            release = release.split('/')[-1]         
        elif 'cvmfs' in release.split('/'):
            release = release.split('/')[-2]         

        platform = os.environ['GEANT4'].split('/')[-1]

        setup="%s,%s,%s"%(release,platform.replace("-", ","),current_nightly)

    logging.info("The environment is "+setup)

    return setup


def main():
    from optparse import OptionParser
    
    parser=OptionParser(usage="\n ./CreateSQLiteJobDatabase.py \n")

#    parser.add_option("-n","--nightly",type="string",dest="nightly",default="rel_0",help="specify nightly") # echo $AtlasVersion  
#    parser.add_option("-r","--release",type="string",dest="release",default="dev"  ,help="specify release") # basename ${ATLAS_RELEASE_BASE}    
#    parser.add_option("-r","--platform",type="string",dest="platform",default="x86_64-slc6-gcc49-opt"  ,help="specify platform") #basename $ROOTSYS 

    parser.add_option("-j","--jobname",type="string",dest="jobname",default="test"  ,help="You must specify the job name") #basename $ROOTSYS 
    parser.add_option("-p","--path",type="string",dest="path",default="./"  ,help="You must specify the job name") #basename $ROOTSYS 


    (options,args)=parser.parse_args()

    mysetup = GetReleaseSetup() 
    mypwd   = pwd()
    
    # 21.0.X-VAL,x86_64,slc6,gcc49,opt,rel_0 

    desc = mysetup.split(',')
    release  = desc[0]
    platform = desc[1]+'_'+desc[2]+'_'+desc[3]+'_'+desc[4]
    nightly  = desc[5]

    path    = options.path
    jobname = options.jobname


    import glob
    StepLogFiles  = glob.glob(path+'log.*')

    for logFile in StepLogFiles:
        step = logFile.split('log.')[1]

    long_name = jobname+"_"+platform+"_"+release+"_"+nightly    


################################################################    

############ CREATE DATABASE
    db = create_engine('sqlite:///'+long_name+'.db')
    db.echo = False  # Try changing this to True and see what happens
    metadata = MetaData(db)

    job_t = Table('job', metadata,
                  Column('jid'             ,  Integer, primary_key=True),   #(1,                                                      
                  Column('name'            ,  String(60)  ),                # u'calo_reco_mc15_ttbar_valid_13tev_25ns_mu00_to_mu40',  
                  Column('step'            ,  String(20)  ),                # u'RAWtoESD',                                            
                  Column('platform'        ,  String(30)  ),                # u'x86_64-slc6-gcc49-opt',                               
                  Column('release'         ,  String(30)  ),                # u'rel_0',                    
                  Column('date'            ,  DATE 	  ),                # datetime.date(2016, 8, 27),                                           
                  Column('nightly'         ,  String(10)  ),                # u'rel_0',     
                  Column('exitcode'        ,  Integer     ),                # 1616876,                                                
                  Column('vmem_peak'       ,  Integer     ),                # 1616876,                                                
                  Column('vmem_mean'       ,  Integer     ),                # 1606616,                                                
                  Column('rss_mean'        ,  Integer     ),                # 971141,                                                 
                  Column('jobcfg_walltime' ,  Integer     ),                # 108007,                                                 
                  Column('jobstart'        ,  Integer     ),                # u'2016-08-28T03:50:48+0000',                            
                  Column('cpu_bmips'       ,  Integer     ),                # 10,                                                     
                  Column('cpu_res'         ,  Integer     ),                # u'rel_0/x86_64-slc6-gcc49-opt',                         
                  Column('cpu_model'       ,  String(60)  ),                # u'Intel(R)_Xeon(R)_CPU_E5-2630_v3_@_2.40GHz/20480_KB',  
                  Column('malloc'          ,  String(30)  ),                # u'libtcmalloc_minimal.so/ok',                           
                  Column('cost_perevt'     ,  Integer     ),                # 2, 1                                                    
                  Column('cost_onceperjob' ,  Integer     ))                # 8)                                                      


    job_t.create()
    
    component = Table('component', metadata,
                      Column('name',     String(80)  ),
                      Column('domain',   String(40)  ),
                      Column('stage',    String(10)  ),
                      Column('n',        Integer     ),
                      Column('cpu',      Integer     ),
                      Column('maxcpu',   Integer     ),
                      Column('wall',     Integer     ),
                      Column('vmem',     Integer     ),
                      Column('maxvmem',  String(10)  ),
                      Column('malloc',   Integer     ),
                      Column('maxmalloc',String(10)  ),
                      Column('jid', Integer, ForeignKey('job.jid'))
                      )
    component.create()

################################################################
    import calendar
    abbr_to_num = {name: num for num, name in enumerate(calendar.month_abbr) if num}
################################################################

    for logFile in StepLogFiles:
        step = logFile.split('log.')[1]
        exitcode="-1"

        try:
            infile = open(logFile)
        except:
            continue

        data = {}
        data["name"]     = jobname
        data["platform"] = platform
        data["nightly"]  = nightly
        data["release"]  = release
        data["step"]     = step
        
        #Read in data into dictionary                                
        my_domain_dict = {}
        my_domain_dict['AthAlgSeq']="master"
        my_domain_dict['RecoMemoryBegin']="master"
        my_domain_dict['RecoTimerBegin']="master"
        my_domain_dict['AthMasterSeq']="master"
        my_domain_dict['AthOutSeq']="master"
        my_domain_dict['AthRegSeq']="master"
        my_domain_dict['StreamAOD']="output"
            
        exitcode=-1
        timestamp=0
        #FIRST ITERATION THROUGH FILE TO INPUT JOB INFO AND GATHER DOMAIN INFORMATION
        for line in infile:
            if ' CET ' in line:
                timestamp=line.split()
                myYear    = timestamp[6]
                myMonth   = timestamp[2]
                myDay     = timestamp[3]

                myTime    = timestamp[4]
                myHour    = myTime.split(':')[0]
                myMinute  = myTime.split(':')[1]
                mySecond  = myTime.split(':')[2]

                myMonthNum =  abbr_to_num[myMonth]
                dt = datetime.datetime(int(myYear), int(myMonthNum), int(myDay), int(myHour), int(myMinute), int(mySecond))
                UTCtime = int((dt-datetime.datetime(1970,1,1)).total_seconds())
                data["jid"]=UTCtime

            if 'INFO leaving with code' in line:
                exitcode = line.split()[6].split(':')[0]
                data["exitcode"]=exitcode
            elif 'jobstart' in line:
                mydate = ((line.split()[4]).split('T')[0]).split('=')[1]
                mydateJ  =mydate.split('-')
                mydate = datetime.date(int(mydateJ[0]),int(mydateJ[1]),int(mydateJ[2]))
                        #Get domain definitions from logfile
                data["date"]=mydateJ

            if 'PerfMon domains :' in line:
                domains = line.split(':')[-1]
                domains = re.sub('[\[\]\',\n"]', '', domains).split(" ")
                domains = [x for x in domains if x is not None]
                domains.remove('')
                domains.remove('')

            if '::: domain [' in line and not 'WARNING' in line:
                specific_domain = [re.sub('[\]\[\n]','',line.split(' ')[-1])]
                nextline = next(infile)
                myalgs = nextline 
                myalgs = myalgs.split('algs:')[-1]
                myalgs = re.sub('[\[\]\',"]', '', myalgs).split()
                myalgs = [x for x in myalgs if x is not None or x is not '']
                 
                for myalg in myalgs:
                        my_domain_dict[str(myalg)]=str(specific_domain[0])


            if 'vmem_peak' in line :
                data["vmem_peak"]=(line.split()[3]).split('=')[1]
                data["vmem_mean"]=(line.split()[4]).split('=')[1]
                data["rss_mean"]=(line.split()[5]).split('=')[1]
                    
            elif 'jobcfg_walltime' in line :
                data["jobcfg_walltime"]=(line.split()[3]).split('=')[1]
                data["jobstart"]       =(line.split()[4]).split('=')[1]
                    
            elif 'cpu_bmips' in line :
                data["cpu_bmips"]=(line.split()[3]).split('=')[1]
                data["cpu_res"]=(line.split()[4]).split('=')[1]

            elif 'cpu_model' in line :
                data["cpu_model"]=(line.split()[3]).split('=')[1]
                    
            elif 'pycintex_vmemfix' in line :
                data["malloc"]=(line.split()[3]).split('=')[1]

            elif 'pmonsd_cost' in line :
                data["pmonsd_cost_onceperjob"]=(line.split()[3]).split('=')[1]
                data["pmonsd_cost_perevt"]    =(line.split()[4]).split('=')[1]

        ins = job_t.insert()
        result = db.execute(ins,
                            jid             =  data["jid"]                   ,
                            name            =  data["name"]                  ,
                            platform        =  data["platform"]              ,
                            release         =  data["release"]               ,
                            step            =  data["step"]                  ,
                            date            =  mydate                        ,
                            nightly         =  data["nightly"]               ,
                            exitcode        =  data["exitcode"]              ,  
                            vmem_peak       =  data["vmem_peak"]             ,  
                            vmem_mean       =  data["vmem_mean"]             ,
                            rss_mean        =  data["rss_mean"]              ,
                            jobcfg_walltime =  data["jobcfg_walltime"]       ,
                            jobstart        =  data["jobstart"]              ,
                            cpu_bmips       =  data["cpu_bmips"]             ,
                            cpu_res         =  data["cpu_res"]               ,
                            cpu_model       =  data["cpu_model"]             ,
                            malloc          =  data["malloc"]                ,
                            cost_perevt     =  data["pmonsd_cost_perevt"]    ,
                            cost_onceperjob =  data["pmonsd_cost_onceperjob"] )                           
        
        data["jid"]=result.inserted_primary_key[0]

        ### SECOND ITERATION TO GATHER JOB INFORMATION AND FILL DOMAIN DICTIONARY
        components = []
        infile = open(logFile)                    
        for line in infile:
            if '[evt]' in line and not '[total_for' in line and not 'collapsed' in line:
                pmon    =line.split()
                stage   =str(pmon[2])
                nevts   =int(pmon[3])
                cpu     =int(pmon[4])
                maxcpu  =int(str(pmon[5]).split("@")[0])
                wall    =-1
                vmem    =int(pmon[6])
                maxvmem =str(pmon[7])
                mlc     =int(pmon[8])
                maxmlc  =str(pmon[9])
                cname   =str(pmon[10])
                try:
                    domain=str(my_domain_dict[cname])
                except Exception:
                    domain="Unknown"

                new_component = { "jid"  :  data["jid"],
                                  "name"   :  cname         ,
                                  "step"   :  step         ,
                                  "domain" :  domain       ,
                                  "stage"  :  "evt"        ,
                                  "n"      :  nevts        ,
                                  "cpu"    :  cpu          ,
                                  "maxcpu" :  maxcpu       ,
                                  "wall"   :  wall         ,
                                  "vmem"   :  vmem         ,
                                  "maxvmem":  maxvmem      ,
                                  "malloc" :  mlc          ,
                                  "maxmalloc" :  maxmlc       ,
                                  "step"   :  step         }
                            
                components.append(new_component)
                            
                ins = component.insert()
                db.execute(ins,
                           jid             =  data["jid"]      ,
                           name            =  cname            ,
                           step            =  step             ,
                           domain          =  domain           ,
                           stage           =  stage            ,
                           n               =  nevts            ,
                           cpu             =  cpu              ,
                           maxcpu          =  maxcpu           ,
                           wall            =  wall             ,
                           vmem            =  vmem             ,
                           maxvmem         =  maxvmem          ,
                           malloc          =  mlc              ,
                           maxmalloc       =  maxmlc           )
                            
                            
            elif 'PMonSD' in line and ('[ini]' in line or '[1st]' in line or '[cbk]' in line or '[dso]' in line or '[fin]' in line or '[preLoadProxy]' in line or '[evt]' in line) and ('vmem_peak' not in line and 'jobstart' not in line and 'bmips' not in line and 'cpu_model' not in line and 'vmemfix' not in line and 'pmonsd_cost_perevt' not in line):
                pmon =line.split()
                
                stage=str(pmon[2])
                nevts=int(pmon[3])
                cpu  =pmon[4]
                vmem =pmon[5]
                mlc  =pmon[6]
                
                
                wall            =  -1
                maxcpu          =  -1           
                maxvmem         =  -1          
                maxmlc          =  -1           
                cname =str(pmon[7])

                            
                if '[---]' in line:
                    stage=str(pmon[2])
                    nevts=int(pmon[3])
                    cpu  =pmon[4]
                    wall =pmon[5]
                    vmem =pmon[6]
                    mlc  =pmon[7]
                    cname =str(pmon[8])
                

                if '[dso]' in line:
                    cname = cname.split('/')[-1]

                if '[cbk]' in line:
                    cname = cname.split('[')[0]

    
                new_component = { "jid"    :  data["jid"]    ,
                                  "name"   :  cname        ,
                                  "step"   :  step         ,
                                  "domain" :  ""           ,
                                  "stage"  :  stage        ,
                                  "n"      :  nevts        ,
                                  "cpu"    :  cpu          ,
                                  "maxcpu" :  maxcpu       ,
                                  "wall"   :  wall         ,
                                  "vmem"   :  vmem         ,
                                  "maxvmem":  maxvmem      ,
                                  "malloc" :  mlc          ,
                                  "maxmalloc" :  maxmlc    ,
                                  "step"   :  step         }
                            
                components.append(new_component)
                            
                ins = component.insert()
                db.execute(ins,
                           jid             =  data["jid"]      ,
                           name            =  cname            ,
                           step            =  step             ,
                           domain          =  ""               ,
                           stage           =  stage            ,
                           n               =  nevts            ,
                           cpu             =  cpu              ,
                           maxcpu          =  maxcpu           ,
                           wall            =  wall             ,
                           vmem            =  vmem             ,
                           maxvmem         =  maxvmem          ,
                           malloc          =  mlc              ,
                           maxmalloc       =  maxmlc           )
                        
                        
        #END SECOND ITERATION THROUGH LOG FILE       
                    
        data["components"] = components
                        
        json_filename = "rtt_"+nightly+"_"+release+"_"+platform+"_"+jobname+"_"+step+".json"
        with open(json_filename, 'w') as fp:
            json.dump(data,fp)

if __name__ == '__main__':
        main()

