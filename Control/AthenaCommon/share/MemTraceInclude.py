import string

from AthenaCommon.Include import Include
class MemTraceInclude( Include ):
   def __init__( self, org ):
      Include.__init__( self, org._show )
      for name, value in org.__dict__.iteritems():
         setattr (self, name, value)
      import AthenaCommon.Logging as L
      self.msg = L.logging.getLogger('Athena')
      
   def __call__( self, fn, *args, **kw ):
      Include.__call__( self, fn, *args, **kw )

      from sys import platform
      if platform != 'darwin' :
          pid = os.getpid()
          statm = string.split( open( '/proc/%d/statm' % pid, 'r' ).readlines()[0] )
          stat = string.split( open( '/proc/%d/stat' % pid, 'r' ).readlines()[0] )

          self.msg.info(
             'VSIZE = %8.3f MB, SIZE = %8.3f MB, RSS = %8.3f MB',
             int(stat[22])/1048576.,
             int(statm[0])/256.,
             int(statm[1])/256.
             )
      else:
            # The following is placeholder code for the Mac to get vmem and rss in MB. This is available
            # via the C task_info() function, for which there isn't a Python wrapper yet. The following is 
         	# a hack using the 'ps' command which is certainly not optimal! The vsz and rss are claimed to
         	# be returned from the 'ps' command in units of KB, but it looks like vsz is in pages rather than 
         	# KB, so I'll scale it by that, whereas rsz does appear to be in units of KB. I don't know
         	# what the equivalent of size is so I'm returning vsize for that.
            from resource import getpagesize
            from os import popen, getpid
            pageSize = float(getpagesize())
            vsz = float(popen('ps -p %d -o %s | tail -1' % (getpid(), "vsz")).read())
            rss = float(popen('ps -p %d -o %s | tail -1' % (getpid(), "rss")).read())
            vmem = vsz*pageSize / 1024.0 / 1024.0
            rss  = rss / 1024.0
         ####    print 'getpid(%d) pageSize(%f) vmem(%f) rss(%f)' % (getpid(), pageSize, vmem, rss)
            self.msg.info(
                'VSIZE = %8.3f MB, SIZE = %8.3f MB, RSS = %8.3f MB',
                vmem,
                vmem,
                rss
                )

include = MemTraceInclude( include )

# hack for folks who call "from AthenaCommon.Include import include" from other
# python modules (those codes should be fixed ... )
sys.modules[ 'AthenaCommon.Include' ].include = include
