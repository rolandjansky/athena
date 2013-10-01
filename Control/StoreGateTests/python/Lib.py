# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: StoreGateTests/python/Lib.py
# @purpose: a set of Py-components to tests py-record performances
# @author: Sebastien Binet <binet@cern.ch>

from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena

class PySgStressProducer( PyAthena.Alg ):
    """A simple python algorithm to create PayLoads
    """
    def __init__(self, name = "PySgStressProducer", **kw):
        ## init base class
        kw['name'] = name
        super(PySgStressProducer,self).__init__(**kw)

        if not kw.has_key('DataName'):     self.DataName = "MyData"
        if not kw.has_key('NbrOfObjects'): self.NbrOfObjects = 1000
        if not kw.has_key('ObjectsSize'):  self.ObjectsSize  = 100
        if not kw.has_key('UseDataPool'):  self.UseDataPool  = False

    def initialize(self):
        self.msg.info( "Initializing %s", self.name() )
        self.sg = PyAthena.py_svc ("StoreGateSvc")
        if not self.sg:
            self.msg.error ("could not retrieve event store")
            return StatusCode.Failure
        #PyROOTFixes.fix_dv_container( "SgTests::PayLoadDv" )
        return StatusCode.Success

    def execute(self):
        self.msg.debug( "Executing %s...", self.name() )
        if self.createData() != StatusCode.Success:
            self.msg.error( "Could not create PayLoad data !!" )
            return StatusCode.Failure
        return StatusCode.Success

    def createData(self):
        _makePayLoadDv = PyAthena.SgTests.PayLoadDv
        _makePayLoad   = PyAthena.SgTests.PayLoad
        _sg_record   = self.sg.record
        _sg_setConst = self.sg.setConst
        
        allGood = True
        for i in xrange(self.NbrOfObjects):
            outName = "%s_payload_%i" % (self.DataName, i)
            dv = _makePayLoadDv()
            data = _makePayLoad()
            dv.push_back( data )
            try:
                _sg_record(dv, outName)
            except Exception:
                del dv
                self.msg.error( "Could not store data at [%s] !!", outName )
                allGood = False
            if not _sg_setConst(dv).isSuccess():
                self.msg.warning("Could not setConst data at [%s] !!", outName)

            # filling data
            data = data.m_data
            data.reserve( self.ObjectsSize )
            pback = data.push_back
            for j in xrange(self.ObjectsSize): pback( j )
            pass # loop over NbrOfObjects
        if allGood: return StatusCode.Success
        return StatusCode.Failure

    def finalize(self):
        self.msg.info( "Finalizing %s...", self.name() )
        return StatusCode.Success

    pass # PySgStressProducer


class PySgStressConsumer(PyAthena.Alg):
    """A simple python algorithm to retrieve PayLoads
    """
    def __init__(self, name = "PySgStressProducer", **kw):
        ## init base class
        kw['name'] = name
        super(PySgStressConsumer,self).__init__(**kw)

        if not kw.has_key('DataName'):     self.DataName = "MyData"
        if not kw.has_key('NbrOfObjects'): self.NbrOfObjects = 1000

    def initialize(self):
        self.msg.info( "Initializing %s...", self.name() )
        self.sg = PyAthena.py_svc("StoreGateSvc")
        if not self.sg:
            self.msg.error ("could not retrieve event store")
            return StatusCode.Failure
        return StatusCode.Success

    def execute(self):
        self.msg.debug( "Executing %s...", self.name() )
        return self.readData()

    def readData(self):
        allGood = True
        for i in xrange(self.NbrOfObjects):
            outName = "%s_payload_%i" % (self.DataName, i)
            dv = self.sg.retrieve( "SgTests::PayLoadDv", outName )
            if dv is None:
                self.msg.error( "Could not retrieve payload !!" )
                allGood = False

            data = dv.at(0).m_data
            if data.empty():
                self.msg.error( "**NOT** my data!!" )
                allGood = False
        if allGood: return StatusCode.Success
        return StatusCode.Failure

    def finalize(self):
        self.msg.info( "Finalizing %s...", self.name() )
        return StatusCode.Success
    
    pass # class PySgStressConsumer

        
class PyClidsTestWriter(PyAthena.Alg):
    """A simple algorithm to put simple objects (std::vector<T>, builtins)
    into StoreGate and see what happens
    """

    def __init__(self, name='PyClidsTestWriter', **kw):
        ## init base class
        super(PyClidsTestWriter,self).__init__(**kw)
        self.intsName    = kw.get('intsName',    'TestInts' )
        self.uintsName   = kw.get('uintsName',   'TestUInts')
        self.floatsName  = kw.get('floatsName',  'TestFloats')
        self.doublesName = kw.get('doublesName', 'TestDoubles')
        
        return

    def initialize(self):
        self.msg.info("initializing...")
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc !")
            return StatusCode.Failure

        _info = self.msg.info
        _info("Configuration:")
        _info(" - ints:    [%s]", self.intsName)
        _info(" - uints:   [%s]", self.uintsName)
        _info(" - floats:  [%s]", self.floatsName)
        _info(" - doubles: [%s]", self.doublesName)

        self._test_matrix = {
            'std::vector<int>'          : self.intsName,
            'std::vector<unsigned int>' : self.uintsName,
            'std::vector<float>'        : self.floatsName,
            'std::vector<double>'       : self.doublesName,
            }
        return StatusCode.Success

    def execute(self):
        allGood = True
        _error = self.msg.error
        self.msg.info("running execute...")

        for tpName,sgKey in self._test_matrix.items():
            tp = getattr(PyAthena, tpName)
            cont = tp(); cont.reserve(100)
            for i in xrange(100): cont.push_back(i)
            try:
                self.sg[sgKey] = cont
            except Exception,err:
                _error("Could not record '%s' at [%s] !",tpName,sgKey)
                _error(err)
                allGood = False
            
        if not allGood: return StatusCode.Failure
        self.sg.dump()
        
        return self.testReadBack()

    def testReadBack(self):
        allGood = True
        _info = self.msg.info
        for tpName,sgKey in self._test_matrix.items():
            cont = self.sg.retrieve(tpName,sgKey)
            if not cont: _info('Could not retrieve [%s] !',sgKey)
            cont = [cont[i] for i in xrange(10)]
            _info('[%s] content: %s', sgKey,cont)
            if len( [i for i in xrange(10) if i != cont[i]] ) > 0:
                self.msg.error('[%s] content is NOT as expected !!')
                allGood = False
        if not allGood: return StatusCode.Failure
        return StatusCode.Success
    
    def finalize(self):
        self.msg.info("finalize...")
        return StatusCode.Success

    pass # class PyClidsTestWriter
