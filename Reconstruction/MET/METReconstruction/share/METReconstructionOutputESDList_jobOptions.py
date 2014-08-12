MissingETESDList = []

outputlist = []
from METReconstruction.METRecoFlags import metFlags
if len(metFlags.METOutputList())==0:
    outputlist = ['Truth','Track','LocHadTopo','RefFinal','Calo',
                  'TruthRegions','TrackRegions','LocHadTopoRegions','RefFinalRegions']
else:
    outputlist = metFlags.METOutputList()

for config in outputlist:
    MissingETESDList.append( 'xAOD::MissingETContainer_v1#MET_'+config )
    MissingETESDList.append( 'xAOD::MissingETAuxContainer_v1#MET_'+config+'Aux.' )
    if not config == 'Calo':
        MissingETESDList.append( 'xAOD::MissingETComponentMap_v1#METMap_'+config )
        MissingETESDList.append( 'xAOD::MissingETAuxComponentMap_v1#METMap_'+config+'Aux.' )
