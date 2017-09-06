# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import AtlasStyle
import ami
import sys
import datetime

c1 = ROOT.TCanvas('c1', '', 400, 400)

location = '/disk/homedisk/home/sjh2/public_web/twiki/'
derivationTag = 'p1851'

def printTable(out, stream, quantity, topq1Only=False):
    toGB = 1.
    if quantity == 'totalSize':
        toGB = 1. / 1000. / 1000. / 1000.

    primaryxAOD = ami.askAmi('data12_8TeV.*' + stream + '.merge.AOD.r572*_p1751_p2309', quantity)
    topq1_round1 = ami.askAmi('data12_8TeV.*' + stream + '*DAOD_TOPQ1*' + derivationTag, quantity)

    topq2_round1 = {}
    if not topq1Only:
        topq2_round1 = ami.askAmi('data12_8TeV.*' + stream + '*DAOD_TOPQ2*' + derivationTag, quantity)

    h_topq1_frac = ROOT.TH1D('h_topq1_frac', ';TOPQ1 Size / Primary Size [%];Number of datasets', 20, 0, 10)
    h_topq2_frac = ROOT.TH1D('h_topq2_frac', ';TOPQ2 Size / Primary Size [%];Number of datasets', 20, 0, 1)

    h_topq1_abs = ROOT.TH1D('h_topq1_abs', ';TOPQ1 Size [GB];Number of datasets', 20, 0, 200)
    h_topq2_abs = ROOT.TH1D('h_topq2_abs', ';TOPQ2 Size [GB];Number of datasets', 30, 0, 15)

    out.write('---+ !%s\n' % stream)
    if not topq1Only:
        out.write('|  *%s*  |  *%s*  |  *%s*  |  *%s*  |  *%s*  |  *%s*  |\n' % ('Primary xAOD name', 'Size [GB]', 'TOPQ1 size [GB]', '%', 'TOPQ2 size [GB]', '%'))
    else:
        out.write('|  *%s*  |  *%s*  |  *%s*  |  *%s*  |\n' % ('Primary xAOD name', 'Size [GB]', 'TOPQ1 size [GB]', '%'))

    tot_prim = 0
    tot_topq1 = 0
    tot_topq2 = 0
    for k in sorted(primaryxAOD.keys()):
        primaryNum = primaryxAOD[k] * toGB

        topq1_name = k.replace('AOD', 'DAOD_TOPQ1') + '_' + derivationTag
        topq1Num = -1
        if topq1_round1.has_key(topq1_name):
            topq1Num = topq1_round1[topq1_name] * toGB

        topq2_name = k.replace('AOD', 'DAOD_TOPQ2') + '_' + derivationTag
        topq2Num = -1
        if topq2_round1.has_key(topq2_name):
            topq2Num = topq2_round1[topq2_name] * toGB

        note = ''

        cstart = '%RED%'
        cend = "%ENDCOLOR%"

        topq1Frac = 0
        topq2Frac = 0
        if topq1Num > 0:
            tot_prim += primaryNum
            tot_topq1 += topq1Num
            topq1Frac = 100. * topq1Num / primaryNum
            h_topq1_frac.Fill(topq1Frac)
            h_topq1_abs.Fill(topq1Num)
            cstart= ''
            cend = ''
            topq1Num = cstart + ('%4.2f' % topq1Num) + cend
            topq1Frac = cstart + ('%4.2f' % topq1Frac) + cend
        else:
            topq1Num = cstart + '-' + cend
            topq1Frac = cstart + '-' + cend

        if topq2Num > 0:
            tot_topq2 += topq2Num
            topq2Frac = 100. * topq2Num / primaryNum
            h_topq2_frac.Fill(topq2Frac)
            h_topq2_abs.Fill(topq2Num)
            cstart = ''
            cend = ''
            topq2Num = cstart + ('%4.2f' % topq2Num) + cend
            topq2Frac = cstart + ('%4.2f' % topq2Frac) + cend
        else:
            topq2Num = cstart + '-' + cend
            topq2Frac = cstart + '-' + cend

        k = cstart + k + cend
        primaryNum = cstart + ('%4.2f' % primaryNum) + cend

        if not topq1Only:
            txt = '| %s |  %s |  %s |  %s |  %s |  %s |\n' % (k, primaryNum, topq1Num, topq1Frac, topq2Num, topq2Frac)
        else:
            txt = '| %s |  %s |  %s |  %s |\n' % (k, primaryNum, topq1Num, topq1Frac)

        out.write(txt)

    if not topq1Only:
        txt = '| total (only include primary that have derivation) |  %4.2f |  %4.2f |  %4.2f |  %4.2f |  %4.2f |\n' % (tot_prim, tot_topq1, 100. * tot_topq1 / tot_prim, tot_topq2, 100. * tot_topq2 / tot_prim)
    else:
        txt = '| total (only include primary that have derivation) |  %4.2f |  %4.2f |  %4.2f |\n' % (tot_prim, tot_topq1, 100. * tot_topq1 / tot_prim)

    out.write(txt)
    out.write('\n')

    name = stream + '_topq1_abs.gif'
    h_topq1_abs.SetFillColor(ROOT.kRed)
    h_topq1_abs.Draw('hist')
    c1.Print(location + name)
    out.write('http://epweb2.ph.bham.ac.uk/user/head/twiki/%s\n' % name)

    name = stream + '_topq1_frac.gif'
    h_topq1_frac.SetFillColor(ROOT.kRed)
    h_topq1_frac.Draw('hist')
    c1.Print(location + name)
    out.write('http://epweb2.ph.bham.ac.uk/user/head/twiki/%s\n' % name)

    if not topq1Only:
        name = stream + '_topq2_abs.gif'
        h_topq2_abs.SetFillColor(ROOT.kRed)
        h_topq2_abs.Draw('hist')
        c1.Print(location + name)
        out.write('http://epweb2.ph.bham.ac.uk/user/head/twiki/%s\n' % name)

        name = stream + '_topq2_frac.gif'
        h_topq2_frac.SetFillColor(ROOT.kRed)
        h_topq2_frac.Draw('hist')
        c1.Print(location + name)
        out.write('http://epweb2.ph.bham.ac.uk/user/head/twiki/%s\n' % name)

if __name__ == "__main__":
    qunatityFromAmi = 'totalEvents'
    qunatityFromAmi = 'totalSize'

    out = open(location + 'derivation_progress.html', 'w')
    out.write('%TOC%\n')

    d_string = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    out.write('---+ Introduction\n')
    txt = '''
    Generated: %s
    
    Hello, this page lists the most recent round of top derivations for DC14 (2012 Period B, about 5 fb-1).
    
    For more information on which containers are kept and the cuts that are applied in TOPQ1, and TOPQ2, see TopDerivations)
    
    The p-tag used for this page is %s
    
    You can find derivations on rucio with:
    =rucio list-dids data12_8TeV:*DAOD_TOPQ1*p1851= (or =TOPQ2=)
    
    ''' % (d_string, derivationTag)
    out.write(txt + '\n')

    printTable(out, 'Egamma', qunatityFromAmi) 
    printTable(out, 'Muons', qunatityFromAmi)
    printTable(out, 'JetTauEtmiss', qunatityFromAmi, True)
    out.close()
