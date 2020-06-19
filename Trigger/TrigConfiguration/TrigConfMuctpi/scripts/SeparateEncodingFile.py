#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import sys, argparse
from TrigConfMuctpi.XMLReader import MioctGeometryXMLReader

def readXML(filename):
    return MioctGeometryXMLReader(filename)


def mioctAsXML(mioct, depth, filter, stats):
    attr = ["id", "slot"]
    s = ' '*depth + "<%s %s>\n" % (mioct.tag, " ".join(['%s="%s"' % (a, mioct[a]) for a in attr]) )
    s += ' '*depth + '    <!-- contains sectors __SECTORS__ -->\n'
    sectorList = []
    for sector in mioct.Sectors:
        if not filter(sector):
            continue
        sectorList += [sector['name']]
        s += sectorAsXML(sector, depth + 4, stats )
    s += ' '*depth + "</%s>\n" % mioct.tag
    s = s.replace('__SECTORS__', ", ".join(sorted(sectorList)) )
    stats['sectors'] += len(sectorList)
    return s


def sectorAsXML(sector, depth, stats):
    attr = ["connector", "name"]
    s = ' '*depth + "<%s %s>\n" % (sector.tag, " ".join(['%s="%s"' % (a, sector[a]) for a in attr]) )
    s += ' '*depth + '    <!-- contains %i ROIs -->\n' % len(sector.ROIs)
    s += ' '*depth + '    <!-- mapping from ROI to coding scheme -->\n'
    for roi in sorted(sector.ROIs, key=lambda roi: int(roi['roiid'])):
        s += roiAsXML(roi, depth + 4 )
    s += ' '*depth + "</%s>\n" % sector.tag
    stats['rois'] += len(sector.ROIs)
    return s

def roiAsXML(roi, depth):
    attr = ["eta", "phi", "etacode", "phicode", "etamin", "etamax", "phimin", "phimax", "roiid"]
    s = ' ' * depth + "<ROI %s/>\n" % (" ".join(['%s="%s"' % (a, roi[a]) for a in attr]) )
    return s


def writeXML(geometry, dettype):

    if dettype != "RPC" and dettype != "TGC":
        return

    stats = {'miocts' : 0, 'sectors' : 0, 'rois' : 0}

    infile = geometry.getFileName()
    outfile = infile.replace(".","_%s." % dettype)

    if dettype == 'RPC':
        sectorFilter = lambda s : s['name'].startswith('B')  # noqa: E731
    else:
        sectorFilter = lambda s : s['name'].startswith('E') or s['name'].startswith('F')  # noqa: E731

    f = open(outfile,"write")

    print('<?xml version="1.0" ?>\n', file=f)
    print('<!DOCTYPE MuCTPiGeometry SYSTEM "MUCTPIGeometry.dtd">\n', file=f)
    print(geometry.MuCTPiGeometry, file=f)
    for mioct in geometry.getMIOCTs():
        print(mioctAsXML(mioct, 4, sectorFilter, stats), file=f)

    print("</%s>" % geometry.MuCTPiGeometry.tag, file=f)
    stats['miocts'] = len(geometry.getMIOCTs())
    print("Wrote %s" % outfile)

    print("Numbers for %s" % dettype)
    print("#MIOCTs  : %i" % stats['miocts'])
    print("#Sectors : %i" % stats['sectors'])
    print("#ROIs    : %i" % stats['rois'])

    f.close()



def main(args):

    print("Using input %s" % args.infile)
    geometry = readXML( args.infile )

    writeXML(geometry, "RPC")
    writeXML(geometry, "TGC")



if __name__=="__main__":


    parser = argparse.ArgumentParser( description=__doc__, 
                                      formatter_class = argparse.RawTextHelpFormatter)

    parser.add_argument('-i', dest='infile', default="TrigConfMuctpi/TestMioctGeometry.xml", type=str,
                        help='name of input combined muon geometry file')

    args = parser.parse_args()

    #try:
    sys.exit( main(args) )
    #except Exception, ex:
    #    print "exception caught %r" % ex
    #    sys.exit(1)
