# This file was automatically created by FeynRules 2.1.78
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Fri 17 Oct 2014 08:59:38


from object_library import all_vertices, all_CTvertices, Vertex, CTVertex
import particles as P
import CT_couplings as C
import lorentz as L


V_1 = CTVertex(name = 'V_1',
               type = 'R2',
               particles = [ P.g, P.g, P.g ],
               color = [ 'f(1,2,3)' ],
               lorentz = [ L.VVV2, L.VVV3, L.VVV4, L.VVV5, L.VVV6, L.VVV7 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(0,0,0):C.R2GC_203_92,(0,0,1):C.R2GC_203_93,(0,1,0):C.R2GC_204_94,(0,1,1):C.R2GC_204_95,(0,2,0):C.R2GC_204_94,(0,2,1):C.R2GC_204_95,(0,3,0):C.R2GC_203_92,(0,3,1):C.R2GC_203_93,(0,4,0):C.R2GC_203_92,(0,4,1):C.R2GC_203_93,(0,5,0):C.R2GC_204_94,(0,5,1):C.R2GC_204_95})

V_2 = CTVertex(name = 'V_2',
               type = 'R2',
               particles = [ P.g, P.g, P.g, P.g ],
               color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
               lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(2,0,0):C.R2GC_145_45,(2,0,1):C.R2GC_145_46,(0,0,0):C.R2GC_145_45,(0,0,1):C.R2GC_145_46,(4,0,0):C.R2GC_143_41,(4,0,1):C.R2GC_143_42,(3,0,0):C.R2GC_143_41,(3,0,1):C.R2GC_143_42,(8,0,0):C.R2GC_144_43,(8,0,1):C.R2GC_144_44,(7,0,0):C.R2GC_149_51,(7,0,1):C.R2GC_208_100,(5,0,0):C.R2GC_143_41,(5,0,1):C.R2GC_143_42,(1,0,0):C.R2GC_143_41,(1,0,1):C.R2GC_143_42,(6,0,0):C.R2GC_148_50,(6,0,1):C.R2GC_209_101,(11,0,0):C.R2GC_147_48,(11,0,1):C.R2GC_147_49,(10,0,0):C.R2GC_147_48,(10,0,1):C.R2GC_147_49,(9,0,1):C.R2GC_146_47,(2,1,0):C.R2GC_145_45,(2,1,1):C.R2GC_145_46,(0,1,0):C.R2GC_145_45,(0,1,1):C.R2GC_145_46,(4,1,0):C.R2GC_143_41,(4,1,1):C.R2GC_143_42,(3,1,0):C.R2GC_143_41,(3,1,1):C.R2GC_143_42,(8,1,0):C.R2GC_144_43,(8,1,1):C.R2GC_208_100,(6,1,0):C.R2GC_206_98,(6,1,1):C.R2GC_206_99,(7,1,0):C.R2GC_149_51,(7,1,1):C.R2GC_144_44,(5,1,0):C.R2GC_143_41,(5,1,1):C.R2GC_143_42,(1,1,0):C.R2GC_143_41,(1,1,1):C.R2GC_143_42,(11,1,0):C.R2GC_147_48,(11,1,1):C.R2GC_147_49,(10,1,0):C.R2GC_147_48,(10,1,1):C.R2GC_147_49,(9,1,1):C.R2GC_146_47,(2,2,0):C.R2GC_145_45,(2,2,1):C.R2GC_145_46,(0,2,0):C.R2GC_145_45,(0,2,1):C.R2GC_145_46,(4,2,0):C.R2GC_143_41,(4,2,1):C.R2GC_143_42,(3,2,0):C.R2GC_143_41,(3,2,1):C.R2GC_143_42,(8,2,0):C.R2GC_144_43,(8,2,1):C.R2GC_205_97,(6,2,0):C.R2GC_148_50,(7,2,0):C.R2GC_205_96,(7,2,1):C.R2GC_205_97,(5,2,0):C.R2GC_143_41,(5,2,1):C.R2GC_143_42,(1,2,0):C.R2GC_143_41,(1,2,1):C.R2GC_143_42,(11,2,0):C.R2GC_147_48,(11,2,1):C.R2GC_147_49,(10,2,0):C.R2GC_147_48,(10,2,1):C.R2GC_147_49,(9,2,1):C.R2GC_146_47})

V_3 = CTVertex(name = 'V_3',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.G__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_224_112,(0,1,0):C.R2GC_226_114})

V_4 = CTVertex(name = 'V_4',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.G0 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS1 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_201_91})

V_5 = CTVertex(name = 'V_5',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.H ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS2 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_200_90})

V_6 = CTVertex(name = 'V_6',
               type = 'R2',
               particles = [ P.t__tilde__, P.t, P.G0 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS1 ],
               loop_particles = [ [ [P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_228_116})

V_7 = CTVertex(name = 'V_7',
               type = 'R2',
               particles = [ P.t__tilde__, P.t, P.H ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS2 ],
               loop_particles = [ [ [P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_229_117})

V_8 = CTVertex(name = 'V_8',
               type = 'R2',
               particles = [ P.b__tilde__, P.t, P.G__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS5 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_227_115,(0,1,0):C.R2GC_223_111})

V_9 = CTVertex(name = 'V_9',
               type = 'R2',
               particles = [ P.g, P.g, P.sig8 ],
               color = [ 'd(1,2,3)' ],
               lorentz = [ L.VVS1, L.VVS3, L.VVS6 ],
               loop_particles = [ [ [P.b] ], [ [P.g] ], [ [P.g, P.sig8] ], [ [P.t] ] ],
               couplings = {(0,2,1):C.R2GC_94_119,(0,1,2):C.R2GC_100_1,(0,0,0):C.R2GC_106_5,(0,0,3):C.R2GC_106_6})

V_10 = CTVertex(name = 'V_10',
                type = 'R2',
                particles = [ P.g, P.sig8, P.sig8 ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VSS1 ],
                loop_particles = [ [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_166_64})

V_11 = CTVertex(name = 'V_11',
                type = 'R2',
                particles = [ P.g, P.g, P.sig8, P.sig8 ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_149_51,(0,0,1):C.R2GC_161_58,(2,0,0):C.R2GC_149_51,(2,0,1):C.R2GC_161_58,(6,0,0):C.R2GC_149_51,(6,0,1):C.R2GC_167_65,(7,0,0):C.R2GC_149_51,(7,0,1):C.R2GC_167_65,(5,0,0):C.R2GC_160_56,(5,0,1):C.R2GC_160_57,(1,0,0):C.R2GC_160_56,(1,0,1):C.R2GC_160_57,(4,0,0):C.R2GC_160_56,(4,0,1):C.R2GC_160_57,(3,0,0):C.R2GC_160_56,(3,0,1):C.R2GC_160_57,(10,0,0):C.R2GC_163_60,(10,0,1):C.R2GC_163_61,(9,0,0):C.R2GC_163_60,(9,0,1):C.R2GC_163_61,(8,0,0):C.R2GC_148_50,(8,0,1):C.R2GC_162_59})

V_12 = CTVertex(name = 'V_12',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.d, P.g] ], [ [P.d, P.g, P.sig8] ] ],
                couplings = {(0,1,0):C.R2GC_122_33,(0,0,1):C.R2GC_180_68,(0,2,0):C.R2GC_101_2})

V_13 = CTVertex(name = 'V_13',
                type = 'R2',
                particles = [ P.s__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g, P.s] ], [ [P.d, P.g, P.sig8], [P.g, P.s, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_182_71,(0,0,1):C.R2GC_182_72,(0,1,0):C.R2GC_181_69,(0,1,1):C.R2GC_181_70})

V_14 = CTVertex(name = 'V_14',
                type = 'R2',
                particles = [ P.b__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.d, P.g] ], [ [P.b, P.g, P.sig8], [P.d, P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_194_84,(0,0,1):C.R2GC_194_85,(0,1,0):C.R2GC_192_80,(0,1,1):C.R2GC_192_81})

V_15 = CTVertex(name = 'V_15',
                type = 'R2',
                particles = [ P.d__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g, P.s] ], [ [P.d, P.g, P.sig8], [P.g, P.s, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_181_69,(0,0,1):C.R2GC_181_70,(0,1,0):C.R2GC_182_71,(0,1,1):C.R2GC_182_72})

V_16 = CTVertex(name = 'V_16',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.g, P.s] ], [ [P.g, P.s, P.sig8] ] ],
                couplings = {(0,1,0):C.R2GC_124_35,(0,0,1):C.R2GC_183_73,(0,2,0):C.R2GC_101_2})

V_17 = CTVertex(name = 'V_17',
                type = 'R2',
                particles = [ P.b__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g, P.s] ], [ [P.b, P.g, P.sig8], [P.g, P.s, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_195_86,(0,0,1):C.R2GC_195_87,(0,1,0):C.R2GC_193_82,(0,1,1):C.R2GC_193_83})

V_18 = CTVertex(name = 'V_18',
                type = 'R2',
                particles = [ P.d__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.d, P.g] ], [ [P.b, P.g, P.sig8], [P.d, P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_192_80,(0,0,1):C.R2GC_192_81,(0,1,0):C.R2GC_194_84,(0,1,1):C.R2GC_194_85})

V_19 = CTVertex(name = 'V_19',
                type = 'R2',
                particles = [ P.s__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g, P.s] ], [ [P.b, P.g, P.sig8], [P.g, P.s, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_193_82,(0,0,1):C.R2GC_193_83,(0,1,0):C.R2GC_195_86,(0,1,1):C.R2GC_195_87})

V_20 = CTVertex(name = 'V_20',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8] ] ],
                couplings = {(0,1,0):C.R2GC_119_30,(0,0,1):C.R2GC_196_88,(0,2,0):C.R2GC_101_2})

V_21 = CTVertex(name = 'V_21',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.g, P.sig8, P.u] ], [ [P.g, P.u] ] ],
                couplings = {(0,1,1):C.R2GC_129_39,(0,0,0):C.R2GC_185_74,(0,2,1):C.R2GC_101_2})

V_22 = CTVertex(name = 'V_22',
                type = 'R2',
                particles = [ P.c__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.u] ], [ [P.c, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_187_77,(0,0,1):C.R2GC_187_78,(0,1,0):C.R2GC_186_75,(0,1,1):C.R2GC_186_76})

V_23 = CTVertex(name = 'V_23',
                type = 'R2',
                particles = [ P.t__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.sig8, P.t], [P.g, P.sig8, P.u] ], [ [P.g, P.t, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_217_106,(0,0,1):C.R2GC_217_107,(0,1,0):C.R2GC_215_102,(0,1,1):C.R2GC_215_103})

V_24 = CTVertex(name = 'V_24',
                type = 'R2',
                particles = [ P.u__tilde__, P.c, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.u] ], [ [P.c, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_186_75,(0,0,1):C.R2GC_186_76,(0,1,0):C.R2GC_187_77,(0,1,1):C.R2GC_187_78})

V_25 = CTVertex(name = 'V_25',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.c, P.g] ], [ [P.c, P.g, P.sig8] ] ],
                couplings = {(0,1,0):C.R2GC_120_31,(0,0,1):C.R2GC_178_66,(0,2,0):C.R2GC_101_2})

V_26 = CTVertex(name = 'V_26',
                type = 'R2',
                particles = [ P.t__tilde__, P.c, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.t] ], [ [P.c, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_218_108,(0,0,1):C.R2GC_218_109,(0,1,0):C.R2GC_216_104,(0,1,1):C.R2GC_216_105})

V_27 = CTVertex(name = 'V_27',
                type = 'R2',
                particles = [ P.u__tilde__, P.t, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.sig8, P.t], [P.g, P.sig8, P.u] ], [ [P.g, P.t, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_215_102,(0,0,1):C.R2GC_215_103,(0,1,0):C.R2GC_217_106,(0,1,1):C.R2GC_217_107})

V_28 = CTVertex(name = 'V_28',
                type = 'R2',
                particles = [ P.c__tilde__, P.t, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.t] ], [ [P.c, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_216_104,(0,0,1):C.R2GC_216_105,(0,1,0):C.R2GC_218_108,(0,1,1):C.R2GC_218_109})

V_29 = CTVertex(name = 'V_29',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2, L.FFS6, L.FFS7 ],
                loop_particles = [ [ [P.g, P.sig8, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,1,1):C.R2GC_127_37,(0,0,0):C.R2GC_219_110,(0,2,1):C.R2GC_101_2})

V_30 = CTVertex(name = 'V_30',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_153_55})

V_31 = CTVertex(name = 'V_31',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_153_55})

V_32 = CTVertex(name = 'V_32',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_153_55})

V_33 = CTVertex(name = 'V_33',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_34 = CTVertex(name = 'V_34',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_35 = CTVertex(name = 'V_35',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_36 = CTVertex(name = 'V_36',
                type = 'R2',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_37 = CTVertex(name = 'V_37',
                type = 'R2',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_38 = CTVertex(name = 'V_38',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_39 = CTVertex(name = 'V_39',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_121_32,(0,1,0):C.R2GC_105_4})

V_40 = CTVertex(name = 'V_40',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_121_32,(0,1,0):C.R2GC_105_4})

V_41 = CTVertex(name = 'V_41',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_121_32,(0,1,0):C.R2GC_105_4})

V_42 = CTVertex(name = 'V_42',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_150_52})

V_43 = CTVertex(name = 'V_43',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_150_52})

V_44 = CTVertex(name = 'V_44',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_150_52})

V_45 = CTVertex(name = 'V_45',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_46 = CTVertex(name = 'V_46',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_47 = CTVertex(name = 'V_47',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_151_53})

V_48 = CTVertex(name = 'V_48',
                type = 'R2',
                particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_49 = CTVertex(name = 'V_49',
                type = 'R2',
                particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_50 = CTVertex(name = 'V_50',
                type = 'R2',
                particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_179_67})

V_51 = CTVertex(name = 'V_51',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_123_34,(0,1,0):C.R2GC_102_3})

V_52 = CTVertex(name = 'V_52',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_123_34,(0,1,0):C.R2GC_102_3})

V_53 = CTVertex(name = 'V_53',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_123_34,(0,1,0):C.R2GC_102_3})

V_54 = CTVertex(name = 'V_54',
                type = 'R2',
                particles = [ P.u__tilde__, P.u ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_152_54})

V_55 = CTVertex(name = 'V_55',
                type = 'R2',
                particles = [ P.c__tilde__, P.c ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_152_54})

V_56 = CTVertex(name = 'V_56',
                type = 'R2',
                particles = [ P.t__tilde__, P.t ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF2, L.FF3, L.FF4, L.FF6 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_225_113,(0,2,0):C.R2GC_225_113,(0,1,0):C.R2GC_152_54,(0,3,0):C.R2GC_152_54})

V_57 = CTVertex(name = 'V_57',
                type = 'R2',
                particles = [ P.d__tilde__, P.d ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_152_54})

V_58 = CTVertex(name = 'V_58',
                type = 'R2',
                particles = [ P.s__tilde__, P.s ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_152_54})

V_59 = CTVertex(name = 'V_59',
                type = 'R2',
                particles = [ P.b__tilde__, P.b ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF5, L.FF7 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_197_89,(0,1,0):C.R2GC_152_54})

V_60 = CTVertex(name = 'V_60',
                type = 'R2',
                particles = [ P.sig8, P.sig8 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.SS1, L.SS2 ],
                loop_particles = [ [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.R2GC_188_79,(0,1,0):C.R2GC_165_63})

V_61 = CTVertex(name = 'V_61',
                type = 'R2',
                particles = [ P.g, P.g ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VV2, L.VV3, L.VV4 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ], [ [P.t] ] ],
                couplings = {(0,2,2):C.R2GC_93_118,(0,0,0):C.R2GC_107_7,(0,0,3):C.R2GC_107_8,(0,1,1):C.R2GC_111_15})

V_62 = CTVertex(name = 'V_62',
                type = 'R2',
                particles = [ P.g, P.g, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVV1 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_114_20,(0,0,1):C.R2GC_114_21})

V_63 = CTVertex(name = 'V_63',
                type = 'R2',
                particles = [ P.g, P.g, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_109_11,(0,0,1):C.R2GC_109_12})

V_64 = CTVertex(name = 'V_64',
                type = 'R2',
                particles = [ P.sig8, P.sig8, P.sig8 ],
                color = [ 'd(1,2,3)' ],
                lorentz = [ L.SSS2, L.SSS3 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,1):C.R2GC_164_62,(0,1,0):C.R2GC_141_40})

V_65 = CTVertex(name = 'V_65',
                type = 'R2',
                particles = [ P.g, P.g, P.W__minus__, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b, P.t], [P.c, P.s], [P.d, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_128_38})

V_66 = CTVertex(name = 'V_66',
                type = 'R2',
                particles = [ P.a, P.a, P.g, P.g ],
                color = [ 'Identity(3,4)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_112_16,(0,0,1):C.R2GC_112_17})

V_67 = CTVertex(name = 'V_67',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.Z ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_115_22,(0,0,1):C.R2GC_115_23})

V_68 = CTVertex(name = 'V_68',
                type = 'R2',
                particles = [ P.g, P.g, P.Z, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_118_28,(0,0,1):C.R2GC_118_29})

V_69 = CTVertex(name = 'V_69',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.g ],
                color = [ 'd(2,3,4)' ],
                lorentz = [ L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_113_18,(0,0,1):C.R2GC_113_19})

V_70 = CTVertex(name = 'V_70',
                type = 'R2',
                particles = [ P.g, P.g, P.g, P.Z ],
                color = [ 'd(1,2,3)', 'f(1,2,3)' ],
                lorentz = [ L.VVVV1, L.VVVV7 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_117_26,(1,0,1):C.R2GC_117_27,(0,1,0):C.R2GC_116_24,(0,1,1):C.R2GC_116_25})

V_71 = CTVertex(name = 'V_71',
                type = 'R2',
                particles = [ P.g, P.g, P.H, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_110_13,(0,0,1):C.R2GC_110_14})

V_72 = CTVertex(name = 'V_72',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_110_13,(0,0,1):C.R2GC_110_14})

V_73 = CTVertex(name = 'V_73',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_126_36})

V_74 = CTVertex(name = 'V_74',
                type = 'R2',
                particles = [ P.g, P.g, P.H, P.sig8 ],
                color = [ 'd(1,2,4)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_108_9,(0,0,1):C.R2GC_108_10})

V_75 = CTVertex(name = 'V_75',
                type = 'UV',
                particles = [ P.g, P.g, P.g ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VVV2, L.VVV3, L.VVV4, L.VVV5, L.VVV6, L.VVV7 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_203_110,(0,0,1):C.UVGC_203_111,(0,0,2):C.UVGC_203_112,(0,0,3):C.UVGC_203_113,(0,0,4):C.UVGC_203_114,(0,0,5):C.UVGC_203_115,(0,1,0):C.UVGC_204_116,(0,1,1):C.UVGC_166_42,(0,1,2):C.UVGC_204_117,(0,1,3):C.UVGC_204_118,(0,1,4):C.UVGC_204_119,(0,1,5):C.UVGC_204_120,(0,2,0):C.UVGC_204_116,(0,2,1):C.UVGC_166_42,(0,2,2):C.UVGC_204_117,(0,2,3):C.UVGC_204_118,(0,2,4):C.UVGC_204_119,(0,2,5):C.UVGC_204_120,(0,3,0):C.UVGC_203_110,(0,3,1):C.UVGC_203_111,(0,3,2):C.UVGC_203_112,(0,3,3):C.UVGC_203_113,(0,3,4):C.UVGC_203_114,(0,3,5):C.UVGC_203_115,(0,4,0):C.UVGC_203_110,(0,4,1):C.UVGC_203_111,(0,4,2):C.UVGC_203_112,(0,4,3):C.UVGC_203_113,(0,4,4):C.UVGC_203_114,(0,4,5):C.UVGC_203_115,(0,5,0):C.UVGC_204_116,(0,5,1):C.UVGC_166_42,(0,5,2):C.UVGC_204_117,(0,5,3):C.UVGC_204_118,(0,5,4):C.UVGC_204_119,(0,5,5):C.UVGC_204_120})

V_76 = CTVertex(name = 'V_76',
                type = 'UV',
                particles = [ P.g, P.g, P.g, P.g ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ], [ [P.t] ] ],
                couplings = {(2,0,3):C.UVGC_144_12,(2,0,4):C.UVGC_144_11,(0,0,3):C.UVGC_144_12,(0,0,4):C.UVGC_144_11,(4,0,3):C.UVGC_143_9,(4,0,4):C.UVGC_143_10,(3,0,3):C.UVGC_143_9,(3,0,4):C.UVGC_143_10,(8,0,3):C.UVGC_144_11,(8,0,4):C.UVGC_144_12,(7,0,0):C.UVGC_208_134,(7,0,2):C.UVGC_167_48,(7,0,3):C.UVGC_208_135,(7,0,4):C.UVGC_208_136,(7,0,5):C.UVGC_208_137,(7,0,6):C.UVGC_208_138,(5,0,3):C.UVGC_143_9,(5,0,4):C.UVGC_143_10,(1,0,3):C.UVGC_143_9,(1,0,4):C.UVGC_143_10,(6,0,0):C.UVGC_208_134,(6,0,2):C.UVGC_167_48,(6,0,3):C.UVGC_209_139,(6,0,4):C.UVGC_209_140,(6,0,5):C.UVGC_208_137,(6,0,6):C.UVGC_208_138,(11,0,3):C.UVGC_147_15,(11,0,4):C.UVGC_147_16,(10,0,3):C.UVGC_147_15,(10,0,4):C.UVGC_147_16,(9,0,3):C.UVGC_146_13,(9,0,4):C.UVGC_146_14,(2,1,3):C.UVGC_144_12,(2,1,4):C.UVGC_144_11,(0,1,3):C.UVGC_144_12,(0,1,4):C.UVGC_144_11,(4,1,3):C.UVGC_143_9,(4,1,4):C.UVGC_143_10,(3,1,3):C.UVGC_143_9,(3,1,4):C.UVGC_143_10,(8,1,0):C.UVGC_210_141,(8,1,2):C.UVGC_210_142,(8,1,3):C.UVGC_208_135,(8,1,4):C.UVGC_210_143,(8,1,5):C.UVGC_210_144,(8,1,6):C.UVGC_210_145,(6,1,0):C.UVGC_205_121,(6,1,3):C.UVGC_206_126,(6,1,4):C.UVGC_206_127,(6,1,5):C.UVGC_206_128,(6,1,6):C.UVGC_205_125,(7,1,1):C.UVGC_148_17,(7,1,3):C.UVGC_144_11,(7,1,4):C.UVGC_149_19,(7,1,5):C.UVGC_149_20,(5,1,3):C.UVGC_143_9,(5,1,4):C.UVGC_143_10,(1,1,3):C.UVGC_143_9,(1,1,4):C.UVGC_143_10,(11,1,3):C.UVGC_147_15,(11,1,4):C.UVGC_147_16,(10,1,3):C.UVGC_147_15,(10,1,4):C.UVGC_147_16,(9,1,3):C.UVGC_146_13,(9,1,4):C.UVGC_146_14,(2,2,3):C.UVGC_144_12,(2,2,4):C.UVGC_144_11,(0,2,3):C.UVGC_144_12,(0,2,4):C.UVGC_144_11,(4,2,3):C.UVGC_143_9,(4,2,4):C.UVGC_143_10,(3,2,3):C.UVGC_143_9,(3,2,4):C.UVGC_143_10,(8,2,0):C.UVGC_207_129,(8,2,2):C.UVGC_207_130,(8,2,3):C.UVGC_205_122,(8,2,4):C.UVGC_207_131,(8,2,5):C.UVGC_207_132,(8,2,6):C.UVGC_207_133,(6,2,1):C.UVGC_148_17,(6,2,4):C.UVGC_146_13,(6,2,5):C.UVGC_148_18,(7,2,0):C.UVGC_205_121,(7,2,3):C.UVGC_205_122,(7,2,4):C.UVGC_205_123,(7,2,5):C.UVGC_205_124,(7,2,6):C.UVGC_205_125,(5,2,3):C.UVGC_143_9,(5,2,4):C.UVGC_143_10,(1,2,3):C.UVGC_143_9,(1,2,4):C.UVGC_143_10,(11,2,3):C.UVGC_147_15,(11,2,4):C.UVGC_147_16,(10,2,3):C.UVGC_147_15,(10,2,4):C.UVGC_147_16,(9,2,3):C.UVGC_146_13,(9,2,4):C.UVGC_146_14})

V_77 = CTVertex(name = 'V_77',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_224_181,(0,0,2):C.UVGC_224_182,(0,0,1):C.UVGC_224_183,(0,1,0):C.UVGC_226_185,(0,1,2):C.UVGC_226_186,(0,1,1):C.UVGC_226_187})

V_78 = CTVertex(name = 'V_78',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_201_106})

V_79 = CTVertex(name = 'V_79',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_200_105})

V_80 = CTVertex(name = 'V_80',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_228_191})

V_81 = CTVertex(name = 'V_81',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_229_192})

V_82 = CTVertex(name = 'V_82',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_227_188,(0,0,2):C.UVGC_227_189,(0,0,1):C.UVGC_227_190,(0,1,0):C.UVGC_223_178,(0,1,2):C.UVGC_223_179,(0,1,1):C.UVGC_223_180})

V_83 = CTVertex(name = 'V_83',
                type = 'UV',
                particles = [ P.g, P.g, P.sig8 ],
                color = [ 'd(1,2,3)' ],
                lorentz = [ L.VVS2, L.VVS4, L.VVS5 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.sig8] ], [ [P.sig8] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_211_146,(0,0,1):C.UVGC_211_147,(0,0,3):C.UVGC_211_148,(0,0,5):C.UVGC_211_149,(0,0,6):C.UVGC_211_150,(0,0,4):C.UVGC_211_151,(0,2,2):C.UVGC_131_1,(0,1,4):C.UVGC_136_5})

V_84 = CTVertex(name = 'V_84',
                type = 'UV',
                particles = [ P.g, P.sig8, P.sig8 ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.sig8] ], [ [P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_166_41,(0,0,1):C.UVGC_166_42,(0,0,2):C.UVGC_166_43,(0,0,3):C.UVGC_166_44,(0,0,5):C.UVGC_166_45,(0,0,4):C.UVGC_166_46})

V_85 = CTVertex(name = 'V_85',
                type = 'UV',
                particles = [ P.g, P.g, P.sig8, P.sig8 ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.sig8] ], [ [P.sig8] ] ],
                couplings = {(0,0,2):C.UVGC_161_33,(0,0,4):C.UVGC_161_34,(2,0,2):C.UVGC_161_33,(2,0,4):C.UVGC_161_34,(6,0,0):C.UVGC_167_47,(6,0,1):C.UVGC_167_48,(6,0,2):C.UVGC_167_49,(6,0,3):C.UVGC_167_50,(6,0,5):C.UVGC_167_51,(6,0,4):C.UVGC_167_52,(7,0,0):C.UVGC_167_47,(7,0,1):C.UVGC_167_48,(7,0,2):C.UVGC_167_49,(7,0,3):C.UVGC_167_50,(7,0,5):C.UVGC_167_51,(7,0,4):C.UVGC_167_52,(5,0,2):C.UVGC_160_31,(5,0,4):C.UVGC_160_32,(1,0,2):C.UVGC_160_31,(1,0,4):C.UVGC_160_32,(4,0,2):C.UVGC_160_31,(4,0,4):C.UVGC_160_32,(3,0,2):C.UVGC_160_31,(3,0,4):C.UVGC_160_32,(10,0,2):C.UVGC_163_37,(10,0,4):C.UVGC_163_38,(9,0,2):C.UVGC_163_37,(9,0,4):C.UVGC_163_38,(8,0,2):C.UVGC_162_35,(8,0,4):C.UVGC_162_36})

V_86 = CTVertex(name = 'V_86',
                type = 'UV',
                particles = [ P.d__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.d, P.g] ], [ [P.d, P.g, P.sig8] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_180_58,(0,0,2):C.UVGC_180_59,(0,0,1):C.UVGC_180_60})

V_87 = CTVertex(name = 'V_87',
                type = 'UV',
                particles = [ P.s__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.s], [P.d, P.g, P.s] ], [ [P.d, P.g, P.sig8], [P.g, P.s, P.sig8] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_182_64,(0,0,2):C.UVGC_182_65,(0,0,1):C.UVGC_182_66,(0,1,0):C.UVGC_181_61,(0,1,2):C.UVGC_181_62,(0,1,1):C.UVGC_181_63})

V_88 = CTVertex(name = 'V_88',
                type = 'UV',
                particles = [ P.b__tilde__, P.d, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8], [P.d, P.g, P.sig8] ], [ [P.d, P.g], [P.b, P.d, P.g] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_194_91,(0,0,2):C.UVGC_194_92,(0,0,3):C.UVGC_194_93,(0,0,1):C.UVGC_194_94,(0,1,0):C.UVGC_192_83,(0,1,2):C.UVGC_192_84,(0,1,3):C.UVGC_192_85,(0,1,1):C.UVGC_192_86})

V_89 = CTVertex(name = 'V_89',
                type = 'UV',
                particles = [ P.d__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.s], [P.d, P.g, P.s] ], [ [P.d, P.g, P.sig8], [P.g, P.s, P.sig8] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_181_61,(0,0,2):C.UVGC_181_62,(0,0,1):C.UVGC_181_63,(0,1,0):C.UVGC_182_64,(0,1,2):C.UVGC_182_65,(0,1,1):C.UVGC_182_66})

V_90 = CTVertex(name = 'V_90',
                type = 'UV',
                particles = [ P.s__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.g, P.s] ], [ [P.g, P.sig8] ], [ [P.g, P.s, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_183_67,(0,0,1):C.UVGC_183_68,(0,0,2):C.UVGC_183_69})

V_91 = CTVertex(name = 'V_91',
                type = 'UV',
                particles = [ P.b__tilde__, P.s, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8], [P.g, P.s, P.sig8] ], [ [P.g, P.sig8] ], [ [P.g, P.s], [P.b, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_195_95,(0,0,3):C.UVGC_195_96,(0,0,2):C.UVGC_195_97,(0,0,1):C.UVGC_195_98,(0,1,0):C.UVGC_193_87,(0,1,3):C.UVGC_193_88,(0,1,2):C.UVGC_193_89,(0,1,1):C.UVGC_193_90})

V_92 = CTVertex(name = 'V_92',
                type = 'UV',
                particles = [ P.d__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8], [P.d, P.g, P.sig8] ], [ [P.d, P.g], [P.b, P.d, P.g] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_192_83,(0,0,2):C.UVGC_192_84,(0,0,3):C.UVGC_192_85,(0,0,1):C.UVGC_192_86,(0,1,0):C.UVGC_194_91,(0,1,2):C.UVGC_194_92,(0,1,3):C.UVGC_194_93,(0,1,1):C.UVGC_194_94})

V_93 = CTVertex(name = 'V_93',
                type = 'UV',
                particles = [ P.s__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8], [P.g, P.s, P.sig8] ], [ [P.g, P.sig8] ], [ [P.g, P.s], [P.b, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_193_87,(0,0,3):C.UVGC_193_88,(0,0,2):C.UVGC_193_89,(0,0,1):C.UVGC_193_90,(0,1,0):C.UVGC_195_95,(0,1,3):C.UVGC_195_96,(0,1,2):C.UVGC_195_97,(0,1,1):C.UVGC_195_98})

V_94 = CTVertex(name = 'V_94',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.sig8] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_196_99,(0,0,2):C.UVGC_196_100,(0,0,1):C.UVGC_196_101})

V_95 = CTVertex(name = 'V_95',
                type = 'UV',
                particles = [ P.u__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.g, P.sig8] ], [ [P.g, P.sig8, P.u] ], [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_185_70,(0,0,2):C.UVGC_185_71,(0,0,1):C.UVGC_185_72})

V_96 = CTVertex(name = 'V_96',
                type = 'UV',
                particles = [ P.c__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.u], [P.c, P.g, P.u] ], [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.u] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_187_76,(0,0,2):C.UVGC_187_77,(0,0,1):C.UVGC_187_78,(0,1,0):C.UVGC_186_73,(0,1,2):C.UVGC_186_74,(0,1,1):C.UVGC_186_75})

V_97 = CTVertex(name = 'V_97',
                type = 'UV',
                particles = [ P.t__tilde__, P.u, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.g, P.sig8] ], [ [P.g, P.sig8, P.t], [P.g, P.sig8, P.u] ], [ [P.g, P.t] ], [ [P.g, P.u], [P.g, P.t, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_217_163,(0,0,2):C.UVGC_217_164,(0,0,3):C.UVGC_217_165,(0,0,1):C.UVGC_217_166,(0,1,0):C.UVGC_215_155,(0,1,2):C.UVGC_215_156,(0,1,3):C.UVGC_215_157,(0,1,1):C.UVGC_215_158})

V_98 = CTVertex(name = 'V_98',
                type = 'UV',
                particles = [ P.u__tilde__, P.c, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS3, L.FFS5 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.u], [P.c, P.g, P.u] ], [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.u] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_186_73,(0,0,2):C.UVGC_186_74,(0,0,1):C.UVGC_186_75,(0,1,0):C.UVGC_187_76,(0,1,2):C.UVGC_187_77,(0,1,1):C.UVGC_187_78})

V_99 = CTVertex(name = 'V_99',
                type = 'UV',
                particles = [ P.c__tilde__, P.c, P.sig8 ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.c, P.g] ], [ [P.c, P.g, P.sig8] ], [ [P.g, P.sig8] ] ],
                couplings = {(0,0,0):C.UVGC_178_53,(0,0,2):C.UVGC_178_54,(0,0,1):C.UVGC_178_55})

V_100 = CTVertex(name = 'V_100',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.c, P.sig8 ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.c, P.g], [P.c, P.g, P.t] ], [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.t] ], [ [P.g, P.sig8] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_218_167,(0,0,2):C.UVGC_218_168,(0,0,3):C.UVGC_218_169,(0,0,1):C.UVGC_218_170,(0,1,0):C.UVGC_216_159,(0,1,2):C.UVGC_216_160,(0,1,3):C.UVGC_216_161,(0,1,1):C.UVGC_216_162})

V_101 = CTVertex(name = 'V_101',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.t, P.sig8 ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.g, P.sig8] ], [ [P.g, P.sig8, P.t], [P.g, P.sig8, P.u] ], [ [P.g, P.t] ], [ [P.g, P.u], [P.g, P.t, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_215_155,(0,0,2):C.UVGC_215_156,(0,0,3):C.UVGC_215_157,(0,0,1):C.UVGC_215_158,(0,1,0):C.UVGC_217_163,(0,1,2):C.UVGC_217_164,(0,1,3):C.UVGC_217_165,(0,1,1):C.UVGC_217_166})

V_102 = CTVertex(name = 'V_102',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.t, P.sig8 ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFS3, L.FFS5 ],
                 loop_particles = [ [ [P.c, P.g], [P.c, P.g, P.t] ], [ [P.c, P.g, P.sig8], [P.g, P.sig8, P.t] ], [ [P.g, P.sig8] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_216_159,(0,0,2):C.UVGC_216_160,(0,0,3):C.UVGC_216_161,(0,0,1):C.UVGC_216_162,(0,1,0):C.UVGC_218_167,(0,1,2):C.UVGC_218_168,(0,1,3):C.UVGC_218_169,(0,1,1):C.UVGC_218_170})

V_103 = CTVertex(name = 'V_103',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.sig8 ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFS2 ],
                 loop_particles = [ [ [P.g, P.sig8] ], [ [P.g, P.sig8, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_219_171,(0,0,2):C.UVGC_219_172,(0,0,1):C.UVGC_219_173})

V_104 = CTVertex(name = 'V_104',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_153_24,(0,1,0):C.UVGC_133_3,(0,2,0):C.UVGC_133_3})

V_105 = CTVertex(name = 'V_105',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_153_24,(0,1,0):C.UVGC_133_3,(0,2,0):C.UVGC_133_3})

V_106 = CTVertex(name = 'V_106',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_153_24,(0,1,0):C.UVGC_213_153,(0,2,0):C.UVGC_213_153})

V_107 = CTVertex(name = 'V_107',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.u] ], [ [P.sig8] ] ],
                 couplings = {(0,0,4):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,1):C.UVGC_154_26,(0,1,2):C.UVGC_154_27,(0,1,3):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,4):C.UVGC_154_30,(0,2,0):C.UVGC_154_25,(0,2,1):C.UVGC_154_26,(0,2,2):C.UVGC_154_27,(0,2,3):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,4):C.UVGC_154_30})

V_108 = CTVertex(name = 'V_108',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.c, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ] ],
                 couplings = {(0,0,2):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,1):C.UVGC_154_26,(0,1,3):C.UVGC_154_27,(0,1,4):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,2):C.UVGC_154_30,(0,2,0):C.UVGC_154_25,(0,2,1):C.UVGC_154_26,(0,2,3):C.UVGC_154_27,(0,2,4):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,2):C.UVGC_154_30})

V_109 = CTVertex(name = 'V_109',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.t] ], [ [P.sig8] ] ],
                 couplings = {(0,0,4):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,1):C.UVGC_154_26,(0,1,2):C.UVGC_154_27,(0,1,3):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,4):C.UVGC_214_154,(0,2,0):C.UVGC_154_25,(0,2,1):C.UVGC_154_26,(0,2,2):C.UVGC_154_27,(0,2,3):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,4):C.UVGC_214_154})

V_110 = CTVertex(name = 'V_110',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_179_56,(0,0,1):C.UVGC_179_57})

V_111 = CTVertex(name = 'V_111',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_179_56,(0,0,1):C.UVGC_179_57})

V_112 = CTVertex(name = 'V_112',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_220_174,(0,0,2):C.UVGC_220_175,(0,0,1):C.UVGC_179_57})

V_113 = CTVertex(name = 'V_113',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t, P.Z ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_221_176,(0,1,0):C.UVGC_222_177})

V_114 = CTVertex(name = 'V_114',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_150_21,(0,1,0):C.UVGC_135_4,(0,2,0):C.UVGC_135_4})

V_115 = CTVertex(name = 'V_115',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_150_21,(0,1,0):C.UVGC_135_4,(0,2,0):C.UVGC_135_4})

V_116 = CTVertex(name = 'V_116',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.a ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_150_21,(0,1,0):C.UVGC_190_81,(0,2,0):C.UVGC_190_81})

V_117 = CTVertex(name = 'V_117',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.d, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ] ],
                 couplings = {(0,0,2):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,1):C.UVGC_154_26,(0,1,3):C.UVGC_154_27,(0,1,4):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,2):C.UVGC_154_30,(0,2,0):C.UVGC_154_25,(0,2,1):C.UVGC_154_26,(0,2,3):C.UVGC_154_27,(0,2,4):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,2):C.UVGC_154_30})

V_118 = CTVertex(name = 'V_118',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.s] ], [ [P.sig8] ] ],
                 couplings = {(0,0,4):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,1):C.UVGC_154_26,(0,1,2):C.UVGC_154_27,(0,1,3):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,4):C.UVGC_154_30,(0,2,0):C.UVGC_154_25,(0,2,1):C.UVGC_154_26,(0,2,2):C.UVGC_154_27,(0,2,3):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,4):C.UVGC_154_30})

V_119 = CTVertex(name = 'V_119',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.b, P.g] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ] ],
                 couplings = {(0,0,1):C.UVGC_151_22,(0,1,0):C.UVGC_154_25,(0,1,2):C.UVGC_154_26,(0,1,3):C.UVGC_154_27,(0,1,4):C.UVGC_154_28,(0,1,5):C.UVGC_154_29,(0,1,1):C.UVGC_191_82,(0,2,0):C.UVGC_154_25,(0,2,2):C.UVGC_154_26,(0,2,3):C.UVGC_154_27,(0,2,4):C.UVGC_154_28,(0,2,5):C.UVGC_154_29,(0,2,1):C.UVGC_191_82})

V_120 = CTVertex(name = 'V_120',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_179_56,(0,0,1):C.UVGC_179_57})

V_121 = CTVertex(name = 'V_121',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_179_56,(0,0,1):C.UVGC_179_57})

V_122 = CTVertex(name = 'V_122',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_220_174,(0,0,2):C.UVGC_220_175,(0,0,1):C.UVGC_179_57})

V_123 = CTVertex(name = 'V_123',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.Z ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_198_103,(0,1,0):C.UVGC_199_104})

V_124 = CTVertex(name = 'V_124',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF3, L.FF6 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_152_23,(0,1,0):C.UVGC_132_2,(0,2,0):C.UVGC_132_2})

V_125 = CTVertex(name = 'V_125',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF3, L.FF6 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_152_23,(0,1,0):C.UVGC_132_2,(0,2,0):C.UVGC_132_2})

V_126 = CTVertex(name = 'V_126',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF2, L.FF3, L.FF4, L.FF6 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_225_184,(0,2,0):C.UVGC_225_184,(0,1,0):C.UVGC_212_152,(0,3,0):C.UVGC_212_152})

V_127 = CTVertex(name = 'V_127',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF3, L.FF6 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_152_23,(0,1,0):C.UVGC_132_2,(0,2,0):C.UVGC_132_2})

V_128 = CTVertex(name = 'V_128',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF3, L.FF6 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_152_23,(0,1,0):C.UVGC_132_2,(0,2,0):C.UVGC_132_2})

V_129 = CTVertex(name = 'V_129',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF5, L.FF7 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_197_102,(0,1,0):C.UVGC_189_80})

V_130 = CTVertex(name = 'V_130',
                 type = 'UV',
                 particles = [ P.sig8, P.sig8 ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.SS1, L.SS2 ],
                 loop_particles = [ [ [P.g, P.sig8] ] ],
                 couplings = {(0,0,0):C.UVGC_188_79,(0,1,0):C.UVGC_165_40})

V_131 = CTVertex(name = 'V_131',
                 type = 'UV',
                 particles = [ P.g, P.g ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.VV1, L.VV5 ],
                 loop_particles = [ [ [P.b] ], [ [P.g] ], [ [P.ghG] ], [ [P.sig8] ], [ [P.t] ] ],
                 couplings = {(0,1,0):C.UVGC_202_107,(0,1,3):C.UVGC_202_108,(0,1,4):C.UVGC_202_109,(0,0,1):C.UVGC_142_7,(0,0,2):C.UVGC_142_8})

V_132 = CTVertex(name = 'V_132',
                 type = 'UV',
                 particles = [ P.sig8, P.sig8, P.sig8 ],
                 color = [ 'd(1,2,3)' ],
                 lorentz = [ L.SSS2, L.SSS3 ],
                 loop_particles = [ [ [P.g] ], [ [P.g, P.sig8] ] ],
                 couplings = {(0,0,1):C.UVGC_164_39,(0,1,0):C.UVGC_141_6})

