#!/usr/bin/env python

################################
#                              #
#    Plotting the PSS usage    #
#                              #
#   Author: Leonardo Badurina  #
#       Date: 21/08/2018       #
#                              #
################################

import os
import re
import matplotlib.pyplot as plt
import pylab

if __name__ == "__main__":

	dict_pss = dict() 
	rootDir = '.'

	# Finding memory files from directories
	for dirName, subdirList, fileList in os.walk(rootDir):
		# Looping through tests that monitor PSS usage
		if "HLT_Ch" or "HLT_physicsV7" in dirName:
			for fname in fileList:
				if fname=="MemMon.txt" or fname=="MemMonSummary.json":
					# Initialising key in dictionary
                			dict_pss[dirName]=[[0,0,0,0,0]]

	for key in dict_pss:

	        # Defining paths to files
		pss_tot_file= key+'/MemMon.txt'
		pss_summ_file= key+'/MemMonSummary.json'

		# Reading MemMon.txt			
		f = open(pss_tot_file, "r")
		pss_tot_lines = f.readlines()
		f.close()
			
		# Reading MemMonSummary.json
		f = open(pss_summ_file, "r")
                pss_summ_lines = f.readlines()
                f.close()
                
		# Finding maximum and average PSS usage from MemMonSummary.json
		for line in pss_summ_lines:    
			max_pss = re.search('"maxPSS":(\d+)',line).group(1)
                       	avg_pss = re.search('"avgPSS":(\d+)',line).group(1)

		# Finding PSS usage from MemMon.txt and appending max and avg PSS
		i = 0 # initialising counter
		for line in pss_tot_lines:
			# Skip line if header
	        	if not line.startswith("15"):
                        	continue
		   	time, vmem, pss, rss, swap, rchar, wchar, rbytes, wbytes = line.split()
			# Set intial time
		        if i == 0:
				timeN = float(time)
			# Appending time, PSS, maximum PSS and average PSS
    		    	dict_pss[key].append([0,float(time)-timeN,float(pss)/1024./1024.,float(max_pss)/1024./1024.,float(avg_pss)/1024./1024.])
		        i =+ 1

	# Plotting PSS as a function of time
	for key, data_list in dict_pss.iteritems():
        	null,time,pss,max_pss,avg_pss  = zip(*data_list)
        	plt.plot(time, pss, label=key)

	plt.legend(loc="best",prop={'size': 10}, title="Test name")
	plt.xlabel("Time / s")
	plt.ylabel("PSS Usage / GB")
	plt.title("Total PSS usage as a function of time\n")
	plt.savefig('PSS_t.png')
	plt.clf()

        # Plotting avg and max PSS as a function of test name
	max_pss_array=[]
	avg_pss_array=[]
	test_name=[]
	for key in sorted(dict_pss):
		max_pss_array.append(dict_pss[key][1][3])
		avg_pss_array.append(dict_pss[key][1][4])
		test_name.append(key)

	pylab.figure(1)

	x = range(len(test_name))
	pylab.xticks(x, test_name, fontsize=7, rotation=10)
	pylab.xlim(-0.3, len(test_name)-0.3)

	pylab.plot(x,max_pss_array,label="Maximum PSS Usage",marker='.')
	pylab.plot(x,avg_pss_array,label="Time-averaged PSS Usage",marker='.')

        pylab.legend(loc="best",prop={'size': 10})
        pylab.xlabel("Test name")
        pylab.ylabel("PSS Usage / GB")
        pylab.title("Maximum and average PSS as a fuction of test name \n")
        pylab.savefig('PSS_t_summary.png')
       	pylab.clf()


	
















