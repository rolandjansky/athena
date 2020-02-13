##
#
# @file EventAthenaPool/share/EventAthenaPoolTestDump.py
# @author sss
# @date Jul 2016
# @brief Dump EventInfo classes for regression tests.
#
# This code is mostly copied from PyDumper to avoid dependency issues.
#


from __future__ import print_function
from PyUtils.fprint import fprint
import ROOT


def toiter (beg, end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return


def formatItemUsingLong (x):
    """Helper for doing formatting compatibly with py2.

Convert X to a string, but add a `L' to the ends of integers.
Can be used for formatting expressions that using longs in py2.
"""
    if isinstance (x, list):
        sl = [formatItemUsingLong (xx) for xx in x]
        return '[' + ', '.join (sl)  + ']'
    if isinstance (x, tuple):
        sl = [formatItemUsingLong (xx) for xx in x]
        return '(' + ', '.join (sl)  + ')'
    if isinstance (x, int):
        return str(x) + 'L'
    return repr(x)


def dump_EventID (e, f):
    fprint (f, '%9d %9d %9d %9d %9d %9d' %
            (e.run_number(),
             e.event_number(),
             e.time_stamp(),
             e.time_stamp_ns_offset(),
             e.lumi_block(),
             e.bunch_crossing_id()))
    return


def dump_EventType (e, f):
    fprint (f, e.typeToString(), e.user_type(), e.mc_event_weight())
    return


def dump_TriggerInfo (e, f):
    fprint (f, e.statusElement(), e.extendedLevel1ID(), e.level1TriggerType())
    fprint (f, '\n    l2:', formatItemUsingLong ([x for x in e.level2TriggerInfo()]))
    fprint (f, '\n    ef:', formatItemUsingLong ([x for x in e.eventFilterInfo()]))
    fprint (f, '\n    st:', [(x.name(), x.type(), x.obeysLumiblock()) for x in e.streamTags()])
    return


def dump_EventInfo (e, f):
    fprint (f, e.clID())
    dump_EventID (e.event_ID(), f)
    fprint (f, '\n   ')
    fprint (f, e.actualInteractionsPerCrossing(), e.averageInteractionsPerCrossing())
    for i in range(ROOT.EventInfo.nDets):
        fprint (f, '\n     det', i, e.eventFlags(i), e.errorState(i))
    fprint (f, '\n   ')
    dump_EventType (e.event_type(), f)
    fprint (f, '\n   tg')
    dump_TriggerInfo (e.trigger_info(), f)
    return


def dump_EventStreamInfo (e, f):
    fprint (f, 'nevents: ', e.getNumberOfEvents())
    fprint (f, '\nrun numbers: ', formatItemUsingLong(list(e.getRunNumbers())))
    fprint (f, '\nlb numbers: ', formatItemUsingLong(list(e.getLumiBlockNumbers())))
    fprint (f, '\nproc tags: ', list(e.getProcessingTags()))
    fprint (f, '\nitem list: ', formatItemUsingLong([(p.first, p.second) for p in e.getItemList()]))
    for typ in list(e.getEventTypes()):
        fprint (f, '\n  ')
        dump_EventType (typ, f)
    return


def dump_PileUpEventInfo (e, f):
    dump_EventInfo (e, f)
    for (i,s) in enumerate (toiter (e.beginSubEvt(), e.endSubEvt())):
        fprint (f, '\n   subevt', i, s.time(), s.index(), s.BCID(), s.type())
    return


def get_dumper_fct(klass, ofile, nmax):
    cname = klass.__name__
    if cname == 'EventInfo':
        def dumper (o):
            return dump_EventInfo (o, ofile)
        return dumper
    elif cname == 'EventStreamInfo':
        def dumper (o):
            return dump_EventStreamInfo (o, ofile)
        return dumper
    elif cname == 'PileUpEventInfo':
        def dumper (o):
            return dump_PileUpEventInfo (o, ofile)
        return dumper
    else:
        print ('xxx', cname)
        assert 0
