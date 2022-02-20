# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Pythonised helper functions for usage in top-level JobOptions
#    written by Zach Marshall <zach.marshall@cern.ch>
#    updates by Christian Gutschow <chris.g@cern.ch>

import os
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MCJobOptionUtils')



def get_LHAPDF_DATA_PATH():
    return get_LHAPDF_PATHS()[1]


def get_LHAPDF_PATHS():
    LHADATAPATH=None
    LHAPATH=None
    for p in os.environ['LHAPATH'].split(':')+os.environ['LHAPDF_DATA_PATH'].split(':'):
        if os.path.exists(p+"/../../lib/") and LHAPATH is None:
            LHAPATH=p
    for p in os.environ['LHAPDF_DATA_PATH'].split(':')+os.environ['LHAPATH'].split(':'):
        if os.path.exists(p) and LHADATAPATH is None and p!=LHAPATH:
            LHADATAPATH=p
    if LHADATAPATH is None:
        LHADATAPATH=LHAPATH
    if LHAPATH is None:
        mglog.error('Could not find path to LHAPDF installation')
    return LHAPATH,LHADATAPATH


def get_lhapdf_id_and_name(pdf):
    ''' Function to get lhapdf id and name from either id or name.'''
    pdfname=''
    pdfid=-999
    LHADATAPATH=get_LHAPDF_DATA_PATH()
    pdflist = open(LHADATAPATH+'/pdfsets.index','r')
    if isinstance(pdf,int) or pdf.isdigit():
        pdf=int(pdf)
        pdfid=pdf
        for line in pdflist:
            splitline=line.split()
            if int(splitline[0]) == pdfid:
                pdfname=splitline[1]
                break
    else:
        pdfname=pdf
        for line in pdflist:
            splitline=line.split()
            if splitline[1] == pdfname:
                pdfid=int(splitline[0])
                break
    pdflist.close()

    if pdfname=='':
        err='Couldn\'t find PDF name associated to ID %i in %s.'%(pdfid,LHADATAPATH+'/pdfsets.index')
        mglog.error(err)
        raise RuntimeError(err)
    if pdfid<0:
        err='Couldn\'t find PDF ID associated to name %s in %s.'%(pdfname,LHADATAPATH+'/pdfsets.index')
        mglog.error(err)
        raise RuntimeError(err)

    return pdfid,pdfname


def resetLHAPDF(origLHAPATH='',origLHAPDF_DATA_PATH=''):
    mglog.info('Restoring original LHAPDF env variables:')
    os.environ['LHAPATH']=origLHAPATH
    os.environ['LHAPDF_DATA_PATH']=origLHAPDF_DATA_PATH
    mglog.info('LHAPATH='+os.environ['LHAPATH'])
    mglog.info('LHAPDF_DATA_PATH='+os.environ['LHAPDF_DATA_PATH'])

