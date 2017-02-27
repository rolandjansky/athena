MissingETAODList = []

outputlist = []
from METReconstruction.METRecoFlags import metFlags
if len(metFlags.METOutputList())==0:
    outputlist = ['Truth','EMTopo','LocHadTopo','Calo','Track',
                  'TruthRegions','EMTopoRegions','LocHadTopoRegions']
else:
    outputlist = metFlags.METOutputList()

for config in outputlist:
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_'+config )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_'+config+'Aux.' )
    if config == 'Truth':
        MissingETAODList.append( 'xAOD::MissingETComponentMap#METMap_'+config )
        MissingETAODList.append( 'xAOD::MissingETAuxComponentMap#METMap_'+config+'Aux.' )

if len(metFlags.METAssocOutputList())==0:
    assocoutput = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow']
    # assocoutput += ['Truth_AntiKt4LCTopo','Truth_AntiKt4EMTopo','Truth_AntiKt4EMPFlow']
else:
    assocoutput = metFlags.METAssocOutputList()
for config in assocoutput:
	MissingETAODList.append( 'xAOD::MissingETAssociationMap#METAssoc_'+config )
	MissingETAODList.append( 'xAOD::MissingETAuxAssociationMap#METAssoc_'+config+'Aux.' )
	MissingETAODList.append( 'xAOD::MissingETContainer#MET_Core_'+config )
	MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Core_'+config+'Aux.' )
	MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reference_'+config )
	MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reference_'+config+'Aux.-ConstitObjectLinks.-ConstitObjectWeights' )
