# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags
from InDetRecExample.TrackingCommon  import setDefaults
import InDetRecExample.TrackingCommon as TrackingCommon
from AthenaCommon.GlobalFlags import globalflags
from PrimaryDPDMaker.PrimaryDPDFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Constants import INFO # ,VERBOSE,DEBUG

def getPixelPrepDataToxAOD(name='xAOD_PixelPrepDataToxAOD', **kwargs) :
    the_name = name

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
      conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
    if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
      conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "PixelConfigCondAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
      condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg")
    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
      condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")
    if not hasattr(condSeq, "PixelDCSCondStatusAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
      condSeq += PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg")

    isIdTrkDxAODSimulation = globalflags.DataSource == 'geant4'
    add_IDTIDE_content = kwargs.pop('AddIDTIDEContent', jobproperties.PrimaryDPDFlags.WriteDAOD_IDTIDEStream.get_Value() is True )
    need_pix_ToTList =add_IDTIDE_content or (InDetDxAODFlags.DumpPixelRdoInfo() or InDetDxAODFlags.DumpPixelNNInfo() )

    topSequence = AlgSequence()
    the_PixelChargeToTConversion_name = "PixelChargeToTConversionSetter"
    if need_pix_ToTList and not hasattr(topSequence,the_PixelChargeToTConversion_name) :
        from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion
        PixelChargeToTConversionSetter = PixelChargeToTConversion(name = the_PixelChargeToTConversion_name,
                                                                  ExtraOutputs = ['PixelClusters_ToTList'])
        topSequence += PixelChargeToTConversionSetter

    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    return PixelPrepDataToxAOD( name = the_name,
                                **setDefaults( kwargs,
                                               ClusterSplitProbabilityName = TrackingCommon.pixelClusterSplitProbName(),
                                               ExtraInputs         = ['PixelClusters_ToTList'] if need_pix_ToTList  else [],
                                               LorentzAngleTool    = TrackingCommon.getPixelLorentzAngleTool(),
                                               OutputLevel         = INFO,
                                               UseTruthInfo        = (add_IDTIDE_content and isIdTrkDxAODSimulation),
                                               WriteRDOinformation = InDetDxAODFlags.DumpPixelRdoInfo() or add_IDTIDE_content,
                                               WriteNNinformation  = InDetDxAODFlags.DumpPixelNNInfo() or add_IDTIDE_content,
                                               # ,WriteSDOs            = True
                                               # ,WriteSiHits          = True # if available
                                              ) )

