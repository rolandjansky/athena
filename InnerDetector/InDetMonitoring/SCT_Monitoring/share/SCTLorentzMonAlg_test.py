#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTLorentzMonAlg.py
@author Susumu Oda
@date 2019-04-02
@brief Based on AthenaMonitoring/ExampleMonitorAlgorithm.py
'''

from SCTLorentzMonAlg import SCTLorentzMonAlgConfig

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG,INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'SCTLorentzMonOutput.root'
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Detector.GeometryID = True
    ConfigFlags.Detector.GeometryMuon = True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg=AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    sctLorentzMonAcc = SCTLorentzMonAlgConfig(ConfigFlags)
    
    cfg.merge(sctLorentzMonAcc)
    cfg.run()
