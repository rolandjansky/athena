# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class DefectExistsError(Exception):
    "The defect ID or name which was inserted already exists in COOL"

class DefectUnknownError(RuntimeError):
    "The defect ID or string specified is unknown in the COOL database"

class InvalidTagError(RuntimeError):
	"The tag specified is unknown"

class InvalidDefectTagError(InvalidTagError):
    """
    The defects tag was requested, but the requested tag couldn't be resolved
    for the defect folder.
    """
    
class InvalidLogicTagError(InvalidTagError):
    """
    The logic tag was requested, but the requested tag couldn't be resolved
    for the logic folder.
    """
