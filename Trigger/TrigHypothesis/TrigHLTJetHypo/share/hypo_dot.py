import os
import re
import subprocess



def dot_event(fn, lines):
    dot_0 = ['digraph G{']
    dot_1 = []
    #print 'pritn lines'
    #for l in lines: print l
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
        if pass_flag: color = 'green'
        if idn != '0':
            dot_0.append('%s -> %s;' % (pid, idn))
        dot_1.append('%s [label=%s color=%s];' % (idn, label, color))
    dot_0.extend(dot_1)
    dot_0.append('}')
    script = '\n'.join(dot_0)
    return script

        
def main():
    indir = '/tmp/peter'
    outdir = '/tmp/peter'
    fn_re = re.compile(
        r'^jetHypo_TrigJetHypoAlgMT.HLT_j0_vbenfSEP50etSEP34mass35SEP50fbet_\d+$')

    fns = os.listdir(indir)
    print fns
    fns = [f for f in fns if fn_re.match(f)]
    print len(fns), ' files were identified'

    print fns
    fns = [os.path.join(indir, fn) for fn in fns]
    for fn in fns:
        with open(fn) as ifile:
            pass_lines = [l[:-1] for l in ifile if 'node:' in l]
        filename = os.path.basename(fn)
        script = dot_event(filename, pass_lines)

        dfn = os.path.join(outdir, filename+'.dot')
        with open(dfn, 'w') as ofile:
            ofile.write(script)

        pdffn = os.path.join(outdir, filename+'.eps')
        p = subprocess.Popen(['dot', '-T', 'eps', dfn, '-o',  pdffn])
        p.communicate()
        
        print 'done with ', dfn
    print 'end of main'
if __name__ == '__main__':
    main()
