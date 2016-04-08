# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: Hephaestus/Auditor.py
# Author Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '1.1.0'
__author__ = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

import AthenaPython.PyAthena as PyAthena

GlobalSettings = { 'label' : 'hephaestus', 'event-based' : True }

class HephaestusAuditor( PyAthena.Aud ):
    def __init__( self, name = "HephaestusAuditor", mode = 'leak-check', **kw ):
        kw[ 'name' ] = name
        super( HephaestusAuditor, self ).__init__( **kw )

      # event counter
        self._evt = 0

      # settable parameter: subselection to audit on
        try:
           self.auditOn = kw[ 'auditOn' ]
        except KeyError:
           self.auditOn = [ 'execute' ]

      # settable parameter: determine event boundaries on the master sequence
        try:
           self.topSequenceName = kw[ 'topSequenceName' ]
        except KeyError:
           self.topSequenceName = 'AthMasterSeq'

        self.__setupMemoryTracker( mode, True )

    def __setupMemoryTracker( self, mode, startup = True ):
      # get going immediately, so that this is the first memory hook
        import MemoryTracker
        self._memtrack = MemoryTracker

        if mode == 'leak-check':
            self._start = self._memtrack.start
            self._stop  = self._memtrack.stop

            if startup or 'full' in self.auditOn:
                self._memtrack.save()
                self._memtrack.install()

        elif mode == 'delete-check':
            import DeleteChecker
            self._start = DeleteChecker.start
            self._stop  = DeleteChecker.stop

        else:
            raise RuntimeError( 'unknown mode: %s (expected leak-check or delete-check)' % mode )

      # for full, just get going and never look back
        if 'full' in self.auditOn:
            self._start()

    def initialize( self ):
        if 'full-athena' in self.auditOn:
            self._start()

        if (self._memtrack.configure() & self._memtrack.PROFILE):
            if not GlobalSettings[ 'event-based' ]:
                self._memtrack._profname( GlobalSettings[ 'label' ] )

        return PyAthena.StatusCode.Success

    def finalize( self ):
        if 'full-athena' in self.auditOn:
            self._stop()

        return PyAthena.StatusCode.Success

    def before( self, evt_name, comp_name ):
        if not self.auditOn or evt_name.lower() in self.auditOn:
            if (self._memtrack.configure() & self._memtrack.PROFILE):
                if GlobalSettings[ 'event-based' ] and evt_name.lower() == 'execute'\
                       and comp_name == self.topSequenceName:
                    self._evt += 1
                    self._memtrack._profname( GlobalSettings[ 'label' ] + '_execute.%d' % self._evt )
            self._start()
        return

    def after( self, evt_name, comp_name, sc ):
        if not self.auditOn or evt_name.lower() in self.auditOn:
            self._stop()
        return

   # pickle support
    def __getstate__( self ):
      # determine mode used
        mode = 'leak-check'
        if self._memtrack.start != self._start:
           mode = 'delete-check'

      # the base class will try to pickle all parts of the dict indiscriminately ...
        mtmod = self._memtrack
        del self._memtrack
        dct = super( HephaestusAuditor, self ).__getstate__()

      # but need to be kept alive if the job does not end on the pickle (resetting
      # in __setupMemoryTracker is fine, since it is cached in sys.modules)
        self._memtrack = mtmod

      # store mode used
        dct[ 'mode' ] = mode

      # store the module-level variables, as they may have been modified
        dct[ 'GlobalSettings' ] = GlobalSettings

      # store the pid of the current process: if we get re-loaded into the same
      # process, then no restart should be done (this isn't foolproof, but there
      # is no current way of checking otherwise)
        import os
        dct[ 'pid' ] = os.getpid()

        return dct

    def __setstate__( self, dct ):
        super( HephaestusAuditor, self ).__setstate__( dct )

      # copy the values instead of resetting the reference, in case it has
      # been imported somewhere
        global GlobalSettings
        for k,v in dct[ 'GlobalSettings' ].items():
            GlobalSettings[ k ] = v

        import os
        self.__setupMemoryTracker( dct[ 'mode' ], dct[ 'pid' ] != os.getpid() )

      # the following is a guess; similar problem really as start(), really
        if dct[ 'pid' ] == os.getpid():
            self._evt = dct[ '_evt' ]
        else:
            self._evt = 0
