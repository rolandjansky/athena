# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from histgrinder.interfaces import InputModule, OutputModule
from histgrinder.HistObject import HistObject
from typing import (Union, Iterable, Mapping, Any, Collection,
                    Pattern, Generator)
import logging
import oh


def readobj(part, server, provider, k, dryrun):
    import oh
    if not dryrun:
        obj = oh.getRootObject(part, server, provider, k)
    else:
        obj = None
    return obj

class Receiver(oh.OHRootReceiver):
    def __init__(self, q, prefix):
        super(Receiver, self).__init__()
        self.queue = q
        self.prefix = prefix

    def receive(self, obj):
        name = obj.GetName().replace(self.prefix, '')
        try:
            self.queue.append(HistObject(name, obj))
        except Exception as e:
            print(e)

class OHInputModule(InputModule):
    def __init__(self):
        self.source = None
        self.classwarnings = set()
        self.selectors = None

    def configure(self, options: Mapping[str, Any]) -> None:
        """
        Configure this module. Potential elements of "options":
        source: a string in the format partition/server
        prefix: string to prepend to histogram names.
        """
        from ispy import IPCPartition
        if 'source' not in options:
            raise ValueError("Must specify 'source' as an "
                             "option to OHInputModule")
        self.source = options['source']
        self.partition, self.server, self.provider = self.source.split(';')
        self.prefix = options.get('prefix', '')
        if not IPCPartition(self.partition).isValid():
            raise ValueError(f'Input partition {self.partition} does not exist')
        log = logging.getLogger(self.__class__.__name__)
        log.info(f'Using source {self.source}')
        self.interval = options.get('interval', 5)
        log.info(f'Check interval is {self.interval} seconds')

    def setSelectors(self, selectors: Collection[Pattern]) -> None:
        """ Do more later """
        self.selectors = selectors

    def iterate(self, dryrun) -> Generator[HistObject, None, None]:
        """ Connect to OH; iterate over matching histograms """
        import ROOT
        ROOT.TH1.AddDirectory(ROOT.kFALSE)
        from oh import OHSubscriber
        from ispy import IPCPartition, InfoReader
        import time
        from collections import deque
        log = logging.getLogger(self.__class__.__name__)
        opartition = IPCPartition(self.partition)

        # Set up two reading methods: a subscriber to new histograms and
        # a first pass over all histograms.
        queue = deque()
        rec = Receiver(queue, f'{self.server}.{self.provider}.{self.prefix}')
        subs = OHSubscriber(opartition, self.server, rec, True)
        subs.subscribe_re2(self.server, self.provider, '.*', True)

        reader = InfoReader(opartition, self.server, rf'{self.provider}\..*')
        reader.update()
        log.debug('First pass update')
        for k in reader.objects:
            toread = k.replace(f'{self.server}.{self.provider}.','')
            obj = readobj(opartition, self.server, self.provider, toread, dryrun)
            log.debug('OH input read '
                      f'{k} as {toread.replace(self.prefix, "")}')
            yield HistObject(toread.replace(self.prefix, '', 1), obj)

        if not dryrun:
            log.info('First pass complete, entering update loop')
            del reader
            try:
                while True:
                    try:
                        time.sleep(self.interval)
                        if len(queue) > 0:
                            log.info('Update detected')
                        while True:
                            yield queue.popleft()
                    except IndexError:
                        pass
                    log.debug('Completed update')
            finally:
                subs.unsubscribe_re2(self.server, self.provider, '.*')

        return
        yield

    def __iter__(self) -> Iterable[HistObject]:
        return self.iterate(dryrun=False)

    def warmup(self) -> Iterable[HistObject]:
        return self.iterate(dryrun=True)


class OHOutputModule(OutputModule):
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
        import oh
        import ispy
        if 'target' not in options:
            raise ValueError("Must specify 'target' as an option "
                             "to OHInputModule")
        self.target = options['target']
        self.partition, self.server, self.provider = self.target.split(';')
        self.partition = ispy.IPCPartition(self.partition)
        if not self.partition.isValid():
            raise ValueError(f'Output partition {self.partition.name()} is not valid')
        self.prefix = options.get('prefix', '')
        self.provider = oh.OHRootProvider(self.partition,
                                        self.server, self.provider, None)
        self.queue = set()
        log = logging.getLogger(self.__class__.__name__)
        log.info(f'Using target {self.target}')

    def publish(self, obj: Union[HistObject, Iterable[HistObject]]) -> None:
        """ Accepts a HistObject containing a ROOT object to write to file """
        import os
        if isinstance(obj, HistObject):
            obj = [obj]

        for o in obj:
            self.provider.publish(o.hist, os.path.join(self.prefix, o.name))

    def finalize(self) -> None:
        """ Writes outstanding HistObjects to file """
        pass


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print("Provide two arguments (input and output files)")
        sys.exit(1)
    im = OHInputModule()
    im.configure({'source': sys.argv[1]})
    om = OHOutputModule()
    om.configure({'target': sys.argv[2]})
    for o in im:
        print(o, o.hist.GetMean())
        om.publish(o)
