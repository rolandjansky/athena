#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import xml.etree.cElementTree as ET

class ThrInfo:
    name = ''
    type = ''
    mapping = ''
    bitnum = 0
    begin = -1
    end = -1
    def __init__(self, name, type, mapping):
        self.name = name
        self.type = type
        self.mapping = mapping
    def __str__(self):
        return '%s (%s/%d)' % (self.name, self.type, self.mapping)
    def includesBit(self, x):
        if x >= self.begin and x <= self.end:
            return True
        else:
            return False

def toString(input):
    return '%s (%s/%s)' % (input[2], input[1], input[0])

def printMap(m, logic_string):
    nbits = 0
    used_thrs = []
    for v in m.values():
        if logic_string.find(v.name) >= 0:
            nbits += v.bitnum
            used_thrs.append(v.name)
    #
    keys = m.keys()
    ctpin_conns = map(lambda x: (x[0], x[1]), keys)
    ctpin_conns.sort()
    lines_for_input = {}
    for ctpin in ('SLOT7', 'SLOT8', 'SLOT9'):
        for conn in ('CON0', 'CON1', 'CON2', 'CON3'):
            if (ctpin, conn) not in ctpin_conns: continue
            name = '???'
            for x in keys:
                if (ctpin, conn) == (x[0], x[1]):
                    name = x[2]
                    break
            lines_for_input[(ctpin,conn)] = ['%s/%s %s' % (ctpin, conn, name)]
            # Get all thresholds in this slot/conn
            thrs = []
            for (k,v) in m.iteritems():
                if k[0] == ctpin and k[1] == conn:
                    thrs.append(v)
            for bit in range(31):
                thrdata = ''
                for thr in thrs:
                    if thr.includesBit(bit):
                        thrdata = str(thr)
                        if thr.name not in used_thrs:
                            thrdata = '---%s' % thrdata
                        break
                s = '  [%02d] %s' % (bit, thrdata)
                lines_for_input[(ctpin,conn)].append(s)
        for i in range(32):
            s = ''
            for conn in ('CON0', 'CON1', 'CON2', 'CON3'):
                if (ctpin, conn) not in ctpin_conns: continue
                if len(lines_for_input[(ctpin,conn)]) > i:
                    s += '%-30s' % lines_for_input[(ctpin,conn)][i]
            print s
        print ''
    print 'Total number of bits for this threshold list: %d' % nbits
##     keys.sort()
##     for k in keys:
##         print '%s -> %s' % (str(k), str(m[k]))
    pass

def usage():
    print 'Usage: %s <L1_XML>' % sys.argv[0]
    
def main():
    fname = 'LVL1config_MC_pp_v3_17.1.1.3.xml'
    if len(sys.argv)==2:
        fname = sys.argv[1]
    else:
        usage()
        exit(1)
    print 'Using XML file: %s' % fname
    doc = ET.parse(fname)
    root = doc.getroot()
    cable_map = {}
    # (ctpin, connector, name, begin) -> ThrInfo
    thrs = root.findall('TriggerThresholdList/TriggerThreshold')
    items = root.findall('TriggerMenu/TriggerItem')
    logic_string = ''
    for item in items:
        logic_string += item.get('definition')
    for thr in thrs:
        thrinfo = ThrInfo(thr.get('name'), thr.get('type'),
                          int(thr.get('mapping')))
        x = thr.find('Cable')
        input = None
        if x:
            thrinfo.bitnum = int(thr.get('bitnum'))
            signal = x.find('Signal')
            if signal != None:
                thrinfo.begin = int(signal.get('range_begin'))
                thrinfo.end = int(signal.get('range_end'))
                input = (x.get('ctpin'), x.get('connector'), x.get('name'),
                         int(signal.get('range_begin')))
                cable_map[input] = thrinfo
    printMap(cable_map, logic_string)
    
if __name__ == '__main__':
    main()
    
