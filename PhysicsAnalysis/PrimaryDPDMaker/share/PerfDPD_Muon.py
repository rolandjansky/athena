##-----------------------------------------------------------------------------
## Name: PrimaryDPD_MuonStream.py
##
## Author: David Lopez Mateos (Columbia University/Caltech)
## Email:  David.Lopez@cern.ch
##
## Description: This defines the content of the Muon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_Muon.py")


## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PrimaryDPD_MuonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec
## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags_MuonStream import primDPDmu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    muonDPDStream_msg.info('Values of all PerfDPD_Muon flags:')
    print primDPDmu
    pass








##============================================================================
## Define the skimming (event selection) for the DESDM_MUON output stream
##============================================================================
include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from PrimaryDPDMaker.PrimaryDPDMakerConf import MuonFilterAlg
from PrimaryDPDMaker.PrimaryDPDMakerConf import DimuonFilterAlg


## Single Muon skimming flags
## Select muons using algorithm with pt-threshold ABOVE MinPt
SkimMinPt={
    "Staco_combined":primDPDmu.Skim_Staco_combined_MinPt(),
    "Staco_standalone":primDPDmu.Skim_Staco_standalone_MinPt(),
    "Staco_lowpt":primDPDmu.Skim_Staco_lowpt_MinPt(),
    "Muid_combined":primDPDmu.Skim_Muid_combined_MinPt(),
    "Muid_standalone":primDPDmu.Skim_Muid_standalone_MinPt(),
    "Muid_lowpt":primDPDmu.Skim_Muid_lowpt_MinPt(),    
    "Calo_calo":primDPDmu.Skim_Calo_MinPt()}


# Prescale algorithm with pt-threshold BELOW MaxPt
PrescaleMaxPt={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_MaxPt(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_MaxPt(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_MaxPt(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_MaxPt(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_MaxPt(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_MaxPt(),    
    "Calo_calo":primDPDmu.Prescale_Calo_MaxPt()}

# Set the prescale factor
PrescalePrescale={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_prescale(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_prescale(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_prescale(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_prescale(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_prescale(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_prescale(),    
    "Calo_calo":primDPDmu.Prescale_Calo_prescale()}


#Muon pT Cuts(GeV): will create single muon filters with these pt cuts (use only integers!)
pTCuts=[4,6,10,15,20,40]
NpTmax=len(pTCuts)
#Muon Containers
MuonContainers={}
if primDPDmu.UseESDContainer():
    MuonContainers={"Staco":"StacoESDMuonCollection",
                     "Muid":"MuidESDMuonCollection",
                     "Calo":"CaloESDMuonCollection"}
else:
    MuonContainers={"Staco":"StacoMuonCollection",
                     "Muid":"MuidMuonCollection",
                     "Calo":"CaloMuonCollection"}
    pass

# reconstruction Algorithms
MuonAlgs=["Staco","Muid","Calo"]
# muon Authors combined(STACO,MuidCo),standalone(MuonBoy,MuidSA),lowpt(MuTag,MuTagIMO,MuGirl),calo(CaloMuonId,CaloTag,CaloLikelihood)
MuonAuthors=["combined","standalone","lowpt","calo"]
MuonFlag={"combined":"CB","standalone":"SA","lowpt":"TAG","calo":"CALO"}

# ID activity Flag (>0 tracks in ID)
IDActivityFlag={"combined":False,
                "standalone":primDPDmu.IDActivity_standalone(),
                "lowpt":False,
                "calo":False,
                }

# hit requirements
# silicon hits
minTotSiHits={"combined":0,
              "standalone":0,
              "lowpt":primDPDmu.minTotSiHits_tag(),
              "calo":0,
              }
#trigger hits
minTotTrigHits={"combined":0,
                "standalone":primDPDmu.minTotTrigHits_standalone(),
                "lowpt":0,
                "calo":0,
                }

#Muon Spectrometer Precision hits
minPrecisionHits={"combined":0,
                  "standalone":primDPDmu.minPrecisionHits_standalone(),
                  "lowpt":0,
                  "calo":0,
                  }


#
#this DESD stream name
StreamName="MuonStream"
#Filter name prefix
NamePrefix=StreamName+"_FMU"
# list of filter names
FilterList=[]
# prescale factor
PrescaleFactors={}

### BITWise configuration
#
# 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f
#              Muid                                          Staco                 on/off
# 0x [(ptmin bin )  (ptmax bin) (prescale)] [(ptmin bin )  (ptmax bin) (prescale)] [(0/1)]
#        4 bits        4 bits     4 bits       4 bits        4 bits      4 bits     4 bits
ptmin_mask=0xf00
ptmin_shift=8
ptmax_mask=0x0f0
ptmax_shift=4
prescale_mask=0x00f
##

words={
    "combined":primDPDmu.CBWord(),
    "standalone":primDPDmu.SAWord(),
    "lowpt":primDPDmu.TAGWord(),
    "calo":primDPDmu.CALOWord()}


for reco in words.keys():
    word=words[reco]
    # print "for reco ",reco," word is ",hex(word)
    switch=word&0xf
    word=word>>4
    if switch==1:
        # print "switch is ON"
        for i in range(0,2):
            # print hex(word)
            jptmin=(word&ptmin_mask)>>ptmin_shift
            jptmax=(word&ptmax_mask)>>ptmax_shift
            prescale=word&prescale_mask
            # print "ptmin ",hex(jptmin)," ptmax ",hex(jptmax)," prescale ",hex(prescale)
            # print "ptmin ",jptmin," ptmax ",jptmax," prescale ",prescale            
            
            if jptmin==0: pTminCut=0
            elif jptmin>=NpTmax: pTminCut=pTCuts[NpTmax-1]+1
            else: pTminCut=pTCuts[jptmin-1]
            
            if jptmax==0: pTmaxCut=0
            elif jptmax>=NpTmax: pTmaxCut=pTCuts[NpTmax-1]+1
            else: pTmaxCut=pTCuts[jptmax-1]
            
            if reco != "calo":
                key=MuonAlgs[i]+"_"+reco
                pass
            elif reco == "calo" and i==0:
                key=MuonAlgs[2]+"_"+reco
            else: continue
            word=word>>4*3
            # print "key is",key
            SkimMinPt[key]=pTminCut*Units.GeV
            PrescaleMaxPt[key]=pTmaxCut*Units.GeV
            if prescale<=0: prescale=1
            PrescalePrescale[key]=prescale
            pass
        pass
    pass



#print "################ ", StreamName," ###################"
#for flag in SkimMinPt.keys():
#    print "Filters FMU ",flag
#    print "Skim from pT      ",SkimMinPt[flag]
#    print "Prescale up to pT ",PrescaleMaxPt[flag]
#    print "a factor          ",PrescalePrescale[flag]
#    pass


## create single muon filters
if primDPDmu.SelectSingleMuons():
    for pt in pTCuts:
        for alg in MuonAlgs:
            for author in MuonAuthors:
                if alg=="Calo" and author!="calo": continue
                if alg!="Calo" and author=="calo": continue
                FilterName=NamePrefix+str(pt)+"_"+alg+MuonFlag[author]    
                key=alg+"_"+author
                if pt*Units.GeV<SkimMinPt[key]: continue
                tmpFilter=MuonFilterAlg(FilterName)
                tmpFilter.MuonIsolationTool  = MuonIsolationTool
                tmpFilter.muonContainer      = MuonContainers[alg]
                tmpFilter.cutPtMinMu         = pt*Units.GeV
                tmpFilter.cutEtaMax          = 4.
                tmpFilter.muonAuthors        = author            
                tmpFilter.doPtTrackIsol      = False
                tmpFilter.doNoTrackIsol      = False
                tmpFilter.doCaloIsol         = False
                tmpFilter.doJetIsol          = False
                tmpFilter.hasIDActivity      = IDActivityFlag[author]
                tmpFilter.minTOTSIHits       = minTotSiHits[author]
                tmpFilter.minTotTrigHits     = minTotTrigHits[author]
                tmpFilter.minPrecisionHits   = minPrecisionHits[author]
                FilterList.append(FilterName)
                topSequence+=tmpFilter
                if pt*Units.GeV<PrescaleMaxPt[key]:
                    tmpPrescale=PrescalePrescale[key]
                    PrescaleFactors.update({FilterName:tmpPrescale})
                else:
                    PrescaleFactors.update({FilterName:1})
                    pass
                pass
            pass
        pass
    pass


## Single Isolated Muon skimming flags
## Select Isolated muons using algorithm with pt-threshold ABOVE MinPt
SkimMinPtIso={
    "Staco_combined":primDPDmu.Skim_Staco_combined_MinPtIso(),
    "Staco_standalone":primDPDmu.Skim_Staco_standalone_MinPtIso(),
    "Staco_lowpt":primDPDmu.Skim_Staco_lowpt_MinPtIso(),
    "Muid_combined":primDPDmu.Skim_Muid_combined_MinPtIso(),
    "Muid_standalone":primDPDmu.Skim_Muid_standalone_MinPtIso(),
    "Muid_lowpt":primDPDmu.Skim_Muid_lowpt_MinPtIso(),    
    "Calo_calo":primDPDmu.Skim_Calo_MinPtIso()}


# Prescale algorithm with pt-threshold BELOW MaxPtIso
PrescaleMaxPtIso={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_MaxPtIso(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_MaxPtIso(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_MaxPtIso(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_MaxPtIso(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_MaxPtIso(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_MaxPtIso(),    
    "Calo_calo":primDPDmu.Prescale_Calo_MaxPtIso()}

# Set the prescale factor
PrescalePrescaleIso={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_prescaleIso(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_prescaleIso(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_prescaleIso(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_prescaleIso(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_prescaleIso(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_prescaleIso(),    
    "Calo_calo":primDPDmu.Prescale_Calo_prescaleIso()}


words={
    "combined":primDPDmu.CBIsoWord(),
    "standalone":primDPDmu.SAIsoWord(),
    "lowpt":primDPDmu.TAGIsoWord(),
    "calo":primDPDmu.CALOIsoWord()}


for reco in words.keys():
    word=words[reco]
    # print "for reco ",reco," word is ",hex(word)
    switch=word&0xf
    word=word>>4
    if switch==1:
        # print "switch is ON"
        for i in range(0,2):
            # print hex(word)
            jptmin=(word&ptmin_mask)>>ptmin_shift
            jptmax=(word&ptmax_mask)>>ptmax_shift
            prescale=word&prescale_mask
            # print "ptmin ",hex(jptmin)," ptmax ",hex(jptmax)," prescale ",hex(prescale)
            # print "ptmin ",jptmin," ptmax ",jptmax," prescale ",prescale            
            
            if jptmin==0: pTminCut=0
            elif jptmin>=NpTmax: pTminCut=pTCuts[NpTmax-1]+1
            else: pTminCut=pTCuts[jptmin-1]
            
            if jptmax==0: pTmaxCut=0
            elif jptmax>=NpTmax: pTmaxCut=pTCuts[NpTmax-1]+1
            else: pTmaxCut=pTCuts[jptmax-1]
            
            if reco != "calo":
                key=MuonAlgs[i]+"_"+reco
                pass
            elif reco == "calo" and i==0:
                key=MuonAlgs[2]+"_"+reco
            else: continue
            word=word>>4*3
            # print "key is",key
            SkimMinPtIso[key]=pTminCut*Units.GeV
            PrescaleMaxPtIso[key]=pTmaxCut*Units.GeV
            if prescale<=0: prescale=1
            PrescalePrescaleIso[key]=prescale
            pass
        pass
    pass



#print "################ ", StreamName," ###################"
#for flag in SkimMinPtIso.keys():
#    print "Filters FMUiso ",flag
#    print "Skim from pT      ",SkimMinPtIso[flag]
#    print "Prescale up to pT ",PrescaleMaxPtIso[flag]
#    print "a factor          ",PrescalePrescaleIso[flag]
#    pass


## create Isolated single muon filters
if primDPDmu.SelectSingleMuons()  and primDPDmu.SelectSingleIsoMuons():
    for pt in pTCuts:
        for alg in MuonAlgs:
            for author in MuonAuthors:
                if alg=="Calo" and author!="calo": continue
                if alg!="Calo" and author=="calo": continue
                FilterName=NamePrefix+"iso"+str(pt)+"_"+alg+MuonFlag[author]    
                key=alg+"_"+author
                if pt*Units.GeV<SkimMinPtIso[key]: continue
                tmpFilter=MuonFilterAlg(FilterName)
                tmpFilter.MuonIsolationTool  = MuonIsolationTool
                tmpFilter.muonContainer      = MuonContainers[alg]
                tmpFilter.cutPtMinMu         = pt*Units.GeV
                tmpFilter.cutEtaMax          = 4.
                tmpFilter.muonAuthors        = author            
                tmpFilter.useIsolationFromContainer = True
                tmpFilter.doPtTrackIsol      = True
                tmpFilter.ptTrackIsol        = primDPDmu.PtTrackIsol()
                tmpFilter.doNoTrackIsol      = False
                tmpFilter.doCaloIsol         = False
                tmpFilter.doJetIsol          = False
                tmpFilter.hasIDActivity      = IDActivityFlag[author]
                tmpFilter.minTOTSIHits       = minTotSiHits[author]
                tmpFilter.minTotTrigHits     = minTotTrigHits[author]
                tmpFilter.minPrecisionHits   = minPrecisionHits[author]
                FilterList.append(FilterName)
                topSequence+=tmpFilter
                if pt*Units.GeV<PrescaleMaxPtIso[key]:
                    tmpPrescale=PrescalePrescaleIso[key]
                    PrescaleFactors.update({FilterName:tmpPrescale})
                else:
                    PrescaleFactors.update({FilterName:1})
                    pass
                pass
            pass
        pass    
    pass




## Special filter for LowBetaCandidates
if primDPDmu.SkimLowBeta() and primDPDmu.SelectSingleMuons():
    topSequence += MuonFilterAlg("MuonStream_LowBetaCandidate")
    topSequence.MuonStream_LowBetaCandidate.muonContainer = "MuGirlLowBetaCollection"
    topSequence.MuonStream_LowBetaCandidate.MuonIsolationTool  = MuonIsolationTool
    topSequence.MuonStream_LowBetaCandidate.cutPtMinMu         = 0.0*Units.GeV
    topSequence.MuonStream_LowBetaCandidate.cutEtaMax          = 10.
    pass


########################## DiMuon Filters
## Di Muon skimming flags
## Select Dimuons using algorithm with M-threshold ABOVE MinMass
SkimMinMass={
    "Staco_combined":primDPDmu.Skim_Staco_combined_MinMass(),
    "Staco_standalone":primDPDmu.Skim_Staco_standalone_MinMass(),
    "Staco_lowpt":primDPDmu.Skim_Staco_lowpt_MinMass(),
    "Muid_combined":primDPDmu.Skim_Muid_combined_MinMass(),
    "Muid_standalone":primDPDmu.Skim_Muid_standalone_MinMass(),
    "Muid_lowpt":primDPDmu.Skim_Muid_lowpt_MinMass(),    
    "Calo_calo":primDPDmu.Skim_Calo_MinMass()}


# Prescale algorithm with Mass BELOW MaxMass
PrescaleMaxMass={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_MaxMass(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_MaxMass(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_MaxMass(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_MaxMass(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_MaxMass(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_MaxMass(),    
    "Calo_calo":primDPDmu.Prescale_Calo_MaxMass()}

# Set the prescale factor
PrescaleHalfPrescale={
    "Staco_combined":primDPDmu.Prescale_Staco_combined_halfprescale(),
    "Staco_standalone":primDPDmu.Prescale_Staco_standalone_halfprescale(),
    "Staco_lowpt":primDPDmu.Prescale_Staco_lowpt_halfprescale(),
    "Muid_combined":primDPDmu.Prescale_Muid_combined_halfprescale(),
    "Muid_standalone":primDPDmu.Prescale_Muid_standalone_halfprescale(),
    "Muid_lowpt":primDPDmu.Prescale_Muid_lowpt_halfprescale(),    
    "Calo_calo":primDPDmu.Prescale_Calo_halfprescale()}


# mass ranges in GeV (use only integers!)
DiMuMassRange=[2,12,40,20000]
NMassRanges=len(DiMuMassRange)-1
# muon pTCut in corresponding mass range
dimuptcut=primDPDmu.Skim_DiMuon_MinPt()
# must be one for each Mass range
Ndimuptcut=int(dimuptcut/Units.GeV)
DiMuPtCut=[  Ndimuptcut, Ndimuptcut, Ndimuptcut]



# Filter Name
NamePrefix=StreamName+"_FDiMU"

### BITWise configuration for DiMuons
#
# 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f
#              Muid                                          Staco                 on/off
# 0x [(Mmin bin )  (Mmax bin) (prescale)] [(Mmin bin )  (Mmax bin) (prescale)] [(0/1)]
#        4 bits        4 bits     4 bits       4 bits        4 bits      4 bits     4 bits
mmin_mask=0xf00
mmin_shift=8
mmax_mask=0x0f0
mmax_shift=4
prescale_mask=0x00f
##

words={
    "combined":primDPDmu.CBDiMuWord(),
    "standalone":primDPDmu.SADiMuWord(),
    "lowpt":primDPDmu.TAGDiMuWord(),
    "calo":primDPDmu.CALODiMuWord()}


for reco in words.keys():
    word=words[reco]
    # print "for reco ",reco," word is ",hex(word)
    switch=word&0xf
    word=word>>4
    if switch==1:
        # print "switch is ON"
        for i in range(0,2):
            # print hex(word)
            jMmin=(word&mmin_mask)>>mmin_shift
            jMmax=(word&mmax_mask)>>mmax_shift
            prescale=word&prescale_mask
            # print "Mmin ",hex(jMmin)," Mmax ",hex(jMmax)," prescale ",hex(prescale)
            # print "Mmin ",jMmin," Mmax ",jMmax," prescale ",prescale            
            
            if jMmin==0: MminCut=0
            elif jMmin>NMassRanges: MminCut=DiMuMassRange[NMassRanges]
            else: MminCut=DiMuMassRange[jMmin-1]
            
            if jMmax==0: MmaxCut=0
            elif jMmax>NMassRanges: MmaxCut=DiMuMassRange[NMassRanges]
            else: MmaxCut=DiMuMassRange[jMmax-1]
            
            if reco != "calo":
                key=MuonAlgs[i]+"_"+reco
                pass
            elif reco == "calo" and i==0:
                key=MuonAlgs[2]+"_"+reco
            else: continue
            word=word>>4*3
            # print "key is",key
            SkimMinMass[key]=MminCut*Units.GeV
            PrescaleMaxMass[key]=MmaxCut*Units.GeV
            if prescale<=0: prescale=1
            PrescaleHalfPrescale[key]=prescale
            pass
        pass
    pass



#print "################ ", StreamName," ###################"
#for flag in SkimMinMass.keys():
#    print "Filters FDiMU ",flag
#    print "Skim from Mass      ",SkimMinMass[flag]
#    print "Prescale up to Mass ",PrescaleMaxMass[flag]
#    print "a half factor       ",PrescaleHalfPrescale[flag]


## loop on diMuon Mass ranges
## loop on MuonAlgs(Staco,Muid,Calo) and authors(combined,standalone,lowpt,calo)
if primDPDmu.SelectDiMuons():
    for IMass in range(0,len(DiMuMassRange)-1):
        Mmin=DiMuMassRange[IMass]
        Mmax=DiMuMassRange[IMass+1]
        pt=DiMuPtCut[IMass]
        for alg1 in MuonAlgs:
            # never mix Staco and Muid and avoid double countind Mu-Calo Calo-Mu
            if alg1=="Calo": alg2List=["Calo"]
            else: alg2List=[alg1,"Calo"]
            for Jauth in range(0,len(MuonAuthors)):
                author1=MuonAuthors[Jauth]
                # use only CB flag
                if primDPDmu.UseOnlyCBTag() and author1!="combined": continue
                # use calo only with Calo and viceversa
                if alg1=="Calo" and author1!="calo": continue
                if alg1!="Calo" and author1=="calo": continue
                for alg2 in alg2List:
                    for Iauth in range(Jauth,len(MuonAuthors)):
                        author2=MuonAuthors[Iauth]
                        if alg2=="Calo" and author2!="calo": continue
                        if alg2!="Calo" and author2=="calo": continue
                        key1=alg1+"_"+author1
                        key2=alg2+"_"+author2
                        # second author supposed to be of equal or lower quality (cb->sa->tag->calo)
                        if Mmin*Units.GeV<SkimMinMass[key2]: continue
                        DiMuonFilterName=NamePrefix+str(pt)+"_"+str(Mmin)+"_"+str(Mmax)+"_"+alg1+MuonFlag[author1]+"_"+alg2+MuonFlag[author2]
                        tmpFilter = DimuonFilterAlg(DiMuonFilterName)
                        tmpFilter.hasIDActivity      = IDActivityFlag[author1]
                        tmpFilter.TagminTOTSIHits       = minTotSiHits[author1]
                        tmpFilter.TagminTotTrigHits     = minTotTrigHits[author1]
                        tmpFilter.ProbeminTOTSIHits       = minTotSiHits[author2]
                        tmpFilter.ProbeminTotTrigHits     = minTotTrigHits[author2]
                        tmpFilter.TagMuonContainer = MuonContainers[alg1]
                        tmpFilter.TagMuonAuthor    = author1
                        if alg1!=alg2: tmpFilter.UseDifferentTagProbeContainers = True
                        tmpFilter.ProbeMuonContainer = MuonContainers[alg2]
                        tmpFilter.ProbeMuonAuthor    = author2
                        # isolation cuts
                        tmpFilter.MuonIsolationTool  = MuonIsolationTool
                        tmpFilter.TaguseIsolationFromContainer = True
                        tmpFilter.ProbeuseIsolationFromContainer = True
                        tmpFilter.TagDoPtTrackIsol   = primDPDmu.DiMuon_DoTagPtTrackIsol()
                        tmpFilter.ProbeDoPtTrackIsol = primDPDmu.DiMuon_DoProbePtTrackIsol()
                        tmpFilter.TagPtTrackIsol     = primDPDmu.DiMuon_TagPtTrackIsol()
                        tmpFilter.ProbePtTrackIsol   = primDPDmu.DiMuon_ProbePtTrackIsol()
                        # kine cuts
                        tmpFilter.MinInvariantMass = Mmin*Units.GeV;
                        tmpFilter.MaxInvariantMass = Mmax*Units.GeV;
                        tmpFilter.UseOnlyOppositeCharge = primDPDmu.Skim_DiMuon_OppositeSign()
                        if key1!=key2: tmpFilter.DeltaPhiCut = primDPDmu.Skim_DiMuon_DeltaPhi()
                        else: tmpFilter.DeltaPhiCut = 0.0
                        # tag cut
                        tmpFilter.TagCutPtMinMu = pt*Units.GeV
                        tmpFilter.TagCutEtaMax = 4
                        # probe cuts
                        tmpFilter.ProbeCutPtMinMu = pt*Units.GeV
                        tmpFilter.ProbeCutEtaMax = 4                    
                        FilterList.append(DiMuonFilterName)
                        topSequence+=tmpFilter
                        if Mmin*Units.GeV<PrescaleMaxMass[key2]:
                            tmpPrescale=PrescaleHalfPrescale[key1]*PrescaleHalfPrescale[key2]
                            PrescaleFactors.update({DiMuonFilterName:tmpPrescale})
                        else:
                            PrescaleFactors.update({DiMuonFilterName:1})
                            pass
                        pass
                    pass
                pass
            pass
        pass
    pass

        
# Define the prescales
commandstring=""

## Single and Di-Muon Prescalers
for filter in FilterList:
    tmpPrescalerName=filter+"_Prescaler"
    tmpPrescale=PrescaleFactors[filter]
    topSequence += PrimaryDPDPrescaler( tmpPrescalerName,
                                        AcceptAlgs = [filter],
                                        Prescale   = tmpPrescale )
    
    if commandstring=="":
        commandstring = " "+tmpPrescalerName+" "
        pass
    else:
        commandstring = commandstring + " or "+tmpPrescalerName
        pass
    pass


## add LowBetaCandidate prescaler
if primDPDmu.SkimLowBeta() and primDPDmu.SelectSingleMuons():
    topSequence += PrimaryDPDPrescaler( "MuonStream_MyPrescalerLowBeta",
                                        AcceptAlgs = ["MuonStream_LowBetaCandidate"],
                                        Prescale   = primDPDmu.LowBetaPrescale() )
    
    if commandstring=="":
        commandstring = "MuonStream_MyPrescalerLowBeta"
        pass
    else:
        commandstring = commandstring + " or MuonStream_MyPrescalerLowBeta"
        pass
    pass







# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
thinSeqDESDM_MUON = AthSequencer("DESDM_MUONThinningSeq")
topSequence += thinSeqDESDM_MUON

# Create the combined decision
thinSeqDESDM_MUON += LogicalFilterCombiner( "StreamDESDM_MUON_AcceptEvent",
                                            cmdstring = commandstring )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESDM_MUON_AcceptEvent" )





##====================================================================
## Define the thinning for the Muon DPD output stream
##====================================================================

#---------------------------------------------------
# PixelCluster, SCT_Cluster, and TRT_DriftCircle Thinning (only available on ESD)
#---------------------------------------------------
if primDPD.ApplyThinning() and ( primDPDmu.ApplyThinning() \
                                 or primDPDmu.ApplyPixelClusterThinning() \
                                 or primDPDmu.ApplySCTClusterThinning() \
                                 or primDPDmu.ApplyTRTDriftCircleThinning()\
                                 or primDPDmu.ApplyCaloCellThinning() \
                                 ) and not primDPD.WriteMuonStream.isVirtual and not primDPD.isVirtual() :
    # Example on how to use the C++ thinning algorithm. If you need to thin different containers
    #  with different selections, you need to create one instance per container to be thinned
    #  (with the SAME thinSvc name, but different instance name).
    from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
    thinSeqDESDM_MUON += ThinContainers("ThinContainersInMuonStream")
    thinSeqDESDM_MUON.ThinContainersInMuonStream.thinSvc                     = 'ThinningSvc/MuonStreamThinning'
    thinSeqDESDM_MUON.ThinContainersInMuonStream.muonCollectionNames         = ["StacoESDMuonCollection",
                                                                          "MuidESDMuonCollection",
                                                                          "CaloESDMuonCollection",
                                                                          "MuGirlLowBetaCollection"]
    
    if rec.readAOD() :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.muonCollectionNames         = ["StacoMuonCollection",
                                                                              "MuidMuonCollection",
                                                                              "CaloMuonCollection"]
                                                                              
        pass
    
    if rec.readESD() and primDPDmu.ApplyCaloCellThinning() :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.thinCaloCells          = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.caloCellCollectionName = 'AllCalo'
        thinSeqDESDM_MUON.ThinContainersInMuonStream.useDeltaEtaPhiMuon     = False            
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaRMuonObject    = 100
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaEtaMuonObject  = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaPhiMuonObject  = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.keepCellsFromLinkContainers = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.cellLinkContainerNames      = ["MuonClusterCollection_Link"]#,\
                                                                              #"LArClusterEMSofte_Link", \
                                                                              #"egClusterCollection_Link", \
                                                                              #"LArClusterEM_Link", \
                                                                              #"EMTopoCluster430_Link" ]
        #thinSeqDESDM_MUON.ThinContainersInMuonStream.keepForwardElectronCells    = True
        pass


    if rec.readESD() and primDPDmu.ApplyPixelClusterThinning() :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.thinPixelClusters           = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.pixelClusterCollectionName  = 'PixelClusters'
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaEtaPixelMuonObject = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaPhiPixelMuonObject = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaZPixelMuonObject   = 100            
        pass
        
    if rec.readESD() and primDPDmu.ApplySCTClusterThinning() :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.thinSCTClusters             = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.sctClusterCollectionName    = 'SCT_Clusters'
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaEtaSCTMuonObject = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaPhiSCTMuonObject = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaZSCTMuonObject   = 100            
        pass

    if rec.readESD() and primDPDmu.ApplyTRTDriftCircleThinning() :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.thinTRTDriftCircles           = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.trtDriftCirclesCollectionName = 'TRT_DriftCircles'
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaEtaTRTMuonObject      = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaPhiTRTMuonObject      = 0.5
        thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaZTRTMuonObject        = 100        
        pass

    if rec.readESD() and ( primDPDmu.ApplyPixelClusterThinning() \
                           or primDPDmu.ApplySCTClusterThinning() \
                           or primDPDmu.ApplyTRTDriftCircleThinning() \
                           ) :
        thinSeqDESDM_MUON.ThinContainersInMuonStream.keepPRDFromTracks           = True
        thinSeqDESDM_MUON.ThinContainersInMuonStream.trackContainerNamesForPRD   = [ "Tracks" ]
        pass

    thinSeqDESDM_MUON.ThinContainersInMuonStream.keepObjectsNearLvl1Rois     = True
    thinSeqDESDM_MUON.ThinContainersInMuonStream.Lvl1RoiNames                = [ "MU0","MU4", "MU5", "MU6", "MU8", "MU10",
                                                                      "MU11", "MU15", "MU20", "MU40" ]
    thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaRLvl1RoiObject      = 0.5
    
    thinSeqDESDM_MUON.ThinContainersInMuonStream.keepObjectsNearMuons    = True
    thinSeqDESDM_MUON.ThinContainersInMuonStream.minEtMuon               = 0.0*Units.GeV
    thinSeqDESDM_MUON.ThinContainersInMuonStream.maxDeltaRMuonObject     = 0.5
    
    pass




##====================================================================
## Define the Muon DPD output stream
##====================================================================
streamName = primDPD.WriteMuonStream.StreamName
fileName   = buildFileName( primDPD.WriteMuonStream )
if primDPD.WriteMuonStream.isVirtual or primDPD.isVirtual() :
    MuonStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    MuonStream=MSMgr.NewPoolStream( streamName, fileName )
    pass


if primDPD.ApplySkimming() and primDPDmu.ApplySkimming() :
    MuonStream.AddAcceptAlgs( ["StreamDESDM_MUON_AcceptEvent"] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Standard ones
dpdOutput.addBasicOutput(streamName)
dpdOutput.addBasicPhysics(streamName)
dpdOutput.addMuons(streamName)
dpdOutput.addTau(streamName)
dpdOutput.addMissingEt(streamName)
dpdOutput.addTracks(streamName)
dpdOutput.addTrackParticles(streamName)
dpdOutput.addTrigger(streamName)
if primDPD.UseMCTruth() : dpdOutput.addTruth(streamName)

if primDPDmu.WriteAllCaloContainer(): MuonStream.AddItem( ["CaloCellContainer#AllCalo"] )

# Special to this stream
if rec.readESD() and primDPDmu.WriteIdPrepRawData() :
    dpdOutput.addInnerDetectorPrepRawData(streamName)
    pass


##====================================================================
## Finalize the thinning of the containers for this stream
##====================================================================
if primDPD.ApplyThinning() and primDPDmu.ApplyThinning() and not primDPD.WriteMuonStream.isVirtual and not primDPD.isVirtual() :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    if ( rec.readESD() and primDPDmu.ApplySCTClusterThinning() ) \
           or ( rec.readESD() and primDPDmu.ApplyPixelClusterThinning() ) \
           or ( rec.readESD() and primDPDmu.ApplyTRTDriftCircleThinning() \
                or ( rec.readESD() and primDPDmu.ApplyCaloCellThinning()) ):
        augStream=MSMgr.GetStream( primDPD.WriteMuonStream.StreamName )
        evtStream=augStream.GetEventStream()
        svcMgr += createThinningSvc( svcName="MuonStreamThinning", outStreams=[evtStream] )
        
        from RegionSelector.RegionSelectorConf import RegSelSvc
        svcMgr += RegSelSvc("RegSelSvcInMuonStream")
        svcMgr.RegSelSvcInMuonStream.enableID    = True
        svcMgr.RegSelSvcInMuonStream.enablePixel = True
        svcMgr.RegSelSvcInMuonStream.enableSCT   = True
        svcMgr.RegSelSvcInMuonStream.enableTRT   = True

        thinSeqDESDM_MUON.ThinContainersInMuonStream.RegionSelectorTool = svcMgr.RegSelSvcInMuonStream
        

        from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
        svcMgr+=SCT_CablingSvc()
        
        IOVDbSvc = Service("IOVDbSvc")
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested('/SCT/DAQ/Configuration/ROD'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/MUR'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/RODMUR'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/Geog'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog")
        svcMgr.SCT_CablingSvc.DataSource = "CORACOOL"
        pass
        
    pass  # End: if primDPD.ApplyThinning()
