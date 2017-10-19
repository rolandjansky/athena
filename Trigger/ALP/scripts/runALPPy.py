#!/usr/bin/env python
import os,sys,argparse,signal

def signal_handler(sig,frame):
    print('Signal Handler Called for' ,signum)
    os.exit()
def parse_extra(parser,extra):
    namespaces=[]
    print "parsing commandline '%s'"%extra
    while extra:
        n,extra = parser.parse_known_args(extra)
        namespaces.append(n)
        print "Namespace=",n,"extra=",extra
    return namespaces

#from http://stackoverflow.com/questions/20094215/argparse-subparser-monolithic-help-output

class _BigHelp(argparse._HelpAction):
    def __call__(self, parser, namespace, values, option_string=None,ErrReason=None):
        parser.print_help()
        
        modGroups={' Data Source Modules ':['dcmds','fileds'],
                   ' Monitoring Modules ':['monsvcis'],
                   ' Trigger Configuration Modules ':['joboptions','DBPython','DB'],
                   ' ALPPy Module ':['ALP'],
                   'help':['-h','--help']
               }
        modMaps={}
        for mg in modGroups.keys():
            for m in modGroups[mg]:
                modMaps[m]=mg
        helpGroups={}
        # retrieve subparsers from parser
        subparsers_actions = [
            action for action in parser._actions
            if isinstance(action, argparse._SubParsersAction)]
        # there will probably only be one subparser_action,
        # but better save than sorry
        for subparsers_action in subparsers_actions:
            # get all subparsers and print help
            for choice, subparser in subparsers_action.choices.items():
                hg=modMaps[choice]
                if hg not in helpGroups:
                    helpGroups[hg]=[]
                helpGroups[hg].append("%s\n%s"%('{:-^40}'.format(" %s "%choice),subparser.format_help()))
        print 20*"*","MODULES HELP",20*"*"
        print " You can specify --help after module name to see only relative modules help"
        for g in helpGroups:
            print ("\n{:*^60}\n".format(g))
            for m in helpGroups[g]:
                print m
        if ErrReason is not None:
            print
            print "%s\n"%ErrReason
        parser.exit()


def getDataSource(cdict):
    NS=cdict["datasource"]
    if NS is None or NS['module']=='dcmds':
        from HLTMPPy.HLTMPPy import DCMDataSource as ds
        return ds()
    if NS['module']=='fileds':
        from HLTMPPy.HLTMPPy import FileDataSource as FD
        fds=FD(fileList=NS['file'],
               outFile=NS["outFile"],
               compressionLevel=NS["compressionLevel"],
               compressionFormat=NS["compressionFormat"],
               preload=NS["preload"],
               numEvents=NS["numEvents"],
               library=NS["dslibrary"],
               loopFiles=NS["loopFiles"]
               )
        return fds
    return None

def getInfoSvc(cdict):
    NS=cdict['monitoring']
    if NS is None:
        from HLTMPPy.HLTMPPy import MonSvcInfoService as MSI
        mon=MSI()
        return mon
    if NS['module']=="monsvcis":
        from HLTMPPy.HLTMPPy import MonSvcInfoService as MSI
        mon=MSI(OHServer=NS['OHServer'],
                OHSlots=NS['OHSlots'],
                OHInterval=NS['OHInterval'],
                OHRegex=NS['OHRegex'],
                ISServer=NS['ISServer'],
                ISSlots=NS['ISSlots'],
                ISInterval=NS['ISInterval'],
                ISRegex=NS['ISRegex'])
        return mon
    return None

def getTriggerConfig(cdict):
    td=cdict['trigger']
    if td['module']=='joboptions':
        from HLTMPPy.HLTMPPy import TriggerConfigJO as TC
        tc=TC(jopath=td['joFile'],
              SMK=td['SMK'],
              prescaleKey=td['l1PSK'],
              bunchKey=td['l1BG'],
              L1MenuFrom=td['l1MenuConfig'])
        return tc
    elif td['module']=='DB':
        from HLTMPPy.HLTMPPy import TriggerConfigDB as TC
        tc=TC(SMK=td['SMK'],
              L1PSK=td['l1PSK'],
              L1BG=td['l1BG'],
              HPSK=td['HLTPSK'],
              Coral=td['use_coral'],
              DBAlias=td['db_alias'])
        return tc
    elif td['module']=='DBPython':
        from HLTMPPy.HLTMPPy import TriggerConfigDBPython as TC
        tc=TC(SMK=td['SMK'],
              L1PSK=td['l1PSK'],
              L1BG=td['l1BG'],
              HPSK=td['HLTPSK'],
              Coral=td['use_coral'],
              preCmds=td['precommand'],
              postCmds=td['postcommand'],
              DBAlias=td['db_alias']
        )
        return tc
    return None

def getHLTMPPUConfig(cdict,DS=None,IS=None):
    hd=cdict['HLTMPPU']
    DSrc=DS
    if DSrc is None : DSrc=getDataSource(cdict)
    ISvc=IS
    if ISvc is None : ISvc=getInfoSvc(cdict)
    
    from HLTMPPy.HLTMPPy import HLTMPPUConfig as HC
    hc=HC(numForks=hd['num_forks'],
          HardTimeout=hd['hard_timeout'],
          softTimeoutFraction=hd['soft_timeout_fraction'],
          extraParams=hd['extra_params'],
          childLogRoot=hd['log_root'],
          partitionName=hd['partition_name'],
          HLTLibs=cdict['trigger']['library'],
          DataSrc=DSrc,
          InfoSvc=ISvc
      )
    return hc

def getPartitionTree(cdict):
 from lxml import etree as et
 root=et.Element("Partition")
 et.SubElement(root,"UID").text=str(cdict['global']['partition_name'])
 et.SubElement(root,"LogRoot").text=str(cdict['global']['log_root'])
 return root

def getConfigurationTree(cdict):
 from lxml import etree as et
 root=et.Element("Configuration")
 ds=getDataSource(cdict)
 inf=getInfoSvc(cdict)
 hlt=getHLTMPPUConfig(cdict,ds,inf)
 trig=getTriggerConfig(cdict)
 part=getPartitionTree(cdict)
 part.append(trig.getTree())
 root.append(hlt.getTree())
 root.append(part)
 et.SubElement(root,"ROS2ROBS")
 return et.tostring(root,pretty_print=True)

def getPrepareForRunTree(cdict):
 RunParams={
  "run_number":cdict['global']['run_number'],
  'max_events':'0',
  'recording_enabled':'0',
  'trigger_type':'0',
  'run_type':'Physics',
  'det_mask':'0'*(32-len(cdict['global']['detector_mask']))+cdict['global']['detector_mask'],
  'beam_type':'0',
  'beam_energy':'0',
  'filename_tag':"",
  'T0_project_tag':'',
  'timeSOR':cdict['global']['date'],
  'timeEOR':'1/1/70 01:00:00',
  'totalTime':'0'
 }
 Magnets={'ToroidsCurrent':
          {
           'value':cdict['global']['toroid_current'],
           'ts':cdict['global']['date']
          },
          'SolenoidCurrent':
          {
           'value':cdict['global']['solenoid_current'],
           'ts':cdict['global']['date']
          }
 }
 from lxml import etree as et
 RT=et.Element("RunParams")
 for k in iter(RunParams):
  et.SubElement(RT,k).text=str(RunParams[k])
 M=et.Element("Magnets")
 for m in ("ToroidsCurrent","SolenoidCurrent"):
  T=et.SubElement(M,m)
  for k in iter(Magnets[m]):
   et.SubElement(T,k).text=str(Magnets[m][k])
 return et.tostring(RT,pretty_print=True)+et.tostring(M,pretty_print=True)

def getConfigDictionary(NamespaceList,modMap):
    d={}
    globalArgs=['extra','log_root','partition_name','with_infrastructure','run_number','save_options',
                'options_file','detector_mask','toroid_current','solenoid_current','date'
    ]
    for n in NamespaceList:
        gk=modMap[n.module]
        args=vars(n)
        d[gk]={key:value for (key,value) in args.items() if key not in globalArgs}
    if d['trigger']['module'] is not 'joboptions':
        d['trigger']['library'].append("TrigConfigSvc")
    ht=d['HLTMPPU']
    ht['log_root']=NamespaceList[0].log_root
    ht['partition_name']=NamespaceList[0].partition_name
    d['global']={key:value for (key,value) in vars(NamespaceList[0]).items() if key in globalArgs[1:]}
    if d['global']['date']=="0":
        import datetime
        now=datetime.datetime.now()
        d['global']['date']="{:%d/%m/%y} {:%H:%M:%S}".format(now,now)
    return d


def main():    
    p=argparse.ArgumentParser(description="HLTMPPU python based steering",formatter_class=argparse.ArgumentDefaultsHelpFormatter,add_help=False)
    p.add_argument('-h',"--help",action=_BigHelp,help="Print this help and exit")
    subp=p.add_subparsers(help="Module configurations",dest='module')
    #subp.required=True
    #p.add_argument('--file',nargs='+')
    # monmods=p.add_subparsers(help='Monitoring modules')
    # trigmods=p.add_subparsers(help='Trigger configuration modules')

    dcmdsopts=subp.add_parser('dcmds',help="DCMDataSource options",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    #dcmdsopts.add_argument('--dslibrary',nargs=1,const="dfinterfaceDcm",default="dfinterfaceDcm")
    dcmdsopts.add_argument('--dslibrary',nargs=1,default="dfinterfaceDcm",help="Library that provides the interface")
    dcmdsopts.add_argument('--ros2robs',nargs=1,default='',help="Either a file or a string in the form of python array that contains ros-rob mappings")
    
#   File DS
#
#
    fileds=subp.add_parser('fileds',help="File based data source options",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    fileds.add_argument('--dslibrary',nargs='?',const="FileDataSource",default="FileDataSource",help="Library that provides the interface")
    fileds.add_argument('--loopFiles',action='store_true',help="Whether to loop over the files")
    fileds.add_argument('--skipEvts',type=int,nargs='?',help="Number of events to skip",const=0,default=0)
    fileds.add_argument('--numEvents',type=int,nargs='?',help="Number of events to process",const=-1,default=-1)
    fileds.add_argument('--file',action='append',help="list of files to process, can be repeated for multiple files",required=True)
    fileds.add_argument('--preload',action='store_true',help="Preload files into memory")
    fileds.add_argument('--outFile',help="name of the output file")
    fileds.add_argument('--compressionLevel',type=int,help="compression level of output file",default=2)
    fileds.add_argument('--compressionFormat',choices=["ZLIB","UNCOMPRESSED"],help="compression level of output file",default="ZLIB")

    monsvcis=subp.add_parser('monsvcis',help="MonSvc (online) based monitoring",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    monsvcis.add_argument('--histogram-server',nargs='?',
                          const='${TDAQ_OH_SERVER=Histogramming}',
                          default='${TDAQ_OH_SERVER=Histogramming}',
                          help="Destination IS server for Histograms",dest='OHServer')
    monsvcis.add_argument('--hist-publish-period',nargs='?',type=int,const=80,
                          default=80,help="Publication period for histograms",
                          dest='OHInterval'
                          )
    monsvcis.add_argument('--histogram-regex',nargs='?',const='.*',default='.*',
                          help='Histogram regex',dest="OHRegex")

    monsvcis.add_argument('--hist-slots',nargs='?',type=int,const=8,default=8,
                          help="Number of slots for OH publication",dest='OHSlots')
    monsvcis.add_argument('--is-publish-period',nargs='?',type=int,const=10,default=10,
                          help="Publication period for IS objects",dest='ISInterval')
    monsvcis.add_argument('--is-server',nargs='?',const='${TDAQ_IS_SERVER=DF}',
                          default='${TDAQ_IS_SERVER=DF}',help="Destination IS server",
                          dest='ISServer')
    monsvcis.add_argument('--is-slots',nargs='?',type=int,const=1,default=1,
                          help="Number of slots for IS publication",dest='ISSlots')
    monsvcis.add_argument('--is-regex',nargs='?',const='.*',default='.*',
                          help='Histogram regex',dest="ISRegex")

    
    trigcommon=argparse.ArgumentParser(add_help=False)
    trigcommon.add_argument('--l1PSK',type=int,help="Level-1 Prescale key",default=0)
    trigcommon.add_argument('--l1BG',type=int,help="Level-1 Bunch Group key",default=0)
    trigcommon.add_argument('--SMK',type=int,help="Super Master Key",default=0)
    trigcommon.add_argument('--library',action='append',default=["TrigServices","TrigPSC"])

    dbcommon=argparse.ArgumentParser(add_help=False)
    
    dbcommon.add_argument('--HLTPSK',help='HLT Prescale key',default=0)
    dbcommon.add_argument('--db-alias',help='Alias for Trigger DB configuration',default="TRIGGERDB")
    dbcommon.add_argument('--use-coral',
                          action='store_true',help='Whether to use local coral proxy')
    dbcommon.add_argument('--coral-server',nargs='?',help='Coral Server url',const='LOCAL_HOST',default='LOCAL_HOST')
    dbcommon.add_argument('--coral-port',nargs='?',type=int,help='Coral Server port',const=3320,default=3320)
    dbcommon.add_argument('--coral-user',nargs='?',help='Coral Server user name',
                          const="ATLAS_CONF_TRIGGER_RUN2_R",
                          default="ATLAS_CONF_TRIGGER_RUN2_R")
    dbcommon.add_argument('--coral-password',nargs='?',help='Coral Server password',
                          const="TrigConfigRead2015",
                          default="TrigConfigRead2015",
                          )
    jotrigConf=subp.add_parser('joboptions',help="Joboptions based trigger config",parents=[trigcommon],
                               formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    jotrigConf.add_argument('--l1MenuConfig',nargs='?',const='DB',default='DB',choices=["DB"]
                            ,help="Where to get L1 menu configuration")
    jotrigConf.add_argument('--joFile',nargs=1,required=True,help="Joboptions file to run")
    
    
    DBPyConf=subp.add_parser("DBPython",help="DBPython based trigger config",parents=[trigcommon,dbcommon],
                             formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    DBPyConf.add_argument('--precommand',action='append',help='pre-command,can be repeated')
    DBPyConf.add_argument('--postcommand',action='append',help='post-command, can be repeated')
    
    
    DBConf=subp.add_parser("DB",help="DB based trigger config",parents=[trigcommon,dbcommon],
                           formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    puConf=subp.add_parser("HLTMPPU",help="HLTMPPU Configuration",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    puConf.add_argument("-N",'--application-name',nargs='?',const="HTLMPPy-1",default="HLTMPPy-1",help="Application Name")
    puConf.add_argument("-i",'--interactive',action='store_true',help="Whether to run in interactive mode")
    puConf.add_argument("-f",'--num-forks',nargs='?',type=int,const=2,default=2,help="Number of children to fork")
    puConf.add_argument('-F','--finalize-timeout',type=int,nargs=1,default=120,help="Timeout at finalization")
    puConf.add_argument('-e','--extra-params',action='append',help='Extra parameters for HLTMPPU')
    puConf.add_argument('-t','--soft-timeout-fraction',type=float,nargs=1,default=0.8,help="Fraction of hard timeout to trigger soft timeout.")
    puConf.add_argument('-T','--hard-timeout',type=int,nargs=1,default=60000,help="Hard timeout duration in milliseconds.")
    p.add_argument("-I",'--with-infrastructure',action='store_true',help="Whether to start ipc and IS infrastructure")
    p.add_argument("-r","--run-number",type=int,nargs='?',const=-1,default=-1,help="Run number")
    p.add_argument("-l","--log-root",nargs=1,default="/tmp/",help="directory to save log files of child processes")
    p.add_argument("-p","--partition-name",nargs='?',default="",help="Partition Name")
    p.add_argument("-O","--options-file",nargs=1,help="Read configuration from options file")
    p.add_argument("-S","--save-options",nargs=1,help="Write configuration to options file. Extension defines the format (json,yaml,xml)")

    p.add_argument("--toroid-current",nargs='?',type=float,default=3.14,const=3.14,help="Value of the toroid current to pass during prepareForRun")
    p.add_argument("--solenoid-current",nargs='?',type=float,default=314.15,const=314.15,help="Value of the solenoid current to pass during prepareForRun")
    p.add_argument("--date",nargs='?',default="0",const="0",help="Run start date to be passed during prepareForRun")
    p.add_argument("--sleep",type=int,default=120,help="Seconds to sleep before calling stop")
    p.add_argument("--detector-mask",nargs='?',default="0000000000000000400269affffffff7",const="0000000000000000400269affffffff7",help="Detector mask")    
    
    #p.add_argument('extra',nargs='*',help=argparse.SUPPRESS)

    commands=sys.argv[1:]
    modGroups={'datasource':['dcmds','fileds'],
               'monitoring':['monsvcis'],
               'trigger':['DBPython','DB','joboptions'],
               'HLTMPPU':['HLTMPPU'],
               }
    modNames=[]
    for k in modGroups.keys():
        modNames.extend(modGroups[k])
    needDefault=True
    modMap={}
    modCount={}
    for k in modGroups.keys():
        modCount[k]=[]
        for m in modGroups[k]:
            modMap[m]=k
    
    #print modNames
    for m in modNames:
        if m in commands:
            needDefault=False
    if needDefault: commands.extend(['HLTMPPU'])

    extra_namespaces=parse_extra(p,commands)
    print 
    for n in extra_namespaces:
        g=modMap[n.module]
        modCount[g].append(n.module)
        print "Module __'%s'__  "%n.module,n
        print
    for m in modCount.keys():
        if len(modCount[m]) > 1: 

            _BigHelp(None,None)(p,None,None,None,'ERROR! More than one module type defined for module class %s %s'%(m,modCount[m]))
        if len(modCount[m]) == 0:
            defMod=modGroups[m][0]
            modCount[m].append(defMod)
            print "Adding default Module '%s' for type '%s' "%(defMod,m)
            extra_namespaces.append((p.parse_known_args(["%s"%((modGroups[m])[0])]))[0])
    print "Final namespaces ",extra_namespaces
    import pprint
    pp=pprint.PrettyPrinter()
    from os import environ as env
    if extra_namespaces[0].partition_name=="":
        if "TDAQ_PARTITION" not in env:
            extra_namespaces[0].partition_name="HLTMPPy_partition"
        else:
            extra_namespaces[0].partition_name=env['TDAQ_PARTITION']
    cdict=getConfigDictionary(extra_namespaces,modMap)
    pp.pprint(cdict)
    #HConf=getHLTMPPUConfig(cdict)
    #tree=g.getTree()
    if "TDAQ_APPLICATION_NAME" not in env:
     env["TDAQ_APPLICATION_NAME"]=cdict['HLTMPPU']['application_name']
    if "TDAQ_PARTITION" not in env:
     env["TDAQ_PARTITION"]=cdict['global']['partition_name']
    tree=getConfigurationTree(cdict)
    print tree
    print
    prtree=getPrepareForRunTree(cdict)
    print prtree
    # needed to initialize ipc::core
    #sys.exit(0)
    from ispy import IPCPartition
    from HLTMPPy.HLTMPPy import HLTMPPy as HPY
    mppy=HPY()
    print mppy.Configure(tree)
    print 10*"* *","Configure Finished"
    print mppy.Connect()
    print 10*"* *","Connect Finished"
    print mppy.PrepareForRun(prtree)
    if cdict['HLTMPPU']['interactive']:
        from IPython import embed
        embed()
    else:
        import time
        print "sleeping for %s seconds"%extra_namespaces[0].sleep
        time.sleep(extra_namespaces[0].sleep)

if "__main__" in __name__:
 main()

