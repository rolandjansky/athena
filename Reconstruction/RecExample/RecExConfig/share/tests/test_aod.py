###
### example file for testing ObjKeyStore loading mechanism
###

def _fill( item ):
    item.add( {
        'CaloCellContainer'    : [ 'AODCellContainer' ],
        'CaloCellLinkContainer': [ 'egClusterCollection_Link' ],
        'CaloClusterContainer' : [ 'CombinedCluster',
                                   'LArClusterEM',
                                   'EMTopoCluster',
                                   'egClusterCollection',
                                   'LArClusterEMSofte' ],
        'DetStatusMap'         : [ 'DetectorStatus' ],
        'egDetailContainer'    : [ 'egDetailAOD' ],
        'egammaContainer'      : [ 'ElectronAODCollection',
                                   'PhotonAODCollection'],
        } )
    return
