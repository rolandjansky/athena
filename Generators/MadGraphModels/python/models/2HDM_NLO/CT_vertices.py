# This file was automatically created by FeynRules 2.1.91
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Tue 14 Apr 2015 09:34:45


from object_library import all_vertices, all_CTvertices, Vertex, CTVertex
import particles as P
import CT_couplings as C
import lorentz as L


V_1 = CTVertex(name = 'V_1',
               type = 'R2',
               particles = [ P.g, P.g, P.g ],
               color = [ 'f(1,2,3)' ],
               lorentz = [ L.VVV5 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(0,0,0):C.R2GC_379_137,(0,0,1):C.R2GC_379_138})

V_2 = CTVertex(name = 'V_2',
               type = 'R2',
               particles = [ P.g, P.g, P.g, P.g ],
               color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
               lorentz = [ L.VVVV10, L.VVVV2, L.VVVV3, L.VVVV5 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(2,1,0):C.R2GC_312_83,(2,1,1):C.R2GC_312_84,(0,1,0):C.R2GC_312_83,(0,1,1):C.R2GC_312_84,(4,1,0):C.R2GC_310_79,(4,1,1):C.R2GC_310_80,(3,1,0):C.R2GC_310_79,(3,1,1):C.R2GC_310_80,(8,1,0):C.R2GC_311_81,(8,1,1):C.R2GC_311_82,(7,1,0):C.R2GC_316_90,(7,1,1):C.R2GC_383_143,(6,1,0):C.R2GC_315_88,(6,1,1):C.R2GC_384_144,(5,1,0):C.R2GC_310_79,(5,1,1):C.R2GC_310_80,(1,1,0):C.R2GC_310_79,(1,1,1):C.R2GC_310_80,(11,0,0):C.R2GC_314_86,(11,0,1):C.R2GC_314_87,(10,0,0):C.R2GC_314_86,(10,0,1):C.R2GC_314_87,(9,0,1):C.R2GC_313_85,(2,2,0):C.R2GC_312_83,(2,2,1):C.R2GC_312_84,(0,2,0):C.R2GC_312_83,(0,2,1):C.R2GC_312_84,(6,2,0):C.R2GC_380_139,(6,2,1):C.R2GC_380_140,(4,2,0):C.R2GC_310_79,(4,2,1):C.R2GC_310_80,(3,2,0):C.R2GC_310_79,(3,2,1):C.R2GC_310_80,(8,2,0):C.R2GC_311_81,(8,2,1):C.R2GC_385_145,(7,2,0):C.R2GC_316_90,(7,2,1):C.R2GC_316_91,(5,2,0):C.R2GC_310_79,(5,2,1):C.R2GC_310_80,(1,2,0):C.R2GC_310_79,(1,2,1):C.R2GC_310_80,(2,3,0):C.R2GC_312_83,(2,3,1):C.R2GC_312_84,(0,3,0):C.R2GC_312_83,(0,3,1):C.R2GC_312_84,(4,3,0):C.R2GC_310_79,(4,3,1):C.R2GC_310_80,(3,3,0):C.R2GC_310_79,(3,3,1):C.R2GC_310_80,(8,3,0):C.R2GC_311_81,(8,3,1):C.R2GC_382_142,(6,3,0):C.R2GC_315_88,(6,3,1):C.R2GC_315_89,(7,3,0):C.R2GC_381_141,(7,3,1):C.R2GC_312_84,(5,3,0):C.R2GC_310_79,(5,3,1):C.R2GC_310_80,(1,3,0):C.R2GC_310_79,(1,3,1):C.R2GC_310_80})

V_3 = CTVertex(name = 'V_3',
               type = 'R2',
               particles = [ P.u__tilde__, P.d, P.h__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.d, P.g, P.u] ] ],
               couplings = {(0,0,0):C.R2GC_359_124,(0,1,0):C.R2GC_357_123})

V_4 = CTVertex(name = 'V_4',
               type = 'R2',
               particles = [ P.c__tilde__, P.s, P.h__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.c, P.g, P.s] ] ],
               couplings = {(0,0,0):C.R2GC_354_120,(0,1,0):C.R2GC_352_118})

V_5 = CTVertex(name = 'V_5',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.h__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_404_155,(0,1,0):C.R2GC_390_147})

V_6 = CTVertex(name = 'V_6',
               type = 'R2',
               particles = [ P.d__tilde__, P.u, P.h__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.d, P.g, P.u] ] ],
               couplings = {(0,0,0):C.R2GC_360_125,(0,1,0):C.R2GC_356_122})

V_7 = CTVertex(name = 'V_7',
               type = 'R2',
               particles = [ P.s__tilde__, P.c, P.h__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.c, P.g, P.s] ] ],
               couplings = {(0,0,0):C.R2GC_355_121,(0,1,0):C.R2GC_351_117})

V_8 = CTVertex(name = 'V_8',
               type = 'R2',
               particles = [ P.b__tilde__, P.t, P.h__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_405_156,(0,1,0):C.R2GC_389_146})

V_9 = CTVertex(name = 'V_9',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.G__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_397_150,(0,1,0):C.R2GC_398_151})

V_10 = CTVertex(name = 'V_10',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_369_128})

V_11 = CTVertex(name = 'V_11',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_399_152})

V_12 = CTVertex(name = 'V_12',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_398_151,(0,1,0):C.R2GC_397_150})

V_13 = CTVertex(name = 'V_13',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_348_114,(0,1,0):C.R2GC_345_111})

V_14 = CTVertex(name = 'V_14',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_349_115,(0,1,0):C.R2GC_346_112})

V_15 = CTVertex(name = 'V_15',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_350_116,(0,1,0):C.R2GC_347_113})

V_16 = CTVertex(name = 'V_16',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_323_96,(0,1,0):C.R2GC_320_93})

V_17 = CTVertex(name = 'V_17',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_324_97,(0,1,0):C.R2GC_321_94})

V_18 = CTVertex(name = 'V_18',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_325_98,(0,1,0):C.R2GC_322_95})

V_19 = CTVertex(name = 'V_19',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_406_157,(0,1,0):C.R2GC_401_153})

V_20 = CTVertex(name = 'V_20',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_407_158,(0,1,0):C.R2GC_403_154})

V_21 = CTVertex(name = 'V_21',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_408_159,(0,1,0):C.R2GC_396_149})

V_22 = CTVertex(name = 'V_22',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_331_102,(0,1,0):C.R2GC_328_99})

V_23 = CTVertex(name = 'V_23',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_332_103,(0,1,0):C.R2GC_329_100})

V_24 = CTVertex(name = 'V_24',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_333_104,(0,1,0):C.R2GC_330_101})

V_25 = CTVertex(name = 'V_25',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_339_108,(0,1,0):C.R2GC_336_105})

V_26 = CTVertex(name = 'V_26',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_340_109,(0,1,0):C.R2GC_337_106})

V_27 = CTVertex(name = 'V_27',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_341_110,(0,1,0):C.R2GC_338_107})

V_28 = CTVertex(name = 'V_28',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_374_131,(0,1,0):C.R2GC_371_129})

V_29 = CTVertex(name = 'V_29',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_375_132,(0,1,0):C.R2GC_373_130})

V_30 = CTVertex(name = 'V_30',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_376_133,(0,1,0):C.R2GC_368_127})

V_31 = CTVertex(name = 'V_31',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_298_69,(0,1,0):C.R2GC_261_2})

V_32 = CTVertex(name = 'V_32',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_298_69,(0,1,0):C.R2GC_261_2})

V_33 = CTVertex(name = 'V_33',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_298_69,(0,1,0):C.R2GC_261_2})

V_34 = CTVertex(name = 'V_34',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_35 = CTVertex(name = 'V_35',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_36 = CTVertex(name = 'V_36',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_37 = CTVertex(name = 'V_37',
                type = 'R2',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_38 = CTVertex(name = 'V_38',
                type = 'R2',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_39 = CTVertex(name = 'V_39',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_40 = CTVertex(name = 'V_40',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_297_68,(0,1,0):C.R2GC_262_3})

V_41 = CTVertex(name = 'V_41',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_297_68,(0,1,0):C.R2GC_262_3})

V_42 = CTVertex(name = 'V_42',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_297_68,(0,1,0):C.R2GC_262_3})

V_43 = CTVertex(name = 'V_43',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_300_71,(0,1,0):C.R2GC_264_4})

V_44 = CTVertex(name = 'V_44',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_300_71,(0,1,0):C.R2GC_264_4})

V_45 = CTVertex(name = 'V_45',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_300_71,(0,1,0):C.R2GC_264_4})

V_46 = CTVertex(name = 'V_46',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_47 = CTVertex(name = 'V_47',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_48 = CTVertex(name = 'V_48',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_317_92})

V_49 = CTVertex(name = 'V_49',
                type = 'R2',
                particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_50 = CTVertex(name = 'V_50',
                type = 'R2',
                particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_51 = CTVertex(name = 'V_51',
                type = 'R2',
                particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_353_119})

V_52 = CTVertex(name = 'V_52',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_299_70,(0,1,0):C.R2GC_265_5})

V_53 = CTVertex(name = 'V_53',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_299_70,(0,1,0):C.R2GC_265_5})

V_54 = CTVertex(name = 'V_54',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_299_70,(0,1,0):C.R2GC_265_5})

V_55 = CTVertex(name = 'V_55',
                type = 'R2',
                particles = [ P.u__tilde__, P.u ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF4 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_260_1,(0,1,0):C.R2GC_260_1})

V_56 = CTVertex(name = 'V_56',
                type = 'R2',
                particles = [ P.c__tilde__, P.c ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF4 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_260_1,(0,1,0):C.R2GC_260_1})

V_57 = CTVertex(name = 'V_57',
                type = 'R2',
                particles = [ P.t__tilde__, P.t ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1, L.FF2, L.FF3, L.FF4 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_392_148,(0,2,0):C.R2GC_392_148,(0,1,0):C.R2GC_260_1,(0,3,0):C.R2GC_260_1})

V_58 = CTVertex(name = 'V_58',
                type = 'R2',
                particles = [ P.d__tilde__, P.d ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF4 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_260_1,(0,1,0):C.R2GC_260_1})

V_59 = CTVertex(name = 'V_59',
                type = 'R2',
                particles = [ P.s__tilde__, P.s ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF4 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_260_1,(0,1,0):C.R2GC_260_1})

V_60 = CTVertex(name = 'V_60',
                type = 'R2',
                particles = [ P.b__tilde__, P.b ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1, L.FF2, L.FF3, L.FF4 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_364_126,(0,2,0):C.R2GC_364_126,(0,1,0):C.R2GC_260_1,(0,3,0):C.R2GC_260_1})

V_61 = CTVertex(name = 'V_61',
                type = 'R2',
                particles = [ P.g, P.g ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VV1, L.VV2, L.VV3 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ], [ [P.t] ] ],
                couplings = {(0,0,2):C.R2GC_378_136,(0,1,0):C.R2GC_272_6,(0,1,3):C.R2GC_272_7,(0,2,1):C.R2GC_377_134,(0,2,2):C.R2GC_377_135})

V_62 = CTVertex(name = 'V_62',
                type = 'R2',
                particles = [ P.g, P.g, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVV1, L.VVV2 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_282_26,(0,0,1):C.R2GC_282_27,(0,1,0):C.R2GC_282_27,(0,1,1):C.R2GC_282_26})

V_63 = CTVertex(name = 'V_63',
                type = 'R2',
                particles = [ P.a, P.g, P.g ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.VVV3, L.VVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_286_32,(0,0,1):C.R2GC_286_33,(0,1,0):C.R2GC_286_33,(0,1,1):C.R2GC_286_32})

V_64 = CTVertex(name = 'V_64',
                type = 'R2',
                particles = [ P.g, P.g, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_274_10,(0,0,1):C.R2GC_274_11})

V_65 = CTVertex(name = 'V_65',
                type = 'R2',
                particles = [ P.g, P.g, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_275_12,(0,0,1):C.R2GC_275_13})

V_66 = CTVertex(name = 'V_66',
                type = 'R2',
                particles = [ P.g, P.g, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_276_14,(0,0,1):C.R2GC_276_15})

V_67 = CTVertex(name = 'V_67',
                type = 'R2',
                particles = [ P.g, P.g, P.W__minus__, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b, P.t], [P.c, P.s], [P.d, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_306_75,(0,1,0):C.R2GC_306_75,(0,2,0):C.R2GC_306_75})

V_68 = CTVertex(name = 'V_68',
                type = 'R2',
                particles = [ P.g, P.g, P.Z, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_290_38,(0,0,1):C.R2GC_290_39,(0,1,0):C.R2GC_290_38,(0,1,1):C.R2GC_290_39,(0,2,0):C.R2GC_290_38,(0,2,1):C.R2GC_290_39})

V_69 = CTVertex(name = 'V_69',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.Z ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_291_40,(0,0,1):C.R2GC_291_41,(0,1,0):C.R2GC_291_40,(0,1,1):C.R2GC_291_41,(0,2,0):C.R2GC_291_40,(0,2,1):C.R2GC_291_41})

V_70 = CTVertex(name = 'V_70',
                type = 'R2',
                particles = [ P.a, P.a, P.g, P.g ],
                color = [ 'Identity(3,4)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_292_42,(0,0,1):C.R2GC_292_43,(0,1,0):C.R2GC_292_42,(0,1,1):C.R2GC_292_43,(0,2,0):C.R2GC_292_42,(0,2,1):C.R2GC_292_43})

V_71 = CTVertex(name = 'V_71',
                type = 'R2',
                particles = [ P.g, P.g, P.g, P.Z ],
                color = [ 'd(1,2,3)', 'f(1,2,3)' ],
                lorentz = [ L.VVVV1, L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_285_30,(1,0,1):C.R2GC_285_31,(0,1,0):C.R2GC_284_28,(0,1,1):C.R2GC_284_29,(0,2,0):C.R2GC_284_28,(0,2,1):C.R2GC_284_29,(0,3,0):C.R2GC_284_28,(0,3,1):C.R2GC_284_29})

V_72 = CTVertex(name = 'V_72',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.g ],
                color = [ 'd(2,3,4)', 'f(2,3,4)' ],
                lorentz = [ L.VVVV1, L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_289_36,(1,0,1):C.R2GC_289_37,(0,1,0):C.R2GC_288_34,(0,1,1):C.R2GC_288_35,(0,2,0):C.R2GC_288_34,(0,2,1):C.R2GC_288_35,(0,3,0):C.R2GC_288_34,(0,3,1):C.R2GC_288_35})

V_73 = CTVertex(name = 'V_73',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_273_8,(0,0,1):C.R2GC_273_9})

V_74 = CTVertex(name = 'V_74',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_303_72})

V_75 = CTVertex(name = 'V_75',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_304_73})

V_76 = CTVertex(name = 'V_76',
                type = 'R2',
                particles = [ P.g, P.g, P.G__plus__, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_305_74})

V_77 = CTVertex(name = 'V_77',
                type = 'R2',
                particles = [ P.g, P.g, P.h__minus__, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ], [ [P.c, P.s] ], [ [P.d, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_307_76,(0,0,1):C.R2GC_307_77,(0,0,2):C.R2GC_307_78})

V_78 = CTVertex(name = 'V_78',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_277_16,(0,0,1):C.R2GC_277_17})

V_79 = CTVertex(name = 'V_79',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c] ], [ [P.d] ], [ [P.s] ], [ [P.t] ], [ [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_293_44,(0,0,1):C.R2GC_293_45,(0,0,2):C.R2GC_293_46,(0,0,3):C.R2GC_293_47,(0,0,4):C.R2GC_293_48,(0,0,5):C.R2GC_293_49})

V_80 = CTVertex(name = 'V_80',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_278_18,(0,0,1):C.R2GC_278_19})

V_81 = CTVertex(name = 'V_81',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c] ], [ [P.d] ], [ [P.s] ], [ [P.t] ], [ [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_294_50,(0,0,1):C.R2GC_294_51,(0,0,2):C.R2GC_294_52,(0,0,3):C.R2GC_294_53,(0,0,4):C.R2GC_294_54,(0,0,5):C.R2GC_294_55})

V_82 = CTVertex(name = 'V_82',
                type = 'R2',
                particles = [ P.g, P.g, P.h2, P.h2 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c] ], [ [P.d] ], [ [P.s] ], [ [P.t] ], [ [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_295_56,(0,0,1):C.R2GC_295_57,(0,0,2):C.R2GC_295_58,(0,0,3):C.R2GC_295_59,(0,0,4):C.R2GC_295_60,(0,0,5):C.R2GC_295_61})

V_83 = CTVertex(name = 'V_83',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_279_20,(0,0,1):C.R2GC_279_21})

V_84 = CTVertex(name = 'V_84',
                type = 'R2',
                particles = [ P.g, P.g, P.h1, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_280_22,(0,0,1):C.R2GC_280_23})

V_85 = CTVertex(name = 'V_85',
                type = 'R2',
                particles = [ P.g, P.g, P.h2, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_281_24,(0,0,1):C.R2GC_281_25})

V_86 = CTVertex(name = 'V_86',
                type = 'R2',
                particles = [ P.g, P.g, P.h3, P.h3 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c] ], [ [P.d] ], [ [P.s] ], [ [P.t] ], [ [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_296_62,(0,0,1):C.R2GC_296_63,(0,0,2):C.R2GC_296_64,(0,0,3):C.R2GC_296_65,(0,0,4):C.R2GC_296_66,(0,0,5):C.R2GC_296_67})

V_87 = CTVertex(name = 'V_87',
                type = 'UV',
                particles = [ P.g, P.g, P.g ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VVV5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_379_84,(0,0,1):C.UVGC_379_85,(0,0,2):C.UVGC_379_86,(0,0,3):C.UVGC_379_87,(0,0,4):C.UVGC_379_88})

V_88 = CTVertex(name = 'V_88',
                type = 'UV',
                particles = [ P.g, P.g, P.g, P.g ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVVV10, L.VVVV2, L.VVVV3, L.VVVV5 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                couplings = {(2,1,3):C.UVGC_311_4,(2,1,4):C.UVGC_311_3,(0,1,3):C.UVGC_311_4,(0,1,4):C.UVGC_311_3,(4,1,3):C.UVGC_310_1,(4,1,4):C.UVGC_310_2,(3,1,3):C.UVGC_310_1,(3,1,4):C.UVGC_310_2,(8,1,3):C.UVGC_311_3,(8,1,4):C.UVGC_311_4,(7,1,0):C.UVGC_383_100,(7,1,2):C.UVGC_383_101,(7,1,3):C.UVGC_383_102,(7,1,4):C.UVGC_383_103,(7,1,5):C.UVGC_383_104,(6,1,0):C.UVGC_383_100,(6,1,2):C.UVGC_383_101,(6,1,3):C.UVGC_384_105,(6,1,4):C.UVGC_384_106,(6,1,5):C.UVGC_383_104,(5,1,3):C.UVGC_310_1,(5,1,4):C.UVGC_310_2,(1,1,3):C.UVGC_310_1,(1,1,4):C.UVGC_310_2,(11,0,3):C.UVGC_314_7,(11,0,4):C.UVGC_314_8,(10,0,3):C.UVGC_314_7,(10,0,4):C.UVGC_314_8,(9,0,3):C.UVGC_313_5,(9,0,4):C.UVGC_313_6,(2,2,3):C.UVGC_311_4,(2,2,4):C.UVGC_311_3,(0,2,3):C.UVGC_311_4,(0,2,4):C.UVGC_311_3,(6,2,0):C.UVGC_380_89,(6,2,3):C.UVGC_380_90,(6,2,4):C.UVGC_380_91,(6,2,5):C.UVGC_380_92,(4,2,3):C.UVGC_310_1,(4,2,4):C.UVGC_310_2,(3,2,3):C.UVGC_310_1,(3,2,4):C.UVGC_310_2,(8,2,0):C.UVGC_385_107,(8,2,2):C.UVGC_385_108,(8,2,3):C.UVGC_385_109,(8,2,4):C.UVGC_385_110,(8,2,5):C.UVGC_385_111,(7,2,1):C.UVGC_315_9,(7,2,3):C.UVGC_316_11,(7,2,4):C.UVGC_316_12,(5,2,3):C.UVGC_310_1,(5,2,4):C.UVGC_310_2,(1,2,3):C.UVGC_310_1,(1,2,4):C.UVGC_310_2,(2,3,3):C.UVGC_311_4,(2,3,4):C.UVGC_311_3,(0,3,3):C.UVGC_311_4,(0,3,4):C.UVGC_311_3,(4,3,3):C.UVGC_310_1,(4,3,4):C.UVGC_310_2,(3,3,3):C.UVGC_310_1,(3,3,4):C.UVGC_310_2,(8,3,0):C.UVGC_382_95,(8,3,2):C.UVGC_382_96,(8,3,3):C.UVGC_382_97,(8,3,4):C.UVGC_382_98,(8,3,5):C.UVGC_382_99,(6,3,1):C.UVGC_315_9,(6,3,3):C.UVGC_315_10,(6,3,4):C.UVGC_313_5,(7,3,0):C.UVGC_380_89,(7,3,3):C.UVGC_381_93,(7,3,4):C.UVGC_381_94,(7,3,5):C.UVGC_380_92,(5,3,3):C.UVGC_310_1,(5,3,4):C.UVGC_310_2,(1,3,3):C.UVGC_310_1,(1,3,4):C.UVGC_310_2})

V_89 = CTVertex(name = 'V_89',
                type = 'UV',
                particles = [ P.u__tilde__, P.d, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_359_58,(0,0,1):C.UVGC_359_59,(0,1,0):C.UVGC_357_56,(0,1,1):C.UVGC_357_57})

V_90 = CTVertex(name = 'V_90',
                type = 'UV',
                particles = [ P.c__tilde__, P.s, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_354_50,(0,0,1):C.UVGC_354_51,(0,1,0):C.UVGC_352_46,(0,1,1):C.UVGC_352_47})

V_91 = CTVertex(name = 'V_91',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.h__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_404_139,(0,0,2):C.UVGC_404_140,(0,0,1):C.UVGC_404_141,(0,1,0):C.UVGC_390_118,(0,1,2):C.UVGC_390_119,(0,1,1):C.UVGC_390_120})

V_92 = CTVertex(name = 'V_92',
                type = 'UV',
                particles = [ P.d__tilde__, P.u, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_360_60,(0,0,1):C.UVGC_360_61,(0,1,0):C.UVGC_356_54,(0,1,1):C.UVGC_356_55})

V_93 = CTVertex(name = 'V_93',
                type = 'UV',
                particles = [ P.s__tilde__, P.c, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_355_52,(0,0,1):C.UVGC_355_53,(0,1,0):C.UVGC_351_44,(0,1,1):C.UVGC_351_45})

V_94 = CTVertex(name = 'V_94',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.h__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_405_142,(0,0,2):C.UVGC_405_143,(0,0,1):C.UVGC_405_144,(0,1,0):C.UVGC_389_115,(0,1,2):C.UVGC_389_116,(0,1,1):C.UVGC_389_117})

V_95 = CTVertex(name = 'V_95',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_397_128,(0,0,2):C.UVGC_397_129,(0,0,1):C.UVGC_397_130,(0,1,0):C.UVGC_398_131,(0,1,2):C.UVGC_398_132,(0,1,1):C.UVGC_398_133})

V_96 = CTVertex(name = 'V_96',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_369_70})

V_97 = CTVertex(name = 'V_97',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_399_134})

V_98 = CTVertex(name = 'V_98',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_398_131,(0,0,2):C.UVGC_398_132,(0,0,1):C.UVGC_398_133,(0,1,0):C.UVGC_397_128,(0,1,2):C.UVGC_397_129,(0,1,1):C.UVGC_397_130})

V_99 = CTVertex(name = 'V_99',
                type = 'UV',
                particles = [ P.u__tilde__, P.u, P.h1 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_348_41,(0,1,0):C.UVGC_345_38})

V_100 = CTVertex(name = 'V_100',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_349_42,(0,1,0):C.UVGC_346_39})

V_101 = CTVertex(name = 'V_101',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_350_43,(0,1,0):C.UVGC_347_40})

V_102 = CTVertex(name = 'V_102',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.h1 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_323_23,(0,1,0):C.UVGC_320_20})

V_103 = CTVertex(name = 'V_103',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_324_24,(0,1,0):C.UVGC_321_21})

V_104 = CTVertex(name = 'V_104',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_325_25,(0,1,0):C.UVGC_322_22})

V_105 = CTVertex(name = 'V_105',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.h1 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS2, L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,1,0):C.UVGC_406_145,(0,2,0):C.UVGC_401_136,(0,0,0):C.UVGC_400_135})

V_106 = CTVertex(name = 'V_106',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS2, L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,1,0):C.UVGC_407_146,(0,2,0):C.UVGC_403_138,(0,0,0):C.UVGC_402_137})

V_107 = CTVertex(name = 'V_107',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_408_147,(0,1,0):C.UVGC_396_127})

V_108 = CTVertex(name = 'V_108',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.h1 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_331_29,(0,1,0):C.UVGC_328_26})

V_109 = CTVertex(name = 'V_109',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_332_30,(0,1,0):C.UVGC_329_27})

V_110 = CTVertex(name = 'V_110',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_333_31,(0,1,0):C.UVGC_330_28})

V_111 = CTVertex(name = 'V_111',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.h1 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_339_35,(0,1,0):C.UVGC_336_32})

V_112 = CTVertex(name = 'V_112',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_340_36,(0,1,0):C.UVGC_337_33})

V_113 = CTVertex(name = 'V_113',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_341_37,(0,1,0):C.UVGC_338_34})

V_114 = CTVertex(name = 'V_114',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.h1 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS2, L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,1,0):C.UVGC_374_75,(0,2,0):C.UVGC_371_72,(0,0,0):C.UVGC_370_71})

V_115 = CTVertex(name = 'V_115',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.h2 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS2, L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,1,0):C.UVGC_375_76,(0,2,0):C.UVGC_373_74,(0,0,0):C.UVGC_372_73})

V_116 = CTVertex(name = 'V_116',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.h3 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_376_77,(0,1,0):C.UVGC_368_69})

V_117 = CTVertex(name = 'V_117',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_395_126,(0,1,0):C.UVGC_388_114})

V_118 = CTVertex(name = 'V_118',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.u] ], [ [P.t] ] ],
                 couplings = {(0,0,4):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,1):C.UVGC_318_15,(0,1,2):C.UVGC_318_16,(0,1,3):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,4):C.UVGC_318_19,(0,2,0):C.UVGC_318_14,(0,2,1):C.UVGC_318_15,(0,2,2):C.UVGC_318_16,(0,2,3):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,4):C.UVGC_318_19})

V_119 = CTVertex(name = 'V_119',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.c, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                 couplings = {(0,0,2):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,1):C.UVGC_318_15,(0,1,3):C.UVGC_318_16,(0,1,4):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,2):C.UVGC_318_19,(0,2,0):C.UVGC_318_14,(0,2,1):C.UVGC_318_15,(0,2,3):C.UVGC_318_16,(0,2,4):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,2):C.UVGC_318_19})

V_120 = CTVertex(name = 'V_120',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.t] ], [ [P.t] ] ],
                 couplings = {(0,0,4):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,1):C.UVGC_318_15,(0,1,2):C.UVGC_318_16,(0,1,3):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,4):C.UVGC_387_113,(0,2,0):C.UVGC_318_14,(0,2,1):C.UVGC_318_15,(0,2,2):C.UVGC_318_16,(0,2,3):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,4):C.UVGC_387_113})

V_121 = CTVertex(name = 'V_121',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_353_48,(0,0,1):C.UVGC_353_49})

V_122 = CTVertex(name = 'V_122',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_353_48,(0,0,1):C.UVGC_353_49})

V_123 = CTVertex(name = 'V_123',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_391_121,(0,0,2):C.UVGC_391_122,(0,0,1):C.UVGC_353_49})

V_124 = CTVertex(name = 'V_124',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.Z ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_393_124,(0,1,0):C.UVGC_394_125})

V_125 = CTVertex(name = 'V_125',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_367_68,(0,1,0):C.UVGC_363_64})

V_126 = CTVertex(name = 'V_126',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.d, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                 couplings = {(0,0,2):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,1):C.UVGC_318_15,(0,1,3):C.UVGC_318_16,(0,1,4):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,2):C.UVGC_318_19,(0,2,0):C.UVGC_318_14,(0,2,1):C.UVGC_318_15,(0,2,3):C.UVGC_318_16,(0,2,4):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,2):C.UVGC_318_19})

V_127 = CTVertex(name = 'V_127',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.s] ], [ [P.t] ] ],
                 couplings = {(0,0,4):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,1):C.UVGC_318_15,(0,1,2):C.UVGC_318_16,(0,1,3):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,4):C.UVGC_318_19,(0,2,0):C.UVGC_318_14,(0,2,1):C.UVGC_318_15,(0,2,2):C.UVGC_318_16,(0,2,3):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,4):C.UVGC_318_19})

V_128 = CTVertex(name = 'V_128',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.b, P.g] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                 couplings = {(0,0,1):C.UVGC_317_13,(0,1,0):C.UVGC_318_14,(0,1,2):C.UVGC_318_15,(0,1,3):C.UVGC_318_16,(0,1,4):C.UVGC_318_17,(0,1,5):C.UVGC_318_18,(0,1,1):C.UVGC_362_63,(0,2,0):C.UVGC_318_14,(0,2,2):C.UVGC_318_15,(0,2,3):C.UVGC_318_16,(0,2,4):C.UVGC_318_17,(0,2,5):C.UVGC_318_18,(0,2,1):C.UVGC_362_63})

V_129 = CTVertex(name = 'V_129',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_353_48,(0,0,1):C.UVGC_353_49})

V_130 = CTVertex(name = 'V_130',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_353_48,(0,0,1):C.UVGC_353_49})

V_131 = CTVertex(name = 'V_131',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_391_121,(0,0,2):C.UVGC_391_122,(0,0,1):C.UVGC_353_49})

V_132 = CTVertex(name = 'V_132',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.Z ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_365_66,(0,1,0):C.UVGC_366_67})

V_133 = CTVertex(name = 'V_133',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF2, L.FF3, L.FF4 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_392_123,(0,2,0):C.UVGC_392_123,(0,1,0):C.UVGC_386_112,(0,3,0):C.UVGC_386_112})

V_134 = CTVertex(name = 'V_134',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF2, L.FF3, L.FF4 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_364_65,(0,2,0):C.UVGC_364_65,(0,1,0):C.UVGC_361_62,(0,3,0):C.UVGC_361_62})

V_135 = CTVertex(name = 'V_135',
                 type = 'UV',
                 particles = [ P.g, P.g ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.VV1, L.VV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.g] ], [ [P.ghG] ], [ [P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_378_80,(0,0,1):C.UVGC_378_81,(0,0,2):C.UVGC_378_82,(0,0,3):C.UVGC_378_83,(0,1,0):C.UVGC_377_78,(0,1,3):C.UVGC_377_79})

