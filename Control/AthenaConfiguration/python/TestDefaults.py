#Files for use in configuration unit tests

class defaultTestFiles():
    import os
    d = os.environ.get ('ATLAS_REFERENCE_DATA',
                       '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art')
    RAW = [d + "/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    RDO = ["myRDO.pool.root"]#needs a test RDO
    AOD = ["myAOD.pool.root"]#needs a test AOD
    ESD = ["myESD.pool.root"]#needs a test ESD
