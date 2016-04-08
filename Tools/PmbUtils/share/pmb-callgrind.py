#!/usr/bin/env python
import sys
import os
import pipes
import datetime
import time
import glob

def usage():
    print "Usage (to profile a command 'cmdname'):"
    print "%s [delay_instr] [outid:<customid>] cmdname <cmdname arguments>"%os.path.basename(sys.argv[0])
    print ""
    print 'Specifying delay_instr means valgrind will get the argument --instr-atstart=no (useful with Valkyrie'

def which(cmdname):
    import subprocess
    p=subprocess.Popen(['/usr/bin/which',cmdname], stdout=subprocess.PIPE).communicate()[0]
    return p.strip()

def bzip2(filename):
    import subprocess
    subprocess.Popen(['/usr/bin/bzip2',filename], stdout=subprocess.PIPE).communicate()[0]

def execute(user_cmd,outputid,delay_instrumentation,never_clean_empty=False,logfile=True,delete_on_error=True):
    assert user_cmd

    if logfile:
        logfile=open('pmb-callgrind.%s.log'%outputid,'w')
    def _log(s):
        s='pmb-callgrind INFO : %s : %s\n'%(datetime.datetime.utcnow(),s)
        print s,
        sys.stdout.flush()
        if logfile:
            logfile.write(s)
            logfile.flush()

    cmd=[which('valgrind' )]+' --tool=callgrind --trace-children=yes --num-callers=8 --collect-jumps=yes'.split()
    outfile_prefix='callgrind.%s'%outputid
    outfile_prefix_tmp = '%s.tmp'%outfile_prefix
    cmd+=['--callgrind-out-file=%s.%%p.out'%outfile_prefix_tmp]
    #cmd+=['--log-file=%s.valgrind.%%p.out'%outfile_prefix_tmp]
    if delay_instrumentation:
        cmd+=['--instr-atstart=no']

    #append the user command:
    user_cmd[0] = which(user_cmd[0])

    cmd+=user_cmd
    for i in range(len(cmd)):
        cmd[i]=pipes.quote(cmd[i])
    cmd=' '.join(cmd)
    tmpfiles_before = glob.glob(outfile_prefix_tmp+'*')
    _log('Executing command : %s'%cmd)
    ec = os.system(cmd)
    _log('Finished with exitcode=%i'%ec)
    ikeep=0
    outfiles=[]
    for of in glob.glob(outfile_prefix_tmp+'*'):
        outfiles+=[(time.ctime(os.path.getmtime(of)),of)]
    outfiles.sort()
    for _,tmpfile in outfiles:
        if tmpfile in tmpfiles_before: continue#not ours
        if ec!=0 and delete_on_error:
            _log('Job failed so removing %s'%tmpfile)
            os.remove(tmpfile)
            continue
        fs = os.path.getsize(tmpfile)
        if not never_clean_empty and fs<10000:
            _log('Removing likely uninteresting output file (just %i bytes): %s'%(fs,tmpfile))
            os.remove(tmpfile)
            continue
        ikeep+=1
        final_name= outfile_prefix+'.out' if ikeep==1 else '%s.%i'%(outfile_prefix+'.out',ikeep)
        _log('Renaming %s to %s'%(tmpfile,final_name))
        os.rename(tmpfile,final_name)
        if os.path.exists(final_name+'.bz2'):
            os.remove(final_name+'.bz2')
        bzip2(final_name)
        fscompr = os.path.getsize(final_name+'.bz2')
        _log('Compressing to %s [from %.1fKB to %.1fKB, %.1f%%]'%(final_name+'.bz2',fs/1024.0,fscompr/1024.0,fscompr*100.0/fs))
    if ec>255:
        ec=255
        _log('Exit code is higher than 255, wrapping to 255.')
    _log('Done')
    return ec

if __name__=='__main__':
    #Parse arguments:
    args=sys.argv[1:]
    if '-h' in args or '--help' in args or not args:
        usage()
        sys.exit(0 if args else 1)
    opt_delay=False
    opt_cmd=[]
    opt_outputid='profile'
    if 'delay_instr' in args:
        args.remove('delay_instr')
        opt_delay=True
    for a in args:
        if a.startswith('outid:'):
            opt_outputid=a[6:]
        else:
            opt_cmd+=[a]
    ec=execute(opt_cmd,opt_outputid,opt_delay)
    if ec==0:
        ec=os.system('pmb-fix-build-paths.py callgrind.%s*'%opt_outputid)
    sys.exit(ec)
