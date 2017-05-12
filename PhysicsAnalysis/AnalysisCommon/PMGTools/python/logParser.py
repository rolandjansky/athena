#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import optparse, sys, math, subprocess, os
from collections import OrderedDict

parser = optparse.OptionParser(usage=__doc__)
parser.add_option("-i", "--input", default="-", dest="INPUT_FILE", metavar="PATH",   help="input logfile")
parser.add_option("-N", "--Ntotal", default=0, dest="TOTAL_EVENTS", metavar="", help="Total number of events")
parser.add_option("-u", "--user", dest="USER", default="-", help="Specify CERN Username in case your system user is different")
parser.add_option("-m", "--mcver", dest="MC_VER", default="MC15", help="Specify MCXX campaign")
parser.add_option("-s", "--nosvn", action="store_true", dest="NO_SVN", default=False, help="Turn off any checks that require SVN access")
parser.add_option("-c", "--nocolour", action="store_true", dest="NO_COLOUR", default=False, help="Turn off colour for copying to file")


opts, fileargs = parser.parse_args()

MCJobOptions='%sJobOptions'%opts.MC_VER
MCXX='%s.'%opts.MC_VER


def main():
    """logParser.py script for parsing log.generate files to check MC production settings and output
     - Written by Josh McFayden <mcfayden@cern.ch> Nov 2016 """


    if opts.INPUT_FILE=="-":
        parser.print_help()
        return 
    
        
    # define dictionaries with keys as variables to be searched for and values to store the results
    
    JOsDict={
        'using release':[],
        "including file \""+MCJobOptions+"/":[],
        "including file \""+MCXX:[]
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
        'physicsComment =':[],
        'generatorName =':[],
        'generatorTune':[],
        'keywords =':[],
        'specialConfig =':[],
        'contactPhysicist =':[],
#        'randomSeed':[],
        'genFilterNames = ':[],
        'cross-section (nb)=':[],
        'generator =':[],
        'weights =':[],
        'PDF =':[],
        'GenFiltEff =':[]
        }
    
    generateDict={
        'minevents':[0]
        }
    
    perfMonDict={
        'snapshot_post_fin':[],
        'last -evt vmem':[]
        }

    # Set username 
    if opts.USER == "-":
        opts.USER = os.environ['USER']
    
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
        JOsErrors.append("including file \""+MCJobOptions+"/")
        JOsErrors.append("including file \""+MCXX)
    else:
          
        if not len(JOsDict["including file \""+MCJobOptions+"/"]):
            JOsErrors.append("including file \""+MCJobOptions+"/")
        if not len(JOsDict["including file \""+MCXX]):
            JOsErrors.append("including file \""+MCXX)
    
        DSIDxxx=''
        topJO=''
        nTopJO=0
        loginfo( '- jobOptions =',"")
        for jo in JOsList:
            pieces=jo.replace('.py','').split('.')
            if len(pieces) and pieces[0]==MCXX.replace('.',''):
                ##This is top JO
                nTopJO=nTopJO+1
                topJO=jo
                DSID=pieces[1]
                DSIDxxx="DSID"+DSID[:3]+"xxx"
    
        if nTopJO!=1:
            logerr( "","ERROR: !=1 (%i) \"top\" JO files found!"%nTopJO)
            raise RuntimeError("!= 1 \"top\" JO file found")
        else:
            #Check if JOs are in SVN!
            if not opts.NO_SVN:
                svnexcomm="svn ls --depth infinity svn+ssh://"+opts.USER+"@svn.cern.ch/reps/atlasoff/Generators/"+MCJobOptions+"/trunk/"
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
            blacklisted=checkBlackList(val.split('-')[0],val.split('-')[1],MCJobOptions,".",JOsList) 
            if blacklisted:
                logerr( '- '+name+' = ',"".join(val)+" <-- ERROR: %s"%blacklisted)
            else:
                loggood( '- '+name+' = ',"".join(val))
        else:
            loginfo( '- '+name+' = ',"".join(val))
    
    
    
    if len(JOsErrors):
        print "---------------------"            
        print "MISSING JOs:"
        for i in JOsErrors:
            if i == "including file \""+MCXX:
                #do nothing
                logwarn("","INFO: local version of JOs used?")
            else:
                logwarn("","ERROR: %s is missing!"%i)
    
    
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
        name=key.replace("=","").strip()
        val=metaDataDict[key]
        if not len(val):
            metaDataErrors.append(name)
        else:
            if name=="contactPhysicist =":
                if '@' in "".join(val):
                    loggood( '- '+name+' = ',"".join(val))
                else:
                    logerr( '- '+name+' = ',"".join(val)+"  <-- ERROR: No email found")
                continue
    
            if name=="keywords =":
                keywords = 'svn export svn+ssh://'+opts.USER+'@svn.cern.ch/reps/atlasoff/Generators/'+MCJobOptions+'/trunk/common/evgenkeywords.txt'
                tmpkeyword= "evgenkeywords.txt"
                svnexcomm='%s %s' % (keywords,tmpkeyword)
                retcode = subprocess.Popen(svnexcomm, shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()

                kfile = open(tmpkeyword)
                klines = kfile.readlines()
                foundkeywordlist=""
                for keyword in (",".join(val)).split(','):
                    keywordfound=False
                    for line in klines:
                        if line.strip().lower()==keyword.strip().lower():
                            keywordfound=True
                            break
                    if not keywordfound:
                        logwarn( '- '+name+' = ',keyword.strip()+"  <-- WARNING: keyword not found in "+MCJobOptions+"/common/evgenkeywords.txt")
                    else:
                        if len(foundkeywordlist): foundkeywordlist+=","+keyword
                        else: foundkeywordlist=keyword
                if len(foundkeywordlist): loggood( '- '+name+' = ',foundkeywordlist)
                        
                continue
                

            loginfo( '- '+name+' = ',"".join(val))
    
    if len(metaDataErrors):
        print "---------------------"            
        print "MISSING Metadata:"
        for i in metaDataErrors:
            if i=="weights" or i=="genFilterNames" or i=="generator" or i=="PDF":
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
                    if float(val[0].replace('%',''))<100. and float(val[0].replace('%',''))>=98.:
                        logwarn( '- '+dictkey+" "+name+' = ',"".join(val))
                    elif float(val[0].replace('%',''))<100.:
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
                if "TestHepMC" in i and "Sherpa" in metaDataDict['generatorName =']:
                    logwarn("","WARNING: %s is missing, but expected as it's Sherpa!"%i)
                else:
                    logerr("","ERROR: %s is missing!"%i)
    
    
    ## Add equivalent lumi information
    if opts.TOTAL_EVENTS:
        print ""
        print ""
        print "---------------------"            
        print " Others:"
    
    if opts.TOTAL_EVENTS:
        xs_nb=float(metaDataDict['cross-section (nb)='][0])
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
        if int(opts.TOTAL_EVENTS) < 20000:
            logwarn("- Total no. of events:",opts.TOTAL_EVENTS+" <-- This total is low enough that the mu profile may be problematic - INFORM MC PROD")
    
    if not opts.TOTAL_EVENTS or opts.NO_SVN:
        print ""
        print ""
        print "---------------------"        
        print "Incomplete tests:"
        if not opts.TOTAL_EVENTS:
            logerr("","ERROR: --Ntotal (-N) flag is not used - total number of events not given - impossible to calculated effective lumi.")
        if opts.NO_SVN:
            logerr("","ERROR: --nosvn (-x) flag is used - could not check that SVN JOs are registered or whether release is blacklisted.")

    print ""
    return 
    


def getJOsList(JOsDict):
    liststr=''
    if len(JOsDict["including file \""+MCJobOptions+"/"]):
        liststr+="|".join(JOsDict["including file \""+MCJobOptions+"/"]).replace("nonStandard/","")
    if len(JOsDict["including file \""+MCXX]):
        if len(liststr): liststr+="|"
        liststr+="|".join(JOsDict["including file \""+MCXX]).replace("nonStandard/","")
    liststr=liststr.replace(MCJobOptions+'/','').replace('"','').replace('including file','').replace(' ','')
    tmplist=liststr.split('|')
    return tmplist


def checkBlackList(branch,cache,MCJobOptions,outnamedir,JOsList) :

    import getpass
    user = getpass.getuser()

    aJOs=[]
    for l in JOsList:
        if MCXX in l:
            aJOs.append(l)   
    ## Black List Caches MC15
    blacklist = 'svn export svn+ssh://'+opts.USER+'@svn.cern.ch/reps/atlasoff/Generators/'+MCJobOptions+'/trunk/common/BlackList_caches.txt'
    tmpblackfile = "%s/BlackList_caches.txt" % (outnamedir)
        
    isError = None
    svnexcomm='%s %s' % (blacklist,tmpblackfile)
    retcode = subprocess.Popen(svnexcomm, shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).communicate()
    if retcode[1].find("exist") != -1 or retcode[1].find("cannot") != -1 :
        logerr("","export failed= BlackList_caches.txt" )
        isError = "export failed= BlackList_caches.txt"
    
    bfile = open(tmpblackfile)
    blines=bfile.readlines()
    for line in blines:
        if not line.strip():
            continue

        bad = "".join(line.split()).split(",")
        
        badgens=[bad[2]]
        if bad[2]=="Pythia8":
            badgens.append("Py8")
        if bad[2]=="Pythia":
            badgens.append("Py")
        if bad[2]=="MadGraph":
            badgens.append("MG")
        if bad[2]=="Powheg":
            badgens.append("Ph")
        if bad[2]=="Herwigpp":
            badgens.append("Hpp")
        if bad[2]=="Herwig7":
            badgens.append("H7")
        if bad[2]=="Sherpa":
            badgens.append("Sh")
        if bad[2]=="Alpgen":
            badgens.append("Ag")
        if bad[2]=="EvtGen":
            badgens.append("EG")
        if bad[2]=="ParticleGun":
            badgens.append("PG")
        
        #Match Generator and release type e.g. AtlasProduction, MCProd
        if any( badgen in s.split('_')[0] for s in aJOs for badgen in badgens ) and branch in bad[0]:
            #Match cache
            cacheMatch=True               
            for i,c in enumerate(cache.split('.')):
                if not c == bad[1].split('.')[i]:
                    cacheMatch=False
                    break

            if cacheMatch:            
                #logerr("", "Combination %s_%s for %s it is blacklisted"%(bad[0],bad[1],bad[2]))
                isError = "%s_%s is blacklisted for %s"%(bad[0],bad[1],bad[2])
                return isError

            
    return isError

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
            if param=="including file \""+MCXX:
                if "including file" in line and MCXX in line:
                    if len(line.split(splitby))==0:
                        raise RuntimeError("Found bad entry %s"%line)
                    else:
                        thing="".join(line.split(lineIdentifier)[1].split(splitby)[1:]).split("/")[-1].strip()
                        dict[param].append(thing)
                    break
            else:
                if param in line:
                    if len(line.split(splitby))==0:
                        raise RuntimeError("Found bad entry %s"%line)
                    else:
                        thing="".join(line.split(lineIdentifier)[1].split(splitby)[1:]).strip()     
                        dict[param].append(thing)
                    break


if __name__ == "__main__":
    main()
