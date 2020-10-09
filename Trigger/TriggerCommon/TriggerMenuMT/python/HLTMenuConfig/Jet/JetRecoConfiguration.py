#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

##########################################################################################
# Helper functions to digest the reconstruction options dictionary
# and translate it into the python configuration objects used by
# jet reco code.

from JetRecConfig.JetDefinition import JetConstitSeq,JetConstitSource, xAODType, JetDefinition
from . import TriggerJetMods # this is to define the ConstitFourMom_copy JetModifierC
TriggerJetMods.ConstitFourMom_copy
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

        trkopt = jetRecoDict['trkopt']
        from JetRecConfig.ConstModHelpers import constitModWithAlternateTrk
        # Generate a new JetConstitModifier with track proterties setup according to trkopt
        constitModWithAlternateTrk("CorrectPFO", trkopt) 
        constitMods = ["CorrectPFO"+trkopt]
        # apply constituent pileup suppression
        if "cs" in jetRecoDict["dataType"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["dataType"]:
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

            
    if "tc" in jetRecoDict["dataType"]:
        # apply constituent pileup suppression
        if "cs" in jetRecoDict["dataType"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["dataType"]:
            constitMods.append("SK")
        # build a modifier identifier :
        modstring = ''.join(constitMods)
        # prepend the cluster calib state :
        if jetRecoDict["calib"] == "em":
            constitMods = ["EM"] + constitMods
        elif jetRecoDict["calib"] == "lcw":
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

def defineReclusteredJets(jetRecoDict,smallRjets):
    rcJetConstit = JetConstitSource("RCJet", xAODType.Jet, smallRjets, label='JetRC')
    rcJetDef = JetDefinition( "AntiKt", 1.0, rcJetConstit)
    return rcJetDef

def defineGroomedJets(jetRecoDict,ungroomedDef):#,ungroomedJetsName):
    from JetRecConfig.JetGrooming import JetTrimming, JetSoftDrop
    groomAlg = jetRecoDict["recoAlg"][3:] if 'sd' in jetRecoDict["recoAlg"] else jetRecoDict["recoAlg"][-1]
    groomDef = {
        "sd":JetSoftDrop(ungroomedDef,zcut=0.1,beta=1.0),
        "t" :JetTrimming(ungroomedDef,smallR=0.2,ptfrac=0.05),
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
        "TrackMoments:"+trkopt,
        "JVF:"+trkopt,
        "JVT:"+trkopt,
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

        if jetalg=="a4":
            calibMods = ["ConstitFourMom_copy",
                         "CaloEnergies", # Needed for GSC
                         "Calib:"+calibSpec,
                         "Sort"]
        else:
            calibMods = ["ConstitFourMom_copy",
                         "Calib:"+calibSpec,
                         "Sort"]

    filtercut = {"a4":7000, "a10":50000, "a10r": 50000, "a10t":50000, "a10sd":50000}[jetalg]
    return calibMods + ["Filter:"+str(filtercut)]
