#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @author: Hasan Ozturk <haozturk@cern.ch>


__author__  = "Hasan Ozturk <haozturk@cern.ch"
__doc__     = "A python module which parses the PerfMonMTSvc results and makes plots"


import sys
import json

import matplotlib
matplotlib.use('PDF') # use PDF backend
import matplotlib.pyplot as plt
import numpy as np

import operator

if ( len(sys.argv) != 2 ):
  print("Please give result file as an argument!")
  sys.exit()

# Get the result Json file
result_file = sys.argv[1] 



def snapshot_plotter(snapshot_data, plotname):
  

  snapshot_steps = [ 'Finalize', 'Execute','Initialize']
  #snapshot_steps = []
  snapshot_cpu_times = []
  snapshot_wall_times = []

  # Get rid of code duplication
  cpu_time = snapshot_data['Finalize']['cpu_time']
  wall_time = snapshot_data['Finalize']['wall_time']
  snapshot_cpu_times.append(cpu_time)
  snapshot_wall_times.append(wall_time)

  cpu_time = snapshot_data['Event Loop']['cpu_time']
  wall_time = snapshot_data['Event Loop']['wall_time']
  snapshot_cpu_times.append(cpu_time)
  snapshot_wall_times.append(wall_time)

  cpu_time = snapshot_data['Initialize']['cpu_time']
  wall_time = snapshot_data['Initialize']['wall_time']
  snapshot_cpu_times.append(cpu_time)
  snapshot_wall_times.append(wall_time)

  ind = np.arange(len(snapshot_steps))
  width = 0.35
     
  fig, ax = plt.subplots()

  rects1 = ax.barh(ind - width/2, snapshot_cpu_times, width, label = 'CPU Time') 
  rects2 = ax.barh(ind + width/2, snapshot_wall_times, width, label = 'Wall Time') 
  ax.set_xlabel('Time(ms)')
  ax.set_ylabel('Steps')
  ax.set_title('CPU & Wall Time Summary')
  ax.set_yticks(ind)
  ax.set_yticklabels(snapshot_steps)
  ax.legend()

  fig.set_tight_layout( True )
  fig.savefig(plotname) 

def time_plotter(complevel_data, plotname):
  # Plot Component Level Monitoring
 
  #fig = plt.figure(figsize=(31,150))
  fig = plt.figure(figsize=(50,150))
  stepNum = len(complevel_data)
 
  measurement_threshold = 5 # 5 ms

  for i, step in enumerate(complevel_data):

    components = []
    cpu_times = []
    wall_times = []
    
    for component in complevel_data[step]:
      cpu_time = complevel_data[step][component]['cpu_time']
      wall_time = complevel_data[step][component]['wall_time']
      
      # Only take components whose measurements are higher a certain threshold
      if cpu_time + wall_time > measurement_threshold:
        components.append(component)   
        cpu_times.append(cpu_time) # Clear!
        wall_times.append(wall_time)

    # Sort the components 

    # Prepare the necessary data structures for sorting ( could be more efficient!  )
    cpu_wall_tuple_list = zip(cpu_times, wall_times)
    comp_dict = dict( zip(components, cpu_wall_tuple_list) )
    sortby_list = [ cpu + wall for cpu, wall in zip(cpu_times, wall_times)]
    sort_dict = dict(zip(components, sortby_list))
    
    # Sort the components according to wall_time + cpu time
    sorted_comp_tuple_list = sorted(sort_dict.items() , key = operator.itemgetter(1))

    sorted_components = []
    sorted_cpu_times = []
    sorted_wall_times = []
 
    # Fill the necessary lists for plotting
    for idx in sorted_comp_tuple_list:
      curr_comp = idx[0]
      curr_cpu = comp_dict[curr_comp][0]
      curr_wall = comp_dict[curr_comp][1]

      sorted_components.append(curr_comp)
      sorted_cpu_times.append(curr_cpu)
      sorted_wall_times.append(curr_wall)
    
    # if there is no nonzero measurement in the step, then skip it
    if len(sorted_components) == 0:
      continue

    # Horizontal Bar Chart
    ax = fig.add_subplot(stepNum,1,i+1)

    index = np.arange(len(sorted_components))
    bar_width = 0.35
    opacity = 0.8

    rects1 = plt.barh(index + (1.5)*bar_width, sorted_cpu_times,bar_width,
    alpha=opacity,
    label='CPU Time')

    rects2 = plt.barh(index + bar_width/2, sorted_wall_times, bar_width,
    alpha=opacity,
    label='Wall Time')

    plt.ylabel('Components',fontsize = 50)
    plt.xlabel('Time(ms)', fontsize = 35)
    plt.title(step, fontsize = 60, fontweight = "bold")
    plt.yticks(index + bar_width, sorted_components)
    plt.legend(prop={'size': 30})
    
    ax.tick_params(axis='both', which='minor', labelsize=40)


    fig.set_tight_layout( True )
    
   
  fig.savefig(plotname)

def mem_plotter(complevel_data, plotname):
 
  fig = plt.figure(figsize=(50,150))
  stepNum = len(complevel_data)
 
  measurement_threshold = 5 # 5 kB

  for i, step in enumerate(complevel_data):

    components = []
    vmem_measurements = []
    rss_measurements = []
    pss_measurements = []
    swap_measurements = []

    
    for component in complevel_data[step]:

      vmem = complevel_data[step][component]['vmem']
      rss = complevel_data[step][component]['rss']
      pss = complevel_data[step][component]['pss']
      swap = complevel_data[step][component]['swap']
      
      # Only take components whose measurements are higher a certain threshold
      if vmem + rss + pss + swap > measurement_threshold:
        components.append(component)  
        vmem_measurements.append(vmem) 
        rss_measurements.append(rss) 
        pss_measurements.append(pss) 
        swap_measurements.append(swap) 

    # Sort the components 

    # Prepare the necessary data structures for sorting ( could be more efficient!  )
    mem_stats_tuple_list = zip(vmem_measurements, rss_measurements,pss_measurements,swap_measurements)
    comp_dict = dict( zip(components, mem_stats_tuple_list) )
    sortby_list = [ vmem + rss + pss + swap for vmem, rss, pss, swap in zip(vmem_measurements, rss_measurements,pss_measurements,swap_measurements)]
    sort_dict = dict(zip(components, sortby_list))
    
    # Sort the components according to vmem + rss + pss + swap
    sorted_comp_tuple_list = sorted(sort_dict.items() , key = operator.itemgetter(1))

    sorted_components = []
    sorted_vmem_measurements = []
    sorted_rss_measurements = []
    sorted_pss_measurements = []
    sorted_swap_measurements = []
 
    # Fill the necessary lists for plotting
    for idx in sorted_comp_tuple_list:
      curr_comp = idx[0]
      curr_vmem = comp_dict[curr_comp][0]
      curr_rss = comp_dict[curr_comp][1]
      curr_pss = comp_dict[curr_comp][2]
      curr_swap = comp_dict[curr_comp][3]

      sorted_components.append(curr_comp)

      sorted_vmem_measurements.append(curr_vmem)
      sorted_rss_measurements.append(curr_rss)
      sorted_pss_measurements.append(curr_pss)
      sorted_swap_measurements.append(curr_swap)
    
    # if there is no nonzero measurement in the step, then skip it
    if len(sorted_components) == 0:
      continue

    # Horizontal Bar Chart
    ax = fig.add_subplot(stepNum,1,i+1)

    index = np.arange(len(sorted_components))
    bar_width = 0.20
    opacity = 0.8

    rects1 = plt.barh(index + (2.5)*bar_width, sorted_vmem_measurements,bar_width,
    alpha=opacity,
    label='Virtual Memory')

    rects2 = plt.barh(index + (1.5)*bar_width, sorted_rss_measurements, bar_width,
    alpha=opacity,
    label='Resident Set Size')

    rects3 = plt.barh(index + (0.5)*bar_width, sorted_pss_measurements,bar_width,
    alpha=opacity,
    label='Proportional Set Size')

    rects4 = plt.barh(index - (0.5)*bar_width, sorted_swap_measurements, bar_width,
    alpha=opacity,
    label='Swap')

    plt.ylabel('Components',fontsize = 50)
    plt.xlabel('Memory Size(kB)', fontsize = 45)
    plt.title(step, fontsize = 70, fontweight = "bold")
    plt.yticks(index + bar_width, sorted_components)
    plt.legend(prop={'size': 60})
    
    ax.tick_params(axis='both', which='minor', labelsize=40)


    fig.set_tight_layout( True )
    
   
  fig.savefig(plotname)


def eventLevelTimeMon_plotter(eventlevel_data, plotname):

  sorted_data = sorted(eventlevel_data.items(), key=lambda i: int(i[0]))

  
  checkPoint_list = []
  cpu_list = []
  wall_list = []

  for entry in sorted_data:

    checkPoint = entry[0]
    measurement = entry[1]

    cpu_time = measurement["cpu_time"]
    wall_time = measurement["wall_time"]

    checkPoint_list.append(checkPoint)
    cpu_list.append(cpu_time)
    wall_list.append(wall_time)

  fig = plt.figure()
 
  plt.plot(checkPoint_list, cpu_list, label = "CPU Time")
  plt.plot(checkPoint_list, wall_list, label = "Wall Time")
  plt.xlabel('Events')
  plt.ylabel('Time[ms]')

  index = np.arange(len(checkPoint_list))

  plt.xticks(index, checkPoint_list, rotation='vertical')
  plt.tick_params(axis='x', labelsize=3)

  plt.title('Event Level Time Measurements')
  plt.legend()

  fig.set_tight_layout( True )
    
   
  fig.savefig(plotname)


def eventLevelMemMon_plotter(eventlevel_data, plotname):

  # Sort by event check points
  sorted_data = sorted(eventlevel_data.items(), key=lambda i: int(i[0]))
  
  checkPoint_list = []
  vmem_list = []
  rss_list = []
  pss_list = []
  swap_list = []

  for entry in sorted_data:

    checkPoint = entry[0]
    measurement = entry[1]

    vmem = measurement["vmem"]
    rss = measurement["rss"]
    pss = measurement["pss"]
    swap = measurement["swap"]

    checkPoint_list.append(checkPoint)
    vmem_list.append(vmem)
    rss_list.append(rss)
    pss_list.append(pss)
    swap_list.append(swap)

  fig = plt.figure()
 
  plt.plot(checkPoint_list, vmem_list, label = "Vmem")
  plt.plot(checkPoint_list, rss_list, label= "Rss")
  plt.plot(checkPoint_list, pss_list,  label = "Pss")
  plt.plot(checkPoint_list, swap_list,  label = "Swap")
  plt.xlabel('Events')
  plt.ylabel('Memory[kB]')

  index = np.arange(len(checkPoint_list))

  plt.xticks(index, checkPoint_list, rotation='vertical')
  plt.tick_params(axis='x', labelsize=3)

  plt.title('Event Level Memory Measurements')
  plt.legend()

  fig.set_tight_layout( True )
   
  fig.savefig(plotname)


  

with open( result_file ) as json_file:
  data = json.load(json_file)

  snapshot_data = data['Snapshot_level']
  snapshot_plotter(snapshot_data, 'snapshot_level.pdf')
  
  timeMon_serial_data = data['TimeMon_Serial']
  time_plotter(timeMon_serial_data, 'TimeMon_Serial.pdf')

  memMon_serial_data = data['MemMon_Serial']
  mem_plotter(memMon_serial_data, 'MemMon_Serial.pdf')

  if 'TimeMon_Parallel' in data:
    timeMon_parallel_data = data['TimeMon_Parallel']
    eventLevelTimeMon_plotter(timeMon_parallel_data, 'TimeMon_Parallel.pdf')

  if 'MemMon_Parallel' in data:
    memMon_parallel_data = data['MemMon_Parallel']
    eventLevelMemMon_plotter(memMon_parallel_data, 'MemMon_Parallel.pdf')
