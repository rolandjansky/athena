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

if ( len(sys.argv) != 2 ):
  print("Please give result file as an argument!")

# Get the result Json file
result_file = sys.argv[1] 

with open( result_file ) as json_file:
  data = json.load(json_file)

  #fig = plt.figure(figsize=(15,75)) # for HelloWorld
  fig = plt.figure(figsize=(31,150)) # for Sisp

  stepNum = len(data)

  for i, step in enumerate(data):
    components = []
    cpu_times = []
    wall_times = []
    
    for component in data[step]:
      
      cpu_time = data[step][component]['cpu_time']
      wall_time = data[step][component]['wall_time']
      
      # Only take nonzero components
      if cpu_time != 0 or wall_time != 0:
        components.append(component)   
        cpu_times.append(cpu_time) # Clear!
        wall_times.append(wall_time)

    # if there is no nonzero in the step, then skip it
    if len(components) == 0:
      continue

    # Horizontal Bar Chart
    ax = fig.add_subplot(stepNum,1,i+1)

    index = np.arange(len(components))
    bar_width = 0.35
    opacity = 0.8

    rects1 = plt.barh(index + (1.5)*bar_width, cpu_times,bar_width,
    alpha=opacity,
    color='b',
    label='CPU Time')

    rects2 = plt.barh(index + bar_width/2, wall_times, bar_width,
    alpha=opacity,
    color='g',
    label='Wall Time')

    plt.ylabel('Components',fontsize = 35)
    plt.xlabel('Time(ms)', fontsize = 35)
    plt.title(step, fontsize = 40, fontweight = "bold")
    plt.yticks(index + bar_width, components)
    plt.legend()
    
    ax.tick_params(axis='both', which='major', labelsize=30)
    ax.tick_params(axis='both', which='minor', labelsize=30)


    fig.set_tight_layout( True )
    
   
  fig.savefig("PerfMonMTSvc_plot.pdf")
