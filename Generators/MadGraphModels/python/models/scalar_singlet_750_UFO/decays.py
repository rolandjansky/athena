# This file was automatically created by FeynRules 2.3.7
# Mathematica version: 10.2.0 for Linux x86 (64-bit) (July 28, 2015)
# Date: Thu 26 Nov 2015 09:52:28


from object_library import all_decays, Decay
import particles as P


Decay_b = Decay(name = 'Decay_b',
                particle = P.b,
                partial_widths = {(P.W__minus__,P.c):'((MB**2 - MW**2)*((3*CKM2x3*ee**2*MB**2*complexconjugate(CKM2x3))/(2.*sw**2) + (3*CKM2x3*ee**2*MB**4*complexconjugate(CKM2x3))/(2.*MW**2*sw**2) - (3*CKM2x3*ee**2*MW**2*complexconjugate(CKM2x3))/sw**2))/(96.*cmath.pi*abs(MB)**3)',
                                  (P.W__minus__,P.t):'(((3*CKM3x3*ee**2*MB**2*complexconjugate(CKM3x3))/(2.*sw**2) + (3*CKM3x3*ee**2*MT**2*complexconjugate(CKM3x3))/(2.*sw**2) + (3*CKM3x3*ee**2*MB**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) - (3*CKM3x3*ee**2*MB**2*MT**2*complexconjugate(CKM3x3))/(MW**2*sw**2) + (3*CKM3x3*ee**2*MT**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) - (3*CKM3x3*ee**2*MW**2*complexconjugate(CKM3x3))/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MB)**3)',
                                  (P.W__minus__,P.u):'((MB**2 - MW**2)*((3*CKM1x3*ee**2*MB**2*complexconjugate(CKM1x3))/(2.*sw**2) + (3*CKM1x3*ee**2*MB**4*complexconjugate(CKM1x3))/(2.*MW**2*sw**2) - (3*CKM1x3*ee**2*MW**2*complexconjugate(CKM1x3))/sw**2))/(96.*cmath.pi*abs(MB)**3)'})

Decay_H = Decay(name = 'Decay_H',
                particle = P.H,
                partial_widths = {(P.b,P.b__tilde__):'((-12*MB**2*yb**2 + 3*MH**2*yb**2)*cmath.sqrt(-4*MB**2*MH**2 + MH**4))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.t,P.t__tilde__):'((3*MH**2*yt**2 - 12*MT**2*yt**2)*cmath.sqrt(MH**4 - 4*MH**2*MT**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((MH**2*ytau**2 - 4*MTA**2*ytau**2)*cmath.sqrt(MH**4 - 4*MH**2*MTA**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.W__minus__,P.W__plus__):'(((3*ee**4*vev**2)/(4.*sw**4) + (ee**4*MH**4*vev**2)/(16.*MW**4*sw**4) - (ee**4*MH**2*vev**2)/(4.*MW**2*sw**4))*cmath.sqrt(MH**4 - 4*MH**2*MW**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.Z,P.Z):'(((9*ee**4*vev**2)/2. + (3*ee**4*MH**4*vev**2)/(8.*MZ**4) - (3*ee**4*MH**2*vev**2)/(2.*MZ**2) + (3*cw**4*ee**4*vev**2)/(4.*sw**4) + (cw**4*ee**4*MH**4*vev**2)/(16.*MZ**4*sw**4) - (cw**4*ee**4*MH**2*vev**2)/(4.*MZ**2*sw**4) + (3*cw**2*ee**4*vev**2)/sw**2 + (cw**2*ee**4*MH**4*vev**2)/(4.*MZ**4*sw**2) - (cw**2*ee**4*MH**2*vev**2)/(MZ**2*sw**2) + (3*ee**4*sw**2*vev**2)/cw**2 + (ee**4*MH**4*sw**2*vev**2)/(4.*cw**2*MZ**4) - (ee**4*MH**2*sw**2*vev**2)/(cw**2*MZ**2) + (3*ee**4*sw**4*vev**2)/(4.*cw**4) + (ee**4*MH**4*sw**4*vev**2)/(16.*cw**4*MZ**4) - (ee**4*MH**2*sw**4*vev**2)/(4.*cw**4*MZ**2))*cmath.sqrt(MH**4 - 4*MH**2*MZ**2))/(32.*cmath.pi*abs(MH)**3)'})

Decay_t = Decay(name = 'Decay_t',
                particle = P.t,
                partial_widths = {(P.W__plus__,P.b):'(((3*CKM3x3*ee**2*MB**2*complexconjugate(CKM3x3))/(2.*sw**2) + (3*CKM3x3*ee**2*MT**2*complexconjugate(CKM3x3))/(2.*sw**2) + (3*CKM3x3*ee**2*MB**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) - (3*CKM3x3*ee**2*MB**2*MT**2*complexconjugate(CKM3x3))/(MW**2*sw**2) + (3*CKM3x3*ee**2*MT**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) - (3*CKM3x3*ee**2*MW**2*complexconjugate(CKM3x3))/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MT)**3)',
                                  (P.W__plus__,P.d):'((MT**2 - MW**2)*((3*CKM3x1*ee**2*MT**2*complexconjugate(CKM3x1))/(2.*sw**2) + (3*CKM3x1*ee**2*MT**4*complexconjugate(CKM3x1))/(2.*MW**2*sw**2) - (3*CKM3x1*ee**2*MW**2*complexconjugate(CKM3x1))/sw**2))/(96.*cmath.pi*abs(MT)**3)',
                                  (P.W__plus__,P.s):'((MT**2 - MW**2)*((3*CKM3x2*ee**2*MT**2*complexconjugate(CKM3x2))/(2.*sw**2) + (3*CKM3x2*ee**2*MT**4*complexconjugate(CKM3x2))/(2.*MW**2*sw**2) - (3*CKM3x2*ee**2*MW**2*complexconjugate(CKM3x2))/sw**2))/(96.*cmath.pi*abs(MT)**3)'})

Decay_ta__minus__ = Decay(name = 'Decay_ta__minus__',
                          particle = P.ta__minus__,
                          partial_widths = {(P.W__minus__,P.vt):'((MTA**2 - MW**2)*((ee**2*MTA**2)/(2.*sw**2) + (ee**2*MTA**4)/(2.*MW**2*sw**2) - (ee**2*MW**2)/sw**2))/(32.*cmath.pi*abs(MTA)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.c,P.b__tilde__):'((-MB**2 + MW**2)*((-3*CKM2x3*ee**2*MB**2*complexconjugate(CKM2x3))/(2.*sw**2) - (3*CKM2x3*ee**2*MB**4*complexconjugate(CKM2x3))/(2.*MW**2*sw**2) + (3*CKM2x3*ee**2*MW**2*complexconjugate(CKM2x3))/sw**2))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.c,P.d__tilde__):'(CKM2x1*ee**2*MW**4*complexconjugate(CKM2x1))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.c,P.s__tilde__):'(CKM2x2*ee**2*MW**4*complexconjugate(CKM2x2))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.t,P.b__tilde__):'(((-3*CKM3x3*ee**2*MB**2*complexconjugate(CKM3x3))/(2.*sw**2) - (3*CKM3x3*ee**2*MT**2*complexconjugate(CKM3x3))/(2.*sw**2) - (3*CKM3x3*ee**2*MB**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) + (3*CKM3x3*ee**2*MB**2*MT**2*complexconjugate(CKM3x3))/(MW**2*sw**2) - (3*CKM3x3*ee**2*MT**4*complexconjugate(CKM3x3))/(2.*MW**2*sw**2) + (3*CKM3x3*ee**2*MW**2*complexconjugate(CKM3x3))/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.t,P.d__tilde__):'((-MT**2 + MW**2)*((-3*CKM3x1*ee**2*MT**2*complexconjugate(CKM3x1))/(2.*sw**2) - (3*CKM3x1*ee**2*MT**4*complexconjugate(CKM3x1))/(2.*MW**2*sw**2) + (3*CKM3x1*ee**2*MW**2*complexconjugate(CKM3x1))/sw**2))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.t,P.s__tilde__):'((-MT**2 + MW**2)*((-3*CKM3x2*ee**2*MT**2*complexconjugate(CKM3x2))/(2.*sw**2) - (3*CKM3x2*ee**2*MT**4*complexconjugate(CKM3x2))/(2.*MW**2*sw**2) + (3*CKM3x2*ee**2*MW**2*complexconjugate(CKM3x2))/sw**2))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.u,P.b__tilde__):'((-MB**2 + MW**2)*((-3*CKM1x3*ee**2*MB**2*complexconjugate(CKM1x3))/(2.*sw**2) - (3*CKM1x3*ee**2*MB**4*complexconjugate(CKM1x3))/(2.*MW**2*sw**2) + (3*CKM1x3*ee**2*MW**2*complexconjugate(CKM1x3))/sw**2))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.u,P.d__tilde__):'(CKM1x1*ee**2*MW**4*complexconjugate(CKM1x1))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.u,P.s__tilde__):'(CKM1x2*ee**2*MW**4*complexconjugate(CKM1x2))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.ve,P.e__plus__):'(ee**2*MW**4)/(48.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.vm,P.mu__plus__):'(ee**2*MW**4)/(48.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.vt,P.ta__plus__):'((-MTA**2 + MW**2)*(-(ee**2*MTA**2)/(2.*sw**2) - (ee**2*MTA**4)/(2.*MW**2*sw**2) + (ee**2*MW**2)/sw**2))/(48.*cmath.pi*abs(MW)**3)'})

Decay_Y0 = Decay(name = 'Decay_Y0',
                 particle = P.Y0,
                 partial_widths = {(P.a,P.a):'(MY0**2*((8*c1**2*ee**4*MY0**4)/Lambda**2 + (16*c1*c2*ee**4*MY0**4)/Lambda**2 + (8*c2**2*ee**4*MY0**4)/Lambda**2))/(32.*cmath.pi*abs(MY0)**3)',
                                   (P.a,P.Z):'((MY0**2 - MZ**2)*((-16*c1*c2*ee**4*MY0**4)/Lambda**2 + (32*c1*c2*ee**4*MY0**2*MZ**2)/Lambda**2 - (16*c1*c2*ee**4*MZ**4)/Lambda**2 + (8*c2**2*cw**2*ee**4*MY0**4)/(Lambda**2*sw**2) - (16*c2**2*cw**2*ee**4*MY0**2*MZ**2)/(Lambda**2*sw**2) + (8*c2**2*cw**2*ee**4*MZ**4)/(Lambda**2*sw**2) + (8*c1**2*ee**4*MY0**4*sw**2)/(cw**2*Lambda**2) - (16*c1**2*ee**4*MY0**2*MZ**2*sw**2)/(cw**2*Lambda**2) + (8*c1**2*ee**4*MZ**4*sw**2)/(cw**2*Lambda**2)))/(16.*cmath.pi*abs(MY0)**3)',
                                   (P.g,P.g):'(2*c3**2*G**4*MY0**6)/(cmath.pi*Lambda**2*abs(MY0)**3)',
                                   (P.t,P.t__tilde__):'(((-24*ct**2*MT**4)/Lambda**2 + (6*ct**2*MT**2*MY0**2)/Lambda**2)*cmath.sqrt(-4*MT**2*MY0**2 + MY0**4))/(16.*cmath.pi*abs(MY0)**3)',
                                   (P.W__minus__,P.W__plus__):'(((48*c2**2*ee**4*MW**4)/(Lambda**2*sw**4) - (32*c2**2*ee**4*MW**2*MY0**2)/(Lambda**2*sw**4) + (8*c2**2*ee**4*MY0**4)/(Lambda**2*sw**4))*cmath.sqrt(-4*MW**2*MY0**2 + MY0**4))/(16.*cmath.pi*abs(MY0)**3)',
                                   (P.Z,P.Z):'(((16*c1*c2*ee**4*MY0**4)/Lambda**2 - (64*c1*c2*ee**4*MY0**2*MZ**2)/Lambda**2 + (96*c1*c2*ee**4*MZ**4)/Lambda**2 + (8*c2**2*cw**4*ee**4*MY0**4)/(Lambda**2*sw**4) - (32*c2**2*cw**4*ee**4*MY0**2*MZ**2)/(Lambda**2*sw**4) + (48*c2**2*cw**4*ee**4*MZ**4)/(Lambda**2*sw**4) + (8*c1**2*ee**4*MY0**4*sw**4)/(cw**4*Lambda**2) - (32*c1**2*ee**4*MY0**2*MZ**2*sw**4)/(cw**4*Lambda**2) + (48*c1**2*ee**4*MZ**4*sw**4)/(cw**4*Lambda**2))*cmath.sqrt(MY0**4 - 4*MY0**2*MZ**2))/(32.*cmath.pi*abs(MY0)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.a,P.Y0):'((-MY0**2 + MZ**2)*((-16*c1*c2*ee**4*MY0**4)/Lambda**2 + (32*c1*c2*ee**4*MY0**2*MZ**2)/Lambda**2 - (16*c1*c2*ee**4*MZ**4)/Lambda**2 + (8*c2**2*cw**2*ee**4*MY0**4)/(Lambda**2*sw**2) - (16*c2**2*cw**2*ee**4*MY0**2*MZ**2)/(Lambda**2*sw**2) + (8*c2**2*cw**2*ee**4*MZ**4)/(Lambda**2*sw**2) + (8*c1**2*ee**4*MY0**4*sw**2)/(cw**2*Lambda**2) - (16*c1**2*ee**4*MY0**2*MZ**2*sw**2)/(cw**2*Lambda**2) + (8*c1**2*ee**4*MZ**4*sw**2)/(cw**2*Lambda**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.b,P.b__tilde__):'((-7*ee**2*MB**2 + ee**2*MZ**2 - (3*cw**2*ee**2*MB**2)/(2.*sw**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) - (17*ee**2*MB**2*sw**2)/(6.*cw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2))*cmath.sqrt(-4*MB**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
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

