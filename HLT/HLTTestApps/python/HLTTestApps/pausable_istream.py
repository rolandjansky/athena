# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Ricardo Abreu <ricardo.abreu@cern.ch>
import eformat
from libpyeformat import FullEventFragment, convert_old 
import libpyeformat_helper as helper
import libpyevent_storage as EventStorage # this lib comes actually from eformat
import logging


class PauseIterationException(Exception): pass

class pausable_istream(eformat.istream):
    """ An istream that can only be used sequentially and that raises a 
    PauseIterationException whenever it gets to a new event file with a 
    different run number, so that prepareForRun can be executed again and things
    are consistent with the new run.
    
    This class doesn't allow random access, that is, it doesn't provide events
    by their index. Therefore, __getitem__ raises NotImplementedError
    
    Additionally, some metadata of the file currently being read can be obtained
    with the methods current_run_number and current_detector_mask
    """
    
    def __init__(self,l):
        """ Constructor. It takes a list of file names. This list should have at
        least one element.
        """
        eformat.istream.__init__(self, l)
        if len(self.filelist) > 1: self.multiple_files = True
        else: self.multiple_files = False
        self.f = 0
        self.i = 0
        self.dr = EventStorage.pickDataReader(self.filelist[self.f])
        import sys, os
        self.app_name = os.path.basename(sys.argv[0]).split('.')[0] 
        
    def __getitem__(self, key):
        """ Not implemented. Random access not allowed.
        """
        raise NotImplementedError, 'Random access to a pausable_istream is not'\
                                   ' allowed.'
                                   
    def __iter__(self):
        return self._iterator(self, False)
    
    def iter_raw(self):
        """ Obtain an iterator that returns raw items
        """
        return self._iterator(self, True)
        
    def _updateDataReader(self):
        self._updatef()
        self.i = 0
        oldrn = self.dr.runNumber()
        self.dr = EventStorage.pickDataReader(self.filelist[self.f])
        if oldrn != self.dr.runNumber():
            raise PauseIterationException, 'need to prepare for run again'
        
    def _updatef(self):
        self.f += 1

    def rewind(self):
        """Rewind to the first event in the first file"""
        self.f = 0
        self.dr = EventStorage.pickDataReader(self.filelist[self.f])
        
    def current_run_number(self):
        """ Obtain the run number that is present in the metadata of the file 
        that contains that last event returned (or the first, in case no event
        was read yet)
        """
        return self.dr.runNumber()
    
    def current_detector_mask(self):
        """ Obtain the detector mask that is present in the metadata of the 
        current file being read. The current file is considered to be:
        1 - if no event was read or if the event returned most recently was the
        last in the last file -> the first file
        2 - if the last attempt to read an event from file n raised a 
        PauseIterationException -> the file n+1
        3 - if none of the previous cases -> the file of the last event returned
        """
        return self.dr.detectorMask()

    def datawriter(self, directory, core_name, compression=0, raw_file_name=[]):
      """Creates a new eformat.ostream with the same meta data of the current
      istream, but using the directory and core_name as given."""
      if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and len(raw_file_name) == 1:
          file_name=eformat.EventStorage.RawFileName(
              self.dr.projectTag(),
              self.dr.runNumber(),
              eformat.EventStorage.RawFileName(self.dr.fileName()).streamType(),
              eformat.EventStorage.RawFileName(self.dr.fileName()).streamName(),
              self.dr.lumiblockNumber(),
              self.app_name,
              raw_file_name[0]).fileNameCore()
      elif len(raw_file_name) == 6:
          projectTag = 'UNKNOWN'
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[0] == '':
              projectTag = self.dr.projectTag()
          elif raw_file_name[0] != '':
              projectTag = raw_file_name[0]

          runNumber=0
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[1] == -1:
              runNumber = self.dr.runNumber()
          elif raw_file_name[1] != -1:
              runNumber = raw_file_name[1]

          streamType='UNKNOWN'
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[2] == '':
              streamType = eformat.EventStorage.RawFileName(self.dr.fileName()).streamType()
          elif raw_file_name[2] != '':
              streamType = raw_file_name[2]

          streamName='UNKNOWN'
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[3] == '':
              streamName = eformat.EventStorage.RawFileName(self.dr.fileName()).streamName()
          elif raw_file_name[3] != '':
              streamName = raw_file_name[3]

          lumiblockNumber=0
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[4] == -1:
              lumiblockNumber = self.dr.lumiblockNumber()
          elif raw_file_name[4] != -1:
              lumiblockNumber = raw_file_name[4]

          productionStep='UNKNOWN'
          if eformat.EventStorage.RawFileName(self.dr.fileName()).hasValidCore() and raw_file_name[5] == '':
              productionStep = eformat.EventStorage.RawFileName(self.dr.fileName()).productionStep()
          elif raw_file_name[5] != '':
              productionStep = raw_file_name[5]

          file_name=eformat.EventStorage.RawFileName(
              projectTag,
              runNumber,
              streamType,
              streamName,
              lumiblockNumber,
              self.app_name,
              productionStep).fileNameCore()
      else:
          file_name=core_name
          
      if (hasattr(eformat,'compstream') and (compression in range(1,6))) : # compression is available in eformat and should be switched on
          return eformat.compstream(core_name=file_name, directory=directory,                                    
                                    run_number=self.dr.runNumber(), trigger_type=self.dr.triggerType(),
                                    detector_mask=self.dr.detectorMask(), beam_type=self.dr.beamType(),
                                    beam_energy=self.dr.beamEnergy(),
                                    compression=eformat.EventStorage.CompressionType.ZLIB,complevel=compression)
      else:
          return eformat.ostream(core_name=file_name, directory=directory,
                                 run_number=self.dr.runNumber(), trigger_type=self.dr.triggerType(),
                                 detector_mask=self.dr.detectorMask(), beam_type=self.dr.beamType(),
                                 beam_energy=self.dr.beamEnergy())
            
    class _iterator:
        def __init__(self, stream, raw):
            self.stream = stream
            self.raw = raw
        def __iter__(self):
            return self
        def next(self):
            if self.stream.i < self.stream.dr.eventsInFile():
                self.stream.i += 1
                blob = self.stream.dr.getData()
                if self.raw:
                    return blob
                else:
                    return self.check_version(blob)
            else:
                try:
                    self.stream._updateDataReader()
                    return self.next()
                except IndexError:
                    self.stream.f = -1
                    try:
                      self.stream._updateDataReader()
                    except PauseIterationException:
                      pass # we actually need to stop
                    raise StopIteration
        def check_version(self, blob):
            # check for people trying old versions and convert it on the spot
            fragment_version = helper.Version(blob[3])
            if fragment_version.major_version() != helper.MAJOR_DEFAULT_VERSION:
                current_version = helper.Version()
                logging.debug("Converting from version %s to %s" % \
                              (fragment_version.human_major(), 
                               current_version.human_major()))
                blob = convert_old(blob)
        
            if blob[0] == helper.HeaderMarker.FULL_EVENT:
                return FullEventFragment(blob)
            else:
                raise SyntaxError, ("Expecting event marker, not 0x%08x" % 
                                    blob[0])


################################################################################
#                                   Tests                                      #
################################################################################

import unittest

class dif_pausable_istream_test_case(unittest.TestCase):
    def setUp(self):
        self.stream = pausable_istream(datafiles)
    
    def testCycle(self):
        for i in range(2):
            self.aux_testCycle()
        self.assertEquals(self.stream.i, 0)
        self.assertEquals(self.stream.f, 0)
        
    def aux_testCycle(self):
        try:
            for e in self.stream:
                pass
        except PauseIterationException:
            pass
            
        
class fixed_pausable_istream_test_case(unittest.TestCase):
    def setUp(self):
        self.f = datafiles[0]
        self.rnum = 177531
        self.numev = 100
        self.stream = pausable_istream([self.f] * 2)
    def test_run_number(self):
        self.assertEquals(self.stream.current_run_number(), self.rnum)
    def test_run_number_event(self):
        self.assertEquals(self.stream.__iter__().next().run_no(), self.rnum)
    def test_run_number_events(self):
        rn = self.stream.__iter__().next().run_no()
        for e in self.stream:
            if self.stream.i == self.numev:
                break
            self.assertEquals(e.run_no(), rn)
    def test_detector_mask(self):
        dm = self.stream.current_detector_mask()
        self.assert_(dm > 0 and dm < 0xffffffffffffffffffffffffffffffffL)
    def testIter(self):
        for e in self.stream:
            if self.stream.i == self.numev:
                break # we get out when the first file was processed
        self.stream.__iter__().next()
        self.assertEquals(self.stream.i, 1)
        self.assertEquals(self.stream.f, 1)
        for e in self.stream:
            if self.stream.i == self.numev:
                break # we get out again - end of second file
        self.assertRaises(StopIteration, self.stream.__iter__().next)
    def testCycle(self):
        try:
            for e in self.stream:
                pass
        except PauseIterationException:
            for e in self.stream:
                pass
        for e in self.stream:
            if self.stream.i == self.numev and self.stream.f == 1:
                break
        self.assertRaises(StopIteration, self.stream.__iter__().next)

class pausable_istream_test_case(unittest.TestCase):
    def setUp(self):
        self.stream = pausable_istream(datafiles)
    def testIterSimple(self):
        try:
            for e in self.stream:
                pass
        except PauseIterationException:
            pass
        self.assertEquals(self.stream.i, 0)
    def testIter(self):
        self.auxTestIter()
        self.assertEquals(self.stream.f, 0)
        self.assertEquals(self.stream.i, 0)
    def testCycle(self):
        self.auxTestIter()
        self.auxTestIter()
        self.assertEquals(self.stream.f, 0)
        self.assertEquals(self.stream.i, 0)
        
    def testRandomAccess(self):
        self.assertRaises(NotImplementedError, lambda: self.stream[0])
    def auxTestIter(self):
        try:
            for e in self.stream:
                pass
        except PauseIterationException:
            self.auxTestIter()
            
if __name__ == '__main__':
    from HLTTestApps import test_main
    test_main()
