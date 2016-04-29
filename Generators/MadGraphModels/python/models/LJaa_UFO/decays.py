# This file was automatically created by FeynRules 2.3.4
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (November 20, 2012)
# Date: Sun 24 Jan 2016 21:45:21


from object_library import all_decays, Decay
import particles as P


Decay_b = Decay(name = 'Decay_b',
                particle = P.b,
                partial_widths = {(P.W__minus__,P.t):'(((3*gL**2*mB**2)/2. + (3*gL**2*mT**2)/2. + (3*gL**2*mB**4)/(2.*mW**2) - (3*gL**2*mB**2*mT**2)/mW**2 + (3*gL**2*mT**4)/(2.*mW**2) - 3*gL**2*mW**2)*cmath.sqrt(mB**4 - 2*mB**2*mT**2 + mT**4 - 2*mB**2*mW**2 - 2*mT**2*mW**2 + mW**4))/(96.*cmath.pi*abs(mB)**3)'})

Decay_e__minus__ = Decay(name = 'Decay_e__minus__',
                         particle = P.e__minus__,
                         partial_widths = {(P.W__minus__,P.ve):'((mE**2 - mW**2)*((gL**2*mE**2)/2. + (gL**2*mE**4)/(2.*mW**2) - gL**2*mW**2))/(32.*cmath.pi*abs(mE)**3)'})

Decay_h = Decay(name = 'Decay_h',
                particle = P.h,
                partial_widths = {(P.b,P.b__tilde__):'(((-24*cd**2*mB**4)/v**2 + (6*cd**2*mB**2*mH**2)/v**2)*cmath.sqrt(-4*mB**2*mH**2 + mH**4))/(16.*cmath.pi*abs(mH)**3)',
                                  (P.g,P.g):'(cgg**2*gs**4*mH**6)/(8.*cmath.pi*v**2*abs(mH)**3)',
                                  (P.nd2,P.nd2):'((8*GHND12**2*mH**2 - 32*GHND12**2*ND2MASS**2)*cmath.sqrt(mH**4 - 4*mH**2*ND2MASS**2))/(32.*cmath.pi*abs(mH)**3)',
                                  (P.t,P.t__tilde__):'(((6*cu**2*mH**2*mT**2)/v**2 - (24*cu**2*mT**4)/v**2)*cmath.sqrt(mH**4 - 4*mH**2*mT**2))/(16.*cmath.pi*abs(mH)**3)',
                                  (P.ta__minus__,P.ta__plus__):'(((2*cl**2*mH**2*mTau**2)/v**2 - (8*cl**2*mTau**4)/v**2)*cmath.sqrt(mH**4 - 4*mH**2*mTau**2))/(16.*cmath.pi*abs(mH)**3)',
                                  (P.W__minus__,P.W__plus__):'(((3*cV**2*gL**4*v**2)/4. + (cV**2*gL**4*mH**4*v**2)/(16.*mW**4) - (cV**2*gL**4*mH**2*v**2)/(4.*mW**2))*cmath.sqrt(mH**4 - 4*mH**2*mW**2))/(16.*cmath.pi*abs(mH)**3)',
                                  (P.Z,P.Z):'(((3*cV**2*gL**4*v**2)/4. + (3*cV**2*gL**2*gY**2*v**2)/2. + (3*cV**2*gY**4*v**2)/4. + (cV**2*gL**4*mH**4*v**2)/(16.*mZ**4) + (cV**2*gL**2*gY**2*mH**4*v**2)/(8.*mZ**4) + (cV**2*gY**4*mH**4*v**2)/(16.*mZ**4) - (cV**2*gL**4*mH**2*v**2)/(4.*mZ**2) - (cV**2*gL**2*gY**2*mH**2*v**2)/(2.*mZ**2) - (cV**2*gY**4*mH**2*v**2)/(4.*mZ**2))*cmath.sqrt(mH**4 - 4*mH**2*mZ**2))/(32.*cmath.pi*abs(mH)**3)'})

Decay_hd1 = Decay(name = 'Decay_hd1',
                  particle = P.hd1,
                  partial_widths = {(P.nd1,P.nd2):'((8*GND12HD1**2*HD1MASS**2 - 8*GND12HD1**2*ND1MASS**2 - 16*GND12HD1**2*ND1MASS*ND2MASS - 8*GND12HD1**2*ND2MASS**2)*cmath.sqrt(HD1MASS**4 - 2*HD1MASS**2*ND1MASS**2 + ND1MASS**4 - 2*HD1MASS**2*ND2MASS**2 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4))/(16.*cmath.pi*abs(HD1MASS)**3)',
                                    (P.zd,P.zd):'((12*GHD1ZD**2 + (GHD1ZD**2*HD1MASS**4)/ZDMASS**4 - (4*GHD1ZD**2*HD1MASS**2)/ZDMASS**2)*cmath.sqrt(HD1MASS**4 - 4*HD1MASS**2*ZDMASS**2))/(32.*cmath.pi*abs(HD1MASS)**3)'})

Decay_mu__minus__ = Decay(name = 'Decay_mu__minus__',
                          particle = P.mu__minus__,
                          partial_widths = {(P.W__minus__,P.vm):'((mM**2 - mW**2)*((gL**2*mM**2)/2. + (gL**2*mM**4)/(2.*mW**2) - gL**2*mW**2))/(32.*cmath.pi*abs(mM)**3)'})

Decay_nd1 = Decay(name = 'Decay_nd1',
                  particle = P.nd1,
                  partial_widths = {(P.hd1,P.nd2):'((-8*GND12HD1**2*HD1MASS**2 + 8*GND12HD1**2*ND1MASS**2 + 16*GND12HD1**2*ND1MASS*ND2MASS + 8*GND12HD1**2*ND2MASS**2)*cmath.sqrt(HD1MASS**4 - 2*HD1MASS**2*ND1MASS**2 + ND1MASS**4 - 2*HD1MASS**2*ND2MASS**2 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4))/(32.*cmath.pi*abs(ND1MASS)**3)',
                                    (P.zd,P.nd2):'((8*GND12ZD**2*ND1MASS**2 - 48*GND12ZD**2*ND1MASS*ND2MASS + 8*GND12ZD**2*ND2MASS**2 + (8*GND12ZD**2*ND1MASS**4)/ZDMASS**2 - (16*GND12ZD**2*ND1MASS**2*ND2MASS**2)/ZDMASS**2 + (8*GND12ZD**2*ND2MASS**4)/ZDMASS**2 - 16*GND12ZD**2*ZDMASS**2)*cmath.sqrt(ND1MASS**4 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4 - 2*ND1MASS**2*ZDMASS**2 - 2*ND2MASS**2*ZDMASS**2 + ZDMASS**4))/(32.*cmath.pi*abs(ND1MASS)**3)'})

Decay_nd2 = Decay(name = 'Decay_nd2',
                  particle = P.nd2,
                  partial_widths = {(P.hd1,P.nd1):'((-8*GND12HD1**2*HD1MASS**2 + 8*GND12HD1**2*ND1MASS**2 + 16*GND12HD1**2*ND1MASS*ND2MASS + 8*GND12HD1**2*ND2MASS**2)*cmath.sqrt(HD1MASS**4 - 2*HD1MASS**2*ND1MASS**2 + ND1MASS**4 - 2*HD1MASS**2*ND2MASS**2 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4))/(32.*cmath.pi*abs(ND2MASS)**3)',
                                    (P.zd,P.nd1):'((8*GND12ZD**2*ND1MASS**2 - 48*GND12ZD**2*ND1MASS*ND2MASS + 8*GND12ZD**2*ND2MASS**2 + (8*GND12ZD**2*ND1MASS**4)/ZDMASS**2 - (16*GND12ZD**2*ND1MASS**2*ND2MASS**2)/ZDMASS**2 + (8*GND12ZD**2*ND2MASS**4)/ZDMASS**2 - 16*GND12ZD**2*ZDMASS**2)*cmath.sqrt(ND1MASS**4 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4 - 2*ND1MASS**2*ZDMASS**2 - 2*ND2MASS**2*ZDMASS**2 + ZDMASS**4))/(32.*cmath.pi*abs(ND2MASS)**3)'})

Decay_t = Decay(name = 'Decay_t',
                particle = P.t,
                partial_widths = {(P.W__plus__,P.b):'(((3*gL**2*mB**2)/2. + (3*gL**2*mT**2)/2. + (3*gL**2*mB**4)/(2.*mW**2) - (3*gL**2*mB**2*mT**2)/mW**2 + (3*gL**2*mT**4)/(2.*mW**2) - 3*gL**2*mW**2)*cmath.sqrt(mB**4 - 2*mB**2*mT**2 + mT**4 - 2*mB**2*mW**2 - 2*mT**2*mW**2 + mW**4))/(96.*cmath.pi*abs(mT)**3)'})

Decay_ta__minus__ = Decay(name = 'Decay_ta__minus__',
                          particle = P.ta__minus__,
                          partial_widths = {(P.W__minus__,P.vt):'((mTau**2 - mW**2)*((gL**2*mTau**2)/2. + (gL**2*mTau**4)/(2.*mW**2) - gL**2*mW**2))/(32.*cmath.pi*abs(mTau)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.c,P.s__tilde__):'(gL**2*mW**4)/(16.*cmath.pi*abs(mW)**3)',
                                          (P.t,P.b__tilde__):'(((-3*gL**2*mB**2)/2. - (3*gL**2*mT**2)/2. - (3*gL**2*mB**4)/(2.*mW**2) + (3*gL**2*mB**2*mT**2)/mW**2 - (3*gL**2*mT**4)/(2.*mW**2) + 3*gL**2*mW**2)*cmath.sqrt(mB**4 - 2*mB**2*mT**2 + mT**4 - 2*mB**2*mW**2 - 2*mT**2*mW**2 + mW**4))/(48.*cmath.pi*abs(mW)**3)',
                                          (P.u,P.d__tilde__):'(gL**2*mW**4)/(16.*cmath.pi*abs(mW)**3)',
                                          (P.ve,P.e__plus__):'((-mE**2 + mW**2)*(-(gL**2*mE**2)/2. - (gL**2*mE**4)/(2.*mW**2) + gL**2*mW**2))/(48.*cmath.pi*abs(mW)**3)',
                                          (P.vm,P.mu__plus__):'((-mM**2 + mW**2)*(-(gL**2*mM**2)/2. - (gL**2*mM**4)/(2.*mW**2) + gL**2*mW**2))/(48.*cmath.pi*abs(mW)**3)',
                                          (P.vt,P.ta__plus__):'((-mTau**2 + mW**2)*(-(gL**2*mTau**2)/2. - (gL**2*mTau**4)/(2.*mW**2) + gL**2*mW**2))/(48.*cmath.pi*abs(mW)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.b,P.b__tilde__):'(((-3*gL**4*mB**2)/(2.*(gL**2 + gY**2)) - (7*gL**2*gY**2*mB**2)/(gL**2 + gY**2) - (17*gY**4*mB**2)/(6.*(gL**2 + gY**2)) + (3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) + (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(6.*(gL**2 + gY**2)))*cmath.sqrt(-4*mB**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.c,P.c__tilde__):'(mZ**2*((3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (17*gY**4*mZ**2)/(6.*(gL**2 + gY**2))))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.d,P.d__tilde__):'(mZ**2*((3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) + (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(6.*(gL**2 + gY**2))))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.e__minus__,P.e__plus__):'((-(gL**4*mE**2)/(2.*(gL**2 + gY**2)) - (5*gL**2*gY**2*mE**2)/(gL**2 + gY**2) + (7*gY**4*mE**2)/(2.*(gL**2 + gY**2)) + (gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(2.*(gL**2 + gY**2)))*cmath.sqrt(-4*mE**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.mu__minus__,P.mu__plus__):'((-(gL**4*mM**2)/(2.*(gL**2 + gY**2)) - (5*gL**2*gY**2*mM**2)/(gL**2 + gY**2) + (7*gY**4*mM**2)/(2.*(gL**2 + gY**2)) + (gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(2.*(gL**2 + gY**2)))*cmath.sqrt(-4*mM**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.s,P.s__tilde__):'(mZ**2*((3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) + (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(6.*(gL**2 + gY**2))))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.t,P.t__tilde__):'(((-3*gL**4*mT**2)/(2.*(gL**2 + gY**2)) - (11*gL**2*gY**2*mT**2)/(gL**2 + gY**2) + (7*gY**4*mT**2)/(6.*(gL**2 + gY**2)) + (3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (17*gY**4*mZ**2)/(6.*(gL**2 + gY**2)))*cmath.sqrt(-4*mT**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((-(gL**4*mTau**2)/(2.*(gL**2 + gY**2)) - (5*gL**2*gY**2*mTau**2)/(gL**2 + gY**2) + (7*gY**4*mTau**2)/(2.*(gL**2 + gY**2)) + (gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (5*gY**4*mZ**2)/(2.*(gL**2 + gY**2)))*cmath.sqrt(-4*mTau**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.u,P.u__tilde__):'(mZ**2*((3*gL**4*mZ**2)/(2.*(gL**2 + gY**2)) - (gL**2*gY**2*mZ**2)/(gL**2 + gY**2) + (17*gY**4*mZ**2)/(6.*(gL**2 + gY**2))))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.ve,P.ve__tilde__):'(mZ**2*((gL**2*mZ**2)/2. + (gY**2*mZ**2)/2.))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.vm,P.vm__tilde__):'(mZ**2*((gL**2*mZ**2)/2. + (gY**2*mZ**2)/2.))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.vt,P.vt__tilde__):'(mZ**2*((gL**2*mZ**2)/2. + (gY**2*mZ**2)/2.))/(48.*cmath.pi*abs(mZ)**3)',
                                  (P.W__minus__,P.W__plus__):'(((-12*gL**4*mW**2)/(gL**2 + gY**2) - (17*gL**4*mZ**2)/(gL**2 + gY**2) + (4*gL**4*mZ**4)/((gL**2 + gY**2)*mW**2) + (gL**4*mZ**6)/(4.*(gL**2 + gY**2)*mW**4))*cmath.sqrt(-4*mW**2*mZ**2 + mZ**4))/(48.*cmath.pi*abs(mZ)**3)'})

Decay_zd = Decay(name = 'Decay_zd',
                 particle = P.zd,
                 partial_widths = {(P.e__minus__,P.e__plus__):'((8*GZDE**2*mE**2 + 4*GZDE**2*ZDMASS**2)*cmath.sqrt(-4*mE**2*ZDMASS**2 + ZDMASS**4))/(48.*cmath.pi*abs(ZDMASS)**3)',
                                   (P.mu__minus__,P.mu__plus__):'((8*GZDMU**2*mM**2 + 4*GZDMU**2*ZDMASS**2)*cmath.sqrt(-4*mM**2*ZDMASS**2 + ZDMASS**4))/(48.*cmath.pi*abs(ZDMASS)**3)',
                                   (P.nd1,P.nd2):'((-8*GND12ZD**2*ND1MASS**2 + 48*GND12ZD**2*ND1MASS*ND2MASS - 8*GND12ZD**2*ND2MASS**2 - (8*GND12ZD**2*ND1MASS**4)/ZDMASS**2 + (16*GND12ZD**2*ND1MASS**2*ND2MASS**2)/ZDMASS**2 - (8*GND12ZD**2*ND2MASS**4)/ZDMASS**2 + 16*GND12ZD**2*ZDMASS**2)*cmath.sqrt(ND1MASS**4 - 2*ND1MASS**2*ND2MASS**2 + ND2MASS**4 - 2*ND1MASS**2*ZDMASS**2 - 2*ND2MASS**2*ZDMASS**2 + ZDMASS**4))/(48.*cmath.pi*abs(ZDMASS)**3)',
                                   (P.pi__tilde__,P.pi):'((-4*GZDPI**2*MPI**2 + GZDPI**2*ZDMASS**2)*cmath.sqrt(-4*MPI**2*ZDMASS**2 + ZDMASS**4))/(48.*cmath.pi*abs(ZDMASS)**3)'})

