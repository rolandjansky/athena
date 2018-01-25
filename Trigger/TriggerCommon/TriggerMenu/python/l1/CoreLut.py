# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class CoreLut(object):

    # LUT  0-19  of size 15 : tip   0 - 299
    # LUT 20-36  of size 12 : tip 300 - 503
    # LUT    37  of size  8 : tip 504 - 511

    # maps LUT number to index of first TIP
    lutDef = {
        0 : 0,
        1 : 15,
        2 : 30,
        3 : 45,
        4 : 60,
        5 : 75,
        6 : 90,
        7 : 105,
        8 : 120,
        9 : 135,
        10 : 150,
        11 : 165,
        12 : 180,
        13 : 195,
        14 : 210,
        15 : 225,
        16 : 240,
        17 : 255,
        18 : 270,
        19 : 285,
        20 : 300,
        21 : 312,
        22 : 324,
        23 : 336,
        24 : 348,
        25 : 360,
        26 : 372,
        27 : 384,
        28 : 396,
        29 : 408,
        30 : 420,
        31 : 432,
        32 : 444,
        33 : 456,
        34 : 468,
        35 : 480,
        36 : 492,
        37 : 504
        }


    def __init__(self):
        pass

    @staticmethod
    def getLutSize(lutnumber):
        if lutnumber>=0  and lutnumber<20 : return 15
        if lutnumber>=20 and lutnumber<37 : return 12
        if lutnumber==37:                   return 8
        raise RuntimeError("CoreLut.py getLutSize(): LUT number %i does not exist" % lutnumber)


    @staticmethod
    def getLutRange(lutnumber):
        if lutnumber>=0 and lutnumber<20 :
            begin = 15 * lutnumber
            end   = begin + 14
        elif lutnumber>=20 and lutnumber<37 :
            begin = 300 + 12 * (lutnumber-16)
            end   = begin + 11
        elif lutnumber==37:
            begin = 504
            end   = 511
        else:
            raise RuntimeError("CoreLut.py getLutRange(): LUT number %i does not exist" % lutnumber)
        return (begin,end)



    @staticmethod
    def getLutAndBitFromTIP(tip):
        tip = int(tip)
        if tip>=0 and tip<300:
            lut = tip/15
        elif tip>=300 and tip<504:
            lut = (tip-300)/12 + 20
        elif tip>=504 and tip<512:
            lut = 37
        else:
            raise RuntimeError("CoreLut.py getLutAndBitFromTIP(tip): tip %i does not exist" % tip)
        return lut



    @staticmethod
    def getLutAndBit(connector, signal):
        firstTIPofConnector = [320, 384, 448] # last if 511

        tip = firstTIPofConnector[ connector ] + signal
            
        return CoreLut.getLutAndBitFromTIP(tip)


    @staticmethod
    def isOnOneLut(connector, phase, listOfCableBits):

        occupiedLuts = set([ CoreLut.getLutAndBit(connector,2*b + phase)[0] for b in listOfCableBits])

        if len(occupiedLuts)>1:
            raise RuntimeError("logic stretches over more than one LUT")

coreLut = CoreLut()
