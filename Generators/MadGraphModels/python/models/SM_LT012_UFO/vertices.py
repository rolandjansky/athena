# This file was automatically created by FeynRules $Revision: 595 $
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (November 20, 2012)
# Date: Fri 7 Jun 2013 19:02:32


from object_library import all_vertices, Vertex
import particles as P
import couplings as C
import lorentz as L


V_1 = Vertex(name = 'V_1',
             particles = [ P.G, P.G, P.G ],
             color = [ 'f(1,2,3)' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):C.GC_12})

V_2 = Vertex(name = 'V_2',
             particles = [ P.G, P.G, P.G, P.G ],
             color = [ 'f(-1,1,2)*f(3,4,-1)', 'f(-1,1,3)*f(2,4,-1)', 'f(-1,1,4)*f(2,3,-1)' ],
             lorentz = [ L.VVVV2, L.VVVV5, L.VVVV6 ],
             couplings = {(1,1):C.GC_14,(0,0):C.GC_14,(2,2):C.GC_14})

V_3 = Vertex(name = 'V_3',
             particles = [ P.A, P.W__minus__, P.W__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):C.GC_61})

V_4 = Vertex(name = 'V_4',
             particles = [ P.A, P.A, P.W__minus__, P.W__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVVV10, L.VVVV12, L.VVVV3, L.VVVV4 ],
             couplings = {(0,2):C.GC_88,(0,0):C.GC_89,(0,1):C.GC_87,(0,3):C.GC_94})

V_5 = Vertex(name = 'V_5',
             particles = [ P.W__minus__, P.W__plus__, P.Z ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):C.GC_15})

V_6 = Vertex(name = 'V_6',
             particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVVV15, L.VVVV3, L.VVVV4, L.VVVV9 ],
             couplings = {(0,1):C.GC_4,(0,0):C.GC_6,(0,3):C.GC_9,(0,2):C.GC_22})

V_7 = Vertex(name = 'V_7',
             particles = [ P.A, P.W__minus__, P.W__plus__, P.Z ],
             color = [ '1' ],
             lorentz = [ L.VVVV1, L.VVVV13, L.VVVV7, L.VVVV8 ],
             couplings = {(0,0):C.GC_57,(0,3):C.GC_59,(0,1):C.GC_58,(0,2):C.GC_68})

V_8 = Vertex(name = 'V_8',
             particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z ],
             color = [ '1' ],
             lorentz = [ L.VVVV10, L.VVVV12, L.VVVV3, L.VVVV4 ],
             couplings = {(0,2):C.GC_7,(0,0):C.GC_10,(0,1):C.GC_5,(0,3):C.GC_23})

V_9 = Vertex(name = 'V_9',
             particles = [ P.H, P.H, P.H, P.H ],
             color = [ '1' ],
             lorentz = [ L.SSSS1 ],
             couplings = {(0,0):C.GC_46})

V_10 = Vertex(name = 'V_10',
              particles = [ P.H, P.H, P.H ],
              color = [ '1' ],
              lorentz = [ L.SSS1 ],
              couplings = {(0,0):C.GC_136})

V_11 = Vertex(name = 'V_11',
              particles = [ P.W__minus__, P.W__plus__, P.H, P.H ],
              color = [ '1' ],
              lorentz = [ L.VVSS1 ],
              couplings = {(0,0):C.GC_47})

V_12 = Vertex(name = 'V_12',
              particles = [ P.W__minus__, P.W__plus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.VVS1 ],
              couplings = {(0,0):C.GC_137})

V_13 = Vertex(name = 'V_13',
              particles = [ P.Z, P.Z, P.H, P.H ],
              color = [ '1' ],
              lorentz = [ L.VVSS1 ],
              couplings = {(0,0):C.GC_132})

V_14 = Vertex(name = 'V_14',
              particles = [ P.Z, P.Z, P.H ],
              color = [ '1' ],
              lorentz = [ L.VVS1 ],
              couplings = {(0,0):C.GC_138})

V_15 = Vertex(name = 'V_15',
              particles = [ P.b__tilde__, P.b, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_16 = Vertex(name = 'V_16',
              particles = [ P.d__tilde__, P.d, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_17 = Vertex(name = 'V_17',
              particles = [ P.s__tilde__, P.s, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_18 = Vertex(name = 'V_18',
              particles = [ P.c__tilde__, P.c, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_19 = Vertex(name = 'V_19',
              particles = [ P.t__tilde__, P.t, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_20 = Vertex(name = 'V_20',
              particles = [ P.u__tilde__, P.u, P.G ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_13})

V_21 = Vertex(name = 'V_21',
              particles = [ P.b__tilde__, P.b, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_22 = Vertex(name = 'V_22',
              particles = [ P.d__tilde__, P.d, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_23 = Vertex(name = 'V_23',
              particles = [ P.s__tilde__, P.s, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_1})

V_24 = Vertex(name = 'V_24',
              particles = [ P.e__plus__, P.e__minus__, P.A ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_25 = Vertex(name = 'V_25',
              particles = [ P.m__plus__, P.m__minus__, P.A ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_26 = Vertex(name = 'V_26',
              particles = [ P.tt__plus__, P.tt__minus__, P.A ],
              color = [ '1' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_3})

V_27 = Vertex(name = 'V_27',
              particles = [ P.c__tilde__, P.c, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_28 = Vertex(name = 'V_28',
              particles = [ P.t__tilde__, P.t, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_29 = Vertex(name = 'V_29',
              particles = [ P.u__tilde__, P.u, P.A ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_30 = Vertex(name = 'V_30',
              particles = [ P.e__plus__, P.ve, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_31 = Vertex(name = 'V_31',
              particles = [ P.m__plus__, P.vm, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_32 = Vertex(name = 'V_32',
              particles = [ P.tt__plus__, P.vt, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_33 = Vertex(name = 'V_33',
              particles = [ P.d__tilde__, P.c, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_51})

V_34 = Vertex(name = 'V_34',
              particles = [ P.s__tilde__, P.c, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_52})

V_35 = Vertex(name = 'V_35',
              particles = [ P.b__tilde__, P.t, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_36 = Vertex(name = 'V_36',
              particles = [ P.d__tilde__, P.u, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_49})

V_37 = Vertex(name = 'V_37',
              particles = [ P.s__tilde__, P.u, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_50})

V_38 = Vertex(name = 'V_38',
              particles = [ P.ve__tilde__, P.e__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_39 = Vertex(name = 'V_39',
              particles = [ P.vm__tilde__, P.m__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_40 = Vertex(name = 'V_40',
              particles = [ P.vt__tilde__, P.tt__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_41 = Vertex(name = 'V_41',
              particles = [ P.t__tilde__, P.b, P.W__plus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_48})

V_42 = Vertex(name = 'V_42',
              particles = [ P.c__tilde__, P.d, P.W__plus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_144})

V_43 = Vertex(name = 'V_43',
              particles = [ P.u__tilde__, P.d, P.W__plus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_142})

V_44 = Vertex(name = 'V_44',
              particles = [ P.c__tilde__, P.s, P.W__plus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_145})

V_45 = Vertex(name = 'V_45',
              particles = [ P.u__tilde__, P.s, P.W__plus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_143})

V_46 = Vertex(name = 'V_46',
              particles = [ P.b__tilde__, P.b, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_55})

V_47 = Vertex(name = 'V_47',
              particles = [ P.d__tilde__, P.d, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_55})

V_48 = Vertex(name = 'V_48',
              particles = [ P.s__tilde__, P.s, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_55})

V_49 = Vertex(name = 'V_49',
              particles = [ P.e__plus__, P.e__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV4 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_56})

V_50 = Vertex(name = 'V_50',
              particles = [ P.m__plus__, P.m__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV4 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_56})

V_51 = Vertex(name = 'V_51',
              particles = [ P.tt__plus__, P.tt__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV4 ],
              couplings = {(0,0):C.GC_53,(0,1):C.GC_56})

V_52 = Vertex(name = 'V_52',
              particles = [ P.c__tilde__, P.c, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV5 ],
              couplings = {(0,0):C.GC_54,(0,1):C.GC_55})

V_53 = Vertex(name = 'V_53',
              particles = [ P.t__tilde__, P.t, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV5 ],
              couplings = {(0,0):C.GC_54,(0,1):C.GC_55})

V_54 = Vertex(name = 'V_54',
              particles = [ P.u__tilde__, P.u, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV5 ],
              couplings = {(0,0):C.GC_54,(0,1):C.GC_55})

V_55 = Vertex(name = 'V_55',
              particles = [ P.ve__tilde__, P.ve, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_130})

V_56 = Vertex(name = 'V_56',
              particles = [ P.vm__tilde__, P.vm, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_130})

V_57 = Vertex(name = 'V_57',
              particles = [ P.vt__tilde__, P.vt, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_130})

V_58 = Vertex(name = 'V_58',
              particles = [ P.b__tilde__, P.b, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1 ],
              couplings = {(0,0):C.GC_139})

V_59 = Vertex(name = 'V_59',
              particles = [ P.tt__plus__, P.tt__minus__, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1 ],
              couplings = {(0,0):C.GC_141})

V_60 = Vertex(name = 'V_60',
              particles = [ P.t__tilde__, P.t, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1 ],
              couplings = {(0,0):C.GC_140})

V_61 = Vertex(name = 'V_61',
              particles = [ P.A, P.A, P.A, P.A ],
              color = [ '1' ],
              lorentz = [ L.VVVV11, L.VVVV14 ],
              couplings = {(0,1):C.GC_135,(0,0):C.GC_123})

V_62 = Vertex(name = 'V_62',
              particles = [ P.A, P.A, P.A, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV11, L.VVVV14 ],
              couplings = {(0,1):C.GC_134,(0,0):C.GC_110})

V_63 = Vertex(name = 'V_63',
              particles = [ P.A, P.A, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV11, L.VVVV14 ],
              couplings = {(0,1):C.GC_133,(0,0):C.GC_90})

V_64 = Vertex(name = 'V_64',
              particles = [ P.A, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV11, L.VVVV14 ],
              couplings = {(0,1):C.GC_131,(0,0):C.GC_60})

V_65 = Vertex(name = 'V_65',
              particles = [ P.Z, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV11, L.VVVV14 ],
              couplings = {(0,1):C.GC_8,(0,0):C.GC_11})

V_66 = Vertex(name = 'V_66',
              particles = [ P.A, P.A, P.A, P.W__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVV1, L.VVVVV6, L.VVVVV7 ],
              couplings = {(0,2):C.GC_113,(0,0):C.GC_112,(0,1):C.GC_111})

V_67 = Vertex(name = 'V_67',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVV11, L.VVVVV16, L.VVVVV2 ],
              couplings = {(0,0):C.GC_66,(0,2):C.GC_62,(0,1):C.GC_64})

V_68 = Vertex(name = 'V_68',
              particles = [ P.A, P.A, P.W__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVV13, L.VVVVV3, L.VVVVV9 ],
              couplings = {(0,2):C.GC_93,(0,0):C.GC_92,(0,1):C.GC_91})

V_69 = Vertex(name = 'V_69',
              particles = [ P.A, P.W__minus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVV10, L.VVVVV17, L.VVVVV4 ],
              couplings = {(0,0):C.GC_67,(0,2):C.GC_65,(0,1):C.GC_63})

V_70 = Vertex(name = 'V_70',
              particles = [ P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV17, L.VVVVVV26, L.VVVVVV8 ],
              couplings = {(0,2):C.GC_95,(0,1):C.GC_97,(0,0):C.GC_99})

V_71 = Vertex(name = 'V_71',
              particles = [ P.A, P.A, P.A, P.A, P.W__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV1, L.VVVVVV10, L.VVVVVV13 ],
              couplings = {(0,0):C.GC_125,(0,1):C.GC_126,(0,2):C.GC_124})

V_72 = Vertex(name = 'V_72',
              particles = [ P.A, P.A, P.A, P.W__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV15, L.VVVVVV3, L.VVVVVV7 ],
              couplings = {(0,1):C.GC_115,(0,0):C.GC_116,(0,2):C.GC_114})

V_73 = Vertex(name = 'V_73',
              particles = [ P.A, P.A, P.W__minus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV14, L.VVVVVV2, L.VVVVVV24 ],
              couplings = {(0,1):C.GC_98,(0,0):C.GC_100,(0,2):C.GC_96})

V_74 = Vertex(name = 'V_74',
              particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVV14, L.VVVVV5, L.VVVVV8 ],
              couplings = {(0,2):C.GC_20,(0,0):C.GC_18,(0,1):C.GC_16})

V_75 = Vertex(name = 'V_75',
              particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVV12, L.VVVVV15, L.VVVVV18 ],
              couplings = {(0,2):C.GC_17,(0,0):C.GC_21,(0,1):C.GC_19})

V_76 = Vertex(name = 'V_76',
              particles = [ P.A, P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV1, L.VVVVVVV11, L.VVVVVVV17 ],
              couplings = {(0,0):C.GC_117,(0,2):C.GC_119,(0,1):C.GC_118})

V_77 = Vertex(name = 'V_77',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV11, L.VVVVVV19, L.VVVVVV22 ],
              couplings = {(0,2):C.GC_71,(0,1):C.GC_73,(0,0):C.GC_69})

V_78 = Vertex(name = 'V_78',
              particles = [ P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV12, L.VVVVVV16, L.VVVVVV4 ],
              couplings = {(0,1):C.GC_30,(0,2):C.GC_27,(0,0):C.GC_24})

V_79 = Vertex(name = 'V_79',
              particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV23, L.VVVVVV27, L.VVVVVV9 ],
              couplings = {(0,0):C.GC_25,(0,1):C.GC_28,(0,2):C.GC_31})

V_80 = Vertex(name = 'V_80',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV3, L.VVVVVVV4, L.VVVVVVV8 ],
              couplings = {(0,0):C.GC_77,(0,1):C.GC_79,(0,2):C.GC_75})

V_81 = Vertex(name = 'V_81',
              particles = [ P.A, P.A, P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV17, L.VVVVVVVV2, L.VVVVVVVV24 ],
              couplings = {(0,1):C.GC_127,(0,2):C.GC_128,(0,0):C.GC_129})

V_82 = Vertex(name = 'V_82',
              particles = [ P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV12, L.VVVVVVV13, L.VVVVVVV6 ],
              couplings = {(0,0):C.GC_102,(0,1):C.GC_103,(0,2):C.GC_101})

V_83 = Vertex(name = 'V_83',
              particles = [ P.A, P.A, P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV16, L.VVVVVVVV22, L.VVVVVVVV4 ],
              couplings = {(0,2):C.GC_106,(0,0):C.GC_108,(0,1):C.GC_104})

V_84 = Vertex(name = 'V_84',
              particles = [ P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV10, L.VVVVVVV16, L.VVVVVVV18 ],
              couplings = {(0,1):C.GC_33,(0,2):C.GC_37,(0,0):C.GC_35})

V_85 = Vertex(name = 'V_85',
              particles = [ P.W__minus__, P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV21 ],
              couplings = {(0,0):C.GC_45})

V_86 = Vertex(name = 'V_86',
              particles = [ P.A, P.W__minus__, P.W__plus__, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV18, L.VVVVVV21, L.VVVVVV5 ],
              couplings = {(0,0):C.GC_74,(0,1):C.GC_70,(0,2):C.GC_72})

V_87 = Vertex(name = 'V_87',
              particles = [ P.A, P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV12, L.VVVVVVVV14, L.VVVVVVVV7 ],
              couplings = {(0,2):C.GC_121,(0,1):C.GC_122,(0,0):C.GC_120})

V_88 = Vertex(name = 'V_88',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV2, L.VVVVVVV5, L.VVVVVVV7 ],
              couplings = {(0,0):C.GC_76,(0,1):C.GC_78,(0,2):C.GC_80})

V_89 = Vertex(name = 'V_89',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV10, L.VVVVVVVV18, L.VVVVVVVV6 ],
              couplings = {(0,2):C.GC_81,(0,0):C.GC_85,(0,1):C.GC_83})

V_90 = Vertex(name = 'V_90',
              particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVV20, L.VVVVVV25, L.VVVVVV6 ],
              couplings = {(0,2):C.GC_29,(0,0):C.GC_32,(0,1):C.GC_26})

V_91 = Vertex(name = 'V_91',
              particles = [ P.A, P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV15, L.VVVVVVVV3, L.VVVVVVVV9 ],
              couplings = {(0,1):C.GC_105,(0,0):C.GC_107,(0,2):C.GC_109})

V_92 = Vertex(name = 'V_92',
              particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVV14, L.VVVVVVV15, L.VVVVVVV9 ],
              couplings = {(0,0):C.GC_36,(0,1):C.GC_38,(0,2):C.GC_34})

V_93 = Vertex(name = 'V_93',
              particles = [ P.W__minus__, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV1, L.VVVVVVVV19, L.VVVVVVVV25 ],
              couplings = {(0,0):C.GC_41,(0,1):C.GC_43,(0,2):C.GC_39})

V_94 = Vertex(name = 'V_94',
              particles = [ P.A, P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV11, L.VVVVVVVV13, L.VVVVVVVV8 ],
              couplings = {(0,2):C.GC_84,(0,1):C.GC_86,(0,0):C.GC_82})

V_95 = Vertex(name = 'V_95',
              particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__, P.Z, P.Z, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVVVVVV20, L.VVVVVVVV23, L.VVVVVVVV5 ],
              couplings = {(0,2):C.GC_40,(0,1):C.GC_42,(0,0):C.GC_44})

