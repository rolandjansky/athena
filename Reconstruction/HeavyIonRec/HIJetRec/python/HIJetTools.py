# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from HIJetRec.HIJetRecFlags import HIJetFlags
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecStandard import jtm

#jtm.ptminFinder=0.
from JetRec.JetRecConf import PseudoJetGetter
ClusterKey=HIJetFlags.HIClusterKey()
jtm += PseudoJetGetter("HIget",
                       InputContainer = ClusterKey,
                       Label = "LCTopo", #Label = "Tower",
                       OutputContainer = "PseudoJet" + ClusterKey,
                       SkipNegativeEnergy = True,
                       GhostScale = 0.0)


HIgetters = [jtm.HIget]
if jetFlags.useMuonSegments(): HIgetters += [jtm.gmusegget]
if jetFlags.useTracks(): HIgetters += [jtm.gtrackget,jtm.gakt4trackget]
if jetFlags.useTruth(): HIgetters += [jtm.gtruthget]
jtm.gettersMap['HI']=HIgetters

from HIJetRec.HIJetRecFlags import HIJetFlags
unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()


#wrap in tool .py 
#
#configuring modifier tools
#

#final pT filter
from JetRec.JetRecConf import JetFilter
jetfil5=JetFilter("jetfil5", PtMin = 5000)
jtm.add(jetfil5)

#DR association- form element links for all clusters w/in DR of each jet
DR=HIJetFlags.ClusterDRAssociation()
from HIJetRec.HIJetRecConf import HIJetDRAssociationTool
assoc=HIJetDRAssociationTool("HIJetDRAssociation")
assoc.ContainerKey=ClusterKey
assoc.DeltaR=DR
assoc.AssociationName="%s_DR%dAssoc" % (ClusterKey,int(10*DR))
jtm.add(assoc)

assoc_name=assoc.AssociationName

#calculate discriminants as moments
from HIJetRec.HIJetRecConf import HIJetMaxOverMeanTool
max_over_mean=HIJetMaxOverMeanTool("HIJetMaxOverMean")
jtm.add(max_over_mean)

#filter jets based on discriminants
from HIJetRec.HIJetRecConf import HIJetDiscriminatorTool
discrim=HIJetDiscriminatorTool("HIJetDiscriminator")
discrim.MaxOverMeanCut=HIJetFlags.DCutMaxOverMean()
discrim.MinimumETMaxCut=HIJetFlags.DCutMax()
jtm.add(discrim)

#helper tool to apply bkgr using cells
from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
cell_subtr=HIJetCellSubtractorTool("HIJetSubtractor")
jtm.add(cell_subtr)

jtm.modifiersMap['HI_Unsubtr']=[assoc,max_over_mean,jetfil5] 


modsHI=[]
modsHI+=[jtm.width,jtm.jetens] #uses unsubtracted constituents
modsHI+=[jtm.bchcorrcell,jtm.caloqual_cluster]
# if jetFlags.useTracks(): 
#      modsHI += [jtm.jvf,jtm.trkmoms]
#      modsHI += ["trackassoc"]
# if jetFlags.useTruth(): modsHI+= ["truthassoc"]
#modsHI+=["truthasoc","trackassoc"] #associating truth jets and track jets
# #other moments...

jtm.modifiersMap['HI']=modsHI
