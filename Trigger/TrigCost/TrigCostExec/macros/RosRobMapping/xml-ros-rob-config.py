#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Read partition and dump configuration for ros/rob to xml file.

Please note that you should refer to all details of:
  https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/DataFlow/dccommon/trunk/src/ROSConf.cxx

  In order to compute the valid channels correctly (for instance, take into
  account if a ROS or ROBIN was disabled).
"""

import sys
from pm.project import Project
import eformat
from xml.dom.minidom import Document

if len(sys.argv)==1:
  print "usage ", sys.argv[0], " partition file <xml output file> "
  exit()
  
xmldoc = Document()
xmldata = xmldoc.createElement("ros-rob-data")
xmldoc.appendChild(xmldata)

print 'Loading database...'
db = Project(sys.argv[1])
print 'Retrieving all ROSs...'
part = db.getObject('Partition', 'ATLAS')

for ros in part.get('ROS'):
  print ros.id, '= [',
  xmlros = xmldoc.createElement("ros")
  xmlros.setAttribute("id",ros.id)
  xmldata.appendChild(xmlros)
  for robin in ros.Contains:
    rob_ids_print = []
    for rom in robin.Contains:
      sd = eformat.helper.SubDetector.values[ros.Detector.LogicalId] 
      source_id = eformat.helper.SourceIdentifier(sd, rom.Id)
      rob_ids_print.append("0x%08x" % source_id.code())
      xmlrob = xmldoc.createElement("rob")
      xmlrob.setAttribute("id","0x%08x" % source_id.code())
      xmlros.appendChild(xmlrob)
      del xmlrob
    print ', '.join(rob_ids_print),
  del xmlros
  print ']'

# print xml   
#print xmldoc.toprettyxml(indent="  ")  

# write xml to file
if len(sys.argv)==3:
  fp = open(sys.argv[2],"w")
else:
  fp = open("ros-rob.xml","w")
# writexml(self, writer, indent='', addindent='', newl='', encoding=None)
xmldoc.writexml(fp,indent="  ", addindent="  ", newl="\n", encoding="UTF-8")
