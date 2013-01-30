#!/bin/env python2.5
#
# ----------------------------------------------------------------
# Script : subproc.py
# Project: AtlRunQuery
# Purpose: Grep lines in file and paste into shtml
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2009
# ----------------------------------------------------------------
#
import os
import sys
import datetime
import time

def durationstr(sec):
    dt = time.gmtime(sec)[2:6]
    if dt[0]>1:
        return "%id %ih %im %is" % ((dt[0]-1,) + dt[1:])
    else:
        return "%ih %im %is" % dt[1:]

if __name__ == '__main__':

    # note, it is assumed that the file exist and that exactly 2 argumentds are given
    d, searchstr, age, reffile = sys.argv

    # search for dataset pattern
    os.system('grep %s %s > data/%s.tmp.txt' % (searchstr, reffile, searchstr))

    # read file and insert
    fr = open( 'data/%s.tmp.txt' % searchstr, "r" )
    ic = 0
    filenames = []
    paths     = []
    for line in fr:
        ic += 1
        filename, sep, info = str(line).partition(' ')
        # remove path - caution, different for RAW files
        if '.RAW' in filename:
            path = filename[:filename.index(searchstr)]
            if not path in paths: paths.append(path)        
            if len(paths) == 1:
                filenames.append( [path, filename[filename.index(searchstr):]] )
            else:
                filenames.append( [path, filename] )
        else:
            path = filename[:filename.index(searchstr)+len(searchstr)+1]
            if not path in paths: paths.append(path)        
            if len(paths) == 1:
                filenames.append( [path, filename[filename.index(searchstr)+len(searchstr)+1:]] )
            else:
                filenames.append( [path, filename] )
    fr.close()    

    content = ''
    if ic>0:
        refpath = paths[0]
        for path, filename in filenames:        
            if path == refpath:
                content += '<tr><td style="padding-top:5px;padding-bottom:5px;">%s<br>\n' % filename
            else:
                content += '<tr><td style="padding-top:5px;padding-bottom:5px;"><font color="#CC0000"><i>The path for the following file differs from the above default. Full path + filename is thus given:</i></font><br>\n' 
                content += '%s<br>\n' % filename
            content += '<font color="#999999"><font size="1px">%s</font></font><br></td></tr>\n'  % info

    # page header
    s = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"><html xmlns:"my"><head><head><title>ATLCAL results for dataset: %s' % searchstr
    s +=  '</title><LINK href="atlas-runquery.css" rel="stylesheet" type="text/css"></head><body>\n\n'
    s += '<table style="font-family:sans-serif;padding:4px;font-size:80%;width:100%">'

    # body
    s += '<tr><td style="font-size:110%%"><b>ATLCAL disk pool &minus; files belonging to dataset: <font color="#CC0000">%s</font><br><hr color="gray" size=1></td></tr>\n' % searchstr
    try:
        s += '<tr><td>Age of dataset: <b>%s</b></td></tr>' % durationstr(float(age))
    except ValueError:
        s += '<tr><td>Age of dataset: <b>%s</b></td></tr>' % 'unknown'
        pass
    if ic == 0: s += '<tr><td><b>No files found on ATLCAL.</b><br>&nbsp;<br>Note that the accounting of newly replicated datasets may take up to 24h (which adds to the production and replication delay). </td></tr>' 
    else:       s += '<tr><td>Number of files found: <b>%i</b></td></tr>' % ic
    
    if ic>0:
        if len(paths)==1:
            s += '<tr><td>Common path for all files:<br> <font color="#AA0000">%s</font><br><hr color="gray" size=1></td></tr>' % refpath
        else:
            s += '<tr><td>Common path for the following files if not indicated otherwise:<br> <font color="#AA0000">%s</font><br><hr color="gray" size=1></td></tr>' % refpath

        # add the content
        s += content

    # end of page
    s += '<tr><td><hr color="red" size=1><font color="#777777"><font size="-1"><i><font size="-2">Created by AtlRunQuery on: %s</font></i></font></td></tr>' % str(datetime.datetime.now())
    s += '</table></body></html>'

    os.system("echo '%s' > data/%s.html" % (s, searchstr))
