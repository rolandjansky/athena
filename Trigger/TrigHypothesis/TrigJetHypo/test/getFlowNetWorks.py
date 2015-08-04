# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Extract FlowNetwork string representations from a log file, eg
an RTT log file."""

import re
import string
fn = 'rtt.log'


class FileNavigator(object):
    def __init__(self):
        self.lines = open(fn).readlines()
        self.cur_line = 0
        self.more = True

    def _move_to_start(self):
        for l in self.lines[self.cur_line:]:
            self.cur_line += 1
            if l.startswith('FlowNetwork:'):
                return
        raise StopIteration

    def get_flownetwork(self):
        self._move_to_start()
        if self.more:
            lines = []
            for l in self.lines[self.cur_line:]:
                l = l.strip()
                if not l:
                    return lines
                lines.append(l)
            

pat = re.compile('(?P<start>\d+)->(?P<end>\d+)')
def to_text(lines):
    
    tlines = []
    tlines.extend(lines[0].split())

    s = set([])
    for l in lines[1:]:
        [s.add(i) for i in  re.findall(pat, l)]

    tlines.extend(sorted(['%s %s 1.0' % e for e in s]))
    tlines.append('\n')
    return '\n'.join(tlines)
        
    
    
    

def get_all_fn():
    navigator = FileNavigator()
    #for i in range(3):
    n_fn = 0
    max_fn = 100
    while 1:
        try:
            # print '-'*20
            flow_lines = navigator.get_flownetwork()
            # print '\n'.join(flow_lines)
            with open('fn_%s' % string.zfill(n_fn, 4), 'w') as ofile:
                ofile.write(to_text(flow_lines))
            print to_text(flow_lines)
            n_fn += 1
            if n_fn == max_fn:
                print 'procecessed %d fns' % n_fn
                return
        except StopIteration:
            print 'processed  %d fns' % n_fn
            return

get_all_fn()
        
        
