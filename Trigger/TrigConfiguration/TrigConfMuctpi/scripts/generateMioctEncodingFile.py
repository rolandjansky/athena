#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,os
from itertools import groupby
from operator import attrgetter
from math import copysign

class Entry:
    """Represents a single input ROI (one line in the .dat file)
    """
    def __init__(self, subsystid, sectorid, roi, etamin, etamax, phimin, phimax, eta, phi, slot, connector, sectorname, mioct, sectoraddress, etacode, phicode):
        self.hemisphere   = "A" if subsystid=="0" else "C"
        self.sectorid     = int(sectorid)
        self.roi          = int(roi)
        self.etamin       = float(etamin)
        self.etamax       = float(etamax)
        self.phimin       = float(phimin)
        self.phimax       = float(phimax)
        self.eta          = float(eta)
        self.phi          = float(phi)
        self.slot         = int(slot)
        self.connector    = int(connector)
        self.sectorname   = sectorname
        self.mioct        = int(mioct)
        self.etacode      = etacode
        self.phicode      = phicode
        
    def __str__(self):
        return "mioct=%i (slot %i), sector=%i, roi=%i, eta=%f, phi=%f, conn=%i, %s" % (self.mioct, self.slot, self.sectorid, self.roi, self.eta, self.phi, self.connector, self.sectorname) 

    def __repr__(self):
        return self.__str__()

#def keyfunc(t):
#    return  (t.mioct, t.connector)

def writeXML(g):
    filename = os.path.basename(sys.argv[1]).replace('.dat','.xml')
    xml = open(filename,"w")
    print >>xml,'<?xml version="1.0" ?>'
    print >>xml,'<MuCTPiGeometry>'
    for mioct in sorted(g.keys()):

        allSlots = set()
        allSectorNames = set()
        for sector in g[mioct].values():
            allSlots.update( [e.slot for e in sector] )
            allSectorNames.update( [e.sectorname for e in sector] )
            if len(allSlots) != 1:
                raise RuntimeError("More than one slot defined for MIOCT %i" % mioct)
        slot = allSlots.pop()
        allSectorNames = sorted(list( allSectorNames ))
        
        print >>xml,'    <MIOCT id="%i" slot="%s">' % ( mioct, slot )
        print >>xml,'        <!-- contains sectors %s -->' % ", ".join(allSectorNames)
        for connector in sorted(g[mioct].keys()):
            sector = sorted(g[mioct][connector], key=attrgetter('eta','phi','etacode','phicode'))
            print >>xml,'        <Sector connector="%s" name="%s">' % (connector, ", ".join(set([str(e.sectorname) for e in sector])) )
            print >>xml,'            <!-- contains %i ROIs -->' % len(sector)
            print >>xml,'            <!-- mapping from ROI to coding scheme -->'
            for e in sector:
                print >>xml,'            <ROI eta="%f" phi="%f" etacode="%s" phicode="%s" etamin="%f" etamax="%f" phimin="%f" phimax="%f" roiid="%i"/>' % (e.eta, e.phi, e.etacode, e.phicode, e.etamin, e.etamax, e.phimin, e.phimax, e.roi)
            print >>xml,'        </Sector>'

        allROIsInMIOCT = []
        map(allROIsInMIOCT.extend, g[mioct].values()) # add all ROIs in this MIOCT

        print >>xml,'        <Decode>'

        for (eta_code,phi_code), rois in groupby(sorted(allROIsInMIOCT,key=attrgetter('etacode','phicode')), key=attrgetter('etacode','phicode')):

            # eta min and max are different in the RPC (barrel) and TGC (endcap and forward)
            # in the RPC: abs(etamin)<abs(etamax)
            # in the TGC: etamin<etamax
            # we are going to use the TGC definition: etamin<etamax

            cells = [(min(e.etamin,e.etamax), max(e.etamin,e.etamax), e.phimin, e.phimax) for e in rois]
            etamins,etamaxs,phimins,phimaxs = zip(*cells)
            etamin = min(etamins) # assumes that no sector crosses the hemisphere (so all eta + or -)
            etamax = max(etamaxs)
            phimin = min(phimins)
            phimax = max(phimaxs)
            print >>xml,'            <TopoCell etacode="%s" phicode="%s" eta="%f" phi="%f" etamin="%f" etamax="%f" phimin="%f" phimax="%f"/>' % (eta_code,phi_code, (etamin+etamax)/2, (phimin+phimax)/2, etamin, etamax, phimin, phimax)
        print >>xml,'        </Decode>'
        print >>xml,'    </MIOCT>'
    print >>xml,'</MuCTPiGeometry>'
    xml.close()
    print "Wrote %s" % filename

def main():
    if len(sys.argv)!=2:
        print "Usage: %s <geomety.dat>" % os.path.basename(sys.argv[0])
        return

    data = open(sys.argv[1])
    lines = [Entry(*l.rstrip().split()) for l in data.readlines() if not l.startswith('#')]
    lines = sorted(lines, key = attrgetter('mioct','connector') )
    data.close()
        
    groups = {}
    for k, g in groupby(lines, key=attrgetter('mioct')):
        groups[k] = {}
        for k2, g2 in groupby(g, key = attrgetter('connector')):
            groups[k][k2] = list(g2)

    writeXML(groups)

    return 0


if __name__=="__main__":
    try:
        sys.exit(main())
    except Exception, ex:
        import traceback
        traceback.print_exc()
        sys.exit(1)
