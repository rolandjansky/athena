# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.Logging import logging


def BunchCrossingCondAlgDefault():
    name = 'BunchCrossingCondAlgDefault'
    mlog = logging.getLogger(name)

    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    
    from LumiBlockComps.LumiBlockCompsConf import BunchCrossingCondAlg
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.BeamFlags import beamFlags

    run1=(conddb.dbdata == 'COMP200')
    folder = None

    if conddb.isMC and beamFlags.bunchStructureSource() != 1:
        mlog.info('This is MC, trying to reset beamFlags.bunchStructureSource to 1')
        beamFlags.bunchStructureSource = 1

    if beamFlags.bunchStructureSource() == 1:
        folder = "/Digitization/Parameters"
        from AthenaCommon.DetFlags import DetFlags
        if not DetFlags.digitize.any_on():
            if not conddb.folderRequested(folder):
                mlog.info("Adding folder %s", folder)
                conddb.addFolderWithTag('', folder, 'HEAD',
                                        className = 'AthenaAttributeList')
            else:
                mlog.info("Folder %s already requested", folder)
        else:
            # Here we are in a digitization job, so the
            # /Digitization/Parameters metadata is not present in the
            # input file and will be created during the job
            mlog.info("Folder %s will be created during the job.", folder)
    elif beamFlags.bunchStructureSource() == 0:
        folder = '/TDAQ/OLC/LHC/FILLPARAMS'
        # Mistakenly created as multi-version folder, must specify HEAD 
        conddb.addFolderWithTag('TDAQ', folder, 'HEAD',
                                className = 'AthenaAttributeList')
    # other possibilities for BunchStructureSource: assume we are running in a context
    # where dependencies are set up
        
    alg = BunchCrossingCondAlg(name,
                               Run1=run1,
                               FillParamsFolderKey =folder,
                               Mode=beamFlags.bunchStructureSource() )

    condSeq += alg

    return alg
