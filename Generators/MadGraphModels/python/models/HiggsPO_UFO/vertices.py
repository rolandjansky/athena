# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.0 for Mac OS X x86 (64-bit) (September 10, 2014)
# Date: Sat 4 Jul 2015 04:38:51


from object_library import all_vertices, Vertex
import particles as P
import couplings as C
import lorentz as L


V_1 = Vertex(name = 'V_1',
             particles = [ P.a, P.a, P.H ],
             color = [ '1' ],
             lorentz = [ L.VVS3, L.VVS5 ],
             couplings = {(0,0):C.GC_25,(0,1):C.GC_23})

V_2 = Vertex(name = 'V_2',
             particles = [ P.g, P.g, P.H ],
             color = [ 'Identity(1,2)' ],
             lorentz = [ L.VVS5 ],
             couplings = {(0,0):C.GC_9})

V_3 = Vertex(name = 'V_3',
             particles = [ P.W__minus__, P.W__plus__, P.H ],
             color = [ '1' ],
             lorentz = [ L.VVS2, L.VVS4, L.VVS5 ],
             couplings = {(0,0):C.GC_11,(0,2):C.GC_10,(0,1):C.GC_7})

V_4 = Vertex(name = 'V_4',
             particles = [ P.a, P.Z, P.H ],
             color = [ '1' ],
             lorentz = [ L.VVS1, L.VVS5 ],
             couplings = {(0,0):C.GC_26,(0,1):C.GC_24})

V_5 = Vertex(name = 'V_5',
             particles = [ P.Z, P.Z, P.H ],
             color = [ '1' ],
             lorentz = [ L.VVS3, L.VVS4, L.VVS5 ],
             couplings = {(0,0):C.GC_20,(0,2):C.GC_19,(0,1):C.GC_8})

V_6 = Vertex(name = 'V_6',
             particles = [ P.b__tilde__, P.b, P.H ],
             color = [ 'Identity(1,2)' ],
             lorentz = [ L.FFS1, L.FFS2 ],
             couplings = {(0,0):C.GC_38,(0,1):C.GC_37})

V_7 = Vertex(name = 'V_7',
             particles = [ P.c__tilde__, P.c, P.H ],
             color = [ 'Identity(1,2)' ],
             lorentz = [ L.FFS1, L.FFS2 ],
             couplings = {(0,0):C.GC_40,(0,1):C.GC_39})

V_8 = Vertex(name = 'V_8',
             particles = [ P.mu__plus__, P.mu__minus__, P.H ],
             color = [ '1' ],
             lorentz = [ L.FFS1, L.FFS2 ],
             couplings = {(0,0):C.GC_42,(0,1):C.GC_41})

V_9 = Vertex(name = 'V_9',
             particles = [ P.ta__plus__, P.ta__minus__, P.H ],
             color = [ '1' ],
             lorentz = [ L.FFS1, L.FFS2 ],
             couplings = {(0,0):C.GC_44,(0,1):C.GC_43})

V_10 = Vertex(name = 'V_10',
              particles = [ P.ghG, P.ghG__tilde__, P.g ],
              color = [ 'f(1,2,3)' ],
              lorentz = [ L.UUV1 ],
              couplings = {(0,0):C.GC_4})

V_11 = Vertex(name = 'V_11',
              particles = [ P.g, P.g, P.g ],
              color = [ 'f(1,2,3)' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):C.GC_4})

V_12 = Vertex(name = 'V_12',
              particles = [ P.g, P.g, P.g, P.g ],
              color = [ 'f(-1,1,2)*f(3,4,-1)', 'f(-1,1,3)*f(2,4,-1)', 'f(-1,1,4)*f(2,3,-1)' ],
              lorentz = [ L.VVVV1, L.VVVV2, L.VVVV3 ],
              couplings = {(1,1):C.GC_6,(0,0):C.GC_6,(2,2):C.GC_6})

V_13 = Vertex(name = 'V_13',
              particles = [ P.g, P.g, P.g, P.H ],
              color = [ 'f(1,2,3)' ],
              lorentz = [ L.VVVS1 ],
              couplings = {(0,0):C.GC_21})

V_14 = Vertex(name = 'V_14',
              particles = [ P.g, P.g, P.g, P.g, P.H ],
              color = [ 'f(-1,1,2)*f(3,4,-1)', 'f(-1,1,3)*f(2,4,-1)', 'f(-1,1,4)*f(2,3,-1)' ],
              lorentz = [ L.VVVVS1, L.VVVVS2, L.VVVVS3 ],
              couplings = {(1,1):C.GC_22,(0,0):C.GC_22,(2,2):C.GC_22})

V_15 = Vertex(name = 'V_15',
              particles = [ P.d__tilde__, P.d, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_16 = Vertex(name = 'V_16',
              particles = [ P.s__tilde__, P.s, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_17 = Vertex(name = 'V_17',
              particles = [ P.b__tilde__, P.b, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_18 = Vertex(name = 'V_18',
              particles = [ P.e__plus__, P.e__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_19 = Vertex(name = 'V_19',
              particles = [ P.mu__plus__, P.mu__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_20 = Vertex(name = 'V_20',
              particles = [ P.ta__plus__, P.ta__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_21 = Vertex(name = 'V_21',
              particles = [ P.u__tilde__, P.u, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_22 = Vertex(name = 'V_22',
              particles = [ P.c__tilde__, P.c, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_23 = Vertex(name = 'V_23',
              particles = [ P.t__tilde__, P.t, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_24 = Vertex(name = 'V_24',
              particles = [ P.u__tilde__, P.u, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_25 = Vertex(name = 'V_25',
              particles = [ P.c__tilde__, P.c, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_26 = Vertex(name = 'V_26',
              particles = [ P.t__tilde__, P.t, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_27 = Vertex(name = 'V_27',
              particles = [ P.d__tilde__, P.d, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_28 = Vertex(name = 'V_28',
              particles = [ P.s__tilde__, P.s, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_29 = Vertex(name = 'V_29',
              particles = [ P.b__tilde__, P.b, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_5})

V_30 = Vertex(name = 'V_30',
              particles = [ P.e__plus__, P.ve, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_27})

V_31 = Vertex(name = 'V_31',
              particles = [ P.e__plus__, P.ve, P.W__minus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_45})

V_32 = Vertex(name = 'V_32',
              particles = [ P.mu__plus__, P.vm, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_28})

V_33 = Vertex(name = 'V_33',
              particles = [ P.mu__plus__, P.vm, P.W__minus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_47})

V_34 = Vertex(name = 'V_34',
              particles = [ P.ta__plus__, P.vt, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_29})

V_35 = Vertex(name = 'V_35',
              particles = [ P.ta__plus__, P.vt, P.W__minus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_49})

V_36 = Vertex(name = 'V_36',
              particles = [ P.ve__tilde__, P.e__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_27})

V_37 = Vertex(name = 'V_37',
              particles = [ P.ve__tilde__, P.e__minus__, P.W__plus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_46})

V_38 = Vertex(name = 'V_38',
              particles = [ P.vm__tilde__, P.mu__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_28})

V_39 = Vertex(name = 'V_39',
              particles = [ P.vm__tilde__, P.mu__minus__, P.W__plus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_48})

V_40 = Vertex(name = 'V_40',
              particles = [ P.vt__tilde__, P.ta__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_29})

V_41 = Vertex(name = 'V_41',
              particles = [ P.vt__tilde__, P.ta__minus__, P.W__plus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_50})

V_42 = Vertex(name = 'V_42',
              particles = [ P.e__plus__, P.e__minus__, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1, L.FFVS2 ],
              couplings = {(0,0):C.GC_12,(0,1):C.GC_13})

V_43 = Vertex(name = 'V_43',
              particles = [ P.e__plus__, P.e__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_30,(0,1):C.GC_31})

V_44 = Vertex(name = 'V_44',
              particles = [ P.mu__plus__, P.mu__minus__, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1, L.FFVS2 ],
              couplings = {(0,0):C.GC_14,(0,1):C.GC_15})

V_45 = Vertex(name = 'V_45',
              particles = [ P.mu__plus__, P.mu__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_32,(0,1):C.GC_33})

V_46 = Vertex(name = 'V_46',
              particles = [ P.ta__plus__, P.ta__minus__, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1, L.FFVS2 ],
              couplings = {(0,0):C.GC_16,(0,1):C.GC_17})

V_47 = Vertex(name = 'V_47',
              particles = [ P.ta__plus__, P.ta__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_34,(0,1):C.GC_35})

V_48 = Vertex(name = 'V_48',
              particles = [ P.ve__tilde__, P.ve, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_18})

V_49 = Vertex(name = 'V_49',
              particles = [ P.ve__tilde__, P.ve, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_36})

V_50 = Vertex(name = 'V_50',
              particles = [ P.vm__tilde__, P.vm, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_18})

V_51 = Vertex(name = 'V_51',
              particles = [ P.vm__tilde__, P.vm, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_36})

V_52 = Vertex(name = 'V_52',
              particles = [ P.vt__tilde__, P.vt, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFVS1 ],
              couplings = {(0,0):C.GC_18})

V_53 = Vertex(name = 'V_53',
              particles = [ P.vt__tilde__, P.vt, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_36})

