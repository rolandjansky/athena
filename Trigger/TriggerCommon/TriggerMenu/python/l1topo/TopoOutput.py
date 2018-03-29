# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class TopoOutput(object):
    def __init__(self, algoname, module, fpga, clock, firstbit):
        self.algo = None
        self.sortingAlgos = []
        self.algoname = algoname
        self.module = module
        self.fpga = fpga
        self.clock = clock
        self.firstbit = firstbit

    def xml(self, ind, step=2):
        t = (self.algoname, ",".join(self.algo.outputs), self.algo.algoId, self.module, self.fpga, self.firstbit, self.clock)
        s = (' '*ind*step) + '<Output algname="%s" triggerline="%s" algoId="%i" module="%i" fpga="%i" firstbit="%i" clock="%i"/>\n' % t
        return s


    def __str__(self):
        return "%s [id=%i] --> %s" % (self.algoname, self.algo.algoId, ",".join(self.algo.outputs))



class TriggerLine(object):

    import re
    multibitPattern = re.compile("(?P<line>.*)\[(?P<bit>\d+)\]")

    def __init__(self, trigger, cable, bit, clock, fpga, ordinal, firstbit):
        """
        trigger    name of the trigger line
        cable      0 or 1, depending on the topo module
        bit
        """
        # namedtuple("TriggerLine","trigger cable bit clock fpga ordinal firstbit")
        self.trigger  = trigger 
        self.cable    = cable   
        self.bit      = bit     
        self.clock    = clock   
        self.fpga     = fpga    
        self.ordinal  = ordinal 
        self.firstbit = firstbit

    def __str__(self):
        return "{0} on cable {1}, fpga {2}, bit {3}, ordinal {4}".format(self.trigger,self.cable, self.fpga, self.bit, self.ordinal)

    @staticmethod
    def createTriggerlinesFromTopoOutput(topoOutput):

        tl = []
        for (idx,line) in enumerate(topoOutput.algo.outputs):
                
            ordinal = 64*topoOutput.module + 32*topoOutput.clock + 16*topoOutput.fpga + topoOutput.firstbit+idx
        
            tl += [ TriggerLine( trigger = line,
                                 cable = topoOutput.module,
                                 bit = topoOutput.firstbit+idx+16*topoOutput.fpga,
                                 clock = topoOutput.clock,
                                 fpga = topoOutput.fpga,
                                 ordinal = ordinal,
                                 firstbit = topoOutput.firstbit )
                    ]
        return tl
            

    @staticmethod
    def checkMultibitConsistency(multibitTriggerlines):

        cs = set()  # set to check consistency

        bitlist = []
        for tl in multibitTriggerlines:
            m = TriggerLine.multibitPattern.match(tl.trigger)

            if not m:
                raise RuntimeError("Triggerline %s does not match multibit pattern 'line[bit]'" % tl.trigger)

            cs.add( (m.groupdict()['line'], tl.bit - int(m.groupdict()['bit']), tl.cable, tl.clock, tl.fpga, tl.ordinal-int(m.groupdict()['bit'])) )

            bitlist += [ int(m.groupdict()['bit']) ]

        minimum = min(bitlist)
        numberOfBits = max(bitlist)+1
        if ( (minimum !=0) or
             (numberOfBits != len(bitlist)) or
             (len(cs) != 1) ):
            raise RuntimeError("This set of triggerlines does not consistently describe a multiplicity topo trigger output %r" % [tl.trigger for tl in multibitTriggerlines])

        uniqueDescription = cs.pop()

        (commonNameOfLines, firstbit, cable, clock, fpga, ordinal) = uniqueDescription

        return (commonNameOfLines, firstbit, numberOfBits, cable, clock, fpga, ordinal)
