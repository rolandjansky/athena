# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def ELG_jediState(sample) :

    try:
        from pandatools import PandaToolsPkgInfo
    except:
        print "prun needs additional setup, try:"
        print "    lsetup panda"
        return 99

    jediTaskID = int(sample.getMetaDouble("nc_jediTaskID", 0))

    if jediTaskID < 100 :
        print "Sample " + sample.name() + " does not have a jediTaskID"
        return ''

    from pandatools import Client

    taskDict = {}
    taskDict['jediTaskID'] = jediTaskID
    ret = Client.getJediTaskDetails(taskDict, False, True)
    if ret[0] != 0 :
        print "Problem checking status of task %s with id %s" % (sample.name(), jediTaskID)
        return ''

    return ret[1]['status']
