# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

__doc__ = """Algorithm for pruning eventdisplay files
"""
__version__ = "0.2"
__author__  = "Eric Jansen <eric.jansen@cern.ch>"

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode
from EventDisplaysOnline import EventUtils

class PruneAlg( PyAthena.Alg ):

	def __init__(self, name = "PruneAlg", **kw):
		kw['name'] = name
		super(PruneAlg,self).__init__(**kw)

		self.directory = kw.get("OutputDirectory", "/EventDisplayEvents/Default")
		self.numevents = kw.get("NumberOfEvents", 250)
	
	def initialize(self):
		self.msg.debug("Going to prune %s, keeping %d events" % (self.directory, self.numevents))
		return StatusCode.Success

	def execute(self):
		EventUtils.cleanDirectory(self.msg, self.directory, self.numevents)
		return StatusCode.Success

	def finalize(self):
		return StatusCode.Success

