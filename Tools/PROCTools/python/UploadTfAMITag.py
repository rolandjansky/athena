#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys,pickle,os

##############################################################
#                                                            #
# UploadTfAMITag.py                                          #
# Script for creating and updating Tier-0 AMI tags w/ pyAMI  #
#                                                            #
##############################################################

HIProjTag = "data[0-9][0-9]_hi" # Only for Pb-Pb and p-Pb collisions (this regular expression matches with both "data11_hi" and "data11_hip")
HIpProjTag = "data[0-9][0-9]_hip" # Only for p-Pb collisions
ppProjTag = "data[0-9][0-9]_.*eV" # Only for pp data
cmProjTag = "data[0-9][0-9]_(cos|1beam|comm)" # only for commissioning
cpProjTag = "data[0-9][0-9]_(cos|1beam|.*eV|comm)" #For commissioning or pp but not HI
pcProjTag = "data[0-9][0-9]_(1beam|.*eV|comm)" #For commissioning or pp but not cosmics
cphipProjTag = "data[0-9][0-9]_(cos|1beam|.*eV|comm|hip)" # Above plus hip, i.e. all bar Pb-Pb HI
pphipProjTag = "data[0-9][0-9]_(.*eV|hip)" # pp or hip, for hip outputs in pp

#Setup script locations
setupScript = "/afs/cern.ch/atlas/tzero/software/setup/setuptrf.sh"
specialT0Setup_Oracle = "/afs/cern.ch/atlas/tzero/software/setup/specialsetup_tier0.sh" # Direct-Oracle - for everything but f-tags
specialT0Setup_Frontier = "/afs/cern.ch/atlas/tzero/software/setup/specialsetup_tier0_frontier.sh" # FronTier - for f-tags. Will also use for v tags with an alert

specialT0Setup = specialT0Setup_Oracle # By Default

#For Commissioning, Colissions and Heavy Ions
OutputsVsStreams = {

# The basics:
    'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)', 'HumanOutputs': 'always produced, except for DRAW input'},
    'outputAODFile': {'dstype': '!replace RAW AOD', 'ifMatch': cpProjTag, 'HumanOutputs': 'always produced except for ZeroBias stream.'},
    'outputTAGFile': {'dstype': 'TAG', 'ifMatch': HIProjTag+'(?!.(.*DRAW.*))', 'HumanOutputs': 'Produced in AOD merging'},
    'outputHISTFile': {'dstype': 'HIST', 'ifMatch': '(?!.(.*DRAW.*|.*debugrec.*))', 'HumanOutputs': 'always produced except for debug stream'},  # note was disabled for Pb-Pb HardProbes
    
# NTuples
    'outputNTUP_MUONCALIBFile': {'dstype': 'NTUP_MUONCALIB', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_IDCosmic\..*|.*physics_CosmicMuons\..*)',
                                 'HumanOutputs': 'produced for IDCosmic and CosmicMuons streams'},

# DRAW production 
   'outputDRAW_ZMUMUFile': {'dstype': 'DRAW_ZMUMU', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                              'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
   'outputDRAW_TAUMUHFile' : {'dstype': 'DRAW_TAUMUH', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
    ### These don't work in 20.1.5.4, code updates from Paul coming soon
    ### Added three new outputs in 20.1.5.5 - DRAW_EGZ, DRAW_EGJPSI, DRAW_EMU
   'outputDRAW_EGZFile'  : {'dstype': 'DRAW_EGZ', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                            'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
   #'outputDRAW_EGJPSIFile'  : {'dstype': 'DRAW_EGJPSI', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
   #                         'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
   'outputDRAW_EMUFile'  : {'dstype': 'DRAW_EMU', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                            'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},

# Special reconstruction outputs for DRAW
    #'outputDESDM_MSPerfFile'  : {'dstype': 'DESDM_ZMCP', 'ifMatch': cpProjTag+'(.*DRAW_ZMUMU.*)', 
    #                             'HumanOutputs': 'produced when reconstructing DRAW_ZMUMU'},
    'outputDESDM_ALLCELLSFile': {'dstype': 'replace RAW ESDM', 'ifMatch': cpProjTag+'(.*DRAW.*)(?!.*TAUMUH.*)', 
                                 'HumanOutputs': 'produced when reconstructing all DRAW formats'},

# DESDs made from full ESD files
    'outputDAOD_IDTRKVALIDFile' : {'dstype': 'DAOD_IDTRKVALID', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*MinBias.*|.*IDCosmic.*)',
                                 'HumanOutputs': 'produced for MinBias and IDCosmic streams.'},
    'outputDAOD_IDTRKLUMIFile' : {'dstype': 'DAOD_IDTRKLUMI', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*VdM.*|.*PixelBeam.*)',
                                 'HumanOutputs': 'produced for calibration streams.'},
    'outputDESDM_RPVLLFile': {'dstype': 'DESDM_RPVLL','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_JetTauEtMiss, physics_Egamma and physics_Muons streams.'},

}

#Print for debugging...
## for outputKey in OutputsVsStreams.keys():
##     try:
##         print outputKey,":",OutputsVsStreams[outputKey]['ifMatch']
##     except:
##         print outputKey,": always produced"

#-------------------------------------------------------------------------------------
def GetProcessConfigs(release,patcharea):
    rel = str(release)
    pa = str(patcharea)
    #print "Got release",release
    #print "Got Patch area",pa
    processConfigs = {}

    # HIST merging
    processConfigs['histmerge'] = {
        'inputs': {'inputHistFiles': {}},
        'outputs': {'outputHistFile': {'dstype': 'HIST'}},
        'phconfig': {},
        'transformation': 'DQM_Tier0Wrapper_trf.py',
        'tasktransinfo': {'trfpath': 'DQM_Tier0Wrapper_trf.py',
                          'trfsetupcmd': setupScript+' '+pa+' '+rel+' '+specialT0Setup }, 
        'description': 'Trf for combined DQM histogram merging and DQM webpage creation, to get periodic DQ monitoring updates. '}

    # TAG merging (no TAG_COMM anymore)
    processConfigs['tagmerge'] = {
        'inputs': {'inputTAGFile': {}},
        'outputs': {'outputTAG_MRGFile': {'dstype': 'TAG' }},
        'phconfig': {},
        'transformation': 'TAGMerge_tf.py',
        'tasktransinfo': {'trfpath': 'TAGMerge_tf.py', 
                          'trfsetupcmd': setupScript + ' '+pa+' '+rel+' none'},
        'description': 'Trf for TAG merging, runs CollAppend from release area. ' }
    
    # NTUP merging
    processConfigs['ntupmerge'] = {
        'inputs': {'inputNTUP_MUONCALIBFile': {}},
        'outputs': {'outputNTUP_MUONCALIB_MRGFile': {'dstype': '!likeinput'}},
        'phconfig': {}, #{'skipFileValidation': 'True'},
        'transformation': 'NTUPMerge_tf.py',
        'tasktransinfo': {'trfpath': 'NTUPMerge_tf.py', 
                          'trfsetupcmd': setupScript + ' '+pa+' '+rel+' none'},

        'description': 'Trf for plain-ROOT n-tuple merging. '}

    # AOD merging
    processConfigs['aodmerge'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputAOD_MRGFile': {'dstype': 'AOD'},
                    'outputTAGFile': {'dstype': 'TAG'}
                    },
        'phconfig': {'ignoreErrors': 'True', 'autoConfiguration': 'everything'},
        'transformation': 'AODMerge_tf.py',
        'tasktransinfo': {'trfpath': 'AODMerge_tf.py',
                          'trfsetupcmd':  setupScript+' '+pa+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged AODs plus associated physics TAGs with AODMerge_tf.py. '}

    # DAOD merging
    processConfigs['daodmerge'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputAOD_MRGFile': {'dstype': '!likeinput'}},
        'phconfig': {'ignoreErrors': 'True', 'autoConfiguration': 'everything'},
        'transformation': 'AODMerge_tf.py',
        'tasktransinfo': {'trfpath': 'AODMerge_tf.py',
                          'trfsetupcmd':  setupScript+' '+pa+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged DAODs plus associated physics TAGs with AODMerge_tf.py. '}

    # DPD merging
    processConfigs['dpdmerge'] = {
        'inputs': {'inputESDFile': {}},
        'outputs': {'outputESDFile': {'dstype': '!likeinput' }},
        'phconfig': {'ignoreerrors': 'ALL', 'autoConfiguration': 'everything', 'preExec': 'rec.doDPD.set_Value_and_Lock(False)'},
        'transformation': 'Merging_tf.py',
        'tasktransinfo': {'trfpath': 'Merging_tf.py',
                          'trfsetupcmd': setupScript+' '+pa+' '+rel+' '+specialT0Setup }, 
        'description': 'Produces merged DPDs with Merging_trf. ' }


    # Fast Physics Monitoring Ntuple
    processConfigs['fastmon'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputNTUP_FASTMONFile': {'dstype': 'NTUP_FASTMON'}},
        'phconfig': {'ignoreerrors': 'ALL', 'autoConfiguration': 'everything' },
        'transformation': 'Reco_tf.py',
        'tasktransinfo': {'trfpath': 'Reco_tf.py',
                          'trfsetupcmd':  setupScript+' '+pa+' '+rel+' '+specialT0Setup },
        'description': 'Produces Fast Physics Monitoring ntuple with Reco_tf.py  '}


    # Reco
    processConfigs['reco'] = {
        'inputs': None,
        'outputs': None,
        'phconfig': None,
        'transformation': 'Reco_tf.py',
        'tasktransinfo': {'trfpath': 'Reco_tf.py',
                          'trfsetupcmd': setupScript+' '+pa+' '+rel+' '+specialT0Setup }, 

        'description': 'Runs reconstruction with Reco_tf. ' }
    return processConfigs
#-------------------------------------------------------------------------------------

if __name__ == '__main__':
    if len(sys.argv) < 5 or len(sys.argv) > 8:
        print "##############"
        print "Application to create or update AMI tags (for manager only)\n"
        print "Usage:"
        print "UploadAMITag.py inputDictionary.pickle create_AMITag <release> <description> [patcharea] [updateConditionsTag]"
        print "or"
        print "UploadAMITag.py inputDictionary.pickle update_AMITag <release> <description> [patcharea] [updateConditionsTag]"
        print "\nTo create inputDictionary.pickle, execute:"
        print "Reco_tf.py --dumpPickle inputDictionary.pickle ..."
        print "or"
        print "CmdToPickledDic.py <command>"
        print "\nMake sure you have stored your pyAMI credentials encrypted in ~/.pyami, if not, please do:"
        print "\n setupATLAS"
        print " localSetupPyAMI"
        print " ami auth"
        print "\n and enter your AMI username and password"
        print "##############"
        sys.exit(0)


    inPickleFile=str(sys.argv[1])
    amiTagTmp=str(sys.argv[2])
    release=str(sys.argv[3])
    description=str(sys.argv[4])
    updateConditionsTag = True # Switch to enable/disable the CURRENT conditions tag update
    if len(sys.argv)>5:
        print sys.argv
        patcharea=str(sys.argv[5])
        if not os.access(patcharea,os.R_OK):
            print "WARNING Can't access patch area at",patcharea
        if not patcharea.startswith("/afs/cern.ch/"):
            print "WARNING Patch area does not start with /afs/cern.ch/ - this is probably a mistake!"
        if len(sys.argv)>6:
            try:
                updateConditionsTag = int(sys.argv[6])
            except:
                if sys.argv[6].lower() == "false":
                    updateConditionsTag = False

    else:
        patcharea="/afs/cern.ch/atlas/tzero/software/patches/"+release

    doWhat="Dunno"
    amiTag=''
    if amiTagTmp.startswith("create_"):
        doWhat="create"
        amiTag=amiTagTmp.replace("create_","")
    elif amiTagTmp.startswith("update_"):
        doWhat="update"
        amiTag=amiTagTmp.replace("update_","")

    if amiTag.startswith("f") or amiTag.startswith("v"):
        specialT0Setup=specialT0Setup_Frontier
        if amiTag.startswith("v"):
            print "INFO: This v-tag is being created with specialT0Setup =",specialT0Setup

    process=None
    if inPickleFile.endswith('.pickle'): 
        process='reco'
    elif (inPickleFile in ['histmerge', 'tagmerge', 'ntupmerge', 'aodmerge', 'daodmerge', 'dpdmerge', 'fastmon']):
        process = inPickleFile
    else:
        s = "Don't know how to interpret argument: '%s'.\n"%inPickleFile
        s += "Possible arguments are: histmerge, tagmerge, ntupmerge, aodmerge, daodmerge, dpdmerge\n"
        s += "...or for reconstruction tags: a pickeled dic named *.pickle\n"
        raise RuntimeError(s)

    #Check if release exists
    relSp=release.split("-")
    if len(relSp)!=2:
        s="ERROR: Expected parameter 'release' in the form Project-number, got "+releases
        raise RuntimeError(s)
    relProj=relSp[0]
    relNbr=relSp[1]
    baseRelNbr=".".join(relNbr.split(".")[:3])
    relPaths=("/afs/cern.ch/atlas/software/builds/"+relProj+"/"+relNbr,
              "/afs/cern.ch/atlas/software/releases/"+baseRelNbr+"/"+relProj+"/"+relNbr)
    for relPath in relPaths:
        if not os.path.isdir(relPath):
            s="ERROR Release directory " + relPath + " does not exists"
            #raise RuntimeError(s)
        #else:
        #    print "Found",relPath
    #Release exists in both releases and builds area if we reach this point

    processConfigs=GetProcessConfigs(release,patcharea)
    humanReadableOutputs=""

    if process=='reco':
        f = open(inPickleFile, 'r')
        startingDic = pickle.load(f)
        f.close()

        currentConditionsTag = ''
        if (amiTag.startswith("f") or amiTag.startswith("x")) and updateConditionsTag:
            # ConditionsTag in autoConfiguration is updated/appended
            # if f- or x-tag is made unless this option is disabled.
            sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
            # from PyCool import cool
            from AtlCoolBKLib import resolveAlias
            currentConditionsTag = 'ConditionsTag='
            if(amiTag.startswith("f")): currentConditionsTag += resolveAlias.getCurrent()
            else:                       currentConditionsTag += resolveAlias.getCurrentES()
            # Starting in 2012 all magnetic field configs are in one tag. Don't put this in the autoconfiguration part.
            if '*' not in currentConditionsTag:
                currentConditionsTag = currentConditionsTag[14:]
        else:
            # ConditionsTag in autoConfiguration is not changed.
            updateConditionsTag = False

        #prepare input, output and config dictionaries
        inputDic={}
        outputDic={}
        configDic={}

        print startingDic

        from PATJobTransforms.Configuration import ConfigDic
        didConditionsUpdate = False
        for key in startingDic.keys():
            if ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isInput"):
                if amiTag.startswith("q"): # only write the input file for q-tags
                    inputDic[key]=startingDic[key]
                else:
                    inputDic[key]={}
                #print "inputDic[%s] = %s" % (key, inputDic[key])

            elif ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isOutput") and (key!="tmpESD" and key!="tmpAOD"):
                try:
                    #print "key: " , key , " isOutput"
                    outputDic[key]=OutputsVsStreams[key]
                except:
                    print "Known outputs defined in the OutputsVsStreams dictionary are:"
                    print OutputsVsStreams
                    raise RuntimeError("Don't know what to do with out key %s, please add it to OutputsVsStreams to use it"%key)
            elif key=='autoConfiguration' and updateConditionsTag and 'Conditions' in currentConditionsTag: # legacy: used for autoconfiguration of conditions like COMCOND-ES1P*-005-04
                if(startingDic[key].find(currentConditionsTag)>=0):
                    print '*** autoConfiguration: %s (CURRENT) is already specified ***' % (currentConditionsTag)
                elif(startingDic[key].find('ConditionsTag')>=0): # ConditionsTag is already defined.
                    autoConfs = startingDic[key].split(',')
                    for i in range(len(autoConfs)):
                        if(autoConfs[i][0:13]=='ConditionsTag'):
                            print '*** autoConfiguration: %s is replaced by %s (CURRENT) ***' % (autoConfs[i], currentConditionsTag)
                            autoConfs[i] = currentConditionsTag
                        if(i==0): startingDic[key] = autoConfs[i]
                        else: startingDic[key] += ','+autoConfs[i]
                else: # ConditionsTag is not yet defined.
                    print '*** autoConfiguration: %s (CURRENT) is appended ***' % (currentConditionsTag)
                    startingDic[key] += ','+currentConditionsTag
                configDic[key]=startingDic[key]
                didConditionsUpdate=True

            elif key=='conditionsTag' and updateConditionsTag:
                if(startingDic[key].find(currentConditionsTag)>=0):
                    print '*** conditionsTag: %s (CURRENT) is already specified ***' % (currentConditionsTag)
                else:
                    print '*** conditionsTag: %s is replaced by %s (CURRENT) ***' % (startingDic[key], currentConditionsTag)
                    startingDic[key]=currentConditionsTag
                configDic[key]=startingDic[key]
                didConditionsUpdate=True
            else:
                configDic[key]=startingDic[key]
                #print "key: " , key , " is neiter input nor output..."

        if updateConditionsTag and not didConditionsUpdate:
            # update conditions
            print '*** conditionsTag: %s (CURRENT) is appended ***' % (currentConditionsTag)
            configDic['conditionsTag']=currentConditionsTag
    
        #Special treatment for maxEvents and AMITag, in configDic
        configDic['maxEvents'] = '-1'
        if amiTag.startswith("q"):
            configDic['maxEvents'] = '25'
        configDic['AMITag']=str(amiTag)

        # remove AMIConfig
        if 'AMIConfig' in configDic.keys():
            del configDic['AMIConfig']

        #Special treatment of outputs for Tier0 internal manipulations (hopefully temporary)
        for key in outputDic.keys():
            dsType=outputDic[key]['dstype']
            #outputDic[key]['dsnamefion']='replace daq.RAW recon.%s merge.RAW recon.%s'%(dsType,dsType)
            #if key=='outputESDFile': outputDic[key]['storageinfo']='CASTORPERM'
            #else: outputDic[key]['storageinfo']='CASTORTEMP' #<---not needed, already the default

        for key in outputDic.keys():
            humanOut=outputDic[key].pop('HumanOutputs')
            if amiTag.startswith("x"):
                humanReadableOutputs+=outputDic[key]['dstype']+", "
            else:
                humanReadableOutputs+=outputDic[key]['dstype']+": "+humanOut+" "
        if amiTag.startswith("x"):
            humanReadableOutputs+="produced for prompt processing of express stream (ES-1)."
            
        processConfigs['reco']['inputs']=inputDic
        processConfigs['reco']['outputs']=outputDic
        processConfigs['reco']['phconfig']=configDic

    #-----------------------------
    # Tier0 specific 'moreInfo'. Hopefully temporary.
    moreInfoDic={}
    moreInfoDic['tasktransinfo']=processConfigs[process]['tasktransinfo']

    try:
        import pyAMI.client
    except ImportError:
        print "WARNING unable to import pyAMI.client"
    amiclient=pyAMI.client.Client('atlas')

    #------------------------
    #Build final AMI tag info
    s={} #stable values
  
    #s['AMIUser']=login
    #s['AMIPass']=password

    c={} #changing values
    c['phconfig']=str(processConfigs[process]['phconfig'].__str__())
    c['inputs']=str(processConfigs[process]['inputs'].__str__())
    c['outputs']=str(processConfigs[process]['outputs'].__str__())
    if humanReadableOutputs: c['humanReadableOutputs']=str(humanReadableOutputs)
    c['SWReleaseCache']=str(release).replace('-','_')
    c['transformation']=str(processConfigs[process]['transformation'])
    c['description']=processConfigs[process]['description']+description+" Using "+release
    c['trfsetupcmd']=str(moreInfoDic['tasktransinfo']['trfsetupcmd'])
    c['moreInfo']=str(moreInfoDic.__str__())
    if amiTag.startswith("f") or amiTag.startswith("x") or amiTag.startswith("q") or amiTag.startswith("v") or amiTag.startswith("c"):
        c['Geometry']='Auto-config. See dataset info.'
        if not amiTag.startswith("x"):
            c['ConditionsTag']='Auto-config. See dataset info.'

    #print c['Human-readable outputs']

    
    #Upload info to AMI
    if doWhat=="create":
        # This should just be a letter. the number will be assigned by AMI
        s['tagType']=amiTag[0]
        l=['AddAMITag']
        for k in s.keys():
            l.append(k+'="'+s[k].replace('"','\\"')+'"')
        for k in c.keys():
            l.append(k+'="'+c[k].replace('"','\\"')+'"')

        print "\nThis is the command that will be sent to AMI:\n"
        print "l=",l
        result=amiclient.execute(l,format='text')
        print "\nHere's what AMI replied:\n\n%s" % result

    elif doWhat=="update":
        s['amiTag']=amiTag
        l=['UpdateAMITag']
      
        for k in s.keys():
            l.append(k+'="'+s[k].replace('"','\\"')+'"')
            
        for k in c.keys():
            if (k=='transformation') :
                # transformation cannot be changed in an update.
                continue
            l.append(k+'="'+c[k].replace('"','\\"')+'"')
            
        print "\nThis is the command that will be sent to AMI:\n"
        print "l=",l
        result=amiclient.execute(l)
        print "\nHere's what AMI replied:\n\n%s" % result

    else:
        raise SyntaxError("Don't know what to do... amiTagTmp='%s'  doWhat='%s'"%(amiTagTmp,doWhat))

    print "\nTo see the corresponding command, go to:\n"
    print "https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?linkId=10226"
    print "\n"
