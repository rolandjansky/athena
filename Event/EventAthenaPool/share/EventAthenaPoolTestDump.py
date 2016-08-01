##
# $Id$
#
# @file EventAthenaPool/share/EventAthenaPoolTestDump.py
# @author sss
# @date Jul 2016
# @brief Dump EventInfo classes for regression tests.
#
# This code is mostly copied from PyDumper to avoid dependency issues.
#


import ROOT


def toiter (beg, end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

def dump_EventID (e, f):
    print >> f, '%9d %9d %9d %9d %9d %9d' % \
          (e.run_number(),
           e.event_number(),
           e.time_stamp(),
           e.time_stamp_ns_offset(),
           e.lumi_block(),
           e.bunch_crossing_id()),
    return


def dump_EventType (e, f):
    print >> f, e.typeToString(), e.user_type(), e.mc_event_weight(),
    return


def dump_TriggerInfo (e, f):
    print >> f, e.statusElement(), e.extendedLevel1ID(), e.level1TriggerType(),
    print >> f, '\n    l2:', [x for x in e.level2TriggerInfo()],
    print >> f, '\n    ef:', [x for x in e.eventFilterInfo()],
    print >> f, '\n    st:', [(x.name(), x.type(), x.obeysLumiblock()) for x in e.streamTags()],
    return


def dump_EventInfo (e, f):
    print >> f, e.clID(),
    dump_EventID (e.event_ID(), f)
    print >> f, '\n   ',
    print >> f, e.actualInteractionsPerCrossing(), e.averageInteractionsPerCrossing(),
    for i in range(ROOT.EventInfo.nDets):
        print >> f, '\n     det', i, e.eventFlags(i), e.errorState(i),
    print >> f, '\n   ',
    dump_EventType (e.event_type(), f)
    print >> f, '\n   tg',
    dump_TriggerInfo (e.trigger_info(), f)
    return


def dump_EventStreamInfo (e, f):
    print >> f, 'nevents: ', e.getNumberOfEvents(),
    print >> f, '\nrun numbers: ', list(e.getRunNumbers()),
    print >> f, '\nlb numbers: ', list(e.getLumiBlockNumbers()),
    print >> f, '\nproc tags: ', list(e.getProcessingTags()),
    print >> f, '\nitem list: ', [(p.first, p.second) for p in e.getItemList()],
    for typ in list(e.getEventTypes()):
        print >> f, '\n  ',
        dump_EventType (typ, f)
    return


def dump_PileUpEventInfo (e, f):
    dump_EventInfo (e, f)
    for (i,s) in enumerate (toiter (e.beginSubEvt(), e.endSubEvt())):
        print >> f, '\n   subevt', i, s.time(), s.index(), s.BCID(), s.type(),
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
        print 'xxx', cname
        assert 0
