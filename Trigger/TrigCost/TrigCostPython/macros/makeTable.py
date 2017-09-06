# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os, sys, string, math, shutil
import xml.dom.minidom

#---------------------------------------------------------------------
# Object to store rates
class chain:
    def __init__(self):
        pass

    def Print(self):
        print 'name =',self.name
        print '   ps    =',self.ps
        print '   pt    =',self.pt
        print '   nevt  =',self.nevt
        print '   rate  =',self.rate
        print '   rerr  =',self.rerr
        print '   eff   =',self.eff
        print '   pseff =',self.pseff
        
    name      = ""
    ps        = -9999
    pt        = -9999
    nevt      = -9999
    rate      = -9999
    rerr      = -9999
    eff       = -9999
    pseff     = -9999
    
#---------------------------------------------------------------------
def ReadXmlFile(filename):

    def GetData(nodelist, name):

        for val in nodelist:
            if val.nodeName != name:
                continue

            svalue = ""
            icount = int(0)
            for s in val.childNodes:
                icount = icount+1
                svalue = s.data
                
            if icount == 1:
                return svalue.strip()
            else:
                raise Exception('Wrong child list')
            
        raise Exception('Failed to find data')

    # create and initialize parser
    myf = open(filename)
    dom = xml.dom.minidom.parse(myf)    

    #copy results to chain objects
    rateDict = {}

    for level in dom.getElementsByTagName("level"):
        for sig in level.getElementsByTagName("signature"):
            ch=chain()            

            ch.name    = GetData(sig.childNodes, "sig_name")
            ch.ps      = string.atof(GetData(sig.childNodes, 'prescale'))
            ch.pt      = string.atof(GetData(sig.childNodes, 'passthrough'))
            ch.nevt    = string.atof(GetData(sig.childNodes, 'evts_passed'))
            ch.rate    = string.atof(GetData(sig.childNodes, 'rate'))
            ch.rerr    = string.atof(GetData(sig.childNodes, 'rate_err'))
            ch.eff     = string.atof(GetData(sig.childNodes, 'efficiency'))
            ch.pseff   = string.atof(GetData(sig.childNodes, 'prescaled_efficiency'))
            
            rateDict[ch.name]=ch
            print ch.name,'=',ch.rate,'Hz',ch.nevt,'events'
            
    return rateDict

#---------------------------------------------------------------------
# Compare two results
def fillBlanks(var, nblanks):
    rstr = str(var)
    for i in range(len(str(var)), nblanks+1):
        rstr += ' '

    return rstr
    
#---------------------------------------------------------------------
# Compare two results
def CompareRates(opath, level, varname, rates, chains):

    ofile = open(opath+'compare_'+varname+'_'+level+'.txt', 'w')

    iwidth = 0
    vwidth = 0
    chainS = []
    
    for rate in rates:        
        for chain in rate.values():
            
            if len(chain.name) < 2 or chain.name[0:2] != level:
                continue

            if chains.count(chain.name) < 1:
                continue

            chainS.append(chain.name)
            iwidth = max([iwidth, len(chain.name)])            
            vwidth = max([vwidth, len(str(getattr(chain, varname)))])

    rateTable = []
    chainU = set(chainS)

    for chain in chainU:

        lineS  = ''
        lineS += fillBlanks(chain, iwidth+1)
        sortV  = -1 

        for rate in rates:
            value = 0
            if chain in rate.keys():
                value = getattr(rate[chain], varname)
                

            if sortV < 0:
                sortV = value
                
            lineS += ' & '+fillBlanks(value, vwidth+1)

        lineS += '\\\\ \n'
        rateTable += [ [ sortV, lineS] ]

        print lineS.rstrip()

    rateTable.sort(lambda x, y: cmp(y[0], x[0]))

    for iline in rateTable:
        ofile.write(iline[1])
        
    ofile.close()                 

#---------------------------------------------------------------------        
# Read XML files using function above
if len(sys.argv) < 2:
    print 'Wrong number of parameters:',len(sys.argv)
    sys.exit(1)

rates = []

for i in range(1, len(sys.argv)):
    xfile = sys.argv[i]
    xrate = ReadXmlFile(xfile)
    print 'Reading xml file:',xfile

    rates.append(xrate)
    
gpath = "/afs/cern.ch/user/r/rustem/www/debug/2009-11/comp_rates/"

if not os.path.isdir(gpath):
    print 'Output directory does not exist:'+gpath
    sys.exit(1)

levels = ['L1']

chains  = []
chains += ['L1_EM3', 'L1_EM5', 'L1_EM10', 'L1_2EM3']
chains += ['L1_J5', 'L1_J10', 'L1_J15', 'L1_2J10']
chains += ['L1_XE10', 'L1_XE15']
chains += ['L1_TAU5', 'L1_TAU6', 'L1_TAU6I', 'L1_TAU6_XE10']
chains += ['L1_MU0', 'L1_MU10']

for level in levels:
    
    CompareRates(gpath, level, 'rate', rates, chains)
    CompareRates(gpath, level, 'nevt', rates, chains)
