# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.0 for Mac OS X x86 (64-bit) (September 10, 2014)
# Date: Sat 4 Jul 2015 04:38:51


from object_library import all_decays, Decay
import particles as P


Decay_H = Decay(name = 'Decay_H',
                particle = P.H,
                partial_widths = {(P.a,P.a):'(MH**2*((2*eAASM**2*kAA**2*MH**4)/vF**2 + (2*eAASM**2*lAACP**2*MH**4)/vF**2))/(32.*cmath.pi*abs(MH)**3)',
                                  (P.g,P.g):'(eggSM**2*MH**6)/(2.*cmath.pi*vF**2*abs(MH)**3)',
                                  (P.W__minus__,P.W__plus__):'(((2*eWW**2*MH**4)/vF**2 + (2*eWWCP**2*MH**4)/vF**2 + (kWW**2*MH**4)/vF**2 - (8*eWW**2*MH**2*MW**2)/vF**2 - (8*eWWCP**2*MH**2*MW**2)/vF**2 + (12*eWW*kWW*MH**2*MW**2)/vF**2 - (4*kWW**2*MH**2*MW**2)/vF**2 + (12*eWW**2*MW**4)/vF**2 - (24*eWW*kWW*MW**4)/vF**2 + (12*kWW**2*MW**4)/vF**2 + (2*kWW*MH**4*noise)/vF**2 + (12*eWW*MH**2*MW**2*noise)/vF**2 - (8*kWW*MH**2*MW**2*noise)/vF**2 - (24*eWW*MW**4*noise)/vF**2 + (24*kWW*MW**4*noise)/vF**2 + (MH**4*noise**2)/vF**2 - (4*MH**2*MW**2*noise**2)/vF**2 + (12*MW**4*noise**2)/vF**2)*cmath.sqrt(MH**4 - 4*MH**2*MW**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.a,P.Z):'((MH**2 - MZ**2)*((2*eZASM**2*kZA**2*MH**4)/vF**2 + (2*eZASM**2*lZACP**2*MH**4)/vF**2 - (4*eZASM**2*kZA**2*MH**2*MZ**2)/vF**2 - (4*eZASM**2*lZACP**2*MH**2*MZ**2)/vF**2 + (2*eZASM**2*kZA**2*MZ**4)/vF**2 + (2*eZASM**2*lZACP**2*MZ**4)/vF**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.Z,P.Z):'(((2*eZZ**2*MH**4)/vF**2 + (2*eZZCP**2*MH**4)/vF**2 + (kZZ**2*MH**4)/vF**2 - (8*eZZ**2*MH**2*MZ**2)/vF**2 - (8*eZZCP**2*MH**2*MZ**2)/vF**2 + (12*eZZ*kZZ*MH**2*MZ**2)/vF**2 - (4*kZZ**2*MH**2*MZ**2)/vF**2 + (12*eZZ**2*MZ**4)/vF**2 - (24*eZZ*kZZ*MZ**4)/vF**2 + (12*kZZ**2*MZ**4)/vF**2 + (2*kZZ*MH**4*noise)/vF**2 + (12*eZZ*MH**2*MZ**2*noise)/vF**2 - (8*kZZ*MH**2*MZ**2*noise)/vF**2 - (24*eZZ*MZ**4*noise)/vF**2 + (24*kZZ*MZ**4*noise)/vF**2 + (MH**4*noise**2)/vF**2 - (4*MH**2*MZ**2*noise**2)/vF**2 + (12*MZ**4*noise**2)/vF**2)*cmath.sqrt(MH**4 - 4*MH**2*MZ**2))/(32.*cmath.pi*abs(MH)**3)',
                                  (P.b,P.b__tilde__):'((-12*kb**2*MB**2*ybeff**2 + 3*kb**2*MH**2*ybeff**2 + 3*lb**2*MH**2*ybeff**2)*cmath.sqrt(-4*MB**2*MH**2 + MH**4))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.c,P.c__tilde__):'((-12*kc**2*MC**2*yceff**2 + 3*kc**2*MH**2*yceff**2 + 3*lc**2*MH**2*yceff**2)*cmath.sqrt(-4*MC**2*MH**2 + MH**4))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.mu__minus__,P.mu__plus__):'(MH**2*(kmu**2*MH**2*ymueff**2 + lmu**2*MH**2*ymueff**2))/(16.*cmath.pi*abs(MH)**3)',
                                  (P.ta__minus__,P.ta__plus__):'((ktau**2*MH**2*ytaueff**2 + ltau**2*MH**2*ytaueff**2 - 4*ktau**2*MTA**2*ytaueff**2)*cmath.sqrt(MH**4 - 4*MH**2*MTA**2))/(16.*cmath.pi*abs(MH)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.a,P.H):'((-MH**2 + MZ**2)*((2*eZASM**2*kZA**2*MH**4)/vF**2 + (2*eZASM**2*lZACP**2*MH**4)/vF**2 - (4*eZASM**2*kZA**2*MH**2*MZ**2)/vF**2 - (4*eZASM**2*lZACP**2*MH**2*MZ**2)/vF**2 + (2*eZASM**2*kZA**2*MZ**4)/vF**2 + (2*eZASM**2*lZACP**2*MZ**4)/vF**2))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.e__minus__,P.e__plus__):'(MZ**2*((8*gZeL**2*MZ**4)/vF**2 + (8*gZeR**2*MZ**4)/vF**2))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.mu__minus__,P.mu__plus__):'(MZ**2*((8*gZmuL**2*MZ**4)/vF**2 + (8*gZmuR**2*MZ**4)/vF**2))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ta__minus__,P.ta__plus__):'(((-8*gZtauL**2*MTA**2*MZ**2)/vF**2 + (48*gZtauL*gZtauR*MTA**2*MZ**2)/vF**2 - (8*gZtauR**2*MTA**2*MZ**2)/vF**2 + (8*gZtauL**2*MZ**4)/vF**2 + (8*gZtauR**2*MZ**4)/vF**2)*cmath.sqrt(-4*MTA**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ve,P.ve__tilde__):'(gZv**2*MZ**6)/(6.*cmath.pi*vF**2*abs(MZ)**3)',
                                  (P.vm,P.vm__tilde__):'(gZv**2*MZ**6)/(6.*cmath.pi*vF**2*abs(MZ)**3)',
                                  (P.vt,P.vt__tilde__):'(gZv**2*MZ**6)/(6.*cmath.pi*vF**2*abs(MZ)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.ve,P.e__plus__):'(gWe**2*MW**6)/(12.*cmath.pi*vF**2*abs(MW)**3)',
                                          (P.vm,P.mu__plus__):'(gWmu**2*MW**6)/(12.*cmath.pi*vF**2*abs(MW)**3)',
                                          (P.vt,P.ta__plus__):'((-MTA**2 + MW**2)*((-2*gWtau**2*MTA**4)/vF**2 - (2*gWtau**2*MTA**2*MW**2)/vF**2 + (4*gWtau**2*MW**4)/vF**2))/(48.*cmath.pi*abs(MW)**3)'})

Decay_ta__minus__ = Decay(name = 'Decay_ta__minus__',
                          particle = P.ta__minus__,
                          partial_widths = {(P.W__minus__,P.vt):'((MTA**2 - MW**2)*((2*gWtau**2*MTA**4)/vF**2 + (2*gWtau**2*MTA**2*MW**2)/vF**2 - (4*gWtau**2*MW**4)/vF**2))/(32.*cmath.pi*abs(MTA)**3)'})

