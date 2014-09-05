# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import makePieCharts
    
def AddPieChart(loc):

    # Make Pie Charts
    charts = makePieCharts.MakePieCharts('%s/TriggerCosts.xml' %loc, 'BW', output=loc) 

    # Select Charts to add to HTML
    charts_keep = []
    for ch in charts :
        if ch.GetName().find('rates') != -1 :
            charts_keep.append(ch)

    # Copy index file to tmp location
    os.system('mv %s/index.html %s/index.html.tmp' %(loc, loc))

    old_index = open('%s/index.html.tmp' %loc, 'r')
    new_index = open('%s/index.html' %loc, 'w')

    # Copy tmp to index line-by-line inserting pie charts in the right place
    for line in old_index :
        new_index.write(line)
        if line.find('TriggerCosts.xml') != -1 :
            new_index.write('\n')
            new_index.write('<table>\n<tr>\n')
            for ch in charts_keep :
                new_index.write('<td><a STYLE="text-decoration:none" href="%s.png"><IMG height=180 width=220 src="%s.png"></a></td>\n' %(ch.GetName(), ch.GetName()))
            new_index.write('</tr>\n<tr>\n')
            for ch in charts_keep :
                new_index.write('<td align="center"><a STYLE="text-decoration:none"  href="%s.png">%s</a></td>' \
                         %(ch.GetName(), ' '.join(ch.GetName().split('_')[0:-1])))

            new_index.write('</tr>\n</table>\n')
            new_index.write(line)


    old_index.close()
    new_index.close()
    os.system('rm %s/index.html.tmp' %loc)
