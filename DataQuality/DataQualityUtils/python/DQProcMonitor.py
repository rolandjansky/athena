# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

#python 2.6 is buggy, it can't import modules in deamon threads one has to import them in to global 
from resource import getrusage, RUSAGE_SELF, RUSAGE_CHILDREN
from time import sleep, time

PROCMONVERSION="$Id: DQProcMonitor.py 359096 2011-04-15 13:25:36Z kama $"

def formatRusage(rusage):
    fieldnames=["utime","stime","maxrss","shrmem","ushrmem","ushrstk","minflt",
                 "majflt","nswap","inblock","oublock","msgsnd","msgrcv",
                 "nsignals","nvcontsw","nivcontsw"]
    return ", ".join(["=".join([n,"%s"%v] if isinstance(v, int) else [n,"%f"%v]) for n,v in zip(fieldnames,rusage)])

def DQProcMon(*args,**kwargs):
    #    from resource import getrusage, RUSAGE_SELF, RUSAGE_CHILDREN
    #    from pprint import pformat,pprint
    #    from time import sleep, time
    pollPeriod = kwargs.pop("poll_period", 1.0)
    if pollPeriod < 0.25: 
        pollPeriod=0.25 #min poll time
    filename = kwargs.pop("file_name","DQResourceUtilization.txt")
    print("Monitoring thread poll period=",pollPeriod,"seconds, output file name=\"%s\""%filename)
    with open(filename,'w') as resoutfile:
        while True:
            try:
                ru=getrusage(RUSAGE_SELF)
                cru=getrusage(RUSAGE_CHILDREN)                
            except Exception:
                pass
            #self_info="[utime=%f, stime=%f, maxrss=%s, minflt=%s, majflt=%s, nvcsw=%s, nivcsw=%s]" % (ru.ru_utime,
            #ru.ru_stime, ru.ru_maxrss, ru.ru_minflt, ru.ru_majflt, ru.ru_)
            resoutfile.write(str(time())+" <self> "+formatRusage(ru)+" <children> "+formatRusage(cru)+"\n")
            #pprint(rusage,resoutfile)
            resoutfile.flush()
            sleep(pollPeriod)

def startProcMonThread(pollPeriod=1,file_name="DQResourceUtilization.txt"):
    from threading import Thread
    ResMon=Thread(target=DQProcMon,args=[],kwargs={'poll_period':pollPeriod,
                                                   'file_name':file_name})
    #ResMon.setDaemon(True)
    ResMon.start()
    print("========Starting DQProcMonThread==============")

