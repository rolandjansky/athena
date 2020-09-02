#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys
import argparse
from itertools import groupby
from operator import attrgetter
from TrigConfMuctpi.XMLReader import MioctGeometryXMLReader
from math import pi as PI
from TrigConfMuctpi.Utils import getLogger
from copy import copy

# since this reads the geometry data from the .dat file and the 2015 geometry xml
# it is best to define a new (small) class hierarchy to hold this data
class ROI(object):
    def __init__( self, eta=0, phi=0, phimin=0, phimax=0, etamin=0, etamax=0,
                  etacode=0, phicode=0, roiid=0 ):
        self.eta = eta
        self.phi = phi
        self.phimin = phimin
        self.phimax = phimax
        self.etamin = etamin
        self.etamax = etamax
        self.etacode = etacode
        self.phicode = phicode
        self.roiid = roiid

    @classmethod
    def fromROIelement(cls,roi):
        newroi = cls( eta=float(roi['eta']), phi=float(roi['phi']), phimin=float(roi['phimin']), phimax=float(roi['phimax']), etamin=float(roi['etamin']), etamax=float(roi['etamax']),
                      etacode=int(roi['etacode'],16), phicode=int(roi['phicode'],16), roiid=int(roi['roiid']))
        return newroi
    def asXML(self,depth):
        s = ' ' * depth + '<ROI eta="%f" phi="%f" etacode="0x%s" phicode="0x%s" etamin="%f" etamax="%f" phimin="%f" phimax="%f" roiid="%i"/>\n' % (self.eta, self.phi, self.etacode, self.phicode, self.etamin, self.etamax, self.phimin, self.phimax, self.roiid )
        return s
    def getStats(self,stats):
        pass


class Sector(object):
    def __init__(self, name, connector, rois=[]):
        self.name = name
        self.connector = connector
        self.id = int(name.lstrip('BECFA'))
        self.rois = copy(rois)
    def addROI(self,roi):
        self.rois += [ roi ]
    def isBarrel(self):
        return self.name.startswith('B')
    def getStats(self,stats):
        stats['rois'] += len(self.rois)
        for roi in self.rois:
            roi.getStats(stats)
    def asXML(self, depth):
        s  = ' '*depth + '<Sector connector="%s" name="%s">\n' % (self.connector, self.name)
        s += ' '*depth + '    <!-- contains %i ROIs -->\n' % len(self.rois)
        s += ' '*depth + '    <!-- mapping from ROI to coding scheme -->\n'
        for roi in sorted(self.rois, key=lambda roi: int(roi.roiid)):
            s += roi.asXML(depth + 4 )
        s += ' '*depth + "</Sector>\n"
        return s


class TopoCell(object):
    def __init__(self, etacode, phicode, eta, phi, ieta, iphi, etamin, etamax , phimin, phimax):
        self.etacode = etacode
        self.phicode = phicode
        self.eta     = eta    
        self.phi     = phi    
        self.ieta    = ieta   
        self.iphi    = iphi   
        self.etamin  = etamin 
        self.etamax  = etamax 
        self.phimin  = phimin 
        self.phimax  = phimax 

    @classmethod
    def fromTopoCellElement(cls,tc):
        newtc = cls( etacode=int(tc['etacode'],16), phicode=int(tc['phicode'],16),
                     eta=float(tc['eta']), phi=float(tc['phi']),
                     ieta=float(tc['ieta']), iphi=float(tc['iphi']),
                     etamin=float(tc['etamin']), etamax=float(tc['etamax']), phimin=float(tc['phimin']), phimax=float(tc['phimax']) )
        return newtc
    def asXML(self, depth):
        return ' '*depth + '<TopoCell etacode="0x%s" phicode="0x%s" eta="%f" phi="%f" ieta="%i" iphi="%i" etamin="%f" etamax="%f" phimin="%f" phimax="%f"/>\n' % (self.etacode, self.phicode,
                                                                                                                                                                  self.eta, self.phi,
                                                                                                                                                                  self.ieta, self.iphi,
                                                                                                                                                                  self.etamin, self.etamax, self.phimin, self.phimax)


class Decodes(object):
    def __init__(self, topocells=[]):
        self.topocells = copy(topocells)

    @classmethod
    def fromAllRois(cls,allrois):
        newDecodes = cls()
        for (etacode,phicode), roisInCell in groupby( sorted(allrois, key=attrgetter('etacode','phicode')), key=attrgetter('etacode','phicode')):
            # make a new cell
            (eta, phi, ieta, iphi, etamin, etamax, phimin, phimax) = cls.getTopoCellPosition(etacode, phicode, roisInCell)
            newDecodes.addTopoCell( TopoCell(etacode=etacode, phicode=phicode, eta=eta, phi=phi, ieta=ieta, iphi=iphi, etamin=etamin, etamax=etamax, phimin=phimin, phimax=phimax) )
        return newDecodes

    @classmethod
    def getTopoCellPosition(cls, etacode, phicode, rois):
        rois = list(rois)

        # ETA

        # eta min and max are different in the RPC (barrel) and TGC (endcap and forward)
        # in the RPC: abs(etamin)<abs(etamax)
        # in the TGC: etamin<etamax
        # we are going to use the TGC definition: etamin<etamax

        cellsEta = [(min(e.etamin,e.etamax), max(e.etamin,e.etamax)) for e in rois]
        etamins, etamaxs = zip(*cellsEta)  # transpose
        etamin = min( [ min(e.etamin,e.etamax) for e in rois] )
        etamax = max( [ max(e.etamin,e.etamax) for e in rois] )
        eta = (etamin+etamax)/2


        # PHI CONVENTION
        cellsPhi = [(e.phimin, e.phimax) for e in rois]

        # check if there are cells on both sides of the PI boundary
        upperedge = any([e.phi>6.2 for e in rois])
        loweredge = any([e.phi<0.2 for e in rois])
        splitTopoCell = upperedge and loweredge


        if splitTopoCell:
            maxAtLowerEdge = max([e.phimax for e in rois if e.phi<1])
            minAtUpperEdge = min([e.phimin for e in rois if e.phi>5])
            centerTopoCell = minAtUpperEdge + maxAtLowerEdge
            if centerTopoCell>=2*PI: # shift down
                phimin = minAtUpperEdge - 2 * PI
                phimax = maxAtLowerEdge
            else: # shift up
                phimin = minAtUpperEdge
                phimax = maxAtLowerEdge + 2 * PI
            phi = (phimin+phimax)/2
        else:

            phimins, phimaxs = zip(*cellsPhi)  # transpose
            phimin = min(phimins)
            phimax = max(phimaxs)
            phi = (phimin+phimax)/2


        # IETA
        ieta = round(eta*10)
        if ieta== 12: ieta= 11
        if ieta==-12: ieta=-11
        if ieta== 9: ieta= 8
        if ieta==-9: ieta=-8

        # IPHI
        iphi = int(phi*10)
        if abs(ieta) in [2,5]:
            if phi>2.05 and phi<2.35: iphi += 1
            if phi>2.75 and phi<3.25: iphi += 1
            if phi>3.45: iphi += 1

        if abs(ieta) == 8:
            if phi>2.05 and phi<2.35: iphi += 1
            if phi>2.75 and phi<3.25: iphi += 1
            if phi>3.45 and phi<4.95: iphi += 1
            if phi>5.05: iphi += 1
        
        if abs(ieta) in [15,18]:
            if phi>2.65: iphi += 1

        if abs(ieta) == 11:
            if phi>2.15 and phi<2.35: iphi += 1
            if phi>2.65 and phi<3.25: iphi += 1
            if phi>3.35: iphi += 1
        
        if abs(ieta) == 22:
            if phi>0.05 and phi<5.35: iphi += 1
            if phi>5.35: iphi += 2

        return (eta, phi, ieta, iphi, etamin, etamax, phimin, phimax)
        
    def addTopoCell(self,topocell):
        self.topocells += [ topocell ]
    def getStats(self,stats):
        stats['decodes'] += 1
        stats['topocells'] += len(self.topocells)
    def asXML(self, depth):
        s = ' '*depth + '<Decode>\n'
        for tc in self.topocells:
            s += tc.asXML(depth+4)
        s += ' '*depth + '</Decode>\n'
        return s


class Mioct(object):
    def __init__(self, id, slot, sectors=[]):
        self.id = id
        self.slot = slot
        self.sectors = copy(sectors)
        self.sectordic = dict([ (s.connector, s) for s in sectors ])
        self.decodes = None
    def getSector(self,connector):
        return self.sectordic.get(connector)
    def addSector(self,sector):
        if sector.connector in self.sectordic:
            raise RuntimeError("Sector with connector %i already exists" % sector.connector)
        self.sectors += [ sector ]
        self.sectordic[sector.connector] = sector
        return sector
    def getStats(self,stats):
        stats['sectors'] += len(self.sectors)
        for sector in self.sectors:
            sector.getStats(stats)
        if self.decodes:
            self.decodes.getStats(stats)
    def asXML(self, depth):
        sectorNames = [s.name for s in self.sectors]
        s  = ' '*depth + '<MIOCT id="%s" slot="%i">\n' % (self.id, self.slot)
        s += ' '*depth + '    <!-- contains sectors %s -->\n' % ", ".join(sorted(sectorNames))
        for sector in self.sectors:
            s += sector.asXML(depth + 4)
        if self.decodes:
            s += self.decodes.asXML(depth + 4)
        s += ' '*depth + "</MIOCT>"
        return s
    def fillTopoCells( self):
        # add all ROIs in this MIOCT
        allROIsInMioct = []
        for sector in self.sectors:
            allROIsInMioct += sector.rois
        self.decodes = Decodes.fromAllRois(allROIsInMioct)


class MuonGeometry(object):
    def __init__(self, name, miocts=[]):
        self.name = name
        self.miocts = copy(miocts)
        self.mioctdic = dict([ (m.id, m) for m in miocts])
    def getMioct(self,mioctId):
        return self.mioctdic.get(mioctId)
    def addMioct(self,mioct):
        if mioct.id in self.mioctdic:
            raise RuntimeError("MIOCT with ID %i already exists" % mioct.id)
        self.miocts += [ mioct ]
        self.mioctdic[mioct.id] = mioct
        return mioct
    def writeXML(self,outfilename):
        f = open(outfilename,"write")
        print('<?xml version="1.0" ?>\n', file=f)
        print('<!DOCTYPE MuCTPiGeometry SYSTEM "MUCTPIGeometry.dtd">\n', file=f)
        print('<MuCTPiGeometry>', file=f)
        for mioct in sorted(self.miocts,key=lambda m:m.id):
            print(mioct.asXML(4), file=f)
        print('    <PtEncoding>', file=f)
        print('        <PtCodeElement pt="1" code="0" value="4"/>', file=f)
        print('        <PtCodeElement pt="2" code="1" value="6"/>', file=f)
        print('        <PtCodeElement pt="3" code="2" value="10"/>', file=f)
        print('        <PtCodeElement pt="4" code="2" value="11"/>', file=f)
        print('        <PtCodeElement pt="5" code="2" value="15"/>', file=f)
        print('        <PtCodeElement pt="6" code="2" value="20"/>', file=f)
        print('    </PtEncoding>', file=f)
        print("</MuCTPiGeometry>", file=f)
        f.close()
        print("Wrote %s" % outfilename)
    def getStats(self,stats):
        stats['miocts'] = len(self.miocts)
        for mioct in self.miocts:
            mioct.getStats(stats)
    def printStats(self):
        stats = {'miocts' : 0, 'sectors' : 0, 'rois' : 0, 'decodes' : 0, 'topocells' : 0}
        self.getStats(stats)
        print("Numbers for %s" % self.name)
        print("#MIOCTs    : %i" % stats['miocts'])
        print("#Sectors   : %i" % stats['sectors'])
        print("#Decodes   : %i" % stats['decodes'])
        print("#ROIs      : %i" % stats['rois'])
        print("#Topocells : %i" % stats['topocells'])
        




def rpcMioctAndSectorInfo(sectorID):
    sectorName = "B%02i" % sectorID
    sectorConnector = (sectorID+2) % 4

    mioctID = ( (sectorID+2) % 32 ) / 4
    mioctSlot = mioctID + 4
    if sectorID >= 32:
        mioctID += 8
        mioctSlot = mioctID + 6

    return mioctID, mioctSlot, sectorConnector, sectorName



def read2015Geometry( fn ):
    xmlgeometry = MioctGeometryXMLReader(fn)
    miocts = []
    for MIOCT in xmlgeometry.getMIOCTs():
        sectors = []
        for sector in MIOCT.Sectors:
            #if sn.startswith('B'): continue
            rois = []
            for roiElem in sector.ROIs:
                rois += [ ROI.fromROIelement(roiElem) ]
            sectors += [ Sector(name=sector['name'], connector=int(sector['connector']), rois=rois) ]
        miocts += [ Mioct( id=int(MIOCT['id']), slot=int(MIOCT['slot']), sectors=sectors) ]
        decodes = Decodes()
        for tc in MIOCT.Decode.TopoCells:
            decodes.addTopoCell( TopoCell.fromTopoCellElement(tc) )
        miocts[-1].decodes=decodes
    return MuonGeometry("full geometry 2015", miocts=miocts)



def read2015RPCCodeMapping( geometry ):
    mapping = {}  
    for mioct in geometry.miocts:
        for sector in mioct.sectors:
            if sector.isBarrel():
                mapping[sector.id] = dict( [ (roi.roiid, (roi.etacode, roi.phicode)) for roi in sector.rois ] )
    return mapping




def feetRegionMapping(sectorId, roiId):
    """ provides the new mapping for the feet and elevator region
    The numbers can be found in doc/roi_map_R2.pdf
    """

    etacode = -1
    phicode = -1
    
    if sectorId in [23,24,55,56]:
        # elevator region

        # eta
        etaMap = { 23 : [ [0,1,2,3,5,7], [4,6,8,9,10,11], range(12,20) + [21,23], [20,22] + range(24,28) ],
                   24 : [ [0,1,2,3,4,6], [5,7,8,9,10,11], range(12,20) + [20,22], [21,23] + range(24,28) ] }
        etaMap[55] = etaMap[24]
        etaMap[56] = etaMap[23]
                
        for ec, tt in enumerate(etaMap[sectorId]):
            if roiId in tt:
                etacode = ec
                break

        # phi
        if sectorId in [23,55]:
            if roiId in [2,3,6,7,10,11,18,19,22,23,27]:
                phicode = 2
            if roiId in [0,1,4,5,8,9,12,13,14,15,16,17,20,21,25]:
                phicode = 3
        elif sectorId in [24,56]:
            if roiId in [0,1,4,5,8,9,12,13,14,15,16,17,20,21,24]:
                phicode = 4
            if roiId in [2,3,6,7,10,11,18,19,22,23,26]:
                phicode = 5

        if sectorId == 23:
            if roiId == 27: phicode = 2
            elif roiId == 25: phicode = 3
        elif sectorId == 24:
            if roiId == 24: phicode = 4
            elif roiId == 26: phicode = 5
        elif sectorId == 55:
            if roiId == 24: phicode = 3
            elif roiId == 26: phicode = 2
        elif sectorId == 56:
            if roiId == 27: phicode = 5
            elif roiId == 25: phicode = 4



    elif [21,22,25,26,53,54,57,58]:
        # feet region

        # eta
        etaMap = { 21 : [ range(0,8), range(8,16) + [17,19], [16,18] + range(20,32) ],
                   22 : [ range(0,8), range(8,16) + [16,18], [17,19] + range(20,32) ] }
        etaMap[25] = etaMap[21]
        etaMap[26] = etaMap[22]
        etaMap[53] = etaMap[22]
        etaMap[54] = etaMap[21]
        etaMap[57] = etaMap[53]
        etaMap[58] = etaMap[54]
                
        for ec, tt in enumerate(etaMap[sectorId]):
            if roiId in tt:
                etacode = ec
                break

        # phi
        if sectorId in [21,53]:
            if roiId in [2,3,6,7,10,11,14,15,18,19,22,23,26,27,30,31]:
                phicode = 6
            elif roiId in [0,1,4,5,8,9,12,13,16,17,20,21,24,25,28,29]:
                phicode = 7
        elif sectorId in [22,54]:
            if roiId in [0,1,4,5,8,9,12,13,16,17,20,21,24,25,28,29]:
                phicode = 0
            elif roiId in [2,3,6,7,10,11,14,15,18,19,22,23,26,27,30,31]:
                phicode = 1
        elif sectorId in [25,57]:
            if roiId in [2,3,6,7,10,11,14,15,18,19,22,23,26,27,30,31]:
                phicode = 6
            elif roiId in [0,1,4,5,8,9,12,13,16,17,20,21,24,25,28,29]:
                phicode = 7
        elif sectorId in [26,58]:
            if roiId in [0,1,4,5,8,9,12,13,16,17,20,21,24,25,28,29]:
                phicode = 0
            elif roiId in [2,3,6,7,10,11,14,15,18,19,22,23,26,27,30,31]:
                phicode = 1

    if etacode == -1:
        raise RuntimeError("No etacode for SL %i and ROI %i" % (sectorId, roiId))
    if phicode == -1:
        raise RuntimeError("No phicode for SL %i and ROI %i" % (sectorId, roiId))

    return (etacode,phicode)




def read2016RPCGeomData(fn, oldMapping):

    rpcGeometry2016 = MuonGeometry("RPC geometry 2016")
    log = getLogger(__file__+".read2016RPCGeomData")


    log.info("Reading %s", fn)
    f = open(fn,"read")
    for line in f:
        if line.lstrip().startswith("#"): # remove comments
            continue
        ls = line.split()
        side     = int(ls[0])
        sector   = int(ls[1]) 
        roiid    = int(ls[2]) 
        etamin   = float(ls[3])
        etamax   = float(ls[4])
        phimin   = float(ls[5])
        phimax   = float(ls[6])
        if max(phimin, phimax) < 0:
            phimin += 2 * PI
            phimax += 2 * PI
        eta = (etamin + etamax) / 2
        phi = (phimin + phimax) / 2

        sectorId = sector + 32 * side
        if sectorId in [21,22,23,24,25,26,53,54,55,56,57,58]:
            (etacode, phicode) = feetRegionMapping(sectorId, roiid)
        else:
            (etacode, phicode) = oldMapping[sectorId][roiid]

        roi = ROI( eta=eta, phi=phi, phimin=phimin, phimax=phimax, etamin=etamin, etamax=etamax, etacode=etacode, phicode=phicode, roiid=roiid )

        # now find MIOCT and Sector where to add the ROI to
        mioctId, mioctSlot, sectorConnector, sectorName = rpcMioctAndSectorInfo(sectorId)
        mioct = rpcGeometry2016.getMioct(mioctId)
        if not mioct:
            mioct = rpcGeometry2016.addMioct(Mioct(id=mioctId, slot=mioctSlot))

        sector = mioct.getSector(sectorConnector)
        if not sector:
            sector = mioct.addSector( Sector(name=sectorName, connector=sectorConnector) )
            
        sector.addROI( roi )

    return rpcGeometry2016


def updateGeometryTGC(newGeometry, oldGeometry):
    newGeometry.name = "updated full geometry for 2016"
    for mioct in newGeometry.miocts:
        oldMioct = oldGeometry.getMioct(mioct.id)
        for sector in oldMioct.sectors:
            if sector.isBarrel(): continue
            mioct.addSector(sector)
        mioct.fillTopoCells()



def sectorAsXML_2(sector, depth, stats):
    attr = ["connector", "name"]
    s = ' '*depth + "<%s %s>\n" % (sector.tag, " ".join(['%s="%s"' % (a, sector[a]) for a in attr]) )
    s += ' '*depth + '    <!-- contains %i ROIs -->\n' % len(sector.ROIs)
    s += ' '*depth + '    <!-- mapping from ROI to coding scheme -->\n'
    for roi in sorted(sector.ROIs, key=lambda roi: int(roi['roiid'])):
        s += roiAsXML_2(roi, depth + 4 )
    s += ' '*depth + "</%s>\n" % sector.tag
    stats['rois'] += len(sector.ROIs)
    return s

def roiAsXML_2(roi, depth):
    attr = ["eta", "phi", "etacode", "phicode", "etamin", "etamax", "phimin", "phimax", "roiid"]
    s = ' ' * depth + "<ROI %s/>\n" % (" ".join(['%s="%s"' % (a, roi[a]) for a in attr]) )
    return s


def sectorAsXML(xxx_todo_changeme, depth, stats):
    (connector, name, rois) = xxx_todo_changeme
    s = ' '*depth + '<Sector connector="%s" name="%s">\n' % (connector, name)
    s += ' '*depth + '    <!-- contains %i ROIs -->\n' % len(rois)
    s += ' '*depth + '    <!-- mapping from ROI to coding scheme -->\n'
    for roi in rois:
        s += roiAsXML(roi, depth + 4 )
    s += ' '*depth + "</Sector>\n"
    stats['rois'] += len(rois)
    return s

def roiAsXML(roi, depth):
    roi['eta'] = (float(roi['etamin']) + float(roi['etamax'])) / 2
    roi['phi'] =  (float(roi['phimin']) + float(roi['phimax'])) / 2
    s = ' ' * depth + '<ROI eta="%f" phi="%f" etacode="0x%s" phicode="0x%s" etamin="%f" etamax="%f" phimin="%f" phimax="%f" roiid="%i"/>\n' % (roi["eta"], roi["phi"], roi["etacode"], roi["phicode"], roi["etamin"], roi["etamax"], roi["phimin"], roi["phimax"], roi["roiid"] )
    return s









def main(args):

    # read 2015 geometry xml file
    muonGeometry2015 = read2015Geometry( args.infile2015 )

    muonGeometry2015.printStats()

    # maps roi number to etacode and phicode
    rpcCodeMapping = read2015RPCCodeMapping( muonGeometry2015 )


    muonGeometry2016 = read2016RPCGeomData( args.infile, rpcCodeMapping )
    muonGeometry2016.printStats()

    
    updateGeometryTGC(muonGeometry2016, muonGeometry2015)
    muonGeometry2016.printStats()

    # write out the RPC info and the TGC info
    muonGeometry2016.writeXML("TestMioctGeometry2016.xml")




if __name__=="__main__":

    parser = argparse.ArgumentParser( description=__doc__, 
                                      formatter_class = argparse.RawTextHelpFormatter)

    parser.add_argument('-i', dest='infile', default="../data/TestMioctGeometry2016.dat", type=str,
                        help='name of input RPC muon geometry file for 2016 [../data/TestMioctGeometry2016.dat]')

    parser.add_argument('-i1', dest='infile2015', default="../data/TestMioctGeometry.xml", type=str,
                        help='name of input RPC muon geometry file used in 2015 [../data/TestMioctGeometry.xml]')

    args = parser.parse_args()

    sys.exit( main(args) )
