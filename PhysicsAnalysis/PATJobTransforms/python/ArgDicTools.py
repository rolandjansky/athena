#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (DESY), September 2008
# Usage:
#   -transform input arguments into a dictionary

import os
import pickle
import pprint
from subprocess import Popen, STDOUT, PIPE, CalledProcessError
import sys
import time

# This function looks for known exceptions occuring when interpreting sysArgs
def SysArgsExceptionCatcher(sysArgs):
    #Help should be provided if the transform is executed withtout argument, e.g. like this: "BStoESDAODDPD_trf.py"
    if len(sysArgs) is 1:
        return "Help"    

    #Help should be provided if the transform is executed with -h argument, e.g. like this: "BStoESDAODDPD_trf.py -h"
    if len(sysArgs) is 2:
        if sysArgs[1] == "-h":
            return "Help"
        
    #No known exception found, return OK
    return "OK"

# This function looks for known exceptions occuring when interpreting sysArgs
def KeyExceptionCatcher(key):
    #This allowed for special options like: --ignoreunknown, -h, etc.
    if key.startswith('-') and key!='--argdict' and key!='--athenaopts':
        return "OK"
    #Unknown exepction... 
    return "ERROR"


def PickleToDico(sysArgv):
    #Expect: Transform.py --argdict=NameOfLocalFile
    #Just get the input dictionary
    if sysArgv[1].startswith('--argdict='):
        fname=sysArgv[1][len('--argdict='):]
        f = open(fname, 'r')
        dic = pickle.load(f)
        f.close()
        print "Successfully interpreted command line: method pickled argDict..."               
        return dic
    else:
        return False


def SysArgvToDico(sysArgv):
    #Expect: Transform.py arg1=value1 ... argN=valueN
    #Create an input dictionary from sysArgv
    dic={}
    #loop over sysArgv values, excluding 0th element
    for arg in sysArgv[1:]:
        try:
            eqPos=arg.index('=')
            key=arg[:eqPos]
            value=arg[eqPos+1:]
            dic[key]=value
            if key is '--argdict':
                print "WARNING - pickled dic method: use PickleToDico()"
                return False
        except:
            if KeyExceptionCatcher(arg) is "OK":
                dic[arg]=''
                print "Special arg: %s accepted..."%arg
            else:
                print "WARNING - positional argument method: use PositionalToDico()"
                return False
    print "Successfully interpreted command line: method arg=value..."               
    return dic


def PositionalToDico(sysArgv):
    #Expect: Transform.py value1 ... valueN, with no key names.
    #In this case, let the transform determines its default behavior.
    print "Positional value method. Returning dic['defaultFromPositionalValues']=True."
    dic={}
    dic['defaultFromPositionalValues']=True
    return dic


def DicHasOutputs(aDic):
    from PATJobTransforms.Configuration import ConfigDic
    for key in aDic.keys():
        if ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isOutput"):
            return True
    return False


def DicInputs(aDic):
    from PATJobTransforms.Configuration import ConfigDic
    for key in aDic.keys():
        if ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isInput"):
            return aDic[key]
    return ""


## @brief Deprecate this now
def GetAMIClient(useReplica=False):
    pprint.pprint(' '.join(["The old job transforms framework is now _deprecated_.",
                            "There is no support for the use of AMI and the old transforms will be removed",
                            "shortly from the release. Please migrate to the new transforms framework.",
                            "https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/JobTransform#New_Style_Transforms"]),
                  stream = sys.stderr) 
    sys.exit(1)

def BuildDicFromCommandLineIgnoreAMI(sysArgv):
    if SysArgsExceptionCatcher(sysArgv) is "Help":
        inDic={}
        inDic['-h']=''
    else:
        inDic = PickleToDico(sysArgv)
        if not inDic:
            inDic=SysArgvToDico(sysArgv)
            if not inDic:
                inDic=PositionalToDico(sysArgv)
                if not inDic:
                    raise RuntimeError("Unable to create input dictionary from sys.argv")

    if inDic.has_key('keepFullCommandUntouched'):
        from PATJobTransforms.TrfFlags import trfFlags
        val=inDic.pop('keepFullCommandUntouched')
        if val=="no" or val=="NO" or val=="False" or val=="false":
            print "INFO DRAW synonyms will be resolved"
            trfFlags.KeepFullCommandUntouched=False
        else:
            print "INFO DRAW synonyms will be kept as they are. Might cause a failure at the end of the transform"
            trfFlags.KeepFullCommandUntouched=True


    if inDic.has_key('applyIfMatchPattern'):
        from PATJobTransforms.TrfFlags import trfFlags
        val=inDic.pop('applyIfMatchPattern')
        if val=="no" or val=="NO" or val=="False" or val=="false":
            print "INFO Will produce all outputs regardless of the input stream name"
            trfFlags.ApplyIfMatchPattern=False
        else:
            print "INFO Will only produce output that match the input stream ifMatch pattern"
            trfFlags.ApplyIfMatchPattern=True

            
            
    PopSynonyms(inDic)
    return inDic


def GetInfoFromAMIXML(amitag, suppressPass = True):
    try:
        import Eowyn.luxml as luxml
    except ImportError:
        print "WARNING unable to import luxml with standard $PYTHONPATH."
        print "Will manually add tzero/prod1/code, then try again..." 
        import sys
        sys.path.insert(0,'/afs/cern.ch/atlas/project/tzero/prod1/code')
        import Eowyn.luxml as luxml
        sys.path.pop(0)
        print "import luxml was succesful"

    #get dic from AMI
    amiclient=GetAMIClient()
    l=['ListConfigurationTag','-configTag='+amitag]
    result=amiclient.execute(l)
    dicOfDico=result.getDict()
    xmlstr = str(dicOfDico[u'rowset_'+amitag][u''+amitag][u'moreInfo'])
    amiPhysDic = luxml.toPy(xmlstr)['phconfig']
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'transformation']
    amiTransform=str(strDic)
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'SWReleaseCache']
    amiRelease=str(strDic)
    results={}
    results['amiPhysDic']=amiPhysDic
    results['amiInputDic']={}
    results['amiOuputDic']={}
    results['amiTransform']=amiTransform
    results['amiRelease']=amiRelease

    #tweak to facilitate compatibility with ProdSys 
    #ReleaseDic={}
    #ReleaseDic['Release']=amiRelease
    #results['amiRelease']=ReleaseDic

    return results


def GetInfoFromAMIPython(amitag, suppressPass = True):
    #get dics from AMI
    amiclient=GetAMIClient()
    l=['ListConfigurationTag','configTag={0}'.format(amitag)]
    
    try:
        result=amiclient.execute(l)
    except Exception, e:
        print "WARNING problem in amiclient.execute, try using CERN replica instead ({0})".format(e)
        amiclient=GetAMIClient(useReplica=True)
        try:
            result=amiclient.execute(l)
        except Exception:
            print "FATAL could not execute AMI-command. Will reraise last exception for debugging."
            raise
        
    dicOfDico=result.to_dict()
    #configuration is a python dic in string format, get back to real python using exec 
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'phconfig']
    exec("amiPhysDic="+strDic)
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'inputs']
    if (amitag=='q130'): strDic="{'inputHitsFile': {}, 'NDMinbiasHitsFile': {}, 'cavernHitsFile': {}}"
    exec("amiInputDic="+strDic)
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'outputs']
    exec("amiOuputDic="+strDic)
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'transformation']
    amiTransform=str(strDic)
    strDic=dicOfDico[u'rowset_'+amitag][u''+amitag][u'SWReleaseCache']
    amiRelease=str(strDic)

    results={}
    results['amiPhysDic']=amiPhysDic
    results['amiInputDic']=amiInputDic
    results['amiOuputDic']=amiOuputDic
    results['amiTransform']=amiTransform
    results['amiRelease']=amiRelease
    
    #tweak to facilitate compatibility with ProdSys 
    #ReleaseDic={}
    #ReleaseDic['Release']=amiRelease
    #results['amiRelease']=ReleaseDic

    return results

def GetInfoFromAMI(amiTag):
    if amiTag=="q109":
        print "\n\n\nAMI tag q109 has been superceded by q116, which does the same thing but has an updated syntax that the Tier0 can better read."
        print "\n\n***   Please try again using the same command but AMI=q116 instead of AMI=q109   ***\n\n\n"
        sys.exit(0)
    try:
        info=GetInfoFromAMIPython(amiTag)
    except KeyError:
        print "unable to interpret AMI tag as Python. Will try with XML."
        try:
            info=GetInfoFromAMIXML(amiTag)
        except:
            raise RuntimeError("Unable to interpret AMI tag!")
    return info


def AppendDic1WithDic2(dic1,dic2):
    pattern='append_'
    for key in dic2.keys():
        if key.startswith(pattern):
            appKey=key[len(pattern):]
            appValue=dic2.pop(key)
            orig=None
            if dic1.has_key(appKey):
                orig=dic1[appKey]
                dic1[appKey]=[orig,appValue]
            else:
                dic1[appKey]=appValue
            print "INFO appended key: %s. Original value: %s. New value: %s."%(appKey,orig,dic1[appKey])
    return


def PopSynonyms_DRAWOutput(aDic):
    from PATJobTransforms.TrfFlags import trfFlags
    if trfFlags.KeepFullCommandUntouched():
        return

    validSynonyms={}
    validSynonyms['outputDESD_ZEEFile']='outputESDFile'
    validSynonyms['outputDESD_ZMUMUFile']='outputESDFile'
    validSynonyms['outputDESD_WENUFile']='outputESDFile'
    validSynonyms['outputDESD_WMUNUFile']='outputESDFile'
    validSynonyms['outputDAOD_ZEEFile']='outputAODFile'
    validSynonyms['outputDAOD_ZMUMUFile']='outputAODFile'
    validSynonyms['outputDAOD_WENUFile']='outputAODFile'
    validSynonyms['outputDAOD_WMUNUFile']='outputAODFile'
    for oldKey in validSynonyms.keys():
        if aDic.has_key(oldKey):
            newKey=validSynonyms[oldKey]
            print "INFO Argument '%s' replaced by synonym '%s'."%(oldKey,newKey)
            newValue=aDic.pop(oldKey)
            if aDic.has_key(newKey):
                print "WARNING argument '%s' specified multiple times. Current value '%s' kept, new value '%s' ignored."%(newKey,aDic[newKey],newValue)
            else:
                aDic[newKey]=newValue


def PopSynonyms(aDic):

    obsoleteArgs={}
    obsoleteArgs['DESD_IDCOMM']='outputDESD_IDCOMMFile'
    obsoleteArgs['DESD_PIXELCOMM']='outputDESD_PIXELCOMMFile'
    obsoleteArgs['DESD_MUONCOMM']='outputDESD_MUONCOMMFile'
    obsoleteArgs['DESD_TILECOMM']='outputDESD_TILECOMMFile'
    obsoleteArgs['DESD_CALOCOMM']='outputDESD_CALOCOMMFile'
    obsoleteArgs['DESD_PHOJET']='outputDESD_PHOJETFile'
    obsoleteArgs['DESD_SGLMU']='outputDESD_SGLMUFile'
    obsoleteArgs['DESDM_TRACK']='outputDESDM_TRACKFile'
    obsoleteArgs['DESDM_MUON']='outputDESDM_MUONFile'
    obsoleteArgs['DESD_MET']='outputDESD_METFile'
    obsoleteArgs['DESD_MBIAS']='outputDESD_MBIASFile'
    obsoleteArgs['DESDM_EGAMMA']='outputDESDM_EGAMMAFile'
    obsoleteArgs['DESDM_CALJET']='outputDESDM_CALJETFile'
    obsoleteArgs['DESD_SGLEL']='outputDESD_SGLELFile'
    obsoleteArgs['outputNTUP_TRIG']='outputNTUP_TRIGFile'
    obsoleteArgs['outputCBNT']='outputCBNTFile'
    obsoleteArgs['outputPixelCalibNtup']='outputNTUP_TRKVALIDFile'
    obsoleteArgs['outputNTUP_PIXELCALIBFile']='outputNTUP_TRKVALIDFile'
    obsoleteArgs['outputMuonCalibNtup']='outputNTUP_MUONCALIBFile'
    obsoleteArgs['outputTAGComm']='outputTAG_COMMFile'
    obsoleteArgs['HIST']='outputHISTFile'
    obsoleteArgs['outputD2PD_TOPFile']='outputDAODM2_TOPFile'
    obsoleteArgs['outputDAODM_TOPFile']='outputDAODM2_TOPFile'
    obsoleteArgs['outputDESDM_CALJETFile']='outputDESD_CALJETFile'
    obsoleteArgs['outputDESD_METFile']='outputDESDM_METFile'
    obsoleteArgs['Geometry']='geometryVersion'
    for oldKey in obsoleteArgs.keys():
        if aDic.has_key(oldKey):
            newKey=obsoleteArgs[oldKey]
            print "WARNING Argument '%s' is obsolete! Please use '%s' instead."%(oldKey,newKey)
            newValue=aDic.pop(oldKey)
            if aDic.has_key(newKey):
                print "WARNING argument '%s' specified multiple times. Current value '%s' overwritten by new value '%s'."%(newKey,aDic[newKey],newValue)
            aDic[newKey]=newValue
    
    if aDic.has_key('extraParameter'):
        extraP=aDic.pop('extraParameter')
        print "INFO extraParamater=%s is removed. This pseudo-argument is stricly internal to ProdSys."%(extraP)
    return


def UpdateDicListWithAMI(userDic,amiTag):
    # Check for tags handled by panda, not AMI
    if amiTag[0] in 'rpdsea':
        from PATJobTransforms.ProdSysDicTools import GetInfoFromPANDA
        infoList=GetInfoFromPANDA(amiTag) 
    else:
        infoList=[GetInfoFromAMI(amiTag)]

    outList=[]
    for info in infoList:
        d={}
        outDic,outInfo=UpdateDicWithAMI(userDic,amiTag,info)
        d['info']=outInfo
        d['outDic']=outDic
        #d['Release']=outInfo['amiRelease']
        outList.append(d)

    return outList


def UpdateDicWithAMI(userDic,amiTag,info):
    amiInputDic=info['amiInputDic']
    amiOuputDic=info['amiOuputDic']
    amiPhysDic=info['amiPhysDic']

    PopSynonyms(amiInputDic)
    PopSynonyms(amiOuputDic)
    PopSynonyms(amiPhysDic)
    
    #Now update userDic, becoming outDic
    outDic={}
    #The rules are:
    # 1) if userDic specifies an input, use it. Otherwise use the default
    # 2) if userDic specifies one output, overwrite whole amiOuputDic. Otherwise use all amiOuputDic with default values.
    # 3) any physConfig specified in userDic overwrites amiConfigDic, unless the append option is used.

    #inputs
    inputFileValue=DicInputs(userDic)
    if inputFileValue=="":
        print "\n"
        if len(amiInputDic.keys())>0:
            from PATJobTransforms.DefaultInputs import DefaultInputs
            #inKey=amiInputDic.keys()[0]
            for inKey in amiInputDic.keys():
                if DefaultInputs.has_key(inKey):
                    inputFileValue=DefaultInputs[inKey]
                    if amiTag=="q120": inputFileValue=DefaultInputs["cosmicsBS"]                
                    elif amiTag=="q126": inputFileValue=DefaultInputs["topBS"]                
                    outDic[inKey]=inputFileValue
                    print "INFO Using default input value: %s=%s"%(inKey,outDic[inKey])
                else:
                    raise RuntimeError("Key %s is not defined in DefaultInputs"%inKey)
    
    #outputs
    #if no output is specified, use default values for all those specified in AMI tag if input matches regular expression 
    if DicHasOutputs(userDic):
        print "\nUsing outputs specified by user, bypassing those from AMI."
    else:
        print "\n"
        from PATJobTransforms.Configuration import ConfigDic
        from PATJobTransforms.TrfFlags import trfFlags
        import re
        for key in amiOuputDic.keys():
            if not ConfigDic.has_key(key):
                raise RuntimeError("Key %s from amiOutputDic is not known by job transform ConfigDic"%key)
            if not hasattr(ConfigDic[key],"isOutput"):
                raise RuntimeError("Key %s from amiOutputDic is not known as an output by job transform ConfigDic"%key)

            pattern=".*" #trivial pattern that always matches
            if amiOuputDic[key].has_key('ifMatch'):
                pattern=amiOuputDic[key]['ifMatch'] #overwrites trivial pattern with the one from AMI

            idx=inputFileValue.rfind("/")
            if idx != -1:
                inputFileValue=inputFileValue[1+idx:]
                print "Reduced input file Name:",inputFileValue

            if (not trfFlags.ApplyIfMatchPattern()) or re.match(pattern,inputFileValue):
                type=ConfigDic[key].isOutput
                defaultValue=None
                if type=='bs':
                    defaultValue='my'+str(amiOuputDic[key]['dstype'])+'.data'
                elif type=='root':
                    defaultValue='my'+str(amiOuputDic[key]['dstype'])+'.root'
                elif type=='pool':
                    defaultValue='my'+str(amiOuputDic[key]['dstype'])+'.pool.root'
                else:
                    raise RuntimeError("Don't know to define a default value for type %s"%type)            
                outDic[key]=defaultValue
                print "INFO Using default output value: %s=%s"%(key,outDic[key])
            else:
                print "INFO %s not produced since input file '%s' does not match pattern '%s'."%(key,inputFileValue,pattern)

        
    #physics_configuration: take it all
    outDic.update(amiPhysDic)
    #now update outDic with everything from userDic (including input/output if relevant)
    #first look for append option...
    AppendDic1WithDic2(outDic,userDic)
    #print "OutDict after append",outDic
    
    #at this point userDic will supercede what was in AMI in case of conflicts
    outDic.update(userDic)

    #Call PopSynonyms_DRAWOutput here and in BuildDicFromCommandLine to be sure it's executed in any case
    PopSynonyms_DRAWOutput(outDic)
    
    return outDic,info



def BuildDicFromCommandLine(sysArgv,returnList=False):
    print "###############################"
    print "Original job transform command:"
    # Take a bit of care with quotes - argv[0] shouldn't need them, nor should any of the 'keys'
    # But with the other stuff, we enclose them in single quotes and quote out any existing single
    # quotes
    origCmd = sysArgv[0] + ' '
    for i in sysArgv[1:]:
        if i.find('=') > -1:
            key, value = i.split('=', 1)
            origCmd += key + '=' + "'" + value.replace("'", "'\\''") + "'" + ' '
        else:
            origCmd += "'" + i.replace("'", "'\\''") + "'" + ' '
    print origCmd
    print "###############################"

    dicList=[] #only used by GetCommand.py
    inDic=BuildDicFromCommandLineIgnoreAMI(sysArgv)

    #dynamicly created user outputs
    for key in inDic.keys():
        from PATJobTransforms.OutputsMgr import outputsMgr,magicKey
        if key.startswith(magicKey):
            outputsMgr.addTrfOutput(key)
            inDic['--test']=''
            inDic['--omitvalidation']='ALL'
            print "BLABLABLA_1",outputsMgr.userOutputList

    if inDic.has_key('AMI'):
        amiTag=inDic.pop('AMI')
        dicList=UpdateDicListWithAMI(inDic,amiTag)
        inDic=dicList[0]['outDic']
    
    # Short report on atlas setup
    print "###############################"
    print 'Atlas Setup Report:'
    for eVar in ('AtlasVersion', 'AtlasProject','AtlasPatch', 'AtlasPatchVersion', 'CMTCONFIG','TestArea'):
        if eVar in os.environ:
            print '\t%s=%s' % (eVar, os.environ[eVar])
        else:
            print '\t%s undefined' % eVar
    # Look for patches so that the job can be rerun 
    if 'TestArea' in os.environ and os.access(os.environ['TestArea'], os.R_OK):
        print "Patch packages are:"
        try:
            cmd = ['cmt', 'show', 'packages', os.environ['TestArea']]
            cmtProc = Popen(cmd, shell = False, stdout = PIPE, stderr = STDOUT, bufsize = 1)
            cmtOut = cmtProc.communicate()[0] 
            for line in cmtOut.split('\n'):
                try:
                    if line.strip() == '':
                        continue
                    (package, packageVersion, packagePath) = line.split()
                    print '\t%s' % (packageVersion)
                except ValueError:
                    print "Warning, unusual output from cmt: %s" % line 
        except (CalledProcessError, OSError), e:
            print 'Execution of CMT failed: %s' % e
    else:
        print "No readable patch area found"
    print "###############################"


    PopSynonyms_DRAWOutput(inDic) #Call a second time to make sure it's executed even if no input comes from AMI

    #Write out inDic in a pickle file
    import pickle
    f = open('inputDictionary.pickle', 'w')
    pickle.dump(inDic, f)
    f.close()
    print "INFO trf configuration written in inputDictionary.pickle"

    if returnList:
        return dicList  #only used by GetCommand.py

    return inDic


def addDefaultArgumentFromPositionalValue(dic,key,value):
    #in itself this method is trivial, but it's required for Tier1-style trf introspection with grep (see e.g. Reco_trf)
    if not dic.has_key(key):
        if value=='NONE' or value=='none': 
            print "Ignored key '%s' with value '%s'"%(key,value)
        elif KeyExceptionCatcher(key)=="OK":
            dic[key]=''
            print "%s=''"%key
        else:
            dic[key]=value
            print "%s=%s"%(key,value)
    else:
        raise RuntimeError("dic key '%s' is already defined. Forbidden!"%key)
    return


def DefaultConfigFromSysArgv(ListOfDefaultPositionalKeys,dic):
    dic.clear()
    #Configure default with positional values from sys.argv
    import sys
    if len(sys.argv) > (len(ListOfDefaultPositionalKeys)+1):
        print "sys.argv:",sys.argv
        print "ListOfDefaultPositionalKeys:",ListOfDefaultPositionalKeys
        raise RuntimeError("Default configuration undefined: too many values in sys.argv")
        
    print "DefaultConfigFromSysArgv..."
    i=0
    for val in sys.argv[1:]:
        addDefaultArgumentFromPositionalValue(dic,key=ListOfDefaultPositionalKeys[i],value=val)
        i+=1

    return dic

