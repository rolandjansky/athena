from AthenaCommon.AlgSequence import AthSequencer

def OnlineLumiCalibrationCondAlgDefault():
    name = 'OnlineLumiCalibrationCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from CoolLumiUtilities.CoolLumiUtilitiesConf import \
         OnlineLumiCalibrationCondAlg

    from IOVDbSvc.CondDB import conddb
    if conddb.isMC:
        return OnlineLumiCalibrationCondAlg()

    # For both runs 1 and 2
    folder = '/TDAQ/OLC/CALIBRATIONS'

    alg = OnlineLumiCalibrationCondAlg (CalibrationFolderName = folder,
                                        CondDataName = 'OnlineLumiCalibrationCondData')
    condSeq += alg

    # Mistakenly created as multi-version folder, must specify HEAD 
    if not conddb.folderRequested( folder ):
        conddb.addFolder ('TDAQ', folder, className='CondAttrListCollection')

    return alg
