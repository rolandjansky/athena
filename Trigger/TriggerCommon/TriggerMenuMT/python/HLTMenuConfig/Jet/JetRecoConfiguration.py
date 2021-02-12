#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

##########################################################################################
# Helper functions to digest the reconstruction options dictionary
# and translate it into the python configuration objects used by
# jet reco code.

from JetRecConfig.JetDefinition import JetConstitSeq,JetConstitSource, xAODType, JetDefinition
# this is to define trigger specific JetModifiers (ex: ConstitFourMom_copy) : 
from .TriggerJetMods import jetmoddict  # noqa: F401

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration")

def interpretJetCalibDefault(recoDict):
    if recoDict['recoAlg'] == 'a4':
        if recoDict['constitType'] == 'tc':
            return 'subresjesgscIS' if recoDict['trkopt'] == 'ftf' else 'subjesIS'
        elif recoDict['constitType'] == 'pf':
            return 'subresjesgscIS'
    elif recoDict['recoAlg'] == 'a10':
        return 'subjes'
    elif recoDict['recoAlg'] == 'a10t':
        return 'jes'
    elif recoDict['recoAlg'] == 'a10r':
        return 'subjesIS' # calibration for the small-R jets used to reconstruct the reclustered jets
    else:
        raise RuntimeError('No default calibration is defined for %s' % recoDict['recoAlg'])

recoKeys = ['recoAlg','constitType','clusterCalib','constitMod','jetCalib','trkopt','trkpresel']

# Extract the jet reco dict from the chainDict
def extractRecoDict(chainParts):
    # interpret the reco configuration only
    # eventually should just be a subdict in the chainDict
    recoDict = {}
    for p in chainParts:
        for k in recoKeys:
            # Look for our key in the chain part
            if k in p.keys():
                # found the key, check for consistency with other chain parts of this chain
                if k in recoDict.keys():
                    if p[k] != recoDict[k]:
                        raise RuntimeError('Inconsistent reco setting for %s' % k)
                # copy this entry to the reco dictionary
                recoDict[k] = p[k]

    # set proper jetCalib key in default case
    if recoDict['jetCalib'] == "default":
        recoDict['jetCalib'] = interpretJetCalibDefault(recoDict)

    return recoDict

# Translate the reco dict to a string for suffixing etc
def jetRecoDictToString(jetRecoDict):
    strtemp = "{recoAlg}_{constitMod}{constitType}_{clusterCalib}_{jetCalib}"
    if jetRecoDict["trkopt"] != "notrk":
        strtemp += "_{trkopt}_{trkpresel}"
    return strtemp.format(**jetRecoDict)

# Inverse of the above, essentially only for CF tests
def jetRecoDictFromString(jet_def_string):

    # Translate the definition string into an approximation
    # of the "recoParts" in the jet chainParts.
    jetRecoDict = {}
    from TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts import JetChainParts,JetChainParts_Default
    for key in recoKeys:
        keyFound = False
        for part in jet_def_string.split('_'):
            if part in JetChainParts[key]:
                jetRecoDict[key] = part
                keyFound         = True
        if not keyFound:
            jetRecoDict[key] = JetChainParts_Default[key]

    # set proper jetCalib key in default case
    if jetRecoDict['jetCalib'] == "default":
        jetRecoDict['jetCalib'] = interpretJetCalibDefault(jetRecoDict)

    return jetRecoDict

# Define the jet constituents to be interpreted by JetRecConfig
# When actually specifying the reco, clustersKey should be
# set, but default to None to allow certain checks, in particular
# grooming configuration
def defineJetConstit(jetRecoDict,clustersKey=None,pfoPrefix=None):
    constitMods = []
    # Get the details of the constituent definition:
    # type, mods and the input container name
    
    if jetRecoDict["constitType"] == "pf":
        if pfoPrefix is None:
            raise RuntimeError("JetRecoConfiguration: Cannot define PF jets without pfo prefix!")

        trkopt = jetRecoDict['trkopt']
        from JetRecConfig.ConstModHelpers import constitModWithAlternateTrk
        # Generate a new JetConstitModifier with track proterties setup according to trkopt
        constitModWithAlternateTrk("CorrectPFO", trkopt) 
        constitMods = ["CorrectPFO"+trkopt]
        # apply constituent pileup suppression
        if "vs" in jetRecoDict["constitMod"]:
            constitMods.append("Vor")
        if "cs" in jetRecoDict["constitMod"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["constitMod"]:
            constitMods.append("SK")
        # Generate a new JetConstitModifier with track proterties setup according to trkopt
        constitModWithAlternateTrk("CHS", trkopt) # 
        constitMods += ["CHS"+trkopt]
        
        inputPFO = pfoPrefix+"ParticleFlowObjects"
        modstring = ''.join(constitMods[1:-1])
        if modstring == '':
            modstring='CHS'
        
        if not constitMods:
            jetConstit = JetConstitSeq( "HLT_EMPFlow", xAODType.ParticleFlow, constitMods, inputname=inputPFO, outputname=pfoPrefix+"CHSParticleFlowObjects", label="EMPFlow")
        else:
            jetConstit = JetConstitSeq( "HLT_EMPFlow"+modstring, xAODType.ParticleFlow, constitMods, inputname=inputPFO, outputname=pfoPrefix+modstring+"ParticleFlowObjects",label='EMPFlow'+(modstring if modstring!='CHS' else '') )

            
    if jetRecoDict["constitType"] == "tc":
        # apply constituent pileup suppression
        if "vs" in jetRecoDict["constitMod"]:
            constitMods.append("Vor")
        if "cs" in jetRecoDict["constitMod"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["constitMod"]:
            constitMods.append("SK")
        # build a modifier identifier :
        modstring = ''.join(constitMods)
        # prepend the cluster calib state :
        if jetRecoDict["clusterCalib"] == "em":
            constitMods = ["EM"] + constitMods
        elif jetRecoDict["clusterCalib"] == "lcw":
            constitMods = ["LC"] + constitMods

        jetConstit = JetConstitSeq( "HLT_"+constitMods[0]+"Topo",xAODType.CaloCluster, constitMods, inputname=clustersKey, outputname=clustersKey+modstring,label=constitMods[0]+'Topo'+modstring)

    # declare our new JetConstitSeq in the standard dictionary
    from JetRecConfig.StandardJetConstits import jetconstitdic
    jetconstitdic.setdefault(jetConstit.name, jetConstit)

    return jetConstit

    
def interpretRecoAlg(recoAlg):
    import re
    jetalg, jetradius, jetextra = re.split(r'(\d+)',recoAlg)    
    return jetalg, int(jetradius), jetextra

# Arbitrary min pt for fastjet, set to be low enough for MHT(?)
# Could/should adjust higher for large-R
def defineJets(jetRecoDict,clustersKey=None,prefix='',pfoPrefix=None):
    minpt = {
        4:  7000,
        10: 50000,
    }
    jetalg, jetradius, jetextra = interpretRecoAlg(jetRecoDict["recoAlg"])
    actualradius = float(jetradius)/10
    jetConstit = defineJetConstit(jetRecoDict,clustersKey,pfoPrefix)

    suffix="_"+jetRecoDict["jetCalib"]
    if jetRecoDict["trkopt"] != "notrk":
        suffix += "_{}".format(jetRecoDict["trkopt"])
    

    jetDef = JetDefinition( "AntiKt", actualradius, jetConstit, ptmin=minpt[jetradius], prefix=prefix, suffix=suffix)
    return jetDef

def defineReclusteredJets(jetRecoDict,smallRjets,inputlabel,prefix,suffix):
    rcJetConstit = JetConstitSource("RCJet", xAODType.Jet, smallRjets, label=inputlabel+'RC')
    rcJetDef = JetDefinition( "AntiKt", 1.0, rcJetConstit, prefix=prefix, suffix=suffix)
    return rcJetDef

def defineGroomedJets(jetRecoDict,ungroomedDef):#,ungroomedJetsName):
    from JetRecConfig.JetGrooming import JetTrimming, JetSoftDrop
    groomAlg = jetRecoDict["recoAlg"][3:] if 'sd' in jetRecoDict["recoAlg"] else jetRecoDict["recoAlg"][-1]
    suffix = "_"+ jetRecoDict["jetCalib"]
    if jetRecoDict["trkopt"]!="notrk":
        suffix += "_"+jetRecoDict["trkopt"]
    
    groomDef = {
        "sd":JetSoftDrop(ungroomedDef,ZCut=0.1,Beta=1.0, suffix=suffix),
        "t" :JetTrimming(ungroomedDef,RClus=0.2,PtFrac=0.04, suffix=suffix),
    }[groomAlg]
    return groomDef

##########################################################################################
# Generation of modifier lists. So far only calib, but can add track, substructure mods


# Make generating the list a bit more comprehensible
def getModSpec(modname,modspec=''):
    return (jetmoddict[modname],str(modspec))

def defineTrackMods(trkopt):
    trkmods = [
        "TrackMoments:"+trkopt,
        "JVF:"+trkopt,
        "JVT:"+trkopt,
    ]
    return trkmods

def getFilterCut(recoAlg):
    return {"a4":5000, "a10":50000, "a10r": 50000, "a10t":50000, "a10sd":50000}[recoAlg]

# Translate calib specification into something understood by
# the calibration config helper
def defineCalibMods(jetRecoDict,dataSource,rhoKey="auto"):

    # Minimum modifier set for calibration w/o track GSC
    # Should eventually build in more mods, depend on track info etc
    jetalg = jetRecoDict["recoAlg"]
    if jetRecoDict["jetCalib"] == "nojcalib" or jetalg=="a10r":
        calibMods = []
    else:
        if jetRecoDict["trkopt"]=="notrk" and "gsc" in jetRecoDict["jetCalib"]:
            raise ValueError("Track GSC requested but no track source provided!")

        if jetRecoDict["trkopt"]=="notrk" and "subres" in jetRecoDict["jetCalib"]:
            raise ValueError("Pileup residual calibration requested but no track source provided!")

        if jetRecoDict["constitType"] == "tc":
            calibContext,calibSeq = {
                ("a4","subjes"):   ("TrigRun2","JetArea_EtaJES_GSC"), # Calo GSC only
                ("a4","subjesIS"): ("TrigRun2","JetArea_EtaJES_GSC"), # Calo GSC only
                ("a4","subjesgscIS"): ("TrigRun2GSC","JetArea_EtaJES_GSC"), # Calo+Trk GSC
                ("a4","subresjesgscIS"): ("TrigRun2GSC","JetArea_Residual_EtaJES_GSC"), # pu residual + calo+trk GSC
                ("a10","subjes"):  ("TrigUngroomed","JetArea_EtaJES"),
                ("a10t","jes"):    ("TrigTrimmed","EtaJES_JMS"),
                }[(jetRecoDict["recoAlg"],jetRecoDict["jetCalib"])]

            pvname = ""
            gscDepth = "EM3"
            if "gsc" in jetRecoDict["jetCalib"]:
                gscDepth = "trackWIDTH"
                pvname = "HLT_IDVertex_FS"

        elif jetRecoDict["constitType"] == "pf":
            gscDepth = "auto"
            if 'sd' in jetRecoDict["recoAlg"]:
                calibContext = "TrigSoftDrop"
                calibSeq = "EtaJES_JMS"
            else:
                calibContext,calibSeq = {
                  ("a4","subjesgscIS"): ("TrigLS2","JetArea_EtaJES_GSC"),             # w/o pu residual  + calo+trk GSC
                  ("a4","subresjesgscIS"): ("TrigLS2","JetArea_Residual_EtaJES_GSC"), # pu residual + calo+trk GSC
                  }[(jetRecoDict["recoAlg"],jetRecoDict["jetCalib"])]
            pvname = "HLT_IDVertex_FS"

        if jetRecoDict["jetCalib"].endswith("IS") and (dataSource=="data"):
            calibSeq += "_Insitu"

        calibSpec = ":".join( [calibContext, dataSource, calibSeq, rhoKey, pvname, gscDepth] )

        if jetalg=="a4":
            calibMods = ["EMScaleMom",
                         "ConstitFourMom_copy",
                         "CaloEnergies", # Needed for GSC
                         "Calib:"+calibSpec]
        else:
            calibMods = ["ConstitFourMom_copy",
                         "Calib:"+calibSpec]

    return calibMods

def getDecorList(doTracks,isPFlow):
    # Basic jet info provided by the jet builder
    decorlist = [ 'AlgorithmType', 'InputType',
                  'ActiveArea', 'ActiveArea4vec_eta', 'ActiveArea4vec_m',
                  'ActiveArea4vec_phi', 'ActiveArea4vec_pt',
                  'EMFrac','HECFrac','JvtRpt','EnergyPerSampling']

    if doTracks:
        decorlist += ["GhostTrack",
                      "NumTrkPt500","NumTrkPt1000",
                      "SumPtTrkPt500","SumPtTrkPt1000",
                      "TrackWidthPt1000",
                      "JVFCorr"]
        if isPFlow:
            decorlist += ["SumPtChargedPFOPt500"]
    return decorlist
