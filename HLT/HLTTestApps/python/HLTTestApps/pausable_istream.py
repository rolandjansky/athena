# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Ricardo Abreu <ricardo.abreu@cern.ch>
import eformat
from eformat import EventStorage
from libpyeformat import FullEventFragment, convert_old 
import libpyeformat_helper as helper
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
        self.i = 0
        self.f = 0
        self.dr = EventStorage.pickDataReader(self.filelist[self.f])
        
    def current_filename(self):
      """ Obtain the file name of the current file"""
      return self.dr.fileName()
        
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

    def datawriter(self, directory, core_name, compression=0):
      """ Creates and returns a new eformat.ostream with the same meta data of 
      the current input stream, but using the directory and core_name as given.
      """
      
      compargs = {}
      if compression in range(1,6):
        compargs['compression'] = EventStorage.CompressionType.ZLIB
        compargs['complevel'] = compression
        
      return eformat.ostream(directory, core_name, self.dr.runNumber(),
                             self.dr.triggerType(), self.dr.detectorMask(),
                             self.dr.beamType(), self.dr.beamEnergy(),
                             **compargs)
            
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

import unittest, string, random, glob, os

class dif_pausable_istream_tests(unittest.TestCase):
    def setUp(self):
        self.stream = pausable_istream(datafiles)
    
    def testCycle(self):
        for i in range(2):
            self.aux_testCycle()
        self.assertEquals(self.stream.i, 0)
        self.assertEquals(self.stream.f, 0)
        
    def testRewindInFirstFile(self):
        self._testRewind(50) # files have 100 and 99 events respectively
                
    def testRewindInSecondFile(self):
        self._testRewind(150) # files have 100 and 99 events respectively
        
    def testRewindAfterCycle(self):
        self._testRewind(250) # files have 100 and 99 events respectively
        
    def aux_testCycle(self):
        try:
            for e in self.stream:
                pass
        except PauseIterationException:
            pass
            
    def _testRewind(self, n):
        # advance n events
        evs1 = self._extract_first_n_events(n)
        
        # now rewind and check we really are at the beginning
        self.stream.rewind()
        self.assertEquals(self.stream.i, 0)
        self.assertEquals(self.stream.f, 0)
        self.assertEquals(self.stream.dr.fileName(), self.stream.filelist[self.stream.f])
        
        #repeat and confirm we get the same events as before
        evs2 = self._extract_first_n_events(n)
        self.assertEquals(evs1, evs2)
        
    def _extract_first_n_events(self, n):
        evs = []
        while True:
            try:
                for e in self.stream:
                    evs.append(e)
                    if len(evs) == n:
                        return evs
            except PauseIterationException:
                pass
        
class fixed_pausable_istream_tests(unittest.TestCase):
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

class some_pausable_istream_tests(unittest.TestCase):
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
            
class pausable_istream_files_tests(unittest.TestCase):
  def setUp(self):
    self.tmpdir = "/tmp"
    self.tmpbasefilename = "tmpoutfile_athenaHLT_pausable_istream_test_"
    self.stream = pausable_istream(datafiles)
  def tearDown(self):
    for f in glob.glob("%s/%s*" % (self.tmpdir, self.tmpbasefilename)):
      os.remove(f)
  def test_advance_file_once(self):
    self._test_advance_file_multiple(1)
  def test_advance_file_twice(self):
    self._test_advance_file_multiple(2)
  def test_advance_file_thrice(self):
    self._test_advance_file_multiple(3)
  def test_advance_file_5times(self):
    self._test_advance_file_multiple(5)
  def test_data_writer_config_plain_fst(self):
    self._test_advance_data_writer_config_plain(0)
  def test_data_writer_config_plain_snd(self): 
    self._test_advance_data_writer_config_plain(1)
  def test_data_writer_config_plain_trd(self):
    self._test_advance_data_writer_config_plain(2)
  def _advance_file(self):
    try:
      self.stream._updateDataReader()
    except PauseIterationException:
      pass
    except IndexError:
      self.stream.f = -1
      try:
        self.stream._updateDataReader()
      except PauseIterationException:
        pass
  def _test_advance_file_multiple(self, n):
    oldf = self.stream.f
    for _ in range(n):
      self._advance_file()
    numf = len(self.stream.filelist)
    expect = (n + oldf) % numf
    self.assertEqual(self.stream.f, expect,
                     "Got unexpected file index %d after advancing %d times on "
                     "a stream with original file index %d and a total of %d "
                     "files (expected to end with file index %d)" 
                     % (self.stream.f, n, oldf, numf, expect))
  def _test_data_writer_config_plain(self):
    outf = EventStorage.pickDataReader(self._create_unique_outfile())
    for item in ["runNumber", "triggerType", "detectorMask", "beamType", 
                 "beamEnergy"]:
      r, w = getattr(self.stream.dr, item)(), getattr(outf, item)()
      self.assertEqual(r, w, "%s different in input (%s) and output (%s) "
                             "streams" % (item, str(r), str(w)))
  def _test_advance_data_writer_config_plain(self, findex):
    for _ in range(findex):
      self._advance_file()
    self._test_data_writer_config_plain()
  def _create_unique_outfile(self):
    ost = self.stream.datawriter(self.tmpdir, self._unique_filename(), 0)
    # get the final file name (ostream adds stuff to the name)
    ret = ost.last_filename() # current_filename would give the ".writable" name
    ost.writer.closeFile()
    del ost
    return ret
  def _unique_filename(self):
    return self.tmpbasefilename + ''.join([random.choice(string.ascii_letters) 
                                           for _ in range(8)])
            
if __name__ == '__main__':
    from HLTTestApps import test_main
    test_main()
