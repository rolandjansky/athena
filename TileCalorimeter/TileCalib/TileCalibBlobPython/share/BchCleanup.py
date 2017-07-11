#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# File:    BchCleanup.py
# Package: TileCalibBlobPython
# Purpose: Clean up BCH folders from an input bch-input-sqlite.db file.  Output saved to output sqlite.db file.
#          Too many reads from input file, so we avoid reading from the DB.  Oracle DB tag to be cleaned
#          must be copied into input sqlite.db file beforehand, using AtlCoolCopy.
#
# Guilherme Lima  - 2013-08-07
#
# WARNING: options explained in usage() have not been tested!!!   Probably they WON'T work as expected!!

import getopt,sys,os,string

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print """Uses bch-input-sqlite.db file as input, joins adjacent IOVs with same contents, keeping the
            comments from the earliest one.  Results written to bch-output-sqlite.db"""
    print ""
    print "-h, --help       shows this help"
    print "-f, --folder=    specify status folder to use ONL01, OFL01 or OFL02 "
    print "-t, --tag=       specify tag to use, f.i. UPD1 or UPD4"
    print "-o, --outtag=    specify output tag to be used for cleaned up result, f.i. UPD4-99"
    print "-i, --instance=  specify DB instance (CONDBR2 or COMP200 or OFLP200)"
    print ""

def showAdcProblems(mgr,ros,mod):
    modName = TileCalibUtils.getDrawerString(ros,mod)        
    for chn in xrange(TileCalibUtils.max_chan()):
        for adc in xrange(TileCalibUtils.max_gain()):

            stat = mgr.getAdcStatus(ros,mod,chn,adc)
            prbs = mgr.getAdcProblems(ros,mod,chn,adc)
            if len(prbs):
                msg = "%s %2i %1i " % (modName,chn,adc)
                for prbCode in sorted(prbs.keys()):
                    prbDesc = prbs[prbCode]
                    msg += " %5i (%s)" % (prbCode,prbDesc)
                    if stat.isBad():
                        msg += "  => BAD"
                    elif stat.isAffected():
                        msg += "  => Affected"
                    elif stat.isGood():
                        msg += "  => good"
                log.debug(msg)


def formatIOV(iov):
    return (int(iov >> 32), int(iov & 0xFFFFFFFF))

#def writeMergedIOV(fdout,outtag,ros,mod,bchDrawer,since,until):
def writeMergedIOV(ros,mod,since,until):
    """ Writes out blob into folder fdout, at channel related to
        ros,mod and with validity range given by since,until
        """
    log.info("Writing merged IOV: [%i,%i]-[%i,%i)" % ((since>>32),(since&0xffffffff),(until>>32),(until&0xffffffff)))

    chanNum = TileCalibUtils.getDrawerIdx(ros,mod)
    #iov1 = TileCalibTools.getCoolValidityKey(since,False)
    #iov2 = TileCalibTools.getCoolValidityKey(until,False)

    #chid=obj.channelId()

    ## setup output zero-size blob
    #newBlob = blobWriter.getDrawer(ros,mod)
    #for chn in xrange(TileCalibUtils.max_chan()):
    #    #.. Bch blobs store one extra bit pattern for channel status
    #    for adc in xrange(TileCalibUtils.max_gain()+1):
    #        for idx in xrange(1):
    #            val = bchDrawer.getData(chn, adc, idx)
    #            log.info("ros=%i mod=%i chn=%i adc=%i val=%i" % (ros,mod,chn,adc,val))
    #            newBlob.setData(chn,adc,idx,val)

    #fdout.storeObject(since, until, blob, chanNum, outtag, True)

    runSince = since>>32
    lumSince = since&0xffffffff
    runUntil = until>>32
    lumUntil = until&0xffffffff

    #.. fix IOVs [r1,l1]-[r2,MAXLBK] --> [r1,l1]-[r2+1,0]
    if lumUntil == 4294967295 and runUntil != 2147483647:
        runUntil += 1
        lumUntil = 0

    msg = 'AtlCoolCopy.exe \"%s\" \"%s\" -folder /TILE/OFL02/STATUS/ADC  -tag TileOfl02StatusAdc-RUN2-UPD4-08 -rls %i %i -rlu %i %i -alliov -outtag %s -ch1 %i -ch2 %i -nrls %i %i -nrlu %i %i\n' % (ischema,oschema,runSince,lumSince,runSince,lumSince+1,outtagFull,chanNum,chanNum,runSince,lumSince,runUntil,lumUntil)
    log.info(msg)


### Main entry point for execution

if __name__ == "__main__":

  letters = "ht:f:o:i"
  keywords = ["help","tag=","folder=","outtag=","instance="]
  try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

  # defaults 
  instance='OFLP200'
  folderPath =  "/TILE/OFL02/STATUS/ADC"
  tag = "SDR-BS8T-09"
  outtag = "test"

  print 'opts:',opts
  for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = "/TILE/%s/STATUS/ADC" % a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-o","--outtag"):
        outtag = a
    elif o in ("-i","--instance"):
        instance = a
    #elif o in ("-s","--schema"):
    #    schema = a
    #elif o in ("-r","--run"):
    #    run = int(a) 
    #elif o in ("-l","--lumi"):
    #    lumi = int(a)
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandled option"


  from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
  log = getLogger("BchCleanup")
  import logging
  log.setLevel(logging.INFO)

  ischema = 'sqlite://;schema=bch-input-sqlite.db;dbname='+instance
  oschema = 'sqlite://;schema=bch-output-sqlite.db;dbname='+instance
  log.info("ischema=%s" % ischema)
  log.info("oschema=%s" % oschema)


  from TileCalibBlobPython import TileCalibTools
  from TileCalibBlobPython import TileBchTools
  from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
  from TileCalibBlobObjs.Classes import *

  #=== open databases
  idb = TileCalibTools.openDbConn(ischema,'READONLY')
  #odb = TileCalibTools.openDbConn(oschema,'RECREATE')

  #-- Workout folderTag
  folderTag = TileCalibTools.getFolderTag(idb if 'CONDBR2' in ischema else ischema, folderPath, tag)

  #-- Blob I/O classes
  blobReader = TileCalibTools.TileBlobReader(idb,folderPath, folderTag)
#  blobWriter = TileCalibTools.TileBlobWriter(odb,folderPath, 'Bch', True) # arg4: False:ONL, True:OFL, default=True
#  blobWriter.setComment("lima","Cleanup of bad channels folder, by merging adjacent folders when identical")

  outtagFull = 'TileOfl02StatusAdc-'+outtag
#  outFolder = odb.getFolder(folderPath)

  #-- initialize BchMgr from input DB
  log.info("Input folder for cleaning: %s with tag %s" % (folderPath, folderTag))

  ##=== Dump the current isBad definition
  #isBadDef = mgr.getAdcProblems(0,1,0,0)
  #log.info( "isBad Definition: " )
  #for prbCode in sorted(isBadDef.keys()):
  #     prbDesc = isBadDef[prbCode]
  #    msg = "- %2i (%s)" % (prbCode,prbDesc)
  #    log.info( msg )
  #log.info( "\n" )


  #=== Loop over DB contents

  #=== get the channel status (if it is bad and will be masked)
  #=== the channel status depends on the definition of isBad stored
  #=== in the database drawer 1, channel 0
  #=== isAffected = has a problem not included in isBad definition
  #=== isGood = has no problem at all

  rosinput = int(sys.argv[1])
  log.info("rosinput=%i"%rosinput)

  for ros in xrange(rosinput,rosinput+1):
    for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):

      modName = TileCalibUtils.getDrawerString(ros,mod)        
      log.info(40*'='+" ros %d, drawer %s" % (ros, modName))

      dbobjs = blobReader.getDBobjsWithinRange(ros, mod)
      if dbobjs == None:
        raise Exception("No DB objects retrieved for ros=%d mod=%d (%s)" % (ros,mod,modName))

      #print "# IOVs: %d - will try to print container below..." % len(dbobjs)
      #print dbobjs

      #.. keep track of validity range of identical and adjacent sets of conditions
      mergedSince = mergedUntil = None

      iovCounter = 0
      objPrev = obj = None
      blobPrev = blob = None
      calibDrawerPrev = calibDrawer = None
      runPrev = run = -1
      lumPrev = lum = -1

      #.. Loop over dbobjs
      while dbobjs.goToNext():
        iovCounter += 1
        objPrev = obj
        blobPrev = blob
        calibDrawerPrev = calibDrawer
        runPrev = run
        lumPrev = lum

        obj = dbobjs.currentRef()
        objsince = obj.since()
        objuntil = obj.until()

        #.. initialize mergedSince,mergeUntil, which keep track of validity range of last set of identical conditions
        if mergedSince is None and mergedUntil is None:
            mergedSince = objsince
            mergedUntil = objuntil

        run = objsince >> 32
        lum = objsince & 0xFFFFFFFF
        blob = obj.payload()[0]

        calibDrawer = None

        if iovCounter%10 == 0:
           log.info("Heartbeat %d: blob size=%d - [%d,%d]-[%d,%d)"
                    % ( iovCounter, blob.size(), run, lum, (objuntil>>32), (objuntil&0xFFFFFFFF) ) )
        else:
           log.debug("blob size=%d - [%d,%d]-[%d,%d)"
                      % ( blob.size(), run, lum, (objuntil>>32), (objuntil&0xFFFFFFFF)) )

        if blob.size() == 596:
            try:
                calibDrawer = TileCalibDrawerBch.getInstance(blob)
            except Exception, err:
                log.error("ros=%i mod=%i last read: runPrev=%i lumPrev=%i\n %s"
                          % (ros, mod, runPrev, lumPrev, str(err)))

        if calibDrawer is None:
            #.. Non-existent, zero-sized or invalid blobs
            if calibDrawerPrev is None:
                # push upper limit to current IOV's upper limit
                mergedUntil = objuntil
                if blob is None:  log.warning( "extending IOV due to non-existent blob!!!" )
                elif blob.size()==0: log.info( "extending IOV due to zero-size blob!!!"    )
                else:             log.warning( "extending IOV due to invalid-size blob!!!" )
                continue

            else:
                # non-identical blobs: write previous blob with new saved validity range...  
                log.info("types: %s  %s" % ( type(blob), type(calibDrawerPrev)) )
                #writeMergedIOV(outFolder, outtagFull, ros, mod, calibDrawerPrev, mergedSince, mergedUntil)
                writeMergedIOV(ros, mod, mergedSince, mergedUntil)
                # ...and then start a new IOV based on current blob's validity range
                mergedSince = objsince
                mergedUntil = objuntil
                tempRunLum = (mergedSince>>32, mergedSince&0xffffffff)
                if blob is None:  log.warning( "Starting new IOV at: [%d,%d] (non-existent blob!!!)" % tempRunLum )
                elif blob.size()==0: log.info( "Starting new IOV at: [%d,%d] (zero-size blob!!!)"    % tempRunLum )
                else:             log.warning( "Starting new IOV at: [%d,%d] (invalid-size blob!!!)" % tempRunLum )
                continue

        #.. Only good calibDrawers reach here

        bchDecoder = TileBchDecoder(calibDrawer.getBitPatternVersion())

        #=== create bad channel manager
        mgr = TileBchTools.TileBchMgr()
        mgr.setLogLvl(logging.ERROR)
        mgr.initialize(idb, folderPath, folderTag, (run,lum))
        log.debug("TileBchMgr initialized.")

        #.. for first IOV, print status summary
        if objPrev is None: showAdcProblems(mgr,ros,mod)

        #.. comparing current and previous blobs ===============
        identical = True

        if objPrev is not None:
          if calibDrawerPrev is None:
              identical = False
          else:
            sizelo = calibDrawerPrev.getObjSizeByte()/4
            sizehi = calibDrawer.getObjSizeByte()/4
            if (sizelo <> sizehi):
              log.info("ERROR!!! object sizes are different for ROS %s (%s %s) drawer %s" % (ros, sizelo, sizehi, modName))

            typelo = calibDrawerPrev.getObjType()
            typehi = calibDrawer.getObjType()
            #ov = flt.getObjVersion()
            #no = flt.getNObjs()
            #nc = flt.getNChans()
            #ng = flt.getNGains()

            if (typelo <> typehi):
              log.info("ERROR!!! object types %s %s are different" % (typelo, typehi))
              sys.exit()

            #=== get all problems of this module
            #showAdcProblems(mgr,ros,mod)

            #.. check for identical conditions
            for chn in xrange(TileCalibUtils.max_chan()):
              #  chnName = " %2i" % chn
              #if identical:
              for adc in xrange(TileCalibUtils.max_gain()):
                #if identical:
                  for ind in xrange(1):  # 4 values per channel/adc

                    #=== build status from both adc and channel bits
                    adcBits = calibDrawer.getData(chn, adc, ind)
                    chnBits = calibDrawer.getData(chn, 2, ind)
                    status = TileBchStatus( bchDecoder.decode(chnBits,adcBits) )

                    adcBits = calibDrawerPrev.getData(chn, adc, ind)
                    chnBits = calibDrawerPrev.getData(chn, 2, ind)
                    statusPrev = TileBchStatus( bchDecoder.decode(chnBits,adcBits) )

                    adclo = calibDrawerPrev.getData(chn, adc, ind)
                    adchi = calibDrawer.getData(chn, adc, ind)
                    chnlo = calibDrawerPrev.getData(chn,   2, ind)
                    chnhi = calibDrawer.getData(chn,   2, ind)
                    diff = adclo - adchi + chnlo - chnhi

                    if not (status==statusPrev): 
                      identical = False
                      log.info("chn=%i adc=%i ind=%i - vlo=%i, vhi=%i, diffs=%i %i" % (chn, adc, ind, adclo+chnlo, adchi+chnhi, adclo-adchi, chnlo-chnhi))
                      #break

          #.. at this point, merge if obj is identical to objPrev
          if identical is True:
            # push upper limit to current IOV's upper limit
            mergedUntil = objuntil
          else:
            showAdcProblems(mgr,ros,mod)
            # non-identical blobs: write previous blob with new saved validity range...  
            log.info("types: %s  %s" % ( type(blob), type(calibDrawerPrev)) )
            writeMergedIOV(ros, mod, mergedSince, mergedUntil)
            #writeMergedIOV(outFolder,outtagFull,ros,mod,calibDrawerPrev,mergedSince,mergedUntil)
            # ...and then start a new IOV based on current blob's validity range
            mergedSince = objsince
            mergedUntil = objuntil
            log.info("Starting new IOV at: [%d,%d]"
              % (mergedSince>>32, mergedSince&0xffffffff) )

      #.. end of loop over dbobjs

      if objuntil == 0x7fffffffffffffff:
        log.info("Writing last IOV: calling writeMergedIOV by hand...")
        #writeMergedIOV(outFolder,outtagFull,ros,mod,calibDrawerPrev,mergedSince,mergedUntil)
        writeMergedIOV(ros,mod,mergedSince,mergedUntil)

  #=== print all bad channels
  #log.info("listing bad channels")
  #mgr.listBadAdcs()

#  blobWriter.register((MINRUN,MINLBK),(MAXRUN,MAXLBK),folderTag)

  #=== close DB
  db.closeDatabase()
