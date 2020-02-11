# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import keV, MeV, ns

"""
Pythong configuration file for additional mesons.

The 'mesons' dictionary holds variables related to particles
and anti-particles are automatically created.

Eventually everything is passed into a
std::map<std::string, std::vector<double>> m_mesonsConfig C++
object where G4ParticleDefinition pointers get created.

"""
mesons = {}

## Additional D mesons
# source:
# http://pdg.lbl.gov/2019/tables/rpp2019-tab-mesons-charm.pdf
#______________________________________________________________
mesons.update({
    'D_star(2007)0' : {
        'mass' : 2006.85 * MeV,
        'width' : 2.1 * MeV,
        'charge' : 0,
        'spin' : 1,
        'parity' : -1,
        'PDG' : 423,
        'lifetime' : 3.13e-13 * ns,
    },
    'D_star(2010)+' : {
        'mass' : 2010.26 * MeV,
        'width' : 83.4 * keV,
        'charge' : 1,
        'spin' : 1,
        'parity' : -1,
        'PDG' : 413,
        'lifetime' : 7.892e-12 * ns,
    },
    'D0_star(2300)0' : {
        'mass' : 2300 * MeV,
        'width' : 274 * MeV,
        'charge' : 0,
        'spin' : 0,
        'parity' : +1,
        'PDG' : 10421,
        'lifetime' : 2.402e-15 * ns,
    },
    'D1(2420)0' : {
        'mass' : 2420.8 * MeV,
        'width' : 31.7 * MeV,
        'charge' : 0,
        'spin' : 1,
        'parity' : +1,
        'PDG' : 10423,
        'lifetime' : 2.076e-14 * ns,
    },
    'D2_star(2460)0' : {
        'mass' : 2460.7 * MeV,
        'width' : 47.5 * MeV,
        'charge' : 0,
        'spin' : 2,
        'parity' : +1,
        'PDG' : 425,
        'lifetime' : 1.386e-14 * ns,
    },
    'D2_star(2460)+' : {
        'mass' : 2465.4 * MeV,
        'width' : 46.7 * MeV,
        'charge' : 1,
        'spin' : 2,
        'parity' : +1,
        'PDG' : 415,
        'lifetime' : 1.409e-14 * ns,
    },
})

## Additional B mesons
# source:
# http://pdg.lbl.gov/2019/tables/rpp2019-tab-mesons-bottom.pdf
#______________________________________________________________
mesons.update({
    'B_star0' : {
        'mass' : 5324.70 * MeV,
        'width' : 45 * MeV,
        'charge' : 0,
        'spin' : 1,
        'parity' : -1,
        'PDG' : 513,
        'lifetime' : 1.463e-14 * ns,
    },
    'B1(5721)+' : {
        'mass' : 5725.9 * MeV,
        'width' : 31 * MeV,
        'charge' : 1,
        'spin' : 1,
        'parity' : +1,
        'PDG' : 10523,
        'lifetime' : 2.123e-14 * ns,
    },
    'B1(5721)0' : {
        'mass' : 5726.1 * MeV,
        'width' : 27.5 * MeV,
        'charge' : 0,
        'spin' : 1,
        'parity' : +1,
        'PDG' : 10513,
        'lifetime' : 2.393e-14 * ns,
    },
    'B2_star(5747)+' : {
        'mass' : 5737.2 * MeV,
        'width' : 20 * MeV,
        'charge' : 1,
        'spin' : 2,
        'parity' : +1,
        'PDG' : 525,
        'lifetime' : 3.291e-14 * ns,
    },
    'B2_star(5747)0' : {
        'mass' : 5739.5 * MeV,
        'width' : 24.2 * MeV,
        'charge' : 0,
        'spin' : 2,
        'parity' : +1,
        'PDG' : 515,
        'lifetime' : 2.72e-14 * ns,
    },
})


def createMesonsList():
    """
    Creates a compact object that holds configuration
    for the MesonsPhysicsTool.

    Gaudi cannot parse* nested std::map objects, so the 'mesons'
    dictionary is converted into a

      std::map< std::string, std::vector< double > >

    object and ordering of values therefore important---

    Rows are:
        name
        mass [MeV]
        width [MeV]
        charge
        2*spin
        parity
        2*isospin3
        PDG
        lifetime [ns]

    *: see Gaudi/GaudiKernel/GaudiKernel/Parsers.h
    """

    mesonsList = {}
    for meson in mesons:
        mesonProperties = mesons[meson]

        # sanity check
        verifyProperties(meson)

        # meson
        mesonsList[meson] = (
            mesonProperties['mass'],
            mesonProperties['width'],
            mesonProperties['charge'],
            mesonProperties['spin'],
            mesonProperties['parity'],
           -1/2.,
            mesonProperties['PDG'],
            mesonProperties['lifetime'],
        )
        # anti-meson
        antiName = "anti_" + meson
        if meson[-1] == '+':
            antiName = meson.replace('+', '-')
        mesonsList[antiName] = (
            mesonProperties['mass'],
            mesonProperties['width'],
           -mesonProperties['charge'],
            mesonProperties['spin'],
            mesonProperties['parity'],
           +1/2.,
           -mesonProperties['PDG'],
            mesonProperties['lifetime'],
        )
    return mesonsList

def verifyProperties(meson):
    mesonProperties = mesons[meson]
    if (mesonProperties['lifetime'] <= 0):
        raise ValueError('Lifetime of %s is not positive' % meson)
    if (mesonProperties['mass'] <= 0):
        raise ValueError('Mass of %s is not positive' % meson)
    if (mesonProperties['width'] <= 0):
        raise ValueError('Width of %s is not positive' % meson)
    if (mesonProperties['spin'] < 0):
        raise ValueError('Spin of %s is negative' % meson)
