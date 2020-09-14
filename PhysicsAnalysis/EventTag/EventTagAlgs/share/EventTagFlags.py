"""
Flag class for EventTag building

usage:
include ('EventTagAlgs/EventTagFlags.py')
EventTagFlags.set_MuonOn()
EventTagFlags.set_ElectornOff()

if EventTagFlags.doAnalysis():
   ...
if EventTagFlags.doPhoton():

"""


class _EventTagFlags(object):

    # attributes
    _attributes = ['Analysis','Electron','DataQuality','MissingET','Muon','ParticleJet',
                   'Photon','TauJet','Trigger','RawSummary']

    # switch on method object
    class _onMethod:
        def __init__(self,target,name):
            self.target = target
            self.name = name
        # method emulation
        def __call__(self):
            setattr(self.target,self.name,True)
            
    # switch off method object
    class _offMethod:
        def __init__(self,target,name):
            self.target = target
            self.name = name
        # method emulation
        def __call__(self):
            setattr(self.target,self.name,False)
            
    # get method object
    class _getMethod:
        def __init__(self,target,name):
            self.target = target
            self.name = name
        # method emulation
        def __call__(self):
            return getattr(self.target,self.name)

    # constructor        
    def __init__(self):
        # set default values, and accessors
        for attr in self._attributes:
            setattr(self,attr,True)
            setattr(self,'set_'+attr+'On',_EventTagFlags._onMethod(self,attr))
            setattr(self,'set_'+attr+'Off',_EventTagFlags._offMethod(self,attr))            
            setattr(self,'do'+attr,_EventTagFlags._getMethod(self,attr))

    # switch on all builders
    def set_AllOn(self):
        for attr in self._attributes:
            setattr(self,attr,True)
            
    # switch off all builders
    def set_NonEventOff(self):
        for attr in self._attributes:
            setattr(self,attr,False)
            
    # dump
    def Print(self):
        print (" === EventTagFlags ===")
        format = "%17s : %s"        
        for attr in self._attributes:
            if getattr(self,attr):
                print (format % (attr, "ON"))
            else:
                print (format % (attr, "--"))
                                
# instantiate            
if not 'EventTagFlags' in dir():
    EventTagFlags = _EventTagFlags()

del _EventTagFlags

