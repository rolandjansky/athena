#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys,pickle,os

HIProjTag="data[0-9][0-9]_hi" # Only for Pb-Pb and p-Pb collisions (this regular expression matches with both "data11_hi" and "data11_hip")
HIpProjTag="data[0-9][0-9]_hip" # Only for p-Pb collisions
ppProjTag="data[0-9][0-9]_.*eV" # Only for pp data
cmProjTag="data[0-9][0-9]_(cos|1beam)" # only for commissioning
cpProjTag="data[0-9][0-9]_(cos|1beam|.*eV|comm)" #For commissioning or pp but not HI
cphipProjTag="data[0-9][0-9]_(cos|1beam|.*eV|comm|hip)" # Above plus hip, i.e. all bar Pb-Pb HI
pphipProjTag="data[0-9][0-9]_(.*eV|hip)" # pp or hip, for hip outputs in pp

#Setup script locations
setupScript="/afs/cern.ch/atlas/tzero/software/setup/setuptrf.sh"
specialT0Setup_Oracle="/afs/cern.ch/atlas/tzero/software/setup/specialsetup_tier0.sh" # Direct-Oracle - for everything but f-tags
specialT0Setup_Frontier="/afs/cern.ch/atlas/tzero/software/setup/specialsetup_tier0_frontier.sh" # FronTier - for f-tags. Will also use for v tags with an alert

specialT0Setup=specialT0Setup_Oracle # By Default

#For Commissioning, Colissions and Heavy Ions
OutputsVsStreams={

# The basics:
    'outputESDFile': {'dstype': 'ESD', 'ifMatch': '(?!.*DRAW.*)', 'HumanOutputs': 'always produced.'},
    'outputAODFile': {'dstype': 'AOD', 'ifMatch': cpProjTag+'(?!.(.*DRAW.*|.*ZeroBias.*x[0-9].*))', 'HumanOutputs': 'always produced except for ZeroBias (ZB-1) stream and HI.'},
    'outputTAGFile': {'dstype': 'TAG', 'ifMatch': HIProjTag+'(?!.(.*DRAW.*|.*ZeroBias.*x[0-9].*))', 'HumanOutputs': 'Produced for HI/HIp and comm when AOD is disabled'},
    'outputHISTFile': {'dstype': 'HIST', 'ifMatch': '(?!.(.*DRAW.*|.*debugrec.*))', 'HumanOutputs': 'always produced except for debug stream'},  # note was disabled for Pb-Pb HardProbes
    
# Ntuples
    'outputNTUP_TRIGFile': {'dstype': 'NTUP_TRIG', 'ifMatch': '(?!.*DRAW.*)(.*express.*|.*physics_MinBias.*|.*physics_HardProbes.*|.*physics_UPC.*)',
                            'HumanOutputs': 'always produced for express. Produced for MinBias, HardProbes and UPC in early hip running'},
    'outputNTUP_MUONCALIBFile': {'dstype': 'NTUP_MUONCALIB', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*EnhancedBias.*|.*MuonswBeam.*|.*physics_Muons\..*|.*physics_L1TT.*|.*physics_L1Muon\..*|.*physics_HLTPassthrough\..*|.*physics_IDCosmic\..*)',
                                 'HumanOutputs': 'produced for EnhancedBias, Muon, L1TT and L1Muon, IDCosmic and HLTPassthrough streams, not for HI'},
    'outputNTUP_TRIGMUFile': {'dstype': 'NTUP_TRIGMU', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_L1Muon\..*|.*physics_IDCosmic\..*|.*physics_HLTPassthrough\..*)',
                                 'HumanOutputs': 'produced for L1Muon, IDCosmic and HLTPassthrough streams, not for HI'},
    'outputNTUP_SCTFile': {'dstype': 'NTUP_SCT', 'ifMatch': cphipProjTag+'(?!.*DRAW.*)(.*express.*|.*Background.*)',
                           'HumanOutputs': 'produced for express and Background stream'},
    'outputNTUP_BKGDFile': {'dstype': 'NTUP_BKGD', 'ifMatch': '(?!.*DRAW.*)(.*Background.*)',
                            'HumanOutputs': 'produced for Background stream'},

    'outputNTUP_EGAMMAFile': {'dstype': 'NTUP_EGAMMA', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*|.*physics_Muons\..*|.*physics_JetTauEtmiss.*)',
                                 'HumanOutputs': 'produced for Egamma, Muons and physics_JetTauEtmiss streams, not for HI'},
    'outputNTUP_PHYSICSFile': {'dstype': 'NTUP_PHYSICS', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*|.*physics_Muons\..*|.*physics_JetTauEtmiss.*)',
                                 'HumanOutputs': 'produced for Egamma, Muons and physics_JetTauEtmiss streams, not for HI'},
    'outputNTUP_SUSYFile': {'dstype': 'NTUP_SUSY', 'ifMatch': cpProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*|.*physics_Muons\..*|.*physics_JetTauEtmiss.*)',
                                 'HumanOutputs': 'produced for Egamma, Muons and physics_JetTauEtmiss streams, not for HI'},

# Commissioning DESDs:    
    'outputDESD_MUONCOMMFile' : {'dstype': 'DESD_MUONCOMM', 'ifMatch': cmProjTag+'(?!.*DRAW.*)(.*CosmicMuons.*|.*MuonswBeam.*)',
                                 'HumanOutputs': 'produced for cosmics runs, for CosmicMuons and MuonswBeam streams.'},
    'outputDESD_TILECOMMFile' : {'dstype': 'DESD_TILECOMM', 'ifMatch': cmProjTag+'(?!.*DRAW.*)(.*CosmicMuons.*)',
                                 'HumanOutputs': 'produced for cosmics runs, for CosmicMuons streams.'},
    'outputDESD_CALOCOMMFile' : {'dstype': 'DESD_CALOCOMM', 'ifMatch': cmProjTag+'(?!.*DRAW.*)(.*L1Calo\..*|.*L1CaloEM.*)',
                                 'HumanOutputs': 'produced for cosmics runs, for L1Calo and L1CaloEM'},
    'outputDESD_IDCOMMFile'   : {'dstype': 'DESD_IDCOMM', 'ifMatch': cmProjTag+'(?!.*DRAW.*)(.*IDCosmic.*|.*CosmicMuons.*)',
                                 'HumanOutputs': 'produced for cosmics runs, for IDCosmic and CosmicMuons streams.'},
    'outputDESD_PIXELCOMMFile': {'dstype': 'DESD_PIXELCOMM', 'ifMatch': cmProjTag+'(?!.*DRAW.*)(.*IDCosmic.*)',
                                 'HumanOutputs': 'produced for cosmics runs, for IDCosmic streams.'},

#Collission DESDs:
    
    #DRAW reconstruction outputs
    'outputDESD_ZMUMUFile'   : {'dstype': 'DESD_ZMUMU', 'ifMatch': ppProjTag+'(.*DRAW_ZMUMU.*)','HumanOutputs': 'produced when reconstructing DRAW_ZMUMU'},
    'outputDAOD_ZMUMUFile'   : {'dstype': 'DAOD_ZMUMU', 'ifMatch': ppProjTag+'(.*DRAW_ZMUMU.*)','HumanOutputs': 'produced when reconstructing DRAW_ZMUMU'},
    'outputDESD_ZEEFile'     : {'dstype': 'DESD_ZEE',   'ifMatch': ppProjTag+'(.*DRAW_ZEE.*)','HumanOutputs': 'produced when reconstructing DRAW_ZEE'},
    'outputDAOD_ZEEFile'     : {'dstype': 'DAOD_ZEE',   'ifMatch': ppProjTag+'(.*DRAW_ZEE.*)','HumanOutputs': 'produced when reconstructing DRAW_ZEE'},
    'outputDESD_WMUNUFile'   : {'dstype': 'DESD_WMUNU', 'ifMatch': ppProjTag+'(.*DRAW_WMUNU.*)','HumanOutputs': 'produced when reconstructing DRAW_WMUNU'},
    'outputDAOD_WMUNUFile'   : {'dstype': 'DAOD_WMUNU', 'ifMatch': ppProjTag+'(.*DRAW_WMUNU.*)','HumanOutputs': 'produced when reconstructing DRAW_WMUNU'},
    'outputDESD_WENUFile'   : {'dstype': 'DESD_WENU',   'ifMatch': ppProjTag+'(.*DRAW_WENU.*)','HumanOutputs': 'produced when reconstructing DRAW_WENU'},
    'outputDAOD_WENUFile'   : {'dstype': 'DAOD_WENU',   'ifMatch': ppProjTag+'(.*DRAW_WENU.*)','HumanOutputs': 'produced when reconstructing DRAW_WENU'},
    #DESD production
    'outputDESDM_RPVLLFile': {'dstype': 'DESDM_RPVLL','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_JetTauEtmiss.*|.*physics_Egamma.*|.*physics_Muons\..*)',
                              'HumanOutputs': 'produced for collision runs, for physics_JetTauEtMiss, physics_Egamma and physics_Muons streams.'},
    'outputDESDM_EGAMMAFile': {'dstype': 'DESDM_EGAMMA','ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*)', 'HumanOutputs': 'produced for collision runs, for physics_Egamma streams.'},
    'outputDESD_CALJETFile': {'dstype': 'DESD_CALJET',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_JetTauEtmiss.*)', 'HumanOutputs': 'produced for collision runs, for the physics_JetTauEtMiss stream.'},
    'outputDESDM_TRACKFile' : {'dstype': 'DESDM_TRACK', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_JetTauEtmiss.*)', 'HumanOutputs': 'produced for collision runs, for the physics_JetTauEtMiss stream.'},
    'outputDESD_SGLELFile'  : {'dstype': 'DESD_SGLEL',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*)', 'HumanOutputs': 'produced for collision runs, for the physics_Egamma stream.'},
    'outputDESD_SGLMUFile'  : {'dstype': 'DESD_SGLMU',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Muons\..*)', 'HumanOutputs': 'produced for collision runs, for the physics_Muons streams.'},
    #'outputDESDM_METFile'    : {'dstype': 'DESDM_MET',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_JetTauEtmiss.*)', 'HumanOutputs': 'produced for collision runs, for physics_JetETMiss stream'},
    'outputDESD_PHOJETFile' : {'dstype': 'DESD_PHOJET', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Egamma.*)', 'HumanOutputs': 'produced for collision runs, for the physics_Egamma stream.'},
    'outputDESD_MBIASFile'  : {'dstype': 'DESD_MBIAS',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_MinBias.*)', 'HumanOutputs': 'produced for collision runs, for the physics_MinBias stream.'},
    #RAW skims 
   'outputDRAW_ZMUMUFile'  : {'dstype': 'DRAW_ZMUMU', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Muons\..*)','HumanOutputs': 'produced for collision runs, for the physics_Muons stream.'},
   'outputDRAW_ZEEFile'    : {'dstype': 'DRAW_ZEE',   'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Egamma\..*)','HumanOutputs': 'produced for collision runs, for the physics_Egamma stream.'}, 
   'outputDRAW_WMUNUFile'  : {'dstype': 'DRAW_WMUNU', 'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Muons\..*)','HumanOutputs': 'produced for collision runs, for the physics_Muons stream.'},
   'outputDRAW_WENUFile'   : {'dstype': 'DRAW_WENU',  'ifMatch': ppProjTag+'(?!.*DRAW.*)(.*physics_Egamma\..*)','HumanOutputs': 'produced for collision runs, for the physics_Egamma stream.'},
   

    #Special heavy ion outputs:
    'outputDESD_HIRAREFile' : {'dstype' : 'DESD_HIRARE', 'ifMatch' : HIProjTag+'(.*physics_HardProbes.*)','HumanOutputs': 'produced in HI runs, for the physics_HardProbes stream'},
    'outputNTUP_HIFile' : {'dstype': 'NTUP_HI','ifMatch' : pphipProjTag+'(.*express.*|.*physics_bulk.*|.*physics_MinBias.*|.*physics_Standby.*|.*physics_HardProbes.*|.*physics_UPC.*)','HumanOutputs':'produced for express, bulk, MinBias, MinBiasOverlay, Standby, HardProbes and UPC streams'},
    'outputNTUP_MINBIASFile': {'dstype': 'NTUP_MINBIAS', 'ifMatch': pphipProjTag+'(?!.*DRAW.*)(.*express.*|.*physics_bulk.*|.*physics_MinBias.*|.*physics_Standby.*|.*physics_HardProbes.*|.*physics_UPC.*)','HumanOutputs': 'produced for express, bulk, MinBias, MinBiasOverlay, Standby, HardProbes and UPC streams'},
    'outputNTUP_TRKVALIDFile': {'dstype': 'NTUP_TRKVALID', 'ifMatch': pphipProjTag+'(?!.*DRAW.*)(.*express.*)','HumanOutputs': 'produced for express only for HI/HIp.'},    

    #Special for pixelStream outputs :
    'outputNTUP_IDVTXLUMIFile' : {'dstype' : 'NTUP_IDVTXLUMI', 'ifMatch' : ppProjTag+'(?!.*DRAW.*)(.*PixelBeam.*|.*calibration_VdM.*)','HumanOutputs': 'produced for luminosity studies, for the PixelBeam stream.'},


    }

#Print for debugging...
## for outputKey in OutputsVsStreams.keys():
##     try:
##         print outputKey,":",OutputsVsStreams[outputKey]['ifMatch']
##     except:
##         print outputKey,": always produced"



#-------------------------------------------------------------------------------------
def GetProcessConfigs(release,patcharea):
    rel=str(release)
    pa=str(patcharea)
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
        'inputs': {'inputTagFiles': {}},
        'outputs': {'outputTagFile': {'dstype': 'TAG' }},
        'phconfig': {},
        'transformation': 'CollAppend',
        'tasktransinfo': {'trfpath': '/afs/cern.ch/atlas/tzero/software/trfs/TAGMerge_trf.py', 
                          'trfsetupcmd': setupScript + ' '+pa+' '+rel+' none'},
        'description': 'Trf for TAG merging, runs CollAppend from release area. ' }
    
    # NTUP merging
    processConfigs['ntupmerge'] = {
        'inputs': {'inputROOTFiles': {}},
        'outputs': {'outputROOTFile': {'dstype': '!likeinput'}},
        'phconfig': {},
        'transformation': 'hadd',
        'tasktransinfo': {'trfpath': '/afs/cern.ch/atlas/tzero/software/trfs/ROOTMerge_trf.py', 
                          'trfsetupcmd': setupScript + ' '+pa+' '+rel+' none'},

        'description': 'Trf for plain-ROOT n-tuple merging. '}

    # AOD merging
    processConfigs['aodmerge'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputAODFile': {'dstype': 'AOD'},
                    'outputTAGFile': {'dstype': 'TAG'}
                    },
        'phconfig': {'--ignoreerrors': 'ALL', 'autoConfiguration': 'everything', 'preExec': 'rec.doDPD.set_Value_and_Lock(False)'},
        'transformation': 'Merging_trf.py',
        'tasktransinfo': {'trfpath': 'Merging_trf.py',
                          'trfsetupcmd':  setupScript+' '+pa+' '+rel+' '+specialT0Setup },
        'description': 'Produces merged AODs plus associated physics TAGs with Merging_trf.py. '}

    # DPD merging
    processConfigs['dpdmerge'] = {
        'inputs': {'inputESDFile': {}},
        'outputs': {'outputESDFile': {'dstype': '!likeinput' }},
        'phconfig': {'--ignoreerrors': 'ALL', 'autoConfiguration': 'everything', 'preExec': 'rec.doDPD.set_Value_and_Lock(False)'},
        'transformation': 'Merging_trf.py',
        'tasktransinfo': {'trfpath': 'Merging_trf.py',
                          'trfsetupcmd': setupScript+' '+pa+' '+rel+' '+specialT0Setup }, 
        'description': 'Produces merged DPDs with Merging_trf. ' }


    # Fast Physics Monitoring Ntuple
    processConfigs['fastmon'] = {
        'inputs': {'inputAODFile': {}},
        'outputs': {'outputNTUP_FASTMONFile': {'dstype': 'NTUP_FASTMON'}},
        'phconfig': {'--ignoreerrors': 'ALL', 'autoConfiguration': 'everything' },
        'transformation': 'Reco_trf.py',
        'tasktransinfo': {'trfpath': 'Reco_trf.py',
                          'trfsetupcmd':  setupScript+' '+pa+' '+rel+' '+specialT0Setup },
        'description': 'Produces Fast Physics Monitoring ntuple with Reco_trf.py  '}


    # Reco
    processConfigs['reco'] = {
        'inputs': None,
        'outputs': None,
        'phconfig': None,
        'transformation': 'Reco_trf.py',
        'tasktransinfo': {'trfpath': 'Reco_trf.py',
                          'trfsetupcmd': setupScript+' '+pa+' '+rel+' '+specialT0Setup }, 

        'description': 'Runs reconstruction with Reco_trf. ' }
    return processConfigs
#-------------------------------------------------------------------------------------

if __name__ == '__main__':
    if len(sys.argv)<7 or len(sys.argv)>9:
        print "##############"
        print "Application to create or update AMI tags (for manager only)\n"
        print "Usage:"
        print "UploadAMITag.py <login> <password> inputDictionary.pickle create_AMITag <release> <description> [patcharea] [updateConditionsTag]"
        print "or"
        print "UploadAMITag.py <login> <password> inputDictionary.pickle update_AMITag <release> <description> [patcharea] [updateConditionsTag]"
        print "\nTo create inputDictionary.pickle, execute:"
        print "Reco_trf.py"
        print "or"
        print "CmdToPickledDic.py <command>"
        print "##############"
        sys.exit(0)


    login=str(sys.argv[1])
    password=str(sys.argv[2])
    inPickleFile=str(sys.argv[3])
    amiTagTmp=str(sys.argv[4])
    release=str(sys.argv[5])
    description=str(sys.argv[6])
    updateConditionsTag = True # Switch to enable/disable the CURRENT conditions tag update
    if len(sys.argv)>7:
        patcharea=str(sys.argv[7])
        if not os.access(patcharea,os.R_OK):
            print "WARNING Can't access patch area at",patcharea
        if not patcharea.startswith("/afs/cern.ch/"):
            print "WARNING Patch area does not start with /afs/cern.ch/ ???"
        if len(sys.argv)>8:    
            try:
                updateConditionsTag = int(sys.argv[8])
            except:
                if sys.argv[8].lower() == "false":
                    updateConditionsTag = False

    else:
        patcharea="/afs/cern.ch/atlas/tzero/software/patches/"+release

    doWhat="Dunno"
    amiTag=''
    if amiTagTmp.startswith("create_"):
        doWhat="create"
        #amiTag=amiTagTmp.strip("create_")
        amiTag=amiTagTmp.replace("create_","")
    elif amiTagTmp.startswith("update_"):
        doWhat="update"
        #amiTag=amiTagTmp.strip("update_")
        amiTag=amiTagTmp.replace("update_","")

    if amiTag.startswith("f") or amiTag.startswith("v"):
        specialT0Setup=specialT0Setup_Frontier
        if amiTag.startswith("v"):
            print "INFO: This v-tag is being created with specialT0Setup =",specialT0Setup

    process=None
    if inPickleFile.endswith('.pickle'): process='reco'
    elif inPickleFile=='histmerge': process='histmerge'
    elif inPickleFile=='tagmerge': process='tagmerge'
    elif inPickleFile=='ntupmerge': process='ntupmerge'
    elif inPickleFile=='aodmerge': process='aodmerge'
    elif inPickleFile=='dpdmerge': process='dpdmerge'
    elif inPickleFile=='fastmon' : process='fastmon'
    else:
        s="Don't know how to interpret argument: '%s'.\n"%inPickleFile
        s+="Possible arguments are: histmerge, tagmerge, ntupmerge, aodmerge, dpdmerge\n"
        s+="...or for reconstruction tags: a pickeled dic named *.pickle\n"
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

    # Forcing histmerge to be in 64 bits
    if process=='histmerge':
        release=release+",64"
        print "INFO: Enforcing histmerge tag to be in 64 bits"
        

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
        from PATJobTransforms.Configuration import ConfigDic
        didConditionsUpdate=False
        for key in startingDic.keys():
            if ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isInput"):
                inputDic[key]={}
            elif ConfigDic.has_key(key) and hasattr(ConfigDic[key],"isOutput") and (key!="tmpESD" and key!="tmpAOD"):
                try:
                    #print "key: " , key , " isOutput"
                    outputDic[key]=OutputsVsStreams[key]
                except:
                    print "Known outputs defined in the OutputsVsStreams dictionnary are:"
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
        configDic['maxEvents']=-1
        configDic['AMITag']=str(amiTag)

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

    #---------------------
    #Get pyAMI client
#    try:
#        from pyAMI.pyAMI import AMI
#    except ImportError:
#        print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
#        print "Will manually add ZSI and 4suite, then try again..."
#        import sys
#        sys.path.insert(0,'/afs/cern.ch/atlas/offline/external/ZSI/2.1-a1/lib/python')
#        sys.path.insert(0,'/afs/cern.ch/sw/lcg/external/4suite/1.0.2_python2.5/slc4_ia32_gcc34/lib/python2.5/site-packages')
#        from pyAMI.pyAMI import AMI
#    print "import pyAMI was succesful"
#    amiclient=AMI(False)

#    import setup_pyAMI
#    from pyAMI.pyAMI import AMI
    try:
        from pyAMI.client import AMIClient
    except ImportError:
        print "WARNING unable to import AMIClient from pyAMI"
    print "import pyAMI4 was successful"    
    amiclient=AMIClient(False)

    #------------------------
    #Build final AMI tag info
    s={} #stable values
    s['configTag']=amiTag
    s['AMIUser']=login
    s['AMIPass']=password

    c={} #changing values
    c['phconfig']=str(processConfigs[process]['phconfig'].__str__())
    c['inputs']=str(processConfigs[process]['inputs'].__str__())
    c['outputs']=str(processConfigs[process]['outputs'].__str__())
    if humanReadableOutputs: c['Human-readable outputs']=str(humanReadableOutputs)
    c['SWReleaseCache']=str(release)
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
        l=['AddConfigurationTag']
        for k in s.keys():
            l.append(k+'='+s[k])
        for k in c.keys():
            l.append(k+'='+c[k])

        #print "command is: "
        #print "l=",l
        result=amiclient.execute(l)
        print "\n\n###############################################"
        print "#  Succesfully created new tag %s !!   :-)  #"%amiTag
        print "###############################################\n\n"        


    elif doWhat=="update":
        l=['UpdateConfigurationTag']
        for k in s.keys():
            l.append(k+'='+s[k])

        s='xx'
        l.append('separator='+s)
        
        nKeys=len(c.keys())
        n=0
        tmp='updateField='
        for k in c.keys():
            tmp+=k
            n+=1
            if n<nKeys: tmp+=s
        l.append(tmp)

        n=0
        tmp='updateValue='
        for k in c.keys():
            tmp+=c[k]
            n+=1
            if n<nKeys: tmp+=s
        l.append(tmp)
        
        #print "l=",l
        result=amiclient.execute(l)
        print "\n\n#####################################################"
        print "#  Succesfully updated existing tag %s !!!   :-)  #"%amiTag
        print "#####################################################\n\n"        

    else:
        raise SyntaxError("Don't know what to do... amiTagTmp='%s'  doWhat='%s'"%(amiTagTmp,doWhat))
            
    print "To see the corresponding command, do:"
    print "GetCommand.py AMI=%s \n"%amiTag
    print "or go to:"
    print "http://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?linkId=501"
    print "\n"
