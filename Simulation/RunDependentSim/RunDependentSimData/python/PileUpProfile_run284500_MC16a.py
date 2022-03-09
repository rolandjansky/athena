# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# WARNING timestamps are derived ad hoc to put them outside run-1 or run-2 periods!
# This is based on assumptions for beta*=60cm collisions.

def setupProfile(flags, scaleTaskLength=1):

    def _evts(x):
        return int(scaleTaskLength * x)

    return [
        {'run':284500, 'lb':1, 'starttstamp':1446539185, 'evts':_evts(0), 'mu':0.5},
        {'run':284500, 'lb':2, 'starttstamp':1446539245, 'evts':_evts(0), 'mu':1.5},
        {'run':284500, 'lb':3, 'starttstamp':1446539305, 'evts':_evts(0), 'mu':2.5},
        {'run':284500, 'lb':4, 'starttstamp':1446539365, 'evts':_evts(0), 'mu':3.5},
        {'run':284500, 'lb':5, 'starttstamp':1446539425, 'evts':_evts(1), 'mu':4.5},
        {'run':284500, 'lb':6, 'starttstamp':1446539485, 'evts':_evts(1), 'mu':5.5},
        {'run':284500, 'lb':7, 'starttstamp':1446539545, 'evts':_evts(1), 'mu':6.5},
        {'run':284500, 'lb':8, 'starttstamp':1446539605, 'evts':_evts(1), 'mu':7.5},
        {'run':284500, 'lb':9, 'starttstamp':1446539665, 'evts':_evts(1), 'mu':8.5},
        {'run':284500, 'lb':10, 'starttstamp':1446539725, 'evts':_evts(3), 'mu':9.5},
        {'run':284500, 'lb':11, 'starttstamp':1446539785, 'evts':_evts(11), 'mu':10.5},
        {'run':284500, 'lb':12, 'starttstamp':1446539845, 'evts':_evts(17), 'mu':11.5},
        {'run':284500, 'lb':13, 'starttstamp':1446539905, 'evts':_evts(25), 'mu':12.5},
        {'run':284500, 'lb':14, 'starttstamp':1446539965, 'evts':_evts(30), 'mu':13.5},
        {'run':284500, 'lb':15, 'starttstamp':1446540025, 'evts':_evts(34), 'mu':14.5},
        {'run':284500, 'lb':16, 'starttstamp':1446540085, 'evts':_evts(35), 'mu':15.5},
        {'run':284500, 'lb':17, 'starttstamp':1446540145, 'evts':_evts(36), 'mu':16.5},
        {'run':284500, 'lb':18, 'starttstamp':1446540205, 'evts':_evts(40), 'mu':17.5},
        {'run':284500, 'lb':19, 'starttstamp':1446540265, 'evts':_evts(46), 'mu':18.5},
        {'run':284500, 'lb':20, 'starttstamp':1446540325, 'evts':_evts(47), 'mu':19.5},
        {'run':284500, 'lb':21, 'starttstamp':1446540385, 'evts':_evts(48), 'mu':20.5},
        {'run':284500, 'lb':22, 'starttstamp':1446540445, 'evts':_evts(49), 'mu':21.5},
        {'run':284500, 'lb':23, 'starttstamp':1446540505, 'evts':_evts(46), 'mu':22.5},
        {'run':284500, 'lb':24, 'starttstamp':1446540565, 'evts':_evts(49), 'mu':23.5},
        {'run':284500, 'lb':25, 'starttstamp':1446540625, 'evts':_evts(48), 'mu':24.5},
        {'run':284500, 'lb':26, 'starttstamp':1446540685, 'evts':_evts(55), 'mu':25.5},
        {'run':284500, 'lb':27, 'starttstamp':1446540745, 'evts':_evts(45), 'mu':26.5},
        {'run':284500, 'lb':28, 'starttstamp':1446540805, 'evts':_evts(44), 'mu':27.5},
        {'run':284500, 'lb':29, 'starttstamp':1446540865, 'evts':_evts(44), 'mu':28.5},
        {'run':284500, 'lb':30, 'starttstamp':1446540925, 'evts':_evts(40), 'mu':29.5},
        {'run':284500, 'lb':31, 'starttstamp':1446540985, 'evts':_evts(38), 'mu':30.5},
        {'run':284500, 'lb':32, 'starttstamp':1446541045, 'evts':_evts(34), 'mu':31.5},
        {'run':284500, 'lb':33, 'starttstamp':1446541105, 'evts':_evts(30), 'mu':32.5},
        {'run':284500, 'lb':34, 'starttstamp':1446541165, 'evts':_evts(27), 'mu':33.5},
        {'run':284500, 'lb':35, 'starttstamp':1446541225, 'evts':_evts(20), 'mu':34.5},
        {'run':284500, 'lb':36, 'starttstamp':1446541285, 'evts':_evts(13), 'mu':35.5},
        {'run':284500, 'lb':37, 'starttstamp':1446541345, 'evts':_evts(11), 'mu':36.5},
        {'run':284500, 'lb':38, 'starttstamp':1446541405, 'evts':_evts(8), 'mu':37.5},
        {'run':284500, 'lb':39, 'starttstamp':1446541465, 'evts':_evts(7), 'mu':38.5},
        {'run':284500, 'lb':40, 'starttstamp':1446541525, 'evts':_evts(6), 'mu':39.5},
        {'run':284500, 'lb':41, 'starttstamp':1446541585, 'evts':_evts(4), 'mu':40.5},
        {'run':284500, 'lb':42, 'starttstamp':1446541645, 'evts':_evts(2), 'mu':41.5},
        {'run':284500, 'lb':43, 'starttstamp':1446541705, 'evts':_evts(1), 'mu':42.5},
        {'run':284500, 'lb':44, 'starttstamp':1446541765, 'evts':_evts(1), 'mu':43.5},
        {'run':284500, 'lb':45, 'starttstamp':1446541825, 'evts':_evts(1), 'mu':44.5}
        #--> end
    ]
