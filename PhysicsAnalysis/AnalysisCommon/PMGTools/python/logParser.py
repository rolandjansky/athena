#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import optparse, sys, math, subprocess
from collections import OrderedDict

parser = optparse.OptionParser(usage=__doc__)
parser.add_option("-i", "--input", default="-", dest="INPUT_FILE", metavar="PATH",   help="input logfile")
parser.add_option("-N", "--Ntotal", default=0, dest="TOTAL_EVENTS", metavar="", help="Total number of events")
parser.add_option("-s", "--nosvn", action="store_true", dest="NO_SVN", default=False, help="Turn off any checks that require SVN access")
parser.add_option("-c", "--nocolour", action="store_true", dest="NO_COLOUR", default=False, help="Turn off colour for copying to file")

opts, fileargs = parser.parse_args()

def main():


    if opts.INPUT_FILE=="-":
        parser.print_help()
        return 
    
    # define dictionaries with keys as variables to be searched for and values to store the results
    
    JOsDict={
        'using release':[],
        "including file \"MC15JobOptions/":[],
        "including file \"MC15.":[]
        }
    
    testHepMCDict={
        'Events passed':[],
        'Efficiency':[]
        }
    
    countHepMCDict={
        'Events passing all checks and written':[]
        }
    
    evgenFilterSeqDict={
        'Weighted Filter Efficiency':[],
        'Filter Efficiency':[]
        }
    
    simTimeEstimateDict={
        'RUN INFORMATION':[]
        }
    
    metaDataDict={ 
        'physicsComment':[],
        'generatorName':[],
        'generatorTune':[],
        'keywords':[],
        'specialConfig':[],
        'contactPhysicist':[],
        'randomSeed':[],
        'genFilterNames':[],
        'cross-section (nb)':[],
        'generator':[],
        'weights':[],
        'GenFiltEff':[]
        }
    
    generateDict={
        'minevents':[0]
        }
    
    perfMonDict={
        'snapshot_post_fin':[],
        'last -evt vmem':[]
        }
    
    
    # open and read log file
    file=open(opts.INPUT_FILE,"r")
    lines=file.readlines()
    
    # check each line
    for line in lines:
    
        checkLine(line,'Py:Athena',JOsDict,'INFO')
    
        checkLine(line,'MetaData',metaDataDict,'=')
        checkLine(line,'Py:Generate',generateDict,'=')
    
        checkLine(line,'Py:PerfMonSvc',perfMonDict,':')
        checkLine(line,'PMonSD',perfMonDict,'---')
    
        checkLine(line,'TestHepMC',testHepMCDict,'=')
        checkLine(line,'Py:EvgenFilterSeq',evgenFilterSeqDict,'=')
        checkLine(line,'CountHepMC',countHepMCDict,'=')
        checkLine(line,'SimTimeEstimate',simTimeEstimateDict,'|')
            
    
            
    # print results
    JOsErrors=[]
    print ""
    print "---------------------"
    print "jobOptions and release:"
    print "---------------------"
    
    #Checking jobOptions
    JOsList=getJOsList(JOsDict)
    if not len(JOsList):
        JOsErrors.append("including file \"MC15JobOptions/")
        JOsErrors.append("including file \"MC15.")
    else:
    
        
        if not len(JOsDict["including file \"MC15JobOptions/"]):                                                                                                          
            JOsErrors.append("including file \"MC15JobOptions/")
        if not len(JOsDict["including file \"MC15."]):
            JOsErrors.append("including file \"MC15.")
    
        DSIDxxx=''
        topJO=''
        nTopJO=0
        loginfo( '- jobOptions =',"")
        for jo in JOsList:
            pieces=jo.replace('.py','').split('.')
            if len(pieces) and pieces[0]=='MC15':
                ##This is top JO
                nTopJO=nTopJO+1
                topJO=jo
                DSID=pieces[1]
                DSIDxxx="DSID"+DSID[:3]+"xxx"
    
        if nTopJO!=1:
            logerr( "","ERROR: More than one \"top\" JO file found!")
            raise RuntimeError("More than one \"top\" JO file found")
        else:
            #Check if JOs are in SVN!
            if not opts.NO_SVN:
                svnexcomm="svn ls --depth infinity svn+ssh://mcfayden@svn.cern.ch/reps/atlasoff/Generators/MC15JobOptions/trunk/"
                retcode = subprocess.Popen(svnexcomm, shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
                svnJOs=retcode[0].split()
                if len(retcode[1]):
                    print retcode[1]
                    logerr("","ERROR: Problem reading from SVN")
                    raise RuntimeError("Problem reading from SVN")
    
                for jo in JOsList:
                    if jo == topJO:
                        if "share/%s/%s"%(DSIDxxx,jo) in retcode[0]:
                            loggood("",jo)
                        else:
                            logerr("",jo+" ERROR <-- jobOptions not found in SVN!")
                    else:
                        indices = [i for i,s in enumerate(svnJOs) if "/"+jo in s]
                        if len(indices)==1:
                            loggood("",jo)
                        else:
                            if not len(indices):
                                logerr("",jo+" - jobOptions not found in SVN!")
                            else:
                                logwarn("",jo+" - multiple instances of jobOptions not found in SVN!")
                                for ix in indices:
                                    logwarn("",jo+" --> %s"%svnJOs[ix])
                                    
            else:
                for jo in JOsList:
                    loginfo("",jo)
    
    
    #Checking release
    if not len(JOsDict['using release']):
        JOsErrors.append(JOsDict['using release'])
    else:
        name='using release'
        tmp=JOsDict[name][0].replace('using release','').strip().split()[0]
        val=tmp.replace('[','').replace(']','')
        #checkForBlacklist
        if not opts.NO_SVN:
            if checkBlackList(val.split('-')[0],val.split('-')[1],'MC15JobOptions',".",JOsList) :
                logerr( '- '+name+' = ',"".join(val)+" <-- ERROR: Cache is blacklisted for this generator")
            else:
                loggood( '- '+name+' = ',"".join(val))
        else:
            loginfo( '- '+name+' = ',"".join(val))
    
    
    
    if len(JOsErrors):
        print "---------------------"            
        print "MISSING JOs:"
        for i in JOsErrors:
            if i == "including file \"MC15.":
                #do nothing
                logwarn("","INFO: local version of JOs used?")
            else:
                logerr("","ERROR: %s is missing!"%i)
    
    
    ###
    generateErrors=[]
    print ""
    print "---------------------"
    print "Generate params:"
    print "---------------------"
    for key in generateDict:
        name=key
        val=generateDict[key]
    
        if not len(val):
            generate.append(name)
        else:
            if key == 'minevents':
                tmp=str(val[2]).split('#')[0].strip()
                generateDict['minevents']=tmp
                val=tmp
    
            loginfo( '- '+name+' = ',"".join(val))
    
    if len(generateErrors):
        print "---------------------"            
        print "MISSING Generate params:"
        for i in generateErrors:
            logerr("","ERROR: %s is missing!"%i)
            
    
    ###
    metaDataErrors=[]
    print ""
    print "---------------------"            
    print "Metadata:"
    print "---------------------"
    for key in metaDataDict:
        name=key
        val=metaDataDict[key]
        if not len(val):
            metaDataErrors.append(name)
        else:
            if name=="contactPhysicist":
                if not '@' in val:
                    logerr( '- '+name+' = ',"".join(val)+"  <-- ERROR: No email found")
                continue
    
            loginfo( '- '+name+' = ',"".join(val))
    
    if len(metaDataErrors):
        print "---------------------"            
        print "MISSING Metadata:"
        for i in metaDataErrors:
            if i=="weights":
                loginfo("INFO:","%s is missing"%i)
            else:
                logerr("","ERROR: %s is missing!"%i)
            
    
    ####
    perfMonErrors=[]
    print ""
    print "---------------------"
    print "Performance metrics:"
    print "---------------------"
    for key in perfMonDict:
        name=key
        val=perfMonDict[key]
        if not len(val):
            perfMonErrors.append(name)
        else:
    
            if key == 'snapshot_post_fin':
                name = 'CPU'
                tmp = 0.
                tmp=float(val[0].split()[3])
                tmp=tmp/(1000.*60.*60.)
                val = "%.2f hrs"%tmp
                if tmp > 18.: 
                    logerr( '- '+name+' = ',"".join(val))
                else:
                    loggood( '- '+name+' = ',"".join(val))
    
            if key == 'last -evt vmem':
                name = 'Virtual memory'
                tmp=float(val[0].split()[0])
                if tmp > 2048.: 
                    logerr( '- '+name+' = ',"".join(val))
                else:
                    loggood( '- '+name+' = ',"".join(val))
    
    
    if len(perfMonErrors):
        print "---------------------"            
        print "MISSING Performance metric:"
        for i in perfMonErrors:
            print "ERROR: %s is missing!"%i
            
    
    
    ####     
    testDict = {
        'TestHepMC':testHepMCDict,
        'EvgenFilterSeq':evgenFilterSeqDict,
        'CountHepMC':countHepMCDict,
        'SimTimeEstimate':simTimeEstimateDict
        }
    
    testErrors=[]
    print ""
    print "---------------------"
    print "Event tests:"
    print "---------------------"
    for dictkey in testDict:
        for key in testDict[dictkey]:
            name=key
            val=testDict[dictkey][key]
            if not len(val):
                testErrors.append("%s %s"%(dictkey,name))
            else:
                #Check final Nevents processed
                if dictkey=="CountHepMC":
                    allowed_minevents_lt1000 = [1, 2, 5, 10, 20, 25, 50, 100, 200, 500]
                    if int(val[0])%1000!=0 and not int(val[0]) in allowed_minevents_lt1000:
                        logerr( '- '+dictkey+" "+name+' = ',"".join(val)+"  <-- ERROR: Not an acceptable number of events for production")
                    elif int(val[0]) != int(generateDict['minevents']):
                        logerr( '- '+dictkey+" "+name+' = ',"".join(val)+"  <-- ERROR: This is not equal to minevents")
                    else:
                        loggood( '- '+dictkey+" "+name+' = ',"".join(val))
                    continue
    
                #Check filter efficiencies aren not too low
                if dictkey=="EvgenFilterSeq":
                    if float(val[0].split()[0])<0.01:
                        logerr( '- '+dictkey+" "+name+' = ',"".join(val))
                    else:
                        loggood( '- '+dictkey+" "+name+' = ',"".join(val))
                    continue
    
                if dictkey=="TestHepMC" and name=="Efficiency":
                    if float(val[0].replace('%',''))<100.:
                        logerr( '- '+dictkey+" "+name+' = ',"".join(val))
                    else:
                        loggood( '- '+dictkey+" "+name+' = ',"".join(val))
                    continue
    
    
                loginfo( '- '+dictkey+" "+name+' = ',"".join(val))
    
    if len(testErrors):
        print "---------------------"            
        print "Failed tests:"
        for i in testErrors:
            if i =="SimTimeEstimate RUN INFORMATION":
                logwarn("","WARNING: %s is missing!"%i)
            else:
                logerr("","ERROR: %s is missing!"%i)
    
    
    ## Add equivalent lumi information
    if opts.TOTAL_EVENTS:
        print ""
        print ""
        print "---------------------"            
        print " Others:"
    
    if opts.TOTAL_EVENTS:
        xs_nb=float(metaDataDict['cross-section (nb)'][0])
        eff_lumi_fb=float(opts.TOTAL_EVENTS)/(1.E+06*xs_nb)
        if eff_lumi_fb > 1000.:
            logwarn("- Effective lumi (fb-1):",str(eff_lumi_fb))
        elif eff_lumi_fb < 40.:
            logwarn("- Effective lumi (fb-1):",str(eff_lumi_fb))
        else:
            loggood("- Effective lumi (fb-1):",str(eff_lumi_fb))
        minevents=int(generateDict['minevents'])
        #int(countHepMCDict['Events passing all checks and written'][0])
        loginfo("- Number of jobs:",int(opts.TOTAL_EVENTS)/minevents)
    
    if not opts.TOTAL_EVENTS or opts.NO_SVN:
        print ""
        print ""
        print "---------------------"        
        print "Incomplete:"
        if not opts.TOTAL_EVENTS:
            logwarn("","WARNING: --Ntotal (-N) flag is not used - total number of events not given - impossible to calculated effective lumi.")
        if opts.NO_SVN:
            logwarn("","WARNING: --nosvn (-x) flag is used - could not check that SVN JOs are registered or whether release is blacklisted.")
    
    print ""
    return 
    


def getJOsList(JOsDict):
    liststr=''
    if len(JOsDict["including file \"MC15JobOptions/"]):
        liststr+="|".join(JOsDict["including file \"MC15JobOptions/"]).replace("nonStandard/","")
    if len(JOsDict["including file \"MC15."]):
        liststr+="|".join(JOsDict["including file \"MC15."].replace("nonStandard/",""))
    liststr=liststr.replace('MC15JobOptions/','').replace('"','').replace('including file','').replace(' ','')
    tmplist=liststr.split('|')
    return tmplist


def checkBlackList(branch,cache,MCJobOptions,outnamedir,JOsList) :

    import getpass
    user = getpass.getuser()

    aJOs=[]
    for l in JOsList:
        if "MC15." in l:
            aJOs.append(l)   
    print "JOSH",aJOs
    ## Black List Caches MC15
    blacklist = 'svn export svn+ssh://svn.cern.ch/reps/atlasoff/Generators/'+MCJobOptions+'/trunk/common/BlackList_caches.txt'
    tmpblackfile = "%s/BlackList_caches.txt" % (outnamedir)
        
    isError = False
    if 'MC15' in MCJobOptions :
        svnexcomm='%s %s' % (blacklist,tmpblackfile)
        retcode = subprocess.Popen(svnexcomm, shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
        if retcode[1].find("exist") != -1 or retcode[1].find("cannot") != -1 :
            logerr("","export failed= BlackList_caches.txt" )
            isError = True
        
        bfile = open(tmpblackfile)
        for line in bfile:
            if not line.strip():
                continue

            bad = "".join(line.split()).split(",")
            
            #Match Generator and release type e.g. AtlasProduction, MCProd
            if any( bad[2] in s for s in aJOs ) and branch in bad[0]:
                #Match cache
                cacheMatch=True               
                for i,c in enumerate(cache.split('.')):
                    if not c == bad[1].split('.')[i]:
                        cacheMatch=False
                        break

                if cacheMatch:            
                    #logerr("", "Combination %s_%s for %s it is NOT allowed"%(bad[0],bad[1],bad[2]))
                    isError = True
                    return isError

                
        return isError
    else:
        return False
                                        

class bcolors:
    if not opts.NO_COLOUR:
        HEADER = '\033[95m'
        OKBLUE = '\033[94m'
        OKGREEN = '\033[92m'
        WARNING = '\033[93m'
        FAIL = '\033[91m'
        ENDC = '\033[0m'
    else:
        HEADER = ''
        OKBLUE = ''
        OKGREEN = ''
        WARNING = ''
        FAIL = ''
        ENDC = ''

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

def loginfo(out1,out2):
    print str(out1),bcolors.OKBLUE + str(out2) + bcolors.ENDC
def loggood(out1,out2):
    print str(out1),bcolors.OKGREEN + str(out2) + bcolors.ENDC
def logerr(out1,out2):
    print str(out1),bcolors.FAIL + str(out2) + bcolors.ENDC
def logwarn(out1,out2):
    print str(out1),bcolors.WARNING + str(out2) + bcolors.ENDC

# find identifiers and variables in log file lines
def checkLine(line, lineIdentifier, dict, splitby ):
    if lineIdentifier in line:
        for param in dict:
            if param in line:
                if len(line.split(splitby))==0:
                    raise RuntimeError("Found bad entry %s"%line)
                else:
                    thing="".join(line.split(lineIdentifier)[1].split(splitby)[1:]).strip()     
                    dict[param].append(thing)
                break


if __name__ == "__main__":
    main()
