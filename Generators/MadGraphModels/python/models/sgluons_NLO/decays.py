# This file was automatically created by FeynRules 2.1.78
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Fri 17 Oct 2014 08:59:37


from object_library import all_decays, Decay
import particles as P


Decay_b = Decay(name = 'Decay_b',
                particle = P.b,
                partial_widths = {(P.sig8,P.d):'((-m8**2 + MB**2)*(-4*g8Ld1x3**2*m8**2 - 4*g8Ld3x1**2*m8**2 - 8*g8Ld3x1*g8Rd1x3*m8**2 - 4*g8Rd1x3**2*m8**2 - 8*g8Ld1x3*g8Rd3x1*m8**2 - 4*g8Rd3x1**2*m8**2 + 4*g8Ld1x3**2*MB**2 + 4*g8Ld3x1**2*MB**2 + 8*g8Ld3x1*g8Rd1x3*MB**2 + 4*g8Rd1x3**2*MB**2 + 8*g8Ld1x3*g8Rd3x1*MB**2 + 4*g8Rd3x1**2*MB**2))/(96.*cmath.pi*abs(MB)**3)',
                                  (P.sig8,P.s):'((-m8**2 + MB**2)*(-4*g8Ld2x3**2*m8**2 - 4*g8Ld3x2**2*m8**2 - 8*g8Ld3x2*g8Rd2x3*m8**2 - 4*g8Rd2x3**2*m8**2 - 8*g8Ld2x3*g8Rd3x2*m8**2 - 4*g8Rd3x2**2*m8**2 + 4*g8Ld2x3**2*MB**2 + 4*g8Ld3x2**2*MB**2 + 8*g8Ld3x2*g8Rd2x3*MB**2 + 4*g8Rd2x3**2*MB**2 + 8*g8Ld2x3*g8Rd3x2*MB**2 + 4*g8Rd3x2**2*MB**2))/(96.*cmath.pi*abs(MB)**3)',
                                  (P.W__minus__,P.t):'(((3*ee**2*MB**2)/(2.*sw**2) + (3*ee**2*MT**2)/(2.*sw**2) + (3*ee**2*MB**4)/(2.*MW**2*sw**2) - (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) + (3*ee**2*MT**4)/(2.*MW**2*sw**2) - (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MB)**3)'})

Decay_H = Decay(name = 'Decay_H',
                particle = P.H,
                partial_widths = {(P.b,P.b__tilde__):'((-12*MB**2*yb**2 + 3*MH**2*yb**2)*cmath.sqrt(-4*MB**2*MH**2 + MH**4))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.t,P.t__tilde__):'((3*MH**2*yt**2 - 12*MT**2*yt**2)*cmath.sqrt(MH**4 - 4*MH**2*MT**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((MH**2*ytau**2 - 4*MTA**2*ytau**2)*cmath.sqrt(MH**4 - 4*MH**2*MTA**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.W__minus__,P.W__plus__):'(((3*ee**4*vev**2)/(4.*sw**4) + (ee**4*MH**4*vev**2)/(16.*MW**4*sw**4) - (ee**4*MH**2*vev**2)/(4.*MW**2*sw**4))*cmath.sqrt(MH**4 - 4*MH**2*MW**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.Z,P.Z):'(((9*ee**4*vev**2)/2. + (3*ee**4*MH**4*vev**2)/(8.*MZ**4) - (3*ee**4*MH**2*vev**2)/(2.*MZ**2) + (3*cw**4*ee**4*vev**2)/(4.*sw**4) + (cw**4*ee**4*MH**4*vev**2)/(16.*MZ**4*sw**4) - (cw**4*ee**4*MH**2*vev**2)/(4.*MZ**2*sw**4) + (3*cw**2*ee**4*vev**2)/sw**2 + (cw**2*ee**4*MH**4*vev**2)/(4.*MZ**4*sw**2) - (cw**2*ee**4*MH**2*vev**2)/(MZ**2*sw**2) + (3*ee**4*sw**2*vev**2)/cw**2 + (ee**4*MH**4*sw**2*vev**2)/(4.*cw**2*MZ**4) - (ee**4*MH**2*sw**2*vev**2)/(cw**2*MZ**2) + (3*ee**4*sw**4*vev**2)/(4.*cw**4) + (ee**4*MH**4*sw**4*vev**2)/(16.*cw**4*MZ**4) - (ee**4*MH**2*sw**4*vev**2)/(4.*cw**4*MZ**2))*cmath.sqrt(MH**4 - 4*MH**2*MZ**2))/(32.*cmath.pi*abs(MH)**3)'})

Decay_sig8 = Decay(name = 'Decay_sig8',
                   particle = P.sig8,
                   partial_widths = {(P.b,P.b__tilde__):'((8*g8Ld3x3**2*m8**2 + 16*g8Ld3x3*g8Rd3x3*m8**2 + 8*g8Rd3x3**2*m8**2 - 32*g8Ld3x3**2*MB**2 - 64*g8Ld3x3*g8Rd3x3*MB**2 - 32*g8Rd3x3**2*MB**2)*cmath.sqrt(m8**4 - 4*m8**2*MB**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.b,P.d__tilde__):'((m8**2 - MB**2)*(4*g8Ld1x3**2*m8**2 + 4*g8Ld3x1**2*m8**2 + 8*g8Ld3x1*g8Rd1x3*m8**2 + 4*g8Rd1x3**2*m8**2 + 8*g8Ld1x3*g8Rd3x1*m8**2 + 4*g8Rd3x1**2*m8**2 - 4*g8Ld1x3**2*MB**2 - 4*g8Ld3x1**2*MB**2 - 8*g8Ld3x1*g8Rd1x3*MB**2 - 4*g8Rd1x3**2*MB**2 - 8*g8Ld1x3*g8Rd3x1*MB**2 - 4*g8Rd3x1**2*MB**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.b,P.s__tilde__):'((m8**2 - MB**2)*(4*g8Ld2x3**2*m8**2 + 4*g8Ld3x2**2*m8**2 + 8*g8Ld3x2*g8Rd2x3*m8**2 + 4*g8Rd2x3**2*m8**2 + 8*g8Ld2x3*g8Rd3x2*m8**2 + 4*g8Rd3x2**2*m8**2 - 4*g8Ld2x3**2*MB**2 - 4*g8Ld3x2**2*MB**2 - 8*g8Ld3x2*g8Rd2x3*MB**2 - 4*g8Rd2x3**2*MB**2 - 8*g8Ld2x3*g8Rd3x2*MB**2 - 4*g8Rd3x2**2*MB**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.c,P.c__tilde__):'(m8**2*(8*g8Lu2x2**2*m8**2 + 16*g8Lu2x2*g8Ru2x2*m8**2 + 8*g8Ru2x2**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.c,P.t__tilde__):'((m8**2 - MT**2)*(4*g8Lu2x3**2*m8**2 + 4*g8Lu3x2**2*m8**2 + 8*g8Lu3x2*g8Ru2x3*m8**2 + 4*g8Ru2x3**2*m8**2 + 8*g8Lu2x3*g8Ru3x2*m8**2 + 4*g8Ru3x2**2*m8**2 - 4*g8Lu2x3**2*MT**2 - 4*g8Lu3x2**2*MT**2 - 8*g8Lu3x2*g8Ru2x3*MT**2 - 4*g8Ru2x3**2*MT**2 - 8*g8Lu2x3*g8Ru3x2*MT**2 - 4*g8Ru3x2**2*MT**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.c,P.u__tilde__):'(m8**2*(4*g8Lu1x2**2*m8**2 + 4*g8Lu2x1**2*m8**2 + 8*g8Lu2x1*g8Ru1x2*m8**2 + 4*g8Ru1x2**2*m8**2 + 8*g8Lu1x2*g8Ru2x1*m8**2 + 4*g8Ru2x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.d,P.b__tilde__):'((m8**2 - MB**2)*(4*g8Ld1x3**2*m8**2 + 4*g8Ld3x1**2*m8**2 + 8*g8Ld3x1*g8Rd1x3*m8**2 + 4*g8Rd1x3**2*m8**2 + 8*g8Ld1x3*g8Rd3x1*m8**2 + 4*g8Rd3x1**2*m8**2 - 4*g8Ld1x3**2*MB**2 - 4*g8Ld3x1**2*MB**2 - 8*g8Ld3x1*g8Rd1x3*MB**2 - 4*g8Rd1x3**2*MB**2 - 8*g8Ld1x3*g8Rd3x1*MB**2 - 4*g8Rd3x1**2*MB**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.d,P.d__tilde__):'(m8**2*(8*g8Ld1x1**2*m8**2 + 16*g8Ld1x1*g8Rd1x1*m8**2 + 8*g8Rd1x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.d,P.s__tilde__):'(m8**2*(4*g8Ld1x2**2*m8**2 + 4*g8Ld2x1**2*m8**2 + 8*g8Ld2x1*g8Rd1x2*m8**2 + 4*g8Rd1x2**2*m8**2 + 8*g8Ld1x2*g8Rd2x1*m8**2 + 4*g8Rd2x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.g,P.g):'(g8g**2*m8**6*(40./3.))/(32.*cmath.pi*abs(m8)**3)',
                                     (P.s,P.b__tilde__):'((m8**2 - MB**2)*(4*g8Ld2x3**2*m8**2 + 4*g8Ld3x2**2*m8**2 + 8*g8Ld3x2*g8Rd2x3*m8**2 + 4*g8Rd2x3**2*m8**2 + 8*g8Ld2x3*g8Rd3x2*m8**2 + 4*g8Rd3x2**2*m8**2 - 4*g8Ld2x3**2*MB**2 - 4*g8Ld3x2**2*MB**2 - 8*g8Ld3x2*g8Rd2x3*MB**2 - 4*g8Rd2x3**2*MB**2 - 8*g8Ld2x3*g8Rd3x2*MB**2 - 4*g8Rd3x2**2*MB**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.s,P.d__tilde__):'(m8**2*(4*g8Ld1x2**2*m8**2 + 4*g8Ld2x1**2*m8**2 + 8*g8Ld2x1*g8Rd1x2*m8**2 + 4*g8Rd1x2**2*m8**2 + 8*g8Ld1x2*g8Rd2x1*m8**2 + 4*g8Rd2x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.s,P.s__tilde__):'(m8**2*(8*g8Ld2x2**2*m8**2 + 16*g8Ld2x2*g8Rd2x2*m8**2 + 8*g8Rd2x2**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.t,P.c__tilde__):'((m8**2 - MT**2)*(4*g8Lu2x3**2*m8**2 + 4*g8Lu3x2**2*m8**2 + 8*g8Lu3x2*g8Ru2x3*m8**2 + 4*g8Ru2x3**2*m8**2 + 8*g8Lu2x3*g8Ru3x2*m8**2 + 4*g8Ru3x2**2*m8**2 - 4*g8Lu2x3**2*MT**2 - 4*g8Lu3x2**2*MT**2 - 8*g8Lu3x2*g8Ru2x3*MT**2 - 4*g8Ru2x3**2*MT**2 - 8*g8Lu2x3*g8Ru3x2*MT**2 - 4*g8Ru3x2**2*MT**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.t,P.t__tilde__):'((8*g8Lu3x3**2*m8**2 + 16*g8Lu3x3*g8Ru3x3*m8**2 + 8*g8Ru3x3**2*m8**2 - 32*g8Lu3x3**2*MT**2 - 64*g8Lu3x3*g8Ru3x3*MT**2 - 32*g8Ru3x3**2*MT**2)*cmath.sqrt(m8**4 - 4*m8**2*MT**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.t,P.u__tilde__):'((m8**2 - MT**2)*(4*g8Lu1x3**2*m8**2 + 4*g8Lu3x1**2*m8**2 + 8*g8Lu3x1*g8Ru1x3*m8**2 + 4*g8Ru1x3**2*m8**2 + 8*g8Lu1x3*g8Ru3x1*m8**2 + 4*g8Ru3x1**2*m8**2 - 4*g8Lu1x3**2*MT**2 - 4*g8Lu3x1**2*MT**2 - 8*g8Lu3x1*g8Ru1x3*MT**2 - 4*g8Ru1x3**2*MT**2 - 8*g8Lu1x3*g8Ru3x1*MT**2 - 4*g8Ru3x1**2*MT**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.u,P.c__tilde__):'(m8**2*(4*g8Lu1x2**2*m8**2 + 4*g8Lu2x1**2*m8**2 + 8*g8Lu2x1*g8Ru1x2*m8**2 + 4*g8Ru1x2**2*m8**2 + 8*g8Lu1x2*g8Ru2x1*m8**2 + 4*g8Ru2x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.u,P.t__tilde__):'((m8**2 - MT**2)*(4*g8Lu1x3**2*m8**2 + 4*g8Lu3x1**2*m8**2 + 8*g8Lu3x1*g8Ru1x3*m8**2 + 4*g8Ru1x3**2*m8**2 + 8*g8Lu1x3*g8Ru3x1*m8**2 + 4*g8Ru3x1**2*m8**2 - 4*g8Lu1x3**2*MT**2 - 4*g8Lu3x1**2*MT**2 - 8*g8Lu3x1*g8Ru1x3*MT**2 - 4*g8Ru1x3**2*MT**2 - 8*g8Lu1x3*g8Ru3x1*MT**2 - 4*g8Ru3x1**2*MT**2))/(128.*cmath.pi*abs(m8)**3)',
                                     (P.u,P.u__tilde__):'(m8**2*(8*g8Lu1x1**2*m8**2 + 16*g8Lu1x1*g8Ru1x1*m8**2 + 8*g8Ru1x1**2*m8**2))/(128.*cmath.pi*abs(m8)**3)'})

Decay_t = Decay(name = 'Decay_t',
                particle = P.t,
                partial_widths = {(P.sig8,P.c):'((-m8**2 + MT**2)*(-4*g8Lu2x3**2*m8**2 - 4*g8Lu3x2**2*m8**2 - 8*g8Lu3x2*g8Ru2x3*m8**2 - 4*g8Ru2x3**2*m8**2 - 8*g8Lu2x3*g8Ru3x2*m8**2 - 4*g8Ru3x2**2*m8**2 + 4*g8Lu2x3**2*MT**2 + 4*g8Lu3x2**2*MT**2 + 8*g8Lu3x2*g8Ru2x3*MT**2 + 4*g8Ru2x3**2*MT**2 + 8*g8Lu2x3*g8Ru3x2*MT**2 + 4*g8Ru3x2**2*MT**2))/(96.*cmath.pi*abs(MT)**3)',
                                  (P.sig8,P.u):'((-m8**2 + MT**2)*(-4*g8Lu1x3**2*m8**2 - 4*g8Lu3x1**2*m8**2 - 8*g8Lu3x1*g8Ru1x3*m8**2 - 4*g8Ru1x3**2*m8**2 - 8*g8Lu1x3*g8Ru3x1*m8**2 - 4*g8Ru3x1**2*m8**2 + 4*g8Lu1x3**2*MT**2 + 4*g8Lu3x1**2*MT**2 + 8*g8Lu3x1*g8Ru1x3*MT**2 + 4*g8Ru1x3**2*MT**2 + 8*g8Lu1x3*g8Ru3x1*MT**2 + 4*g8Ru3x1**2*MT**2))/(96.*cmath.pi*abs(MT)**3)',
                                  (P.W__plus__,P.b):'(((3*ee**2*MB**2)/(2.*sw**2) + (3*ee**2*MT**2)/(2.*sw**2) + (3*ee**2*MB**4)/(2.*MW**2*sw**2) - (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) + (3*ee**2*MT**4)/(2.*MW**2*sw**2) - (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MT)**3)'})

Decay_ta__minus__ = Decay(name = 'Decay_ta__minus__',
                          particle = P.ta__minus__,
                          partial_widths = {(P.W__minus__,P.vt):'((MTA**2 - MW**2)*((ee**2*MTA**2)/(2.*sw**2) + (ee**2*MTA**4)/(2.*MW**2*sw**2) - (ee**2*MW**2)/sw**2))/(32.*cmath.pi*abs(MTA)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.c,P.s__tilde__):'(ee**2*MW**4)/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.t,P.b__tilde__):'(((-3*ee**2*MB**2)/(2.*sw**2) - (3*ee**2*MT**2)/(2.*sw**2) - (3*ee**2*MB**4)/(2.*MW**2*sw**2) + (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) - (3*ee**2*MT**4)/(2.*MW**2*sw**2) + (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.u,P.d__tilde__):'(ee**2*MW**4)/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.ve,P.e__plus__):'(ee**2*MW**4)/(48.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.vm,P.mu__plus__):'(ee**2*MW**4)/(48.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.vt,P.ta__plus__):'((-MTA**2 + MW**2)*(-(ee**2*MTA**2)/(2.*sw**2) - (ee**2*MTA**4)/(2.*MW**2*sw**2) + (ee**2*MW**2)/sw**2))/(48.*cmath.pi*abs(MW)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.b,P.b__tilde__):'((-7*ee**2*MB**2 + ee**2*MZ**2 - (3*cw**2*ee**2*MB**2)/(2.*sw**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) - (17*ee**2*MB**2*sw**2)/(6.*cw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2))*cmath.sqrt(-4*MB**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.c,P.c__tilde__):'(MZ**2*(-(ee**2*MZ**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.d,P.d__tilde__):'(MZ**2*(ee**2*MZ**2 + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.e__minus__,P.e__plus__):'(MZ**2*(-(ee**2*MZ**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.mu__minus__,P.mu__plus__):'(MZ**2*(-(ee**2*MZ**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.s,P.s__tilde__):'(MZ**2*(ee**2*MZ**2 + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.t,P.t__tilde__):'((-11*ee**2*MT**2 - ee**2*MZ**2 - (3*cw**2*ee**2*MT**2)/(2.*sw**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (7*ee**2*MT**2*sw**2)/(6.*cw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2))*cmath.sqrt(-4*MT**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((-5*ee**2*MTA**2 - ee**2*MZ**2 - (cw**2*ee**2*MTA**2)/(2.*sw**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (7*ee**2*MTA**2*sw**2)/(2.*cw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2))*cmath.sqrt(-4*MTA**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.u,P.u__tilde__):'(MZ**2*(-(ee**2*MZ**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ve,P.ve__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vm,P.vm__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vt,P.vt__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.W__minus__,P.W__plus__):'(((-12*cw**2*ee**2*MW**2)/sw**2 - (17*cw**2*ee**2*MZ**2)/sw**2 + (4*cw**2*ee**2*MZ**4)/(MW**2*sw**2) + (cw**2*ee**2*MZ**6)/(4.*MW**4*sw**2))*cmath.sqrt(-4*MW**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)'})

