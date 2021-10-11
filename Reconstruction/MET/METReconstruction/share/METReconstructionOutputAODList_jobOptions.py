MissingETAODList = []

from METReconstruction.METRecoFlags import metFlags

# don't write MET quantities to AOD unless the user has specifically said so and locked it
if not metFlags.WriteMETToOutput.is_locked():
	metFlags.WriteMETToOutput.set_Value_and_Lock(False)
if not metFlags.WriteMETAssocToOutput.is_locked():
	metFlags.WriteMETAssocToOutput.set_Value_and_Lock(False)

if metFlags.WriteMETToOutput():
	for config in metFlags.METOutputList():
			MissingETAODList.append( 'xAOD::MissingETContainer#MET_'+config )
			MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_'+config+'Aux.' )
			if config == 'Truth':
					MissingETAODList.append( 'xAOD::MissingETComponentMap#METMap_'+config )
					MissingETAODList.append( 'xAOD::MissingETAuxComponentMap#METMap_'+config+'Aux.' )

if metFlags.WriteMETAssocToOutput():
        if len(metFlags.METAssocOutputList())==0:
                assocoutput = ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow']
        else:
                assocoutput = metFlags.METAssocOutputList()

        for config in assocoutput:
                MissingETAODList.append( 'xAOD::MissingETAssociationMap#METAssoc_'+config )
                MissingETAODList.append( 'xAOD::MissingETAuxAssociationMap#METAssoc_'+config+'Aux.' )
                MissingETAODList.append( 'xAOD::MissingETContainer#MET_Core_'+config )
                MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Core_'+config+'Aux.' )
                MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reference_'+config )
                MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reference_'+config+'Aux.-ConstitObjectLinks.-ConstitObjectWeights' )
