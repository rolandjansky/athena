# This code is almost identical to ColourFlow_LHE.py by Fabian Wilk
# It provides an interface to reading, writing, and possibly modify events in lhe files

from AthenaCommon import Logging
LHEToolsLog = Logging.logging.getLogger('MCJobOptionUtils/LHETools.py')

import os
import subprocess
import glob
import collections
import re


class Record(object):
    def __init__(self, **kwargs):
        self.invalid = not (set(kwargs.keys()) == set(self.FIELD_NAMES))
        self.__dict__.update({field: self.FIELD_TYPES[field](value)
                              for field, value in kwargs.items()})

    @classmethod
    def from_string(cls, string):
        if len(cls.FIELD_NAMES) != len(string.split()):
            LHEToolsLog.warning("Inconsistent number of fields in lhe file line:")
            LHEToolsLog.warning(string)
            return cls()
        try:
            return cls(**dict(zip(cls.FIELD_NAMES, string.split())))
        except:
            LHEToolsLog.warning("Impossible to interpret lhe file line:")
            LHEToolsLog.warning(string)
            return cls()

    def to_string(self):
        return self.FMT.format(**{field: getattr(self, field) for field in self.FIELD_NAMES}) if not self.invalid else ""


class EventInfo(Record):
    #: These are the allowed field names for each LHE Event Info Record.
    FIELD_NAMES = ['nparticles', 'id', 'weight', 'scale', 'A_QED', 'A_QCD']

    #: These are the python types of each field.
    FIELD_TYPES = {
        'nparticles': int,
        'id': int,
        'weight': float,
        'scale': float,
        'A_QCD': float,
        'A_QED': float
    }

    #: This is the formatting string.
    FMT = "{nparticles:7d} {id:6d} {weight: .5E} {scale: .5E} {A_QED: .5E} {A_QCD: .5E}"


class Particle(Record):
    #: These are the allowed field names for each LHE Particle Record.
    FIELD_NAMES = [
        'pdg_id', 'status', 'mother0', 'mother1', 'colour0', 'colour1', 'px',
        'py', 'pz', 'e', 'm', 'lifetime', 'spin'
    ]

    #: These are the python types of each field.
    FIELD_TYPES = {
        'pdg_id': int,
        'status': int,
        'mother0': int,
        'mother1': int,
        'colour0': int,
        'colour1': int,
        'px': float,
        'py': float,
        'pz': float,
        'e': float,
        'm': float,
        'lifetime': float,
        'spin': float
    }

    #: This is the formatting string.
    FMT = "{pdg_id:8d} {status:5d} {mother0:5d} {mother1:5d} {colour0:5d} {colour1:5d} {px: .9E} {py: .9E} {pz: .9E} {e: .9E} {m: .9E} {lifetime: .5E} {spin: .3E}"

    def mothers(self):
        return (self.event.particles[mo]
                for mo in xrange(self.mother0 - 1, self.mother1))

    def daughters(self):
        self_id = self.index() + 1
        for p in self.event:
            if p.mother0 <= self_id <= p.mother1:
                yield p

    def index(self):
        return self.event.particles.index(self)


class Event(object):
    def __init__(self, info, particles, extra_lines=None, spurious_event_markup=False):
        self.info = info
        self.particles = particles
        self.extra_lines = extra_lines or []
        self.spurious_event_markup = spurious_event_markup

        for p in self.particles:
            p.event = self

    @classmethod
    def from_lines(cls, lines):
        info = None
        particles = []
        extra_lines = []
        spurious_event_markup = False
        event_markup = re.compile('<event>')

        for i, datum in enumerate(lines):
            # there shouldn't be a <event> markup in the middle of the event record; if yes, we shouldn't keep the event
            if event_markup.match(datum):
                LHEToolsLog.warning("Spurious <event> markup was found in the middle of an event record. Probably the result of one event being incompletely written.")
                spurious_event_markup = True
            
            # The first line is the info block.
            if not info:
                info = EventInfo.from_string(datum)
                if info.invalid:
                    LHEToolsLog.warning("Could not retrieve EventInfo from input lhe file")
                continue

            # The lines [1, N_PARTICLE] define particles.
            if 1 <= i <= info.nparticles:
                p = Particle.from_string(datum)
                if p.invalid:
                    LHEToolsLog.warning("Could not retrieve Particle from input lhe file")
                particles.append(p)
                continue

            # All subsequent lines are additional configuration which is
            # retrieved but isn't parsed.
            extra_lines.append(datum)

        return cls(info=info,
                   particles=particles,
                   extra_lines=extra_lines,
                   spurious_event_markup=spurious_event_markup)

    def is_buggy(self):
        return self.info.invalid or self.spurious_event_markup or any(part.invalid for part in self.particles)

        
    def to_string(self):
        lines = [self.info.to_string() + '\n']
        for p in self.particles:
            lines.append(p.to_string() + '\n')
        lines.extend(self.extra_lines)

        return ''.join(lines)

    def __len__(self):
        return len(self.particles)

    def __getitem__(self, index):
        if not isinstance(index, (int, long)):
            LHEToolsLog.error("Index must be integer")
            raise TypeError

        return self.particles[index]

    def __delitem__(self, index):
        if not isinstance(index, (int, long)):
            LHEToolsLog.error("Index must be integer")
            raise TypeError

        # When removing a particle at a location ``index`` -- which corresponds
        # to LHE id ``i_lhe`` -- we need to correct two things:
        #
        #   1.) Any particle that had ``i_lhe`` as mother, needs to get the
        #       mother of the particle to be removed as mother.
        #
        #          - If both have exactly one mother, perform the replacement
        #            verbatim.
        #          - If the particle that is deleted has two mothers but the
        #            child particle has only one (the particle to be deleted),
        #            set the two mother reference of the child particle to the
        #            ones of the particle to be deleted.
        #          - If the particle that is deleted has one mother but the
        #            child particle has two, abort.
        #
        #   2.) Since we've changed the LHE indexing, any particle that had a
        #       mother reference ``mother > i_lhe`` must get a new mother
        #       reference ``mother - 1``
        p_rm = self[index]
        i_lhe = index + 1

        mo0, mo1 = p_rm.mother0, p_rm.mother1
        for p in self.particles:
            if p is p_rm:
                continue

            if p.mother0 == i_lhe or p.mother1 == i_lhe:
                if p.mother0 != p.mother1:
                    LHEToolsLog.error("Can not delete particle whose child has also other parents")
                    raise RuntimeError

                p.mother0 = mo0
                p.mother1 = mo1

        # Remove the particle.
        del self.particles[index]

        for p in self.particles:
            if p.mother0 > i_lhe:
                p.mother0 -= 1
            if p.mother1 > i_lhe:
                p.mother1 -= 1

        # Decrement the particle counter.
        self.info.nparticles -= 1


def FindDuplicates(inFileName=None, crashMode=False, printDuplicated=False):
    if inFileName == None:
        LHEToolsLog.error("No input file for Duplicate finder")
        raise RuntimeError

    if printDuplicated:
        LHEToolsLog.info("Possible duplicated events in lhe file will be printed out if any")

    if printDuplicated:
        LHEToolsLog.info("Possible duplicated events in lhe file will be printed out if any")
    
    # writing awk script to be executed on the input lhe file
    fScript = open("finder.awk", 'w')
    fScript.write(
"""BEGIN{RS="<event>"; ORS=""; FS="</event>"; OFS=""; nDuplicates=0}
{
if (NR==1) {
    print $1 > \""""+inFileName+"-new"+"""\"
    ORS=\"\\n\"
}
else {
    c[$1]++;
    if (c[$1]==1) {print \"<event>\"$1\"</event>\" > \""""+inFileName+"-new"+"""\" }
    else {print \"<event>\"$1\"</event>\" > \""""+inFileName+"-duplicates"+"""\"; nDuplicates++}
}
}
END{print \"</LesHouchesEvents>\\n\" > \""""+inFileName+"-new"+"""\"; print nDuplicates}""")
    fScript.close()

    command = 'awk -f finder.awk '+inFileName+" > nDuplicates.txt"
    try:
        os.system(command)
    except:
        LHEToolsLog.error("Impossible to execute awk script")
        raise RuntimeError

    # read the number of duplicate events from temp file
    fnDuplicates = open("nDuplicates.txt", 'r')
    s = fnDuplicates.read().rstrip()
    fnDuplicates.close()

    if int(s) >0:
        fDuplicates = open(inFileName+"-duplicates", 'r')
        # some usefull prints
        LHEToolsLog.info(s+" duplicate events were found in "+inFileName)
        if printDuplicated:
            LHEToolsLog.info("Possible duplicated events in lhe file will be printed out if any")
            for line in fDuplicates:
                LHEToolsLog.info(line)
        fDuplicates.close() # don't need it anymore
        # either crash of replace file
        if(crashMode):
            LHEToolsLog.error("Duplicate events found. To avoid this crash, use crashmode=False. Exiting...")
            raise RuntimeError
        else:
            LHEToolsLog.info("Duplicate events found. Replacing input file "+inFileName+" by fixed file "+inFileName+"-new")
            LHEToolsLog.info("Old file saved in "+inFileName+"-old")
            os.rename(inFileName,inFileName+"-old")
            os.rename(inFileName+"-new",inFileName)
    else:
        LHEToolsLog.info("No duplicate events was found in "+inFileName)
        os.remove(inFileName+"-new") # removing the useless "-new" file
