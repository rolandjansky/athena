# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# a function to obtain process memeory usage

def memstat():
    memfile = open('/proc/self/status', 'r')
    lines = memfile.readlines()
    memfile.close()

    pid = 0
    for line in lines:
        if line.startswith('Pid'): pid = int(line.split()[1])
        #break
    
    lines = [l for l in lines if l.startswith('Vm')]
    msg = 'Memory usage for process %d:\n' % pid
    for l in lines:
        msg += l

    # memfile = open('/proc/meminfo', 'r')
    # msg += memfile.read()
    # memfile.close()
    
    return msg
