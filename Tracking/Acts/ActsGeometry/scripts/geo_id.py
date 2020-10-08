from __future__ import print_function

# import ROOT

import sys

# file = sys.argv[1]

class GeometryIdentifier:
    volume_mask    = 0xff00000000000000
    boundary_mask  = 0x00ff000000000000
    layer_mask     = 0x0000ff0000000000
    approach_mask  = 0x000000f000000000
    sensitive_mask = 0x0000000ffff00000
    channel_mask   = 0x00000000000fffff

    def __init__(self, value):
        assert type(value) == long
        self._value = value

    def _ffs(self, x):
        """Returns the index, counting from 0, of the
        least significant set bit in `x`.
        """
        return (x&-x).bit_length()-1

    @property
    def vol_id(self):
        return self.value(self.volume_mask)

    @property
    def bnd_id(self):
        return self.value(self.boundary_mask)

    @property
    def lay_id(self):
        return self.value(self.layer_mask)

    @property
    def app_id(self):
        return self.value(self.approach_mask)

    @property
    def sen_id(self):
        return self.value(self.sensitive_mask)

    @property
    def chn_id(self):
        return self.value(self.channel_mask)

    def value(self, mask = None):
        if mask == None: return self._value
        return (self._value & mask) >> self._ffs(mask)

    def __repr__(self):
        fmt = "GeometryIdentifier(vol={}, bnd={}, lay={}, app={}, sen={}, chn={})"
        return fmt.format(self.vol_id, 
                          self.bnd_id, 
                          self.lay_id, 
                          self.app_id, 
                          self.sen_id, 
                          self.chn_id)

# infile = ROOT.TFile.Open(file)

# tree = infile.Get("MaterialTracks")

# for idx, event in enumerate(tree):
    # for geo_id_ in event.step_geo_id:
        # geo_id = GeometryIdentifier(geo_id_)
        # print(geo_id)

    # print("----")


