# This file was automatically created by FeynRules =.2.4
# Mathematica version: 8.0 for Linux x86 (32-bit) (October 10, 2011)
# Date: Sun 25 Oct 2015 16:38:30


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
               couplings = {(0,0,0):C.R2GC_178_50,(0,0,1):C.R2GC_178_51,(0,1,0):C.R2GC_181_52,(0,1,1):C.R2GC_181_53,(0,2,0):C.R2GC_181_52,(0,2,1):C.R2GC_181_53,(0,3,0):C.R2GC_178_50,(0,3,1):C.R2GC_178_51,(0,4,0):C.R2GC_178_50,(0,4,1):C.R2GC_178_51,(0,5,0):C.R2GC_181_52,(0,5,1):C.R2GC_181_53})

V_2 = CTVertex(name = 'V_2',
               type = 'R2',
               particles = [ P.g, P.g, P.g, P.g ],
               color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
               lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
               loop_particles = [ [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ] ],
               couplings = {(2,0,0):C.R2GC_117_9,(2,0,1):C.R2GC_117_10,(0,0,0):C.R2GC_117_9,(0,0,1):C.R2GC_117_10,(4,0,0):C.R2GC_115_5,(4,0,1):C.R2GC_115_6,(3,0,0):C.R2GC_115_5,(3,0,1):C.R2GC_115_6,(8,0,0):C.R2GC_116_7,(8,0,1):C.R2GC_116_8,(7,0,0):C.R2GC_122_16,(7,0,1):C.R2GC_187_58,(6,0,0):C.R2GC_121_14,(6,0,1):C.R2GC_188_59,(5,0,0):C.R2GC_115_5,(5,0,1):C.R2GC_115_6,(1,0,0):C.R2GC_115_5,(1,0,1):C.R2GC_115_6,(11,0,0):C.R2GC_119_12,(11,0,1):C.R2GC_119_13,(10,0,0):C.R2GC_119_12,(10,0,1):C.R2GC_119_13,(9,0,1):C.R2GC_118_11,(2,1,0):C.R2GC_117_9,(2,1,1):C.R2GC_117_10,(0,1,0):C.R2GC_117_9,(0,1,1):C.R2GC_117_10,(6,1,0):C.R2GC_185_55,(6,1,1):C.R2GC_185_56,(4,1,0):C.R2GC_115_5,(4,1,1):C.R2GC_115_6,(3,1,0):C.R2GC_115_5,(3,1,1):C.R2GC_115_6,(8,1,0):C.R2GC_116_7,(8,1,1):C.R2GC_189_60,(7,1,0):C.R2GC_122_16,(7,1,1):C.R2GC_122_17,(5,1,0):C.R2GC_115_5,(5,1,1):C.R2GC_115_6,(1,1,0):C.R2GC_115_5,(1,1,1):C.R2GC_115_6,(11,1,0):C.R2GC_119_12,(11,1,1):C.R2GC_119_13,(10,1,0):C.R2GC_119_12,(10,1,1):C.R2GC_119_13,(9,1,1):C.R2GC_118_11,(2,2,0):C.R2GC_117_9,(2,2,1):C.R2GC_117_10,(0,2,0):C.R2GC_117_9,(0,2,1):C.R2GC_117_10,(4,2,0):C.R2GC_115_5,(4,2,1):C.R2GC_115_6,(3,2,0):C.R2GC_115_5,(3,2,1):C.R2GC_115_6,(8,2,0):C.R2GC_116_7,(8,2,1):C.R2GC_186_57,(6,2,0):C.R2GC_121_14,(6,2,1):C.R2GC_121_15,(7,2,0):C.R2GC_184_54,(7,2,1):C.R2GC_117_10,(5,2,0):C.R2GC_115_5,(5,2,1):C.R2GC_115_6,(1,2,0):C.R2GC_115_5,(1,2,1):C.R2GC_115_6,(11,2,0):C.R2GC_119_12,(11,2,1):C.R2GC_119_13,(10,2,0):C.R2GC_119_12,(10,2,1):C.R2GC_119_13,(9,2,1):C.R2GC_118_11})

V_3 = CTVertex(name = 'V_3',
               type = 'R2',
               particles = [ P.ve__tilde__, P.d__tilde__, P.lqsd ],
               color = [ 'Identity(2,3)' ],
               lorentz = [ L.FFS3 ],
               loop_particles = [ [ [P.d, P.g, P.lqsd] ] ],
               couplings = {(0,0,0):C.R2GC_156_31})

V_4 = CTVertex(name = 'V_4',
               type = 'R2',
               particles = [ P.e__plus__, P.d, P.lqsu__tilde__ ],
               color = [ 'Identity(2,3)' ],
               lorentz = [ L.FFS3 ],
               loop_particles = [ [ [P.d, P.g, P.lqsu] ] ],
               couplings = {(0,0,0):C.R2GC_158_32})

V_5 = CTVertex(name = 'V_5',
               type = 'R2',
               particles = [ P.ve__tilde__, P.u, P.lqsu__tilde__ ],
               color = [ 'Identity(2,3)' ],
               lorentz = [ L.FFS3 ],
               loop_particles = [ [ [P.g, P.lqsu, P.u] ] ],
               couplings = {(0,0,0):C.R2GC_164_40})

V_6 = CTVertex(name = 'V_6',
               type = 'R2',
               particles = [ P.u, P.e__minus__, P.lqsd__tilde__ ],
               color = [ 'Identity(1,3)' ],
               lorentz = [ L.FFS3 ],
               loop_particles = [ [ [P.g, P.lqsd, P.u] ] ],
               couplings = {(0,0,0):C.R2GC_162_39})

V_7 = CTVertex(name = 'V_7',
               type = 'R2',
               particles = [ P.t__tilde__, P.b, P.G__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS3, L.FFS4 ],
               loop_particles = [ [ [P.b, P.g, P.t] ] ],
               couplings = {(0,0,0):C.R2GC_199_64,(0,1,0):C.R2GC_200_65})

V_8 = CTVertex(name = 'V_8',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.G0 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS1 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_173_44})

V_9 = CTVertex(name = 'V_9',
               type = 'R2',
               particles = [ P.b__tilde__, P.b, P.H ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFS2 ],
               loop_particles = [ [ [P.b, P.g] ] ],
               couplings = {(0,0,0):C.R2GC_172_43})

V_10 = CTVertex(name = 'V_10',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_202_67})

V_11 = CTVertex(name = 'V_11',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_203_68})

V_12 = CTVertex(name = 'V_12',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_201_66,(0,1,0):C.R2GC_198_63})

V_13 = CTVertex(name = 'V_13',
                type = 'R2',
                particles = [ P.u__tilde__, P.e__plus__, P.lqsd ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.g, P.lqsd, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_162_39})

V_14 = CTVertex(name = 'V_14',
                type = 'R2',
                particles = [ P.d__tilde__, P.e__minus__, P.lqsu ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.d, P.g, P.lqsu] ] ],
                couplings = {(0,0,0):C.R2GC_158_32})

V_15 = CTVertex(name = 'V_15',
                type = 'R2',
                particles = [ P.ve, P.d, P.lqsd__tilde__ ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.d, P.g, P.lqsd] ] ],
                couplings = {(0,0,0):C.R2GC_156_31})

V_16 = CTVertex(name = 'V_16',
                type = 'R2',
                particles = [ P.u__tilde__, P.ve, P.lqsu ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.g, P.lqsu, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_164_40})

V_17 = CTVertex(name = 'V_17',
                type = 'R2',
                particles = [ P.g, P.lqsu__tilde__, P.lqsu ],
                color = [ 'T(1,3,2)' ],
                lorentz = [ L.VSS1, L.VSS2 ],
                loop_particles = [ [ [P.g, P.lqsu] ] ],
                couplings = {(0,0,0):C.R2GC_137_27,(0,1,0):C.R2GC_136_26})

V_18 = CTVertex(name = 'V_18',
                type = 'R2',
                particles = [ P.g, P.g, P.lqsu__tilde__, P.lqsu ],
                color = [ 'Identity(1,2)*Identity(3,4)', 'T(1,-1,3)*T(2,4,-1)', 'T(1,4,-1)*T(2,-1,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.lqsu] ] ],
                couplings = {(2,0,0):C.R2GC_138_28,(2,0,1):C.R2GC_138_29,(1,0,0):C.R2GC_138_28,(1,0,1):C.R2GC_138_29,(0,0,0):C.R2GC_119_13,(0,0,1):C.R2GC_134_24})

V_19 = CTVertex(name = 'V_19',
                type = 'R2',
                particles = [ P.g, P.lqsd__tilde__, P.lqsd ],
                color = [ 'T(1,3,2)' ],
                lorentz = [ L.VSS1, L.VSS2 ],
                loop_particles = [ [ [P.g, P.lqsd] ] ],
                couplings = {(0,0,0):C.R2GC_137_27,(0,1,0):C.R2GC_136_26})

V_20 = CTVertex(name = 'V_20',
                type = 'R2',
                particles = [ P.g, P.g, P.lqsd__tilde__, P.lqsd ],
                color = [ 'Identity(1,2)*Identity(3,4)', 'T(1,-1,3)*T(2,4,-1)', 'T(1,4,-1)*T(2,-1,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.lqsd] ] ],
                couplings = {(2,0,0):C.R2GC_138_28,(2,0,1):C.R2GC_138_29,(1,0,0):C.R2GC_138_28,(1,0,1):C.R2GC_138_29,(0,0,0):C.R2GC_119_13,(0,0,1):C.R2GC_134_24})

V_21 = CTVertex(name = 'V_21',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_126_21})

V_22 = CTVertex(name = 'V_22',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_126_21})

V_23 = CTVertex(name = 'V_23',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_126_21})

V_24 = CTVertex(name = 'V_24',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_25 = CTVertex(name = 'V_25',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_26 = CTVertex(name = 'V_26',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_27 = CTVertex(name = 'V_27',
                type = 'R2',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_28 = CTVertex(name = 'V_28',
                type = 'R2',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_29 = CTVertex(name = 'V_29',
                type = 'R2',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_30 = CTVertex(name = 'V_30',
                type = 'R2',
                particles = [ P.u__tilde__, P.u, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_100_1,(0,1,0):C.R2GC_197_62})

V_31 = CTVertex(name = 'V_31',
                type = 'R2',
                particles = [ P.c__tilde__, P.c, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_100_1,(0,1,0):C.R2GC_197_62})

V_32 = CTVertex(name = 'V_32',
                type = 'R2',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_100_1,(0,1,0):C.R2GC_197_62})

V_33 = CTVertex(name = 'V_33',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_123_18})

V_34 = CTVertex(name = 'V_34',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_123_18})

V_35 = CTVertex(name = 'V_35',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_123_18})

V_36 = CTVertex(name = 'V_36',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_37 = CTVertex(name = 'V_37',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_38 = CTVertex(name = 'V_38',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_124_19})

V_39 = CTVertex(name = 'V_39',
                type = 'R2',
                particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_40 = CTVertex(name = 'V_40',
                type = 'R2',
                particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_41 = CTVertex(name = 'V_41',
                type = 'R2',
                particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_155_30})

V_42 = CTVertex(name = 'V_42',
                type = 'R2',
                particles = [ P.d__tilde__, P.d, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_101_2,(0,1,0):C.R2GC_171_42})

V_43 = CTVertex(name = 'V_43',
                type = 'R2',
                particles = [ P.s__tilde__, P.s, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_101_2,(0,1,0):C.R2GC_171_42})

V_44 = CTVertex(name = 'V_44',
                type = 'R2',
                particles = [ P.b__tilde__, P.b, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_101_2,(0,1,0):C.R2GC_171_42})

V_45 = CTVertex(name = 'V_45',
                type = 'R2',
                particles = [ P.u__tilde__, P.u ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_125_20})

V_46 = CTVertex(name = 'V_46',
                type = 'R2',
                particles = [ P.c__tilde__, P.c ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_125_20})

V_47 = CTVertex(name = 'V_47',
                type = 'R2',
                particles = [ P.t__tilde__, P.t ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF3, L.FF4, L.FF5, L.FF6 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_194_61,(0,2,0):C.R2GC_194_61,(0,1,0):C.R2GC_125_20,(0,3,0):C.R2GC_125_20})

V_48 = CTVertex(name = 'V_48',
                type = 'R2',
                particles = [ P.d__tilde__, P.d ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_125_20})

V_49 = CTVertex(name = 'V_49',
                type = 'R2',
                particles = [ P.s__tilde__, P.s ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF1 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.R2GC_125_20})

V_50 = CTVertex(name = 'V_50',
                type = 'R2',
                particles = [ P.b__tilde__, P.b ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FF3, L.FF4, L.FF5, L.FF6 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.R2GC_169_41,(0,2,0):C.R2GC_169_41,(0,1,0):C.R2GC_125_20,(0,3,0):C.R2GC_125_20})

V_51 = CTVertex(name = 'V_51',
                type = 'R2',
                particles = [ P.lqsu__tilde__, P.lqsu ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.SS1, L.SS2 ],
                loop_particles = [ [ [P.g, P.lqsu] ] ],
                couplings = {(0,0,0):C.R2GC_175_46,(0,1,0):C.R2GC_135_25})

V_52 = CTVertex(name = 'V_52',
                type = 'R2',
                particles = [ P.lqsd__tilde__, P.lqsd ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.SS1, L.SS2 ],
                loop_particles = [ [ [P.g, P.lqsd] ] ],
                couplings = {(0,0,0):C.R2GC_174_45,(0,1,0):C.R2GC_135_25})

V_53 = CTVertex(name = 'V_53',
                type = 'R2',
                particles = [ P.g, P.g ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VV1, L.VV2, L.VV3 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.g] ], [ [P.t] ] ],
                couplings = {(0,0,2):C.R2GC_177_49,(0,1,0):C.R2GC_91_69,(0,1,3):C.R2GC_91_70,(0,2,1):C.R2GC_176_47,(0,2,2):C.R2GC_176_48})

V_54 = CTVertex(name = 'V_54',
                type = 'R2',
                particles = [ P.g, P.g, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_92_71,(0,0,1):C.R2GC_92_72})

V_55 = CTVertex(name = 'V_55',
                type = 'R2',
                particles = [ P.g, P.g, P.W__minus__, P.W__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b, P.t], [P.c, P.s], [P.d, P.u] ] ],
                couplings = {(0,0,0):C.R2GC_104_4,(0,1,0):C.R2GC_104_4,(0,2,0):C.R2GC_104_4})

V_56 = CTVertex(name = 'V_56',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.Z ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_96_79,(0,0,1):C.R2GC_96_80,(0,1,0):C.R2GC_96_79,(0,1,1):C.R2GC_96_80,(0,2,0):C.R2GC_96_79,(0,2,1):C.R2GC_96_80})

V_57 = CTVertex(name = 'V_57',
                type = 'R2',
                particles = [ P.g, P.g, P.Z, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_99_85,(0,0,1):C.R2GC_99_86,(0,1,0):C.R2GC_99_85,(0,1,1):C.R2GC_99_86,(0,2,0):C.R2GC_99_85,(0,2,1):C.R2GC_99_86})

V_58 = CTVertex(name = 'V_58',
                type = 'R2',
                particles = [ P.a, P.a, P.g, P.g ],
                color = [ 'Identity(3,4)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_94_75,(0,0,1):C.R2GC_94_76,(0,1,0):C.R2GC_94_75,(0,1,1):C.R2GC_94_76,(0,2,0):C.R2GC_94_75,(0,2,1):C.R2GC_94_76})

V_59 = CTVertex(name = 'V_59',
                type = 'R2',
                particles = [ P.g, P.g, P.g, P.Z ],
                color = [ 'd(1,2,3)', 'f(1,2,3)' ],
                lorentz = [ L.VVVV1, L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(1,0,0):C.R2GC_98_83,(1,0,1):C.R2GC_98_84,(0,1,0):C.R2GC_97_81,(0,1,1):C.R2GC_97_82,(0,2,0):C.R2GC_97_81,(0,2,1):C.R2GC_97_82,(0,3,0):C.R2GC_97_81,(0,3,1):C.R2GC_97_82})

V_60 = CTVertex(name = 'V_60',
                type = 'R2',
                particles = [ P.a, P.g, P.g, P.g ],
                color = [ 'd(2,3,4)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b], [P.d], [P.s] ], [ [P.c], [P.t], [P.u] ] ],
                couplings = {(0,0,0):C.R2GC_95_77,(0,0,1):C.R2GC_95_78,(0,1,0):C.R2GC_95_77,(0,1,1):C.R2GC_95_78,(0,2,0):C.R2GC_95_77,(0,2,1):C.R2GC_95_78})

V_61 = CTVertex(name = 'V_61',
                type = 'R2',
                particles = [ P.g, P.g, P.H, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_93_73,(0,0,1):C.R2GC_93_74})

V_62 = CTVertex(name = 'V_62',
                type = 'R2',
                particles = [ P.g, P.g, P.G0, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.R2GC_93_73,(0,0,1):C.R2GC_93_74})

V_63 = CTVertex(name = 'V_63',
                type = 'R2',
                particles = [ P.g, P.g, P.G__minus__, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b, P.t] ] ],
                couplings = {(0,0,0):C.R2GC_103_3})

V_64 = CTVertex(name = 'V_64',
                type = 'R2',
                particles = [ P.lqsu__tilde__, P.lqsu__tilde__, P.lqsu, P.lqsu ],
                color = [ 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.SSSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.lqsu] ] ],
                couplings = {(1,0,0):C.R2GC_133_22,(1,0,1):C.R2GC_133_23,(0,0,0):C.R2GC_133_22,(0,0,1):C.R2GC_133_23})

V_65 = CTVertex(name = 'V_65',
                type = 'R2',
                particles = [ P.lqsd__tilde__, P.lqsd, P.lqsu__tilde__, P.lqsu ],
                color = [ 'Identity(1,2)*Identity(3,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.SSSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.lqsd], [P.g, P.lqsu] ], [ [P.g, P.lqsd, P.lqsu] ] ],
                couplings = {(1,0,0):C.R2GC_160_36,(1,0,1):C.R2GC_160_37,(1,0,2):C.R2GC_160_38,(0,0,0):C.R2GC_159_33,(0,0,1):C.R2GC_159_34,(0,0,2):C.R2GC_159_35})

V_66 = CTVertex(name = 'V_66',
                type = 'R2',
                particles = [ P.lqsd__tilde__, P.lqsd__tilde__, P.lqsd, P.lqsd ],
                color = [ 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.SSSS1 ],
                loop_particles = [ [ [P.g] ], [ [P.g, P.lqsd] ] ],
                couplings = {(1,0,0):C.R2GC_133_22,(1,0,1):C.R2GC_133_23,(0,0,0):C.R2GC_133_22,(0,0,1):C.R2GC_133_23})

V_67 = CTVertex(name = 'V_67',
                type = 'UV',
                particles = [ P.g, P.g, P.g ],
                color = [ 'f(1,2,3)' ],
                lorentz = [ L.VVV1, L.VVV2, L.VVV3, L.VVV4, L.VVV5, L.VVV6, L.VVV7, L.VVV8 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsd], [P.lqsu] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_178_101,(0,0,1):C.UVGC_178_102,(0,0,2):C.UVGC_178_103,(0,0,3):C.UVGC_120_15,(0,0,4):C.UVGC_178_104,(0,0,6):C.UVGC_178_105,(0,0,7):C.UVGC_178_106,(0,1,0):C.UVGC_181_113,(0,1,1):C.UVGC_181_114,(0,1,2):C.UVGC_182_119,(0,1,3):C.UVGC_182_120,(0,1,4):C.UVGC_182_121,(0,1,6):C.UVGC_182_122,(0,1,7):C.UVGC_181_118,(0,3,0):C.UVGC_181_113,(0,3,1):C.UVGC_181_114,(0,3,2):C.UVGC_181_115,(0,3,3):C.UVGC_114_5,(0,3,4):C.UVGC_181_116,(0,3,6):C.UVGC_181_117,(0,3,7):C.UVGC_181_118,(0,5,0):C.UVGC_178_101,(0,5,1):C.UVGC_178_102,(0,5,2):C.UVGC_180_109,(0,5,3):C.UVGC_180_110,(0,5,4):C.UVGC_180_111,(0,5,6):C.UVGC_180_112,(0,5,7):C.UVGC_178_106,(0,6,0):C.UVGC_178_101,(0,6,1):C.UVGC_178_102,(0,6,2):C.UVGC_179_107,(0,6,3):C.UVGC_179_108,(0,6,4):C.UVGC_178_104,(0,6,6):C.UVGC_178_105,(0,6,7):C.UVGC_178_106,(0,7,0):C.UVGC_181_113,(0,7,1):C.UVGC_181_114,(0,7,2):C.UVGC_183_123,(0,7,3):C.UVGC_183_124,(0,7,4):C.UVGC_182_121,(0,7,6):C.UVGC_182_122,(0,7,7):C.UVGC_181_118,(0,2,2):C.UVGC_114_4,(0,2,3):C.UVGC_114_5,(0,4,2):C.UVGC_120_14,(0,4,3):C.UVGC_120_15,(0,4,5):C.UVGC_120_16})

V_68 = CTVertex(name = 'V_68',
                type = 'UV',
                particles = [ P.g, P.g, P.g, P.g ],
                color = [ 'd(-1,1,3)*d(-1,2,4)', 'd(-1,1,3)*f(-1,2,4)', 'd(-1,1,4)*d(-1,2,3)', 'd(-1,1,4)*f(-1,2,3)', 'd(-1,2,3)*f(-1,1,4)', 'd(-1,2,4)*f(-1,1,3)', 'f(-1,1,2)*f(-1,3,4)', 'f(-1,1,3)*f(-1,2,4)', 'f(-1,1,4)*f(-1,2,3)', 'Identity(1,2)*Identity(3,4)', 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                lorentz = [ L.VVVV2, L.VVVV3, L.VVVV4 ],
                loop_particles = [ [ [P.b] ], [ [P.b], [P.c], [P.d], [P.lqsd], [P.lqsu], [P.s], [P.t], [P.u] ], [ [P.b], [P.c], [P.d], [P.s], [P.t], [P.u] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsd], [P.lqsu] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(2,0,4):C.UVGC_116_9,(2,0,5):C.UVGC_116_8,(0,0,4):C.UVGC_116_9,(0,0,5):C.UVGC_116_8,(4,0,4):C.UVGC_115_6,(4,0,5):C.UVGC_115_7,(3,0,4):C.UVGC_115_6,(3,0,5):C.UVGC_115_7,(8,0,4):C.UVGC_116_8,(8,0,5):C.UVGC_116_9,(7,0,0):C.UVGC_187_142,(7,0,3):C.UVGC_138_49,(7,0,4):C.UVGC_187_143,(7,0,5):C.UVGC_187_144,(7,0,6):C.UVGC_187_145,(7,0,8):C.UVGC_187_146,(7,0,9):C.UVGC_187_147,(6,0,0):C.UVGC_187_142,(6,0,3):C.UVGC_138_49,(6,0,4):C.UVGC_188_148,(6,0,5):C.UVGC_188_149,(6,0,6):C.UVGC_187_145,(6,0,8):C.UVGC_187_146,(6,0,9):C.UVGC_187_147,(5,0,4):C.UVGC_115_6,(5,0,5):C.UVGC_115_7,(1,0,4):C.UVGC_115_6,(1,0,5):C.UVGC_115_7,(11,0,4):C.UVGC_119_12,(11,0,5):C.UVGC_119_13,(10,0,4):C.UVGC_119_12,(10,0,5):C.UVGC_119_13,(9,0,4):C.UVGC_118_10,(9,0,5):C.UVGC_118_11,(2,1,4):C.UVGC_116_9,(2,1,5):C.UVGC_116_8,(0,1,4):C.UVGC_116_9,(0,1,5):C.UVGC_116_8,(6,1,0):C.UVGC_184_125,(6,1,4):C.UVGC_185_131,(6,1,5):C.UVGC_185_132,(6,1,6):C.UVGC_185_133,(6,1,8):C.UVGC_185_134,(6,1,9):C.UVGC_184_130,(4,1,4):C.UVGC_115_6,(4,1,5):C.UVGC_115_7,(3,1,4):C.UVGC_115_6,(3,1,5):C.UVGC_115_7,(8,1,0):C.UVGC_189_150,(8,1,3):C.UVGC_189_151,(8,1,4):C.UVGC_189_152,(8,1,5):C.UVGC_189_153,(8,1,6):C.UVGC_189_154,(8,1,8):C.UVGC_189_155,(8,1,9):C.UVGC_189_156,(7,1,1):C.UVGC_121_17,(7,1,4):C.UVGC_122_20,(7,1,5):C.UVGC_122_21,(5,1,4):C.UVGC_115_6,(5,1,5):C.UVGC_115_7,(1,1,4):C.UVGC_115_6,(1,1,5):C.UVGC_115_7,(11,1,4):C.UVGC_119_12,(11,1,5):C.UVGC_119_13,(10,1,4):C.UVGC_119_12,(10,1,5):C.UVGC_119_13,(9,1,4):C.UVGC_118_10,(9,1,5):C.UVGC_118_11,(2,2,4):C.UVGC_116_9,(2,2,5):C.UVGC_116_8,(0,2,4):C.UVGC_116_9,(0,2,5):C.UVGC_116_8,(4,2,4):C.UVGC_115_6,(4,2,5):C.UVGC_115_7,(3,2,4):C.UVGC_115_6,(3,2,5):C.UVGC_115_7,(8,2,0):C.UVGC_186_135,(8,2,3):C.UVGC_186_136,(8,2,4):C.UVGC_186_137,(8,2,5):C.UVGC_186_138,(8,2,6):C.UVGC_186_139,(8,2,8):C.UVGC_186_140,(8,2,9):C.UVGC_186_141,(6,2,2):C.UVGC_121_17,(6,2,4):C.UVGC_121_18,(6,2,5):C.UVGC_118_10,(6,2,7):C.UVGC_121_19,(7,2,0):C.UVGC_184_125,(7,2,4):C.UVGC_184_126,(7,2,5):C.UVGC_184_127,(7,2,6):C.UVGC_184_128,(7,2,8):C.UVGC_184_129,(7,2,9):C.UVGC_184_130,(5,2,4):C.UVGC_115_6,(5,2,5):C.UVGC_115_7,(1,2,4):C.UVGC_115_6,(1,2,5):C.UVGC_115_7,(11,2,4):C.UVGC_119_12,(11,2,5):C.UVGC_119_13,(10,2,4):C.UVGC_119_12,(10,2,5):C.UVGC_119_13,(9,2,4):C.UVGC_118_10,(9,2,5):C.UVGC_118_11})

V_69 = CTVertex(name = 'V_69',
                type = 'UV',
                particles = [ P.ve__tilde__, P.d__tilde__, P.lqsd ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.FFS3 ],
                loop_particles = [ [ [P.d, P.g, P.lqsd] ], [ [P.g, P.lqsd] ] ],
                couplings = {(0,0,1):C.UVGC_156_62,(0,0,0):C.UVGC_156_63})

V_70 = CTVertex(name = 'V_70',
                type = 'UV',
                particles = [ P.e__plus__, P.d, P.lqsu__tilde__ ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.FFS3 ],
                loop_particles = [ [ [P.d, P.g] ], [ [P.d, P.g, P.lqsu] ], [ [P.g, P.lqsu] ] ],
                couplings = {(0,0,0):C.UVGC_158_65,(0,0,2):C.UVGC_158_66,(0,0,1):C.UVGC_158_67})

V_71 = CTVertex(name = 'V_71',
                type = 'UV',
                particles = [ P.ve__tilde__, P.u, P.lqsu__tilde__ ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.FFS3 ],
                loop_particles = [ [ [P.g, P.lqsu] ], [ [P.g, P.lqsu, P.u] ], [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_164_77,(0,0,2):C.UVGC_164_78,(0,0,1):C.UVGC_164_79})

V_72 = CTVertex(name = 'V_72',
                type = 'UV',
                particles = [ P.u, P.e__minus__, P.lqsd__tilde__ ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS3 ],
                loop_particles = [ [ [P.g, P.lqsd] ], [ [P.g, P.lqsd, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_162_74,(0,0,1):C.UVGC_162_75})

V_73 = CTVertex(name = 'V_73',
                type = 'UV',
                particles = [ P.t__tilde__, P.b, P.G__plus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_199_169,(0,0,2):C.UVGC_199_170,(0,0,1):C.UVGC_199_171,(0,1,0):C.UVGC_200_172,(0,1,2):C.UVGC_200_173,(0,1,1):C.UVGC_200_174})

V_74 = CTVertex(name = 'V_74',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_173_88})

V_75 = CTVertex(name = 'V_75',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_172_87})

V_76 = CTVertex(name = 'V_76',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.G0 ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS1 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_202_178})

V_77 = CTVertex(name = 'V_77',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.H ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS2 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_203_179})

V_78 = CTVertex(name = 'V_78',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.G__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFS3, L.FFS4 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_201_175,(0,0,2):C.UVGC_201_176,(0,0,1):C.UVGC_201_177,(0,1,0):C.UVGC_198_166,(0,1,2):C.UVGC_198_167,(0,1,1):C.UVGC_198_168})

V_79 = CTVertex(name = 'V_79',
                type = 'UV',
                particles = [ P.u__tilde__, P.e__plus__, P.lqsd ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.g, P.lqsd] ], [ [P.g, P.lqsd, P.u] ], [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_162_74,(0,0,2):C.UVGC_163_76,(0,0,1):C.UVGC_162_75})

V_80 = CTVertex(name = 'V_80',
                type = 'UV',
                particles = [ P.d__tilde__, P.e__minus__, P.lqsu ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.d, P.g] ], [ [P.d, P.g, P.lqsu] ], [ [P.g, P.lqsu] ] ],
                couplings = {(0,0,0):C.UVGC_158_65,(0,0,2):C.UVGC_158_66,(0,0,1):C.UVGC_158_67})

V_81 = CTVertex(name = 'V_81',
                type = 'UV',
                particles = [ P.ve, P.d, P.lqsd__tilde__ ],
                color = [ 'Identity(2,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.d, P.g] ], [ [P.d, P.g, P.lqsd] ], [ [P.g, P.lqsd] ] ],
                couplings = {(0,0,0):C.UVGC_157_64,(0,0,2):C.UVGC_156_62,(0,0,1):C.UVGC_156_63})

V_82 = CTVertex(name = 'V_82',
                type = 'UV',
                particles = [ P.u__tilde__, P.ve, P.lqsu ],
                color = [ 'Identity(1,3)' ],
                lorentz = [ L.FFS4 ],
                loop_particles = [ [ [P.g, P.lqsu] ], [ [P.g, P.lqsu, P.u] ], [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_164_77,(0,0,2):C.UVGC_164_78,(0,0,1):C.UVGC_164_79})

V_83 = CTVertex(name = 'V_83',
                type = 'UV',
                particles = [ P.g, P.lqsu__tilde__, P.lqsu ],
                color = [ 'T(1,3,2)' ],
                lorentz = [ L.VSS1, L.VSS2 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.lqsu] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_137_40,(0,0,1):C.UVGC_137_41,(0,0,2):C.UVGC_137_42,(0,0,3):C.UVGC_137_43,(0,0,5):C.UVGC_137_44,(0,0,6):C.UVGC_137_45,(0,0,7):C.UVGC_137_46,(0,0,4):C.UVGC_143_58,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,2):C.UVGC_124_25,(0,1,3):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,4):C.UVGC_142_57})

V_84 = CTVertex(name = 'V_84',
                type = 'UV',
                particles = [ P.g, P.g, P.lqsu__tilde__, P.lqsu ],
                color = [ 'Identity(1,2)*Identity(3,4)', 'T(1,-1,3)*T(2,4,-1)', 'T(1,4,-1)*T(2,-1,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.lqsu] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(2,0,0):C.UVGC_138_48,(2,0,1):C.UVGC_138_49,(2,0,2):C.UVGC_138_50,(2,0,3):C.UVGC_138_51,(2,0,5):C.UVGC_138_52,(2,0,6):C.UVGC_138_53,(2,0,7):C.UVGC_138_54,(2,0,4):C.UVGC_144_59,(1,0,0):C.UVGC_138_48,(1,0,1):C.UVGC_138_49,(1,0,2):C.UVGC_138_50,(1,0,3):C.UVGC_138_51,(1,0,5):C.UVGC_138_52,(1,0,6):C.UVGC_138_53,(1,0,7):C.UVGC_138_54,(1,0,4):C.UVGC_144_59,(0,0,2):C.UVGC_134_36,(0,0,4):C.UVGC_134_37})

V_85 = CTVertex(name = 'V_85',
                type = 'UV',
                particles = [ P.g, P.lqsd__tilde__, P.lqsd ],
                color = [ 'T(1,3,2)' ],
                lorentz = [ L.VSS1, L.VSS2 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.lqsd] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,0):C.UVGC_137_40,(0,0,1):C.UVGC_137_41,(0,0,2):C.UVGC_137_42,(0,0,3):C.UVGC_137_43,(0,0,5):C.UVGC_137_44,(0,0,6):C.UVGC_137_45,(0,0,7):C.UVGC_137_46,(0,0,4):C.UVGC_137_47,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,2):C.UVGC_124_25,(0,1,3):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,4):C.UVGC_136_39})

V_86 = CTVertex(name = 'V_86',
                type = 'UV',
                particles = [ P.g, P.g, P.lqsd__tilde__, P.lqsd ],
                color = [ 'Identity(1,2)*Identity(3,4)', 'T(1,-1,3)*T(2,4,-1)', 'T(1,4,-1)*T(2,-1,3)' ],
                lorentz = [ L.VVSS1 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.lqsd] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(2,0,0):C.UVGC_138_48,(2,0,1):C.UVGC_138_49,(2,0,2):C.UVGC_138_50,(2,0,3):C.UVGC_138_51,(2,0,5):C.UVGC_138_52,(2,0,6):C.UVGC_138_53,(2,0,7):C.UVGC_138_54,(2,0,4):C.UVGC_138_55,(1,0,0):C.UVGC_138_48,(1,0,1):C.UVGC_138_49,(1,0,2):C.UVGC_138_50,(1,0,3):C.UVGC_138_51,(1,0,5):C.UVGC_138_52,(1,0,6):C.UVGC_138_53,(1,0,7):C.UVGC_138_54,(1,0,4):C.UVGC_138_55,(0,0,2):C.UVGC_134_36,(0,0,4):C.UVGC_134_37})

V_87 = CTVertex(name = 'V_87',
                type = 'UV',
                particles = [ P.u__tilde__, P.u, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_126_32,(0,1,0):C.UVGC_107_2,(0,2,0):C.UVGC_107_2})

V_88 = CTVertex(name = 'V_88',
                type = 'UV',
                particles = [ P.c__tilde__, P.c, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.c, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_126_32,(0,1,0):C.UVGC_107_2,(0,2,0):C.UVGC_107_2})

V_89 = CTVertex(name = 'V_89',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_126_32,(0,1,0):C.UVGC_191_158,(0,2,0):C.UVGC_191_158})

V_90 = CTVertex(name = 'V_90',
                type = 'UV',
                particles = [ P.u__tilde__, P.u, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.u] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,4):C.UVGC_124_30,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,2):C.UVGC_124_25,(0,1,3):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,4):C.UVGC_127_33,(0,2,0):C.UVGC_124_23,(0,2,1):C.UVGC_124_24,(0,2,2):C.UVGC_124_25,(0,2,3):C.UVGC_124_26,(0,2,5):C.UVGC_124_27,(0,2,6):C.UVGC_124_28,(0,2,7):C.UVGC_124_29,(0,2,4):C.UVGC_127_33})

V_91 = CTVertex(name = 'V_91',
                type = 'UV',
                particles = [ P.c__tilde__, P.c, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.c, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,2):C.UVGC_124_30,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,3):C.UVGC_124_25,(0,1,4):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,2):C.UVGC_127_33,(0,2,0):C.UVGC_124_23,(0,2,1):C.UVGC_124_24,(0,2,3):C.UVGC_124_25,(0,2,4):C.UVGC_124_26,(0,2,5):C.UVGC_124_27,(0,2,6):C.UVGC_124_28,(0,2,7):C.UVGC_124_29,(0,2,2):C.UVGC_127_33})

V_92 = CTVertex(name = 'V_92',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.g ],
                color = [ 'T(3,2,1)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.t] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                couplings = {(0,0,4):C.UVGC_124_30,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,2):C.UVGC_124_25,(0,1,3):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,4):C.UVGC_192_159,(0,2,0):C.UVGC_124_23,(0,2,1):C.UVGC_124_24,(0,2,2):C.UVGC_124_25,(0,2,3):C.UVGC_124_26,(0,2,5):C.UVGC_124_27,(0,2,6):C.UVGC_124_28,(0,2,7):C.UVGC_124_29,(0,2,4):C.UVGC_192_159})

V_93 = CTVertex(name = 'V_93',
                type = 'UV',
                particles = [ P.d__tilde__, P.u, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                couplings = {(0,0,0):C.UVGC_155_60,(0,0,1):C.UVGC_155_61})

V_94 = CTVertex(name = 'V_94',
                type = 'UV',
                particles = [ P.s__tilde__, P.c, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_155_60,(0,0,1):C.UVGC_155_61})

V_95 = CTVertex(name = 'V_95',
                type = 'UV',
                particles = [ P.b__tilde__, P.t, P.W__minus__ ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2 ],
                loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_195_162,(0,0,2):C.UVGC_195_163,(0,0,1):C.UVGC_155_61})

V_96 = CTVertex(name = 'V_96',
                type = 'UV',
                particles = [ P.t__tilde__, P.t, P.Z ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.t] ] ],
                couplings = {(0,0,0):C.UVGC_196_164,(0,1,0):C.UVGC_197_165})

V_97 = CTVertex(name = 'V_97',
                type = 'UV',
                particles = [ P.d__tilde__, P.d, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.d, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_123_22,(0,1,0):C.UVGC_109_3,(0,2,0):C.UVGC_109_3})

V_98 = CTVertex(name = 'V_98',
                type = 'UV',
                particles = [ P.s__tilde__, P.s, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.g, P.s] ] ],
                couplings = {(0,0,0):C.UVGC_123_22,(0,1,0):C.UVGC_109_3,(0,2,0):C.UVGC_109_3})

V_99 = CTVertex(name = 'V_99',
                type = 'UV',
                particles = [ P.b__tilde__, P.b, P.a ],
                color = [ 'Identity(1,2)' ],
                lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                loop_particles = [ [ [P.b, P.g] ] ],
                couplings = {(0,0,0):C.UVGC_123_22,(0,1,0):C.UVGC_166_81,(0,2,0):C.UVGC_166_81})

V_100 = CTVertex(name = 'V_100',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.d, P.g] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                 couplings = {(0,0,2):C.UVGC_124_30,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,3):C.UVGC_124_25,(0,1,4):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,2):C.UVGC_127_33,(0,2,0):C.UVGC_124_23,(0,2,1):C.UVGC_124_24,(0,2,3):C.UVGC_124_25,(0,2,4):C.UVGC_124_26,(0,2,5):C.UVGC_124_27,(0,2,6):C.UVGC_124_28,(0,2,7):C.UVGC_124_29,(0,2,2):C.UVGC_127_33})

V_101 = CTVertex(name = 'V_101',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.g, P.s] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                 couplings = {(0,0,4):C.UVGC_124_30,(0,1,0):C.UVGC_124_23,(0,1,1):C.UVGC_124_24,(0,1,2):C.UVGC_124_25,(0,1,3):C.UVGC_124_26,(0,1,5):C.UVGC_124_27,(0,1,6):C.UVGC_124_28,(0,1,7):C.UVGC_124_29,(0,1,4):C.UVGC_127_33,(0,2,0):C.UVGC_124_23,(0,2,1):C.UVGC_124_24,(0,2,2):C.UVGC_124_25,(0,2,3):C.UVGC_124_26,(0,2,5):C.UVGC_124_27,(0,2,6):C.UVGC_124_28,(0,2,7):C.UVGC_124_29,(0,2,4):C.UVGC_127_33})

V_102 = CTVertex(name = 'V_102',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.g ],
                 color = [ 'T(3,2,1)' ],
                 lorentz = [ L.FFV1, L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.b, P.g] ], [ [P.c], [P.d], [P.s], [P.u] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_124_23,(0,0,2):C.UVGC_124_24,(0,0,3):C.UVGC_124_25,(0,0,4):C.UVGC_124_26,(0,0,5):C.UVGC_124_27,(0,0,6):C.UVGC_124_28,(0,0,7):C.UVGC_124_29,(0,0,1):C.UVGC_124_30,(0,1,1):C.UVGC_167_82,(0,2,1):C.UVGC_167_82})

V_103 = CTVertex(name = 'V_103',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.d, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.d, P.g], [P.g, P.u] ], [ [P.d, P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_155_60,(0,0,1):C.UVGC_155_61})

V_104 = CTVertex(name = 'V_104',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.s, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.c, P.g], [P.g, P.s] ], [ [P.c, P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_155_60,(0,0,1):C.UVGC_155_61})

V_105 = CTVertex(name = 'V_105',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.b, P.W__plus__ ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2 ],
                 loop_particles = [ [ [P.b, P.g] ], [ [P.b, P.g, P.t] ], [ [P.g, P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_195_162,(0,0,2):C.UVGC_195_163,(0,0,1):C.UVGC_155_61})

V_106 = CTVertex(name = 'V_106',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b, P.Z ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FFV2, L.FFV3 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_170_85,(0,1,0):C.UVGC_171_86})

V_107 = CTVertex(name = 'V_107',
                 type = 'UV',
                 particles = [ P.u__tilde__, P.u ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF4, L.FF6 ],
                 loop_particles = [ [ [P.g, P.u] ] ],
                 couplings = {(0,0,0):C.UVGC_125_31,(0,1,0):C.UVGC_106_1,(0,2,0):C.UVGC_106_1})

V_108 = CTVertex(name = 'V_108',
                 type = 'UV',
                 particles = [ P.c__tilde__, P.c ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF4, L.FF6 ],
                 loop_particles = [ [ [P.c, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_125_31,(0,1,0):C.UVGC_106_1,(0,2,0):C.UVGC_106_1})

V_109 = CTVertex(name = 'V_109',
                 type = 'UV',
                 particles = [ P.t__tilde__, P.t ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF2, L.FF3, L.FF4, L.FF5, L.FF6 ],
                 loop_particles = [ [ [P.g, P.t] ] ],
                 couplings = {(0,1,0):C.UVGC_194_161,(0,3,0):C.UVGC_194_161,(0,2,0):C.UVGC_190_157,(0,4,0):C.UVGC_190_157,(0,0,0):C.UVGC_193_160})

V_110 = CTVertex(name = 'V_110',
                 type = 'UV',
                 particles = [ P.d__tilde__, P.d ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF4, L.FF6 ],
                 loop_particles = [ [ [P.d, P.g] ] ],
                 couplings = {(0,0,0):C.UVGC_125_31,(0,1,0):C.UVGC_106_1,(0,2,0):C.UVGC_106_1})

V_111 = CTVertex(name = 'V_111',
                 type = 'UV',
                 particles = [ P.s__tilde__, P.s ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF1, L.FF4, L.FF6 ],
                 loop_particles = [ [ [P.g, P.s] ] ],
                 couplings = {(0,0,0):C.UVGC_125_31,(0,1,0):C.UVGC_106_1,(0,2,0):C.UVGC_106_1})

V_112 = CTVertex(name = 'V_112',
                 type = 'UV',
                 particles = [ P.b__tilde__, P.b ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.FF2, L.FF3, L.FF4, L.FF5, L.FF6 ],
                 loop_particles = [ [ [P.b, P.g] ] ],
                 couplings = {(0,1,0):C.UVGC_169_84,(0,3,0):C.UVGC_169_84,(0,2,0):C.UVGC_165_80,(0,4,0):C.UVGC_165_80,(0,0,0):C.UVGC_168_83})

V_113 = CTVertex(name = 'V_113',
                 type = 'UV',
                 particles = [ P.lqsu__tilde__, P.lqsu ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.SS1, L.SS2 ],
                 loop_particles = [ [ [P.g, P.lqsu] ] ],
                 couplings = {(0,0,0):C.UVGC_175_90,(0,1,0):C.UVGC_141_56})

V_114 = CTVertex(name = 'V_114',
                 type = 'UV',
                 particles = [ P.lqsd__tilde__, P.lqsd ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.SS1, L.SS2 ],
                 loop_particles = [ [ [P.g, P.lqsd] ] ],
                 couplings = {(0,0,0):C.UVGC_174_89,(0,1,0):C.UVGC_135_38})

V_115 = CTVertex(name = 'V_115',
                 type = 'UV',
                 particles = [ P.g, P.g ],
                 color = [ 'Identity(1,2)' ],
                 lorentz = [ L.VV1, L.VV3 ],
                 loop_particles = [ [ [P.b] ], [ [P.g] ], [ [P.ghG] ], [ [P.lqsd] ], [ [P.lqsu] ], [ [P.t] ] ],
                 couplings = {(0,0,0):C.UVGC_177_95,(0,0,1):C.UVGC_177_96,(0,0,2):C.UVGC_177_97,(0,0,3):C.UVGC_177_98,(0,0,4):C.UVGC_177_99,(0,0,5):C.UVGC_177_100,(0,1,0):C.UVGC_176_91,(0,1,3):C.UVGC_176_92,(0,1,4):C.UVGC_176_93,(0,1,5):C.UVGC_176_94})

V_116 = CTVertex(name = 'V_116',
                 type = 'UV',
                 particles = [ P.lqsu__tilde__, P.lqsu__tilde__, P.lqsu, P.lqsu ],
                 color = [ 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                 lorentz = [ L.SSSS1 ],
                 loop_particles = [ [ [P.g] ], [ [P.g, P.lqsu] ] ],
                 couplings = {(1,0,0):C.UVGC_133_34,(1,0,1):C.UVGC_133_35,(0,0,0):C.UVGC_133_34,(0,0,1):C.UVGC_133_35})

V_117 = CTVertex(name = 'V_117',
                 type = 'UV',
                 particles = [ P.lqsd__tilde__, P.lqsd, P.lqsu__tilde__, P.lqsu ],
                 color = [ 'Identity(1,2)*Identity(3,4)', 'Identity(1,4)*Identity(2,3)' ],
                 lorentz = [ L.SSSS1 ],
                 loop_particles = [ [ [P.g] ], [ [P.g, P.lqsd], [P.g, P.lqsu] ], [ [P.g, P.lqsd, P.lqsu] ] ],
                 couplings = {(1,0,0):C.UVGC_160_71,(1,0,1):C.UVGC_160_72,(1,0,2):C.UVGC_160_73,(0,0,0):C.UVGC_159_68,(0,0,1):C.UVGC_159_69,(0,0,2):C.UVGC_159_70})

V_118 = CTVertex(name = 'V_118',
                 type = 'UV',
                 particles = [ P.lqsd__tilde__, P.lqsd__tilde__, P.lqsd, P.lqsd ],
                 color = [ 'Identity(1,3)*Identity(2,4)', 'Identity(1,4)*Identity(2,3)' ],
                 lorentz = [ L.SSSS1 ],
                 loop_particles = [ [ [P.g] ], [ [P.g, P.lqsd] ] ],
                 couplings = {(1,0,0):C.UVGC_133_34,(1,0,1):C.UVGC_133_35,(0,0,0):C.UVGC_133_34,(0,0,1):C.UVGC_133_35})

