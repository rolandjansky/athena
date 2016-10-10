# -*- coding: utf-8 -*-
#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================

from ALFA_LocRec.ALFA_LocRecConf      import ALFA_LocRec
from AthenaCommon.AlgSequence         import AlgSequence

TopLocRecSeq  = AlgSequence()
TopLocRecSeq += ALFA_LocRec("ALFA_LocRec")

alfaLocRec = TopLocRecSeq.ALFA_LocRec


# select reconstruction methods for the MD and the OD
#alfaLocRec.ListAlgoMD = ["MDMultiple", "HalfReco", "EdgeMethod"]
alfaLocRec.ListAlgoMD = ["EdgeMethod"]
alfaLocRec.ListAlgoOD = ["ODTracking"]


# specify local reconstruction properties
alfaLocRec.MultiplicityCutOD = 30
alfaLocRec.DistanceCutOD     = 0.5
alfaLocRec.MultiplicityCutMD = 5
alfaLocRec.OverlapCutMD      = 0.5
alfaLocRec.UVCutMD           = 3
alfaLocRec.NumLayerCutMD     = 3

alfaLocRec.EdgeMethod_Opt_Sisters = False
alfaLocRec.EdgeMethod_Opt_UseGaps = False


# select between a real data (1) or a simulation mode (0),# specify a detector metrology type and its source
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
	alfaLocRec.DataType        = 1
else:
	alfaLocRec.DataType        = 0
alfaLocRec.MetrologyType   = 3
alfaLocRec.MetrologySource = ""

# specify position type and fiber metrology properties of the RP
alfaLocRec.B7L1U_PosType        = 1
alfaLocRec.B7L1U_MDGeometryType = 2
alfaLocRec.B7L1U_ODGeometryType = 2
alfaLocRec.B7L1U_MDSource       = ""
alfaLocRec.B7L1U_ODSource       = ""

alfaLocRec.B7L1L_PosType        = 1
alfaLocRec.B7L1L_MDGeometryType = 2
alfaLocRec.B7L1L_ODGeometryType = 2
alfaLocRec.B7L1L_MDSource       = ""
alfaLocRec.B7L1L_ODSource       = ""

alfaLocRec.A7L1U_PosType        = 1
alfaLocRec.A7L1U_MDGeometryType = 2
alfaLocRec.A7L1U_ODGeometryType = 2
alfaLocRec.A7L1U_MDSource       = ""
alfaLocRec.A7L1U_ODSource       = ""

alfaLocRec.A7L1L_PosType        = 1
alfaLocRec.A7L1L_MDGeometryType = 2
alfaLocRec.A7L1L_ODGeometryType = 2
alfaLocRec.A7L1L_MDSource       = ""
alfaLocRec.A7L1L_ODSource       = ""

alfaLocRec.A7R1U_PosType        = 1
alfaLocRec.A7R1U_MDGeometryType = 2
alfaLocRec.A7R1U_ODGeometryType = 2
alfaLocRec.A7R1U_MDSource       = ""
alfaLocRec.A7R1U_ODSource       = ""

alfaLocRec.A7R1L_PosType        = 1
alfaLocRec.A7R1L_MDGeometryType = 2
alfaLocRec.A7R1L_ODGeometryType = 2
alfaLocRec.A7R1L_MDSource       = ""
alfaLocRec.A7R1L_ODSource       = ""

alfaLocRec.B7R1U_PosType        = 1
alfaLocRec.B7R1U_MDGeometryType = 2
alfaLocRec.B7R1U_ODGeometryType = 2
alfaLocRec.B7R1U_MDSource       = ""
alfaLocRec.B7R1U_ODSource       = ""

alfaLocRec.B7R1L_PosType        = 1
alfaLocRec.B7R1L_MDGeometryType = 2
alfaLocRec.B7R1L_ODGeometryType = 2
alfaLocRec.B7R1L_MDSource       = ""
alfaLocRec.B7R1L_ODSource       = ""
