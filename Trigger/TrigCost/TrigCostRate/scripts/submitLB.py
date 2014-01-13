#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
############
# > python scripts/submitLB.py --stream L2CostMonitoring  --queue 8nh --filter all --submit
#
#
# Parse command line options
#   
from optparse import OptionParser
p = OptionParser()
p.add_option( '--stream',   type="string",       default='L2CostMonitoring',    dest="stream", help="Stream Name")
p.add_option( '--filter',   type="string",       default='COSTMON',             dest="filter", help="File Filter")
p.add_option( '--runnr',    type="string",       default='190256',              dest="runnr",  help="runnr")
p.add_option( '--queue',    type="string",       default='1nd',                 dest="queue",  help="batch queue")

p.add_option( '--beglb',    type="int",          default=0,                     dest="beglb",  help="start LB")
p.add_option( '--endlb',    type="int",          default=1200,                  dest="endlb",  help="final LB")
p.add_option( '--lbjobs',   type="int",          default=20,                    dest="lbjobs", help="Number of jobs")

p.add_option( '--submit',   action='store_true', default=False,                 dest="submit", help="Submit jobs")

(options, args) = p.parse_args()


if __name__=="__main__":

    if options.endlb<options.beglb:
        print 'ERROR begin must be smaller than end'
        sys.exit(0)

    tot_lbs = options.endlb-options.beglb
    lb_per_job =0

    if options.lbjobs>0:
        lb_per_job=int(tot_lbs/options.lbjobs)
        print 'Will submit '+str(options.lbjobs)+' jobs with '+str(lb_per_job)+' lbs per job'
    else:
        print 'njobs must be greater than zero'
        sys.exit(0)

    file_filters = ['']#'._006','._005','._004','._003','._002','._001','._000']

    if options.filter=='CAF':
        file_filters = ['.root'] #['.root.1','.root.2','.root.3','.root.4','.root.5']

    filters=[]
    if options.filter == 'all':
        filters = ['SFO-4','SFO-9','SFO-10','SFO-11','SFO-12']
        #file_filters = [''] 
    elif options.filter == 'CAF':
        filters =      ['00',
                        '01',
                        '02',
                        '03',
                        '04',
                        '05',
                        '06',
                        '07',
                        '08',
                        '09',
                        '50',
                        '51',
                        '52',
                        '53',
                        '54',
                        '55',
                        '56',
                        '57',
                        '58',
                        '59',
                        '30',
                        '31',
                        '32',
                        '33',
                        '34',
                        '35',
                        '36',
                        '37',
                        '38',
                        '39',
                        '80',
                        '81',
                        '82',
                        '83',
                        '84',
                        '85',
                        '86',
                        '87',
                        '88',
                        '89']

    else:
        filters = [options.filter]

    if os.path.exists(options.filter):
        fil=open(options.filter)
        u=0
        print fil
        for line in fil:
            u+=1
            filters.append(line.strip())
            command = 'bsub -q '+options.queue+' source /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/Trigger/TrigCost/TrigCostRate/scripts/run_LB.sh /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC '+options.runnr+' '+options.stream+' '+line.strip()+' '+str(u)+' 0'
            if options.submit:
                os.system(command)
            else:
                print 'command: ',command
                
        print 'Done!'
        sys.exit(0)
                                                        


    for i in range(0,options.lbjobs):

        startLB=str(int(i*lb_per_job))
        endLB  =str(int((i+1)*lb_per_job))

        for il in file_filters:
            for filter in filters:
                command = 'bsub -q '+options.queue+' source /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/Trigger/TrigCost/TrigCostRate/scripts/run_LB.sh /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC '+options.runnr+' '+options.stream+' '+filter+il+' '+startLB+' '+endLB
            
                if options.submit:
                    os.system(command)
                else:
                    print 'command: ',command

    print 'Done!'
