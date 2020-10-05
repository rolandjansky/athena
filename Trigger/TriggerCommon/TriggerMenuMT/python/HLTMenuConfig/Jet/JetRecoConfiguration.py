#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

##########################################################################################
# Helper functions to digest the reconstruction options dictionary
# and translate it into the python configuration objects used by
# jet reco code.

from JetRecConfig.JetDefinition import JetConstit, xAODType, JetDefinition
from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration")

# Extract the jet reco dict from the chainDict
def extractRecoDict(chainParts):
    # interpret the reco configuration only
    # eventually should just be a subdict in the chainDict
    recoKeys = ['recoAlg','dataType','calib','jetCalib','trkopt','cleaning']
    recoDict = {}
    for p in chainParts:
        for k in recoKeys:
            # Look for our key in the chain part
            if k in p.keys():
                # found the key, check for consistency with other chain parts of this chain
                if k in recoDict.keys():
                    if p[k] != recoDict[k]:
                        log.error('Inconsistent reco setting for' + k)
                        exit(1)
                # copy this entry to the reco dictionary
                recoDict[k] = p[k]

    return recoDict

# Define the jet constituents to be interpreted by JetRecConfig
# When actually specifying the reco, clustersKey should be
# set, but default to None to allow certain checks, in particular
# grooming configuration
def defineJetConstit(jetRecoDict,clustersKey=None,pfoPrefix=None):
    constitMods = []
    # Get the details of the constituent definition:
    # type, mods and the input container name
    if "pf" in jetRecoDict["dataType"]:
        if pfoPrefix is None:
            raise RuntimeError("JetRecoConfiguration: Cannot define PF jets without pfo prefix!")
        # apply constituent pileup suppression
        if "cs" in jetRecoDict["dataType"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["dataType"]:
            constitMods.append("SK")
        if not constitMods:
            jetConstit = JetConstit( xAODType.ParticleFlow, constitMods, rawname=pfoPrefix+"ParticleFlowObjects", inputname=pfoPrefix+"CHSParticleFlowObjects")
        else:
            jetConstit = JetConstit( xAODType.ParticleFlow, constitMods, rawname=pfoPrefix+"ParticleFlowObjects", prefix=pfoPrefix)
        
    if "tc" in jetRecoDict["dataType"]:
        # apply this scale
        if jetRecoDict["calib"] == "em":
            constitMods = ["EM"] + constitMods
        elif jetRecoDict["calib"] == "lcw":
            constitMods = ["LC"] + constitMods
        # read from this cluster collection,
        # overriding the standard offline collection
        jetConstit = JetConstit( xAODType.CaloCluster, constitMods, rawname=clustersKey, prefix="HLT_")
        # apply constituent pileup suppression
        if "cs" in jetRecoDict["dataType"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["dataType"]:
            constitMods.append("SK")
        jetConstit.modifiers = constitMods
        if clustersKey is not None and jetRecoDict["dataType"]=="tc":
            jetConstit.inputname = clustersKey
    return jetConstit

    
def interpretRecoAlg(recoAlg):
    import re
    jetalg, jetradius, jetextra = re.split(r'(\d+)',recoAlg)    
    return jetalg, int(jetradius), jetextra

# Arbitrary min pt for fastjet, set to be low enough for MHT(?)
# Could/should adjust higher for large-R
def defineJets(jetRecoDict,clustersKey=None,pfoPrefix=None):
    minpt = {
        4:  7000,
        10: 50000,
    }
    jetalg, jetradius, jetextra = interpretRecoAlg(jetRecoDict["recoAlg"])
    actualradius = float(jetradius)/10
    jetConstit = defineJetConstit(jetRecoDict,clustersKey,pfoPrefix)
    jetDef = JetDefinition( "AntiKt", actualradius, jetConstit, ptmin=minpt[jetradius])
    return jetDef

def defineReclusteredJets(jetRecoDict):
    rcJetConstit = JetConstit( xAODType.Jet, [])
    rcJetDef = JetDefinition( "AntiKt", 1.0, rcJetConstit)
    return rcJetDef

def defineGroomedJets(jetRecoDict,ungroomedDef,ungroomedJetsName):
    from JetRecConfig.JetGrooming import JetTrimming, JetSoftDrop
    groomAlg = jetRecoDict["recoAlg"][3:] if 'sd' in jetRecoDict["recoAlg"] else jetRecoDict["recoAlg"][-1]
    groomDef = {
        "sd":JetSoftDrop(ungroomedDef,ungroomedJetsName,zcut=0.1,beta=1.0),
        "t" :JetTrimming(ungroomedDef,ungroomedJetsName,smallR=0.2,ptfrac=0.05),
    }[groomAlg]
    return groomDef

##########################################################################################
# Generation of modifier lists. So far only calib, but can add track, substructure mods

from JetRecConfig.StandardJetMods import jetmoddict

# Make generating the list a bit more comprehensible
def getModSpec(modname,modspec=''):
    return (jetmoddict[modname],str(modspec))

def defineTrackMods(trkopt):
    trkmods = [
        (jetmoddict["TrackMoments"],trkopt),
        (jetmoddict["JVF"],trkopt),
        (jetmoddict["JVT"],trkopt)
    ]
    return trkmods

# Translate calib specification into something understood by
# the calibration config helper
def defineCalibFilterMods(jetRecoDict,dataSource,rhoKey="auto"):

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

        if jetRecoDict["dataType"].endswith("tc"):
            calibContext,calibSeq = {
                ("a4","subjes"):   ("TrigRun2","JetArea_EtaJES_GSC"),        # Calo GSC only
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

        elif jetRecoDict["dataType"].endswith("pf"):
            gscDepth = "auto"
            if 'sd' in jetRecoDict["recoAlg"]:
                calibContext = "TrigSoftDrop"
                calibSeq = "EtaJES_JMS"
            else:
                calibContext = "TrigLS2"
                calibSeq = "JetArea_Residual_EtaJES_GSC"
            pvname = "HLT_IDVertex_FS"

        if jetRecoDict["jetCalib"].endswith("IS") and (dataSource=="data"):
            calibSeq += "_Insitu"

        calibSpec = ":".join( [calibContext, dataSource, calibSeq, rhoKey, pvname, gscDepth] )
        from .TriggerJetMods import ConstitFourMom_copy
        if jetalg=="a4":
            calibMods = [(ConstitFourMom_copy,""),
                         getModSpec("CaloEnergies"), # Needed for GSC
                         getModSpec("Calib",calibSpec),
                         getModSpec("Sort")]
        else:
            calibMods = [(ConstitFourMom_copy,""),
                         getModSpec("Calib",calibSpec),
                         getModSpec("Sort")]

    filtercut = {"a4":7000, "a10":50000, "a10r": 50000, "a10t":50000, "a10sd":50000}[jetalg]
    return calibMods + [getModSpec("Filter",filtercut)]
