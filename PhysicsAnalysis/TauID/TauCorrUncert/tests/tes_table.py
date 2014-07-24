# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from load import SHARE_DIR, TauCorrUncert as TCU
import os

pts = [17, 35, 75, 125, 175]
etas = [0, 0.5, 1.0, 1.5, 1.8]

for wp in ('medium', 'tight'):
    print
    tes = TCU.TESUncertainty(
            os.path.join(SHARE_DIR, 'TES', 'mc12_p1344_%s.root' % wp))
    print "%s working point" % wp
    for prong in (1, 3):
        print
        print "%d-prong" % prong
        for pt in pts:
            print "%d GeV" % pt, ' '.join([
                "%.2f" % (tes.GetTESUncertainty(pt * 1E3, eta, prong) * 100)
                for eta in etas])
