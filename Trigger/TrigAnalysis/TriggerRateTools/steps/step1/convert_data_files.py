# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Mar 20, 2007                            #
#                                                      #
# Description: Convert a list of filenames in a list   #
#              that athena will accept                 #
########################################################

import sys

#print "0 : "+sys.argv[0]
#print "1 : "+sys.argv[1]
#print "2 : "+sys.argv[2]
#print "3 : "+sys.argv[3]
#print "4 : "+sys.argv[4]
#print "5 : "+sys.argv[5]
#print "6 : "+sys.argv[6]

data_path=""
path_prefix=""

if sys.argv[1] != "none":
    data_path = sys.argv[1]+"/"

if sys.argv[2] != "none":
    path_prefix = sys.argv[2]

f = open(sys.argv[3],'r')
fw = open(sys.argv[4],'w')

lines = f.readlines()

num_lines = len(lines)
curr_line = 1
if len(sys.argv) == 5:
    #print "Number of events"
    for line in lines:
        line = line.strip()
        if curr_line == num_lines:
            fw.write('\"'+path_prefix+data_path+line+'\"]\n')
        else:
            fw.write('\"'+path_prefix+data_path+line+'\",\n')
        curr_line = curr_line+1

elif len(sys.argv) == 7:
    #print "Number of files"
    min_line = int(sys.argv[5])
    max_line = int(sys.argv[6])

    if (min_line < num_lines):

        if (max_line > num_lines):
            max_line = num_lines

        for curr_line in range(min_line,max_line):
            line = lines[curr_line]
            #print line
	    line = line.strip()
            if curr_line == max_line-1:
	        fw.write('\"'+path_prefix+data_path+line+'\"]\n')
	    else:
	        fw.write('\"'+path_prefix+data_path+line+'\",\n')
            

f.close()
fw.close
