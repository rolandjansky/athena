# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import os
import re

indir = '/tmp/peter'
outdir = '/tmp/peter'

def dot_tree_info(lines):
    info_list = []
    for l in lines:
        tokens = l.split()
        scenario = tokens[0].split('.')[-1]
        idn = tokens[2]
        pid = tokens[4]

        info_list.append((scenario, idn, pid))

    return info_list

def make_dot_script(tree_info):
    dot_0 = ['digraph G{']
    dot_1 = []

    for scenario, idn, pid in tree_info:

        label = '"%s"' % scenario

        color='red'
        if idn != '0':
            dot_0.append('%s -> %s;' % (pid, idn))
        dot_1.append('%s [label=%s color=%s];' % (idn, label, color))
    dot_0.extend(dot_1)
    dot_0.append('}')
    script = '\n'.join(dot_0)
    return script

def get_tree_fns():
    fn_re = re.compile(
        r'^HypoTree_TrigJetHypoAlgMT.HLT')

    fns = os.listdir(indir)
    print (fns)
    fns = [f for f in fns if fn_re.match(f)]
    print (len(fns), ' files were identified')

    print (fns)
    fns = [os.path.join(indir, fn) for fn in fns]
    return fns

def get_node_lines(fn):
    with open(fn) as ifile:
        node_lines = [l[:-1] for l in ifile if 'node:' in l]
    return node_lines


def make_out_fn(in_fn):
    tfn = in_fn.split('/')[-1]
    tokens = tfn.split('.')
    assert tokens[0] == 'HypoTree_TrigJetHypoAlgMT'
    out_fn = tokens[1]+'_tree.dot'
    return os.path.join(outdir, out_fn)
    
def write_dotscript(fn):
    node_lines = get_node_lines(fn)
    info_list = dot_tree_info(node_lines)
    script = make_dot_script(info_list)

    out_fn = make_out_fn(fn)
    with open(out_fn, 'w') as ofile:
        ofile.write(script)
    print ('wrote ', out_fn)
    

    
def main():
    fns = get_tree_fns()
    [write_dotscript(fn) for fn in fns]
    
    # for fn in fns:
    #    filename = os.path.basename(fn)

    #    script = dot_tree(filename, node_lines)

    #    pdffn = os.path.join(outdir, filename+'.eps')
    #    p = subprocess.Popen(['dot', '-T', 'eps', dfn, '-o',  pdffn])
    #    p.communicate()
        
    #    print ('done with ', dfn)
    #print ('end of main')
if __name__ == '__main__':
    main()
