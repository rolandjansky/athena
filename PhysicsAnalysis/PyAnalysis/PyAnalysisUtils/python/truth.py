# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: truth.py,v 1.4 2007-08-02 22:21:56 ssnyder Exp $
# File: truth.py
# Created: sss, March 2005.
# Purpose: Dump out MC truth information.
#

"""Dump out MC truth information.

This module contains the function dump_truth which will dump out the
truth information for the current event in a tabular format.
"""

from PyAnalysisUtils import PDG
from PyParticleTools import PyParticleTools
import sys

def do_decay (t, n):
    """Return the barcode number for decay N of truth particle T
as a string.  Return an empty string if there's no such decay.
"""
    if t.nDecay() <= n: return ''
    bc = t.child(n).genParticle().barcode()
    return `bc`

class _Truthtmp:
    pass
def dump_one_truth (t, f=sys.stdout):
    """Dump truth information for truth particle T to file F."""
    d = _Truthtmp()
    d.bc = t.genParticle().barcode()
    d.name = PDG.pdgid_to_name(t.pdgId())
    d.da1 = do_decay(t, 0)
    d.da2 = do_decay(t, 1)
    d.da3 = do_decay(t, 2)
    d.pt = t.pt()/1000
    d.eta = t.eta()
    d.phi = t.phi()
    d.m = t.m()/1000
    d.px = t.px()/1000
    d.py = t.py()/1000
    d.pz = t.pz()/1000
    d.e = t.e()/1000
    print >> f, "%(bc)3d %(name)-4s %(da1)4s %(da2)4s %(da3)4s %(pt)6.1f %(eta)5.2f %(phi)5.2f %(m)5.1f %(px)6.1f %(py)6.1f %(pz)6.1f %(e)6.1f" % d.__dict__
    if t.nDecay() > 3: print>> f, "  (more than 3 decays)"


def dump_truth(f=sys.stdout, sgkey="SpclMC", maxn=None):
    """Dump out truth information for the current event.
F is the file to which the dump is sent.
SGKEY is the StoreGate key which is used to retrieve the truth information.

The first column in the dump is the particle barcode number.
This is followed by the particle type, and then by the barcode numbers
of any decay daughters (up to 3).  This is followed by the four-momentum
in two versions: first as pt, eta, phi, m and then as px, py, pz, e.
"""
    l = [p for p in PyParticleTools.getTruthParticles (sgkey)]
    if maxn != None:
        l = l[:maxn]
    for t in l:
        dump_one_truth (t, f)
    return
