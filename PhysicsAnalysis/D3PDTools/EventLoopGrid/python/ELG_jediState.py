# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def ELG_jediState(sample) :

    from pandatools import PandaToolsPkgInfo
    if int(float(PandaToolsPkgInfo.release_version[2])) < 4 :
        print "Need prun with JEDI support, try:"
        print "    localSetupPandaClient currentJedi --noAthenaCheck"
        return ''

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
