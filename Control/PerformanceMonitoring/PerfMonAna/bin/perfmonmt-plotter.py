#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @author: Hasan Ozturk <haozturk@cern.ch>


__author__  = "Hasan Ozturk <haozturk@cern.ch"
__doc__     = "A python module which parses the PerfMonMTSvc results and makes plots"


import json

import matplotlib
matplotlib.use('PDF') # use PDF backend
import matplotlib.pyplot as plt
import numpy as np

import operator
import argparse

colors = { "dCPU" : "tab:blue", "dWall" : "tab:orange",
           "dVmem" : "tab:blue", "dPss" : "tab:green",
           "dRss" : "tab:orange", "dSwap" : "tab:red",
           "cpuTime" : "tab:blue", "malloc" : "tab:orange",
           "vmem" : "tab:blue", "pss" : "tab:green",
           "rss" : "tab:orange", "swap" : "tab:red" }

def plotBarChart(params):

  ax = params["ax"]

  offset = 1 - len(params["vals"])
  for metric in params["vals"]:
    vals = params["vals"][metric]
    ax.barh(params["index"] + (offset*0.5)*params["width"], vals, params["width"],
            color = colors[metric], alpha=0.8, label = metric)
    offset += 2

  ax.set_xlabel(params["xlabel"], fontsize=params['xlabelFontSize'])
  ax.set_ylabel(params["ylabel"], fontsize=params['ylabelFontSize'])
  ax.set_title(params["title"], fontsize=params['titleFontSize'], fontweight='bold')
  ax.set_yticks(params["index"])
  ax.set_yticklabels(params["yTickLabels"])
  handles, labels = ax.get_legend_handles_labels()
  ax.legend(reversed(handles), reversed(labels), prop={'size': params['legendFontSize']})
  ax.tick_params(axis='both', which='major', labelsize=30)
  ax.grid(linestyle=':',linewidth=0.1)

def plotLineChart(params):

  ax = params['ax']

  for label, metric in params['yVals'].items():
    ax.plot(params['xVals'], metric, color = colors[label], label = label)

  ax.set_xlabel(params['xlabel'])
  ax.set_ylabel(params['ylabel'])

  ax.set_xticklabels(params['xVals'], rotation='vertical')
  ax.tick_params(axis='both', which='major', labelsize=10)
  ax.set_title(params['title'])
  ax.legend()
  ax.grid(linestyle=':',linewidth=0.1)

def sortComponents(compNames, measDict, compCountPerPlot):

  metricToCompMeasDict = {} # This is a lookup map, we use it to map components to measurements after sorting
  measList = []
  for metric, meas in measDict.items():

    metricToCompMeasDict[metric] = dict(zip(compNames, meas))
    measList.append(meas)

  # Sort by the sum of each metric: Ex: Sort by (vmem + malloc) for memMon
  sortByList = [sum(x) for x in zip(*measList)]
  sortedCompMeasTuple = sorted(dict(zip(compNames, sortByList)).items(), key = operator.itemgetter(1))
  sortedCompNames = [ compMeas[0] for compMeas in sortedCompMeasTuple]

  sortedMeasurements = {}
  # Populate sortedMeasurements according to the sortedCompNames
  for comp in sortedCompNames[-compCountPerPlot:]:
    for metric, compMeas in metricToCompMeasDict.items():

      if metric not in sortedMeasurements:
        sortedMeasurements[metric] = []

      sortedMeasurements[metric].append(compMeas[comp])



  return sortedCompNames[-compCountPerPlot:], sortedMeasurements

def plotSnapshotLevel(snapshotData, plotname):

  # Collect data
  stepNames, dCPUVals, dWallVals, dVmemVals, dRssVals, dPssVals, dSwapVals = [],[],[],[],[],[],[]
  for step in ['Finalize', 'Execute', 'Initialize', 'Configure']:
    meas = snapshotData[step]
    
    # Show in seconds
    dCPU = meas["dCPU"] * 0.001
    dWall = meas["dWall"] * 0.001

    # Show in megabytes
    dVmem = meas["dVmem"] * 0.001
    dRss = meas["dRss"] * 0.001
    dPss = meas["dPss"] * 0.001
    dSwap = meas["dSwap"] * 0.001
 
    stepNames.append(step)
    dCPUVals.append(dCPU)
    dWallVals.append(dWall)

    dVmemVals.append(dVmem)
    dRssVals.append(dRss)
    dPssVals.append(dPss)
    dSwapVals.append(dSwap)

  timeMonVals = {
    "dCPU": dCPUVals,
    "dWall": dWallVals
  }

  memMonVals = {
    "dVmem": dVmemVals,
    "dRss": dRssVals,
    "dPss": dPssVals,
    "dSwap": dSwapVals,
  }


  timeMonFig, timeMonAx = plt.subplots(figsize=(20,15))
  memMonFig, memMonAx = plt.subplots(figsize=(20,15))

  timeMonParams = {
    "ax": timeMonAx,
    "index": np.arange(len(stepNames)),
    "width": 0.5/len(timeMonVals),
    "vals": timeMonVals,
    "yTickLabels": stepNames,
    "xlabel": "Time [sec]",
    "ylabel": "Steps",
    "title": "Snapshot Level Monitoring: Time Metrics",
    "titleFontSize": 40,
    "xlabelFontSize": 40,
    "ylabelFontSize": 40,
    "legendFontSize": 30

  }

  memMonParams = {
    "ax": memMonAx,
    "index": np.arange(len(stepNames)),
    "width": 0.5/len(memMonVals), # Think about this
    "vals": memMonVals,
    "yTickLabels": stepNames,
    "xlabel": "Memory [MB]",
    "ylabel": "Steps",
    "title": "Snapshot Level Monitoring: Memory Metrics",
    "titleFontSize": 40,
    "xlabelFontSize": 40,
    "ylabelFontSize": 40,
    "legendFontSize": 30
  }
  

  plotBarChart(timeMonParams)
  plotBarChart(memMonParams)


  timeMonFig.set_tight_layout( True )
  timeMonFig.savefig("Snaphot_Level_Time")

  memMonFig.set_tight_layout(True)
  memMonFig.savefig("Snapshot_Level_Memory")


def plotComponentLevel(componentLevelData, compCountPerPlot):
  
  timeMonFig = plt.figure(figsize=(35,105))
  memMonFig = plt.figure(figsize=(35,105))

  for idx, step in enumerate(componentLevelData):

    compNames, vmemVals, cpuTimeVals, mallocVals, countVals = [],[],[],[],[]
    for comp, meas in componentLevelData[step].items():

      count = meas["count"]
      cpuTime = meas["cpuTime"] * 0.001 # seconds
      malloc = meas["malloc"] * 0.001 # MB
      vmem = meas["vmem"] * 0.001 # MB

      # Discard negative measurements
      if vmem < 0 or malloc < 0:
        continue

      compNames.append(comp + " [" + str(count) + "]")
      vmemVals.append(vmem)
      cpuTimeVals.append(cpuTime)
      mallocVals.append(malloc)
      countVals.append(count)

    timeMonVals = {
      "cpuTime": cpuTimeVals
    }

    memMonVals = {
      "vmem": vmemVals,
      "malloc": mallocVals,
    }

    # Sort the components
    sortedTimeMonCompNames, sortedTimeMonVals = sortComponents(compNames, timeMonVals, compCountPerPlot)
    sortedCompNamesMem, sortedMemMonVals = sortComponents(compNames, memMonVals, compCountPerPlot)

    timeMonAx = timeMonFig.add_subplot(len(componentLevelData),1,idx+1)
    memMonAx = memMonFig.add_subplot(len(componentLevelData),1,idx+1)

    timeMonParams = {
      "ax": timeMonAx,
      "index": np.arange(len(sortedTimeMonCompNames)),
      "width": 0.5/len(sortedTimeMonVals), # Think about this
      "vals": sortedTimeMonVals,
      "yTickLabels": sortedTimeMonCompNames,
      "xlabel": "Time [sec]",
      "ylabel": "Components",
      "title": step,
      "titleFontSize": 70,
      "xlabelFontSize": 50,
      "ylabelFontSize": 50,
      "legendFontSize": 30
    }

    memMonParams = {
      "ax": memMonAx,
      "index": np.arange(len(sortedCompNamesMem)),
      "width": 0.5/len(sortedMemMonVals), # Think about this
      "vals": sortedMemMonVals,
      "yTickLabels": sortedCompNamesMem,
      "xlabel": "Memory [MB]",
      "ylabel": "Components",
      "title": step,
      "titleFontSize": 70,
      "xlabelFontSize": 50,
      "ylabelFontSize": 50,
      "legendFontSize": 30
    }

    plotBarChart(timeMonParams)
    plotBarChart(memMonParams)

  timeMonFig.set_tight_layout( True )
  timeMonFig.savefig("Component_Level_Time")

  memMonFig.set_tight_layout( True )
  memMonFig.savefig("Component_Level_Memory")


def plotEventLevel(eventLevelData):

  sortedEventLevelData = sorted(eventLevelData.items(), key=lambda i: int(i[0]))

  eventVals, cpuTimeVals, vmemVals, rssVals, pssVals, swapVals = [], [], [], [], [], []

  timeMonFig, timeMonAx = plt.subplots()
  memMonFig, memMonAx = plt.subplots()

  for entry in sortedEventLevelData:

    event = entry[0]
    meas = entry[1]

    # Time metrics in seconds, Memory metrics in megabytes
    eventVals.append(event)
    cpuTimeVals.append(meas['cpuTime'] * 0.001)
    vmemVals.append(meas['vmem'] * 0.001)
    rssVals.append(meas['rss'] * 0.001)
    pssVals.append(meas['pss'] * 0.001)
    swapVals.append(meas['swap'] * 0.001)


  timeMonVals = {
    "cpuTime": cpuTimeVals
  }

  memMonVals = {
    "vmem": vmemVals,
    "rss": rssVals,
    "pss": pssVals,
    "swap": swapVals
  }

  timeMonParams = {
    "ax": timeMonAx,
    "yVals": timeMonVals, 
    "xVals": eventVals, # Maybe x ticks?
    "xlabel": "Events",
    "ylabel": "Time [sec]",
    "title": "Event Level Time Measurements"
  }

  memMonParams = {
    "ax": memMonAx,
    "yVals": memMonVals,
    "xVals": eventVals, # Maybe x ticks?
    "xlabel": "Events",
    "ylabel": "Memory [MB]",
    "title": "Event Level Memory Measurements"
  }


  plotLineChart(timeMonParams)
  plotLineChart(memMonParams)

  timeMonFig.set_tight_layout(True)
  timeMonFig.savefig("Event_Level_Time")

  memMonFig.set_tight_layout(True)
  memMonFig.savefig("Event_Level_Memory")
  
def main():
    ''' Main function for producing plots from PerfMonMT JSON file.'''

    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--inFile", dest = "inFile",
                        default = 'PerfMonMTSvc_result.json',
                        help = 'The input JSON file')
    parser.add_argument("-n", "--numberOfCompsPerPlot",
                        dest = "numberOfCompsPerPlot", default = 20,
                        help = "The number of components to be plotted")

    args = parser.parse_args()

    inFile = args.inFile
    numberOfCompsPerPlot = args.numberOfCompsPerPlot

    with open( inFile ) as jsonFile:

      data = json.load(jsonFile)

      if "snapshotLevel" in data["summary"]:
        snapshotData = data["summary"]["snapshotLevel"]
        plotSnapshotLevel(snapshotData, 'snapshotLevel.pdf')

      if "componentLevel" in data:
        componentLevelData = data["componentLevel"]
        plotComponentLevel(componentLevelData, int(numberOfCompsPerPlot))

      if "eventLevel" in data:
        eventLevelData = data["eventLevel"]
        plotEventLevel(eventLevelData)


if __name__ == "__main__":
    main()

