# A test of the property setting/getting on JetDefinition classes
# We want the defining attributes on each class to trigger resets of
# derived attributes, such as container names, labels etc

def printJetConstit(name, jetconstit):
    print "{0} constituents".format(name)
    print "    basetype:        {0}".format(jetconstit.basetype)
    print "    modifiers list:  {0}".format(str(jetconstit.modifiers))
    print "    label:           {0}".format(jetconstit.label)
    print "    input container: {0}".format(jetconstit.inputname)
    print ""

def printJetDefinition(name, jetdef):
    print "{0} jets".format(name)
    print "    algorithm:      {0}".format(jetdef.algorithm)
    print "    radius:         {0}".format(jetdef.radius)
    print "    input label:    {0}".format(jetdef.inputdef.label)
    print "    basename:       {0}".format(jetdef.basename)
    print "    modifiers list: {0}".format(str(jetdef.modifiers))
    print ""

# Start with a basic constituent and print some information
from JetRecConfig.JetDefinition import *
CHSPFlow = JetConstit(xAOD.Type.ParticleFlow, [])
printJetConstit("CHSPFlow",CHSPFlow)

# Now copy and progressively change some of the aspects
from copy import deepcopy
CSSKPFlow = deepcopy(CHSPFlow)
printJetConstit("CSSKPFlow (fresh copy from CHSPFlow)",CSSKPFlow)
CSSKPFlow.modifiers += ["CS","SK"]
printJetConstit("CSSKPFlow (set mods)",CSSKPFlow)

CSSKLCTopo = deepcopy(CSSKPFlow)
printJetConstit("CSSKLCTopo (fresh copy from CSSKPFlow)",CSSKLCTopo)
CSSKLCTopo.basetype = xAOD.Type.CaloCluster
printJetConstit("CSSKLCTopo (set input type)",CSSKLCTopo)

# Now we do this with a basic jet definition
AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,CHSPFlow,ptminfilter=10e3)
AntiKt4EMPFlow.modifiers = ["Calib:T0:mc","Sort"]
printJetDefinition("AntiKt4EMPFlow",AntiKt4EMPFlow)

AntiKt4EMPFlowCSSK = deepcopy(AntiKt4EMPFlow)
printJetDefinition("AntiKt4EMPFlowCSSK (fresh copy from AntiKt4EMPFlow)",AntiKt4EMPFlowCSSK)
AntiKt4EMPFlowCSSK.inputdef = CSSKPFlow
AntiKt4EMPFlowCSSK.modifiers = ["JetConstitFourMomTool"]
printJetDefinition("AntiKt4EMPFlowCSSK (update input def)",AntiKt4EMPFlowCSSK)

AntiKt10EMPFlowCSSK = deepcopy(AntiKt4EMPFlowCSSK)
printJetDefinition("AntiKt10EMPFlowCSSK (fresh copy from AntiKt4EMPFlowCSSK)",AntiKt10EMPFlowCSSK)
AntiKt10EMPFlowCSSK.radius = 1.0
printJetDefinition("AntiKt10EMPFlowCSSK (update radius)",AntiKt10EMPFlowCSSK)

CamKt10EMPFlowCSSK = deepcopy(AntiKt10EMPFlowCSSK)
printJetDefinition("CamKt10EMPFlowCSSK (fresh copy from AntiKt10EMPFlowCSSK)",CamKt10EMPFlowCSSK)
CamKt10EMPFlowCSSK.algorithm = "CamKt"
printJetDefinition("CamKt10EMPFlowCSSK (update radius)",CamKt10EMPFlowCSSK)
