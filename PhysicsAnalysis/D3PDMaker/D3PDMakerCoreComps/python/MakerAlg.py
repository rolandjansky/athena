# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: MakerAlg.py 711763 2015-11-30 21:06:16Z ssnyder $
#
# @file D3PDMakerCoreComps/python/MakerAlg.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Configure algorithm for making a D3PD tree.
#

from D3PDMakerCoreComps import D3PDMakerCoreCompsConf
from D3PDMakerCoreComps.D3PDObject import D3PDObject
import D3PDMakerCoreComps

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaPython import PyAthena

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _stream_from_file (file, seq, tuplename, streamNameRoot,
                       clevel):
    """Infer the Athena stream name from the file name.
    Create a new stream if needed.
"""

    # Make sure that THistSvc exists.
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr (ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()

    # Search existing streams for one writing to this file.
    # If we find one, return it.
    # Also build a list of the names of all existing streams.
    streams = {}
    for s in ServiceMgr.THistSvc.Output:
        stream = s.split()[0]
        streams[stream] = 1
        sfile = None
        ifile = s.find ("DATAFILE='")
        if ifile >= 0:
            ifile += 10
            i2 = s.find ("'", ifile)
            if i2 >= 0:
                sfile = s[ifile:i2]
        if sfile == file:
            return stream

    # Didn't find the stream.  We'll need to create a new one.
    # This involves making another algorithm, so we need to have
    # the sequence available.
    if not seq:
        raise TypeError ("Need to create a new stream for tuple %s, "
                         "but sequence wasn't specified." % tuplename)

    # Find a unique name for the new stream.
    stream = streamNameRoot
    istream = 1
    while streams.has_key (stream):
        stream = streamNameRoot + `istream`
        istream += 1

    # Create the stream, and add it to the sequence.
    from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
    st = AANTupleStream (stream,
                         ExtraRefNames = ['StreamESD',
                                          'StreamRDO',
                                          'StreamAOD'],
                         OutputName = file,
                         WriteInputDataHeader = True,
                         StreamName = stream)
    seq += [st]

    # Register with THistSvc.
    ServiceMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE' CL='%i'" %
                                   (stream, file, clevel)]
    return stream


class MakerAlg (D3PDMakerCoreCompsConf.D3PD__MakerAlg):
    """Configure algorithm for making a D3PD tree.

    Each distinct D3PD tree is make by a separate algorithm.
    This Configurable class is used to configure these algorithms.

    The constructor takes the following arguments:

      name: The name of the algorithm (required).
      seq:  The Gaudi sequence to which this algorithm should be added,
            or None to skip adding the algorithm to a sequence.
      file: Name of the file containing the tuple.  Only used
            if both stream and TuplePath are None.
      stream: Athena stream for the tuple.  Only used if TuplePath
              is None.  If this is supplied, the stream must have already
              been created.  If omitted, the stream will be inferred
              from the file name, and will be implicitly created if needed.
              The Gaudi sequence must be provided for this implicit
              creation to work.
      tuplename: Name to the tuple.  Defaults to the algorithm name.
                 Only used if TuplePath is None.
      TuplePath: The full name of the tuple.  If given, this overrides
                 all other specifications.  The interpretation of this
                 is up to the D3PDSvc.
      preD3PDAlgSeqName: Name of the algorithm sequence to use to schedule
               algorithms that must run before the d3pd maker.
      streamNameRoot: Name to use for generated streams.
                      We'll add a suffix, if needed, to make it unique.
      clevel: Compresson level for the output ROOT file. By default it is
              controlled by the D3PDMakerFlags.CompressionLevel flag, but
              can be controlled D3PD-by-D3PD as well.

    Any additional arguments are passed directly to D3PD::MakerAlg.


    Once the algorithm is created, object filler tools should be added
    to it using +=.  This will automatically properly set the
    collection getter registry for the contained tools.
    
    Typical usage is like this:

      alg = D3PDMakerCoreComps.MakerAlg ('testtuple', topSequence,
                                         file = 'tuplefile.root')
      alg += FooObject()
      alg += BarObject()
"""

    def __init__ (self,
                  name,
                  seq = topSequence,
                  file = None,
                  stream = None,
                  tuplename = None,
                  TuplePath = None,
                  preD3PDAlgSeqName = D3PDMakerFlags.PreD3PDAlgSeqName(),
                  streamNameRoot = None,
                  clevel = D3PDMakerFlags.CompressionLevel(),
                  **kwargs):
        """MakerAlg constructor.  See the class documentation for a full description.
"""

        if streamNameRoot == None:
            streamNameRoot = 'D3PD'

        # Work around initialization order issue.
        if seq:
            seq.__iadd__ (D3PDMakerCoreComps.DummyInitAlg (name + 'DummyInit'),
                          index = 0)

        # If the tuple path wasn't supplied, build it from the other args.
        if TuplePath == None:
            # tuple name defaults to the algorithm name.
            if tuplename == None:
                tuplename = name

            if stream == None:
                # If no stream was given, infer it from the file.
                # This creates the stream if needed.
                if file == None:
                    raise TypeError ("Neither stream nor file specified "
                                     "for tuple %s" % tuplename)
                stream = _stream_from_file (file, seq, tuplename,
                                            streamNameRoot, clevel)
            TuplePath = '/%s/%s' % (stream, tuplename)

        # Create the algorithm Configurable.
        D3PDMakerCoreCompsConf.D3PD__MakerAlg.__init__ (self, name,
                                               TuplePath = TuplePath,
                                               **kwargs)

        # Ensure configuration parameters are set.
        if (D3PDMakerFlags.AutoFlush() != -1 and
            self.D3PDSvc.getFullName() == 'D3PD::RootD3PDSvc'):
            from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
            rootsvc=D3PD__RootD3PDSvc()
            rootsvc.AutoFlush = D3PDMakerFlags.AutoFlush()
            from AthenaCommon.AppMgr import ServiceMgr
            ServiceMgr += rootsvc


        # Add to the supplied sequence.
        if seq:
            # But first, add a sequence for algorithms that should run
            # before D3PD making, if it's not already there.
            preseq = AlgSequence (preD3PDAlgSeqName)
            if not hasattr (seq, preD3PDAlgSeqName):
                seq += [preseq]

            # We don't want to do filtering in the presequence.
            preseq.StopOverride = True

            # Now set up another sequence for filtering.
            # Unlike the presequence, there should be a unique one of these
            # per algorithm.  We also need to break out an additional
            # sequence to which users can add, and to wrap the whole
            # thing in a sequence to prevent a failed filter
            # decision from stopping other algorithms.
            # Like this:
            #
            #   ALG_FilterAlgorithmsWrap (StopOverride = True)
            #     ALG_FilterAlgorithmsHolder
            #       ALG_FilterAlgorithms
            #       ALG
            #     Dummy alg, to reset filter flag

            suffix = D3PDMakerFlags.FilterAlgSeqSuffix()
            wrap = AlgSequence (name + suffix + 'Wrap',
                                StopOverride = True)
            holder = AlgSequence (name + suffix + 'Holder')
            self.filterSeq = AlgSequence (name + suffix)
            holder += self.filterSeq
            holder += self
            wrap += holder
            wrap += PyAthena.Alg (name + 'Dummy')
            
            seq += wrap


        # Create a unique collection getter registry tool for this tree.
        from AthenaCommon.AppMgr import ToolSvc
        self._registry = \
           D3PDMakerCoreComps.CollectionGetterRegistryTool (self.name() +
                                                   '_CollectionGetterRegistry')
        ToolSvc += self._registry
        return


    def __iadd__( self, configs ):
        """Add a new IObjFillerTool to a tree."""

        # FIXME: should make sure name is unique within alg.
        nchild = len (self)
        if type(configs) != type([]):
            configs = [configs]
        self.Tools += configs
        super(MakerAlg, self).__iadd__ (configs)

        for c in self.getChildren()[nchild:]:
            # Scan children to set the proper collection getter registry.
            self._setRegistry (c)

            D3PDObject.runHooks (c)
        return self


    def _setRegistry (self, conf):
        """Scan CONF and all children to set the proper
        collection getter registry for this tree.
"""
        
        if conf.properties().has_key ('CollectionGetterRegistry'):
            conf.CollectionGetterRegistry = self._registry
        for c in conf.getAllChildren():
            self._setRegistry (c)
        return
    
       



