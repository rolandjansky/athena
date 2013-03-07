#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from itertools import repeat

HIERARCHY = (
    ("Inner_Detector",(
        ("Pixel",    ("PIXB", "PIX0", "PIXEA", "PIXEC")),
        ("SCT",      ("SCTB", "SCTEA", "SCTEC")),
        ("TRT",      ("TRTB", "TRTEA", "TRTEC")),
        ("IDGlobal", ("IDGL", "IDAL", "IDBS", "IDPF", "IDBCM")),
    )),
    ("Calorimeters",(
        ("LAr",  ("EMBA", "EMBC", "EMECA", "EMECC", "HECA", 
                  "HECC", "FCALA", "FCALC")),
        ("Tile", ("TIGB", "TILBA", "TILBC", "TIEBA", "TIEBC")),
        ("MBTS", ("MBTSA", "MBTSC")),
        ("CaloGlobal", ("CALBA", "CALEA", "CALEC")),
    )),
    ("Muons",(
        ("MDT", ("MDTBA", "MDTBC", "MDTEA", "MDTEC")),
        ("RPC", ("RPCBA", "RPCBC")),
        ("TGC", ("TGCEA", "TGCEC")),
        ("CSC", ("CSCEA", "CSCEC")),
    )),
    ("Luminosity",(
        ("LUCID", ("LCD", "LCDA", "LCDC")),
        ("Other", ("ALFA", "ZDC", "LUMI")),
        ("RC",    ("RUNCLT", "RCOPS")),
        ("GL",    ("ATLGL",))
    )),
    ("Trigger",(
        ("L1",  ("L1CAL", "L1MUB", "L1MUE", "L1CTP")),
        ("HLT", ("TRCAL", "TRBJT", "TRBPH", "TRCOS", "TRGAM", 
                 "TRMET", "TRMBI", "TRMUO", "TRTAU", "TRIDT")),
    )),        
    ("Physics_Objects",(
        ("egammaID", ("EIDCR", "EIDE", "PIDB", "PIDCR", "PIDE",
                      "EIDF", "EIDSOFT")),
        ("muonID", ("MSTACO", "MMUIDCB", "MMUIDVX", "MMUGIRL",
                    "MMUBOY", "MMUIDSA", "MMUTAG", "MMTIMO",
                    "MCMUTAG", "MCALLHR")),
        ("Jets",  ("JETB", "JETEA", "JETEC")),
        ("MET",   ("MET", "METCALO", "METMUON")),
        ("b_tag", ("BTGB", "BTGEA", "BTGEC")),
        ("tauID", ("TAUE", "TAUB", "TAUCR")),
    )),
)

class DetectorHierarchy(object):
    def __init__(self, hierarchy, depth=0, parent=None):
        """
        Create a DetectorHierarchy object
        """
        
        self.depth = depth
        self.parent = parent
        
        if isinstance(hierarchy, str):
            self.name, sub_hierarchy = hierarchy, []
        else:
            self.name, sub_hierarchy = hierarchy
        
        self.sub_hierarchy = [DetectorHierarchy(sub_part, depth+1, self)
                              for sub_part in sub_hierarchy]
    
        if self.sub_hierarchy:
            self.max_sub_width = max(len(x.name) for x in self.sub_hierarchy)
        else:
            self.max_sub_width = 7
    
    def get_channels(self, name, selected=False, parts=()):
        """
        Accumulate the lowest parts of the hierarchy which contain "name"
        """
        if self.name == name:
            selected = True
        
        if not self.sub_hierarchy:
            return [(self.name, parts)] if selected else []
        
        result = []
        for sub_part in self.sub_hierarchy:
            breadcrumbs = parts + (self.name,)
            result.extend(sub_part.get_channels(name, selected, breadcrumbs))
        return result
    
    def __repr__(self):
        """
        String representation of the hierarchy
        """
        sub_part = ()
        if self.sub_hierarchy:
            jstr = "\n"
            indent = "\n"
            if self.depth == 2:
                jstr = " "
                indent = " : "
            parlen = len(self.parent.name) if self.parent else 0
            sub_part = indent, jstr.join(repr(x) for x in self.sub_hierarchy)
        
        width = self.parent.max_sub_width if self.parent else len(self.name)
        indent = " "*self.depth if self.depth < 3 else ""
        name = self.name.ljust(width)
        
        return "".join((indent, name) + sub_part)
    
detector_hierarchy = DetectorHierarchy(("All", HIERARCHY))

def test():
    print "Detector hierarchy:"
    print detector_hierarchy
    print
    
    accumulated = detector_hierarchy.get_channels("Inner_Detector")
    
    from pprint import pprint
    pprint(accumulated)
    
    from itertools import groupby
    pprint(list(groupby(accumulated, key=lambda x: x[1])))

if __name__ == "__main__":
    test()
