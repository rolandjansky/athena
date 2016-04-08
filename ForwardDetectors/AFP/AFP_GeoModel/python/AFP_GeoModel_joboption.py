# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from IOVDbSvc.CondDB import conddb
from AFP_GeoModel.AFP_GeoModelConf import AFP_DetectorTool

def getAFP_DetectorTool(name="AFP_DetectorTool"):

	#instatiate the tool
	theAFP_DetectorTool=AFP_DetectorTool(name)

	return theAFP_DetectorTool
