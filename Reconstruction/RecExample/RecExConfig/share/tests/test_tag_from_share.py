###
### example file for testing ObjKeyStore loading mechanism
###

def _fill( item ):
    item.add( {
      'JetCollection' : ['Tag_Cone4Jets', 'Tag_Cone7Jets',
                         'Tag_KtJets',    'Tag_Cone4TruthJets'],
      'McEventCollection' : ['Tag_GEN_EVENT', 'Tag_GEN_AOD', 'Tag_TruthEvent']
    } )

    ## we intentionally erase the previous 'streamTAG' definition
    item.removeAllItems( 'McEventCollection#Tag_GEN_EVENT'  )
    item.removeAllItems( 'McEventCollection#Tag_GEN_AOD'    )
    item.removeAllItems( 'McEventCollection#Tag_TruthEvent' )
    return
    
