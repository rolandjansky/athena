# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def addAlfaDcsMetadata( d3pdalg = None ):

	"""Helper function that adds the necessary tool(s) and service(s) to the
	   job to save the trigger configuration metadata to the output D3PD
	   file.

		Arguments:
		  d3pdalg: The D3PD::MakerAlg that is creating the D3PD. If not specified,
				   the configuration is saved in a file called TrigConfig.root
	"""

	# Create a logger for the function:
	if "logger" in dir(): orig_logger = logger
	from AthenaCommon.Logging import logging
	logger = logging.getLogger( "addAlfaDcsMetadata" )

	# Let the user know what we're doing:
	logger.info( "Adding ALFA DCS metadata to the D3PD" )

	# The tool needs a special D3PDSvc in which the indexing is turned off
	# for the TTree-s:
	_d3pdSvcName = "AlfaDcsD3PDSvc"
	from AthenaCommon.AppMgr import ServiceMgr
	if not hasattr( ServiceMgr, _d3pdSvcName ):
		from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
		ServiceMgr += D3PD__RootD3PDSvc( _d3pdSvcName )
		ServiceMgr.AlfaDcsD3PDSvc.MasterTree = ""
		ServiceMgr.AlfaDcsD3PDSvc.IndexMajor = ""
	else:
		logger.info( "The private D3PDSvc for the metadata TTree already exists" )
	_d3pdSvc = getattr( ServiceMgr, _d3pdSvcName )

	# If no D3PD::MakerAlg has been provided, create a dummy one:
	if d3pdalg == None:
		logger.warning( "No D3PD MakerAlg given to function!" )
		logger.warning( "The DCS data will be saved into file: " + "\"AlfaDCS.root\"" )
		from AthenaCommon.AlgSequence import AlgSequence
		theJob = AlgSequence()
		import D3PDMakerCoreComps
		d3pdalg = D3PDMakerCoreComps.MakerAlg( "AlfaDCSAlg", theJob, file = "AlfaDCS.root", D3PDSvc = _d3pdSvc )

	# Add the metadata tool:
	_d3pdToolName = "AlfaMetaDataTool"
	if not _d3pdToolName in [ t.name() for t in d3pdalg.MetadataTools ]:
		import ForwardDetectorsD3PDMaker
		d3pdalg.MetadataTools += [
			ForwardDetectorsD3PDMaker.AlfaMetaDataTool( _d3pdToolName, D3PDSvc = _d3pdSvc, MetaDataDir=d3pdalg.TuplePath+"Meta")
			]
	else:
		logger.info( "AlfaMetaDataTool was already added to the D3PD::MakerAlg" )

	# Restore the original logger if necessary:
	if "orig_logger" in dir(): logger = orig_logger

	return
