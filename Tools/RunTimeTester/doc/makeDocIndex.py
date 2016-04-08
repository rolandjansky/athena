# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



import os, os.path, string

fn='index.html'
if os.path.exists(fn):
    os.remove(fn)

ofile=open(fn,'w')


html="""
<html>
  <head>
    <title>Index for pydoc generated doc files</title>
  </head>

  <body>
    <h3>%s</h3>
    <ul>
""" % (os.getcwd().split('/'))[-2]


ofile.write(html)
for file in os.listdir("."):
    print 
    if os.path.splitext(file)[1] == '.html':
        ofile.write('      <li><a href= "./'+file+'">'+file+'</a></li>\n')

html="""
    </ul>
  </body>
</html>
"""

ofile.write(html)





