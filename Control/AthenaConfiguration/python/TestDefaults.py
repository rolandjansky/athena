#Files for use in configuration unit tests

class defaultTestFiles():
    import os
    d = os.environ.get ('ATLAS_REFERENCE_DATA',
                       '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art')
    RAW = [d + "/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    RDO = ["myRDO.pool.root"]#needs a test RDO
    AOD = [d + '/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1']
    ESD = ["myESD.pool.root"]#needs a test ESD
