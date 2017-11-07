# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, shutil, re

from dqu_subprocess import apply as _local_apply

def _dolsrwrapper(fname):
    import ROOT
    rf = ROOT.TFile.Open(fname, 'READ')
    if not rf or not rf.IsOpen():
        print '   %s is empty or not accessible' % f
        return False

    cleancache = ROOT.gROOT.MustClean(); ROOT.gROOT.SetMustClean(False)
    _dolsr(rf)
    rf.Close()
    ROOT.gROOT.SetMustClean(cleancache)

def _dolsr(dir):
    import ROOT

    resultdir = dir.Get('Results'); statusobj = None
        
    if resultdir:
        statusobj = resultdir.Get('Status')

        if statusobj:
            l = statusobj.GetListOfKeys()
            l[0].GetName()

    keys = dir.GetListOfKeys()
    for key in keys:
        name = key.GetName()
        if ROOT.TClass.GetClass(key.GetClassName()).InheritsFrom('TDirectory'):
            dirobj = key.ReadObj()
            resultdir = dirobj.Get('Results'); statusobj = None
            if resultdir:
                statusobj = resultdir.Get('Status')

            # is a check? is a summary? or recurse?
            if name[-1] == '_' and resultdir:
                hist = dir.Get(name[:-1])
                subkeys = resultdir.GetListOfKeys()
                for subkey in subkeys:
                    ressub = subkey.GetName()
                    if ressub not in ('Status', 'Reference', 'ResultObject') and ROOT.TClass.GetClass(subkey.GetClassName()).InheritsFrom('TDirectory'):
                        l = subkey.ReadObj().GetListOfKeys()
                        l[0].GetName()

                # status flag
                l = statusobj.GetListOfKeys()
                l[0].GetName()
            else:
                _dolsr(dirobj)

def _ProtectPostProcessing( funcinfo, outFileName, isIncremental ):
    import os
    isProduction = 'DQPRODUCTION' in os.environ and bool(os.environ['DQPRODUCTION'])
    func, mail_to = funcinfo
    tmpfilename = outFileName + "-safe-" + str(os.getpid())
    shutil.copy2(outFileName, tmpfilename)
    success = False
    #try to get log file name and curr directory in order to find job info
    logFiles=""
    currDir=" pwd failed!"
    try:
        currDir=os.environ["PWD"]
        logFilesList=[]
        for files in os.listdir(currDir):
            if re.match(r'^[0-9]*_.*.log$',files):
                logFilesList.append("%s"%files)
        logFiles='\r\n'.join(logFilesList)
    except Exception, e:
        pass
    try:
        _local_apply(func, (tmpfilename, isIncremental))
        _local_apply(_dolsrwrapper, (tmpfilename,))
        success = True
    except Exception, e:
        print 'WARNING!!!: POSTPROCESSING FAILED FOR', func.__name__
        print e
        print 'Relevant results will not be in output'
        if isProduction:
            import smtplib
            server = smtplib.SMTP('localhost')
            mail_cc = ['ponyisi@utexas.edu', 'yuriy.ilchenko@cern.ch']
            msg = ['From: atlasdqm@cern.ch',
                   'To: %s' % ', '.join(mail_to),
                   'Cc: %s' % ', '.join(mail_cc),
                   'Reply-to: atlasdqm-no-reply@cern.ch',
                   'Subject: WARNING: Postprocessing failure in production job',
                   '',
                   'There has been a failure in Tier-0 postprocessing monitoring.',
                   '',
                   'Function: %s' % func.__name__,
                   'File: %s' % outFileName,
                   'isIncremental? %s' % isIncremental,
                   'Error:',
                   str(e),
                   'Current directory (contains LSF job ID): %s' % currDir,
                   'Log file name (contains ATLAS T0 job ID): %s' % logFiles
                   ]
            print 'Emailing', ', '.join(mail_to)
            server.sendmail('atlasdqm@cern.ch', mail_to + mail_cc, '\r\n'.join(msg))
            server.quit()
        print
    finally:
        if success:
            shutil.move(tmpfilename, outFileName)
        else:
            os.unlink(tmpfilename)
    return success

def DQPostProcess( outFileName, isIncremental=False ):
    ## Import the ROOT library for reading han results
    from ROOT import gSystem
    gSystem.Load('libDataQualityUtils')
    from ROOT import dqutils
    
    mf = dqutils.MonitoringFile()

    def rpc_create(dummy, dummy2):
        from DataQualityUtils.createRpcFolders import (createRPCDQMFDB,
                                                       createRPCConditionDB)
        createRPCDQMFDB()
        createRPCConditionDB()

    def mdt_create(dummy, isIncremental):
#        if False and not isIncremental:
        if not isIncremental:            
            from DataQualityUtils.createMdtFolders import (createMDTConditionDBDead,
                                                           createMDTConditionDBNoisy)
            createMDTConditionDBDead()
            createMDTConditionDBNoisy()
 
    funclist = [ 
                 (mf.fitMergedFile_IDPerfMonManager,
                  ['weina.ji@cern.ch', 'ana.ovcharova@cern.ch']),
                 (mf.fitMergedFile_DiMuMonManager,
                  ['ana.ovcharova@cern.ch']),
                 (mf.fitMergedFile_IDAlignMonManager,
                  ['John.Alison@cern.ch', 'anthony.morley@cern.ch']),
                 (mf.pv_PrimaryVertexMonitoring_calcResoAndEfficiency,
                  ['Andreas.Wildauer@cern.ch']),
                 (mf.VxMon_move,
                 ['federico.meloni@cern.ch']),
                 (mf.CSCPostProcess,
                  ['venkat.kaushik@cern.ch', 'jason.veatch@cern.ch']),
                 (rpc_create,
                  ['michele.bianco@le.infn.it', 'monica.verducci@cern.ch']),
                 (mf.RPCPostProcess,
                  ['michele.bianco@le.infn.it', 'monica.verducci@cern.ch']),
                 (mdt_create,
                  ['john.stakely.keller@cern.ch', 'monica.verducci@cern.ch']),
                 (mf.MDTPostProcess,
                  ['john.stakely.keller@cern.ch']),
                 (mf.TGCPostProcess,
                  ['lyuan@ihep.ac.cn', 'kingmgl@stu.kobe-u.ac.jp']),
                 (mf.MDTvsTGCPostProcess,
                  ['lyuan@ihep.ac.cn', 'kingmgl@stu.kobe-u.ac.jp']),
                 (mf.HLTMuonPostProcess,
                  ['lyuan@ihep.ac.cn', 'kingmgl@stu.kobe-u.ac.jp', 'yamazaki@phys.sci.kobe-u.ac.jp']),
                 (mf.HLTEgammaPostProcess,
                  ['yan.jie.schnellbach@cern.ch', 'alessandro.tricoli@cern.ch']),
                 (mf.HLTTauPostProcess,
                  ['Cristobal.Cuenca@cern.ch', 'Geng-Yuan.Jeng@cern.ch', 'Giovanna.Cottin@cern.ch']),
                 (mf.HLTMETPostProcess,
                  ['venkat.kaushik@cern.ch']),
                 (mf.HLTCaloPostProcess,
                  ['gareth.brown@cern.ch']),
                 (mf.HLTJetPostProcess,
                  ['venkat.kaushik@cern.ch']),
                 (mf.HLTMinBiasMonPostProcess,
                  ['azemla@cern.ch']),
                 (mf.BJetTaggingPostProcess,
                  ['m.neumann@cern.ch']),
                 (mf.L1CaloPostProcess,
                  ['ivana.hristova@cern.ch', 'pjwf@hep.ph.bham.ac.uk']),
                 (mf.SCTPostProcess,
                  ['masaki.endo@cern.ch']),
                 (mf.PixelPostProcess,
                  ['daiki.yamaguchi@cern.ch']),
                 (mf.MuonTrackPostProcess,
                  ['baojia.tong@cern.ch', 'alexander.tuna@cern.ch'])
               ]

    # first try all at once
    def go_all(fname, isIncremental):
        for funcinfo in funclist:
            funcinfo[0](fname, isIncremental)
    
    success = _ProtectPostProcessing( (go_all, []), outFileName, isIncremental )

    if not success:
    #if True:
        for funcinfo in funclist:
            _ProtectPostProcessing( funcinfo, outFileName, isIncremental )
