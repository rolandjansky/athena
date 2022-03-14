# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# from AthenaPython.PyAthenaComps import StatusCode
from AthenaPython import PyAthena

import histgrinder
import histgrinder.interfaces
from histgrinder.HistObject import HistObject
from typing import Union, Any, Pattern, Dict, Optional
from collections.abc import Iterable, Iterator, Mapping, Collection, Generator

StatusCode = PyAthena.StatusCode

class DQPostProcessingAlg(PyAthena.Alg):
    def __init__(self, name=None, **kw):
        super(DQPostProcessingAlg, self).__init__(name, **kw)
        self.Interval = 1
        self.FileKey = '/CombinedMonitoring/run_%(run)s/'
        self.ConfigFiles = None
        self._ctr = 0
        self._run = 0
        self._transformermap = {}
        self._timings = {}
        self.DoTiming = True
        self.DoEntryOptimization = True

    def initialize(self):
        from histgrinder.config import read_configuration
        from histgrinder.transform import Transformer
        import os, glob
        from DataQualityUtils._resolve_data_path import resolve_data_path

        self.hsvc = PyAthena.py_svc('THistSvc')

        dpath = resolve_data_path("DataQualityUtils")
        if dpath is None:
            self.msg.error("Unable to resolve DataQualityUtils data path, not running new-style postprocessing")
            return StatusCode.Failure

        # read configuration & set up transformations
        self._transformers = []
        if self.ConfigFiles is None:
            postprocfiles = glob.glob(os.path.join(dpath,'postprocessing/*.yaml'))
        else:
            postprocfiles = self.ConfigFiles
        self.msg.info(f'The postprocessing config file list is {postprocfiles}')
        for configfile in glob.glob(os.path.join(dpath,'postprocessing/*.yaml')):
            config = read_configuration(configfile)
            self._transformers += [Transformer(_) for _ in config]
        selectors = set()
        for transform in self._transformers:
            selectors.update(transform.inregexes)
            if self.DoTiming:
                self._timings[transform] = 0.

        # Configure input
        self._im = AthInputModule()
        in_configuration: Mapping[str, Any] = {'source': self}
        # if args.prefix:
        in_configuration['prefix'] = f'{self.FileKey}'
        self._im.configure(in_configuration)
        self._im.setSelectors(selectors)

        # Configure output
        self._om = AthOutputModule()
        out_configuration: Mapping[str, Any] = {'target': self}
        # if args.prefix:
        out_configuration['prefix'] = f'{self.FileKey}'
        self._om.configure(out_configuration)
        return StatusCode.Success

    def _process(self):
        import time
        for obj in self._im:
            self.msg.debug(f'now processing for {obj.name}')
            translist = self._transformermap.get(obj.name, self._transformers)
            needtocache = (translist == self._transformers)
            cached = []
            for _ in translist:
                self.msg.debug(f'consider transformer {_.tc.description}')
                if self.DoTiming:
                    t0 = time.perf_counter()
                v = _.consider(obj, defer=True)
                if self.DoTiming:
                    t = time.perf_counter()-t0
                    self._timings[_] += t
                if v:
                    if needtocache:
                        cached.append(_)
                    self.msg.debug('Match made!')
                    self._om.publish(v)
            if needtocache:
                self._transformermap[obj.name] = cached
        # Process deferred transformations
        for _ in self._transformers:
            lv = _.transform()
            for v in lv:
                self._om.publish(v)
        self._om.finalize()

    def execute(self):
        self._ctr += 1
        if ((self._ctr - 1) % self.Interval) != 0: return StatusCode.Success
        self._run = self.evtStore['EventInfo'].runNumber()
        try:
            self._process()
        except Exception as e:
            import traceback
            self.msg.info(f"Caught exception: {e}")
            self.msg.info(traceback.format_exc())
        self.msg.debug("I've got it made")
        return StatusCode.Success

    def finalize(self):
        self.msg.debug("Finalizing")
        if self.DoTiming:
            self.msg.info('Timings')
            for k, v in self._timings.items():
                self.msg.info(f'{k.tc.description}, {v}')
        try:
            self._process()
        except Exception as e:
            import traceback
            self.msg.info(f"Caught transformation exception: {e}")
            self.msg.warning(traceback.format_exc())
        return StatusCode.Success

class AthInputModule(histgrinder.interfaces.InputModule):
    def __init__(self):
        self.source = None
        self.classwarnings = set()
        self.selectors = None
        self.entries = {}

    def configure(self, options: Mapping[str, Any]) -> None:
        """
        Configure this module. Potential elements of "options":
        source: should be a ROOT-openable filename or URL.
        prefix: directory path to search under. Returned histogram names
            will not include this.
        """
        if 'source' not in options:
            raise ValueError("Must specify 'source' as an "
                             "option to AthInputModule")
        self.source = options['source']
        self.prefix = options.get('prefix', '/')
        self.cachednames = set()
        self.matchednames = {}

    def setSelectors(self, selectors: Collection[Pattern]) -> None:
        """ Do more later """
        self.selectors = selectors

    def _getklass(self, k):
        import ROOT
        hsvc = self.source.hsvc
        if hsvc.existsHist(k):
            hptr = ROOT.MakeNullPointer(ROOT.TH1)
            if hsvc.getHist(k, hptr).isSuccess():
                klass = getattr(ROOT, hptr.ClassName())
                return klass
        return None

    def iterate(self, dryrun) -> Generator[HistObject, None, None]:
        """ Iterate over all histograms in THistSvc """
        import ROOT
        log = self.source.msg
        specprefix = self.prefix % { 'run': self.source._run }
        log.debug(f'Would like to match {specprefix}')
        hsvc = self.source.hsvc

        # check if we have new histograms; if so, check against selectors to see if we're interested
        currenthists = set(str(_) for _ in hsvc.getHists())
        for k in currenthists - self.cachednames:
            # log.info(f'We have ... ? {k}')
            if not k.startswith(specprefix):
                continue
            shortk = k.replace(specprefix, '', 1)
            if self.selectors is not None:
                if not any(_.match(shortk) for _ in self.selectors):
                    continue
            self.matchednames[k] = None
        self.cachednames.update(currenthists)
        log.debug(f'We now have {len(self.cachednames)} entries in our cache, of {len(currenthists)} total plots')
        log.debug(f'There are {len(self.matchednames)} matches to be considered')

        # postprocess only matched histograms
        for k, klass in self.matchednames.items():
            if dryrun:
                yield HistObject(k.replace(specprefix, '', 1), None)
                
            log.debug(f'ROOT input trying to read {k}')
            if klass is None:
                klass = self._getklass(k)
                self.matchednames[k] = klass
            hptr = ROOT.MakeNullPointer(klass)
            if hsvc.getHist(k, hptr).isSuccess():
                log.debug(f'ROOT input read {k} as {type(hptr)}')
                # obj = hptr.Clone()
                # obj.SetDirectory(0)
                obj = hptr
                if k in self.entries:
                    if obj.GetEntries() == self.entries[k]:
                        continue
                    self.entries[k] = obj.GetEntries()
                yield HistObject(k.replace(specprefix, '', 1), obj)
            else:
                log.error(f'Cannot read {k}')
            
        log.debug('Done on input side')

    def __iter__(self) -> Iterator[HistObject]:
        return self.iterate(dryrun=False)

    def warmup(self) -> Iterable[HistObject]:
        return self.iterate(dryrun=True)

class AthOutputModule(histgrinder.interfaces.OutputModule):
    def __init__(self):
        self.target = None

    def configure(self, options: Mapping[str, Any]) -> None:
        """
        Configure this module. Potential elements of "options":
        target: should be a ROOT-openable filename or URL which
            can be opened for writing.
        prefix: directory path to place results under.
        overwrite: boolean to indicate whether results should overwrite
            existing histograms in the file.
        delay: only write histograms in finalize() (not during publish()).
        """
        if 'target' not in options:
            raise ValueError("Must specify 'target' as an option "
                             "to AthInputModule")
        self.target = options['target']
        self.overwrite = bool(options.get('overwrite', True))
        self.prefix = options.get('prefix', '/')
        self.delay = bool(options.get('delay', True))
        self.queue: Optional[Dict[str, HistObject]] = {}

    def publish(self, obj: Union[HistObject, Iterable[HistObject]]) -> None:
        """ Accepts a HistObject containing a ROOT object to write to file """
        if isinstance(obj, HistObject):
            obj = [obj]
        d_obj = { _.name: _ for _ in obj }
        if self.delay:
            if not self.queue:
                self.queue = d_obj
            else:
                self.queue.update(d_obj)
        else:
            self.queue = d_obj
            self._write()
            self.queue = None

    def _write(self) -> None:
        """ write obj to THistSvc """
        import ROOT
        import os.path
        if not self.queue:
            return  # Nothing to do
        log = self.target.msg
        hsvc = self.target.hsvc
        for _, o in self.queue.items():
            ROOT.SetOwnership(o.hist, False)
            fulltargetname = os.path.join(self.prefix, o.name) % { 'run': self.target._run }
            log.debug(f"Attempt to publish {fulltargetname}")
            o.hist.SetName(os.path.basename(fulltargetname))
            if hsvc.existsHist(fulltargetname):
                # following kind of silly procedure is necessary to avoid memory leaks
                hptr = ROOT.MakeNullPointer(ROOT.TH1)
                if hsvc.getHist(fulltargetname, hptr).isSuccess():
                    hsvc.deReg(hptr)
                    ROOT.SetOwnership(hptr, True) # clean up the histogram from our side
            if not hsvc._cpp_regHist(fulltargetname, o.hist).isSuccess():
                log.error(f"Unable to register {fulltargetname}")
            else:
                log.debug("Published")
        self.queue.clear()

    def finalize(self) -> None:
        """ Writes outstanding HistObjects to file """
        self._write()