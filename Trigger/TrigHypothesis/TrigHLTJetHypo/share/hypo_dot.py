# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import os
import re
from collections import defaultdict
from hypo_dot_tree import get_node_lines

indir = '/tmp/peter'
outdir = '/tmp/peter'

def dot_event(fn, lines):
    dot_0 = ['digraph G{']
    dot_1 = []
    #print ('pritn lines')
    #for l in lines: print (l)
    #assert False
    for l in lines:
        tokens = l.split()
        scenario = tokens[0].split('.')[-1]
        idn = tokens[2]
        pid = tokens[4]
        pass_flags = {'false': False,
                      'true': True}

        pass_flag =  pass_flags[tokens[6]]
        proctime =  tokens[8]
        ncalls = tokens[10]

        re_units = re.compile(r'^time\((?P<units>[a-z]+)\):$')
        m = re_units.match(tokens[7])
        units = m.groupdict()['units']

        label = '"%s %s%s %s (%s) %s call(s)"' % (scenario, '\\', 'n',
                                                  proctime, units, ncalls)

        color='red'
        if pass_flag:
            color = 'green'
        if idn != '0':
            dot_0.append('%s -> %s;' % (pid, idn))
        dot_1.append('%s [label=%s color=%s];' % (idn, label, color))
    dot_0.extend(dot_1)
    dot_0.append('}')
    script = '\n'.join(dot_0)
    return script


def get_event_fns():
    fn_re = re.compile(
        r'^jetHypo_TrigJetHypoAlgMT.HLT_')

    
    fns = os.listdir(indir)
    print (fns)
    fns = [f for f in fns if fn_re.match(f)]
    print (len(fns), ' files were identified')

    print (fns)
    fns = [os.path.join(indir, fn) for fn in fns]
    return fns

def dot_event_info(lines):

    
    i_list = []
    
    for l in lines:
        tokens = l.split()
        scenario = tokens[0].split('.')[-1]
        idn = tokens[2]
        pid = tokens[4]
        status = tokens[6]  # pass
        units = tokens[7]
        tav = tokens[-1]
        i_list.append((scenario, idn, pid, status, units, tav))

    sorter = defaultdict(list)
    for i in i_list:
        sorter[i[0]].append(i)

    info_list = []
    for k, ll in sorter.items():
        print (k)
        print (ll)
        scenario_set = set([l[0] for l in ll])
        idn_set = set([l[1] for l in ll])
        pid_set = set([l[2] for l in ll])
        pass_set = set([l[3] for l in ll])
        units_set = set([l[4] for l in ll])
        tot_time = str(sum([float(l[5]) for l in ll]))
        n_calls = str(len(ll))

        assert len(scenario_set) == 1
        assert len(idn_set) == 1
        assert len(pid_set) == 1
        assert len(pass_set) < 3
        assert len(units_set) == 1

        scenario = scenario_set.pop()
        pid = pid_set.pop()
        idn = idn_set.pop()
        if 'true' in pass_set:
            pass_str = 'pass'
        else:
            pass_str = 'fail'

        info_list.append([k, pass_str, tot_time, units, idn, pid, n_calls])
    return info_list


def make_dotevent_script(event_info):
    dot_0 = ['digraph G{']
    dot_1 = []

    print (event_info)
    for scenario, pass_str, tot_time, units, idn, pid, n_calls in event_info:

        label = '"%s %s%s %s (%s) %s call(s)"' % (scenario, '\\', 'n',
                                                  tot_time, units, n_calls)

        color='red'
        if pass_str == 'pass':
            color='green'
        if idn != '0':
            dot_0.append('%s -> %s;' % (pid, idn))
        dot_1.append('%s [label=%s color=%s];' % (idn, label, color))
    dot_0.extend(dot_1)
    dot_0.append('}')
    script = '\n'.join(dot_0)
    return script

    
def make_eventout_fn(fn):
    ofn = fn.split('/')[-1] + '.dot'
    return  os.path.join(outdir, ofn)

    
def write_eventdotscript(fn):
    node_lines = get_node_lines(fn)
    info_list = dot_event_info(node_lines)
    script = make_dotevent_script(info_list)

    out_fn = make_eventout_fn(fn)
    with open(out_fn, 'w') as ofile:
        ofile.write(script)
    print ('wrote ', out_fn)
    
def main():
    fns =  get_event_fns()
    [write_eventdotscript(fn) for fn in fns]
        
if __name__ == '__main__':
    main()
