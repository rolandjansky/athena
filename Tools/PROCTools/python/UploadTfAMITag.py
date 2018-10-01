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
cphiProjTag = "data[0-9][0-9]_(cos|1beam|.*eV|comm|hi)" # Above plus hi, HI
pphipProjTag = "data[0-9][0-9]_(.*eV|hip)" # pp or hip, for hip outputs in pp

#Setup script locations
setupScript = "/afs/cern.ch/atlas/tzero/software/setup/usetuptrf.sh"
specialT0Setup = ""

#For Commissioning, Colissions and Heavy Ions
OutputsVsStreams = {

# The basics:
    ## modification for pp reference run - only produce for the express stream
    #'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)', 'HumanOutputs': 'always produced, except for DRAW input'},
    ## special version for 2 part of HI
    #'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(.*express.*|.*L1Calo.*|.*ZeroBias.*|.*CosmicCalo.*|.*Late.*)', 'HumanOutputs': 'always produced, except for DRAW input'},
    ## previous pp setup
    ##'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(?!.*physics_Main.*)', 'HumanOutputs': 'always produced, except for DRAW input and physics_Main'},
    ## setup for cosmics
    #'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(?!.*IDCosmic.*)', 'HumanOutputs': 'always produced, except for DRAW input and IDCosmic'},
    #'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)', 'HumanOutputs': 'always produced, except for DRAW'},
    #'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(?!.*physics_Main.*)', 'HumanOutputs': 'always produced, except for DRAW input and physics_Main'},
    'outputESDFile': {'dstype': '!replace RAW ESD', 'ifMatch': '(?!.*DRAW.*)(?!.(.*physics_Main\..*|.*Background.*|.*L1Topo.*))', 'HumanOutputs': 'always produced, except for DRAW input and physics_Main'},
    'outputAODFile': {'dstype': '!replace RAW AOD', 'ifMatch': cphiProjTag+'(?!.*DRAW_RPVLL.*)(?!.*Background.*)', 'HumanOutputs': 'always produced except for DRAW_RPVLL.'},
    'outputTAGFile': {'dstype': 'TAG', 'ifMatch': 'data[0-9][0-9]_(cos|1beam|.*eV|comm)(.*express_express\..*)(?!.(.*DRAW.*))', 'HumanOutputs': 'Produced in express stream'},
    'outputHISTFile': {'dstype': 'HIST', 'ifMatch': '(?!.(.*DRAW.*|.*debugrec.*))', 'HumanOutputs': 'always produced except for DRAW and debugrec'},  # note was disabled for Pb-Pb HardProbes
    'outputRDOFile': {'dstype': 'RDO', 'ifMatch': cphiProjTag, 'HumanOutputs': 'always produced.'},
# NTuples
#    'outputNTUP_MUONCALIBFile': {'dstype': 'NTUP_MUONCALIB', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_IDCosmic\..*|.*physics_CosmicMuons\..*)',
#                                 'HumanOutputs': 'produced for IDCosmic and CosmicMuons streams'},

### put it to all streams for M10 tags
#    'outputNTUP_MUONCALIBFile': {'dstype': 'NTUP_MUONCALIB', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(?!.*Background.*)',
#                                 'HumanOutputs': 'always produced except for DRAW'},

# DRAW production 
   'outputDRAW_ZMUMUFile': {'dstype': 'DRAW_ZMUMU', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                              'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
   'outputDRAW_TAUMUHFile' : {'dstype': 'DRAW_TAUMUH', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
    ### These don't work in 20.1.5.4, code updates from Paul coming soon
    ### Added three new outputs in 20.1.5.5 - DRAW_EGZ, DRAW_EGJPSI, DRAW_EMU
   'outputDRAW_EGZFile'  : {'dstype': 'DRAW_EGZ', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                            'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
   #'outputDRAW_EGJPSIFile'  : {'dstype': 'DRAW_EGJPSI', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
   #                         'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},
    'outputDRAW_EMUFile'  : {'dstype': 'DRAW_EMU', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},

    'outputDRAW_RPVLLFile'  : {'dstype': 'DRAW_RPVLL', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                               'HumanOutputs': 'produced for collision runs, for the physics_Main stream.'},

    'outputDRAW_BCID1File'  : {'dstype': 'DRAW_BCID1', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*|.*physics_EnhancedBias\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main & EnhancedBias stream.'},
    'outputDRAW_BCID2File'  : {'dstype': 'DRAW_BCID2', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*|.*physics_EnhancedBias\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main & EnhancedBias stream.'},

    'outputDRAW_TOPSLMUFile'  : {'dstype': 'DRAW_TOPSLMU', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                             'HumanOutputs': 'produced for collision runs, for the physics_Main.'},

# Special reconstruction outputs for DRAW
#    'outputDESDM_MSPerfFile'  : {'dstype': '!replace RAW DESDM_MCP DDESDM_MCP_ZMUMU DESDM_ZMCP', 'ifMatch': pcProjTag+'(.*physics_Main\..*|.*CosmicMuons.*)(.*\.RAW\.*|.*DRAW_ZMUMU.*)',
#                                 'HumanOutputs': 'produced when reconstructing DRAW_ZMUMU'},
#    'outputDESDM_MSPerfFile'  : {'dstype': 'DESDM_ZMCP', 'ifMatch': pcProjTag+'(.*DRAW_ZMUMU.*)', 
#                                 'HumanOutputs': 'produced when reconstructing DRAW_ZMUMU'},
    'outputDESDM_ALLCELLSFile': {'dstype': '!replace RAW ESDM', 'ifMatch': pcProjTag+'(?!.*DRAW_TAUMUH.*)(.*DRAW.*)', 
                                 'HumanOutputs': 'produced when reconstructing all DRAW formats'},
    'outputDESDM_EOVERPFile'  : {'dstype': 'DESDM_EOVERP', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_MinBias\..*)',
                                 'HumanOutputs': 'produced when reconstructing RAW from MinBias'},

#    'outputDESDM_MSPerfFile'  : {'dstype': 'DESDM_MCP', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*|.*CosmicMuons.*)',
#                                 'HumanOutputs': 'produced when reconstructing RAW'},
    'outputDESDM_MCPFile'  : {'dstype': 'DESDM_MCP', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*|.*CosmicMuons.*)',
                                 'HumanOutputs': 'produced when reconstructing RAW'},

    'outputDESDM_EXOTHIPFile'  : {'dstype': 'DESDM_EXOTHIP', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*physics_Main\..*)',
                                 'HumanOutputs': 'produced when reconstructing RAW'},

#    'outputDESDM_IDALIGNFile' : {'dstype': '!replace RAW ESDM', 'ifMatch': pcProjTag+'(.*\.DRAW_EGZ\.*|.*DRAW_ZMUMU.*)(.*physics_Main.*)',
    'outputDESDM_IDALIGNFile' : {'dstype': '!replace RAW ESDM', 'ifMatch': pcProjTag+'(.*physics_Main.*)(.*\.DRAW_EGZ\.*|.*DRAW_ZMUMU.*)',
                                 'HumanOutputs': 'produced when reconstructing DRAW_EGZ and DRAW_ZMUMU formats'},

# DESDs made from full ESD files
    'outputDAOD_IDNCBFile' : {'dstype': 'DAOD_IDNCB', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*Background.*)',
                                 'HumanOutputs': 'produced for Background streams.'},
 
    'outputDAOD_IDTRKVALIDFile' : {'dstype': 'DAOD_IDTRKVALID', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*MinBias.*|.*IDCosmic.*)',
                                 'HumanOutputs': 'produced for MinBias and IDCosmic streams.'},

    'outputDAOD_IDTRKLUMIFile' : {'dstype': 'DAOD_IDTRKLUMI', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*VdM.*|.*PixelBeam*.)',
                                 'HumanOutputs': 'produced for calibration streams.'},

    'outputDAOD_IDPIXLUMIFile' : {'dstype': 'DAOD_IDPIXLUMI', 'ifMatch': pcProjTag+'(?!.*DRAW.*)(.*VdM.*|.*PixelBeam*.)',
                                 'HumanOutputs': 'produced for PixelBeam and VdM calibration streams.'},

    'outputDAOD_SCTVALIDFile': {'dstype': 'DAOD_SCTVALID', 'ifMatch': cphiProjTag+'(?!.*DRAW.*)(.*express_express.*)',
                                'HumanOutputs': 'produced for express stream only'},

    'outputDAOD_IDTIDEFile' : {'dstype': 'DAOD_IDTIDE', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                                 'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_RPVLLFile': {'dstype': 'DESDM_RPVLL','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_SLTTMUFile': {'dstype': 'DESDM_SLTTMU','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_PHOJETFile': {'dstype': 'DESDM_PHOJET','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_CALJETFile': {'dstype': 'DESDM_CALJET','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main.'},

    'outputDESDM_SGLELFile': {'dstype': 'DESDM_SGLEL','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_EGAMMAFile': {'dstype': 'DESDM_EGAMMA','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDESDM_TILEMUFile': {'dstype': 'DESDM_TILEMU','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Main.*)',
                              'HumanOutputs': 'produced for collision runs, for physics_Main stream.'},

    'outputDAOD_L1CALO2File': {'dstype': 'DAOD_L1CALO2','ifMatch': cphiProjTag+'(?!.*DRAW.*)(.*physics_Main.*|.*ZeroBias.*)',
                              'HumanOutputs': 'produced for special L1Calo runs, for physics_Main and ZeroBias streams'},

}

#Print for debugging...
## for outputKey in OutputsVsStreams.keys():
##     try:
##         print outputKey,":",OutputsVsStreams[outputKey]['ifMatch']
##     except:
##         print outputKey,": always produced"

#-------------------------------------------------------------------------------------
def GetProcessConfigs(release):
    rel = str(release)
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
                          'trfsetupcmd': setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Trf for combined DQM histogram merging and DQM webpage creation, to get periodic DQ monitoring updates. '}

    # TAG merging (no TAG_COMM anymore)
    processConfigs['tagmerge'] = {
        'inputs': {'inputTAGFile': {}},
        'outputs': {'outputTAG_MRGFile': {'dstype': 'TAG' }},
        'phconfig': {},
        'transformation': 'TAGMerge_tf.py',
        'tasktransinfo': {'trfpath': 'TAGMerge_tf.py', 
                          'trfsetupcmd': setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Trf for TAG merging, runs CollAppend from release area. ' }
    
    # NTUP merging
    processConfigs['ntupmerge'] = {
        'inputs': {'inputNTUP_MUONCALIBFile': {}},
        'outputs': {'outputNTUP_MUONCALIB_MRGFile': {'dstype': '!likeinput'}},
        'phconfig': {}, #{'skipFileValidation': 'True'},
        'transformation': 'NTUPMerge_tf.py',
        'tasktransinfo': {'trfpath': 'NTUPMerge_tf.py', 
                          'trfsetupcmd': setupScript+' '+rel+' '+specialT0Setup },

        'description': 'Trf for plain-ROOT n-tuple merging. '}

    # AOD merging
    processConfigs['aodmerge'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputAOD_MRGFile': {'dstype': 'AOD'}
                    #'outputTAGFile': {'dstype': 'TAG', 'ifMatch': '(?!.*calibration.*)'}
                    },
        'phconfig': {'autoConfiguration': 'everything','preExec': {'all': ['from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags;athenaCommonFlags.UseLZMA.set_Value_and_Lock(True)']}},
        'transformation': 'AODMerge_tf.py',
        'tasktransinfo': {'trfpath': 'AODMerge_tf.py',
                          'trfsetupcmd':  setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged AODs with AODMerge_tf.py. '}

    # DAOD merging
    processConfigs['daodmerge'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputAOD_MRGFile': {'dstype': '!likeinput'}},
        'phconfig': {'ignoreErrors': 'True', 'autoConfiguration': 'everything'},
        'transformation': 'AODMerge_tf.py',
        'tasktransinfo': {'trfpath': 'AODMerge_tf.py',
                          'trfsetupcmd':  setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged DAODs with AODMerge_tf.py. '}

    # DPD merging
    processConfigs['dpdmerge'] = {
        'inputs': {'inputESDFile': {}},
        'outputs': {'outputESD_MRGFile': {'dstype': '!likeinput' }},
        'phconfig': {'ignoreErrors': 'True', 'autoConfiguration': 'everything'},
        'transformation': 'ESDMerge_tf.py',
        'tasktransinfo': {'trfpath': 'ESDMerge_tf.py',
                          'trfsetupcmd': setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged DPDs with ESDMerge_tf. ' }


    # Fast Physics Monitoring Ntuple
    processConfigs['fastmon'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputNTUP_FASTMONFile': {'dstype': 'NTUP_FASTMON'}},
        'phconfig': {'ignoreerrors': 'ALL', 'autoConfiguration': 'everything' },
        'transformation': 'Reco_tf.py',
        'tasktransinfo': {'trfpath': 'Reco_tf.py',
                          'trfsetupcmd':  setupScript+' '+rel+' '+specialT0Setup },
        'description': 'Produces Fast Physics Monitoring ntuple with Reco_tf.py  '}


    # Reco
    processConfigs['reco'] = {
        'inputs': None,
        'outputs': None,
        'phconfig': None,
        'transformation': 'Reco_tf.py',
        'tasktransinfo': {'trfpath': 'Reco_tf.py',
                          'trfsetupcmd': setupScript+' '+rel+' '+specialT0Setup },

        'description': 'Runs reconstruction with Reco_tf. ' }
    return processConfigs
#-------------------------------------------------------------------------------------

if __name__ == '__main__':
    if len(sys.argv) < 5 or len(sys.argv) > 8:
        print "##############"
        print "Application to create or update AMI tags (for manager only)\n"
        print "Usage:"
        print "UploadAMITag.py inputDictionary.pickle create_AMITag <release number> <description> [updateConditionsTag] [coolsource] [project] [patcharea]"
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
    release_number=str(sys.argv[3])
    description=str(sys.argv[4])
    ### Defaults for optional arguements
    updateConditionsTag = True # Switch to enable/disable the CURRENT conditions tag update
    coolSource = ""
    project    = ""
    patchArea  = ""
    if len(sys.argv)>5:
        print sys.argv
        ### Update conditions
        try:
            updateConditionsTag = int(sys.argv[5])
        except:
            if sys.argv[5].lower() == "false":
                updateConditionsTag = False
        ### coolSource
        try:
            coolSource = sys.argv[6]
        except:
            print "INFO: No coolSource specified -- this defaults to fronTier"
        ### Release project
        try:
            project = sys.argv[7]
        except:
            print "INFO: No project specified -- this defaults to Athena"
        ### Patch Area
        try:
            patchArea = sys.argv[8]
        except:
            print "INFO: No patch area specified -- this defaults to the default set by Tier0"

    doWhat="Dunno"
    amiTag=''
    if amiTagTmp.startswith("create_"):
        doWhat="create"
        amiTag=amiTagTmp.replace("create_","")
    elif amiTagTmp.startswith("update_"):
        doWhat="update"
        amiTag=amiTagTmp.replace("update_","")

    ### Completing the setup script arguements
    specialT0Setup = ""
    if coolSource:
        specialT0Setup += (' ' + coolSource)
    elif not (amiTag.startswith("f") or amiTag.startswith("v") ):
        specialT0Setup += (' oracleCOOL')
    if project:
        specialT0Setup += (' ' + project)
    if patchArea:
        specialT0Setup += (' ' + patchArea)

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
    relProj = ""
    if project:
        relProj = project
    else:
        relProj = "Athena" # the default
    baseRelNbr=".".join(release_number.split(".")[:2])
    relPath = "/cvmfs/atlas.cern.ch/repo/sw/software/%s/%s/%s" % (baseRelNbr, relProj, release_number)
    if not os.path.isdir(relPath):
        s="ERROR Release directory " + relPath + " does not exists"
        #raise RuntimeError(s)
    else:
        print "INFO: Found",relPath

    processConfigs=GetProcessConfigs(release_number)
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

        didConditionsUpdate = False
        for key in startingDic.keys():
            if key.startswith("input"):
                if amiTag.startswith("q"): # only write the input file for q-tags
                    inputDic[key]=startingDic[key]
                else:
                    inputDic[key]={}
                #print "inputDic[%s] = %s" % (key, inputDic[key])

            elif key.startswith("output"):
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
    c['SWReleaseCache']= "%s_%s" % (relProj, release_number)
    c['transformation']=str(processConfigs[process]['transformation'])
    c['description']=processConfigs[process]['description']+description+" Using "+ relProj + "-" + release_number
    c['trfsetupcmd']=str(moreInfoDic['tasktransinfo']['trfsetupcmd'])
    c['moreInfo']=str(moreInfoDic.__str__())
    if amiTag.startswith("f") or amiTag.startswith("x") or amiTag.startswith("q") or amiTag.startswith("v") or amiTag.startswith("c"):
        c['Geometry']=startingDic['geometryVersion']['all']
        #c['Geometry']='Auto-config. See dataset info.'
        #if not amiTag.startswith("x"):
        c['ConditionsTag']=configDic['conditionsTag']['all']
        #c['ConditionsTag']='Auto-config. See dataset info.'

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
