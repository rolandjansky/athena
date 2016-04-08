#!/usr/bin/env python
import sys
import os
import fnmatch
import pipes

#input:
assert len(sys.argv)>2
bn=os.path.basename(sys.argv[0]).replace('.py','')
selection=sys.argv[1]
cmd=sys.argv[2:]

#environment
branch=os.getenv('AtlasBaseDir').split('/')[-1]
cmtconfig=os.getenv('CMTCONFIG')
reln=os.getenv('AtlasVersion')
print '%s INFO Detected branch=%s cmtconfig=%s reln=%s'%(bn,branch,cmtconfig,reln)

#check selection
def matches(str,patterns):
    for p in patterns:
        if fnmatch.fnmatch(str,p):
            return True
        #if re.match(p,str):
        #    return True

sel_parts=selection.split(':')
assert len(sel_parts) in [2,3]
sel_branches=sel_parts[0].split(',')
sel_cmtconfig=sel_parts[1].split(',')
sel_reln=(sel_parts[2] if len(sel_parts)==3 else '*').split(',')
allOK=True
print '%s INFO Require branch match in: %s'%(bn,sel_branches)
if not matches(branch,sel_branches):
    allOK=False
    print '%s INFO   => %s does not match any of these patterns'%(bn,branch)
print '%s INFO Require cmtconfig match in: %s'%(bn,sel_cmtconfig)
if not matches(cmtconfig,sel_cmtconfig):
    allOK=False
    print '%s INFO   => %s does not match any of these patterns'%(bn,cmtconfig)
print '%s INFO Require rel_n match in: %s'%(bn,sel_reln)
if not matches(reln,sel_reln):
    allOK=False
    print '%s INFO   => %s does not match any of these patterns'%(bn,reln)

#run command or abort without error
if not allOK:
    print '%s INFO Some requirements not passed so I will not run the command (this is intentional)'%bn
else:
    cmdstr=' '.join([pipes.quote(c) for c in cmd])
    print '%s INFO Launching this exact command:  %s'%(bn,cmdstr)
    sys.stdout.flush()
    ec=os.system(cmdstr)
    sys.stdout.flush()
    if ec==0:
        print '%s INFO Command finished with exitcode=0'%bn
        sys.exit(0)
    else:
        print '%s ERROR Command finished with exitcode!=0'%bn
        sys.exit(1 if ec>127 else ec)
