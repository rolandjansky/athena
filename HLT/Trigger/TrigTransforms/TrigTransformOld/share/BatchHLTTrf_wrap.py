#!/usr/bin/env python

import re
import os, commands
from time import sleep
import sys
import pickle
import tempfile

ListOfDefaultPositionalKeys = ['extraParameter','inputBSFile','outputRAWFile','comdir','smk','doStreaming','filters','stream_out','release','connection','lvl1ps','hltps','local_patch_area','jobOptions','options','conditions','enLVL1prescales','outputNTUP_TRIGCOSTEFFile','outputNTUP_TRIGCOSTL2File','outputNTUP_TRIGRATEEFFile','outputNTUP_TRIGRATEL2File','outputHIST_HLTMONEFFile','outputHIST_HLTMONL2File']


        
if __name__ == '__main__':

    #trfDir  = '/afs/cern.ch/user/s/sgeorge/scratch0/PANDA/trunk/TOMscripts'
    #trfDir  = '/afs/cern.ch/atlas/maxidisk/d76/Test_new'
    trfName = 'BatchHLTTrf.py'

    # convert arguments to a pickle
    argMap = {}
    for tmpKeyVal in sys.argv[1:]:
        try:
            tmpMatch = re.search('^([^=]+)=(.+)$',tmpKeyVal)
            if tmpMatch != None:
                mapKey = tmpMatch.group(1)
                mapVal = tmpMatch.group(2)
                if mapKey in ['inputBSFile']:
                    # convert to list
                    argMap[mapKey] = mapVal.split(',')
                else:
                    # convert to int
                    try:
                        argMap[mapKey] = eval(mapVal)
                    except:
                        # use string
                        argMap[mapKey] = mapVal
        except:
            pass
    
    print "BatchHLTTrf_wrap.py: arguments : " + str(sys.argv[1:])
    print "BatchHLTTrf_wrap.py: arg map   : " + str(argMap)
    
    ''' FIXME '''
    # get partid and attempt from filename like RAW.310755._000001.data.1
    # attempt = argMap['outputRAWFile'].split('_')[1].split('.')[2]
    partid  = argMap['outputRAWFile'].split('_')[1].split('.')[0].lstrip('0')
    # throttle jobs to avoid Oracle overload
    #sleep (float(partid))
    
    # dataset#filename
    for key in ['inputBSFile']:
        for fileName in argMap[key]:
            argMap[key][argMap[key].index(fileName)] = "#%s" % fileName
    for key in ['outputRAWFile', 'outputNTUP_TRIGCOSTEFFile', 'outputNTUP_TRIGCOSTL2File', 'outputNTUP_TRIGRATEEFFile', 'outputNTUP_TRIGRATEL2File','outputHIST_HLTMONEFFile','outputHIST_HLTMONL2File']:
        if key in argMap.keys():
            argMap[key] = "#%s" % argMap[key]
    
    print "BatchHLTTrf_wrap.py: arg map 2  : " + str(argMap)

    replace_sequence = {' ': ''}
    
    singArgMapKeys = ListOfDefaultPositionalKeys
    signArgMap = {}
    signArgMap['applicationConfig'] = {}

    ''' FIXME '''
    # set _partid to prodsys taskID_partID from filename like RAW.310755._000001.data.1
    signArgMap['_partid'] = argMap['outputRAWFile'].split('.')[1]+argMap['outputRAWFile'].split('.')[2]
    
    for key in singArgMapKeys:
        if key in argMap.keys():
            if key == 'inputBSFile':
                signArgMap['inputRAWFiles'] = argMap[key]
            elif key == 'outputRAWFile':
                signArgMap['outputRAWFiles'] = argMap[key]
            elif key == 'outputNTUP_TRIGCOSTEFFile':
                signArgMap['outputCostEF'] = argMap[key]
            elif key == 'outputNTUP_TRIGCOSTL2File':
                signArgMap['outputCostL2'] = argMap[key]
            elif key == 'outputNTUP_TRIGRATEEFFile':
                signArgMap['outputRateEF'] = argMap[key]
            elif key == 'outputHISTFile':
                signArgMap['outputRoot'] = argMap[key]
            elif key == 'outputNTUP_TRIGRATEL2File':
                signArgMap['outputRateL2'] = argMap[key]
            elif key == 'outputHIST_HLTMONEFFile':
                signArgMap['outputRoot'] = argMap[key]
            elif key == 'outputHIST_HLTMONL2File':
                signArgMap['outputRoot'] = argMap[key]
            else:
                signArgMap['applicationConfig'][key] = argMap[key]
                #if key == 'comdir':
                #    signArgMap['applicationConfig'][key] = trfDir

    print "BatchHLTTrf_wrap.py: signArgMap: " + str(signArgMap)
    
    # make pickle file
    curDir = os.path.abspath(os.getcwd())
    tmpFD,tmpName = tempfile.mkstemp(dir=curDir)
    tmpFH = os.fdopen(tmpFD, 'w')
    pickle.dump(signArgMap,tmpFH)
    tmpFH.close()
    
    # make .asetup file (fix for 16.1.2.8.1 TRIGGERDBREPR access problem)
    if argMap['release'].split(',')[1] == '16.1.2.8.1' and argMap['connection'] == 'TRIGGERDBREPR' :
        try:
            os.remove(".asetup")
        except:
            pass
        try:
            asetupFile = open(".asetup", "w")
            asetupFile.write('[defaults]\n')
            asetupFile.write('userpriority = True\n')
            asetupFile.write('[environment]\n')
            asetupFile.write('CORAL_DBLOOKUP_PATH = /afs/cern.ch/atlas/software/releases/16.1.2/AtlasCAFHLT/16.1.2.8.1/InstallArea/XML/AtlasAuthentication\n')
            asetupFile.write('CORAL_AUTH_PATH = /afs/cern.ch/atlas/software/releases/16.1.2/AtlasCAFHLT/16.1.2.8.1/InstallArea/XML/AtlasAuthentication\n')
            asetupFile.close()
            print '\n Created special .asetup for release', argMap['release'].split(',')[1], 'connection', argMap['connection']
        except:
            pass
            
    # execute original trf
    #com = "%s/%s --argdict=%s" % (trfDir,trfName,tmpName)
    com = "%s --argdict=%s" % (trfName,tmpName)
    retStat = os.system(com)
    try:
        os.remove(tmpName)
    except:
        pass
    
    ''' FIXME: rename output file '''
    try:
        listOfFiles = os.listdir('.')
        for fileName in listOfFiles:
            if 'Reproc.RAW' in fileName:
                newOutputFileName = argMap['outputRAWFile'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputFileName)
                os.rename(fileName, newOutputFileName) 
            if 'TrigRateL2.NTUP' in fileName:
                newOutputNTUP_TRIGRATEL2FileName = argMap['outputNTUP_TRIGRATEL2File'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputNTUP_TRIGRATEL2FileName)
                os.rename(fileName, newOutputNTUP_TRIGRATEL2FileName)
            if 'TrigCostL2.NTUP' in fileName:
                newOutputNTUP_TRIGCOSTL2FileName = argMap['outputNTUP_TRIGCOSTL2File'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputNTUP_TRIGCOSTL2FileName)
                os.rename(fileName, newOutputNTUP_TRIGCOSTL2FileName)
            if 'TrigRateEF.NTUP' in fileName:
                newOutputNTUP_TRIGRATEEFFileName = argMap['outputNTUP_TRIGRATEEFFile'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputNTUP_TRIGRATEEFFileName)
                os.rename(fileName, newOutputNTUP_TRIGRATEEFFileName)
            if 'TrigCostEF.NTUP' in fileName:
                newOutputNTUP_TRIGCOSTEFFileName = argMap['outputNTUP_TRIGCOSTEFFile'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputNTUP_TRIGCOSTEFFileName)
                os.rename(fileName, newOutputNTUP_TRIGCOSTEFFileName)
            if 'hlt-monitoring_EF' in fileName:
                newOutputHIST_HLTMONEFFile = argMap['outputHIST_HLTMONEFFile'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputHIST_HLTMONEFFile)
                os.rename(fileName, newOutputHIST_HLTMONEFFile)
            if 'hlt-monitoring_L2' in fileName:
                newOutputHIST_HLTMONL2File = argMap['outputHIST_HLTMONL2File'].split('#')[1]
                print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputHIST_HLTMONL2File)
                os.rename(fileName, newOutputHIST_HLTMONL2File)
            if 'Athena' in fileName:
                print "BatchHLTTrf_wrap.py: removing intermediate file %s " % fileName
                os.remove(fileName)
# no HIST file produced
#           if 'Reproc.HIST' in fileName:
#               newOutputHISTFileName = argMap['outputHISTFile'].split('#')[1]
#               print "BatchHLTTrf_wrap.py: rename %s to %s" % (fileName, newOutputHISTFileName)
#               os.rename(fileName, newOutputHISTFileName)
    except:
        pass
    
    ''' FIXME: grep logs for Oracle errors '''
    logName='CAF_'+signArgMap['_partid']+'.log'
    errMsg=''
    try:
        listOfFiles = os.listdir('.')
        for fileName in listOfFiles:
            if fileName.endswith(logName):
                print "BatchHLTTrf_wrap.py: checking log: " + fileName
                errMsg = commands.getoutput("grep -i 'CORAL/RelationalPlugins/oracle Error ORA' %s " % (fileName))
                if errMsg:
                    print errMsg
                    break
    except:
        pass

    if errMsg:
        sys.exit(134)

    sys.exit(retStat % 255)
    
