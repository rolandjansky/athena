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

  cpu_time = snapshot_data['Event_loop']['cpu_time']
  wall_time = snapshot_data['Event_loop']['wall_time']
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
  ax.set_title('Snapshot Level Monitoring')
  ax.set_yticks(ind)
  ax.set_yticklabels(snapshot_steps)
  ax.legend()

  fig.set_tight_layout( True )
  fig.savefig(plotname) 

def comp_plotter(complevel_data, plotname):
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

    plt.ylabel('Components',fontsize = 35)
    plt.xlabel('Time(ms)', fontsize = 35)
    plt.title(step, fontsize = 40, fontweight = "bold")
    plt.yticks(index + bar_width, sorted_components)
    plt.legend(prop={'size': 30})
    
    ax.tick_params(axis='both', which='major', labelsize=30)
    ax.tick_params(axis='both', which='minor', labelsize=30)


    fig.set_tight_layout( True )
    
   
  fig.savefig(plotname)


with open( result_file ) as json_file:
  data = json.load(json_file)

  snapshot_data = data['Snapshot_level']
  snapshot_plotter(snapshot_data, 'snapshot_level.pdf')
  
  serial_complevel_data = data['Serial_Component_level']
  comp_plotter(serial_complevel_data, 'serial_complevel.pdf')

  parallel_complevel_data = data['Parallel_Component_level']
  comp_plotter(parallel_complevel_data, 'parallel_complevel.pdf')

