# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
import sys

import grid
import ami

toGB = 1. / 1000. / 1000. / 1000.

def makeDict(scope, query):
    map = {}
    map2 = {}
    #print scope, query
    for ds in grid.listDatasets(scope, query):
        ar = ds.replace('/','').split('.')
        tag = ar[5]

        if tag.find('_p'):
            tag = tag.split('_p')[0]

        short = ar[1] + '.' + tag
        map[short] = ds
        map2[int(ar[1])] = ds

    return map, map2

def makeTable(name, ids):

    print '---++ %s (%d)' % (name, len(ids))

    print '''%TABLE{tablewidth="100%" columnwidths="6%,70%,12%,12%"}%'''
    print '|  %8s  |  %60s  |  %22s  |  %22s  |  %22s  |  %22s  |' % ( '*DSID*', '*Description*', '*Primary xAOD  yield*', '*TOPQ1 yield*', '*Primary size [GB]*', '*xAOD size [GB]*' )

    for id in ids:
        dsids_sofar.append(id)

        primary_name = ''
        topq1_name = 'Not Available (requested?)'
        aod_yield = 0
        topq1_yield = 0
        aod_size = 0
        topq1_size = 0

        if aods_idonly.has_key(id):
            primary_name = aods_idonly[id]

            if topq1_idonly.has_key(id):
                topq1_name = topq1_idonly[id]

            if aods_ami.has_key(primary_name.replace('/','')):
                aod_yield = aods_ami[primary_name.replace('/','')]

            if topq1_ami.has_key(topq1_name.replace('/','')):
                topq1_yield = topq1_ami[topq1_name.replace('/','')]


            if aods_ami_size.has_key(primary_name.replace('/','')):
                aod_size = aods_ami_size[primary_name.replace('/','')]

            if topq1_ami_size.has_key(topq1_name.replace('/','')):
                topq1_size = topq1_ami_size[topq1_name.replace('/','')]


        #print id, primary_name
        desc = primary_name.split('.')[2]

        #%TWISTY{mode="div"showlink="AOD: 6.298.988"hidelink="Collapse"showimgleft="%ICONURLPATH{toggleopen-small}%"hideimgleft="%ICONURLPATH{toggleclose-small}%"}%INPUT%ICON{choice-no}%

        twisty = '''%TWISTY{
            mode="div"
            showlink="'''+desc+'''"
            hidelink="'''+desc+'''"
            showimgleft="%ICONURLPATH{toggleopen-small}%"
            hideimgleft="%ICONURLPATH{toggleclose-small}%"}%''' + '<b>Primary:</b> ' + primary_name + '<br><b>TOPQ1:</b> ' + topq1_name + '<br>' + '%ENDTWISTY%'

        frac = 0
        if aod_yield > 0:
            frac = 100. * topq1_yield / aod_yield

        topq1_yield_txt = "%15d (%0.0f%%)" % (topq1_yield, frac)
        cstart = ''
        cend = ''
        if aod_yield < topq1_yield:
            cstart = '%RED%'
            cend = "%ENDCOLOR%"
        elif aod_yield > topq1_yield:
            cstart = '%ORANGE%'
            cend = "%ENDCOLOR%"

        topq1_yield_txt = cstart + topq1_yield_txt + cend

        frac = 0
        if topq1_size > 0:
            frac = 100. * topq1_size / aod_size

        topq1_size_txt = "%15d (%0.0f%%)" % (topq1_size * toGB, frac)

        print '|  %d|%s|  %d|  %s|  %4.2f|  %s|' % (id, twisty, aod_yield, topq1_yield_txt, aod_size * toGB, topq1_size_txt)

    print ''

if __name__ == '__main__':
    query = '*.AOD.*_r5591_r5625'
    query13 = '*.merge.AOD.*_r5787_r5853' 

    print '%TOC%'

    print '''
On this page we have listed the main DC14 samples.  You can find more details on
the DC14StartPage and DC14DataMCSampleInfo.
 
For 8 !TeV we list
   * *Primary xAOD:* =%s=
   * *TOPQ1:* =%s=

For 13 !TeV we list
   * *Primary xAOD:* =%s=
   * *TOPQ1:* =%s=
 
For more details on which containers are removed see TopDerivations. 
No event selection is currently applied to MC in TOPQ1.
Since the only difference between TOPQ1 and TOPQ2 is the event selection we
don't make TOPQ2 for MC. 
''' % (query, query.replace('AOD','DAOD_TOPQ1') + '_p1854', query13, query13.replace('AOD','DAOD_TOPQ1') + '_p1852')

    print '---+ Introduction'

    aods_idtag, aods_idonly = makeDict('mc14_8TeV', query)
    topq1_idtag, topq1_idonly = makeDict('mc14_8TeV', query.replace('AOD','DAOD_TOPQ1') + '_p1854')
    aods_ami = ami.askAmi('mc14_8TeV.' + query)
    topq1_ami = ami.askAmi('mc14_8TeV.' + query.replace('AOD','DAOD_TOPQ1') + '_p1854')

    aods_ami_size = ami.askAmi('mc14_8TeV.' + query, 'totalSize')
    topq1_ami_size = ami.askAmi('mc14_8TeV.' + query.replace('AOD','DAOD_TOPQ1') + '_p1854', 'totalSize')

    #print len(aods_ami), len(topq1_ami)
    #sys.exit()
    dsids_sofar = []

    print '---+ 8 !TeV'

    makeTable('ttbar', [117050])
    makeTable('single top', [110101, 110119, 110140])
    makeTable('Z+jets sherpa', [147770, 147771])
    makeTable('Z+jets sherpa with massive b/c', range(167749, 167758) + range(167797, 167806) + range(167809, 167818) + range(167821, 167830) + range(167833, 167842))
    makeTable('W+jets sherpa with massive b/c', range(167740, 167750) + range(180534, 180543) + range(167761, 167770) + range(167770, 167779) + range(167779, 167788) + range(167788, 167797))
    makeTable('Diboson powheg+pythia8', range(126928, 126937))

    remaining = set(aods_idonly.keys()).difference(set(dsids_sofar))
    makeTable('Remaining / uncategorised (as of now)', sorted(remaining))


    ####13 TEV
    query = query13
    aods_idtag, aods_idonly = makeDict('mc14_13TeV', query)
    topq1_idtag, topq1_idonly = makeDict('mc14_13TeV', query.replace('AOD','DAOD_TOPQ1') + '_p1852')

    aods_ami = ami.askAmi('mc14_13TeV.' + query)
    topq1_ami = ami.askAmi('mc14_13TeV.' + query.replace('AOD','DAOD_TOPQ1') + '_p1852')
    aods_ami_size = ami.askAmi('mc14_13TeV.' + query, 'totalSize')
    topq1_ami_size = ami.askAmi('mc14_13TeV.' + query.replace('AOD','DAOD_TOPQ1') + '_p1852', 'totalSize')
    
    dsids_sofar = []

    print '---+ 13 !TeV'

    makeTable('ttbar', [110401])
    makeTable('single top', [110070, 110071, 110305])

    remaining = set(aods_idonly.keys()).difference(set(dsids_sofar))
    makeTable('Remaining / uncategorised (as of now)', sorted(remaining))

