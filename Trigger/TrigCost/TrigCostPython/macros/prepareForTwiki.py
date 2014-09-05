# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os

if len(sys.argv) < 1 :
    print "Please indicate the directory to prepare"
    sys.exit(-1)

dir = sys.argv[1]

if not os.path.exists(dir) :
    print "Directory does not exist"
    sys.exit(-1)

if not os.path.exists(dir+"/TriggerCosts.xml") :
    print "Could not find TriggerCosts.xml"
    sys.exit(-1)

if not os.path.exists(dir+"/index.html") :
    print "Could not find index.html"
    sys.exit(-1)

#testarea = os.getenv('TestArea')

#Make group and cumulative tables

os.system("./makeGroupTables.py %s/TriggerCosts.xml -o %s/GroupRatesTables.html" %( dir, dir))
os.system("./makeCumuTables.py %s/TriggerCosts.xml -o %s/CumuRatesTables.html" %( dir, dir))

#Edit TriggerCosts.xml

file = open(dir+"/TriggerCosts.xml", "r")
temp = open(dir+"/Temp_TriggerCosts.xml", "w")

for line in file :
    loc = line.find("trigger_rates.xsl")
    if loc != -1 :
        temp.write(line[0:loc] + "../" + line[loc:])
    else :
        temp.write(line)

file.close()
temp.close()

os.system("mv %s/Temp_TriggerCosts.xml %s/TriggerCosts.xml" %(dir, dir))

#Edit index.html

file = open(dir+"/index.html", "r")
temp = open(dir+"/Temp_index.html", "w")

for line in file :
    loc = line.find(r"</pre>")
    if loc != -1 :
        temp.write("\n")
        temp.write("<a STYLE=\"text-decoration:none\" href=\"GroupRatesTables.html\">Group Rates Tables</a>\n\n")
        temp.write("<a STYLE=\"text-decoration:none\" href=\"CumuRatesTables.html\">Cumulative Rates Tables</a>\n")
        temp.write("</pre>\n") 
    else :
        temp.write(line)

file.close()
temp.close()

os.system("mv %s/Temp_index.html %s/index.html" %(dir, dir))

    

