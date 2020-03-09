# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from six import print_

def label(code):
    if isinstance(code, str):
        return ('~', 0, code)    # built-in functions ('~' sorts at the end)
    else:
        return '%s %s:%d' % (code.co_name, code.co_filename, code.co_firstlineno)



class KCacheGrind(object):
    def __init__(self, profiler):
        self.data = profiler.getstats()
        self.out_file = None

    def output(self, out_file):
        self.out_file = out_file
        print_('events: Ticks', file=out_file)
        self._print_summary()
        for entry in self.data:
            self._entry(entry)

    def _print_summary(self):
        max_cost = 0
        for entry in self.data:
            totaltime = int(entry.totaltime * 1000)
            max_cost = max(max_cost, totaltime)
        print_('summary: %d' % (max_cost,), file=self.out_file)

    def _entry(self, entry):
        out_file = self.out_file
        code = entry.code
        inlinetime = int(entry.inlinetime * 1000)
        #print >> out_file, 'ob=%s' % (code.co_filename,)
        if isinstance(code, str):
            print_('fi=~', file=out_file)
        else:
            print_('fi=%s' % (code.co_filename,), file=out_file)
        print_('fn=%s' % (label(code),), file=out_file)
        if isinstance(code, str):
            print_('0 ', inlinetime, file=out_file)
        else:
            print_('%d %d' % (code.co_firstlineno, inlinetime), file=out_file)
        # recursive calls are counted in entry.calls
        if entry.calls:
            calls = entry.calls
        else:
            calls = []
        if isinstance(code, str):
            lineno = 0
        else:
            lineno = code.co_firstlineno
        for subentry in calls:
            self._subentry(lineno, subentry)
        print_(file=out_file)

    def _subentry(self, lineno, subentry):
        out_file = self.out_file
        code = subentry.code
        totaltime = int(subentry.totaltime * 1000)
        #print >> out_file, 'cob=%s' % (code.co_filename,)
        print_('cfn=%s' % (label(code),), file=out_file)
        if isinstance(code, str):
            print_('cfi=~', file=out_file)
            print_('calls=%d 0' % (subentry.callcount,), file=out_file)
        else:
            print_('cfi=%s' % (code.co_filename,), file=out_file)
            print_('calls=%d %d' % (
                subentry.callcount, code.co_firstlineno), file=out_file)
        print_('%d %d' % (lineno, totaltime), file=out_file)
