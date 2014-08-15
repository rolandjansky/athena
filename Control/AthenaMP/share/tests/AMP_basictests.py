import sys, os, unittest
sys.path.append( os.path.join( os.getcwd(), os.pardir ) )

from common import *

__all__ = [
   'Basic011ModuleTestCase',
   'Basic02ExecutionTestCase',
   'Basic03GroupTestCase',
   'Basic04SharedQueueTestCase',
]

if '--build' in sys.argv:
   res = os.system( "cd ../../cmt; make QUICK=1" )
   if res:
      sys.exit( res )


### basic module test cases ==================================================
class Basic01ModuleTestCase( MyTestCase ):
   def test01API( self ):
      """Test module loading and API existence"""

      import _athenamp

      self.assert_( hasattr( _athenamp, 'launch' ) )
      self.assert_( hasattr( _athenamp, 'ProcessGroup' ) )
      self.assert_( hasattr( _athenamp, 'Process' ) )
      self.assert_( hasattr( _athenamp, 'SharedQueue' ) )

   def test02ArgumentsAndErrors( self ):
      """Test basic faulty argument error handling"""

      import _athenamp

      self.assertRaises( TypeError, _athenamp.launch, 1 )

   def test03Instantiations( self ):
      """Test class instantiations"""

      import _athenamp

      proc = _athenamp.Process( -1 )
      self.assertEqual( proc.pid, -1 )

      proc = _athenamp.Process( pid = -1 )
      self.assertEqual( proc.pid, -1 )

      group = _athenamp.ProcessGroup()
      group = _athenamp.ProcessGroup( 4 )
      group = _athenamp.ProcessGroup( nprocs = 4 )

      queue = _athenamp.SharedQueue()
      queue = _athenamp.SharedQueue( 100 )


### basic execution test cases ===============================================
class Basic02ExecutionTestCase( MyTestCase ):
   def _checkChildren( self ):
    # the following tests that there are no children running
      self.assertRaises( OSError, os.wait )

   def setUp( self ):
      self._checkChildren()

   def tearDown( self ):
      self._checkChildren()

   def test01RunChild( self ): 
      """Test running and destruction of a child"""

      import _athenamp

      proc = _athenamp.launch()
      self.assert_( 0 <= proc.pid )

      if proc.pid == 0:
         import signal
         signal.pause()
      else:
         import time, signal
         time.sleep(1)
         sigtosend = signal.SIGKILL
         os.kill( proc.pid, sigtosend )
         result = os.waitpid( proc.pid, 0 )
         self.assertEqual( result[0], proc.pid )
         self.assertEqual( result[1], sigtosend )

   def test02RunChildren( self ):
      """Test running and destruction of a group of children"""

      import _athenamp

      pids = []

      leader = _athenamp.launch()
      if leader.pid == 0:     # make child wait
         import signal
         signal.pause()
      else:
         os.setpgid( leader.pid, 0 )
         pids.append( leader.pid )

      for i in range( 2 ):
         proc = _athenamp.launch();
         self.assert_( 0 <= proc.pid )

         if proc.pid == 0:    # make all children wait
            import signal
            signal.pause()
         else:
            assert leader.pid
            os.setpgid( proc.pid, os.getpgid( leader.pid ) )
            pids.append( proc.pid )

      import time, signal
      time.sleep( 1 )
      sigtosend = signal.SIGKILL
      pgid = os.getpgid( leader.pid )
      os.killpg( pgid, sigtosend )
      while pids:
         result = os.waitpid( -pgid, 0) 
         self.assert_( result[0] in pids )
         self.assertEqual( result[1], sigtosend )
         pids.remove( result[0] )


### basic group usage test cases =============================================
class Basic03GroupTestCase( MyTestCase ):
   def test01GroupLifetime( self ):
      """Test creation and life time of a group"""

      import _athenamp

      group = _athenamp.ProcessGroup( 4 )

    # nothing started yet, so waiting should simply return
      self.assertEqual( group.wait(), () )
      self.assertEqual( group.wait( 0 ), () )
      self.assertEqual( group.wait( options = 0 ), () )

   def test02RunMapAsync( self ):
      """Test no-op running of map_async on a worker group"""
      
      import _athenamp

      group = _athenamp.ProcessGroup( 4 )
      group.map_async( "exit" )
      self.assertEqual( len(group._children()), 4 )   # now instantiated

      status = group.wait()
      self.assertEqual( [ x[1] for x in status ], 4*[0,] )

   def test03PythonTaskMapAsync( self ):
      """Test running a python task via map_async on a worker group"""
      
      import _athenamp, __main__

      def myfunc():
      #  print 'called myfunc'
          return 1
      __main__.myfunc = myfunc

    # existing function with return value
      group = _athenamp.ProcessGroup( 4 )
      group.map_async( "myfunc" )
      status = group.wait()

      self.assertEqual( [ x[1] for x in status ], 4*[0,] )
      self.assertEqual( [ x[2] for x in status ], 4*[1,] )

    # non-existing function, leading to failure
      group = _athenamp.ProcessGroup( 4 )
      group.map_async( "no_such_func" )
      status = group.wait()

      self.assertEqual( [ x[1] for x in status ], 4*[0x0B,] )


### basic group usage test cases =============================================
class Basic04SharedQueueTestCase( MyTestCase ):
   def test01SharedQueueSending( self ):
      """Test put functionality of shared queue"""

      import _athenamp, random

      q = _athenamp.SharedQueue( 5 )

      r = random.Random( 1 )
      largebuf = ''.join( [ str(r.random()) for i in range(4096) ] )
      self.assertRaises( OverflowError, q.put_nowait, largebuf ) # too large for buffer

      for i in range(5):
         q.put_nowait( i )
      self.assertRaises( OverflowError, q.put_nowait, 5 )        # too many elements

      for i in range(5):
         self.assertEqual( q.get_nowait(), i )
      self.assertRaises( EOFError, q.get_nowait )

      sdata = [ "text", "text\0with\0null", "morenull\0\0" ]
      for t in sdata:
         q.put_nowait( t )
         self.assertEqual( q.get_nowait(), t )


## actual test run
if __name__ == '__main__':
   from MyTextTestRunner import MyTextTestRunner

   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = MyTextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
