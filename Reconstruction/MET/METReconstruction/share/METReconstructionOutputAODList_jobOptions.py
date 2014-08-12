MissingETAODList = []

outputlist = []
from METReconstruction.METRecoFlags import metFlags
if len(metFlags.METOutputList())==0:
    outputlist = ['Truth','Track','LocHadTopo','RefFinal','Calo',
                  'TruthRegions','TrackRegions','LocHadTopoRegions','RefFinalRegions']
else:
    outputlist = metFlags.METOutputList()

for config in outputlist:
    MissingETAODList.append( 'xAOD::MissingETContainer_v1#MET_'+config )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer_v1#MET_'+config+'Aux.' )
    if not config == 'Calo':
        MissingETAODList.append( 'xAOD::MissingETComponentMap_v1#METMap_'+config )
        MissingETAODList.append( 'xAOD::MissingETAuxComponentMap_v1#METMap_'+config+'Aux.' )
