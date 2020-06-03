#!/usr/bin/python
import os

f = open("filter.data.txt.tmp","w");

with open("filter.data.txt") as inf:
    for line in inf:
        parts = line.split() # split line into parts
        if len(parts) > 4:   # if at least 2 parts/columns

            evno = int(parts[1])

            if (evno < 0):
                evno = 2147483647 + (evno - (-2147483648)) + 1

            f.write( parts[0]+" "+str(evno)+" "+parts[2]+" "+parts[3]+" "+parts[4]+"\n" )

        else:
            print 'malformed line'

f.close()
os.rename("filter.data.txt.tmp","filter.data.txt")
