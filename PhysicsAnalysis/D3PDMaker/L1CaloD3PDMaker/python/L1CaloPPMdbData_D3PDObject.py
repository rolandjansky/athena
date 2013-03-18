# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/L1CaloPPMdbData_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @brief Fills L1Calo PPM Database information
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from IOVDbSvc.CondDB import conddb
conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib')
conddb.addFolder('TRIGGER','/TRIGGER/Receivers/Conditions/VgaDac')
conddb.addFolder('TRIGGER','/TRIGGER/Receivers/Factors/CalibGains')
  

from AthenaCommon.AppMgr import ToolSvc
# include LAr HV Correction tool
from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
theLArHVCorrTool=LArHVCorrToolDefault()
ToolSvc+=theLArHVCorrTool


# Maker
def makeL1CaloPPMdbData_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloPPMdbData_D3PDObject = D3PDObject(makeL1CaloPPMdbData_D3PDObject,
                                        default_object_name = 'L1CaloPPMdbData_D3PDObject',
                                        default_name = 'L1CaloPPMdbDataFiller')

# define blocks

L1CaloPPMdbData_D3PDObject.defineBlock(0,'L1CaloPPMdbData',
                                      func = L1CaloD3PDMaker.L1CaloPPMdbDataFillerTool,
                                      prefix = '',
                                      LArHVCorrTool = theLArHVCorrTool)


