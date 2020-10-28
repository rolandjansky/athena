# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Parser for the PDGTABLE.MeV file

    Creates a compact object that holds configuration
    for the ExtraParticlesPhysicsTool.

    Gaudi cannot parse nested std::map objects, so the extra
    particles are converted into a

      std::map< std::string, std::vector< double > >

    object and ordering of the values is therefore important.

    Rows are:
        name
        mass [MeV]
        width [MeV]
        pdg
        lifetime [ns]

    Charge, spin, parity, and isospin3 are set to zero.

    AUTHOR: miha muskinja
    DATE: August 2019

"""

import os

from AthenaCommon.SystemOfUnits import MeV, joule
from AthenaCommon.PhysicalConstants import hbar_Planck, h_Planck
from AthenaCommon.Logging import logging


class ExtraParticle(object):
    name = ""
    mass = -1
    width = -1
    charge = 0
    pdg = 0
    lifetime = h_Planck / joule

    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)

    def __str__(self):
        string = ''
        string += 'name: %s\n' % self.name
        string += '  mass: %s\n' % self.mass
        string += '  width: %s\n' % self.width
        string += '  charge: %s\n' % self.charge
        string += '  pdg: %s\n' % self.pdg
        string += '  lifetime: %s\n' % self.lifetime
        return string

    def __neg__(self):
        antiParticle = ExtraParticle(**self.__dict__)
        antiParticle.name = self.getAntiName()
        antiParticle.charge *= -1
        antiParticle.pdg *= -1
        return antiParticle

    def getAntiName(self):
        if self.name[-1] == '0':
            if 'anti_' in self.name:
                return self.name.replace('anti_', '')
            else:
                return 'anti_' + self.name
        elif self.name[-1] == '+':
            return self.name.replace('+', '-')
        elif self.name[-1] == '-':
            return self.name.replace('-', '+')


class PDGParser(object):

    def __init__(self, table, ranges):
        self.log = logging.getLogger(__name__)
        self.table = table
        self.ranges = ranges
        self.extraParticles = {}
        self.parsePDGTABLE()

    def accept(self, pdg):
        """Function to determine which extra particles are added

        Function reads the ExtraParticlesRanges property in SimFlags
        and evaluates whether the particle should be accepted.

        For example, '111-556,1112-9090226' matches everything from
        111 to 555 and 1112 to 9090225.
        """
        ranges = [r.split("-") for r in self.ranges.split(",")]
        for r in ranges:
            if int(r[0]) <= pdg < int(r[1]):
                return True

    def getPDGTABLE(self):
        # Delete a local file if present
        if os.path.isfile(self.table):
            os.remove(self.table)
        # Grab the file
        os.system('get_files -data %s' % self.table)

    def parsePDGTABLE(self):

        # retreive the PDGTABLE file
        self.getPDGTABLE()

        # parse the PDGTABLE
        with open(self.table, 'r') as f:
            for line in f:
                if line.startswith('*'):
                    # Comments start with '*'
                    continue
                splitLine = line.split()

                # Name of the particle
                baseName = splitLine[-2]

                # Number of particle entries
                charges = splitLine[-1].split(',')

                # Mass or Width
                prop = ''
                symbol = splitLine[0]
                if symbol == 'M':
                    prop = 'mass'
                elif symbol == 'W':
                    prop = 'width'
                else:
                    raise ValueError(
                        'Unidentified symbol %s for particle %s' % (
                            symbol, baseName))

                pdgs = splitLine[1:1+len(charges)]
                value = float(splitLine[1+len(charges)])

                for pdg, charge in zip(pdgs, charges):
                    if not self.accept(int(pdg)):
                        continue
                    name = self.formatName(baseName, charge)
                    kwargs = dict()
                    kwargs.setdefault('name', name)
                    kwargs.setdefault(prop, value * MeV)
                    kwargs.setdefault('pdg', int(pdg))
                    kwargs.setdefault('charge', self.formatCharge(charge))
                    if name not in self.extraParticles.keys():
                        self.extraParticles[name] = ExtraParticle(**kwargs)
                    else:
                        if getattr(self.extraParticles[name], prop) != -1:
                            self.log.warning(
                                "Property %s is already"
                                "set for particle %s."
                                "Current value is %s and"
                                "incoming value is %s.",
                                prop, name,
                                getattr(self.extraParticles[name], prop),
                                value)
                            continue
                        setattr(self.extraParticles[name], prop, value)

        for name in self.extraParticles:
            # calculate lifetime
            width = self.extraParticles[name].width
            if width > 0:
                self.extraParticles[name].lifetime = hbar_Planck/width

        # create anti-particles
        for name in [x for x in self.extraParticles.keys()]:
            antiParticle = -self.extraParticles[name]
            self.extraParticles.update({antiParticle.name: antiParticle})

    def formatName(self, name, charge):
        nameOut = name
        nameOut = nameOut.replace("*", "_star").replace("'", "_prime")
        nameOut += charge
        return nameOut

    def formatCharge(self, charge):
        # check that all characters are the same
        # e.g.: '0', '+', '--', ...
        if not (charge == len(charge) * charge[0]):
            raise ValueError('Unexpected charge %s' % charge)

        # parse string
        if '+' in charge:
            return len(charge)
        elif '-' in charge:
            return -len(charge)
        elif '0' in charge:
            return 0
        else:
            raise ValueError('Unexpected charge %s' % charge)

    def createList(self):

        # make a new whitelist for GenParticleSimWhiteList
        with open('G4particle_whitelist_ExtraParticles.txt', 'w') as writer:
            for name in self.extraParticles:
                writer.write('%s\n' % self.extraParticles[name].pdg)

        # generate output in correct format
        outDict = dict()
        for name in self.extraParticles:
            outDict.update({name: [
                self.extraParticles[name].mass,
                self.extraParticles[name].width,
                self.extraParticles[name].charge,
                self.extraParticles[name].pdg,
                self.extraParticles[name].lifetime
            ]})

        return outDict