#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###----------------------------------------------------------------------------------
### Process options
###
from optparse import OptionParser

p = OptionParser(usage="usage: -r run number", version="1.0")

p.add_option('-r',         type="int",          default= 0,    dest="run",      help="run number")
p.add_option('--lb-beg',   type="int",          default=-1,    dest="lbbeg",    help="start lumi block")
p.add_option('--lb-end',   type="int",          default=-1,    dest="lbend",    help="end lumi block")
p.add_option('--ctpid',    type="int",          default=-1,    dest="ctpid",    help="L1 ctp id")

p.add_option('--ctpname',  type="string",       default="",    dest="ctpname",  help="L1 item name")
p.add_option('--xmlfile',  type="string",       default="",    dest="xmlfile",  help="XML file path")
p.add_option('--lv1-bit',  type="string",       default="TAV", dest="lv1_bit",  help="XML file path")

p.add_option('-d',          action="store_true", default=False, dest="debug",    help="print debug messages")
p.add_option('--fillhist',  action="store_true", default=False, dest="fillhist", help="fill histograms")
p.add_option('--printL1A',  action="store_true", default=False, dest="printL1A", help="print L1 global counters per lumi block")

(opts, args) = p.parse_args()

import sys
import os
import logging
import math

from PyCool import cool
from PyCool import coral
from CoolConvUtilities.AtlCoolLib import indirectOpen

if opts.debug:
    logging.basicConfig(level=logging.DEBUG)
else:
    logging.basicConfig(level=logging.INFO)

log = logging.getLogger('CoolRates')

if(opts.run < 1): 
    log.warning('Need valid run number')
    sys.exit(2)

log.info('Processing run '+str(opts.run)+' lb beg='+str(opts.lbbeg)+' to lb end='+str(opts.lbend))

###----------------------------------------------------------------------------------
### Check database service and folders
###

dbSvc=cool.DatabaseSvcFactory.databaseService()
dbTrigString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"
dbTdaqString="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO"

try:
    dbTrig=dbSvc.openDatabase(dbTrigString, False)
    log.info("OpenRed database: "+dbTrigString)
except Exception,e:
    log.error('Error opening database:'+str(e))
    sys.exit(-1)

lvl1counters_foldername = '/TRIGGER/LUMI/LVL1COUNTERS'
lblb_foldername         = '/TRIGGER/LUMI/LBLB'
lvl1menu_foldername     = '/TRIGGER/LVL1/Menu'
lvl1lbdata_foldername   = '/TRIGGER/LVL1/CTPCORELBDATA'
l1ps_fname              = '/TRIGGER/LVL1/Prescales'
lumi_foldername         = '/TDAQ/OLC/LUMINOSITY'

if not dbTrig.existsFolder(lvl1counters_foldername) :
    print "Folder",lvl1counters_foldername,"not found"
    dbTrig.closeDatabase()
    sys.exit(-1)
if not dbTrig.existsFolder(lvl1menu_foldername) :
    print "Folder",lvl1menu_foldername,"not found"
    dbTrig.closeDatabase()
    sys.exit(-1)
if not dbTrig.existsFolder(lvl1lbdata_foldername) :
    print "Folder",lvl1lbdata_foldername,"not found"
    dbTrig.closeDatabase()
    sys.exit(-1)
if not dbTrig.existsFolder(lblb_foldername) :
    print "Folder",lblb_foldername,"not found"
    dbTrig.closeDatabase()
    sys.exit(-1)
if not dbTrig.existsFolder(l1ps_fname) :
    print "Folder",l1ps_fname,"not found"
    dbTrig.closeDatabase()
    sys.exit(-1)

###----------------------------------------------------------------------------------
### Data classes
###
orbitFreq  = 40.0790e6 / 3564.0

class L1Data:
    def __init__(self):
        pass

    lb         = -1
    ctpid      = -1
    ps         = 0.0    
    name       = "unknown"
    tav        = long(0)
    tav_scaled = long(0)
    tap        = long(0)
    tbp        = long(0)
    tav_over   = False
    tap_over   = False
    tbp_over   = False

    def AddData(self, rhs):
        if not hasattr(rhs, 'tav') or not hasattr(rhs, 'tap') or not hasattr(rhs, 'tbp'):
            log.warning('Wrong rhs in AddData function')
            return        
        if self.ctpid != rhs.ctpid:
            log.warning('Wrong CTP ids in AddData function: '+str(self.ctpid)+'!='+str(rhs.ctpid))
            return
        
        self.lb   = -1        
        self.tav += rhs.tav
        self.tap += rhs.tap
        self.tbp += rhs.tbp
        
        if rhs.ps > 0:
            self.tav_scaled += rhs.tav*ps

    def getRate_TAV(self):
        turns_tav = getLB_TAV(self.lb)
        if not turns_tav > 0:
            return 0.0
        return orbitFreq*self.tav/turns_tav

    def getRateErr_TAV(self):
        turns_tav = getLB_TAV(self.lb)
        if not turns_tav > 0:
            return 0.0
        return orbitFreq*math.sqrt(self.tav)/turns_tav

    def getRate_TAV_scaled(self):
        turns_tav = getLB_TAV(self.lb)
        if not turns_tav > 0:
            return 0.0
        return orbitFreq*self.tav_scaled/turns_tav

    def getRateErr_TAV_scaled(self):
        turns_tav = getLB_TAV(self.lb)
        if not turns_tav > 0:
            return 0.0
        return orbitFreq*math.sqrt(self.tav_scaled)/turns_tav

    def getRate_TAP(self):
        turns_tap = getLB_TAP(self.lb)
        if not turns_tap > 0:
            return 0.0
        return orbitFreq*self.tap/turns_tap

    def getRate_TBP(self):
        turns_tbp = getLB_TBP(self.lb)
        if not turns_tbp > 0:
            return 0.0
        return orbitFreq*self.tbp/turns_tbp

    def getRate_TBP_scaled(self):
        turns_tbp = getLB_TBP(self.lb)
        if not turns_tbp > 0:
            return 0.0
        return orbitFreq*self.tbp/turns_tbp

    def getRateErr_TBP_scaled(self):
        turns_tbp = getLB_TBP(self.lb)
        if not turns_tbp > 0:
            return 0.0
        return orbitFreq*math.sqrt(self.tbp)/turns_tbp
    
class LumiBlock:
    def __init__(self):
        pass

    lb        = -1
    beg_time  = 0
    end_time  = 0
    l1_accept = 0    
    l1a_turn  = 0
    tav_turn  = 0
    tap_turn  = 0
    tbp_turn  = 0      

    avr_lumi  = 0 # average instantenous lumi:       LBAvInstLumi 
    avr_evts  = 0 # average number of pp collisions: LBAvEvtsPerBX
    
    def getDT(self):
        return (self.end_time-self.beg_time)/1.0E9

    def getIntLumi(self):
        return self.avr_lumi*self.tav_turn/orbitFreq

    def getDeliveredLumi(self):
        return self.avr_lumi*self.getDT()

    def AsString(self):
        from CoolConvUtilities import AtlCoolTool
        beg_ = AtlCoolTool.time.ctime(self.beg_time/1.0E9)
        end_ = AtlCoolTool.time.ctime(self.end_time/1.0E9)
        lumi = self.getIntLumi()
        
        rval = 'lb %4d'%self.lb+' '+beg_+' '+end_+'\n'\
               '   l1_accept=%d'%self.l1_accept+'\n'+\
               '   l1a_turn =%d'%self.l1a_turn +'\n'+\
               '   tav_turn =%d'%self.tav_turn +'\n'+\
               '   tap_turn =%d'%self.tap_turn +'\n'+\
               '   tbp_turn =%d'%self.tbp_turn +'\n'+\
               '   avr_lumi =%f'%self.avr_lumi +'\n'+\
               '   avr_evts =%f'%self.avr_evts +'\n'+\
               '   int_lumi =%f'%lumi +'\n'
        
        return rval
    
###----------------------------------------------------------------------------------
### Data objects
###
ctpMap = {} # map from ctp id to L1 item name
lumis  = {} # Luminosity block start, stop times
rates  = {} # L1 rates by lumi block and item id

def addNewRateEntry(lb_, id_):
    if lb_ < 0 or id_ < 0:
        log.error('Bad lb and/or id values: lb, id= '+str(lb_)+', '+str(id_))
        return

    if lb_ not in rates.keys():
        rates[lb_] = {}
        log.debug('Adding new lumi block to rates dict: '+str(lb_))
        
    if id_ not in rates[lb_].keys():
        rates[lb_][id_]       = L1Data()
        rates[lb_][id_].lb    = lb_
        rates[lb_][id_].ctpid = id_
        log.debug('Adding new itemNo '+str(id_)+' to rates dict for lb: '+str(lb_))

    return rates[lb_][id_]

def getLBPeriod(lb_):
    if lb_ not in lumis:
        log.warning('getLBPeriod: unknown lb=%d'%lb_)
        return 120.0
      
    return (lumis[lb_].end_time-lumis[lb_].beg_time)/1.0E9

def countLB_Attr(attr):
    count = 0
    for lumi in lumis.itervalues():
        if type(attr) == str and hasattr(lumi, attr):
            count += getattr(lumi, attr)            
    return count

def getLB_L1A(lb_):
    if lb_ < 0:        
        return countLB_Attr('l1a_turn')
    elif lb_ not in lumis:
        return 0
    return lumis[lb_].l1a_turn

def getLB_TAV(lb_):
    if lb_ < 0:
        return countLB_Attr('tav_turn')
    elif lb_ not in lumis:
        return 0
    return lumis[lb_].tav_turn

def getLB_TAP(lb_):
    if lb_ < 0:
        return countLB_Attr('tap_turn')
    elif lb_ not in lumis:
        return 0
    return lumis[lb_].tap_turn

def getLB_TBP(lb_):
    if lb_ < 0:
        return countLB_Attr('tbp_turn')
    elif lb_ not in lumis:
        return 0
    return lumis[lb_].tbp_turn

def getIdFromName(name_):
    for ctpId, ctpName in ctpMap.iteritems():
        if ctpName == name_:
            return ctpId
    return -1

def IncludeLB(lb):
    if lb < 0:
        return False
    if opts.lbbeg >= 0 and lb < opts.lbbeg:
        return False
    if opts.lbend >= 0 and lb > opts.lbend:
        return False
    return True

def PrintItemCounts(id_):
    if id_ < 0 or id_ > 254:
        return

    count_tav = 0
    print_ctp = ' ctpid %d'%id_+' '+ctpMap[id_]
    
    for lb, rds in rates.iteritems():
        if id_ not in rds: continue

        tav = rds[id_].tav
        tbp = rds[id_].tbp
        count_tav += tav
        
        log.info(print_ctp+' lb %3d'%lb+' tav %6d'%tav+' rate %5.1f'%rds[id_].getRate_TAV())
            
    print log.info(print_ctp+' TAV count=%d'%count_tav)

###----------------------------------------------------------------------------------
### Read map: ctp id to name
###
folder=dbTrig.getFolder(lvl1menu_foldername)
try:
    itr=folder.browseObjects(opts.run << 32, (opts.run+1) << 32, cool.ChannelSelection.all())
    while itr.goToNext() :
        obj            = itr.currentRef()
        itemNo         = int(obj.channelId())
        payload        = obj.payload()
        ctpMap[itemNo] = payload['ItemName']
        
        log.debug('ctpId=%3d' %itemNo+ ': ' + ctpMap[itemNo])
        
    itr.close()
except Exception,e:
    log.error('Reading data from '+lvl1menu_foldername,' failed: '+str(e))

###----------------------------------------------------------------------------------
### Read global counters
###
folder=dbTrig.getFolder(lvl1lbdata_foldername)
try:
    itr=folder.browseObjects(opts.run << 32, (opts.run+1) << 32, cool.ChannelSelection.all())

    lb = 0
    while itr.goToNext() :
        obj     = itr.currentRef()
        itemNo  = obj.channelId()
        payload = obj.payload()

        if not IncludeLB(lb):
            lb += 1            
            continue

        if lb not in lumis:
            lumis[lb] = LumiBlock()
        
        lbData           = lumis[lb]
        lbData.lb        = lb
        lbData.l1_accept = payload['GlobalL1AcceptCounter']
        lbData.l1a_turn  = payload['GlobalL1AcceptTurnCounter']
        lbData.tav_turn  = payload['L1AcceptTurnCounter']
        lbData.tap_turn  = payload['AfterPrescaleTurnCounter']
        lbData.tbp_turn  = payload['BeforePrescaleTurnCounter']        
        lb += 1

        if opts.printL1A:
            log.info('lumi block %4d'%lbData.lb+' global L1 accept counter: %d'%lbData.l1_accept)
                
    del lb
    itr.close()
except Exception,e:
    log.error('Reading data from'+lvl1lbdata_foldername+'failed:'+str(e))

###----------------------------------------------------------------------------------
### Read luminosity block information
###
run_beg_time = -1
run_end_time = -1

folder=dbTrig.getFolder(lblb_foldername)
try:
    itr=folder.browseObjects(opts.run << 32, (opts.run+1) << 32, cool.ChannelSelection.all())

    lb = 0
    while itr.goToNext() :
        obj     = itr.currentRef()
        payload = obj.payload()

        if not IncludeLB(lb):
            lb += 1            
            continue

        if lb not in lumis:
            lumis[lb] = LumiBlock()
            
        lbData          = lumis[lb]
        lbData.beg_time = payload['StartTime']
        lbData.end_time = payload['EndTime']
        lbData.lb       = lb
        lb += 1
        
        if run_beg_time < 0 or lbData.beg_time < run_beg_time:
            run_beg_time = lbData.beg_time
            
        if run_end_time < 0 or lbData.end_time > run_end_time:
            run_end_time = lbData.end_time    

    del lb
    itr.close()
except Exception,e:
    log.error('Reading data from '+lvl1lbdata_foldername+' failed: '+str(e))


###----------------------------------------------------------------------------------
### Read luminosity information
###
try:
    #dbTdaq=dbSvc.openDatabase(dbTdaqString, False)
    dbTdaq = indirectOpen('COOLONL_TDAQ/COMP200', True, False, False)
 
    log.info("Opened database: "+dbTdaqString)
except Exception,e:
    log.error('Error opening database:'+str(e))
    sys.exit(-1)
    
if not dbTdaq.existsFolder(lumi_foldername) :
    print "Folder",lumi_foldername,"not found"
    dbTaq.closeDatabase()
    sys.exit(-1)
    
try:
    folder=dbTdaq.getFolder(lumi_foldername)
    itr=folder.browseObjects(run_beg_time, run_end_time, cool.ChannelSelection(0))    
    while itr.goToNext():
        obj     = itr.currentRef()
        payload = obj.payload()

        run = payload['RunLB'] >> 32
        lb  = payload['RunLB'] & 0xffff

        if run != opts.run:
            log.warning('For lumi folder run mismatch: '+str(run))
            continue

        if lb not in lumis:
            log.warning('Found lumi for missing lumi block:'+str(lb))
            lumis[lb] = LumiBlock()
        
        lumis[lb].avr_lumi = payload['LBAvInstLumPhys']
        lumis[lb].avr_evts = payload['LBAvEvtsPerBXPhys']
        
        log.debug('New lumi block: %d'%lb+lumis[lb].AsString())
        
    itr.close()
except Exception,e:
    log.error('Reading data from '+lvl1lbdata_foldername+' failed: '+str(e))    

###----------------------------------------------------------------------------------
### Read L1 counters
###
folder=dbTrig.getFolder(lvl1counters_foldername)
try:
    counter =  0
    oldItem = -1
    itr=folder.browseObjects(opts.run << 32,(opts.run+1) << 32,cool.ChannelSelection.all())
    while itr.goToNext():
        obj     = itr.currentRef()
        lb      = (obj.since() & 0xffff)
        itemNo  = obj.channelId()
        payload = obj.payload()
        
        if oldItem != itemNo:
            counter = 0
            oldItem = itemNo            
        else:
            counter += 1            

        if not IncludeLB(counter):
            continue

        entry          = addNewRateEntry(counter, itemNo)
        entry.tav      = int(payload['L1Accept'])
        entry.tbp      = int(payload['BeforePrescale'])
        entry.tap      = int(payload['AfterPrescale'])
        entry.tav_over = bool(payload['L1AcceptOverflow'])
        entry.tbp_over = bool(payload['BeforePrescaleOverflow'])
        entry.tap_over = bool(payload['AfterPrescaleOverflow'])
        
        if itemNo in ctpMap:
            entry.name = ctpMap[itemNo]
        else:
            entry.name = 'ctpid_%03d'%itemNo

    del counter
    del oldItem
    itr.close()
except Exception,e:
    print "Reading data from",lvl1lbdata_foldername,"failed:",e

###----------------------------------------------------------------------------------
### Read prescale data for lumi blocks which have counters in database
###
folder=dbTrig.getFolder(l1ps_fname)
try:
    itr=folder.browseObjects(opts.run << 32, (opts.run+1) << 32, cool.ChannelSelection.all())
    pssets = {} #indexed on lb (and then itemNo)
    while itr.goToNext():
        obj     = itr.currentRef()
        lb      = (obj.since() & 0xffff)
        itemNo  = obj.channelId()
        payload = obj.payload()
        ps      = payload['Lvl1Prescale']
        
        if not pssets.has_key(lb):
            pssets[lb] = {}
        
        if pssets[lb].has_key(itemNo):
            log.warning("Double setting of prescale for lb="+str(lb))
            
        pssets[lb][itemNo] = ps

    cur_ps_idx=0
    for lb, vals in rates.iteritems():
        if cur_ps_idx+1 < len(pssets.keys()):
            if lb >= pssets.keys()[cur_ps_idx+1]:
                cur_ps_idx+=1
        for itemNo, data in vals.iteritems():
            data.ps = pssets.values()[cur_ps_idx][itemNo]
            log.debug('Setting PS lb='+str(lb)+' name='+data.name+' itemNo='+
                      str(itemNo)+' ctpId='+str(data.ctpid)+
                      ' PS='+str(data.ps))
        
except Exception,e:
    log.error('Reading data from '+l1ps_fname+' failed: '+str(e))

dbTrig.closeDatabase()


###----------------------------------------------------------------------------------
### Print counts for single item
###
PrintItemCounts(opts.ctpid)
PrintItemCounts(getIdFromName(opts.ctpname))

if opts.fillhist:
    from ROOT import TH1F, TFile, TProfile, TObject
    
    fileName = "ONLINE_RUN_" + str(opts.run) + "_hist.root"
    histFile = TFile(fileName,"RECREATE")
    histList = {}
    
    def findHist(label, name):
        if label+name in histList:
            return histList[label+name]
        
        if not histFile.GetDirectory(label):
            histFile.mkdir(label)
            
        histFile.cd(label)                
        min_lb = -1
        max_lb = -1
        for k in lumis.iterkeys():
            if min_lb < 0 or k < min_lb: min_lb = k
            if max_lb < 0 or k > max_lb: max_lb = k
            
        hist = TH1F(name, name, max_lb-min_lb+1, min_lb, max_lb+1.0)
        histList[label+name] = hist        
        return hist

totals = {} # total rate: ctp id -> L1Data dictionary

for lb, items in rates.iteritems():
    if opts.fillhist:
        hLbLen = findHist("LB", "Len")
        hLbLen.Fill(lb+0.5,getLBPeriod(lb))
        
    for ctpId, rate in items.iteritems():

        if opts.fillhist:
            histBase = rate.name
            hTav = findHist(histBase, "TAV")
            hTap = findHist(histBase, "TAP")
            hTbp = findHist(histBase, "TBP")

            hTav.Fill(lb+0.5, rate.getRate_TAV())
            hTap.Fill(lb+0.5, rate.getRate_TAP())
            hTbp.Fill(lb+0.5, rate.getRate_TBP())

        if ctpId not in totals:
            totals[ctpId] = rate
            log.debug('New total rate: '+str(rate.ctpid)+', '+str(totals[ctpId].ctpid)+','+str(ctpId))
        else:
            totals[ctpId].AddData(rate)

ilumi = 0.0 # integrated luminosit for selected lumi blocks
dlumi = 0.0 # integrated delivered luminosity 

for lb, lumi in lumis.iteritems():
    if IncludeLB(lb):
        ilumi += lumi.getIntLumi()
        dlumi += lumi.getDeliveredLumi()

print 'Integrated delivered luminosity:',ilumi,'microbarn^-1'
print 'Integrated recorded  luminosity:',ilumi,'microbarn^-1'

if len(opts.xmlfile) > 4:
    xout = open(opts.xmlfile, 'w')
    
    xout.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')  
    xout.write('<trigger>\n')
    xout.write('  <level>\n')
    xout.write('    <lvl_name>L1</lvl_name>\n')

    for rate in totals.itervalues():
        xout.write('    <signature>\n')
        xout.write('      <sig_name>'            +rate.name              +'</sig_name>\n')
        xout.write('      <sig_counter>'         +str(rate.ctpid)        +'</sig_counter>\n')
        xout.write('      <evts_passed>'         +str(rate.tav)          +'</evts_passed>\n')
        xout.write('      <evts_passed_weighted>'+str(rate.tav)          +'</evts_passed_weighted>\n')

        if opts.lv1_bit == "TAV":
            xout.write('      <rate>'                +str(rate.getRate_TAV_scaled())+'</rate>\n')
            xout.write('      <rate_err>'            +str(rate.getRateErr_TAV_scaled())+'</rate_err>\n')
        elif opts.lv1_bit == "TBP":
            xout.write('      <rate>'                +str(rate.getRate_TBP_scaled())+'</rate>\n')
            xout.write('      <rate_err>'            +str(rate.getRateErr_TBP_scaled())+'</rate_err>\n')
            
        xout.write('      <prescale>'            +str(rate.ps)           +'</prescale>\n')
        xout.write('      <passthrough>'             +str(0)+'</passthrough>\n')             # no pt in L1
        xout.write('      <efficiency>'              +str(0)+'</efficiency>\n')              # no eff in data
        xout.write('      <efficiency_err>'          +str(0)+'</efficiency_err>\n')          # no eff in data
        xout.write('      <prescaled_efficiency>'    +str(0)+'</prescaled_efficiency>\n')    # no eff in data
        xout.write('      <prescaled_efficiency_err>'+str(0)+'</prescaled_efficiency_err>\n')# no eff in data
        xout.write('    </signature>\n')

    for lb in rates.iterkeys():
        xout.write('    <lb>'+str(lb)+'</lb>\n')

    xout.write('  </level>\n')
    xout.write('</trigger>\n')
    xout.close()
    
if opts.fillhist:
    histFile.Write()
    histFile.Close()
