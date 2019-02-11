#Files for use in configuration unit tests

class defaultTestFiles():
    import os
    d = os.environ.get ('ATLAS_REFERENCE_DATA',
                       '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art')
    EVNT= [d + "/SimCoreTests/mcatnlojimmy_ttbar_leptonfilter.19.2.5.37.EVNT.pool.root"] #find an official file.
    HITS= [d + "/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"]
    RAW = [d + "/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    RDO = ["myRDO.pool.root"]#needs a test RDO
    ESD = [d + '/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root']
    AOD = [d + '/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1']
