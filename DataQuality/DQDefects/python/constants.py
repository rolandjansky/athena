# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# This defines some constants for the DQDefects module

from DQUtils import define_iov_type

DEFAULT_CONNECTION_STRING = "COOLOFL_GLOBAL/CONDBR2"

PARENT_FOLDERSET = "/GLOBAL/DETSTATUS"
DEFECTS_FOLDER = "/GLOBAL/DETSTATUS/DEFECTS"
DEFECT_FOLDER_DESC = "Data Quality defects folder"

DEFECT_LOGIC_FOLDER = "/GLOBAL/DETSTATUS/DEFECTLOGIC"
DEFECT_LOGIC_FOLDER_DESC = "Data Quality virtual defects logic"
DEFECT_LOGIC_TAG_FORMAT = "DetStatusDEFECTLOGIC-%04i"

@define_iov_type
def DEFECT_IOV(channel, present, recoverable, user, comment):
    pass
