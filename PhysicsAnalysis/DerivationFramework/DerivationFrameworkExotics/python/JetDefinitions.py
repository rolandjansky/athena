# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetRec.JetRecStandard import jtm
from JetRec.JetRecFlags import jetFlags

# does not work
#from JetRec.JetRecStandardToolManager import filterout
#jtm.modifiersMap["largeR_notruth"] = filterout(["truthassoc"], jtm.modifiersMap["calib"])

jtm.modifiersMap["largeR_notruth"] = []

if jetFlags.useTracks():
    jtm.modifiersMap["largeR_notruth"] += [jtm.jetorigincorr, "calib", jtm.jetsorter]
jtm.modifiersMap["largeR_notruth"] += ["jetfilter"]

jtm.modifiersMap["largeR_notruth"] += [jtm.width,
                                       jtm.jetisol,
                                       jtm.ktdr,
                                       jtm.nsubjettiness,
                                       jtm.ktsplitter,
                                       jtm.angularity,
                                       jtm.dipolarity,
                                       jtm.planarflow,
                                       jtm.ktmassdrop,
                                       jtm.encorr,
                                       jtm.comshapes ]
if jetFlags.useTruth():
  if jtm.haveParticleJetTools:
    jtm.modifiersMap["largeR_notruth"] += [jtm.partontruthlabel]
    jtm.modifiersMap["largeR_notruth"] += [jtm.truthpartondr]

jtm.modifiersMap["largeR_notruth"] += [jtm.jetens]
jtm.modifiersMap["largeR_notruth"] += [jtm.larhvcorr]
if jetFlags.useCaloQualityTool():
  jtm.modifiersMap["largeR_notruth"] += [jtm.caloqual_cluster]
if jetFlags.useTracks():
  jtm.modifiersMap["largeR_notruth"] += [jtm.jvf, jtm.jvt, jtm.trkmoms]
  jtm.modifiersMap["largeR_notruth"] += ["trackassoc"]
jtm.modifiersMap["largeR_notruth"] += [jtm.clsmoms]
if jetFlags.useTracks():
    jtm.modifiersMap["largeR_notruth"] += [jtm.jetorigincorr]
if jetFlags.useBTagging():
    jtm.modifiersMap["largeR_notruth"] += ["btags"]

print "EXOT derivations use the largeR_notruth modifier for jets defined as: ", jtm.modifiersMap["largeR_notruth"]

