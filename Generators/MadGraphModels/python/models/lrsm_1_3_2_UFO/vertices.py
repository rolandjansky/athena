# This file was automatically created by FeynRules 2.1
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Tue 2 Dec 2014 07:52:51


from object_library import all_vertices, Vertex
import particles as P
import couplings as C
import lorentz as L


V_1 = Vertex(name = 'V_1',
             particles = [ P.a, P.a, P.A02, P.A02 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_308, C.GC_139, C.GC_258 ]})

V_2 = Vertex(name = 'V_2',
             particles = [ P.a, P.a, P.H02, P.H02 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_308, C.GC_145, C.GC_266 ]})

V_3 = Vertex(name = 'V_3',
             particles = [ P.a, P.a, P.H03, P.H03 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_308, C.GC_139, C.GC_258 ]})

V_4 = Vertex(name = 'V_4',
             particles = [ P.a, P.a, P.HM1, P.HP1 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):C.GC_308})

V_5 = Vertex(name = 'V_5',
             particles = [ P.a, P.a, P.HL__minus____minus__, P.HL__plus____plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_308, C.GC_139, C.GC_259 ]})

V_6 = Vertex(name = 'V_6',
             particles = [ P.a, P.a, P.HR__minus____minus__, P.HR__plus____plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_308, C.GC_145, C.GC_267 ]})

V_7 = Vertex(name = 'V_7',
             particles = [ P.a, P.a, P.A01, P.A01 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_560, C.GC_563, C.GC_570 ]})

V_8 = Vertex(name = 'V_8',
             particles = [ P.a, P.a, P.H, P.H ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_560, C.GC_563, C.GC_570 ]})

V_9 = Vertex(name = 'V_9',
             particles = [ P.a, P.a, P.H01, P.H01 ],
             color = [ '1' ],
             lorentz = [ L.VVSS1 ],
             couplings = {(0,0):[ C.GC_560, C.GC_563, C.GC_570 ]})

V_10 = Vertex(name = 'V_10',
              particles = [ P.a, P.a, P.HM2, P.HP2 ],
              color = [ '1' ],
              lorentz = [ L.VVSS1 ],
              couplings = {(0,0):[ C.GC_560, C.GC_564, C.GC_570 ]})

V_11 = Vertex(name = 'V_11',
              particles = [ P.a, P.a, P.H ],
              color = [ '1' ],
              lorentz = [ L.VVS1 ],
              couplings = {(0,0):[ C.GC_779, C.GC_780, C.GC_782 ]})

V_12 = Vertex(name = 'V_12',
              particles = [ P.a, P.a, P.H03 ],
              color = [ '1' ],
              lorentz = [ L.VVS1 ],
              couplings = {(0,0):[ C.GC_938, C.GC_909, C.GC_915 ]})

V_13 = Vertex(name = 'V_13',
              particles = [ P.a, P.a, P.H02 ],
              color = [ '1' ],
              lorentz = [ L.VVS1 ],
              couplings = {(0,0):[ C.GC_1056, C.GC_1026, C.GC_1034 ]})

V_14 = Vertex(name = 'V_14',
              particles = [ P.a, P.A01, P.H01 ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):[ C.GC_551, C.GC_555 ]})

V_15 = Vertex(name = 'V_15',
              particles = [ P.a, P.A02, P.H03 ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):[ C.GC_74, C.GC_232 ]})

V_16 = Vertex(name = 'V_16',
              particles = [ P.a, P.HM1, P.HP1 ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):C.GC_231})

V_17 = Vertex(name = 'V_17',
              particles = [ P.a, P.HM2, P.HP2 ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):[ C.GC_552, C.GC_554 ]})

V_18 = Vertex(name = 'V_18',
              particles = [ P.a, P.HL__minus____minus__, P.HL__plus____plus__ ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):[ C.GC_78, C.GC_231 ]})

V_19 = Vertex(name = 'V_19',
              particles = [ P.a, P.HR__minus____minus__, P.HR__plus____plus__ ],
              color = [ '1' ],
              lorentz = [ L.VSS1 ],
              couplings = {(0,0):[ C.GC_94, C.GC_231 ]})

V_20 = Vertex(name = 'V_20',
              particles = [ P.g, P.g, P.g ],
              color = [ 'f(1,2,3)' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):C.GC_1})

V_21 = Vertex(name = 'V_21',
              particles = [ P.g, P.g, P.g, P.g ],
              color = [ 'f(-1,1,2)*f(3,4,-1)', 'f(-1,1,3)*f(2,4,-1)', 'f(-1,1,4)*f(2,3,-1)' ],
              lorentz = [ L.VVVV1, L.VVVV3, L.VVVV4 ],
              couplings = {(1,1):C.GC_3,(0,0):C.GC_3,(2,2):C.GC_3})

V_22 = Vertex(name = 'V_22',
              particles = [ P.d__tilde__, P.u, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_674,(0,1):C.GC_707})

V_23 = Vertex(name = 'V_23',
              particles = [ P.s__tilde__, P.u, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_675,(0,1):C.GC_708})

V_24 = Vertex(name = 'V_24',
              particles = [ P.b__tilde__, P.u, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_676,(0,1):C.GC_709})

V_25 = Vertex(name = 'V_25',
              particles = [ P.d__tilde__, P.c, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_677,(0,1):C.GC_710})

V_26 = Vertex(name = 'V_26',
              particles = [ P.s__tilde__, P.c, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_678,(0,1):C.GC_711})

V_27 = Vertex(name = 'V_27',
              particles = [ P.b__tilde__, P.c, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_679,(0,1):C.GC_712})

V_28 = Vertex(name = 'V_28',
              particles = [ P.d__tilde__, P.t, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_680,(0,1):C.GC_713})

V_29 = Vertex(name = 'V_29',
              particles = [ P.s__tilde__, P.t, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_681,(0,1):C.GC_714})

V_30 = Vertex(name = 'V_30',
              particles = [ P.b__tilde__, P.t, P.HM2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_682,(0,1):C.GC_715})

V_31 = Vertex(name = 'V_31',
              particles = [ P.d__tilde__, P.d, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS2, L.FFS3 ],
              couplings = {(0,0):C.GC_716,(0,1):C.GC_1072,(0,2):C.GC_735})

V_32 = Vertex(name = 'V_32',
              particles = [ P.s__tilde__, P.d, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_718,(0,1):C.GC_737})

V_33 = Vertex(name = 'V_33',
              particles = [ P.b__tilde__, P.d, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_720,(0,1):C.GC_739})

V_34 = Vertex(name = 'V_34',
              particles = [ P.d__tilde__, P.s, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_722,(0,1):C.GC_741})

V_35 = Vertex(name = 'V_35',
              particles = [ P.s__tilde__, P.s, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS2, L.FFS3 ],
              couplings = {(0,0):C.GC_724,(0,1):C.GC_1075,(0,2):C.GC_743})

V_36 = Vertex(name = 'V_36',
              particles = [ P.b__tilde__, P.s, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_726,(0,1):C.GC_745})

V_37 = Vertex(name = 'V_37',
              particles = [ P.d__tilde__, P.b, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_728,(0,1):C.GC_747})

V_38 = Vertex(name = 'V_38',
              particles = [ P.s__tilde__, P.b, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_730,(0,1):C.GC_749})

V_39 = Vertex(name = 'V_39',
              particles = [ P.b__tilde__, P.b, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS2, L.FFS3 ],
              couplings = {(0,0):C.GC_732,(0,1):C.GC_1078,(0,2):C.GC_751})

V_40 = Vertex(name = 'V_40',
              particles = [ P.d__tilde__, P.d, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3, L.FFS4 ],
              couplings = {(0,0):C.GC_717,(0,2):C.GC_1071,(0,1):C.GC_734})

V_41 = Vertex(name = 'V_41',
              particles = [ P.s__tilde__, P.d, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_719,(0,1):C.GC_736})

V_42 = Vertex(name = 'V_42',
              particles = [ P.b__tilde__, P.d, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_721,(0,1):C.GC_738})

V_43 = Vertex(name = 'V_43',
              particles = [ P.d__tilde__, P.s, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_723,(0,1):C.GC_740})

V_44 = Vertex(name = 'V_44',
              particles = [ P.s__tilde__, P.s, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3, L.FFS4 ],
              couplings = {(0,0):C.GC_725,(0,2):C.GC_1074,(0,1):C.GC_742})

V_45 = Vertex(name = 'V_45',
              particles = [ P.b__tilde__, P.s, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_727,(0,1):C.GC_744})

V_46 = Vertex(name = 'V_46',
              particles = [ P.d__tilde__, P.b, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_729,(0,1):C.GC_746})

V_47 = Vertex(name = 'V_47',
              particles = [ P.s__tilde__, P.b, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_731,(0,1):C.GC_748})

V_48 = Vertex(name = 'V_48',
              particles = [ P.b__tilde__, P.b, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3, L.FFS4 ],
              couplings = {(0,0):C.GC_733,(0,2):C.GC_1077,(0,1):C.GC_750})

V_49 = Vertex(name = 'V_49',
              particles = [ P.d__tilde__, P.d, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1073})

V_50 = Vertex(name = 'V_50',
              particles = [ P.s__tilde__, P.s, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1076})

V_51 = Vertex(name = 'V_51',
              particles = [ P.b__tilde__, P.b, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1079})

V_52 = Vertex(name = 'V_52',
              particles = [ P.u__tilde__, P.u, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1099})

V_53 = Vertex(name = 'V_53',
              particles = [ P.c__tilde__, P.c, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1104})

V_54 = Vertex(name = 'V_54',
              particles = [ P.t__tilde__, P.t, P.H ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS4 ],
              couplings = {(0,0):C.GC_1109})

V_55 = Vertex(name = 'V_55',
              particles = [ P.u__tilde__, P.u, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1095,(0,1):C.GC_1098})

V_56 = Vertex(name = 'V_56',
              particles = [ P.c__tilde__, P.u, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_753,(0,1):C.GC_764})

V_57 = Vertex(name = 'V_57',
              particles = [ P.t__tilde__, P.u, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_755,(0,1):C.GC_766})

V_58 = Vertex(name = 'V_58',
              particles = [ P.u__tilde__, P.c, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_757,(0,1):C.GC_768})

V_59 = Vertex(name = 'V_59',
              particles = [ P.c__tilde__, P.c, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1100,(0,1):C.GC_1103})

V_60 = Vertex(name = 'V_60',
              particles = [ P.t__tilde__, P.c, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_759,(0,1):C.GC_770})

V_61 = Vertex(name = 'V_61',
              particles = [ P.u__tilde__, P.t, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_761,(0,1):C.GC_772})

V_62 = Vertex(name = 'V_62',
              particles = [ P.c__tilde__, P.t, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_763,(0,1):C.GC_774})

V_63 = Vertex(name = 'V_63',
              particles = [ P.t__tilde__, P.t, P.A01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1105,(0,1):C.GC_1108})

V_64 = Vertex(name = 'V_64',
              particles = [ P.u__tilde__, P.u, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1096,(0,1):C.GC_1097})

V_65 = Vertex(name = 'V_65',
              particles = [ P.c__tilde__, P.u, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_752,(0,1):C.GC_765})

V_66 = Vertex(name = 'V_66',
              particles = [ P.t__tilde__, P.u, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_754,(0,1):C.GC_767})

V_67 = Vertex(name = 'V_67',
              particles = [ P.u__tilde__, P.c, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_756,(0,1):C.GC_769})

V_68 = Vertex(name = 'V_68',
              particles = [ P.c__tilde__, P.c, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1101,(0,1):C.GC_1102})

V_69 = Vertex(name = 'V_69',
              particles = [ P.t__tilde__, P.c, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_758,(0,1):C.GC_771})

V_70 = Vertex(name = 'V_70',
              particles = [ P.u__tilde__, P.t, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_760,(0,1):C.GC_773})

V_71 = Vertex(name = 'V_71',
              particles = [ P.c__tilde__, P.t, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_762,(0,1):C.GC_775})

V_72 = Vertex(name = 'V_72',
              particles = [ P.t__tilde__, P.t, P.H01 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_1106,(0,1):C.GC_1107})

V_73 = Vertex(name = 'V_73',
              particles = [ P.u__tilde__, P.d, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_656,(0,1):C.GC_665})

V_74 = Vertex(name = 'V_74',
              particles = [ P.c__tilde__, P.d, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_657,(0,1):C.GC_666})

V_75 = Vertex(name = 'V_75',
              particles = [ P.t__tilde__, P.d, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_658,(0,1):C.GC_667})

V_76 = Vertex(name = 'V_76',
              particles = [ P.u__tilde__, P.s, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_659,(0,1):C.GC_668})

V_77 = Vertex(name = 'V_77',
              particles = [ P.c__tilde__, P.s, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_660,(0,1):C.GC_669})

V_78 = Vertex(name = 'V_78',
              particles = [ P.t__tilde__, P.s, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_661,(0,1):C.GC_670})

V_79 = Vertex(name = 'V_79',
              particles = [ P.u__tilde__, P.b, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_662,(0,1):C.GC_671})

V_80 = Vertex(name = 'V_80',
              particles = [ P.c__tilde__, P.b, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_663,(0,1):C.GC_672})

V_81 = Vertex(name = 'V_81',
              particles = [ P.t__tilde__, P.b, P.HP2 ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_664,(0,1):C.GC_673})

V_82 = Vertex(name = 'V_82',
              particles = [ P.v1, P.v1, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_837,(0,1):C.GC_864})

V_83 = Vertex(name = 'V_83',
              particles = [ P.N1, P.v1, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_840,(0,1):C.GC_867})

V_84 = Vertex(name = 'V_84',
              particles = [ P.v2, P.v2, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_838,(0,1):C.GC_865})

V_85 = Vertex(name = 'V_85',
              particles = [ P.N2, P.v2, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_842,(0,1):C.GC_869})

V_86 = Vertex(name = 'V_86',
              particles = [ P.v3, P.v3, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_839,(0,1):C.GC_866})

V_87 = Vertex(name = 'V_87',
              particles = [ P.N3, P.v3, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_844,(0,1):C.GC_871})

V_88 = Vertex(name = 'V_88',
              particles = [ P.N1, P.N1, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_841,(0,1):C.GC_868})

V_89 = Vertex(name = 'V_89',
              particles = [ P.N2, P.N2, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_843,(0,1):C.GC_870})

V_90 = Vertex(name = 'V_90',
              particles = [ P.N3, P.N3, P.H ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_845,(0,1):C.GC_872})

V_91 = Vertex(name = 'V_91',
              particles = [ P.v1, P.v1, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_847,(0,1):C.GC_873})

V_92 = Vertex(name = 'V_92',
              particles = [ P.N1, P.v1, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_853,(0,1):C.GC_879})

V_93 = Vertex(name = 'V_93',
              particles = [ P.v2, P.v2, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_849,(0,1):C.GC_875})

V_94 = Vertex(name = 'V_94',
              particles = [ P.N2, P.v2, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_857,(0,1):C.GC_883})

V_95 = Vertex(name = 'V_95',
              particles = [ P.v3, P.v3, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_851,(0,1):C.GC_877})

V_96 = Vertex(name = 'V_96',
              particles = [ P.N3, P.v3, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_861,(0,1):C.GC_887})

V_97 = Vertex(name = 'V_97',
              particles = [ P.N1, P.N1, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_855,(0,1):C.GC_881})

V_98 = Vertex(name = 'V_98',
              particles = [ P.N2, P.N2, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_859,(0,1):C.GC_885})

V_99 = Vertex(name = 'V_99',
              particles = [ P.N3, P.N3, P.A01 ],
              color = [ '1' ],
              lorentz = [ L.FFS1, L.FFS3 ],
              couplings = {(0,0):C.GC_863,(0,1):C.GC_889})

V_100 = Vertex(name = 'V_100',
               particles = [ P.v1, P.v1, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_846,(0,1):C.GC_874})

V_101 = Vertex(name = 'V_101',
               particles = [ P.N1, P.v1, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_852,(0,1):C.GC_880})

V_102 = Vertex(name = 'V_102',
               particles = [ P.v2, P.v2, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_848,(0,1):C.GC_876})

V_103 = Vertex(name = 'V_103',
               particles = [ P.N2, P.v2, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_856,(0,1):C.GC_884})

V_104 = Vertex(name = 'V_104',
               particles = [ P.v3, P.v3, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_850,(0,1):C.GC_878})

V_105 = Vertex(name = 'V_105',
               particles = [ P.N3, P.v3, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_860,(0,1):C.GC_888})

V_106 = Vertex(name = 'V_106',
               particles = [ P.N1, P.N1, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_854,(0,1):C.GC_882})

V_107 = Vertex(name = 'V_107',
               particles = [ P.N2, P.N2, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_858,(0,1):C.GC_886})

V_108 = Vertex(name = 'V_108',
               particles = [ P.N3, P.N3, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_862,(0,1):C.GC_890})

V_109 = Vertex(name = 'V_109',
               particles = [ P.v1, P.e__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_683,(0,1):C.GC_689})

V_110 = Vertex(name = 'V_110',
               particles = [ P.N1, P.e__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_686,(0,1):C.GC_692})

V_111 = Vertex(name = 'V_111',
               particles = [ P.v2, P.mu__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_684,(0,1):C.GC_690})

V_112 = Vertex(name = 'V_112',
               particles = [ P.N2, P.mu__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_687,(0,1):C.GC_693})

V_113 = Vertex(name = 'V_113',
               particles = [ P.v3, P.ta__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_685,(0,1):C.GC_691})

V_114 = Vertex(name = 'V_114',
               particles = [ P.N3, P.ta__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_688,(0,1):C.GC_694})

V_115 = Vertex(name = 'V_115',
               particles = [ P.e__plus__, P.v1, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_695,(0,1):C.GC_701})

V_116 = Vertex(name = 'V_116',
               particles = [ P.mu__plus__, P.v2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_696,(0,1):C.GC_702})

V_117 = Vertex(name = 'V_117',
               particles = [ P.ta__plus__, P.v3, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_697,(0,1):C.GC_703})

V_118 = Vertex(name = 'V_118',
               particles = [ P.e__plus__, P.N1, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_698,(0,1):C.GC_704})

V_119 = Vertex(name = 'V_119',
               particles = [ P.mu__plus__, P.N2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_699,(0,1):C.GC_705})

V_120 = Vertex(name = 'V_120',
               particles = [ P.ta__plus__, P.N3, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_700,(0,1):C.GC_706})

V_121 = Vertex(name = 'V_121',
               particles = [ P.e__plus__, P.e__minus__, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1084,(0,1):C.GC_1083})

V_122 = Vertex(name = 'V_122',
               particles = [ P.mu__plus__, P.mu__minus__, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1089,(0,1):C.GC_1088})

V_123 = Vertex(name = 'V_123',
               particles = [ P.ta__plus__, P.ta__minus__, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1094,(0,1):C.GC_1093})

V_124 = Vertex(name = 'V_124',
               particles = [ P.e__plus__, P.e__minus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1080,(0,1):C.GC_1081})

V_125 = Vertex(name = 'V_125',
               particles = [ P.mu__plus__, P.mu__minus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1085,(0,1):C.GC_1086})

V_126 = Vertex(name = 'V_126',
               particles = [ P.ta__plus__, P.ta__minus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1090,(0,1):C.GC_1091})

V_127 = Vertex(name = 'V_127',
               particles = [ P.e__plus__, P.e__minus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1082,(0,1):C.GC_1082})

V_128 = Vertex(name = 'V_128',
               particles = [ P.mu__plus__, P.mu__minus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1087,(0,1):C.GC_1087})

V_129 = Vertex(name = 'V_129',
               particles = [ P.ta__plus__, P.ta__minus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1092,(0,1):C.GC_1092})

V_130 = Vertex(name = 'V_130',
               particles = [ P.v1, P.v1, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_945,(0,1):C.GC_954})

V_131 = Vertex(name = 'V_131',
               particles = [ P.N1, P.v1, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_948,(0,1):C.GC_957})

V_132 = Vertex(name = 'V_132',
               particles = [ P.v2, P.v2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_946,(0,1):C.GC_955})

V_133 = Vertex(name = 'V_133',
               particles = [ P.N2, P.v2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_950,(0,1):C.GC_959})

V_134 = Vertex(name = 'V_134',
               particles = [ P.v3, P.v3, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_947,(0,1):C.GC_956})

V_135 = Vertex(name = 'V_135',
               particles = [ P.N3, P.v3, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_952,(0,1):C.GC_961})

V_136 = Vertex(name = 'V_136',
               particles = [ P.N1, P.N1, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_949,(0,1):C.GC_958})

V_137 = Vertex(name = 'V_137',
               particles = [ P.N2, P.N2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_951,(0,1):C.GC_960})

V_138 = Vertex(name = 'V_138',
               particles = [ P.N3, P.N3, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_953,(0,1):C.GC_962})

V_139 = Vertex(name = 'V_139',
               particles = [ P.e__minus__, P.e__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_963})

V_140 = Vertex(name = 'V_140',
               particles = [ P.mu__minus__, P.mu__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_964})

V_141 = Vertex(name = 'V_141',
               particles = [ P.ta__minus__, P.ta__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_965})

V_142 = Vertex(name = 'V_142',
               particles = [ P.v1, P.v1, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_985,(0,1):C.GC_966})

V_143 = Vertex(name = 'V_143',
               particles = [ P.N1, P.v1, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_991,(0,1):C.GC_972})

V_144 = Vertex(name = 'V_144',
               particles = [ P.v2, P.v2, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_987,(0,1):C.GC_968})

V_145 = Vertex(name = 'V_145',
               particles = [ P.N2, P.v2, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_995,(0,1):C.GC_976})

V_146 = Vertex(name = 'V_146',
               particles = [ P.v3, P.v3, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_989,(0,1):C.GC_970})

V_147 = Vertex(name = 'V_147',
               particles = [ P.N3, P.v3, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_999,(0,1):C.GC_980})

V_148 = Vertex(name = 'V_148',
               particles = [ P.N1, P.N1, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_993,(0,1):C.GC_974})

V_149 = Vertex(name = 'V_149',
               particles = [ P.N2, P.N2, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_997,(0,1):C.GC_978})

V_150 = Vertex(name = 'V_150',
               particles = [ P.N3, P.N3, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1001,(0,1):C.GC_982})

V_151 = Vertex(name = 'V_151',
               particles = [ P.v1, P.v1, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_984,(0,1):C.GC_967})

V_152 = Vertex(name = 'V_152',
               particles = [ P.N1, P.v1, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_990,(0,1):C.GC_973})

V_153 = Vertex(name = 'V_153',
               particles = [ P.v2, P.v2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_986,(0,1):C.GC_969})

V_154 = Vertex(name = 'V_154',
               particles = [ P.N2, P.v2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_994,(0,1):C.GC_977})

V_155 = Vertex(name = 'V_155',
               particles = [ P.v3, P.v3, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_988,(0,1):C.GC_971})

V_156 = Vertex(name = 'V_156',
               particles = [ P.N3, P.v3, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_998,(0,1):C.GC_981})

V_157 = Vertex(name = 'V_157',
               particles = [ P.N1, P.N1, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_992,(0,1):C.GC_975})

V_158 = Vertex(name = 'V_158',
               particles = [ P.N2, P.N2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_996,(0,1):C.GC_979})

V_159 = Vertex(name = 'V_159',
               particles = [ P.N3, P.N3, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.FFS1, L.FFS3 ],
               couplings = {(0,0):C.GC_1000,(0,1):C.GC_983})

V_160 = Vertex(name = 'V_160',
               particles = [ P.v1, P.e__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1002})

V_161 = Vertex(name = 'V_161',
               particles = [ P.N1, P.e__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1005})

V_162 = Vertex(name = 'V_162',
               particles = [ P.v2, P.mu__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1003})

V_163 = Vertex(name = 'V_163',
               particles = [ P.N2, P.mu__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1006})

V_164 = Vertex(name = 'V_164',
               particles = [ P.v3, P.ta__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1004})

V_165 = Vertex(name = 'V_165',
               particles = [ P.N3, P.ta__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1007})

V_166 = Vertex(name = 'V_166',
               particles = [ P.e__minus__, P.e__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1008})

V_167 = Vertex(name = 'V_167',
               particles = [ P.mu__minus__, P.mu__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1009})

V_168 = Vertex(name = 'V_168',
               particles = [ P.ta__minus__, P.ta__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1010})

V_169 = Vertex(name = 'V_169',
               particles = [ P.a, P.W__minus__, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_90, C.GC_234 ]})

V_170 = Vertex(name = 'V_170',
               particles = [ P.a, P.W__minus__, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_89, C.GC_235 ]})

V_171 = Vertex(name = 'V_171',
               particles = [ P.a, P.W__minus__, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_87, C.GC_233 ]})

V_172 = Vertex(name = 'V_172',
               particles = [ P.a, P.W__minus__, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_556, C.GC_830 ]})

V_173 = Vertex(name = 'V_173',
               particles = [ P.a, P.W__minus__, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_558, C.GC_831 ]})

V_174 = Vertex(name = 'V_174',
               particles = [ P.a, P.W__minus__, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_626})

V_175 = Vertex(name = 'V_175',
               particles = [ P.a, P.W__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_804})

V_176 = Vertex(name = 'V_176',
               particles = [ P.a, P.W__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_901, C.GC_913 ]})

V_177 = Vertex(name = 'V_177',
               particles = [ P.W__minus__, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_541, C.GC_822 ]})

V_178 = Vertex(name = 'V_178',
               particles = [ P.W__minus__, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_11})

V_179 = Vertex(name = 'V_179',
               particles = [ P.W__minus__, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_619})

V_180 = Vertex(name = 'V_180',
               particles = [ P.W__minus__, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_539, C.GC_821 ]})

V_181 = Vertex(name = 'V_181',
               particles = [ P.W__minus__, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_9})

V_182 = Vertex(name = 'V_182',
               particles = [ P.W__minus__, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_8})

V_183 = Vertex(name = 'V_183',
               particles = [ P.a, P.W__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_449, C.GC_459 ]})

V_184 = Vertex(name = 'V_184',
               particles = [ P.a, P.W__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_79, C.GC_503 ]})

V_185 = Vertex(name = 'V_185',
               particles = [ P.W__minus__, P.W__minus__, P.A02, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_31})

V_186 = Vertex(name = 'V_186',
               particles = [ P.W__minus__, P.W__minus__, P.H03, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_30})

V_187 = Vertex(name = 'V_187',
               particles = [ P.W__minus__, P.W__minus__, P.H02, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_499})

V_188 = Vertex(name = 'V_188',
               particles = [ P.W__minus__, P.W__minus__, P.HP2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_826})

V_189 = Vertex(name = 'V_189',
               particles = [ P.W__minus__, P.W__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_894})

V_190 = Vertex(name = 'V_190',
               particles = [ P.W__minus__, P.W__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1055})

V_191 = Vertex(name = 'V_191',
               particles = [ P.a, P.W2__minus__, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_457, C.GC_474 ]})

V_192 = Vertex(name = 'V_192',
               particles = [ P.a, P.W2__minus__, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_456, C.GC_475 ]})

V_193 = Vertex(name = 'V_193',
               particles = [ P.a, P.W2__minus__, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_454, C.GC_473 ]})

V_194 = Vertex(name = 'V_194',
               particles = [ P.a, P.W2__minus__, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_557})

V_195 = Vertex(name = 'V_195',
               particles = [ P.a, P.W2__minus__, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_819, C.GC_625 ]})

V_196 = Vertex(name = 'V_196',
               particles = [ P.a, P.W2__minus__, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_818, C.GC_627 ]})

V_197 = Vertex(name = 'V_197',
               particles = [ P.a, P.W2__minus__, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_778})

V_198 = Vertex(name = 'V_198',
               particles = [ P.a, P.W2__minus__, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_931, C.GC_933 ]})

V_199 = Vertex(name = 'V_199',
               particles = [ P.W2__minus__, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_614})

V_200 = Vertex(name = 'V_200',
               particles = [ P.W2__minus__, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_399})

V_201 = Vertex(name = 'V_201',
               particles = [ P.W2__minus__, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_544})

V_202 = Vertex(name = 'V_202',
               particles = [ P.W2__minus__, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_612})

V_203 = Vertex(name = 'V_203',
               particles = [ P.W2__minus__, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_397})

V_204 = Vertex(name = 'V_204',
               particles = [ P.W2__minus__, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_396})

V_205 = Vertex(name = 'V_205',
               particles = [ P.a, P.W2__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_95, C.GC_502 ]})

V_206 = Vertex(name = 'V_206',
               particles = [ P.a, P.W2__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_449, C.GC_459 ]})

V_207 = Vertex(name = 'V_207',
               particles = [ P.W__minus__, P.W2__minus__, P.A02, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_418})

V_208 = Vertex(name = 'V_208',
               particles = [ P.W__minus__, P.W2__minus__, P.H03, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_417})

V_209 = Vertex(name = 'V_209',
               particles = [ P.W__minus__, P.W2__minus__, P.H02, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_431})

V_210 = Vertex(name = 'V_210',
               particles = [ P.W__minus__, P.W2__minus__, P.HP2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_815, C.GC_834 ]})

V_211 = Vertex(name = 'V_211',
               particles = [ P.W__minus__, P.W2__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_928})

V_212 = Vertex(name = 'V_212',
               particles = [ P.W__minus__, P.W2__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1053})

V_213 = Vertex(name = 'V_213',
               particles = [ P.W2__minus__, P.W2__minus__, P.H02, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_44})

V_214 = Vertex(name = 'V_214',
               particles = [ P.W2__minus__, P.W2__minus__, P.A02, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_496})

V_215 = Vertex(name = 'V_215',
               particles = [ P.W2__minus__, P.W2__minus__, P.H03, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_495})

V_216 = Vertex(name = 'V_216',
               particles = [ P.W2__minus__, P.W2__minus__, P.HP2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_825})

V_217 = Vertex(name = 'V_217',
               particles = [ P.W2__minus__, P.W2__minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_937})

V_218 = Vertex(name = 'V_218',
               particles = [ P.W2__minus__, P.W2__minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1024})

V_219 = Vertex(name = 'V_219',
               particles = [ P.a, P.W2__plus__, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_455, C.GC_476 ]})

V_220 = Vertex(name = 'V_220',
               particles = [ P.a, P.W2__plus__, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_456, C.GC_475 ]})

V_221 = Vertex(name = 'V_221',
               particles = [ P.a, P.W2__plus__, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_454, C.GC_473 ]})

V_222 = Vertex(name = 'V_222',
               particles = [ P.a, P.W2__plus__, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_557})

V_223 = Vertex(name = 'V_223',
               particles = [ P.a, P.W2__plus__, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_817, C.GC_628 ]})

V_224 = Vertex(name = 'V_224',
               particles = [ P.a, P.W2__plus__, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_818, C.GC_627 ]})

V_225 = Vertex(name = 'V_225',
               particles = [ P.a, P.W2__plus__, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_778})

V_226 = Vertex(name = 'V_226',
               particles = [ P.a, P.W2__plus__, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_931, C.GC_933 ]})

V_227 = Vertex(name = 'V_227',
               particles = [ P.W2__plus__, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_614})

V_228 = Vertex(name = 'V_228',
               particles = [ P.W2__plus__, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_399})

V_229 = Vertex(name = 'V_229',
               particles = [ P.W2__plus__, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_545})

V_230 = Vertex(name = 'V_230',
               particles = [ P.W2__plus__, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_613})

V_231 = Vertex(name = 'V_231',
               particles = [ P.W2__plus__, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_398})

V_232 = Vertex(name = 'V_232',
               particles = [ P.W2__plus__, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_395})

V_233 = Vertex(name = 'V_233',
               particles = [ P.W__minus__, P.W2__plus__, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_234 = Vertex(name = 'V_234',
               particles = [ P.W__minus__, P.W2__plus__, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_430})

V_235 = Vertex(name = 'V_235',
               particles = [ P.W__minus__, P.W2__plus__, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_236 = Vertex(name = 'V_236',
               particles = [ P.W__minus__, P.W2__plus__, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_415})

V_237 = Vertex(name = 'V_237',
               particles = [ P.W__minus__, P.W2__plus__, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_238 = Vertex(name = 'V_238',
               particles = [ P.W__minus__, P.W2__plus__, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_430})

V_239 = Vertex(name = 'V_239',
               particles = [ P.W__minus__, P.W2__plus__, P.A01, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_547, C.GC_646 ]})

V_240 = Vertex(name = 'V_240',
               particles = [ P.W__minus__, P.W2__plus__, P.H, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_548, C.GC_647 ]})

V_241 = Vertex(name = 'V_241',
               particles = [ P.W__minus__, P.W2__plus__, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_616, C.GC_642 ]})

V_242 = Vertex(name = 'V_242',
               particles = [ P.W__minus__, P.W2__plus__, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_616, C.GC_642 ]})

V_243 = Vertex(name = 'V_243',
               particles = [ P.W__minus__, P.W2__plus__, P.A01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_814, C.GC_833 ]})

V_244 = Vertex(name = 'V_244',
               particles = [ P.W__minus__, P.W2__plus__, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_617, C.GC_641 ]})

V_245 = Vertex(name = 'V_245',
               particles = [ P.W__minus__, P.W2__plus__, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_615, C.GC_623 ]})

V_246 = Vertex(name = 'V_246',
               particles = [ P.W__minus__, P.W2__plus__, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_653, C.GC_807 ]})

V_247 = Vertex(name = 'V_247',
               particles = [ P.W__minus__, P.W2__plus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_654, C.GC_808 ]})

V_248 = Vertex(name = 'V_248',
               particles = [ P.W__minus__, P.W2__plus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_799, C.GC_805 ]})

V_249 = Vertex(name = 'V_249',
               particles = [ P.W__minus__, P.W2__plus__, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_927})

V_250 = Vertex(name = 'V_250',
               particles = [ P.W__minus__, P.W2__plus__, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1052})

V_251 = Vertex(name = 'V_251',
               particles = [ P.a, P.a, P.W__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_465, C.GC_467 ]})

V_252 = Vertex(name = 'V_252',
               particles = [ P.W__minus__, P.W2__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_409, C.GC_466, C.GC_477 ]})

V_253 = Vertex(name = 'V_253',
               particles = [ P.W__minus__, P.W2__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_525, C.GC_468 ]})

V_254 = Vertex(name = 'V_254',
               particles = [ P.W2__minus__, P.W2__plus__, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_43})

V_255 = Vertex(name = 'V_255',
               particles = [ P.W2__minus__, P.W2__plus__, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_43})

V_256 = Vertex(name = 'V_256',
               particles = [ P.W2__minus__, P.W2__plus__, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_492})

V_257 = Vertex(name = 'V_257',
               particles = [ P.W2__minus__, P.W2__plus__, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_492})

V_258 = Vertex(name = 'V_258',
               particles = [ P.W2__minus__, P.W2__plus__, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_493})

V_259 = Vertex(name = 'V_259',
               particles = [ P.W2__minus__, P.W2__plus__, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_492})

V_260 = Vertex(name = 'V_260',
               particles = [ P.W2__minus__, P.W2__plus__, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_644})

V_261 = Vertex(name = 'V_261',
               particles = [ P.W2__minus__, P.W2__plus__, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_644})

V_262 = Vertex(name = 'V_262',
               particles = [ P.W2__minus__, P.W2__plus__, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_645})

V_263 = Vertex(name = 'V_263',
               particles = [ P.W2__minus__, P.W2__plus__, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_643})

V_264 = Vertex(name = 'V_264',
               particles = [ P.W2__minus__, P.W2__plus__, P.H, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_622})

V_265 = Vertex(name = 'V_265',
               particles = [ P.W2__minus__, P.W2__plus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_806})

V_266 = Vertex(name = 'V_266',
               particles = [ P.W2__minus__, P.W2__plus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_802})

V_267 = Vertex(name = 'V_267',
               particles = [ P.W2__minus__, P.W2__plus__, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_936})

V_268 = Vertex(name = 'V_268',
               particles = [ P.W2__minus__, P.W2__plus__, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1023})

V_269 = Vertex(name = 'V_269',
               particles = [ P.a, P.a, P.W2__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):C.GC_532})

V_270 = Vertex(name = 'V_270',
               particles = [ P.W2__minus__, P.W2__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_144, C.GC_526 ]})

V_271 = Vertex(name = 'V_271',
               particles = [ P.W2__minus__, P.W2__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_163, C.GC_238, C.GC_501 ]})

V_272 = Vertex(name = 'V_272',
               particles = [ P.W2__plus__, P.W2__plus__, P.H02, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_44})

V_273 = Vertex(name = 'V_273',
               particles = [ P.W2__plus__, P.W2__plus__, P.A02, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_494})

V_274 = Vertex(name = 'V_274',
               particles = [ P.W2__plus__, P.W2__plus__, P.H03, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_495})

V_275 = Vertex(name = 'V_275',
               particles = [ P.W2__plus__, P.W2__plus__, P.HM2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_825})

V_276 = Vertex(name = 'V_276',
               particles = [ P.W2__plus__, P.W2__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_937})

V_277 = Vertex(name = 'V_277',
               particles = [ P.W2__plus__, P.W2__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1024})

V_278 = Vertex(name = 'V_278',
               particles = [ P.W__minus__, P.W__minus__, P.W2__plus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_497, C.GC_500 ]})

V_279 = Vertex(name = 'V_279',
               particles = [ P.W__minus__, P.W2__minus__, P.W2__plus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):C.GC_536})

V_280 = Vertex(name = 'V_280',
               particles = [ P.W2__minus__, P.W2__minus__, P.W2__plus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_45, C.GC_520 ]})

V_281 = Vertex(name = 'V_281',
               particles = [ P.a, P.W__plus__, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_88, C.GC_236 ]})

V_282 = Vertex(name = 'V_282',
               particles = [ P.a, P.W__plus__, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_89, C.GC_235 ]})

V_283 = Vertex(name = 'V_283',
               particles = [ P.a, P.W__plus__, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_87, C.GC_233 ]})

V_284 = Vertex(name = 'V_284',
               particles = [ P.a, P.W__plus__, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_559, C.GC_832 ]})

V_285 = Vertex(name = 'V_285',
               particles = [ P.a, P.W__plus__, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_558, C.GC_831 ]})

V_286 = Vertex(name = 'V_286',
               particles = [ P.a, P.W__plus__, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_626})

V_287 = Vertex(name = 'V_287',
               particles = [ P.a, P.W__plus__, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_804})

V_288 = Vertex(name = 'V_288',
               particles = [ P.a, P.W__plus__, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_901, C.GC_913 ]})

V_289 = Vertex(name = 'V_289',
               particles = [ P.W__plus__, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_541, C.GC_822 ]})

V_290 = Vertex(name = 'V_290',
               particles = [ P.W__plus__, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_11})

V_291 = Vertex(name = 'V_291',
               particles = [ P.W__plus__, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_618})

V_292 = Vertex(name = 'V_292',
               particles = [ P.W__plus__, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_540, C.GC_820 ]})

V_293 = Vertex(name = 'V_293',
               particles = [ P.W__plus__, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_10})

V_294 = Vertex(name = 'V_294',
               particles = [ P.W__plus__, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):C.GC_7})

V_295 = Vertex(name = 'V_295',
               particles = [ P.W__minus__, P.W__plus__, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_27})

V_296 = Vertex(name = 'V_296',
               particles = [ P.W__minus__, P.W__plus__, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_27})

V_297 = Vertex(name = 'V_297',
               particles = [ P.W__minus__, P.W__plus__, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_28})

V_298 = Vertex(name = 'V_298',
               particles = [ P.W__minus__, P.W__plus__, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_27})

V_299 = Vertex(name = 'V_299',
               particles = [ P.W__minus__, P.W__plus__, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_498})

V_300 = Vertex(name = 'V_300',
               particles = [ P.W__minus__, P.W__plus__, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_498})

V_301 = Vertex(name = 'V_301',
               particles = [ P.W__minus__, P.W__plus__, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_543, C.GC_824, C.GC_649 ]})

V_302 = Vertex(name = 'V_302',
               particles = [ P.W__minus__, P.W__plus__, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_543, C.GC_824, C.GC_649 ]})

V_303 = Vertex(name = 'V_303',
               particles = [ P.W__minus__, P.W__plus__, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_543, C.GC_823, C.GC_649 ]})

V_304 = Vertex(name = 'V_304',
               particles = [ P.W__minus__, P.W__plus__, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_543, C.GC_649 ]})

V_305 = Vertex(name = 'V_305',
               particles = [ P.W__minus__, P.W__plus__, P.H, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_621})

V_306 = Vertex(name = 'V_306',
               particles = [ P.W__minus__, P.W__plus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_651, C.GC_836, C.GC_810 ]})

V_307 = Vertex(name = 'V_307',
               particles = [ P.W__minus__, P.W__plus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_801})

V_308 = Vertex(name = 'V_308',
               particles = [ P.W__minus__, P.W__plus__, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_893})

V_309 = Vertex(name = 'V_309',
               particles = [ P.W__minus__, P.W__plus__, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1054})

V_310 = Vertex(name = 'V_310',
               particles = [ P.a, P.a, P.W__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_140, C.GC_505 ]})

V_311 = Vertex(name = 'V_311',
               particles = [ P.W__minus__, P.W__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_21, C.GC_504, C.GC_512 ]})

V_312 = Vertex(name = 'V_312',
               particles = [ P.W__minus__, P.W__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_61, C.GC_506, C.GC_511 ]})

V_313 = Vertex(name = 'V_313',
               particles = [ P.W2__minus__, P.W__plus__, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_314 = Vertex(name = 'V_314',
               particles = [ P.W2__minus__, P.W__plus__, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_430})

V_315 = Vertex(name = 'V_315',
               particles = [ P.W2__minus__, P.W__plus__, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_316 = Vertex(name = 'V_316',
               particles = [ P.W2__minus__, P.W__plus__, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_415})

V_317 = Vertex(name = 'V_317',
               particles = [ P.W2__minus__, P.W__plus__, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_414})

V_318 = Vertex(name = 'V_318',
               particles = [ P.W2__minus__, P.W__plus__, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_430})

V_319 = Vertex(name = 'V_319',
               particles = [ P.W2__minus__, P.W__plus__, P.A01, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_549, C.GC_648 ]})

V_320 = Vertex(name = 'V_320',
               particles = [ P.W2__minus__, P.W__plus__, P.H, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_548, C.GC_647 ]})

V_321 = Vertex(name = 'V_321',
               particles = [ P.W2__minus__, P.W__plus__, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_616, C.GC_642 ]})

V_322 = Vertex(name = 'V_322',
               particles = [ P.W2__minus__, P.W__plus__, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_616, C.GC_642 ]})

V_323 = Vertex(name = 'V_323',
               particles = [ P.W2__minus__, P.W__plus__, P.A01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_816, C.GC_835 ]})

V_324 = Vertex(name = 'V_324',
               particles = [ P.W2__minus__, P.W__plus__, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_617, C.GC_641 ]})

V_325 = Vertex(name = 'V_325',
               particles = [ P.W2__minus__, P.W__plus__, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_615, C.GC_623 ]})

V_326 = Vertex(name = 'V_326',
               particles = [ P.W2__minus__, P.W__plus__, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_655, C.GC_809 ]})

V_327 = Vertex(name = 'V_327',
               particles = [ P.W2__minus__, P.W__plus__, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_654, C.GC_808 ]})

V_328 = Vertex(name = 'V_328',
               particles = [ P.W2__minus__, P.W__plus__, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_799, C.GC_805 ]})

V_329 = Vertex(name = 'V_329',
               particles = [ P.W2__minus__, P.W__plus__, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_927})

V_330 = Vertex(name = 'V_330',
               particles = [ P.W2__minus__, P.W__plus__, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1052})

V_331 = Vertex(name = 'V_331',
               particles = [ P.a, P.a, P.W2__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_465, C.GC_467 ]})

V_332 = Vertex(name = 'V_332',
               particles = [ P.W2__minus__, P.W__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_409, C.GC_466, C.GC_477 ]})

V_333 = Vertex(name = 'V_333',
               particles = [ P.W2__minus__, P.W__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVV1 ],
               couplings = {(0,0):[ C.GC_525, C.GC_468 ]})

V_334 = Vertex(name = 'V_334',
               particles = [ P.W2__plus__, P.W__plus__, P.A02, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_416})

V_335 = Vertex(name = 'V_335',
               particles = [ P.W2__plus__, P.W__plus__, P.H03, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_417})

V_336 = Vertex(name = 'V_336',
               particles = [ P.W2__plus__, P.W__plus__, P.H02, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_431})

V_337 = Vertex(name = 'V_337',
               particles = [ P.W2__plus__, P.W__plus__, P.HM2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_815, C.GC_834 ]})

V_338 = Vertex(name = 'V_338',
               particles = [ P.W2__plus__, P.W__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_928})

V_339 = Vertex(name = 'V_339',
               particles = [ P.W2__plus__, P.W__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1053})

V_340 = Vertex(name = 'V_340',
               particles = [ P.W__minus__, P.W__minus__, P.W2__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_420, C.GC_519 ]})

V_341 = Vertex(name = 'V_341',
               particles = [ P.W__minus__, P.W2__minus__, P.W2__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_497, C.GC_500 ]})

V_342 = Vertex(name = 'V_342',
               particles = [ P.W2__minus__, P.W2__minus__, P.W2__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):C.GC_536})

V_343 = Vertex(name = 'V_343',
               particles = [ P.W__plus__, P.W__plus__, P.A02, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_29})

V_344 = Vertex(name = 'V_344',
               particles = [ P.W__plus__, P.W__plus__, P.H03, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_30})

V_345 = Vertex(name = 'V_345',
               particles = [ P.W__plus__, P.W__plus__, P.H02, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_499})

V_346 = Vertex(name = 'V_346',
               particles = [ P.W__plus__, P.W__plus__, P.HM2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_826})

V_347 = Vertex(name = 'V_347',
               particles = [ P.W__plus__, P.W__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_894})

V_348 = Vertex(name = 'V_348',
               particles = [ P.W__plus__, P.W__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_1055})

V_349 = Vertex(name = 'V_349',
               particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_33, C.GC_521 ]})

V_350 = Vertex(name = 'V_350',
               particles = [ P.W__minus__, P.W2__minus__, P.W__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_420, C.GC_519 ]})

V_351 = Vertex(name = 'V_351',
               particles = [ P.W2__minus__, P.W2__minus__, P.W__plus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_497, C.GC_500 ]})

V_352 = Vertex(name = 'V_352',
               particles = [ P.e__plus__, P.e__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1011})

V_353 = Vertex(name = 'V_353',
               particles = [ P.mu__plus__, P.mu__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1012})

V_354 = Vertex(name = 'V_354',
               particles = [ P.ta__plus__, P.ta__plus__, P.HR__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS3 ],
               couplings = {(0,0):C.GC_1013})

V_355 = Vertex(name = 'V_355',
               particles = [ P.e__plus__, P.v1, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1014})

V_356 = Vertex(name = 'V_356',
               particles = [ P.mu__plus__, P.v2, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1015})

V_357 = Vertex(name = 'V_357',
               particles = [ P.ta__plus__, P.v3, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1016})

V_358 = Vertex(name = 'V_358',
               particles = [ P.e__plus__, P.N1, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1017})

V_359 = Vertex(name = 'V_359',
               particles = [ P.mu__plus__, P.N2, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1018})

V_360 = Vertex(name = 'V_360',
               particles = [ P.ta__plus__, P.N3, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1019})

V_361 = Vertex(name = 'V_361',
               particles = [ P.e__plus__, P.e__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1020})

V_362 = Vertex(name = 'V_362',
               particles = [ P.mu__plus__, P.mu__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1021})

V_363 = Vertex(name = 'V_363',
               particles = [ P.ta__plus__, P.ta__plus__, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFS1 ],
               couplings = {(0,0):C.GC_1022})

V_364 = Vertex(name = 'V_364',
               particles = [ P.a, P.Z, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_341, C.GC_351, C.GC_177, C.GC_275, C.GC_282, C.GC_290 ]})

V_365 = Vertex(name = 'V_365',
               particles = [ P.a, P.Z, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_342, C.GC_351, C.GC_177, C.GC_275, C.GC_282, C.GC_289 ]})

V_366 = Vertex(name = 'V_366',
               particles = [ P.a, P.Z, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_351, C.GC_86, C.GC_158, C.GC_390, C.GC_275 ]})

V_367 = Vertex(name = 'V_367',
               particles = [ P.a, P.Z, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_351, C.GC_86, C.GC_158, C.GC_390, C.GC_275 ]})

V_368 = Vertex(name = 'V_368',
               particles = [ P.a, P.Z, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_351, C.GC_275 ]})

V_369 = Vertex(name = 'V_369',
               particles = [ P.a, P.Z, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_351, C.GC_86, C.GC_159, C.GC_389, C.GC_275 ]})

V_370 = Vertex(name = 'V_370',
               particles = [ P.a, P.Z, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_553, C.GC_578, C.GC_601, C.GC_602 ]})

V_371 = Vertex(name = 'V_371',
               particles = [ P.a, P.Z, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_553, C.GC_578, C.GC_601, C.GC_602 ]})

V_372 = Vertex(name = 'V_372',
               particles = [ P.a, P.Z, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_553, C.GC_578, C.GC_601, C.GC_602 ]})

V_373 = Vertex(name = 'V_373',
               particles = [ P.a, P.Z, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_553, C.GC_577, C.GC_600, C.GC_602 ]})

V_374 = Vertex(name = 'V_374',
               particles = [ P.a, P.Z, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_777, C.GC_784, C.GC_793, C.GC_794 ]})

V_375 = Vertex(name = 'V_375',
               particles = [ P.a, P.Z, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_939, C.GC_900, C.GC_910, C.GC_943, C.GC_918 ]})

V_376 = Vertex(name = 'V_376',
               particles = [ P.a, P.Z, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_1059, C.GC_1061, C.GC_1028, C.GC_1037, C.GC_1038, C.GC_1041 ]})

V_377 = Vertex(name = 'V_377',
               particles = [ P.Z, P.A01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_537, C.GC_561, C.GC_588 ]})

V_378 = Vertex(name = 'V_378',
               particles = [ P.Z, P.A02, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_4, C.GC_123, C.GC_254 ]})

V_379 = Vertex(name = 'V_379',
               particles = [ P.Z, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_122, C.GC_256 ]})

V_380 = Vertex(name = 'V_380',
               particles = [ P.Z, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_538, C.GC_562, C.GC_589 ]})

V_381 = Vertex(name = 'V_381',
               particles = [ P.Z, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_6, C.GC_122, C.GC_256 ]})

V_382 = Vertex(name = 'V_382',
               particles = [ P.Z, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_122, C.GC_143, C.GC_241, C.GC_256 ]})

V_383 = Vertex(name = 'V_383',
               particles = [ P.W__minus__, P.Z, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_26, C.GC_127, C.GC_265 ]})

V_384 = Vertex(name = 'V_384',
               particles = [ P.W__minus__, P.Z, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_25, C.GC_128, C.GC_264 ]})

V_385 = Vertex(name = 'V_385',
               particles = [ P.W__minus__, P.Z, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_23, C.GC_126, C.GC_262 ]})

V_386 = Vertex(name = 'V_386',
               particles = [ P.W__minus__, P.Z, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_569, C.GC_609 ]})

V_387 = Vertex(name = 'V_387',
               particles = [ P.W__minus__, P.Z, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_568, C.GC_610 ]})

V_388 = Vertex(name = 'V_388',
               particles = [ P.W__minus__, P.Z, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_620})

V_389 = Vertex(name = 'V_389',
               particles = [ P.W__minus__, P.Z, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_800})

V_390 = Vertex(name = 'V_390',
               particles = [ P.W__minus__, P.Z, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_892, C.GC_904, C.GC_917 ]})

V_391 = Vertex(name = 'V_391',
               particles = [ P.W2__minus__, P.Z, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_413, C.GC_461, C.GC_481 ]})

V_392 = Vertex(name = 'V_392',
               particles = [ P.W2__minus__, P.Z, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_412, C.GC_462, C.GC_480 ]})

V_393 = Vertex(name = 'V_393',
               particles = [ P.W2__minus__, P.Z, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_410, C.GC_460, C.GC_478 ]})

V_394 = Vertex(name = 'V_394',
               particles = [ P.W2__minus__, P.Z, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_546})

V_395 = Vertex(name = 'V_395',
               particles = [ P.W2__minus__, P.Z, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_813, C.GC_631, C.GC_640 ]})

V_396 = Vertex(name = 'V_396',
               particles = [ P.W2__minus__, P.Z, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_812, C.GC_630, C.GC_639 ]})

V_397 = Vertex(name = 'V_397',
               particles = [ P.W2__minus__, P.Z, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_652})

V_398 = Vertex(name = 'V_398',
               particles = [ P.W2__minus__, P.Z, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_926, C.GC_932, C.GC_934 ]})

V_399 = Vertex(name = 'V_399',
               particles = [ P.W2__plus__, P.Z, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_411, C.GC_463, C.GC_479 ]})

V_400 = Vertex(name = 'V_400',
               particles = [ P.W2__plus__, P.Z, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_412, C.GC_462, C.GC_480 ]})

V_401 = Vertex(name = 'V_401',
               particles = [ P.W2__plus__, P.Z, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_410, C.GC_460, C.GC_478 ]})

V_402 = Vertex(name = 'V_402',
               particles = [ P.W2__plus__, P.Z, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_546})

V_403 = Vertex(name = 'V_403',
               particles = [ P.W2__plus__, P.Z, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_811, C.GC_629, C.GC_638 ]})

V_404 = Vertex(name = 'V_404',
               particles = [ P.W2__plus__, P.Z, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_812, C.GC_630, C.GC_639 ]})

V_405 = Vertex(name = 'V_405',
               particles = [ P.W2__plus__, P.Z, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_652})

V_406 = Vertex(name = 'V_406',
               particles = [ P.W2__plus__, P.Z, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_926, C.GC_932, C.GC_934 ]})

V_407 = Vertex(name = 'V_407',
               particles = [ P.a, P.W__minus__, P.W2__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_458, C.GC_469, C.GC_487 ]})

V_408 = Vertex(name = 'V_408',
               particles = [ P.a, P.W2__minus__, P.W2__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_530, C.GC_283 ]})

V_409 = Vertex(name = 'V_409',
               particles = [ P.W__plus__, P.Z, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_24, C.GC_129, C.GC_263 ]})

V_410 = Vertex(name = 'V_410',
               particles = [ P.W__plus__, P.Z, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_25, C.GC_128, C.GC_264 ]})

V_411 = Vertex(name = 'V_411',
               particles = [ P.W__plus__, P.Z, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_23, C.GC_126, C.GC_262 ]})

V_412 = Vertex(name = 'V_412',
               particles = [ P.W__plus__, P.Z, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_567, C.GC_611 ]})

V_413 = Vertex(name = 'V_413',
               particles = [ P.W__plus__, P.Z, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_568, C.GC_610 ]})

V_414 = Vertex(name = 'V_414',
               particles = [ P.W__plus__, P.Z, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_620})

V_415 = Vertex(name = 'V_415',
               particles = [ P.W__plus__, P.Z, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_800})

V_416 = Vertex(name = 'V_416',
               particles = [ P.W__plus__, P.Z, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_892, C.GC_904, C.GC_917 ]})

V_417 = Vertex(name = 'V_417',
               particles = [ P.a, P.W__minus__, P.W__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_91, C.GC_507, C.GC_514 ]})

V_418 = Vertex(name = 'V_418',
               particles = [ P.a, P.W2__minus__, P.W__plus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_458, C.GC_469, C.GC_487 ]})

V_419 = Vertex(name = 'V_419',
               particles = [ P.Z, P.Z, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_22, C.GC_124, C.GC_359, C.GC_176, C.GC_261, C.GC_293 ]})

V_420 = Vertex(name = 'V_420',
               particles = [ P.Z, P.Z, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_22, C.GC_124, C.GC_359, C.GC_176, C.GC_261, C.GC_293 ]})

V_421 = Vertex(name = 'V_421',
               particles = [ P.Z, P.Z, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_22, C.GC_125, C.GC_359, C.GC_176, C.GC_260, C.GC_293 ]})

V_422 = Vertex(name = 'V_422',
               particles = [ P.Z, P.Z, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_349, C.GC_356, C.GC_359, C.GC_176, C.GC_179, C.GC_287, C.GC_293, C.GC_298, C.GC_304 ]})

V_423 = Vertex(name = 'V_423',
               particles = [ P.Z, P.Z, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_349, C.GC_355, C.GC_359, C.GC_176, C.GC_179, C.GC_286, C.GC_293, C.GC_298, C.GC_305 ]})

V_424 = Vertex(name = 'V_424',
               particles = [ P.Z, P.Z, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_359, C.GC_176, C.GC_293 ]})

V_425 = Vertex(name = 'V_425',
               particles = [ P.Z, P.Z, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_542, C.GC_576, C.GC_566, C.GC_593, C.GC_607 ]})

V_426 = Vertex(name = 'V_426',
               particles = [ P.Z, P.Z, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_542, C.GC_576, C.GC_566, C.GC_593, C.GC_607 ]})

V_427 = Vertex(name = 'V_427',
               particles = [ P.Z, P.Z, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_542, C.GC_576, C.GC_566, C.GC_593, C.GC_607 ]})

V_428 = Vertex(name = 'V_428',
               particles = [ P.Z, P.Z, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_542, C.GC_576, C.GC_565, C.GC_591, C.GC_607 ]})

V_429 = Vertex(name = 'V_429',
               particles = [ P.Z, P.Z, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_650, C.GC_783, C.GC_781, C.GC_789, C.GC_797 ]})

V_430 = Vertex(name = 'V_430',
               particles = [ P.Z, P.Z, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_891, C.GC_902, C.GC_941, C.GC_911, C.GC_916, C.GC_923 ]})

V_431 = Vertex(name = 'V_431',
               particles = [ P.Z, P.Z, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_1060, C.GC_1065, C.GC_1067, C.GC_1027, C.GC_1030, C.GC_1039, C.GC_1043, C.GC_1047, C.GC_1050 ]})

V_432 = Vertex(name = 'V_432',
               particles = [ P.W__minus__, P.W2__plus__, P.Z, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_419, C.GC_523, C.GC_471, C.GC_489 ]})

V_433 = Vertex(name = 'V_433',
               particles = [ P.W2__minus__, P.W2__plus__, P.Z, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_350, C.GC_527, C.GC_300 ]})

V_434 = Vertex(name = 'V_434',
               particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_32, C.GC_534, C.GC_509, C.GC_517 ]})

V_435 = Vertex(name = 'V_435',
               particles = [ P.W2__minus__, P.W__plus__, P.Z, P.Z ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_419, C.GC_523, C.GC_471, C.GC_489 ]})

V_436 = Vertex(name = 'V_436',
               particles = [ P.a, P.Z2, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_312, C.GC_354, C.GC_178, C.GC_257, C.GC_270, C.GC_296 ]})

V_437 = Vertex(name = 'V_437',
               particles = [ P.a, P.Z2, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_311, C.GC_354, C.GC_178, C.GC_257, C.GC_270, C.GC_295 ]})

V_438 = Vertex(name = 'V_438',
               particles = [ P.a, P.Z2, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_354, C.GC_130, C.GC_138, C.GC_392, C.GC_257 ]})

V_439 = Vertex(name = 'V_439',
               particles = [ P.a, P.Z2, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_354, C.GC_130, C.GC_138, C.GC_392, C.GC_257 ]})

V_440 = Vertex(name = 'V_440',
               particles = [ P.a, P.Z2, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_354, C.GC_257 ]})

V_441 = Vertex(name = 'V_441',
               particles = [ P.a, P.Z2, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_354, C.GC_130, C.GC_137, C.GC_391, C.GC_257 ]})

V_442 = Vertex(name = 'V_442',
               particles = [ P.a, P.Z2, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_596, C.GC_599, C.GC_579 ]})

V_443 = Vertex(name = 'V_443',
               particles = [ P.a, P.Z2, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_596, C.GC_599, C.GC_579 ]})

V_444 = Vertex(name = 'V_444',
               particles = [ P.a, P.Z2, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_596, C.GC_599, C.GC_579 ]})

V_445 = Vertex(name = 'V_445',
               particles = [ P.a, P.Z2, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_597, C.GC_598, C.GC_579 ]})

V_446 = Vertex(name = 'V_446',
               particles = [ P.a, P.Z2, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_791, C.GC_792, C.GC_785 ]})

V_447 = Vertex(name = 'V_447',
               particles = [ P.a, P.Z2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_940, C.GC_905, C.GC_908, C.GC_944, C.GC_914 ]})

V_448 = Vertex(name = 'V_448',
               particles = [ P.a, P.Z2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_1057, C.GC_1063, C.GC_1029, C.GC_1033, C.GC_1036, C.GC_1045 ]})

V_449 = Vertex(name = 'V_449',
               particles = [ P.Z2, P.A01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_582, C.GC_571 ]})

V_450 = Vertex(name = 'V_450',
               particles = [ P.Z2, P.A02, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_58, C.GC_70, C.GC_272 ]})

V_451 = Vertex(name = 'V_451',
               particles = [ P.Z2, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_73, C.GC_274 ]})

V_452 = Vertex(name = 'V_452',
               particles = [ P.Z2, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_581, C.GC_572 ]})

V_453 = Vertex(name = 'V_453',
               particles = [ P.Z2, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_60, C.GC_73, C.GC_274 ]})

V_454 = Vertex(name = 'V_454',
               particles = [ P.Z2, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VSS1 ],
               couplings = {(0,0):[ C.GC_73, C.GC_162, C.GC_237, C.GC_274 ]})

V_455 = Vertex(name = 'V_455',
               particles = [ P.W__minus__, P.Z2, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_66, C.GC_85, C.GC_281 ]})

V_456 = Vertex(name = 'V_456',
               particles = [ P.W__minus__, P.Z2, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_65, C.GC_84, C.GC_280 ]})

V_457 = Vertex(name = 'V_457',
               particles = [ P.W__minus__, P.Z2, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_63, C.GC_82, C.GC_278 ]})

V_458 = Vertex(name = 'V_458',
               particles = [ P.W__minus__, P.Z2, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_575, C.GC_585, C.GC_827 ]})

V_459 = Vertex(name = 'V_459',
               particles = [ P.W__minus__, P.Z2, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_574, C.GC_586, C.GC_828 ]})

V_460 = Vertex(name = 'V_460',
               particles = [ P.W__minus__, P.Z2, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_624})

V_461 = Vertex(name = 'V_461',
               particles = [ P.W__minus__, P.Z2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_803})

V_462 = Vertex(name = 'V_462',
               particles = [ P.W__minus__, P.Z2, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_896, C.GC_899, C.GC_920 ]})

V_463 = Vertex(name = 'V_463',
               particles = [ P.W2__minus__, P.Z2, P.A02, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_447, C.GC_453, C.GC_486 ]})

V_464 = Vertex(name = 'V_464',
               particles = [ P.W2__minus__, P.Z2, P.H03, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_446, C.GC_452, C.GC_485 ]})

V_465 = Vertex(name = 'V_465',
               particles = [ P.W2__minus__, P.Z2, P.HM1, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_444, C.GC_450, C.GC_483 ]})

V_466 = Vertex(name = 'V_466',
               particles = [ P.W2__minus__, P.Z2, P.H, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_550})

V_467 = Vertex(name = 'V_467',
               particles = [ P.W2__minus__, P.Z2, P.A01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_635, C.GC_634 ]})

V_468 = Vertex(name = 'V_468',
               particles = [ P.W2__minus__, P.Z2, P.H01, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_636, C.GC_633 ]})

V_469 = Vertex(name = 'V_469',
               particles = [ P.W2__minus__, P.Z2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_776})

V_470 = Vertex(name = 'V_470',
               particles = [ P.W2__minus__, P.Z2, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_929, C.GC_930, C.GC_935 ]})

V_471 = Vertex(name = 'V_471',
               particles = [ P.W2__plus__, P.Z2, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_445, C.GC_451, C.GC_484 ]})

V_472 = Vertex(name = 'V_472',
               particles = [ P.W2__plus__, P.Z2, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_446, C.GC_452, C.GC_485 ]})

V_473 = Vertex(name = 'V_473',
               particles = [ P.W2__plus__, P.Z2, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_444, C.GC_450, C.GC_483 ]})

V_474 = Vertex(name = 'V_474',
               particles = [ P.W2__plus__, P.Z2, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_550})

V_475 = Vertex(name = 'V_475',
               particles = [ P.W2__plus__, P.Z2, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_637, C.GC_632 ]})

V_476 = Vertex(name = 'V_476',
               particles = [ P.W2__plus__, P.Z2, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_636, C.GC_633 ]})

V_477 = Vertex(name = 'V_477',
               particles = [ P.W2__plus__, P.Z2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_776})

V_478 = Vertex(name = 'V_478',
               particles = [ P.W2__plus__, P.Z2, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_929, C.GC_930, C.GC_935 ]})

V_479 = Vertex(name = 'V_479',
               particles = [ P.a, P.W__minus__, P.W2__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_464, C.GC_470, C.GC_482 ]})

V_480 = Vertex(name = 'V_480',
               particles = [ P.a, P.W2__minus__, P.W2__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_531, C.GC_271 ]})

V_481 = Vertex(name = 'V_481',
               particles = [ P.W__plus__, P.Z2, P.A02, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_64, C.GC_83, C.GC_279 ]})

V_482 = Vertex(name = 'V_482',
               particles = [ P.W__plus__, P.Z2, P.H03, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_65, C.GC_84, C.GC_280 ]})

V_483 = Vertex(name = 'V_483',
               particles = [ P.W__plus__, P.Z2, P.HP1, P.HL__minus____minus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_63, C.GC_82, C.GC_278 ]})

V_484 = Vertex(name = 'V_484',
               particles = [ P.W__plus__, P.Z2, P.A01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_573, C.GC_587, C.GC_829 ]})

V_485 = Vertex(name = 'V_485',
               particles = [ P.W__plus__, P.Z2, P.H01, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_574, C.GC_586, C.GC_828 ]})

V_486 = Vertex(name = 'V_486',
               particles = [ P.W__plus__, P.Z2, P.H, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):C.GC_624})

V_487 = Vertex(name = 'V_487',
               particles = [ P.W__plus__, P.Z2, P.HM2 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):C.GC_803})

V_488 = Vertex(name = 'V_488',
               particles = [ P.W__plus__, P.Z2, P.HM1 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_896, C.GC_899, C.GC_920 ]})

V_489 = Vertex(name = 'V_489',
               particles = [ P.a, P.W__minus__, P.W__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_131, C.GC_508, C.GC_513 ]})

V_490 = Vertex(name = 'V_490',
               particles = [ P.a, P.W2__minus__, P.W__plus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV5 ],
               couplings = {(0,0):[ C.GC_464, C.GC_470, C.GC_482 ]})

V_491 = Vertex(name = 'V_491',
               particles = [ P.Z, P.Z2, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_62, C.GC_81, C.GC_132, C.GC_180, C.GC_277, C.GC_288, C.GC_301 ]})

V_492 = Vertex(name = 'V_492',
               particles = [ P.Z, P.Z2, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_360, C.GC_352, C.GC_358, C.GC_180, C.GC_393, C.GC_288, C.GC_291, C.GC_301, C.GC_302 ]})

V_493 = Vertex(name = 'V_493',
               particles = [ P.Z, P.Z2, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_62, C.GC_81, C.GC_132, C.GC_180, C.GC_277, C.GC_288, C.GC_301 ]})

V_494 = Vertex(name = 'V_494',
               particles = [ P.Z, P.Z2, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_62, C.GC_80, C.GC_133, C.GC_180, C.GC_276, C.GC_288, C.GC_301 ]})

V_495 = Vertex(name = 'V_495',
               particles = [ P.Z, P.Z2, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_360, C.GC_353, C.GC_357, C.GC_180, C.GC_394, C.GC_288, C.GC_291, C.GC_301, C.GC_302 ]})

V_496 = Vertex(name = 'V_496',
               particles = [ P.Z, P.Z2, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_180, C.GC_288, C.GC_301 ]})

V_497 = Vertex(name = 'V_497',
               particles = [ P.Z, P.Z2, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_583, C.GC_595, C.GC_604, C.GC_608 ]})

V_498 = Vertex(name = 'V_498',
               particles = [ P.Z, P.Z2, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_583, C.GC_595, C.GC_604, C.GC_608 ]})

V_499 = Vertex(name = 'V_499',
               particles = [ P.Z, P.Z2, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_583, C.GC_595, C.GC_604, C.GC_608 ]})

V_500 = Vertex(name = 'V_500',
               particles = [ P.Z, P.Z2, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_584, C.GC_594, C.GC_603, C.GC_608 ]})

V_501 = Vertex(name = 'V_501',
               particles = [ P.Z, P.Z2, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_787, C.GC_790, C.GC_795, C.GC_798 ]})

V_502 = Vertex(name = 'V_502',
               particles = [ P.Z, P.Z2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_895, C.GC_898, C.GC_906, C.GC_912, C.GC_919, C.GC_922, C.GC_925 ]})

V_503 = Vertex(name = 'V_503',
               particles = [ P.Z, P.Z2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_1068, C.GC_1062, C.GC_1066, C.GC_1031, C.GC_1070, C.GC_1040, C.GC_1042, C.GC_1048, C.GC_1049 ]})

V_504 = Vertex(name = 'V_504',
               particles = [ P.W__minus__, P.W2__plus__, P.Z, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_448, C.GC_524, C.GC_488, C.GC_491 ]})

V_505 = Vertex(name = 'V_505',
               particles = [ P.W2__minus__, P.W2__plus__, P.Z, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_528, C.GC_292, C.GC_303 ]})

V_506 = Vertex(name = 'V_506',
               particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_67, C.GC_535, C.GC_515, C.GC_518 ]})

V_507 = Vertex(name = 'V_507',
               particles = [ P.W2__minus__, P.W__plus__, P.Z, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_448, C.GC_524, C.GC_488, C.GC_491 ]})

V_508 = Vertex(name = 'V_508',
               particles = [ P.Z2, P.Z2, P.H02, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_313, C.GC_355, C.GC_134, C.GC_361, C.GC_181, C.GC_269, C.GC_294, C.GC_297, C.GC_306 ]})

V_509 = Vertex(name = 'V_509',
               particles = [ P.Z2, P.Z2, P.HR__minus____minus__, P.HR__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_313, C.GC_356, C.GC_134, C.GC_361, C.GC_181, C.GC_268, C.GC_294, C.GC_297, C.GC_307 ]})

V_510 = Vertex(name = 'V_510',
               particles = [ P.Z2, P.Z2, P.A02, P.A02 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_68, C.GC_125, C.GC_134, C.GC_361, C.GC_285, C.GC_294 ]})

V_511 = Vertex(name = 'V_511',
               particles = [ P.Z2, P.Z2, P.H03, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_68, C.GC_125, C.GC_134, C.GC_361, C.GC_285, C.GC_294 ]})

V_512 = Vertex(name = 'V_512',
               particles = [ P.Z2, P.Z2, P.HL__minus____minus__, P.HL__plus____plus__ ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_68, C.GC_124, C.GC_134, C.GC_361, C.GC_284, C.GC_294 ]})

V_513 = Vertex(name = 'V_513',
               particles = [ P.Z2, P.Z2, P.HM1, P.HP1 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_134, C.GC_361, C.GC_294 ]})

V_514 = Vertex(name = 'V_514',
               particles = [ P.Z2, P.Z2, P.A01, P.A01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_590, C.GC_606, C.GC_580 ]})

V_515 = Vertex(name = 'V_515',
               particles = [ P.Z2, P.Z2, P.H, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_590, C.GC_606, C.GC_580 ]})

V_516 = Vertex(name = 'V_516',
               particles = [ P.Z2, P.Z2, P.H01, P.H01 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_590, C.GC_606, C.GC_580 ]})

V_517 = Vertex(name = 'V_517',
               particles = [ P.Z2, P.Z2, P.HM2, P.HP2 ],
               color = [ '1' ],
               lorentz = [ L.VVSS1 ],
               couplings = {(0,0):[ C.GC_592, C.GC_605, C.GC_580 ]})

V_518 = Vertex(name = 'V_518',
               particles = [ P.Z2, P.Z2, P.H ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_788, C.GC_796, C.GC_786 ]})

V_519 = Vertex(name = 'V_519',
               particles = [ P.Z2, P.Z2, P.H03 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_897, C.GC_903, C.GC_907, C.GC_942, C.GC_921, C.GC_924 ]})

V_520 = Vertex(name = 'V_520',
               particles = [ P.Z2, P.Z2, P.H02 ],
               color = [ '1' ],
               lorentz = [ L.VVS1 ],
               couplings = {(0,0):[ C.GC_1058, C.GC_1064, C.GC_1025, C.GC_1069, C.GC_1032, C.GC_1035, C.GC_1044, C.GC_1046, C.GC_1051 ]})

V_521 = Vertex(name = 'V_521',
               particles = [ P.W__minus__, P.W2__plus__, P.Z2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_522, C.GC_472, C.GC_490 ]})

V_522 = Vertex(name = 'V_522',
               particles = [ P.W2__minus__, P.W2__plus__, P.Z2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_314, C.GC_529, C.GC_299 ]})

V_523 = Vertex(name = 'V_523',
               particles = [ P.W__minus__, P.W__plus__, P.Z2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_69, C.GC_533, C.GC_510, C.GC_516 ]})

V_524 = Vertex(name = 'V_524',
               particles = [ P.W2__minus__, P.W__plus__, P.Z2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.VVVV2 ],
               couplings = {(0,0):[ C.GC_522, C.GC_472, C.GC_490 ]})

V_525 = Vertex(name = 'V_525',
               particles = [ P.v1, P.v1, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_96, C.GC_97 ],(0,1):[ C.GC_108, C.GC_109 ]})

V_526 = Vertex(name = 'V_526',
               particles = [ P.N1, P.v1, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_188, C.GC_189 ],(0,1):[ C.GC_182, C.GC_183 ]})

V_527 = Vertex(name = 'V_527',
               particles = [ P.v2, P.v2, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_98, C.GC_99 ],(0,1):[ C.GC_110, C.GC_111 ]})

V_528 = Vertex(name = 'V_528',
               particles = [ P.N2, P.v2, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_190, C.GC_191 ],(0,1):[ C.GC_184, C.GC_185 ]})

V_529 = Vertex(name = 'V_529',
               particles = [ P.v3, P.v3, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_100, C.GC_101 ],(0,1):[ C.GC_112, C.GC_113 ]})

V_530 = Vertex(name = 'V_530',
               particles = [ P.N3, P.v3, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_192, C.GC_193 ],(0,1):[ C.GC_186, C.GC_187 ]})

V_531 = Vertex(name = 'V_531',
               particles = [ P.N1, P.N1, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_102, C.GC_103 ],(0,1):[ C.GC_114, C.GC_115 ]})

V_532 = Vertex(name = 'V_532',
               particles = [ P.N2, P.N2, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_104, C.GC_105 ],(0,1):[ C.GC_116, C.GC_117 ]})

V_533 = Vertex(name = 'V_533',
               particles = [ P.N3, P.N3, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_106, C.GC_107 ],(0,1):[ C.GC_118, C.GC_119 ]})

V_534 = Vertex(name = 'V_534',
               particles = [ P.e__plus__, P.v1, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_46,(0,1):C.GC_438})

V_535 = Vertex(name = 'V_535',
               particles = [ P.mu__plus__, P.v2, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_47,(0,1):C.GC_439})

V_536 = Vertex(name = 'V_536',
               particles = [ P.ta__plus__, P.v3, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_48,(0,1):C.GC_440})

V_537 = Vertex(name = 'V_537',
               particles = [ P.e__plus__, P.N1, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_49,(0,1):C.GC_441})

V_538 = Vertex(name = 'V_538',
               particles = [ P.mu__plus__, P.N2, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_50,(0,1):C.GC_442})

V_539 = Vertex(name = 'V_539',
               particles = [ P.ta__plus__, P.N3, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_51,(0,1):C.GC_443})

V_540 = Vertex(name = 'V_540',
               particles = [ P.e__plus__, P.v1, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_432,(0,1):C.GC_52})

V_541 = Vertex(name = 'V_541',
               particles = [ P.mu__plus__, P.v2, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_433,(0,1):C.GC_53})

V_542 = Vertex(name = 'V_542',
               particles = [ P.ta__plus__, P.v3, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_434,(0,1):C.GC_54})

V_543 = Vertex(name = 'V_543',
               particles = [ P.e__plus__, P.N1, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_435,(0,1):C.GC_55})

V_544 = Vertex(name = 'V_544',
               particles = [ P.mu__plus__, P.N2, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_436,(0,1):C.GC_56})

V_545 = Vertex(name = 'V_545',
               particles = [ P.ta__plus__, P.N3, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_437,(0,1):C.GC_57})

V_546 = Vertex(name = 'V_546',
               particles = [ P.v1, P.v1, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_321, C.GC_212, C.GC_146, C.GC_242 ],(0,1):[ C.GC_327, C.GC_218, C.GC_152, C.GC_248 ]})

V_547 = Vertex(name = 'V_547',
               particles = [ P.N1, P.v1, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_333, C.GC_224, C.GC_318, C.GC_386 ],(0,1):[ C.GC_328, C.GC_219, C.GC_315, C.GC_383 ]})

V_548 = Vertex(name = 'V_548',
               particles = [ P.v2, P.v2, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_322, C.GC_213, C.GC_147, C.GC_243 ],(0,1):[ C.GC_329, C.GC_220, C.GC_153, C.GC_249 ]})

V_549 = Vertex(name = 'V_549',
               particles = [ P.N2, P.v2, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_335, C.GC_226, C.GC_319, C.GC_387 ],(0,1):[ C.GC_330, C.GC_221, C.GC_316, C.GC_384 ]})

V_550 = Vertex(name = 'V_550',
               particles = [ P.v3, P.v3, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_323, C.GC_214, C.GC_148, C.GC_244 ],(0,1):[ C.GC_331, C.GC_222, C.GC_154, C.GC_250 ]})

V_551 = Vertex(name = 'V_551',
               particles = [ P.N3, P.v3, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_337, C.GC_228, C.GC_320, C.GC_388 ],(0,1):[ C.GC_332, C.GC_223, C.GC_317, C.GC_385 ]})

V_552 = Vertex(name = 'V_552',
               particles = [ P.N1, P.N1, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_324, C.GC_215, C.GC_149, C.GC_245 ],(0,1):[ C.GC_334, C.GC_225, C.GC_155, C.GC_251 ]})

V_553 = Vertex(name = 'V_553',
               particles = [ P.N2, P.N2, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_325, C.GC_216, C.GC_150, C.GC_246 ],(0,1):[ C.GC_336, C.GC_227, C.GC_156, C.GC_252 ]})

V_554 = Vertex(name = 'V_554',
               particles = [ P.N3, P.N3, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_326, C.GC_217, C.GC_151, C.GC_247 ],(0,1):[ C.GC_338, C.GC_229, C.GC_157, C.GC_253 ]})

V_555 = Vertex(name = 'V_555',
               particles = [ P.v1, P.v1, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_365, C.GC_194, C.GC_164 ],(0,1):[ C.GC_371, C.GC_200, C.GC_170 ]})

V_556 = Vertex(name = 'V_556',
               particles = [ P.N1, P.v1, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_377, C.GC_206, C.GC_346 ],(0,1):[ C.GC_372, C.GC_201, C.GC_343 ]})

V_557 = Vertex(name = 'V_557',
               particles = [ P.v2, P.v2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_366, C.GC_195, C.GC_165 ],(0,1):[ C.GC_373, C.GC_202, C.GC_171 ]})

V_558 = Vertex(name = 'V_558',
               particles = [ P.N2, P.v2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_379, C.GC_208, C.GC_347 ],(0,1):[ C.GC_374, C.GC_203, C.GC_344 ]})

V_559 = Vertex(name = 'V_559',
               particles = [ P.v3, P.v3, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_367, C.GC_196, C.GC_166 ],(0,1):[ C.GC_375, C.GC_204, C.GC_172 ]})

V_560 = Vertex(name = 'V_560',
               particles = [ P.N3, P.v3, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_381, C.GC_210, C.GC_348 ],(0,1):[ C.GC_376, C.GC_205, C.GC_345 ]})

V_561 = Vertex(name = 'V_561',
               particles = [ P.N1, P.N1, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_368, C.GC_197, C.GC_167 ],(0,1):[ C.GC_378, C.GC_207, C.GC_173 ]})

V_562 = Vertex(name = 'V_562',
               particles = [ P.N2, P.N2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_369, C.GC_198, C.GC_168 ],(0,1):[ C.GC_380, C.GC_209, C.GC_174 ]})

V_563 = Vertex(name = 'V_563',
               particles = [ P.N3, P.N3, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_370, C.GC_199, C.GC_169 ],(0,1):[ C.GC_382, C.GC_211, C.GC_175 ]})

V_564 = Vertex(name = 'V_564',
               particles = [ P.e__plus__, P.e__minus__, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_78,(0,1):[ C.GC_76, C.GC_92 ]})

V_565 = Vertex(name = 'V_565',
               particles = [ P.mu__plus__, P.mu__minus__, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_78,(0,1):[ C.GC_76, C.GC_92 ]})

V_566 = Vertex(name = 'V_566',
               particles = [ P.ta__plus__, P.ta__minus__, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_78,(0,1):[ C.GC_76, C.GC_92 ]})

V_567 = Vertex(name = 'V_567',
               particles = [ P.v1, P.e__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_432,(0,1):C.GC_52})

V_568 = Vertex(name = 'V_568',
               particles = [ P.N1, P.e__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_435,(0,1):C.GC_55})

V_569 = Vertex(name = 'V_569',
               particles = [ P.v2, P.mu__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_433,(0,1):C.GC_53})

V_570 = Vertex(name = 'V_570',
               particles = [ P.N2, P.mu__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_436,(0,1):C.GC_56})

V_571 = Vertex(name = 'V_571',
               particles = [ P.v3, P.ta__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_434,(0,1):C.GC_54})

V_572 = Vertex(name = 'V_572',
               particles = [ P.N3, P.ta__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_437,(0,1):C.GC_57})

V_573 = Vertex(name = 'V_573',
               particles = [ P.v1, P.e__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_46,(0,1):C.GC_438})

V_574 = Vertex(name = 'V_574',
               particles = [ P.N1, P.e__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_49,(0,1):C.GC_441})

V_575 = Vertex(name = 'V_575',
               particles = [ P.v2, P.mu__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_47,(0,1):C.GC_439})

V_576 = Vertex(name = 'V_576',
               particles = [ P.N2, P.mu__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_50,(0,1):C.GC_442})

V_577 = Vertex(name = 'V_577',
               particles = [ P.v3, P.ta__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_48,(0,1):C.GC_440})

V_578 = Vertex(name = 'V_578',
               particles = [ P.N3, P.ta__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_51,(0,1):C.GC_443})

V_579 = Vertex(name = 'V_579',
               particles = [ P.e__plus__, P.e__minus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_310, C.GC_121 ],(0,1):[ C.GC_121, C.GC_136, C.GC_142, C.GC_240 ]})

V_580 = Vertex(name = 'V_580',
               particles = [ P.mu__plus__, P.mu__minus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_310, C.GC_121 ],(0,1):[ C.GC_121, C.GC_136, C.GC_142, C.GC_240 ]})

V_581 = Vertex(name = 'V_581',
               particles = [ P.ta__plus__, P.ta__minus__, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_310, C.GC_121 ],(0,1):[ C.GC_121, C.GC_136, C.GC_142, C.GC_240 ]})

V_582 = Vertex(name = 'V_582',
               particles = [ P.e__plus__, P.e__minus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_340, C.GC_72 ],(0,1):[ C.GC_72, C.GC_363, C.GC_161 ]})

V_583 = Vertex(name = 'V_583',
               particles = [ P.mu__plus__, P.mu__minus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_340, C.GC_72 ],(0,1):[ C.GC_72, C.GC_363, C.GC_161 ]})

V_584 = Vertex(name = 'V_584',
               particles = [ P.ta__plus__, P.ta__minus__, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_340, C.GC_72 ],(0,1):[ C.GC_72, C.GC_363, C.GC_161 ]})

V_585 = Vertex(name = 'V_585',
               particles = [ P.u__tilde__, P.u, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_77,(0,1):[ C.GC_75, C.GC_93 ]})

V_586 = Vertex(name = 'V_586',
               particles = [ P.c__tilde__, P.c, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_77,(0,1):[ C.GC_75, C.GC_93 ]})

V_587 = Vertex(name = 'V_587',
               particles = [ P.t__tilde__, P.t, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_77,(0,1):[ C.GC_75, C.GC_93 ]})

V_588 = Vertex(name = 'V_588',
               particles = [ P.u__tilde__, P.u, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_589 = Vertex(name = 'V_589',
               particles = [ P.c__tilde__, P.c, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_590 = Vertex(name = 'V_590',
               particles = [ P.t__tilde__, P.t, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_591 = Vertex(name = 'V_591',
               particles = [ P.d__tilde__, P.u, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_12,(0,1):C.GC_421})

V_592 = Vertex(name = 'V_592',
               particles = [ P.s__tilde__, P.u, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_13,(0,1):C.GC_422})

V_593 = Vertex(name = 'V_593',
               particles = [ P.b__tilde__, P.u, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_14,(0,1):C.GC_423})

V_594 = Vertex(name = 'V_594',
               particles = [ P.d__tilde__, P.c, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_15,(0,1):C.GC_424})

V_595 = Vertex(name = 'V_595',
               particles = [ P.s__tilde__, P.c, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_16,(0,1):C.GC_425})

V_596 = Vertex(name = 'V_596',
               particles = [ P.b__tilde__, P.c, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_17,(0,1):C.GC_426})

V_597 = Vertex(name = 'V_597',
               particles = [ P.d__tilde__, P.t, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_18,(0,1):C.GC_427})

V_598 = Vertex(name = 'V_598',
               particles = [ P.s__tilde__, P.t, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_19,(0,1):C.GC_428})

V_599 = Vertex(name = 'V_599',
               particles = [ P.b__tilde__, P.t, P.W__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_20,(0,1):C.GC_429})

V_600 = Vertex(name = 'V_600',
               particles = [ P.d__tilde__, P.u, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_400,(0,1):C.GC_34})

V_601 = Vertex(name = 'V_601',
               particles = [ P.s__tilde__, P.u, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_401,(0,1):C.GC_35})

V_602 = Vertex(name = 'V_602',
               particles = [ P.b__tilde__, P.u, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_402,(0,1):C.GC_36})

V_603 = Vertex(name = 'V_603',
               particles = [ P.d__tilde__, P.c, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_403,(0,1):C.GC_37})

V_604 = Vertex(name = 'V_604',
               particles = [ P.s__tilde__, P.c, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_404,(0,1):C.GC_38})

V_605 = Vertex(name = 'V_605',
               particles = [ P.b__tilde__, P.c, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_405,(0,1):C.GC_39})

V_606 = Vertex(name = 'V_606',
               particles = [ P.d__tilde__, P.t, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_406,(0,1):C.GC_40})

V_607 = Vertex(name = 'V_607',
               particles = [ P.s__tilde__, P.t, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_407,(0,1):C.GC_41})

V_608 = Vertex(name = 'V_608',
               particles = [ P.b__tilde__, P.t, P.W2__minus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_408,(0,1):C.GC_42})

V_609 = Vertex(name = 'V_609',
               particles = [ P.u__tilde__, P.u, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_309, C.GC_120 ],(0,1):[ C.GC_120, C.GC_135, C.GC_141, C.GC_239 ]})

V_610 = Vertex(name = 'V_610',
               particles = [ P.c__tilde__, P.c, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_309, C.GC_120 ],(0,1):[ C.GC_120, C.GC_135, C.GC_141, C.GC_239 ]})

V_611 = Vertex(name = 'V_611',
               particles = [ P.t__tilde__, P.t, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_309, C.GC_120 ],(0,1):[ C.GC_120, C.GC_135, C.GC_141, C.GC_239 ]})

V_612 = Vertex(name = 'V_612',
               particles = [ P.u__tilde__, P.u, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_339, C.GC_71 ],(0,1):[ C.GC_71, C.GC_364, C.GC_160 ]})

V_613 = Vertex(name = 'V_613',
               particles = [ P.c__tilde__, P.c, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_339, C.GC_71 ],(0,1):[ C.GC_71, C.GC_364, C.GC_160 ]})

V_614 = Vertex(name = 'V_614',
               particles = [ P.t__tilde__, P.t, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_339, C.GC_71 ],(0,1):[ C.GC_71, C.GC_364, C.GC_160 ]})

V_615 = Vertex(name = 'V_615',
               particles = [ P.d__tilde__, P.d, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_76, C.GC_230 ],(0,1):[ C.GC_75, C.GC_92 ]})

V_616 = Vertex(name = 'V_616',
               particles = [ P.s__tilde__, P.s, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_76, C.GC_230 ],(0,1):[ C.GC_75, C.GC_92 ]})

V_617 = Vertex(name = 'V_617',
               particles = [ P.b__tilde__, P.b, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_76, C.GC_230 ],(0,1):[ C.GC_75, C.GC_92 ]})

V_618 = Vertex(name = 'V_618',
               particles = [ P.d__tilde__, P.d, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_619 = Vertex(name = 'V_619',
               particles = [ P.s__tilde__, P.s, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_620 = Vertex(name = 'V_620',
               particles = [ P.b__tilde__, P.b, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_621 = Vertex(name = 'V_621',
               particles = [ P.u__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1111,(0,1):C.GC_1128})

V_622 = Vertex(name = 'V_622',
               particles = [ P.c__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1117,(0,1):C.GC_1134})

V_623 = Vertex(name = 'V_623',
               particles = [ P.t__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1123,(0,1):C.GC_1140})

V_624 = Vertex(name = 'V_624',
               particles = [ P.u__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1113,(0,1):C.GC_1130})

V_625 = Vertex(name = 'V_625',
               particles = [ P.c__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1119,(0,1):C.GC_1136})

V_626 = Vertex(name = 'V_626',
               particles = [ P.t__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1125,(0,1):C.GC_1142})

V_627 = Vertex(name = 'V_627',
               particles = [ P.u__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1115,(0,1):C.GC_1132})

V_628 = Vertex(name = 'V_628',
               particles = [ P.c__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1121,(0,1):C.GC_1138})

V_629 = Vertex(name = 'V_629',
               particles = [ P.t__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1127,(0,1):C.GC_1144})

V_630 = Vertex(name = 'V_630',
               particles = [ P.u__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1110,(0,1):C.GC_1129})

V_631 = Vertex(name = 'V_631',
               particles = [ P.c__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1116,(0,1):C.GC_1135})

V_632 = Vertex(name = 'V_632',
               particles = [ P.t__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1122,(0,1):C.GC_1141})

V_633 = Vertex(name = 'V_633',
               particles = [ P.u__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1112,(0,1):C.GC_1131})

V_634 = Vertex(name = 'V_634',
               particles = [ P.c__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1118,(0,1):C.GC_1137})

V_635 = Vertex(name = 'V_635',
               particles = [ P.t__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1124,(0,1):C.GC_1143})

V_636 = Vertex(name = 'V_636',
               particles = [ P.u__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1114,(0,1):C.GC_1133})

V_637 = Vertex(name = 'V_637',
               particles = [ P.c__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1120,(0,1):C.GC_1139})

V_638 = Vertex(name = 'V_638',
               particles = [ P.t__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_1126,(0,1):C.GC_1145})

V_639 = Vertex(name = 'V_639',
               particles = [ P.d__tilde__, P.d, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_5, C.GC_120, C.GC_255 ],(0,1):[ C.GC_120, C.GC_135, C.GC_142, C.GC_240 ]})

V_640 = Vertex(name = 'V_640',
               particles = [ P.s__tilde__, P.s, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_5, C.GC_120, C.GC_255 ],(0,1):[ C.GC_120, C.GC_135, C.GC_142, C.GC_240 ]})

V_641 = Vertex(name = 'V_641',
               particles = [ P.b__tilde__, P.b, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_5, C.GC_120, C.GC_255 ],(0,1):[ C.GC_120, C.GC_135, C.GC_142, C.GC_240 ]})

V_642 = Vertex(name = 'V_642',
               particles = [ P.d__tilde__, P.d, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_59, C.GC_71, C.GC_273 ],(0,1):[ C.GC_71, C.GC_362, C.GC_161 ]})

V_643 = Vertex(name = 'V_643',
               particles = [ P.s__tilde__, P.s, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_59, C.GC_71, C.GC_273 ],(0,1):[ C.GC_71, C.GC_362, C.GC_161 ]})

V_644 = Vertex(name = 'V_644',
               particles = [ P.b__tilde__, P.b, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_59, C.GC_71, C.GC_273 ],(0,1):[ C.GC_71, C.GC_362, C.GC_161 ]})

