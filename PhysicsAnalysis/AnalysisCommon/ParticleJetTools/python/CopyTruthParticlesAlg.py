# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# CopyTruthParticlesAlgs for Athena
# based loosely on
# atlasoff/Reconstruction/EventShapes/EventShapeTools/python/EventDensityConfig.py
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
class CopyTruthParticlesAlg (PyAthena.Alg):
    """Athena algorithm wrapper around CopyTruthParticles tools"""
    def __init__(self, ctptool, name, **kw):
        self.ctptool = ctptool
        kw['name'] = name
        super(CopyTruthParticlesAlg, self).__init__(**kw)

        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.ctptool = PyAthena.py_tool(self.ctptool.getFullName(), iface="CopyTruthParticles")
        return StatusCode.Success

    def execute(self):
        self.msg.debug('==> executing ...')
        if self.ctptool.execute() != 0:
            return StatusCode.Failure
        else:
            return StatusCode.Success


    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success
