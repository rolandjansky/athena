#
# TrackJet 
#
from JetRec.JetGetters import *
from JetMomentTools.JetMomentToolsConf import JetVertexAssociationTool, JetOriginCorrectionTool
#jvaTool = JetVertexAssociationTool("JetVertexAssociation")
from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
jvaTool = getJetVertexAssociationTool("AntiKt",0.4,"LCTopo")
#-- TrackJet --
include("QcdD3PDMaker/JetTrackZClusterTool_jobOptions.py")
#... Anti-Kt D=0.4
my_att_dict1 = dict(jet_attribute_dict)
my_att_dict1['_finderType'] = 'AntiKt'
my_att_dict1['_finderParams'] = [0.4]
my_att_dict1['_inputType'] = 'TrackZ'
my_att_dict1['_inputColl'] = []
mygetter1 = make_customJetGetter(my_att_dict1, [MyAKt4JetTrackZClusterTool,JetSorterTool(),JetWidthTool(),jvaTool])
mygetter1.OutputLevel = INFO
#... Anti-Kt D=0.6
my_att_dict2 = dict(jet_attribute_dict)
my_att_dict2['_finderType'] = 'AntiKt'
my_att_dict2['_finderParams'] = [0.6]
my_att_dict2['_inputType'] = 'TrackZ'
my_att_dict2['_inputColl'] = []
mygetter2 = make_customJetGetter(my_att_dict2, [MyAKt6JetTrackZClusterTool,JetSorterTool(),JetWidthTool(),jvaTool])
mygetter2.OutputLevel = INFO
#... Anti-Kt D=0.5
my_att_dict3 = dict(jet_attribute_dict)
my_att_dict3['_finderType'] = 'AntiKt'
my_att_dict3['_finderParams'] = [0.5]
my_att_dict3['_inputType'] = 'TrackZ'
my_att_dict3['_inputColl'] = []
mygetter3 = make_customJetGetter(my_att_dict3, [MyAKt5JetTrackZClusterTool,JetSorterTool(),JetWidthTool(),jvaTool])
mygetter3.OutputLevel = INFO
#... Kt D=0.4
#my_att_dict3 = dict(jet_attribute_dict)
#my_att_dict3['_finderType'] = 'Kt'
#my_att_dict3['_finderParams'] = [0.4]
#my_att_dict3['_inputType'] = 'TrackZ'
#my_att_dict3['_inputColl'] = []
#mygetter3 = make_customJetGetter(my_att_dict3, [MyKt4JetTrackZClusterTool,JetSorterTool()])
#mygetter3.OutputLevel = INFO
#... Kt D=0.6
#my_att_dict4 = dict(jet_attribute_dict)
#my_att_dict4['_finderType'] = 'Kt'
#my_att_dict4['_finderParams'] = [0.6]
#my_att_dict4['_inputType'] = 'TrackZ'
#my_att_dict4['_inputColl'] = []
#mygetter4 = make_customJetGetter(my_att_dict4, [MyKt6JetTrackZClusterTool,JetSorterTool()])
#mygetter4.OutputLevel = INFO
