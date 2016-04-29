# This file was automatically created by FeynRules 2.1.48
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Wed 2 Apr 2014 11:19:42


from object_library import all_vertices, all_CTvertices, Vertex, CTVertex
import particles as P
import CT_couplings as C
import lorentz as L


V_1 = CTVertex(name = 'V_1',
               type = 'R2',
               particles = [ P.g, P.g, P.g ],
               color = [ 'f(1,2,3)' ],
               lorentz = [ L.VVV1, L.VVV2, L.VVV4, L.VVV6, L.VVV7, L.VVV8 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(0,0,0):C.R2GC_320_79,(0,0,1):C.R2GC_320_80,(0,1,0):C.R2GC_323_81,(0,1,1):C.R2GC_323_82,(0,2,0):C.R2GC_323_81,(0,2,1):C.R2GC_323_82,(0,3,0):C.R2GC_320_79,(0,3,1):C.R2GC_320_80,(0,4,0):C.R2GC_320_79,(0,4,1):C.R2GC_320_80,(0,5,0):C.R2GC_323_81,(0,5,1):C.R2GC_323_82})

V_2 = CTVertex(name = 'V_2',
               type = 'R2',
               particles = [ P.g, P.g, P.g, P.g ],
               color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
               lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(2,0,0):C.R2GC_293_62,(2,0,1):C.R2GC_293_63,(0,0,0):C.R2GC_293_62,(0,0,1):C.R2GC_293_63,(4,0,0):C.R2GC_291_58,(4,0,1):C.R2GC_291_59,(3,0,0):C.R2GC_291_58,(3,0,1):C.R2GC_291_59,(8,0,0):C.R2GC_292_60,(8,0,1):C.R2GC_292_61,(7,0,0):C.R2GC_298_69,(7,0,1):C.R2GC_329_87,(6,0,0):C.R2GC_297_67,(6,0,1):C.R2GC_330_88,(5,0,0):C.R2GC_291_58,(5,0,1):C.R2GC_291_59,(1,0,0):C.R2GC_291_58,(1,0,1):C.R2GC_291_59,(11,0,0):C.R2GC_295_65,(11,0,1):C.R2GC_295_66,(10,0,0):C.R2GC_295_65,(10,0,1):C.R2GC_295_66,(9,0,1):C.R2GC_294_64,(2,1,0):C.R2GC_293_62,(2,1,1):C.R2GC_293_63,(0,1,0):C.R2GC_293_62,(0,1,1):C.R2GC_293_63,(6,1,0):C.R2GC_326_83,(6,1,1):C.R2GC_326_84,(4,1,0):C.R2GC_291_58,(4,1,1):C.R2GC_291_59,(3,1,0):C.R2GC_291_58,(3,1,1):C.R2GC_291_59,(8,1,0):C.R2GC_292_60,(8,1,1):C.R2GC_331_89,(7,1,0):C.R2GC_298_69,(7,1,1):C.R2GC_298_70,(5,1,0):C.R2GC_291_58,(5,1,1):C.R2GC_291_59,(1,1,0):C.R2GC_291_58,(1,1,1):C.R2GC_291_59,(11,1,0):C.R2GC_295_65,(11,1,1):C.R2GC_295_66,(10,1,0):C.R2GC_295_65,(10,1,1):C.R2GC_295_66,(9,1,1):C.R2GC_294_64,(2,2,0):C.R2GC_293_62,(2,2,1):C.R2GC_293_63,(0,2,0):C.R2GC_293_62,(0,2,1):C.R2GC_293_63,(4,2,0):C.R2GC_291_58,(4,2,1):C.R2GC_291_59,(3,2,0):C.R2GC_291_58,(3,2,1):C.R2GC_291_59,(8,2,0):C.R2GC_292_60,(8,2,1):C.R2GC_328_86,(6,2,0):C.R2GC_297_67,(6,2,1):C.R2GC_297_68,(7,2,0):C.R2GC_327_85,(7,2,1):C.R2GC_293_63,(5,2,0):C.R2GC_291_58,(5,2,1):C.R2GC_291_59,(1,2,0):C.R2GC_291_58,(1,2,1):C.R2GC_291_59,(11,2,0):C.R2GC_295_65,(11,2,1):C.R2GC_295_66,(10,2,0):C.R2GC_295_65,(10,2,1):C.R2GC_295_66,(9,2,1):C.R2GC_294_64})

V_3 = CTVertex(name = 'V_3',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.h__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_352_100,(0,1,0):C.R2GC_336_91})

V_4 = CTVertex(name = 'V_4',
               type = 'R2',
               particles = [ P.b__tilde__, P.t, P.h__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_353_101,(0,1,0):C.R2GC_335_90})

V_5 = CTVertex(name = 'V_5',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.G__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_343_94,(0,1,0):C.R2GC_349_98})

V_6 = CTVertex(name = 'V_6',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.G0 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS1 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_311_74})

V_7 = CTVertex(name = 'V_7',
               type = 'R2',
               particles = [ P.t__tilde__, P.t, P.G0 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS1 ],
               loop_particles = [ [ [P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_347_96})

V_8 = CTVertex(name = 'V_8',
               type = 'R2',
               particles = [ P.b__tilde__, P.t, P.G__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_348_97,(0,1,0):C.R2GC_344_95})

V_9 = CTVertex(name = 'V_9',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.h1 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_316_76,(0,1,0):C.R2GC_315_75})

V_10 = CTVertex(name = 'V_10',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_354_102,(0,1,0):C.R2GC_351_99})

V_11 = CTVertex(name = 'V_11',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_317_77,(0,1,0):C.R2GC_357_105})

V_12 = CTVertex(name = 'V_12',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_355_103,(0,1,0):C.R2GC_358_106})

V_13 = CTVertex(name = 'V_13',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_356_104,(0,1,0):C.R2GC_342_93})

V_14 = CTVertex(name = 'V_14',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_318_78,(0,1,0):C.R2GC_310_73})

V_15 = CTVertex(name = 'V_15',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_234_7})

V_16 = CTVertex(name = 'V_16',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_234_7})

V_17 = CTVertex(name = 'V_17',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_234_7})

V_18 = CTVertex(name = 'V_18',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_19 = CTVertex(name = 'V_19',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_20 = CTVertex(name = 'V_20',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_21 = CTVertex(name = 'V_21',
                type = 'R2',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_22 = CTVertex(name = 'V_22',
                type = 'R2',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_23 = CTVertex(name = 'V_23',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_24 = CTVertex(name = 'V_24',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_232_5,(0,1,0):C.R2GC_233_6})

V_25 = CTVertex(name = 'V_25',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_232_5,(0,1,0):C.R2GC_233_6})

V_26 = CTVertex(name = 'V_26',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_232_5,(0,1,0):C.R2GC_233_6})

V_27 = CTVertex(name = 'V_27',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_240_9})

V_28 = CTVertex(name = 'V_28',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_240_9})

V_29 = CTVertex(name = 'V_29',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,1,0):C.R2GC_231_4,(0,0,0):C.R2GC_240_9})

V_30 = CTVertex(name = 'V_30',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_31 = CTVertex(name = 'V_31',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_32 = CTVertex(name = 'V_32',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_230_3})

V_33 = CTVertex(name = 'V_33',
                type = 'R2',
                particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_34 = CTVertex(name = 'V_34',
                type = 'R2',
                particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_35 = CTVertex(name = 'V_35',
                type = 'R2',
                particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_301_71})

V_36 = CTVertex(name = 'V_36',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_238_8,(0,1,0):C.R2GC_233_6})

V_37 = CTVertex(name = 'V_37',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_238_8,(0,1,0):C.R2GC_233_6})

V_38 = CTVertex(name = 'V_38',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_238_8,(0,1,0):C.R2GC_233_6})

V_39 = CTVertex(name = 'V_39',
                type = 'R2',
                particles = [ P.b__tilde__, P.b ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_307_72,(0,1,0):C.R2GC_229_2})

V_40 = CTVertex(name = 'V_40',
                type = 'R2',
                particles = [ P.c__tilde__, P.c ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_229_2})

V_41 = CTVertex(name = 'V_41',
                type = 'R2',
                particles = [ P.d__tilde__, P.d ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_229_2})

V_42 = CTVertex(name = 'V_42',
                type = 'R2',
                particles = [ P.s__tilde__, P.s ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_229_2})

V_43 = CTVertex(name = 'V_43',
                type = 'R2',
                particles = [ P.t__tilde__, P.t ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_339_92,(0,1,0):C.R2GC_229_2})

V_44 = CTVertex(name = 'V_44',
                type = 'R2',
                particles = [ P.u__tilde__, P.u ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_229_2})

V_45 = CTVertex(name = 'V_45',
                type = 'R2',
                particles = [ P.g, P.g ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VV2, L.VV3, L.VV4 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ], [ [P.t] ] ],
                couplings = {(0,2,2):C.R2GC_228_1,(0,0,0):C.R2GC_253_10,(0,0,3):C.R2GC_253_11,(0,1,1):C.R2GC_267_38})

V_46 = CTVertex(name = 'V_46',
                type = 'R2',
                particles = [ P.g, P.g, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_255_14,(0,0,1):C.R2GC_255_15})

V_47 = CTVertex(name = 'V_47',
                type = 'R2',
                particles = [ P.g, P.g, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_256_16,(0,0,1):C.R2GC_256_17})

V_48 = CTVertex(name = 'V_48',
                type = 'R2',
                particles = [ P.g, P.g, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_257_18,(0,0,1):C.R2GC_257_19})

V_49 = CTVertex(name = 'V_49',
                type = 'R2',
                particles = [ P.g, P.g, P.W__minus__, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b, P.t], [P.c, P.s], [P.d, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_279_57})

V_50 = CTVertex(name = 'V_50',
                type = 'R2',
                particles = [ P.g, P.g, P.Z, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_272_47,(0,0,1):C.R2GC_272_48})

V_51 = CTVertex(name = 'V_51',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.Z ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_273_49,(0,0,1):C.R2GC_273_50})

V_52 = CTVertex(name = 'V_52',
                type = 'R2',
                particles = [ P.a, P.a, P.g, P.g ],
                color = [ 'Identity(3,4)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_274_51,(0,0,1):C.R2GC_274_52})

V_53 = CTVertex(name = 'V_53',
                type = 'R2',
                particles = [ P.g, P.g, P.g, P.Z ],
                color = [ 'd(1,2,3)', 'f(1,2,3)' ],
                lorentz = [ L.VVVV1, L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_269_41,(1,0,1):C.R2GC_269_42,(0,1,0):C.R2GC_268_39,(0,1,1):C.R2GC_268_40})

V_54 = CTVertex(name = 'V_54',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.g ],
                color = [ 'd(2,3,4)', 'f(2,3,4)' ],
                lorentz = [ L.VVVV1, L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_271_45,(1,0,1):C.R2GC_271_46,(0,1,0):C.R2GC_270_43,(0,1,1):C.R2GC_270_44})

V_55 = CTVertex(name = 'V_55',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_254_12,(0,0,1):C.R2GC_254_13})

V_56 = CTVertex(name = 'V_56',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_275_53})

V_57 = CTVertex(name = 'V_57',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_276_54})

V_58 = CTVertex(name = 'V_58',
                type = 'R2',
                particles = [ P.g, P.g, P.G__plus__, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_278_56})

V_59 = CTVertex(name = 'V_59',
                type = 'R2',
                particles = [ P.g, P.g, P.h__minus__, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_277_55})

V_60 = CTVertex(name = 'V_60',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_259_22,(0,0,1):C.R2GC_259_23})

V_61 = CTVertex(name = 'V_61',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_260_24,(0,0,1):C.R2GC_260_25})

V_62 = CTVertex(name = 'V_62',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_261_26,(0,0,1):C.R2GC_261_27})

V_63 = CTVertex(name = 'V_63',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_262_28,(0,0,1):C.R2GC_262_29})

V_64 = CTVertex(name = 'V_64',
                type = 'R2',
                particles = [ P.g, P.g, P.h2, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_263_30,(0,0,1):C.R2GC_263_31})

V_65 = CTVertex(name = 'V_65',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_264_32,(0,0,1):C.R2GC_264_33})

V_66 = CTVertex(name = 'V_66',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_265_34,(0,0,1):C.R2GC_265_35})

V_67 = CTVertex(name = 'V_67',
                type = 'R2',
                particles = [ P.g, P.g, P.h2, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_266_36,(0,0,1):C.R2GC_266_37})

V_68 = CTVertex(name = 'V_68',
                type = 'R2',
                particles = [ P.g, P.g, P.h3, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_258_20,(0,0,1):C.R2GC_258_21})

V_69 = CTVertex(name = 'V_69',
                type = 'UV',
                particles = [ P.g, P.g, P.g ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VVV1, L.VVV2, L.VVV3, L.VVV4, L.VVV5, L.VVV6, L.VVV7, L.VVV8 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_320_46,(0,0,1):C.UVGC_320_47,(0,0,2):C.UVGC_320_48,(0,0,3):C.UVGC_290_8,(0,0,4):C.UVGC_320_49,(0,1,0):C.UVGC_323_54,(0,1,1):C.UVGC_323_55,(0,1,2):C.UVGC_323_56,(0,1,3):C.UVGC_323_57,(0,1,4):C.UVGC_323_58,(0,3,0):C.UVGC_323_54,(0,3,1):C.UVGC_323_55,(0,3,2):C.UVGC_325_61,(0,3,3):C.UVGC_289_6,(0,3,4):C.UVGC_323_58,(0,5,0):C.UVGC_320_46,(0,5,1):C.UVGC_320_47,(0,5,2):C.UVGC_322_52,(0,5,3):C.UVGC_322_53,(0,5,4):C.UVGC_320_49,(0,6,0):C.UVGC_320_46,(0,6,1):C.UVGC_320_47,(0,6,2):C.UVGC_321_50,(0,6,3):C.UVGC_321_51,(0,6,4):C.UVGC_320_49,(0,7,0):C.UVGC_323_54,(0,7,1):C.UVGC_323_55,(0,7,2):C.UVGC_324_59,(0,7,3):C.UVGC_324_60,(0,7,4):C.UVGC_323_58,(0,2,2):C.UVGC_289_5,(0,2,3):C.UVGC_289_6,(0,4,2):C.UVGC_290_7,(0,4,3):C.UVGC_290_8})

V_70 = CTVertex(name = 'V_70',
                type = 'UV',
                particles = [ P.g, P.g, P.g, P.g ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(2,0,3):C.UVGC_292_12,(2,0,4):C.UVGC_292_11,(0,0,3):C.UVGC_292_12,(0,0,4):C.UVGC_292_11,(4,0,3):C.UVGC_291_9,(4,0,4):C.UVGC_291_10,(3,0,3):C.UVGC_291_9,(3,0,4):C.UVGC_291_10,(8,0,3):C.UVGC_292_11,(8,0,4):C.UVGC_292_12,(7,0,0):C.UVGC_329_73,(7,0,2):C.UVGC_329_74,(7,0,3):C.UVGC_329_75,(7,0,4):C.UVGC_329_76,(7,0,5):C.UVGC_329_77,(6,0,0):C.UVGC_329_73,(6,0,2):C.UVGC_329_74,(6,0,3):C.UVGC_330_78,(6,0,4):C.UVGC_330_79,(6,0,5):C.UVGC_329_77,(5,0,3):C.UVGC_291_9,(5,0,4):C.UVGC_291_10,(1,0,3):C.UVGC_291_9,(1,0,4):C.UVGC_291_10,(11,0,3):C.UVGC_295_15,(11,0,4):C.UVGC_295_16,(10,0,3):C.UVGC_295_15,(10,0,4):C.UVGC_295_16,(9,0,3):C.UVGC_294_13,(9,0,4):C.UVGC_294_14,(2,1,3):C.UVGC_292_12,(2,1,4):C.UVGC_292_11,(0,1,3):C.UVGC_292_12,(0,1,4):C.UVGC_292_11,(6,1,0):C.UVGC_326_62,(6,1,3):C.UVGC_326_63,(6,1,4):C.UVGC_326_64,(6,1,5):C.UVGC_326_65,(4,1,3):C.UVGC_291_9,(4,1,4):C.UVGC_291_10,(3,1,3):C.UVGC_291_9,(3,1,4):C.UVGC_291_10,(8,1,0):C.UVGC_331_80,(8,1,2):C.UVGC_331_81,(8,1,3):C.UVGC_331_82,(8,1,4):C.UVGC_331_83,(8,1,5):C.UVGC_331_84,(7,1,1):C.UVGC_297_22,(7,1,3):C.UVGC_298_24,(7,1,4):C.UVGC_298_25,(5,1,3):C.UVGC_291_9,(5,1,4):C.UVGC_291_10,(1,1,3):C.UVGC_291_9,(1,1,4):C.UVGC_291_10,(11,1,3):C.UVGC_295_15,(11,1,4):C.UVGC_295_16,(10,1,3):C.UVGC_295_15,(10,1,4):C.UVGC_295_16,(9,1,3):C.UVGC_294_13,(9,1,4):C.UVGC_294_14,(2,2,3):C.UVGC_292_12,(2,2,4):C.UVGC_292_11,(0,2,3):C.UVGC_292_12,(0,2,4):C.UVGC_292_11,(4,2,3):C.UVGC_291_9,(4,2,4):C.UVGC_291_10,(3,2,3):C.UVGC_291_9,(3,2,4):C.UVGC_291_10,(8,2,0):C.UVGC_328_68,(8,2,2):C.UVGC_328_69,(8,2,3):C.UVGC_328_70,(8,2,4):C.UVGC_328_71,(8,2,5):C.UVGC_328_72,(6,2,1):C.UVGC_297_22,(6,2,3):C.UVGC_297_23,(6,2,4):C.UVGC_294_13,(7,2,0):C.UVGC_326_62,(7,2,3):C.UVGC_327_66,(7,2,4):C.UVGC_327_67,(7,2,5):C.UVGC_326_65,(5,2,3):C.UVGC_291_9,(5,2,4):C.UVGC_291_10,(1,2,3):C.UVGC_291_9,(1,2,4):C.UVGC_291_10,(11,2,3):C.UVGC_295_15,(11,2,4):C.UVGC_295_16,(10,2,3):C.UVGC_295_15,(10,2,4):C.UVGC_295_16,(9,2,3):C.UVGC_294_13,(9,2,4):C.UVGC_294_14})

V_71 = CTVertex(name = 'V_71',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_352_118,(0,0,2):C.UVGC_352_119,(0,0,1):C.UVGC_352_120,(0,1,0):C.UVGC_336_91,(0,1,2):C.UVGC_336_92,(0,1,1):C.UVGC_336_93})

V_72 = CTVertex(name = 'V_72',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_353_121,(0,0,2):C.UVGC_353_122,(0,0,1):C.UVGC_353_123,(0,1,0):C.UVGC_335_88,(0,1,2):C.UVGC_335_89,(0,1,1):C.UVGC_335_90})

V_73 = CTVertex(name = 'V_73',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_343_101,(0,0,2):C.UVGC_343_102,(0,0,1):C.UVGC_343_103,(0,1,0):C.UVGC_349_113,(0,1,2):C.UVGC_349_114,(0,1,1):C.UVGC_349_115})

V_74 = CTVertex(name = 'V_74',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1, L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_311_36,(0,1,0):C.UVGC_313_38,(0,2,0):C.UVGC_312_37})

V_75 = CTVertex(name = 'V_75',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1, L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_347_109,(0,1,0):C.UVGC_345_107,(0,2,0):C.UVGC_346_108})

V_76 = CTVertex(name = 'V_76',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_348_110,(0,0,2):C.UVGC_348_111,(0,0,1):C.UVGC_348_112,(0,1,0):C.UVGC_344_104,(0,1,2):C.UVGC_344_105,(0,1,1):C.UVGC_344_106})

V_77 = CTVertex(name = 'V_77',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2, L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,1,0):C.UVGC_316_41,(0,2,0):C.UVGC_315_40,(0,0,0):C.UVGC_314_39})

V_78 = CTVertex(name = 'V_78',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2, L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,1,0):C.UVGC_354_124,(0,2,0):C.UVGC_351_117,(0,0,0):C.UVGC_350_116})

V_79 = CTVertex(name = 'V_79',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_317_42,(0,1,0):C.UVGC_357_127})

V_80 = CTVertex(name = 'V_80',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_355_125,(0,1,0):C.UVGC_358_128})

V_81 = CTVertex(name = 'V_81',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_356_126,(0,1,0):C.UVGC_342_100})

V_82 = CTVertex(name = 'V_82',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_318_43,(0,1,0):C.UVGC_310_35})

V_83 = CTVertex(name = 'V_83',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,1,0):C.UVGC_334_87,(0,0,0):C.UVGC_341_99})

V_84 = CTVertex(name = 'V_84',
                type = 'UV',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV4, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.u] ], [ [P.t] ] ],
                couplings = {(0,0,4):C.UVGC_281_2,(0,1,0):C.UVGC_296_17,(0,1,1):C.UVGC_296_18,(0,1,2):C.UVGC_296_19,(0,1,3):C.UVGC_296_20,(0,1,5):C.UVGC_296_21})

V_85 = CTVertex(name = 'V_85',
                type = 'UV',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV4, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.c, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(0,0,2):C.UVGC_281_2,(0,1,0):C.UVGC_296_17,(0,1,1):C.UVGC_296_18,(0,1,3):C.UVGC_296_19,(0,1,4):C.UVGC_296_20,(0,1,5):C.UVGC_296_21})

V_86 = CTVertex(name = 'V_86',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.t] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_296_17,(0,0,1):C.UVGC_296_18,(0,0,2):C.UVGC_296_19,(0,0,3):C.UVGC_296_20,(0,0,5):C.UVGC_296_21,(0,0,4):C.UVGC_281_2,(0,1,4):C.UVGC_333_86})

V_87 = CTVertex(name = 'V_87',
                type = 'UV',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_301_26,(0,0,1):C.UVGC_301_27})

V_88 = CTVertex(name = 'V_88',
                type = 'UV',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_301_26,(0,0,1):C.UVGC_301_27})

V_89 = CTVertex(name = 'V_89',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_337_94,(0,0,2):C.UVGC_337_95,(0,0,1):C.UVGC_301_27})

V_90 = CTVertex(name = 'V_90',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV7 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_338_96,(0,1,0):C.UVGC_340_98})

V_91 = CTVertex(name = 'V_91',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,1,0):C.UVGC_305_30,(0,0,0):C.UVGC_309_34})

V_92 = CTVertex(name = 'V_92',
                type = 'UV',
                particles = [ P.d__tilde__, P.d, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV4, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.d, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(0,0,2):C.UVGC_281_2,(0,1,0):C.UVGC_296_17,(0,1,1):C.UVGC_296_18,(0,1,3):C.UVGC_296_19,(0,1,4):C.UVGC_296_20,(0,1,5):C.UVGC_296_21})

V_93 = CTVertex(name = 'V_93',
                type = 'UV',
                particles = [ P.s__tilde__, P.s, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV4, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.s] ], [ [P.t] ] ],
                couplings = {(0,0,4):C.UVGC_281_2,(0,1,0):C.UVGC_296_17,(0,1,1):C.UVGC_296_18,(0,1,2):C.UVGC_296_19,(0,1,3):C.UVGC_296_20,(0,1,5):C.UVGC_296_21})

V_94 = CTVertex(name = 'V_94',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1, L.FFV5 ],
                loop_particles = [ [ [P.b] ], [ [P.b, P.g] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(0,0,1):C.UVGC_281_2,(0,1,0):C.UVGC_296_17,(0,1,2):C.UVGC_296_18,(0,1,3):C.UVGC_296_19,(0,1,4):C.UVGC_296_20,(0,1,5):C.UVGC_296_21,(0,1,1):C.UVGC_304_29})

V_95 = CTVertex(name = 'V_95',
                type = 'UV',
                particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_301_26,(0,0,1):C.UVGC_301_27})

V_96 = CTVertex(name = 'V_96',
                type = 'UV',
                particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_301_26,(0,0,1):C.UVGC_301_27})

V_97 = CTVertex(name = 'V_97',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_337_94,(0,0,2):C.UVGC_337_95,(0,0,1):C.UVGC_301_27})

V_98 = CTVertex(name = 'V_98',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_306_31,(0,1,0):C.UVGC_308_33})

V_99 = CTVertex(name = 'V_99',
                type = 'UV',
                particles = [ P.b__tilde__, P.b ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_307_32,(0,1,0):C.UVGC_303_28})

V_100 = CTVertex(name = 'V_100',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF4 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_280_1})

V_101 = CTVertex(name = 'V_101',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF4 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_280_1})

V_102 = CTVertex(name = 'V_102',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF4 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_280_1})

V_103 = CTVertex(name = 'V_103',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF2, L.FF3 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_339_97,(0,1,0):C.UVGC_332_85})

V_104 = CTVertex(name = 'V_104',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF4 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_280_1})

V_105 = CTVertex(name = 'V_105',
                 type = 'UV',
                 particles = [ P.g, P.g ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.VV1, L.VV5 ],
                 loop_particles = [ [ [P.b] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                 couplings = {(0,1,0):C.UVGC_319_44,(0,1,3):C.UVGC_319_45,(0,0,1):C.UVGC_288_3,(0,0,2):C.UVGC_288_4})

