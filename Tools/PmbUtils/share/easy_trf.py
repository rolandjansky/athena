#!/usr/bin/env python
import PmbUtils.AMIUtils
import os
import sys

def system_call(cmd):
    """Like os.system, but returns exit code in 0..255 rather than exit code and signal number packed in one int. Also returns original exit code and signal number""" 
    #assumes unix (of course)
    ret=os.system(cmd)
    minret,maxret=0,255
    if ret: minret=1#always return non-zero code if ret not 0
    #NB: Ignoring core file production info:
    subprocess_exit_code = (ret >> 8) & 0x7f
    subprocess_signal_number = ret & 0xff
    ec=subprocess_exit_code if subprocess_signal_number==0 else 128+subprocess_signal_number
    return (max(minret,min(maxret,ec)),subprocess_exit_code,subprocess_signal_number)

def __badusage():
    bn=os.path.basename(sys.argv[0])
    print
    print "Usage:"
    print "  %s [run|print] <amitag> <inputfile> [<maxevents>] [--extra=...] [--extrapreexec] [--extrapostexec]"%bn
    print "Or:"
    print "  %s diff <amitag1> <amitag2>"%bn
    print "Or:"
    print "  %s cache <amitag1> <outfile>"%bn
    print
    print "Supply any amount of --extra=... flags to add extra flags."
    print
    print "Supply any amount of --extrapreexec/--extrapostexec to append lines to preExec/postExec."
    print
    print "Example 1, finding out what ami tag f361 would do with a given input file from JetTauEtmiss:\n"
    print "  %s print f361 data11_7TeV.00178044.physics_JetTauEtmiss.merge.RAW._lb0092._SFO-11._0001.1"%bn
    print
    print "Example 2, run f361 on an input file from JetTauEtmiss for max 100 events (file must exist):\n"
    print "  %s run f361 data11_7TeV.00178044.physics_JetTauEtmiss.merge.RAW._lb0092._SFO-11._0001.1 100"%bn
    print
    print "Example 3, figure out the difference between f323 and x103:\n"
    print "  %s diff f323 x103"%bn
    print
    print "Example 4, query ami and save the output in a pickle file:\n"
    print "  %s cache f323 myinfo_f323.pkl"%bn
    #Hidden feature: Append :pmonsd or :pmondet to the tag to get
    #extra pmb features like env-vars and perfmon semi-det mode.
    sys.exit(1)

if len(sys.argv)==4 and sys.argv[1]=='diff':
    PmbUtils.AMIUtils.print_diff(sys.argv[2],sys.argv[3])
    sys.exit(0)

if len(sys.argv)==4 and sys.argv[1]=='cache':
    PmbUtils.AMIUtils.save_amiinfo_to_pickle(sys.argv[2],sys.argv[3])
    sys.exit(0)

#run/print option has hidden amicache: feature and less hidden --extra feature
amicache=None
extras=[]
extrapreexecs=[]
extrapostexecs=[]
for a in sys.argv[1:]:
    if a.startswith('--extrapreexec='):
        extrapreexecs+=[a[15:]]
        sys.argv.remove(a)
    elif a.startswith('--extrapostexec='):
        extrapostexecs+=[a[16:]]
        sys.argv.remove(a)
    elif a.startswith('--extra='):
        extras+=[a[8:]]
        sys.argv.remove(a)
    elif a.startswith('amicache:'):
        amicache=a[9:]
        sys.argv.remove(a)

if not len(sys.argv) in [4,5] or not sys.argv[1] in ['run','print']:
    __badusage()
do_run=sys.argv[1]=='run'
tag=sys.argv[2]
infile=sys.argv[3]
max_evts = int(sys.argv[4]) if len(sys.argv)==5 else None
assert max_evts==None or max_evts>=-1

ignore=['--uploadtoami=1.0']#flags not appropriate for non-production tests

if do_run:
    assert os.path.exists(infile),"ERROR: Input file '%s' does not exists"%infile
cmds=PmbUtils.AMIUtils.get_info(tag,infile,max_evts,ami_cachefile=amicache,
                                extra_preExec=extrapreexecs,
                                extra_postExec=extrapostexecs)
if not cmds:
    print "ERROR: Unable to retrieve info."
    sys.exit(1)
cmds+=extras
if not do_run:
    for i in range(len(cmds)):
        if cmds[i] in ignore:
            cmds[i]+=' #<ignored by easy_trf.py run>'
    print '\n'.join(cmds)
else:
    for cmd in cmds[:]:
        if cmd in ignore and cmd in cmds: cmds.remove(cmd)
    this_script=os.path.basename(sys.argv[0])
    print "%s Launching command: %s"%(this_script,' '.join(cmds))
    sys.stdout.flush()
    ec,proc_ec,proc_signal_number=system_call(' '.join(cmds))
    print "%s Command exited with signal number: %i"%(this_script,proc_signal_number)
    print "%s Command exited with exit code: %i"%(this_script,proc_ec)
    print "%s Now exiting with code: %i"%(this_script,ec)
    sys.exit(ec)
