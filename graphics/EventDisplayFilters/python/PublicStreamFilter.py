# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Module that filters public events for Atlas-Live based on IS information
"""
__version__ = "0.1"
__author__  = "Eric Jansen <eric.jansen@cern.ch>"

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode
from PyAnalysisCore import PyEventTools

from ipc import IPCPartition
from ispy import ISObject, ISInfoDictionary, ISInfoAny

import time

class PublicStreamFilter( PyAthena.Alg ):

	def __init__(self, name = "PublicStreamFilter", **kw):
		kw['name'] = name
		super(PublicStreamFilter,self).__init__(**kw)

		self.checkready = kw.get("CheckReadyForPhysics", True)
		self.checkrun = kw.get("CheckRunNumber", True)
		self.t0project = kw.get("Tier0Project", "data10_7TeV")
	
	def initialize(self):
		return StatusCode.Success

	def execute(self):
		if self.checkrun:
			try:
				eventInfo = PyEventTools.getEventInfo("")
			except LookupError, err:
				self.msg.warning("Could not retrieve EventInfo: %s" % err)
				self.setFilterPassed(False)

		try:
			partition = IPCPartition("ATLAS")

			dictionary = ISInfoDictionary(partition)
			readyForPhysics = ISInfoAny()
			dictionary.getValue("RunParams.Ready4Physics", readyForPhysics)
			ready = readyForPhysics.get()
			
			runParams = ISObject(partition, 'RunParams.RunParams','RunParams')
			runParams.checkout()

		except UserWarning, err:
			self.msg.warning("Could not retrieve data from IS: %s" % err)
			self.setFilterPassed(False)
			time.sleep(5)

		self.msg.debug("IS publishes T0_project_tag %s and readyForPhysics %s for run %d" % (runParams.T0_project_tag, ready, runParams.run_number))

		if self.checkready and not ready:
			self.msg.info("ReadyForPhysics not set... rejecting event");
			self.setFilterPassed(False);
			time.sleep(1)

		elif runParams.T0_project_tag != self.t0project:
			self.msg.info("T0_project_tag is not %s (%s)... rejecting event" % (self.t0project, runParams.T0_project_tag))
			self.setFilterPassed(False)
			time.sleep(1)

		elif self.checkrun and runParams.run_number != eventInfo.event_ID().run_number():
			self.msg.info("Information in IS is not for this run (%d != %d)... rejecting event" % (runParams.run_number, eventInfo.event_ID().run_number()))
			self.setFilterPassed(False)

		else:
			self.msg.info("Good event, passing into the public stream...");
			self.setFilterPassed(True)

		return StatusCode.Success

	def finalize(self):
		return StatusCode.Success
