# ScheduleClusterOriginCorrection.py
#
# Teng Jian Khoo
# April 2018
#
# Apply origin correction to topoclusters
# * Checks first if the collection to be created exists in the file
# * Then checks if a correction algorithm was already added to the sequence
#   based on the algorithm name (only)

from AthenaCommon import Logging
tclog = Logging.logging.getLogger('JetRecTools/ScheduleClusterOriginCorrection')

def applyClusterOriginCorrection(scale='LC'):

    clustersname = scale+'OriginTopoClusters'

    # Check if the container was saved to file
    from RecExConfig.AutoConfiguration import IsInInputFile
    if IsInInputFile('xAOD::CaloClusterContainer',clustersname):
        tclog.info('Cluster collection {0} already present -- skip origin correction.'.format(clustersname))
        return

    algname = 'jetalgTCOrigin'+scale

    # Check if the alg has already been scheduled    
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    if hasattr(job,):
        tclog.info('Origin correction alg {0} already present -- skip origin correction.'.format(algname))
        return

    from JetRec.JetRecStandard import jtm
    origintools = {
        'LC': jtm.JetConstitSeq_LCOrigin,
        'EM': jtm.JetConstitSeq_EMOrigin
        }

    # Output a clear error message if a bad scale was specified,
    # then carry on and allow the KeyError to be raised.
    if not scale in origintools.keys():
        tclog.error('Invalid cluster scale {0} supplied! Allowable values: "EM", "LC".'.format(scale))

    toolrunner += JetToolRunner('jetconstitTCOrigin'+scale,
                                EventShapeTools=[],
                                Tools=origintools[scale],
                                )
    jtm += toolrunner

    from JetRec.JetRecConf import JetAlgorithm
    job += JetAlgorithm('jetalgTCOrigin'+scale,
                        Tools=[toolrunner])
