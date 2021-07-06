# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration



class DETIDClass(object):
  def __init__(self):
    self.LAR    = 23 
    self.TTEM   = 24
    self.TTHEC  = 25
    self.TILE   = 26
    self.FCALEM = 31
    self.FCALHAD= 32

class LayerClass(object):
  def __init__(self):
    self.PreSamplerB = 0
    self.EMB1 = 1
    self.EMB2 = 2
    self.EMB3 = 3
    self.PreSamplerE = 4
    self.EME1 = 5
    self.EME2 = 6
    self.EME3 = 7
    self.HEC0 = 8
    self.HEC1 = 9
    self.HEC2 = 10
    self.HEC3 = 11
    self.TileBar0 = 12
    self.TileBar1 = 13
    self.TileBar2 = 14
    self.TileGap0 = 15
    self.TileGap1 = 16
    self.TileGap2 = 17
    self.TileExt0 = 18
    self.TileExt1 = 19
    self.TileExt2 = 20
    self.FCAL0 = 21
    self.FCAL1 = 22
    self.FCAL2 = 23
    self.Unknown = 24

Layer = LayerClass()
DETID = DETIDClass()

