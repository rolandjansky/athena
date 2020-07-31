#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import json
import argparse

# Print Header
def printHeader():
    print('='*105)
    print('{0:^105}'.format('PerfMonMTSvc Report'))
    print('='*105)
    print('{0:<105}'.format('IMPORTANT : PLEASE NOTE THAT THIS SERVICE IS CURRENTLY IN R&D PHASE.'))
    print('{0:<105}'.format('            FOR FURTHER INFORMATION/QUERIES PLEASE GET IN TOUCH WITH THE SPOT TEAM.'))
    print('='*105)

# Print Footer
def printFooter():
    print('='*105)
    print('{0:^105}'.format('ALL DONE!'))
    print('='*105)

# Print Component Level Data in Descending Order
def printComponentLevelInfo(execOnly = True, orderBy = 'cpuTime', maxComps = -1):
    if 'componentLevel' not in data:
        return
    print('='*105)
    print('{0:^105}'.format('Component Level Monitoring'))
    print('='*105)
    print('{0:<18}{1:<10}{2:<20}{3:<20}{4:<20}{5:<20}'.format('Step',
                                                              'Count',
                                                              'CPU Time [ms]',
                                                              'Vmem [kB]',
                                                              'Malloc [kB]',
                                                              'Component'))
    print('='*105)
    steps = ['Initialize', 'Execute', 'Finalize', 'Callback', 'preLoadProxy']
    if execOnly:
        steps = ['Execute']
    ncomps = 0
    for step in steps:
        for entry in sorted(data['componentLevel'][step],
                            key=lambda x: data['componentLevel'][step][x][orderBy], reverse = True):
            print('{0:<18}{1:<10}{2:<20.2f}{3:<20}{4:<20}{5:<20}'.format(step,
                                                                        data['componentLevel'][step][entry]['count'],
                                                                        data['componentLevel'][step][entry]['cpuTime'],
                                                                        data['componentLevel'][step][entry]['vmem'],
                                                                        data['componentLevel'][step][entry]['malloc'],
                                                                        entry))
            ncomps += 1
            if (ncomps == maxComps):
                break
        print('='*105)

# Event Level Data in Ascending Order
def printEventLevelInfo():
    if 'eventLevel' not in data:
        return
    print('='*105)
    print('{0:^105}'.format('Event Level Monitoring'))
    print('='*105)
    print('{0:^15}{1:^15}{2:^15}{3:^15}{4:^15}{5:^15}{6:^15}'.format('Event',
                                                                     'CPU [s]',
                                                                     'Wall [s]',
                                                                     'Vmem [kB]',
                                                                     'Rss [kB]',
                                                                     'Pss [kB]',
                                                                     'Swap [kB]'))
    print('='*105)
    for entry in sorted(data['eventLevel'], key=float):
        print('{0:^15}{1:^15.2f}{2:^15.2f}{3:^15}{4:^15}{5:^15}{6:^15}'.format(entry,
                                                                               data['eventLevel'][entry]['cpuTime']*0.001,
                                                                               data['eventLevel'][entry]['wallTime']*0.001,
                                                                               data['eventLevel'][entry]['vmem'],
                                                                               data['eventLevel'][entry]['rss'],
                                                                               data['eventLevel'][entry]['pss'],
                                                                               data['eventLevel'][entry]['swap']))
# Snapshots Summary
def printSnapshotsInfo():
    if 'summary' not in data:
        return
    if 'snapshotLevel' not in data['summary']:
        return
    print('='*105)
    print('{0:^105}'.format('Snapshots Summary'))
    print('='*105)
    print('{0:<14}{1:<13}{2:<13}{3:<13}{4:<13}{5:<13}{6:<13}{7:<13}'.format('Step',
                                                                            'dCPU [s]',
                                                                            'dWall [s]',
                                                                            '<CPU>',
                                                                            'dVmem [kB]',
                                                                            'dRss [kB]',
                                                                            'dPss [kB]',
                                                                            'dSwap [kB]'))
    print('-'*105)
    for entry in ['Configure','Initialize','Execute','Finalize']:
        print('{0:<14}{1:<13.2f}{2:<13.2f}{3:<13.2f}{4:<13}{5:<13}{6:<13}{7:<13}'.format(entry,
                                                                                         data['summary']['snapshotLevel'][entry]['dCPU']*0.001,
                                                                                         data['summary']['snapshotLevel'][entry]['dWall']*0.001,
                                                                                         data['summary']['snapshotLevel'][entry]['cpuUtil'],
                                                                                         data['summary']['snapshotLevel'][entry]['dVmem'],
                                                                                         data['summary']['snapshotLevel'][entry]['dRss'],
                                                                                         data['summary']['snapshotLevel'][entry]['dPss'],
                                                                                         data['summary']['snapshotLevel'][entry]['dSwap']))
    print('*'*105)
    print('{0:<40}{1:<}'.format('Number of events processed:',
                                  data['summary']['nEvents']))
    print('{0:<40}{1:<.0f}'.format('CPU usage per event[ms]:',
                                  float(data['summary']['snapshotLevel']['Execute']['dCPU'])/float(data['summary']['nEvents'])))
    print('{0:<40}{1:<.3f}'.format('Events per second:',
                                  float(data['summary']['nEvents'])/float(data['summary']['snapshotLevel']['Execute']['dWall']*0.001)))
    print('*'*105)
    print('{0:<40}{1:<.2f} GB'.format('Max Vmem:',
                                     float(data['summary']['peaks']['vmemPeak'])/1024./1024.))
    print('{0:<40}{1:<.2f} GB'.format('Max Rss:',
                                     float(data['summary']['peaks']['rssPeak'])/1024./1024))
    print('{0:<40}{1:<.2f} GB'.format('Max Pss:',
                                      float(data['summary']['peaks']['pssPeak'])/1024./1024.))
    print('*'*105)
    print('{0:<40}{1:<.2f} MB'.format('Leak estimate per event Vmem:',
                                 float(data['summary']['leakEstimates']['vmemLeak'])/1024.))
    print('{0:<40}{1:<.2f} MB'.format('Leak estimate per event Pss:',
                                 float(data['summary']['leakEstimates']['pssLeak'])/1024.))
    print('  >> Estimated using the last {0} measurements from the Event Level Monitoring'.format(data['summary']['leakEstimates']['nPoints']))
    print('  >> Events prior to the first 25 are omitted...')

# Print System Info
def printSystemInfo():
    print('='*105)
    print('{0:^105}'.format('System Information'))
    print('='*105)
    print('{0:<40}{1:<}'.format('CPU Model:',data['summary']['sysInfo']['cpuModel'].lstrip()))
    print('{0:<40}{1:<}'.format('Number of Available Cores:',data['summary']['sysInfo']['coreNum']))
    print('='*105)

# Main function
if '__main__' in __name__:

    # Parse the user input
    parser = argparse.ArgumentParser(description = 'Script to print tables using PerfMonMTSvc JSON')

    parser.add_argument('-i', '--input', type = str, required = True,
                        help = 'PerfMonMTSvc JSON file')
    parser.add_argument('-l', '--level', nargs = '?', default = 'All',
                        choices = ['All', 'SummaryLevel', 'EventLevel', 'ComponentLevel'],
                        help = 'Sets the level of detail in the printout')
    parser.add_argument('-e', '--exec-only', dest = 'exec_only', action = 'store_true',
                        help = 'Print only the execute step for the components')
    parser.add_argument('-o', '--order-by', dest = 'order_by', nargs = '?', default = 'cpuTime',
                        choices = ['cpuTime', 'vmem', 'malloc'],
                        help = 'Order components by a specific metric')
    parser.add_argument('-m', '--max-components', dest = 'max_comps', type = int, default = -1,
                        help = 'The maximum number of compoments to be printed '
                        '(default: -1)')

    args = parser.parse_args()

    # Load the data and print the requested information
    with(open(args.input)) as json_file:

        data = json.load(json_file)

        # Print Header
        printHeader()

        # Print Component Level Data
        if args.level in ['All', 'ComponentLevel']:
            printComponentLevelInfo(args.exec_only,
                                    args.order_by,
                                    args.max_comps)

        # Print Event Level Data
        if args.level in ['All', 'EventLevel']:
            printEventLevelInfo()

        # Print Snapshots Summary
        if args.level in ['All', 'SummaryLevel']:
            printSnapshotsInfo()

        # Print System Information
        if args.level in ['All']:
            printSystemInfo()
        # Print Footer
        printFooter()
