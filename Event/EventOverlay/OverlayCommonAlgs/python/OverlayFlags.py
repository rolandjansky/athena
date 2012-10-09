# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Flag class for Pileup Overlay building

usage:
from OverlayCommonAlgs.OverlayFlags import OverlayFlags
OverlayFlags.set_MuonOn()
OverlayFlags.set_ElectornOff()

if OverlayFlags.doLAr():
   ...
if OverlayFlags.doPixel():

"""

class _OverlayFlags(object):

    # attributes
    _InDet   = ['Pixel','SCT','TRT'] 
    _Calo    = ['LAr','Tile']
    _Muon    = ['MDT','CSC','RPC','TGC']
    _Trigger = ['LVL1']
    _Truth   = ['Truth']
    _Beam    = ['BCM', 'LUCID']
    _Signal  = ['Signal']
    _Bkg     = ['Bkg']
    _attributes = _InDet+_Calo+_Muon+_Trigger+_Truth+_Beam+_Signal+_Bkg

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
            setattr(self,'set_'+attr+'On',_OverlayFlags._onMethod(self,attr))
            setattr(self,'set_'+attr+'Off',_OverlayFlags._offMethod(self,attr))            
            setattr(self,'do'+attr,_OverlayFlags._getMethod(self,attr))

    # switch on all builders
    def set_AllOn(self):
        for attr in self._attributes:
            setattr(self,attr,True)
            
    # switch off all builders
    def set_AllOff(self):
        for attr in self._attributes:
            setattr(self,attr,False)

    # switch on the InDet             
    def set_InDetOn(self):
        for attr in self._InDet:
            setattr(self,attr,True)

    # switch off the InDet             
    def set_InDetOff(self):
        for attr in self._InDet:
            setattr(self,attr,False)

    # switch on the Calo             
    def set_CaloOn(self):
        for attr in self._Calo:
            setattr(self,attr,True)

    # switch off the Calo             
    def set_CaloOff(self):
        for attr in self._Calo:
            setattr(self,attr,False)

    # switch on the Muons  
    def set_MuonOn(self):
        for attr in self._Muon:
            setattr(self,attr,True)

    # switch off the Muons  
    def set_MuonOff(self):
        for attr in self._Muon:
            setattr(self,attr,False)

    # switch on the  Beam 
    def set_BeamOn(self):
        for attr in self._Beam:
            setattr(self,attr,True)

    # switch off the Beam  
    def set_BeamOff(self):
        for attr in self._Beam:
            setattr(self,attr,False)
 
    # switch on the  Trigger 
    def set_TriggerOn(self):
        for attr in self._Trigger:
            setattr(self,attr,True)

    # switch off the Trigger  
    def set_TriggerOff(self):
        for attr in self._Trigger:
            setattr(self,attr,False)

    # dump
    def Print(self):
        print " === OverlayFlags ==="
        format = "%17s : %s"        
        for attr in self._attributes:
            if getattr(self,attr):
                print format % (attr, "ON")
            else:
                print format % (attr, "--")
                                
# instantiate            
if not 'OverlayFlags' in dir():
    OverlayFlags = _OverlayFlags()

del _OverlayFlags


