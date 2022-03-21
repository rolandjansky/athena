# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def setupProfile(flags, scaleTaskLength=1):

  def _evts(x):
    return int(scaleTaskLength * x)

  return [
    {'run':410000, 'lb':1, 'starttimestamp':1625000060, 'evts':_evts(1000), 'mu':51.5, 'step':0},
    {'run':410000, 'lb':2, 'starttimestamp':1625000120, 'evts':_evts(1000), 'mu':52.5, 'step':0},
    {'run':410000, 'lb':1001, 'starttimestamp':1625060060, 'evts':_evts(1000), 'mu':51.5, 'step':1},
    {'run':410000, 'lb':1002, 'starttimestamp':1625060120, 'evts':_evts(1000), 'mu':52.5, 'step':1},
    {'run':410000, 'lb':2001, 'starttimestamp':1625120060, 'evts':_evts(1000), 'mu':51.5, 'step':2},
    {'run':410000, 'lb':2002, 'starttimestamp':1625120120, 'evts':_evts(1000), 'mu':52.5, 'step':2},
    {'run':410000, 'lb':3001, 'starttimestamp':1625180060, 'evts':_evts(74), 'mu':25.5, 'step':3},
    {'run':410000, 'lb':3002, 'starttimestamp':1625180120, 'evts':_evts(74), 'mu':26.5, 'step':3},
    {'run':410000, 'lb':3003, 'starttimestamp':1625180180, 'evts':_evts(74), 'mu':27.5, 'step':3},
    {'run':410000, 'lb':3004, 'starttimestamp':1625180240, 'evts':_evts(74), 'mu':28.5, 'step':3},
    {'run':410000, 'lb':3005, 'starttimestamp':1625180300, 'evts':_evts(74), 'mu':29.5, 'step':3},
    {'run':410000, 'lb':3006, 'starttimestamp':1625180360, 'evts':_evts(74), 'mu':30.5, 'step':3},
    {'run':410000, 'lb':3007, 'starttimestamp':1625180420, 'evts':_evts(74), 'mu':31.5, 'step':3},
    {'run':410000, 'lb':3008, 'starttimestamp':1625180480, 'evts':_evts(74), 'mu':32.5, 'step':3},
    {'run':410000, 'lb':3009, 'starttimestamp':1625180540, 'evts':_evts(75), 'mu':33.5, 'step':3},
    {'run':410000, 'lb':3010, 'starttimestamp':1625180600, 'evts':_evts(74), 'mu':34.5, 'step':3},
    {'run':410000, 'lb':3011, 'starttimestamp':1625180660, 'evts':_evts(74), 'mu':35.5, 'step':3},
    {'run':410000, 'lb':3012, 'starttimestamp':1625180720, 'evts':_evts(74), 'mu':36.5, 'step':3},
    {'run':410000, 'lb':3013, 'starttimestamp':1625180780, 'evts':_evts(74), 'mu':37.5, 'step':3},
    {'run':410000, 'lb':3014, 'starttimestamp':1625180840, 'evts':_evts(74), 'mu':38.5, 'step':3},
    {'run':410000, 'lb':3015, 'starttimestamp':1625180900, 'evts':_evts(74), 'mu':39.5, 'step':3},
    {'run':410000, 'lb':3016, 'starttimestamp':1625180960, 'evts':_evts(74), 'mu':40.5, 'step':3},
    {'run':410000, 'lb':3017, 'starttimestamp':1625181020, 'evts':_evts(74), 'mu':41.5, 'step':3},
    {'run':410000, 'lb':3018, 'starttimestamp':1625181080, 'evts':_evts(75), 'mu':42.5, 'step':3},
    {'run':410000, 'lb':3019, 'starttimestamp':1625181140, 'evts':_evts(74), 'mu':43.5, 'step':3},
    {'run':410000, 'lb':3020, 'starttimestamp':1625181200, 'evts':_evts(74), 'mu':44.5, 'step':3},
    {'run':410000, 'lb':3021, 'starttimestamp':1625181260, 'evts':_evts(74), 'mu':45.5, 'step':3},
    {'run':410000, 'lb':3022, 'starttimestamp':1625181320, 'evts':_evts(74), 'mu':46.5, 'step':3},
    {'run':410000, 'lb':3023, 'starttimestamp':1625181380, 'evts':_evts(74), 'mu':47.5, 'step':3},
    {'run':410000, 'lb':3024, 'starttimestamp':1625181440, 'evts':_evts(74), 'mu':48.5, 'step':3},
    {'run':410000, 'lb':3025, 'starttimestamp':1625181500, 'evts':_evts(74), 'mu':49.5, 'step':3},
    {'run':410000, 'lb':3026, 'starttimestamp':1625181560, 'evts':_evts(74), 'mu':50.5, 'step':3},
    {'run':410000, 'lb':3027, 'starttimestamp':1625181620, 'evts':_evts(74), 'mu':51.5, 'step':3},
  ]
