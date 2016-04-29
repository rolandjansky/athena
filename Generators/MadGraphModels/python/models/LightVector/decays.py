# This file was automatically created by FeynRules 2.0.6
# Mathematica version: 8.0 for Linux x86 (64-bit) (October 10, 2011)
# Date: Thu 20 Feb 2014 17:14:11


from object_library import all_decays, Decay
import particles as P


Decay_b = Decay(name = 'Decay_b',
                particle = P.b,
                partial_widths = {(P.W__minus__,P.t):'(((3*ee**2*MB**2)/(2.*sw**2) + (3*ee**2*MT**2)/(2.*sw**2) + (3*ee**2*MB**4)/(2.*MW**2*sw**2) - (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) + (3*ee**2*MT**4)/(2.*MW**2*sw**2) - (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MB)**3)'})

Decay_n2 = Decay(name = 'Decay_n2',
                 particle = P.n2,
                 partial_widths = {(P.vx,P.n1):'((-4*gx**2*MVdark**2 + 2*gx**2*MX**2 + (2*gx**2*MX**4)/MVdark**2 - 12*gx**2*MX*MX2 + 2*gx**2*MX2**2 - (4*gx**2*MX**2*MX2**2)/MVdark**2 + (2*gx**2*MX2**4)/MVdark**2)*cmath.sqrt(MVdark**4 - 2*MVdark**2*MX**2 + MX**4 - 2*MVdark**2*MX2**2 - 2*MX**2*MX2**2 + MX2**4))/(32.*cmath.pi*abs(MX2)**3)'})

Decay_n1 = Decay(name = 'Decay_n1',
                 particle = P.n1,
                 partial_widths = {(P.vx,P.n2):'((-4*gx**2*MVdark**2 + 2*gx**2*MX**2 + (2*gx**2*MX**4)/MVdark**2 - 12*gx**2*MX*MX2 + 2*gx**2*MX2**2 - (4*gx**2*MX**2*MX2**2)/MVdark**2 + (2*gx**2*MX2**4)/MVdark**2)*cmath.sqrt(MVdark**4 - 2*MVdark**2*MX**2 + MX**4 - 2*MVdark**2*MX2**2 - 2*MX**2*MX2**2 + MX2**4))/(32.*cmath.pi*abs(MX)**3)'})

Decay_H = Decay(name = 'Decay_H',
                particle = P.H,
                partial_widths = {(P.b,P.b__tilde__):'((-12*MB**2*yb**2 + 3*MH**2*yb**2)*cmath.sqrt(-4*MB**2*MH**2 + MH**4))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.t,P.t__tilde__):'((3*MH**2*yt**2 - 12*MT**2*yt**2)*cmath.sqrt(MH**4 - 4*MH**2*MT**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((MH**2*ytau**2 - 4*MTA**2*ytau**2)*cmath.sqrt(MH**4 - 4*MH**2*MTA**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.W__minus__,P.W__plus__):'(((3*ee**4*vev**2)/(4.*sw**4) + (ee**4*MH**4*vev**2)/(16.*MW**4*sw**4) - (ee**4*MH**2*vev**2)/(4.*MW**2*sw**4))*cmath.sqrt(MH**4 - 4*MH**2*MW**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.Z,P.Z):'(((9*ee**4*vev**2)/2. + (3*ee**4*MH**4*vev**2)/(8.*MZ**4) - (3*ee**4*MH**2*vev**2)/(2.*MZ**2) + (3*cw**4*ee**4*vev**2)/(4.*sw**4) + (cw**4*ee**4*MH**4*vev**2)/(16.*MZ**4*sw**4) - (cw**4*ee**4*MH**2*vev**2)/(4.*MZ**2*sw**4) + (3*cw**2*ee**4*vev**2)/sw**2 + (cw**2*ee**4*MH**4*vev**2)/(4.*MZ**4*sw**2) - (cw**2*ee**4*MH**2*vev**2)/(MZ**2*sw**2) + (3*ee**4*sw**2*vev**2)/cw**2 + (ee**4*MH**4*sw**2*vev**2)/(4.*cw**2*MZ**4) - (ee**4*MH**2*sw**2*vev**2)/(cw**2*MZ**2) + (3*ee**4*sw**4*vev**2)/(4.*cw**4) + (ee**4*MH**4*sw**4*vev**2)/(16.*cw**4*MZ**4) - (ee**4*MH**2*sw**4*vev**2)/(4.*cw**4*MZ**2))*cmath.sqrt(MH**4 - 4*MH**2*MZ**2))/(32.*cmath.pi*abs(MH)**3)'})

Decay_mu__minus__ = Decay(name = 'Decay_mu__minus__',
                          particle = P.mu__minus__,
                          partial_widths = {(P.W__minus__,P.vm):'((MM**2 - MW**2)*((ee**2*MM**2)/(2.*sw**2) + (ee**2*MM**4)/(2.*MW**2*sw**2) - (ee**2*MW**2)/sw**2))/(32.*cmath.pi*abs(MM)**3)'})

Decay_t = Decay(name = 'Decay_t',
                particle = P.t,
                partial_widths = {(P.W__plus__,P.b):'(((3*ee**2*MB**2)/(2.*sw**2) + (3*ee**2*MT**2)/(2.*sw**2) + (3*ee**2*MB**4)/(2.*MW**2*sw**2) - (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) + (3*ee**2*MT**4)/(2.*MW**2*sw**2) - (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MT)**3)'})

Decay_ta__minus__ = Decay(name = 'Decay_ta__minus__',
                          particle = P.ta__minus__,
                          partial_widths = {(P.W__minus__,P.vt):'((MTA**2 - MW**2)*((ee**2*MTA**2)/(2.*sw**2) + (ee**2*MTA**4)/(2.*MW**2*sw**2) - (ee**2*MW**2)/sw**2))/(32.*cmath.pi*abs(MTA)**3)'})

Decay_vx = Decay(name = 'Decay_vx',
                 particle = P.vx,
                 partial_widths = {(P.b,P.b__tilde__):'((24*gxd**2*MB**2 + 12*gxd**2*MVdark**2)*cmath.sqrt(-4*MB**2*MVdark**2 + MVdark**4))/(48.*cmath.pi*abs(MVdark)**3)',
                                   (P.c,P.c__tilde__):'(gxu**2*MVdark**4)/(4.*cmath.pi*abs(MVdark)**3)',
                                   (P.n2,P.n1):'((4*gx**2*MVdark**2 - 2*gx**2*MX**2 - (2*gx**2*MX**4)/MVdark**2 + 12*gx**2*MX*MX2 - 2*gx**2*MX2**2 + (4*gx**2*MX**2*MX2**2)/MVdark**2 - (2*gx**2*MX2**4)/MVdark**2)*cmath.sqrt(MVdark**4 - 2*MVdark**2*MX**2 + MX**4 - 2*MVdark**2*MX2**2 - 2*MX**2*MX2**2 + MX2**4))/(48.*cmath.pi*abs(MVdark)**3)',
                                   (P.d,P.d__tilde__):'(gxd**2*MVdark**4)/(4.*cmath.pi*abs(MVdark)**3)',
                                   (P.e__minus__,P.e__plus__):'(gxl**2*MVdark**4)/(12.*cmath.pi*abs(MVdark)**3)',
                                   (P.mu__minus__,P.mu__plus__):'((8*gxl**2*MM**2 + 4*gxl**2*MVdark**2)*cmath.sqrt(-4*MM**2*MVdark**2 + MVdark**4))/(48.*cmath.pi*abs(MVdark)**3)',
                                   (P.s,P.s__tilde__):'(gxd**2*MVdark**4)/(4.*cmath.pi*abs(MVdark)**3)',
                                   (P.t,P.t__tilde__):'((24*gxu**2*MT**2 + 12*gxu**2*MVdark**2)*cmath.sqrt(-4*MT**2*MVdark**2 + MVdark**4))/(48.*cmath.pi*abs(MVdark)**3)',
                                   (P.ta__minus__,P.ta__plus__):'((8*gxl**2*MTA**2 + 4*gxl**2*MVdark**2)*cmath.sqrt(-4*MTA**2*MVdark**2 + MVdark**4))/(48.*cmath.pi*abs(MVdark)**3)',
                                   (P.u,P.u__tilde__):'(gxu**2*MVdark**4)/(4.*cmath.pi*abs(MVdark)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.c,P.d__tilde__):'(CKM2x1*ee**2*MW**4*complexconjugate(CKM2x1))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.c,P.s__tilde__):'(CKM2x2*ee**2*MW**4*complexconjugate(CKM2x2))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.t,P.b__tilde__):'(((-3*ee**2*MB**2)/(2.*sw**2) - (3*ee**2*MT**2)/(2.*sw**2) - (3*ee**2*MB**4)/(2.*MW**2*sw**2) + (3*ee**2*MB**2*MT**2)/(MW**2*sw**2) - (3*ee**2*MT**4)/(2.*MW**2*sw**2) + (3*ee**2*MW**2)/sw**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.u,P.d__tilde__):'(CKM1x1*ee**2*MW**4*complexconjugate(CKM1x1))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.u,P.s__tilde__):'(CKM1x2*ee**2*MW**4*complexconjugate(CKM1x2))/(16.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.ve,P.e__plus__):'(ee**2*MW**4)/(48.*cmath.pi*sw**2*abs(MW)**3)',
                                          (P.vm,P.mu__plus__):'((-MM**2 + MW**2)*(-(ee**2*MM**2)/(2.*sw**2) - (ee**2*MM**4)/(2.*MW**2*sw**2) + (ee**2*MW**2)/sw**2))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.vt,P.ta__plus__):'((-MTA**2 + MW**2)*(-(ee**2*MTA**2)/(2.*sw**2) - (ee**2*MTA**4)/(2.*MW**2*sw**2) + (ee**2*MW**2)/sw**2))/(48.*cmath.pi*abs(MW)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.b,P.b__tilde__):'((-7*ee**2*MB**2 + ee**2*MZ**2 - (3*cw**2*ee**2*MB**2)/(2.*sw**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) - (17*ee**2*MB**2*sw**2)/(6.*cw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2))*cmath.sqrt(-4*MB**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.c,P.c__tilde__):'(MZ**2*(-(ee**2*MZ**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.d,P.d__tilde__):'(MZ**2*(ee**2*MZ**2 + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.e__minus__,P.e__plus__):'(MZ**2*(-(ee**2*MZ**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.mu__minus__,P.mu__plus__):'((-5*ee**2*MM**2 - ee**2*MZ**2 - (cw**2*ee**2*MM**2)/(2.*sw**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (7*ee**2*MM**2*sw**2)/(2.*cw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2))*cmath.sqrt(-4*MM**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.s,P.s__tilde__):'(MZ**2*(ee**2*MZ**2 + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (5*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.t,P.t__tilde__):'((-11*ee**2*MT**2 - ee**2*MZ**2 - (3*cw**2*ee**2*MT**2)/(2.*sw**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (7*ee**2*MT**2*sw**2)/(6.*cw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2))*cmath.sqrt(-4*MT**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((-5*ee**2*MTA**2 - ee**2*MZ**2 - (cw**2*ee**2*MTA**2)/(2.*sw**2) + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (7*ee**2*MTA**2*sw**2)/(2.*cw**2) + (5*ee**2*MZ**2*sw**2)/(2.*cw**2))*cmath.sqrt(-4*MTA**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.u,P.u__tilde__):'(MZ**2*(-(ee**2*MZ**2) + (3*cw**2*ee**2*MZ**2)/(2.*sw**2) + (17*ee**2*MZ**2*sw**2)/(6.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ve,P.ve__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vm,P.vm__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vt,P.vt__tilde__):'(MZ**2*(ee**2*MZ**2 + (cw**2*ee**2*MZ**2)/(2.*sw**2) + (ee**2*MZ**2*sw**2)/(2.*cw**2)))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.W__minus__,P.W__plus__):'(((-12*cw**2*ee**2*MW**2)/sw**2 - (17*cw**2*ee**2*MZ**2)/sw**2 + (4*cw**2*ee**2*MZ**4)/(MW**2*sw**2) + (cw**2*ee**2*MZ**6)/(4.*MW**4*sw**2))*cmath.sqrt(-4*MW**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)'})

