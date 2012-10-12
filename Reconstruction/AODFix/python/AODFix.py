# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix = logging.getLogger( 'AODFix' )

from AODFix_r160 import *
from AODFix_r170 import *
from AODFix_r172 import *
from AODFixFlags import aodfix

def AODFix_Init():
    from RecExConfig.RecFlags import rec
    firstRun=False
    if rec.inputFileFromAthenaVersion.get_Value()=="n/a" and aodfix.AODFixVersion.get_Value()=="n/a":
        firstRun=True

    import os
    atlasversionstr=""
    atlasproj=os.getenv("AtlasProject")
    if atlasproj != None:
        atlasversionstr+=atlasproj
    atlasversion=os.getenv("AtlasVersion")
    if atlasversion  != None:
        atlasversionstr+="-"+atlasversion
    rec.AtlasReleaseVersion=atlasversionstr
    del atlasversionstr
    
    if "-rel_" in rec.AtlasReleaseVersion.get_Value():
        logAODFix.info("running in a nightly. AODFix not scheduled.")
        logAODFix.info("Please do manually 'rec.doApplyAODFix.set_Value_and_Lock(True)' to overwrite")
        rec.doApplyAODFix.set_Value_and_Lock(False)
        return
    if not ( "-16.0" in rec.AtlasReleaseVersion.get_Value() or "-17.0" in rec.AtlasReleaseVersion.get_Value() or '-17.2' in rec.AtlasReleaseVersion.get_Value() ):
        logAODFix.info("This AODFix is only for releases 16.0, 17.0 and 17.2 releases. AODFix not scheduled.")
        logAODFix.info("Please do manually 'rec.doApplyAODFix.set_Value_and_Lock(True)' to overwrite")
        rec.doApplyAODFix.set_Value_and_Lock(False)
        return
    
    # RDO doesn't have MetaData
    if rec.readRDO():
        if firstRun:
            logAODFix.info("reading BS or RDO, no metadata available. AODFix not scheduled (fix in reconstruction might be applied - check MetaData).")
        rec.doApplyAODFix.set_Value_and_Lock(False)
        rec.inputFileFromAthenaVersion.set_Value("bs")
        aodfix.AODFixVersion.set_Value("bs")
        return

    # determine athena version with which input file was created
    if rec.inputFileFromAthenaVersion.get_Value()=="n/a":
        from RecExConfig.InputFilePeeker import inputFileSummary
        logAODFix.debug("inputfileFromAthenaVersion not determined yet, doing so now")
        AtlasReleaseVersionString='none'
        try:
            AtlasReleaseVersionString=inputFileSummary['metadata']['/TagInfo']['AtlasRelease']
        except Exception:
            logAODFix.warning("no AthenRelease found in input file, setting to <none> !!")
            AtlasReleaseVersionString='none'
        rec.inputFileFromAthenaVersion.set_Value_and_Lock(AtlasReleaseVersionString)
        logAODFix.info("File was produced with Athena version <%s>."%rec.inputFileFromAthenaVersion.get_Value())
    
    if aodfix.AODFixVersion.get_Value()=="n/a":
        from RecExConfig.InputFilePeeker import inputFileSummary
        logAODFix.debug("AODFixVersion not determined yet, doing so now")
        
        AODFixVersionString='none'
        try:
            AODFixVersionString=inputFileSummary['tag_info']['AODFixVersion']
            # logAODFix.info("AODFix ROLF  %s"%AODFixVersionString)
            if AODFixVersionString.startswith("AODFix_"):
                AODFixVersionString=AODFixVersionString[7:]
            # logAODFix.info("AODFix ROLF  %s"%AODFixVersionString)
        except Exception:
            logAODFix.info("no AODFixVersion found in input file, setting to <none>.")
            AODFixVersionString='none'
        aodfix.AODFixVersion.set_Value(AODFixVersionString)
    if firstRun:
        logAODFix.info("Summary of MetaData for AODFix:")
        logAODFix.info(" Inputfile produced with Athena version <%s>."%rec.inputFileFromAthenaVersion.get_Value())
        logAODFix.info(" AODFix version <%s> was previously applied."%aodfix.AODFixVersion.get_Value())
    # now decide, if we have to apply AODFix:
    if rec.doApplyAODFix.is_locked():
        if firstRun:
            logAODFix.warning("rec.doApplyAODFix is locked to <%s>! Not doing anything !"%(rec.doApplyAODFix.get_Value()) )
        return
    # no AODFix for old releases
    if rec.inputFileFromAthenaVersion.get_Value().find("-")<0:
        if firstRun:
            logAODFix.warning("Unknown Athena version in file metadata <%s>, don't know what to do, do nothing !"%rec.inputFileFromAthenaVersion.get_Value() )
        return
    AtlasReleaseVersion=rec.inputFileFromAthenaVersion.get_Value().rsplit("-",2)[1]
    # file produced by nightly ?
    if AtlasReleaseVersion.startswith("rel_"):
        if aodfix.AODFixVersion.get_Value()=='none':
            logAODFix.info("input file produced from a nightly, and no AODFix applied. Will *not* schedule AODFix ! Please do manually 'rec.doApplyAODFix.set_Value_and_Lock(True)' to overwrite")
        else:
            logAODFix.info("input file produced from a nightly, and found AODFix version <%s> already applied. Doing nothing."%aodfix.AODFixVersion.get_Value())
        rec.doApplyAODFix.set_Value_and_Lock(False)
        return

    AtlasReleaseVersionD1=0
    AtlasReleaseVersionD2=0
    AtlasReleaseVersionD3=0
    AtlasReleaseVersionD4=0
    AtlasReleaseVersionD5=0
    if AtlasReleaseVersion.count(".")>=2:
        AtlasReleaseVersionD1=int(AtlasReleaseVersion.split(".",2)[0])
        AtlasReleaseVersionD2=int(AtlasReleaseVersion.split(".",3)[1])
        logAODFix.debug("found AtlasVersion for inputfile : %s.%s taken from %s"%(AtlasReleaseVersionD1,
                                                                                  AtlasReleaseVersionD2,
                                                                                  rec.inputFileFromAthenaVersion.get_Value()))
        AtlasReleaseVersionD3=int(AtlasReleaseVersion.split(".")[2])
        if AtlasReleaseVersion.count(".")>=3:
            AtlasReleaseVersionD4=int(AtlasReleaseVersion.split(".")[3])
        if AtlasReleaseVersion.count(".")>3:
            AtlasReleaseVersionD5=int(AtlasReleaseVersion.split(".")[4])
    else:
        logAODFix.warning("malformed AthenaVersion in file metadata <%s>, not scheduling AODFix !"%AtlasReleaseVersion)
        rec.doApplyAODFix.set_Value_and_Lock(False)
        return
    if  AtlasReleaseVersionD1<16:
        logAODFix.info("old input file, no AODFix available")
        rec.doApplyAODFix.set_Value_and_Lock(False)
        return
    applyAodFixVersion=None
    if AtlasReleaseVersionD1==16 and AtlasReleaseVersionD2==0:
       applyAodFixVersion="v1"
    if AtlasReleaseVersionD1==17 and AtlasReleaseVersionD2==0:
       applyAodFixVersion="r170"
    if AtlasReleaseVersionD1==17 and AtlasReleaseVersionD2==2: 
        # 17.2 is now a bit special, as we first apply version 1 of the AODFix in reconstruvtion and then run only missing ET again in the DPD
        # production, so we must have two AODFix flags. v1 means AODFix in reco has been applid, v2 means MET on AOD has been applied.
        applyAodFixVersion="r172v1"
        if rec.readAOD():
            applyAodFixVersion="r172v2"
    if applyAodFixVersion:
            if aodfix.AODFixVersion.get_Value()=='none':
                if rec.doESD():
                    # running from RAW or ESD, don't apply it
                    logAODFix.info("detected doESD=True, *not* applying AODFix")
                    rec.doApplyAODFix.set_Value_and_Lock(False)
                    aodfix.AODFixVersion.set_Value_and_Lock("none")
                else:
                    # change in policy: no AODFix if reading ESD.
                    if rec.readAOD():
                        logAODFix.debug("The inputfile was produced with athena version %s, which has known problems."%rec.inputFileFromAthenaVersion.get_Value())
                        logAODFix.info("Scheduling AODFix now")
                        rec.doApplyAODFix.set_Value_and_Lock(True)
                        #
                        ## aodfix.AODFixVersion.set_Value_and_Lock(applyAodFixVersion)
                        aodfix.AODFixVersion.set_Value(applyAodFixVersion)
            else:
                logAODFix.debug("AODFix version <%s> already applied to this file"%aodfix.AODFixVersion.get_Value())
                
                # check which version of AODFix was applied, and schedule AODFix, if newer version available
                if "v1" in aodfix.AODFixVersion.get_Value():
                    rec.doApplyAODFix.set_Value_and_Lock(False)
                if "r170" in aodfix.AODFixVersion.get_Value():
                    rec.doApplyAODFix.set_Value_and_Lock(False)
                # this is the newest one, won't need to schedule anything
                if "r172v2_" in aodfix.AODFixVersion.get_Value():
                    logAODFix.info("AODFix version <%s> already applied to this file, newest AODFix availabe in this release"%aodfix.AODFixVersion.get_Value())
                    aodfix.AODFixPreviousMinorVersion.set_Value_and_Lock(int(2))
                    rec.doApplyAODFix.set_Value_and_Lock(False)
                # older versions of this release must run at least parts of AODFix
                if "r172v1_" in aodfix.AODFixVersion.get_Value() and rec.readAOD():
                    logAODFix.info("AODFix version <%s> already applied to this file, newer AODFix version 2 available"%aodfix.AODFixVersion.get_Value())
                    aodfix.AODFixPreviousMinorVersion.set_Value_and_Lock(1)
                    rec.doApplyAODFix.unlock()
                    rec.doApplyAODFix.set_Value_and_Lock(True)
                    aodfix.AODFixVersion.set_Value_and_Lock(applyAodFixVersion)
                    
                if ( not rec.doApplyAODFix() ):
                    aodfix.AODFixVersion.set_Value_and_Lock("none")
    
    return

def AODFix_addMetaData():
    logAODFix.debug("in addMetaData")
    logAODFix.info("RS in addMetaData")
    from RecExConfig.RecFlags import rec

    suffix="RAW"
    if rec.readESD():
        suffix="ESD"
    if rec.readAOD():
        suffix="AOD"

    # reconstruction from raw or re-reconstruction from ESD, write AODFix in any case (the proper fix went into the nightlies)
    # otherwise only if we have doApplyAODFix=true
    if not rec.readRDO() and not rec.doESD():
        if not rec.doApplyAODFix():
            return

    # for releases 16.0 and 17.2 write AODFix in any case, others only if AODFix requested.
    if "-17.2" in rec.AtlasReleaseVersion() or "-16.0" in rec.AtlasReleaseVersion() or rec.doApplyAODFix():
        if rec.readRDO() or rec.doESD() or rec.doApplyAODFix():
            str="AODFix_v1_" + suffix
            if "-17.2" in rec.AtlasReleaseVersion():
                str="AODFix_r172v1_" + suffix
                if rec.readAOD() and rec.doApplyAODFix():
                    str="AODFix_" + aodfix.AODFixVersion.get_Value() + "_" + suffix
            logAODFix.info("running in fixed 17.2 a.k.a. RecoFix is applied - will write MetaData to ESD file" )
            logAODFix.info("executing addMetaData, will add as AODFixVersion %s - doApplyAODFix is %s"% (str,rec.doApplyAODFix()) )
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AODFixVersion", str]
            del str

# inside RecExCommon_topOptions.py:
def AODFix_preInclude():
    logAODFix.debug("in preInclude")
    from RecExConfig.RecFlags import rec
    
    logAODFix.info("executing preInclude %s version %d %s"%(aodfix.AODFixVersion.get_Value(),aodfix.AODFixPreviousMinorVersion.get_Value(),rec.doApplyAODFix()))

    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing preInclude %s version %d %s"%(aodfix.AODFixVersion.get_Value(),aodfix.AODFixPreviousMinorVersion.get_Value(),rec.doApplyAODFix()))
    
    if aodfix.AODFixVersion.get_Value() == "v1":
        AODFix_r160_preInclude()
    elif aodfix.AODFixVersion.get_Value() == "r170":
        AODFix_r170_preInclude()
    elif "r172" in aodfix.AODFixVersion.get_Value():
        logAODFix.info("executing preInclude %s version %d %s"%(aodfix.AODFixVersion.get_Value(),aodfix.AODFixPreviousMinorVersion.get_Value(),rec.doApplyAODFix()))
        AODFix_r172_preInclude()

def AODFix_postAtlfast():
    logAODFix.debug("in postAtlfast")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postAtlfast")

def AODFix_postTrigger():
    logAODFix.debug("in postTrigger")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postTrigger")

def AODFix_postSystemRec():
    logAODFix.debug("in postSystemRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postSystemRec")

    if "r172" in aodfix.AODFixVersion.get_Value():
        AODFix_r172_postSystemRec()
    

def AODFix_postCombinedRec():
    logAODFix.debug("in postCombinedRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postCombinedRec")

    if aodfix.AODFixVersion.get_Value() == "AODFix_v1":
        AODFix_r160_postCombinedRec()
    if "r172" in aodfix.AODFixVersion.get_Value():
        AODFix_r172_postCombinedRec()

def AODFix_postHeavyIon():
    logAODFix.debug("in postHeavyIon")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postHeavyIon")

# inside SystemRec_config.py:
def AODFix_postCaloRec():
    logAODFix.debug("in postCaloRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postCaloRec")

def AODFix_postInDetRec():
    logAODFix.debug("in postInDetRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postInDetRec")

def AODFix_postMuonRec():
    logAODFix.debug("in postMuonRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postMuonRec")

def AODFix_postForwardRec():
    logAODFix.debug("in postForwardRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postForwardRec")

# inside CombinedRec_config.py:
def AODFix_postEgammaRec():
    logAODFix.debug("in postEgammaRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postEgammaRec")

def AODFix_postMuonCombinedRec():
    logAODFix.debug("in postMuonCombinedRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postMuonCombinedRec")

    if aodfix.AODFixVersion.get_Value() == "v1":
        AODFix_r160_postMuonCombinedRec()
        

def AODFix_postJetRec():
    logAODFix.debug("in postJetRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postJetRec")

def AODFix_posttauRec():
    logAODFix.debug("in posttauRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing posttauRec")

def AODFix_postMissingETRec():
    logAODFix.debug("in postMissingETRec")
    from RecExConfig.RecFlags import rec
    if not rec.doApplyAODFix():
        return
    logAODFix.info("executing postMissingETRec")

    if aodfix.AODFixVersion.get_Value() == "v1":
        AODFix_r160_postMissingETRec()
