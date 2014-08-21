import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import os
class TarJiveXMLPy(PyAthena.Alg):
    """PyAlgorithm to tar JiveXML files
    """
    def __init__(self, name="TarJiveXMLPy"):
        super(TarJiveXMLPy,self).__init__(name=name)
        pass
    
    def initialize(self):
        return StatusCode.Success
    
    def execute(self):
        return StatusCode.Success
    
    def finalize(self):
        self.msg.debug("finalizing [%s]", self.name())
        self.msg.info("tarring...")
        os.system("tar -czvf JiveXML.tar.gz JiveXML_*")
        return StatusCode.Success