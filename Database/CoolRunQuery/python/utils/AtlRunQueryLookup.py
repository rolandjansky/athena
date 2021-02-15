#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryLookup.py
# Project: AtlRunQuery
# Purpose: Library with references for DQ flags and detector mask
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2008
# ----------------------------------------------------------------
#
# See: DetectorDescription/DetDescrCond/DetectorStatus/doc/mainpage.h
# Decription:
#   http://alxr.usatlas.bnl.gov/lxr/source/atlas/DetectorDescription/DetDescrCond/DetectorStatus/doc/mainpage.h
# Implementation:
#   http://alxr.usatlas.bnl.gov/lxr/source/atlas/DetectorDescription/DetDescrCond/DetectorStatus/python/DetStatusLib.py

from __future__ import print_function
import sys

DQChannelDict = {'PIXB':101,'PIX0':102,'PIXEA':104,'PIXEC':105,
                 'SCTB':111,'SCTEA':114,'SCTEC':115,
                 'TRTB':121,'TRTEA':124,'TRTEC':125,'TRTTR':126,
                 'IDGL':130, # Obsolete: 'IDGB OBSOLETE':131,'IDGEA OBSOLETE':134,'IDGEC OBSOLETE':135,
                 'IDAL':140, # Obsolete: 'IDAB OBSOLETE':141,'IDAEA OBSOLETE':144,'IDAEC OBSOLETE':145,
                 'IDBS':150,
                 'IDVX':161,
                 'IDBCM':170,
                 'EMBA':202,'EMBC':203,'EMECA':204,'EMECC':205,
                 'HECA':214,'HECC':215,
                 'FCALA':224,'FCALC':225,
                 'TIGB':230,
                 'TILBA':232,'TILBC':233,'TIEBA':234,'TIEBC':235,
                 'MBTSA':244,'MBTSC':245,
                 'CALBA':251,'CALEA':254,'CALEC':255,
                 'MDTBA':302,'MDTBC':303,'MDTEA':304,'MDTEC':305,
                 'RPCBA':312,'RPCBC':313,
                 'TGCEA':324,'TGCEC':325,
                 'CSCEA':334,'CSCEC':335,
                 'LCDA':353,'LCDC':354,
                 'ALFA':360,'ZDC':370,
                 'L1CAL':401,'L1MUB':402,'L1MUE':403,'L1CTP':404,
                 'TRCAL':411,
                 'TRBJT':421,'TRBPH':422,'TRCOS':423,'TRELE':424,
                 'TRGAM':425,'TRJET':426,'TRMET':427,'TRMBI':428,
                 'TRMUO':429,'TRTAU':430,'TRIDT':431,
                 'LUMI':450, 'LUMIONL':451,
                 'RUNCLT':460,
                 'RCOPS':461,
                 'ATLGL':480,'ATLSOL':481,'ATLTOR':482,
                 'EIDB':501,'EIDCR':502,'EIDE':503,
                 'PIDB':505,'PIDCR':506,'PIDE':507,
                 'EIDF':508,'EIDSOFT':509,
                 'MSTACO':510,'MMUIDCB':511,'MMUIDVX':512,
                 'MMUGIRL':513,'MMUBOY':514,'MMUIDSA':515,
                 'MMUTAG':516,'MMTIMO':517,'MCMUTAG':518,
                 'MCALLHR':519,
                 'JETB':521,'JETEA':524,'JETEC':525,
                 'JETFA':526,'JETFC':527,
                 'METCALO':531,'METMUON':532,
                 'BTGLIFE':541,'BTGSOFTE':544,'BTGSOFTM':545,
                 'TAUB':551,'TAUCR':552,'TAUE':553}

DQSuperGroupsDict = {1 : ['Inner detector',       ['PIXB','PIX0','PIXEA','PIXEC','SCTB','SCTEA','SCTEC','TRTB','TRTEA','TRTEC','TRTTR',
                                                   'IDGL','IDAL','IDBS','IDVX']], 
                     2 : ['Calorimeter',          ['EMBA','EMBC','EMECA','EMECC','HECA','HECC','FCALA','FCALC',
                                                   'TIGB','TILBA','TILBC','TIEBA','TIEBC','CALBA','CALEA','CALEC']],
                     3 : ['Muon systems',         ['MDTBA','MDTBC','MDTEA','MDTEC','RPCBA','RPCBC','TGCEA','TGCEC','CSCEA','CSCEC']],
                     4 : ['Forward detectors and luminosity', ['MBTSA','MBTSC','IDBCM','LCDA','LCDC','ALFA','ZDC','LUMI']],
                     5 : ['Global, Magnets, DAQ and Trigger',['ATLGL','ATLSOL','ATLTOR','RUNCLT','RCOPS',
                                                   'L1CAL','L1MUB','L1MUE','L1CTP',
                                                   'TRCAL','TRBJT','TRBPH','TRCOS','TRELE','TRGAM','TRJET','TRMET','TRMBI','TRMUO','TRTAU','TRIDT']],
                     6 : ['Combined performance', ['EIDB','EIDCR','EIDE',
                                                   'PIDB','PIDCR','PIDE',
                                                   'EIDF','EIDSOFT',
                                                   'MSTACO','MMUIDCB','MMUIDVX',
                                                   'MMUGIRL','MMUBOY','MMUIDSA',
                                                   'MMUTAG','MMTIMO','MCMUTAG',
                                                   'MCALLHR',
                                                   'JETB','JETEA','JETEC','JETFA','JETFC',
                                                   'METCALO','METMUON',
                                                   'BTGLIFE','BTGSOFTE','BTGSOFTM',
                                                   'TAUB','TAUCR','TAUE']]}

DQGroupDict =  { 100:'PIX',
                 110:'SCT',
                 120:'TRT',
                 130:'IDGL',
                 140:'IDAL',
                 150:'IDBS',
                 160:'IDVX',
                 170:'IDBCM',
                 200:'LArEM',
                 210:'HEC',
                 220:'FCAL',
                 230:'TILE',
                 240:'MBTS',
                 250:'CAL',
                 300:'MDT',
                 310:'RPC',
                 320:'TGC',
                 330:'CSC',
                 350:'LCD',
                 360:'ALFA',
                 370:'ZDC',
                 400:'L1',
                 410:'TRIG',
                 420:'TRBJT',
                 450:'LUMI',
                 460:'RUNCLT',
                 500:'EGAMCP',
                 510:'MUONCP',
                 520:'JETCP',
                 530:'METCP',
                 540:'BTAGCP',
                 550:'TAUCP'}

def DQChannels():
    # sort disctionary
    dqitems = DQChannelDict.items()
    dqitems = [(v, k) for (k, v) in dqitems]
    dqitems.sort()
    return dqitems

def DQChannel(name):
    name = name.upper()
    if ':' in name:
        name = name[name.index(':')+1:]
    # OLD: if name.startswith('CP_'): return -1
    if '_' in name:
        return -1
    return DQChannelDict[name]


def isDQ(name):
    name = (name.split(':')[-1].split('#')[0]).upper()
    if name.startswith("CP_") or name.startswith("PHYS_") or name.startswith("TRIG_") or name.startswith("LUM_") or name.startswith("GLOBAL_"):
        return True
    return name in DQChannelDict


OLCAlgorithms = { 0:     'ATLAS_PREFERRED',
                  1:     'LHC',
                  101:	 'LUCID_ZEROS_OR',
                  102:	 'LUCID_ZEROS_AND',
                  103:	 'LUCID_HITS_OR',
                  104:	 'LUCID_HITS_AND',
                  151:	 'LUCID_AND',
                  201:	 'BCM_H_ZEROS_AND',
                  202:	 'BCM_H_EVENTS_AND',
                  203:	 'BCM_H_EVENTS_XORA',
                  204:	 'BCM_H_EVENTS_XORC',
                  205:	 'BCM_V_ZEROS_AND',
                  206:	 'BCM_V_EVENTS_AND',
                  207:	 'BCM_V_EVENTS_XORA',
                  208:	 'BCM_V_EVENTS_XORC',
                  301:	 'MBTS_ZEROS_AND',
                  302:	 'MBTS_ZEROS_OR',
                  303:	 'MBTS_HITS_AND',
                  304:	 'MBTS_HITS_OR',
                  401:	 'ZDC',
                  501:	 'FCAL',
                  601:	 'HLT',
                  901:	 'OffLumi_LArTime_Events',
                  998:	 'OflLumi_Fake0',
                  999:	 'OflLumi_Fake1' }

def InitDetectorMaskDecoder( run2 ):
    if run2:
        return InitDetectorMaskDecoderRun2()
    else:
        return InitDetectorMaskDecoderRun1()
    
def InitDetectorMaskDecoderRun1():

    # taken from:
    #   https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/DataFlow/eformat/tags/eformat-04-04-03/src/DetectorMask.cxx
    #
    # note that these here are obsolete:
    #   https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/online/RCUtils/trunk/src/get_detectormask.cc
    #   http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/DAQ/online/RunControl/src/get_detectormask.cc?root=atlastdaq&view=markup

    dName = ['unknown']*64
    dName[0]  = "Pix Barrel"
    dName[1]  = "Pix Disks"
    dName[2]  = "Pix B-Layer"
    dName[3]  = "REMOVED"
    dName[4]  = "SCT BA"
    dName[5]  = "SCT BC"
    dName[6]  = "SCT EA"
    dName[7]  = "SCT EC"
    dName[8]  = "TRT BA"
    dName[9]  = "TRT BC"
    dName[10] = "TRT EA"
    dName[11] = "TRT EC"
    dName[12] = "LAr EMBA"
    dName[13] = "LAr EMBC"
    dName[14] = "LAr EMECA"
    dName[15] = "LAr EMECC"
    dName[16] = "LAr HECA"
    dName[17] = "LAr HECC"
    dName[18] = "LAr FCALA"
    dName[19] = "LAr FCALC"
    dName[20] = "Tile BA"
    dName[21] = "Tile BC"
    dName[22] = "Tile EA"
    dName[23] = "Tile EC"
    dName[24] = "MDT BA"
    dName[25] = "MDT BC"
    dName[26] = "MDT EA"
    dName[27] = "MDT EC"
    dName[28] = "RPC BA"
    dName[29] = "RPC BC"
    dName[30] = "TGC EA"
    dName[31] = "TGC EC"
    dName[32] = "CSC EA"
    dName[33] = "CSC EC"
    dName[34] = "L1Calo preprocessor"
    dName[35] = "L1Calo cluster DAQ"
    dName[36] = "L1Calo cluster RoI"
    dName[37] = "L1Calo Jet/E DAQ"
    dName[38] = "L1Calo Jet/E RoI"
    dName[39] = "MUCTPI"
    dName[40] = "CTP"
    dName[41] = "L2SV"
    dName[42] = "SFI"
    dName[43] = "SFO"
    dName[44] = "LVL2"
    dName[45] = "EF"
    dName[46] = "BCM"
    dName[47] = "Lucid"
    dName[48] = "ZDC"
    dName[49] = "Alfa"
    dName[50] = "TRT_ANCILLARY_CRATE"
    dName[51] = "TILECAL_LASER_CRATE"
    dName[52] = "MUON_ANCILLARY_CRATE"
    dName[53] = "TDAQ_BEAM_CRATE"

    # list of detector NOT part of 'all' tag (means, they can be in, but are not required)
    notInAll = ['CSC', 'L2SV', 'SFI', 'SFO', 'LVL2', 'EF', 'Lucid', 'ZDC', 'Alfa',
                'TRT_ANCILLARY_CRATE','TILECAL_LASER_CRATE','MUON_ANCILLARY_CRATE','TDAQ_BEAM_CRATE'  ]

    NotInAll = ['']*64
    for i in range(0,len(dName)): 
        for n in notInAll:
            if dName[i] == n:
                NotInAll[i] = ' NotInAll'

    vetoedbits = [3, 50, 51, 52, 53] + list(range(54,64))

    return (dName, NotInAll, vetoedbits)


def InitDetectorMaskDecoderRun2():

    # taken from:
    #   http://alxr.usatlas.bnl.gov/lxr/source/tdaq-common/eformat/src/DetectorMask.cxx
    #
    # note that these here are obsolete:
    #   https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/online/RCUtils/trunk/src/get_detectormask.cc
    #   http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/DAQ/online/RunControl/src/get_detectormask.cc?root=atlastdaq&view=markup

    dName = ['unknown']*128
    dName[0]  = "Pix Barrel"
    dName[1]  = "Pix Disks"
    dName[2]  = "Pix B-Layer"
    dName[3]  = "REMOVED"
    dName[4]  = "SCT BA"
    dName[5]  = "SCT BC"
    dName[6]  = "SCT EA"
    dName[7]  = "SCT EC"
    dName[8]  = "TRT BA"
    dName[9]  = "TRT BC"
    dName[10] = "TRT EA"
    dName[11] = "TRT EC"
    dName[12] = "LAr EMBA"
    dName[13] = "LAr EMBC"
    dName[14] = "LAr EMECA"
    dName[15] = "LAr EMECC"
    dName[16] = "LAr HECA"
    dName[17] = "LAr HECC"
    dName[18] = "LAr FCALA"
    dName[19] = "LAr FCALC"
    dName[20] = "Tile BA"
    dName[21] = "Tile BC"
    dName[22] = "Tile EA"
    dName[23] = "Tile EC"
    dName[24] = "MDT BA"
    dName[25] = "MDT BC"
    dName[26] = "MDT EA"
    dName[27] = "MDT EC"
    dName[28] = "RPC BA"
    dName[29] = "RPC BC"
    dName[30] = "TGC EA"
    dName[31] = "TGC EC"
    dName[32] = "CSC EA"
    dName[33] = "CSC EC"
    dName[34] = "L1Calo preprocessor"
    dName[35] = "L1Calo cluster DAQ"
    dName[36] = "L1Calo cluster RoI"
    dName[37] = "L1Calo Jet/E DAQ"
    dName[38] = "L1Calo Jet/E RoI"
    dName[39] = "MUCTPI"
    dName[40] = "CTP"
    dName[41] = "L2SV"
    dName[42] = "SFI"
    dName[43] = "SFO"
    dName[44] = "LVL2"
    dName[45] = "HLT"
    dName[46] = "BCM"
    dName[47] = "Lucid"
    dName[48] = "ZDC"
    dName[49] = "Alfa"
    dName[50] = "TRT_ANCILLARY_CRATE"
    dName[51] = "TILECAL_LASER_CRATE"
    dName[52] = "MUON_ANCILLARY_CRATE"
    dName[53] = "TDAQ_BEAM_CRATE"
    dName[54] = "FTK"
    dName[55] = "Offline"
    dName[56] = "L1Topo"
    dName[57] = "L1Calo DIG"
    dName[58] = "L1Calo DAQ"
    dName[59] = "L1Calo ROI"
    dName[60] = "MMEGA EA"
    dName[61] = "MMEGA EC"
    dName[62] = "Pix IBL"
    dName[63] = "Pix DBM"

    # list of detector NOT part of 'all' tag (means, they can be in, but are not required)
    notInAll = ['L2SV', 'SFI', 'SFO', 'LVL2', 'Lucid', 'ZDC', 'Alfa',
                'TRT_ANCILLARY_CRATE','TILECAL_LASER_CRATE','MUON_ANCILLARY_CRATE','TDAQ_BEAM_CRATE'  ]

    NotInAll = map(lambda x: ' NotInAll' if x in notInAll else '', dName)

    vetoedbits = [3, 41, 42, 44, 50, 51, 52, 53, 55, 57, 58, 59] + list(range(64,128)) #+ range(64,128)

    return (dName, NotInAll, vetoedbits)




def DecodeDetectorMaskToString( detmask, isRun2, smart ):
    """
    takes (int) detmask
    returns ( (string) listOfDetectors, (bool) inclusion )
    if smart is set to True, then the listOfDetectors might be inversed and excluded ones are shown
    """

    dName, NotInAll, vetoedbits = InitDetectorMaskDecoder(isRun2)

    ic = 0
    res = ""
    inclusion = True
    for i in range( len(dName) ):
        if i not in vetoedbits and (detmask & (1 << i)):
            res += dName[i] + ", "
            ic += 1
    if res=="":
        res = "none"
    else:
        res = res[:-2] # chop off last comma-space

    if smart and ic > 30:
        # many included prefer to show excluded sub detectors
        inclusion = False
        res = ""
        for i in range( len(dName) ):
            if i not in vetoedbits and not (detmask & (1 << i)):
                res += dName[i] + ", "
        if res=="":
            res = "all"
        else:
            res = res[:-2] # chop off last comma-space

    return (res, inclusion)


def splitAt(line, splitstring=',', splitsize=80):
    res = [line]
    while len( res[-1] ):
        linebreak = res[-1].find( splitstring, splitsize)
        if linebreak==-1:
            break
        res[-1:] = [ res[-1][:linebreak], res[-1][linebreak+len(splitstring):] ]
    return res


def DecodeDetectorMask( mask, isRun2, smart=False ):
    """
    takes an decimal or hex string (hex string must start with 0x) and returns an HTML element
    """
    if type(mask)!=str:
        raise RuntimeError("DetectorMask must be a string")

    if mask.startswith('0x'):
        mask = int(mask[2:],16)
    else:
        mask = int(mask)
        
    detectors, inclusion = DecodeDetectorMaskToString(mask, isRun2, smart)
    #detectors = ",<BR>".join( splitAt(detectors,", ") )

    col = '#106734' if smart else '#000000'
    if inclusion:
        res = '<b>Detector mask = %s (%s), corresponding to the systems:</b><br><font color="%s">' % (mask,hex(mask).rstrip('L'),col)
    else:
        res = '<b>Detector mask = %i (%s).<br> <font color="#aa0000">The following systems are NOT included:</b><br>' % (mask,hex(mask).rstrip('L'))
    res += detectors
    res += '</font>'

    return res


def LArConfig(type):
    # runtype, format
    if 'runtype' in type.lower():
        return {0:'RawData', 1:'RawDataResult', 2:'Result'}
    elif 'format' in type.lower():
        return {0:'Transparent', 1:'Format 1', 2:'Format 2'}
    else:
        print ('ERROR in LArconfig: unknown type %s' % type)
        sys.exit()
             


if __name__=="__main__":

    InitDetectorMaskDecoder(run2=True)

