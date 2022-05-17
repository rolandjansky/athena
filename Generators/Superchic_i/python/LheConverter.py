# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os
import re
import math
import xml.dom.minidom

from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode


class LheConverter(EvgenAlg):
    '''
    Class for converting output LHE file from SuperChic to LHE recognised by Pythia.
    '''

    def __init__(self, name='LheConverter'):
        super(LheConverter, self).__init__(name=name)

    inFileName = 'evrecs/evrecout.dat'
    outFileName = 'events.lhe'
    done = False
    leptons = ['11', '-11', '13', '-13', '15', '-15']  # e, µ, τ

    def initialize(self):
        if(os.path.isfile(self.inFileName)):
            print(self.fileName)

            return self.convert()
        else:
            return StatusCode.Failure

    def convert(self):
        '''
        Converts `evrecs/evrecout.dat` input file to `events.lhe` recognised by the Pythia:
            - Replaces init block
            - Changes photon px and py to zero and energy to pz
            - Recalculates energy scales as an average of lepton pair transverse momentum
        '''

        if not self.done:
            DOMTree = xml.dom.minidom.parse(self.inFileName)
            collection = DOMTree.documentElement

            # Replace init block
            init = collection.getElementsByTagName('init')
            init_repl = r'''
 13  -13  2.510000e+03  2.510000e+03  0  0  0  0  3  1
 1.000000e+00  0.000000e+00  1.000000e+00   9999
 '''
            init[0].firstChild.data = init_repl

            #                                        1 -- energy scale
            event_header = r'^(\s*\S*\s*\S*\s*\S*\s*)(\S*)(.*)$'
            #                          pdgid                                                           px        py        pz        e
            event_particle = r'^(\s*)([0-9-]+)(\s+[0-9-]+\s+[0-9-]+\s+[0-9-]+\s+[0-9-]+\s+[0-9-]+\s+)(\S+)(\s+)(\S+)(\s+)(\S+)(\s+)(\S+)(.*)$'

            events = collection.getElementsByTagName('event')
            for i, event in enumerate(events):
                new_particles = []
                lepton_mom = []

                particles = re.findall(event_particle, event.firstChild.data, re.MULTILINE)
                for particle in particles:
                    particle = list(particle)
                    if particle[1] == '22':  # photon
                        # Zero photon transverse momentum, change energy to be equal pz
                        particle[3] = '0.'
                        particle[5] = '0.'
                        particle[9] = particle[7]
                        new_particles.append(''.join(particle))
                    elif particle[1] in self.leptons:
                        # Read leptons px and py
                        lepton_mom.append(float(particle[3]))
                        lepton_mom.append(float(particle[5]))
                        new_particles.append(''.join(particle))
                    else:
                        new_particles.append(''.join(particle))

                # Calculate new energy scale
                l1_px, l1_py, l2_px, l2_py = lepton_mom
                l1_pt = math.sqrt(l1_px**2 + l1_py**2)
                l2_pt = math.sqrt(l2_px**2 + l2_py**2)
                energy_scale = f'{(l1_pt + l2_pt) / 2.:.9E}'

                header = re.search(event_header, event.firstChild.data, re.MULTILINE)
                header = list(header.groups())
                header[1] = energy_scale

                event.firstChild.data = '\n'.join([''.join(header)] + new_particles) + '\n '

            with open(self.outFileName, 'w') as output:
                output.write(DOMTree.toxml().replace('<?xml version="1.0" ?>', ' '))

            self.done = True

        return StatusCode.Success
