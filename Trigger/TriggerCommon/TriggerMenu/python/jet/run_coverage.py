# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import subprocess
import os
import sys

try:
    mod_name = sys.argv[1]
except IndexError:
    mod_name = 'run_testsuite.py'


cmd = 'coverage run %s' % mod_name

print cmd + ' ...'
subprocess.check_call(cmd, shell=True)

cmd = 'coverage report -m'
print cmd + ' ...'
cov_out = subprocess.check_output(cmd, shell=True)


cwd = os.getcwd()
ldir = os.listdir(cwd)
ltokens = []
for l in cov_out.strip().split('\n'):
    tokens =  l.split()
    if tokens[0].endswith('_test'):
        continue
    try:
        fn = tokens[0] + '.py'
    except IndexError:
        continue
    else:
        if fn in ldir:
            # 'coverage report -m'' gives
            # # module, n lines, n missing, %missing [missing line list]' e.g:
            # ChainConfi 18      9    50%   7-15, 38-41

            assert len(tokens) >= 4
            if len(tokens) > 4:
                linenos = ' '.join(tokens[4:])
            else:
                linenos = ''
            tokens = tokens[0:4]
            tokens.append(linenos)
            ltokens.append(tokens)

w = [max(len(t[i]) for t in ltokens) for i in range(4)]

for t in ltokens:
    print '%s %s %s %s %s' % (t[0].ljust(w[0]),
                              t[1].ljust(w[1]),
                              t[2].ljust(w[2]),
                              t[3].ljust(w[3]),
                              t[4])
            
    # else:
    #    print tokens[0]
    # if tokens[0] in ldir:
    #    print l
    
