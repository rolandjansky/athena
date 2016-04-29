# This file was automatically created by FeynRules 2.1.91
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Tue 14 Apr 2015 09:34:45


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot



R2GC_260_1 = Coupling(name = 'R2GC_260_1',
                      value = '(complex(0,1)*G**2)/(12.*cmath.pi**2)',
                      order = {'QCD':2})

R2GC_261_2 = Coupling(name = 'R2GC_261_2',
                      value = '-(cw*complex(0,1)*G**2*g1)/(9.*cmath.pi**2)',
                      order = {'QCD':2,'QED':1})

R2GC_262_3 = Coupling(name = 'R2GC_262_3',
                      value = '(complex(0,1)*G**2*g1*sw)/(9.*cmath.pi**2)',
                      order = {'QCD':2,'QED':1})

R2GC_264_4 = Coupling(name = 'R2GC_264_4',
                      value = '(cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2)',
                      order = {'QCD':2,'QED':1})

R2GC_265_5 = Coupling(name = 'R2GC_265_5',
                      value = '-(complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2)',
                      order = {'QCD':2,'QED':1})

R2GC_272_6 = Coupling(name = 'R2GC_272_6',
                      value = '-(complex(0,1)*G**2*MB**2)/(8.*cmath.pi**2)',
                      order = {'QCD':2})

R2GC_272_7 = Coupling(name = 'R2GC_272_7',
                      value = '-(complex(0,1)*G**2*MT**2)/(8.*cmath.pi**2)',
                      order = {'QCD':2})

R2GC_273_8 = Coupling(name = 'R2GC_273_8',
                      value = '-(complex(0,1)*G**2*yb**2)/(16.*cmath.pi**2)',
                      order = {'QCD':2,'QED':2})

R2GC_273_9 = Coupling(name = 'R2GC_273_9',
                      value = '-(complex(0,1)*G**2*yt**2)/(16.*cmath.pi**2)',
                      order = {'QCD':2,'QED':2})

R2GC_274_10 = Coupling(name = 'R2GC_274_10',
                       value = '-(complex(0,1)*G**2*GD3x3*MB*TH2x1)/(16.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MB*TH1x1*yb)/(8.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MB*TH2x1*complexconjugate(GD3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_274_11 = Coupling(name = 'R2GC_274_11',
                       value = '-(complex(0,1)*G**2*GU3x3*MT*TH2x1)/(16.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MT*TH1x1*yt)/(8.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MT*TH2x1*complexconjugate(GU3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_275_12 = Coupling(name = 'R2GC_275_12',
                       value = '-(complex(0,1)*G**2*GD3x3*MB*TH2x2)/(16.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MB*TH1x2*yb)/(8.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MB*TH2x2*complexconjugate(GD3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_275_13 = Coupling(name = 'R2GC_275_13',
                       value = '-(complex(0,1)*G**2*GU3x3*MT*TH2x2)/(16.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MT*TH1x2*yt)/(8.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*MT*TH2x2*complexconjugate(GU3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_276_14 = Coupling(name = 'R2GC_276_14',
                       value = '(G**2*GD3x3*MB*TH3x3)/(16.*cmath.pi**2*cmath.sqrt(2)) - (G**2*MB*TH3x3*complexconjugate(GD3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_276_15 = Coupling(name = 'R2GC_276_15',
                       value = '-(G**2*GU3x3*MT*TH3x3)/(16.*cmath.pi**2*cmath.sqrt(2)) + (G**2*MT*TH3x3*complexconjugate(GU3x3))/(16.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_277_16 = Coupling(name = 'R2GC_277_16',
                       value = '-(G**2*GD3x3*TH2x1*yb)/(32.*cmath.pi**2) + (G**2*TH2x1*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_277_17 = Coupling(name = 'R2GC_277_17',
                       value = '(G**2*GU3x3*TH2x1*yt)/(32.*cmath.pi**2) - (G**2*TH2x1*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_278_18 = Coupling(name = 'R2GC_278_18',
                       value = '-(G**2*GD3x3*TH2x2*yb)/(32.*cmath.pi**2) + (G**2*TH2x2*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_278_19 = Coupling(name = 'R2GC_278_19',
                       value = '(G**2*GU3x3*TH2x2*yt)/(32.*cmath.pi**2) - (G**2*TH2x2*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_279_20 = Coupling(name = 'R2GC_279_20',
                       value = '-(complex(0,1)*G**2*GD3x3*TH3x3*yb)/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH3x3*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_279_21 = Coupling(name = 'R2GC_279_21',
                       value = '-(complex(0,1)*G**2*GU3x3*TH3x3*yt)/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH3x3*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_280_22 = Coupling(name = 'R2GC_280_22',
                       value = '(G**2*GD3x3*TH1x1*TH3x3*yb)/(32.*cmath.pi**2) - (G**2*TH1x1*TH3x3*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_280_23 = Coupling(name = 'R2GC_280_23',
                       value = '-(G**2*GU3x3*TH1x1*TH3x3*yt)/(32.*cmath.pi**2) + (G**2*TH1x1*TH3x3*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_281_24 = Coupling(name = 'R2GC_281_24',
                       value = '(G**2*GD3x3*TH1x2*TH3x3*yb)/(32.*cmath.pi**2) - (G**2*TH1x2*TH3x3*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_281_25 = Coupling(name = 'R2GC_281_25',
                       value = '-(G**2*GU3x3*TH1x2*TH3x3*yt)/(32.*cmath.pi**2) + (G**2*TH1x2*TH3x3*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_282_26 = Coupling(name = 'R2GC_282_26',
                       value = '(cw*G**2*gw)/(48.*cmath.pi**2) + (G**2*g1*sw)/(48.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_282_27 = Coupling(name = 'R2GC_282_27',
                       value = '-(cw*G**2*gw)/(48.*cmath.pi**2) - (G**2*g1*sw)/(48.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_284_28 = Coupling(name = 'R2GC_284_28',
                       value = '-(cw*complex(0,1)*G**3*gw)/(192.*cmath.pi**2) + (complex(0,1)*G**3*g1*sw)/(576.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_284_29 = Coupling(name = 'R2GC_284_29',
                       value = '(cw*complex(0,1)*G**3*gw)/(192.*cmath.pi**2) - (5*complex(0,1)*G**3*g1*sw)/(576.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_285_30 = Coupling(name = 'R2GC_285_30',
                       value = '(-3*cw*complex(0,1)*G**3*gw)/(64.*cmath.pi**2) - (3*complex(0,1)*G**3*g1*sw)/(64.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_285_31 = Coupling(name = 'R2GC_285_31',
                       value = '(3*cw*complex(0,1)*G**3*gw)/(64.*cmath.pi**2) + (3*complex(0,1)*G**3*g1*sw)/(64.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_286_32 = Coupling(name = 'R2GC_286_32',
                       value = '-(cw*G**2*g1)/(48.*cmath.pi**2) + (G**2*gw*sw)/(48.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_286_33 = Coupling(name = 'R2GC_286_33',
                       value = '(cw*G**2*g1)/(48.*cmath.pi**2) - (G**2*gw*sw)/(48.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_288_34 = Coupling(name = 'R2GC_288_34',
                       value = '-(cw*complex(0,1)*G**3*g1)/(576.*cmath.pi**2) - (complex(0,1)*G**3*gw*sw)/(192.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_288_35 = Coupling(name = 'R2GC_288_35',
                       value = '(5*cw*complex(0,1)*G**3*g1)/(576.*cmath.pi**2) + (complex(0,1)*G**3*gw*sw)/(192.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_289_36 = Coupling(name = 'R2GC_289_36',
                       value = '(-3*cw*complex(0,1)*G**3*g1)/(64.*cmath.pi**2) + (3*complex(0,1)*G**3*gw*sw)/(64.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_289_37 = Coupling(name = 'R2GC_289_37',
                       value = '(3*cw*complex(0,1)*G**3*g1)/(64.*cmath.pi**2) - (3*complex(0,1)*G**3*gw*sw)/(64.*cmath.pi**2)',
                       order = {'QCD':3,'QED':1})

R2GC_290_38 = Coupling(name = 'R2GC_290_38',
                       value = '(cw**2*complex(0,1)*G**2*gw**2)/(192.*cmath.pi**2) + (cw*complex(0,1)*G**2*g1*gw*sw)/(288.*cmath.pi**2) + (5*complex(0,1)*G**2*g1**2*sw**2)/(1728.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_290_39 = Coupling(name = 'R2GC_290_39',
                       value = '(cw**2*complex(0,1)*G**2*gw**2)/(192.*cmath.pi**2) - (cw*complex(0,1)*G**2*g1*gw*sw)/(288.*cmath.pi**2) + (17*complex(0,1)*G**2*g1**2*sw**2)/(1728.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_291_40 = Coupling(name = 'R2GC_291_40',
                       value = '-(cw**2*complex(0,1)*G**2*g1*gw)/(576.*cmath.pi**2) - (5*cw*complex(0,1)*G**2*g1**2*sw)/(1728.*cmath.pi**2) + (cw*complex(0,1)*G**2*gw**2*sw)/(192.*cmath.pi**2) + (complex(0,1)*G**2*g1*gw*sw**2)/(576.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_291_41 = Coupling(name = 'R2GC_291_41',
                       value = '(cw**2*complex(0,1)*G**2*g1*gw)/(576.*cmath.pi**2) - (17*cw*complex(0,1)*G**2*g1**2*sw)/(1728.*cmath.pi**2) + (cw*complex(0,1)*G**2*gw**2*sw)/(192.*cmath.pi**2) - (complex(0,1)*G**2*g1*gw*sw**2)/(576.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_292_42 = Coupling(name = 'R2GC_292_42',
                       value = '(5*cw**2*complex(0,1)*G**2*g1**2)/(1728.*cmath.pi**2) - (cw*complex(0,1)*G**2*g1*gw*sw)/(288.*cmath.pi**2) + (complex(0,1)*G**2*gw**2*sw**2)/(192.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_292_43 = Coupling(name = 'R2GC_292_43',
                       value = '(17*cw**2*complex(0,1)*G**2*g1**2)/(1728.*cmath.pi**2) + (cw*complex(0,1)*G**2*g1*gw*sw)/(288.*cmath.pi**2) + (complex(0,1)*G**2*gw**2*sw**2)/(192.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_44 = Coupling(name = 'R2GC_293_44',
                       value = '-(complex(0,1)*G**2*GD3x3*TH1x1*TH2x1*yb)/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1**2*yb**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*TH2x1**2*complexconjugate(GD3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH2x1*yb*complexconjugate(GD3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_45 = Coupling(name = 'R2GC_293_45',
                       value = '-(complex(0,1)*G**2*GU2x2*TH2x1**2*complexconjugate(GU2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_46 = Coupling(name = 'R2GC_293_46',
                       value = '-(complex(0,1)*G**2*GD1x1*TH2x1**2*complexconjugate(GD1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_47 = Coupling(name = 'R2GC_293_47',
                       value = '-(complex(0,1)*G**2*GD2x2*TH2x1**2*complexconjugate(GD2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_48 = Coupling(name = 'R2GC_293_48',
                       value = '-(complex(0,1)*G**2*GU3x3*TH1x1*TH2x1*yt)/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1**2*yt**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU3x3*TH2x1**2*complexconjugate(GU3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH2x1*yt*complexconjugate(GU3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_293_49 = Coupling(name = 'R2GC_293_49',
                       value = '-(complex(0,1)*G**2*GU1x1*TH2x1**2*complexconjugate(GU1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_50 = Coupling(name = 'R2GC_294_50',
                       value = '-(complex(0,1)*G**2*GD3x3*TH1x2*TH2x1*yb)/(32.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*TH1x1*TH2x2*yb)/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH1x2*yb**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*TH2x1*TH2x2*complexconjugate(GD3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2*TH2x1*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH2x2*yb*complexconjugate(GD3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_51 = Coupling(name = 'R2GC_294_51',
                       value = '-(complex(0,1)*G**2*GU2x2*TH2x1*TH2x2*complexconjugate(GU2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_52 = Coupling(name = 'R2GC_294_52',
                       value = '-(complex(0,1)*G**2*GD1x1*TH2x1*TH2x2*complexconjugate(GD1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_53 = Coupling(name = 'R2GC_294_53',
                       value = '-(complex(0,1)*G**2*GD2x2*TH2x1*TH2x2*complexconjugate(GD2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_54 = Coupling(name = 'R2GC_294_54',
                       value = '-(complex(0,1)*G**2*GU3x3*TH1x2*TH2x1*yt)/(32.*cmath.pi**2) - (complex(0,1)*G**2*GU3x3*TH1x1*TH2x2*yt)/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH1x2*yt**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU3x3*TH2x1*TH2x2*complexconjugate(GU3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2*TH2x1*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2) - (complex(0,1)*G**2*TH1x1*TH2x2*yt*complexconjugate(GU3x3))/(32.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_294_55 = Coupling(name = 'R2GC_294_55',
                       value = '-(complex(0,1)*G**2*GU1x1*TH2x1*TH2x2*complexconjugate(GU1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_56 = Coupling(name = 'R2GC_295_56',
                       value = '-(complex(0,1)*G**2*GD3x3*TH1x2*TH2x2*yb)/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2**2*yb**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*TH2x2**2*complexconjugate(GD3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2*TH2x2*yb*complexconjugate(GD3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_57 = Coupling(name = 'R2GC_295_57',
                       value = '-(complex(0,1)*G**2*GU2x2*TH2x2**2*complexconjugate(GU2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_58 = Coupling(name = 'R2GC_295_58',
                       value = '-(complex(0,1)*G**2*GD1x1*TH2x2**2*complexconjugate(GD1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_59 = Coupling(name = 'R2GC_295_59',
                       value = '-(complex(0,1)*G**2*GD2x2*TH2x2**2*complexconjugate(GD2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_60 = Coupling(name = 'R2GC_295_60',
                       value = '-(complex(0,1)*G**2*GU3x3*TH1x2*TH2x2*yt)/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2**2*yt**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU3x3*TH2x2**2*complexconjugate(GU3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*TH1x2*TH2x2*yt*complexconjugate(GU3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_295_61 = Coupling(name = 'R2GC_295_61',
                       value = '-(complex(0,1)*G**2*GU1x1*TH2x2**2*complexconjugate(GU1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_62 = Coupling(name = 'R2GC_296_62',
                       value = '-(complex(0,1)*G**2*GD3x3*TH3x3**2*complexconjugate(GD3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_63 = Coupling(name = 'R2GC_296_63',
                       value = '-(complex(0,1)*G**2*GU2x2*TH3x3**2*complexconjugate(GU2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_64 = Coupling(name = 'R2GC_296_64',
                       value = '-(complex(0,1)*G**2*GD1x1*TH3x3**2*complexconjugate(GD1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_65 = Coupling(name = 'R2GC_296_65',
                       value = '-(complex(0,1)*G**2*GD2x2*TH3x3**2*complexconjugate(GD2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_66 = Coupling(name = 'R2GC_296_66',
                       value = '-(complex(0,1)*G**2*GU3x3*TH3x3**2*complexconjugate(GU3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_296_67 = Coupling(name = 'R2GC_296_67',
                       value = '-(complex(0,1)*G**2*GU1x1*TH3x3**2*complexconjugate(GU1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_297_68 = Coupling(name = 'R2GC_297_68',
                       value = '-(cw*complex(0,1)*G**2*gw)/(12.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_298_69 = Coupling(name = 'R2GC_298_69',
                       value = '-(cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(12.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_299_70 = Coupling(name = 'R2GC_299_70',
                       value = '(cw*complex(0,1)*G**2*gw)/(12.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_300_71 = Coupling(name = 'R2GC_300_71',
                       value = '-(cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(12.*cmath.pi**2)',
                       order = {'QCD':2,'QED':1})

R2GC_303_72 = Coupling(name = 'R2GC_303_72',
                       value = '-(complex(0,1)*G**2*yb**2)/(16.*cmath.pi**2) - (complex(0,1)*G**2*yt**2)/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_304_73 = Coupling(name = 'R2GC_304_73',
                       value = '-(complex(0,1)*G**2*GU3x3*yt)/(16.*cmath.pi**2) - (complex(0,1)*G**2*yb*complexconjugate(GD3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_305_74 = Coupling(name = 'R2GC_305_74',
                       value = '-(complex(0,1)*G**2*GD3x3*yb)/(16.*cmath.pi**2) - (complex(0,1)*G**2*yt*complexconjugate(GU3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_306_75 = Coupling(name = 'R2GC_306_75',
                       value = '(complex(0,1)*G**2*gw**2)/(96.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_307_76 = Coupling(name = 'R2GC_307_76',
                       value = '-(complex(0,1)*G**2*GD3x3*complexconjugate(GD3x3))/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU3x3*complexconjugate(GU3x3))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_307_77 = Coupling(name = 'R2GC_307_77',
                       value = '-(complex(0,1)*G**2*GD2x2*complexconjugate(GD2x2))/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU2x2*complexconjugate(GU2x2))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_307_78 = Coupling(name = 'R2GC_307_78',
                       value = '-(complex(0,1)*G**2*GD1x1*complexconjugate(GD1x1))/(16.*cmath.pi**2) - (complex(0,1)*G**2*GU1x1*complexconjugate(GU1x1))/(16.*cmath.pi**2)',
                       order = {'QCD':2,'QED':2})

R2GC_310_79 = Coupling(name = 'R2GC_310_79',
                       value = '-G**4/(192.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_310_80 = Coupling(name = 'R2GC_310_80',
                       value = 'G**4/(64.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_311_81 = Coupling(name = 'R2GC_311_81',
                       value = '-(complex(0,1)*G**4)/(192.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_311_82 = Coupling(name = 'R2GC_311_82',
                       value = '(complex(0,1)*G**4)/(64.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_312_83 = Coupling(name = 'R2GC_312_83',
                       value = '(complex(0,1)*G**4)/(192.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_312_84 = Coupling(name = 'R2GC_312_84',
                       value = '-(complex(0,1)*G**4)/(64.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_313_85 = Coupling(name = 'R2GC_313_85',
                       value = '-(complex(0,1)*G**4)/(48.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_314_86 = Coupling(name = 'R2GC_314_86',
                       value = '(complex(0,1)*G**4)/(288.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_314_87 = Coupling(name = 'R2GC_314_87',
                       value = '-(complex(0,1)*G**4)/(32.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_315_88 = Coupling(name = 'R2GC_315_88',
                       value = '-(complex(0,1)*G**4)/(16.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_315_89 = Coupling(name = 'R2GC_315_89',
                       value = '(complex(0,1)*G**4)/(4.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_316_90 = Coupling(name = 'R2GC_316_90',
                       value = '(-3*complex(0,1)*G**4)/(64.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_316_91 = Coupling(name = 'R2GC_316_91',
                       value = '(-23*complex(0,1)*G**4)/(64.*cmath.pi**2)',
                       order = {'QCD':4})

R2GC_317_92 = Coupling(name = 'R2GC_317_92',
                       value = '-(complex(0,1)*G**3)/(6.*cmath.pi**2)',
                       order = {'QCD':3})

R2GC_320_93 = Coupling(name = 'R2GC_320_93',
                       value = '(complex(0,1)*G**2*GU2x2*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_321_94 = Coupling(name = 'R2GC_321_94',
                       value = '(complex(0,1)*G**2*GU2x2*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_322_95 = Coupling(name = 'R2GC_322_95',
                       value = '(G**2*GU2x2*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_323_96 = Coupling(name = 'R2GC_323_96',
                       value = '(complex(0,1)*G**2*TH2x1*complexconjugate(GU2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_324_97 = Coupling(name = 'R2GC_324_97',
                       value = '(complex(0,1)*G**2*TH2x2*complexconjugate(GU2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_325_98 = Coupling(name = 'R2GC_325_98',
                       value = '-(G**2*TH3x3*complexconjugate(GU2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_328_99 = Coupling(name = 'R2GC_328_99',
                       value = '(complex(0,1)*G**2*GD1x1*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2))',
                       order = {'QCD':2,'QED':1})

R2GC_329_100 = Coupling(name = 'R2GC_329_100',
                        value = '(complex(0,1)*G**2*GD1x1*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_330_101 = Coupling(name = 'R2GC_330_101',
                        value = '-(G**2*GD1x1*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_331_102 = Coupling(name = 'R2GC_331_102',
                        value = '(complex(0,1)*G**2*TH2x1*complexconjugate(GD1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_332_103 = Coupling(name = 'R2GC_332_103',
                        value = '(complex(0,1)*G**2*TH2x2*complexconjugate(GD1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_333_104 = Coupling(name = 'R2GC_333_104',
                        value = '(G**2*TH3x3*complexconjugate(GD1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_336_105 = Coupling(name = 'R2GC_336_105',
                        value = '(complex(0,1)*G**2*GD2x2*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_337_106 = Coupling(name = 'R2GC_337_106',
                        value = '(complex(0,1)*G**2*GD2x2*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_338_107 = Coupling(name = 'R2GC_338_107',
                        value = '-(G**2*GD2x2*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_339_108 = Coupling(name = 'R2GC_339_108',
                        value = '(complex(0,1)*G**2*TH2x1*complexconjugate(GD2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_340_109 = Coupling(name = 'R2GC_340_109',
                        value = '(complex(0,1)*G**2*TH2x2*complexconjugate(GD2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_341_110 = Coupling(name = 'R2GC_341_110',
                        value = '(G**2*TH3x3*complexconjugate(GD2x2))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_345_111 = Coupling(name = 'R2GC_345_111',
                        value = '(complex(0,1)*G**2*GU1x1*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_346_112 = Coupling(name = 'R2GC_346_112',
                        value = '(complex(0,1)*G**2*GU1x1*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_347_113 = Coupling(name = 'R2GC_347_113',
                        value = '(G**2*GU1x1*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_348_114 = Coupling(name = 'R2GC_348_114',
                        value = '(complex(0,1)*G**2*TH2x1*complexconjugate(GU1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_349_115 = Coupling(name = 'R2GC_349_115',
                        value = '(complex(0,1)*G**2*TH2x2*complexconjugate(GU1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_350_116 = Coupling(name = 'R2GC_350_116',
                        value = '-(G**2*TH3x3*complexconjugate(GU1x1))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_351_117 = Coupling(name = 'R2GC_351_117',
                        value = '(complex(0,1)*G**2*GD2x2)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_352_118 = Coupling(name = 'R2GC_352_118',
                        value = '-(complex(0,1)*G**2*GU2x2)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_353_119 = Coupling(name = 'R2GC_353_119',
                        value = '-(complex(0,1)*G**2*gw)/(6.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_354_120 = Coupling(name = 'R2GC_354_120',
                        value = '(complex(0,1)*G**2*complexconjugate(GD2x2))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_355_121 = Coupling(name = 'R2GC_355_121',
                        value = '-(complex(0,1)*G**2*complexconjugate(GU2x2))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_356_122 = Coupling(name = 'R2GC_356_122',
                        value = '(complex(0,1)*G**2*GD1x1)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_357_123 = Coupling(name = 'R2GC_357_123',
                        value = '-(complex(0,1)*G**2*GU1x1)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_359_124 = Coupling(name = 'R2GC_359_124',
                        value = '(complex(0,1)*G**2*complexconjugate(GD1x1))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_360_125 = Coupling(name = 'R2GC_360_125',
                        value = '-(complex(0,1)*G**2*complexconjugate(GU1x1))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_364_126 = Coupling(name = 'R2GC_364_126',
                        value = '(complex(0,1)*G**2*MB)/(6.*cmath.pi**2)',
                        order = {'QCD':2})

R2GC_368_127 = Coupling(name = 'R2GC_368_127',
                        value = '-(G**2*GD3x3*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_369_128 = Coupling(name = 'R2GC_369_128',
                        value = '-(G**2*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_371_129 = Coupling(name = 'R2GC_371_129',
                        value = '(complex(0,1)*G**2*GD3x3*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_373_130 = Coupling(name = 'R2GC_373_130',
                        value = '(complex(0,1)*G**2*GD3x3*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_374_131 = Coupling(name = 'R2GC_374_131',
                        value = '(complex(0,1)*G**2*TH1x1*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_375_132 = Coupling(name = 'R2GC_375_132',
                        value = '(complex(0,1)*G**2*TH1x2*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_376_133 = Coupling(name = 'R2GC_376_133',
                        value = '(G**2*TH3x3*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_377_134 = Coupling(name = 'R2GC_377_134',
                        value = '(complex(0,1)*G**2)/(48.*cmath.pi**2)',
                        order = {'QCD':2})

R2GC_377_135 = Coupling(name = 'R2GC_377_135',
                        value = '(3*complex(0,1)*G**2)/(32.*cmath.pi**2)',
                        order = {'QCD':2})

R2GC_378_136 = Coupling(name = 'R2GC_378_136',
                        value = '-(complex(0,1)*G**2)/(16.*cmath.pi**2)',
                        order = {'QCD':2})

R2GC_379_137 = Coupling(name = 'R2GC_379_137',
                        value = 'G**3/(24.*cmath.pi**2)',
                        order = {'QCD':3})

R2GC_379_138 = Coupling(name = 'R2GC_379_138',
                        value = '(11*G**3)/(64.*cmath.pi**2)',
                        order = {'QCD':3})

R2GC_380_139 = Coupling(name = 'R2GC_380_139',
                        value = '(5*complex(0,1)*G**4)/(48.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_380_140 = Coupling(name = 'R2GC_380_140',
                        value = '(19*complex(0,1)*G**4)/(32.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_381_141 = Coupling(name = 'R2GC_381_141',
                        value = '(23*complex(0,1)*G**4)/(192.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_382_142 = Coupling(name = 'R2GC_382_142',
                        value = '(31*complex(0,1)*G**4)/(64.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_383_143 = Coupling(name = 'R2GC_383_143',
                        value = '(-17*complex(0,1)*G**4)/(64.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_384_144 = Coupling(name = 'R2GC_384_144',
                        value = '(-7*complex(0,1)*G**4)/(32.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_385_145 = Coupling(name = 'R2GC_385_145',
                        value = '(7*complex(0,1)*G**4)/(64.*cmath.pi**2)',
                        order = {'QCD':4})

R2GC_389_146 = Coupling(name = 'R2GC_389_146',
                        value = '(complex(0,1)*G**2*GD3x3)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_390_147 = Coupling(name = 'R2GC_390_147',
                        value = '-(complex(0,1)*G**2*GU3x3)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_392_148 = Coupling(name = 'R2GC_392_148',
                        value = '(complex(0,1)*G**2*MT)/(6.*cmath.pi**2)',
                        order = {'QCD':2})

R2GC_396_149 = Coupling(name = 'R2GC_396_149',
                        value = '(G**2*GU3x3*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_397_150 = Coupling(name = 'R2GC_397_150',
                        value = '(complex(0,1)*G**2*yb)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_398_151 = Coupling(name = 'R2GC_398_151',
                        value = '-(complex(0,1)*G**2*yt)/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_399_152 = Coupling(name = 'R2GC_399_152',
                        value = '(G**2*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_401_153 = Coupling(name = 'R2GC_401_153',
                        value = '(complex(0,1)*G**2*GU3x3*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_403_154 = Coupling(name = 'R2GC_403_154',
                        value = '(complex(0,1)*G**2*GU3x3*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_404_155 = Coupling(name = 'R2GC_404_155',
                        value = '(complex(0,1)*G**2*complexconjugate(GD3x3))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_405_156 = Coupling(name = 'R2GC_405_156',
                        value = '-(complex(0,1)*G**2*complexconjugate(GU3x3))/(3.*cmath.pi**2)',
                        order = {'QCD':2,'QED':1})

R2GC_406_157 = Coupling(name = 'R2GC_406_157',
                        value = '(complex(0,1)*G**2*TH1x1*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_407_158 = Coupling(name = 'R2GC_407_158',
                        value = '(complex(0,1)*G**2*TH1x2*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

R2GC_408_159 = Coupling(name = 'R2GC_408_159',
                        value = '-(G**2*TH3x3*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',
                        order = {'QCD':2,'QED':1})

UVGC_310_1 = Coupling(name = 'UVGC_310_1',
                      value = {-1:'(3*G**4)/(512.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_310_2 = Coupling(name = 'UVGC_310_2',
                      value = {-1:'(-3*G**4)/(512.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_311_3 = Coupling(name = 'UVGC_311_3',
                      value = {-1:'(3*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_311_4 = Coupling(name = 'UVGC_311_4',
                      value = {-1:'(-3*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_313_5 = Coupling(name = 'UVGC_313_5',
                      value = {-1:'-(complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_313_6 = Coupling(name = 'UVGC_313_6',
                      value = {-1:'(complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_314_7 = Coupling(name = 'UVGC_314_7',
                      value = {-1:'(-3*complex(0,1)*G**4)/(256.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_314_8 = Coupling(name = 'UVGC_314_8',
                      value = {-1:'(3*complex(0,1)*G**4)/(256.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_315_9 = Coupling(name = 'UVGC_315_9',
                      value = {-1:'-(complex(0,1)*G**4)/(24.*cmath.pi**2)'},
                      order = {'QCD':4})

UVGC_315_10 = Coupling(name = 'UVGC_315_10',
                       value = {-1:'(47*complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_316_11 = Coupling(name = 'UVGC_316_11',
                       value = {-1:'(-253*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_316_12 = Coupling(name = 'UVGC_316_12',
                       value = {-1:'(5*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_317_13 = Coupling(name = 'UVGC_317_13',
                       value = {-1:'(-13*complex(0,1)*G**3)/(48.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_318_14 = Coupling(name = 'UVGC_318_14',
                       value = {-1:'( 0 if MB else (complex(0,1)*G**3)/(48.*cmath.pi**2) )'},
                       order = {'QCD':3})

UVGC_318_15 = Coupling(name = 'UVGC_318_15',
                       value = {-1:'(complex(0,1)*G**3)/(48.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_318_16 = Coupling(name = 'UVGC_318_16',
                       value = {-1:'(-19*complex(0,1)*G**3)/(128.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_318_17 = Coupling(name = 'UVGC_318_17',
                       value = {-1:'-(complex(0,1)*G**3)/(128.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_318_18 = Coupling(name = 'UVGC_318_18',
                       value = {-1:'( 0 if MT else (complex(0,1)*G**3)/(48.*cmath.pi**2) )'},
                       order = {'QCD':3})

UVGC_318_19 = Coupling(name = 'UVGC_318_19',
                       value = {-1:'(complex(0,1)*G**3)/(12.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_320_20 = Coupling(name = 'UVGC_320_20',
                       value = {-1:'(complex(0,1)*G**2*GU2x2*TH2x1)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_321_21 = Coupling(name = 'UVGC_321_21',
                       value = {-1:'(complex(0,1)*G**2*GU2x2*TH2x2)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_322_22 = Coupling(name = 'UVGC_322_22',
                       value = {-1:'(G**2*GU2x2*TH3x3)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_323_23 = Coupling(name = 'UVGC_323_23',
                       value = {-1:'(complex(0,1)*G**2*TH2x1*complexconjugate(GU2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_324_24 = Coupling(name = 'UVGC_324_24',
                       value = {-1:'(complex(0,1)*G**2*TH2x2*complexconjugate(GU2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_325_25 = Coupling(name = 'UVGC_325_25',
                       value = {-1:'-(G**2*TH3x3*complexconjugate(GU2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_328_26 = Coupling(name = 'UVGC_328_26',
                       value = {-1:'(complex(0,1)*G**2*GD1x1*TH2x1)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_329_27 = Coupling(name = 'UVGC_329_27',
                       value = {-1:'(complex(0,1)*G**2*GD1x1*TH2x2)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_330_28 = Coupling(name = 'UVGC_330_28',
                       value = {-1:'-(G**2*GD1x1*TH3x3)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_331_29 = Coupling(name = 'UVGC_331_29',
                       value = {-1:'(complex(0,1)*G**2*TH2x1*complexconjugate(GD1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_332_30 = Coupling(name = 'UVGC_332_30',
                       value = {-1:'(complex(0,1)*G**2*TH2x2*complexconjugate(GD1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_333_31 = Coupling(name = 'UVGC_333_31',
                       value = {-1:'(G**2*TH3x3*complexconjugate(GD1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_336_32 = Coupling(name = 'UVGC_336_32',
                       value = {-1:'(complex(0,1)*G**2*GD2x2*TH2x1)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_337_33 = Coupling(name = 'UVGC_337_33',
                       value = {-1:'(complex(0,1)*G**2*GD2x2*TH2x2)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_338_34 = Coupling(name = 'UVGC_338_34',
                       value = {-1:'-(G**2*GD2x2*TH3x3)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_339_35 = Coupling(name = 'UVGC_339_35',
                       value = {-1:'(complex(0,1)*G**2*TH2x1*complexconjugate(GD2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_340_36 = Coupling(name = 'UVGC_340_36',
                       value = {-1:'(complex(0,1)*G**2*TH2x2*complexconjugate(GD2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_341_37 = Coupling(name = 'UVGC_341_37',
                       value = {-1:'(G**2*TH3x3*complexconjugate(GD2x2))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_345_38 = Coupling(name = 'UVGC_345_38',
                       value = {-1:'(complex(0,1)*G**2*GU1x1*TH2x1)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_346_39 = Coupling(name = 'UVGC_346_39',
                       value = {-1:'(complex(0,1)*G**2*GU1x1*TH2x2)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_347_40 = Coupling(name = 'UVGC_347_40',
                       value = {-1:'(G**2*GU1x1*TH3x3)/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_348_41 = Coupling(name = 'UVGC_348_41',
                       value = {-1:'(complex(0,1)*G**2*TH2x1*complexconjugate(GU1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_349_42 = Coupling(name = 'UVGC_349_42',
                       value = {-1:'(complex(0,1)*G**2*TH2x2*complexconjugate(GU1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_350_43 = Coupling(name = 'UVGC_350_43',
                       value = {-1:'-(G**2*TH3x3*complexconjugate(GU1x1))/(4.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_351_44 = Coupling(name = 'UVGC_351_44',
                       value = {-1:'-(complex(0,1)*G**2*GD2x2)/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_351_45 = Coupling(name = 'UVGC_351_45',
                       value = {-1:'(complex(0,1)*G**2*GD2x2)/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_352_46 = Coupling(name = 'UVGC_352_46',
                       value = {-1:'(complex(0,1)*G**2*GU2x2)/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_352_47 = Coupling(name = 'UVGC_352_47',
                       value = {-1:'-(complex(0,1)*G**2*GU2x2)/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_353_48 = Coupling(name = 'UVGC_353_48',
                       value = {-1:'(complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_353_49 = Coupling(name = 'UVGC_353_49',
                       value = {-1:'-(complex(0,1)*G**2*gw)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_354_50 = Coupling(name = 'UVGC_354_50',
                       value = {-1:'-(complex(0,1)*G**2*complexconjugate(GD2x2))/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_354_51 = Coupling(name = 'UVGC_354_51',
                       value = {-1:'(complex(0,1)*G**2*complexconjugate(GD2x2))/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_355_52 = Coupling(name = 'UVGC_355_52',
                       value = {-1:'(complex(0,1)*G**2*complexconjugate(GU2x2))/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_355_53 = Coupling(name = 'UVGC_355_53',
                       value = {-1:'-(complex(0,1)*G**2*complexconjugate(GU2x2))/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_356_54 = Coupling(name = 'UVGC_356_54',
                       value = {-1:'-(complex(0,1)*G**2*GD1x1)/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_356_55 = Coupling(name = 'UVGC_356_55',
                       value = {-1:'(complex(0,1)*G**2*GD1x1)/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_357_56 = Coupling(name = 'UVGC_357_56',
                       value = {-1:'(complex(0,1)*G**2*GU1x1)/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_357_57 = Coupling(name = 'UVGC_357_57',
                       value = {-1:'-(complex(0,1)*G**2*GU1x1)/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_359_58 = Coupling(name = 'UVGC_359_58',
                       value = {-1:'-(complex(0,1)*G**2*complexconjugate(GD1x1))/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_359_59 = Coupling(name = 'UVGC_359_59',
                       value = {-1:'(complex(0,1)*G**2*complexconjugate(GD1x1))/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_360_60 = Coupling(name = 'UVGC_360_60',
                       value = {-1:'(complex(0,1)*G**2*complexconjugate(GU1x1))/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_360_61 = Coupling(name = 'UVGC_360_61',
                       value = {-1:'-(complex(0,1)*G**2*complexconjugate(GU1x1))/(3.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_361_62 = Coupling(name = 'UVGC_361_62',
                       value = {-1:'( (complex(0,1)*G**2)/(6.*cmath.pi**2) if MB else -(complex(0,1)*G**2)/(12.*cmath.pi**2) ) + (complex(0,1)*G**2)/(12.*cmath.pi**2)',0:'( (5*complex(0,1)*G**2)/(12.*cmath.pi**2) - (complex(0,1)*G**2*reglog(MB/MU_R))/(2.*cmath.pi**2) if MB else (complex(0,1)*G**2)/(12.*cmath.pi**2) ) - (complex(0,1)*G**2)/(12.*cmath.pi**2)'},
                       order = {'QCD':2})

UVGC_362_63 = Coupling(name = 'UVGC_362_63',
                       value = {-1:'( -(complex(0,1)*G**3)/(6.*cmath.pi**2) if MB else (complex(0,1)*G**3)/(12.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**3)/(12.*cmath.pi**2) + (complex(0,1)*G**3*reglog(MB/MU_R))/(2.*cmath.pi**2) if MB else -(complex(0,1)*G**3)/(12.*cmath.pi**2) ) + (complex(0,1)*G**3)/(12.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_363_64 = Coupling(name = 'UVGC_363_64',
                       value = {-1:'( (cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2) if MB else -(cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2)',0:'( (5*cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) - (cw*complex(0,1)*G**2*g1*reglog(MB/MU_R))/(6.*cmath.pi**2) if MB else (cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_364_65 = Coupling(name = 'UVGC_364_65',
                       value = {-1:'( (complex(0,1)*G**2*MB)/(6.*cmath.pi**2) if MB else -(complex(0,1)*G**2*MB)/(12.*cmath.pi**2) ) + (complex(0,1)*G**2*MB)/(3.*cmath.pi**2)',0:'( (3*complex(0,1)*G**2*MB)/(4.*cmath.pi**2) - (complex(0,1)*G**2*MB*reglog(MB/MU_R))/cmath.pi**2 if MB else (complex(0,1)*G**2*MB)/(12.*cmath.pi**2) ) - (complex(0,1)*G**2*MB)/(12.*cmath.pi**2)'},
                       order = {'QCD':2})

UVGC_365_66 = Coupling(name = 'UVGC_365_66',
                       value = {-1:'( (cw*complex(0,1)*G**2*gw)/(12.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2) if MB else -(cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2)',0:'( (5*cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (5*complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) - (cw*complex(0,1)*G**2*gw*reglog(MB/MU_R))/(4.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_366_67 = Coupling(name = 'UVGC_366_67',
                       value = {-1:'( -(complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2) if MB else (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2) ) - (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2)',0:'( (-5*complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw*reglog(MB/MU_R))/(6.*cmath.pi**2) if MB else -(complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2) ) + (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_367_68 = Coupling(name = 'UVGC_367_68',
                       value = {-1:'( -(cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(12.*cmath.pi**2) if MB else (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2)',0:'( (-5*cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) + (5*complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) + (cw*complex(0,1)*G**2*g1*reglog(MB/MU_R))/(12.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else -(cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2)'},
                       order = {'QCD':2,'QED':1})

UVGC_368_69 = Coupling(name = 'UVGC_368_69',
                       value = {-1:'( -(G**2*GD3x3*TH3x3)/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else (G**2*GD3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*GD3x3*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (-5*G**2*GD3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) + (G**2*GD3x3*TH3x3*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else -(G**2*GD3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*GD3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_369_70 = Coupling(name = 'UVGC_369_70',
                       value = {-1:'( -(G**2*yb)/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else (G**2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (-3*G**2*yb)/(4.*cmath.pi**2*cmath.sqrt(2)) + (G**2*yb*reglog(MB/MU_R))/(cmath.pi**2*cmath.sqrt(2)) if MB else -(G**2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*yb)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_370_71 = Coupling(name = 'UVGC_370_71',
                       value = {0:'( (complex(0,1)*G**2*TH1x1*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else 0 )'},
                       order = {'QCD':2,'QED':1})

UVGC_371_72 = Coupling(name = 'UVGC_371_72',
                       value = {-1:'( (complex(0,1)*G**2*GD3x3*TH2x1)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yb)/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else -(complex(0,1)*G**2*GD3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*GD3x3*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*GD3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*GD3x3*TH2x1*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else (complex(0,1)*G**2*GD3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*GD3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_372_73 = Coupling(name = 'UVGC_372_73',
                       value = {0:'( (complex(0,1)*G**2*TH1x2*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else 0 )'},
                       order = {'QCD':2,'QED':1})

UVGC_373_74 = Coupling(name = 'UVGC_373_74',
                       value = {-1:'( (complex(0,1)*G**2*GD3x3*TH2x2)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yb)/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else -(complex(0,1)*G**2*GD3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*GD3x3*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yb)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*GD3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*GD3x3*TH2x2*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else (complex(0,1)*G**2*GD3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*GD3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_374_75 = Coupling(name = 'UVGC_374_75',
                       value = {-1:'( (complex(0,1)*G**2*TH1x1*yb)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else -(complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*TH1x1*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3)*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else (complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*TH1x1*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_375_76 = Coupling(name = 'UVGC_375_76',
                       value = {-1:'( (complex(0,1)*G**2*TH1x2*yb)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else -(complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*TH1x2*yb)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yb*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3)*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else (complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*TH1x2*yb)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_376_77 = Coupling(name = 'UVGC_376_77',
                       value = {-1:'( (G**2*TH3x3*complexconjugate(GD3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MB else -(G**2*TH3x3*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*TH3x3*complexconjugate(GD3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*G**2*TH3x3*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) - (G**2*TH3x3*complexconjugate(GD3x3)*reglog(MB/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MB else (G**2*TH3x3*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*TH3x3*complexconjugate(GD3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                       order = {'QCD':2,'QED':1})

UVGC_377_78 = Coupling(name = 'UVGC_377_78',
                       value = {-1:'( 0 if MB else -(complex(0,1)*G**2)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2)/(24.*cmath.pi**2)',0:'( -(complex(0,1)*G**2*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                       order = {'QCD':2})

UVGC_377_79 = Coupling(name = 'UVGC_377_79',
                       value = {-1:'( 0 if MT else -(complex(0,1)*G**2)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2)/(24.*cmath.pi**2)',0:'( -(complex(0,1)*G**2*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                       order = {'QCD':2})

UVGC_378_80 = Coupling(name = 'UVGC_378_80',
                       value = {-1:'( 0 if MB else (complex(0,1)*G**2)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2)/(24.*cmath.pi**2)',0:'( (complex(0,1)*G**2*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                       order = {'QCD':2})

UVGC_378_81 = Coupling(name = 'UVGC_378_81',
                       value = {-1:'(3*complex(0,1)*G**2)/(64.*cmath.pi**2)'},
                       order = {'QCD':2})

UVGC_378_82 = Coupling(name = 'UVGC_378_82',
                       value = {-1:'(-3*complex(0,1)*G**2)/(64.*cmath.pi**2)'},
                       order = {'QCD':2})

UVGC_378_83 = Coupling(name = 'UVGC_378_83',
                       value = {-1:'( 0 if MT else (complex(0,1)*G**2)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2)/(24.*cmath.pi**2)',0:'( (complex(0,1)*G**2*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                       order = {'QCD':2})

UVGC_379_84 = Coupling(name = 'UVGC_379_84',
                       value = {-1:'( 0 if MB else -G**3/(16.*cmath.pi**2) ) + G**3/(24.*cmath.pi**2)',0:'( -(G**3*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                       order = {'QCD':3})

UVGC_379_85 = Coupling(name = 'UVGC_379_85',
                       value = {-1:'-G**3/(48.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_379_86 = Coupling(name = 'UVGC_379_86',
                       value = {-1:'(21*G**3)/(64.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_379_87 = Coupling(name = 'UVGC_379_87',
                       value = {-1:'G**3/(64.*cmath.pi**2)'},
                       order = {'QCD':3})

UVGC_379_88 = Coupling(name = 'UVGC_379_88',
                       value = {-1:'( 0 if MT else -G**3/(16.*cmath.pi**2) ) + G**3/(24.*cmath.pi**2)',0:'( -(G**3*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                       order = {'QCD':3})

UVGC_380_89 = Coupling(name = 'UVGC_380_89',
                       value = {-1:'( 0 if MB else -(complex(0,1)*G**4)/(12.*cmath.pi**2) ) + (complex(0,1)*G**4)/(12.*cmath.pi**2)',0:'( -(complex(0,1)*G**4*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                       order = {'QCD':4})

UVGC_380_90 = Coupling(name = 'UVGC_380_90',
                       value = {-1:'(147*complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_380_91 = Coupling(name = 'UVGC_380_91',
                       value = {-1:'(3*complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_380_92 = Coupling(name = 'UVGC_380_92',
                       value = {-1:'( 0 if MT else -(complex(0,1)*G**4)/(12.*cmath.pi**2) ) + (complex(0,1)*G**4)/(12.*cmath.pi**2)',0:'( -(complex(0,1)*G**4*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                       order = {'QCD':4})

UVGC_381_93 = Coupling(name = 'UVGC_381_93',
                       value = {-1:'(147*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_381_94 = Coupling(name = 'UVGC_381_94',
                       value = {-1:'(21*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_382_95 = Coupling(name = 'UVGC_382_95',
                       value = {-1:'( 0 if MB else -(complex(0,1)*G**4)/(12.*cmath.pi**2) )',0:'( -(complex(0,1)*G**4*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                       order = {'QCD':4})

UVGC_382_96 = Coupling(name = 'UVGC_382_96',
                       value = {-1:'-(complex(0,1)*G**4)/(12.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_382_97 = Coupling(name = 'UVGC_382_97',
                       value = {-1:'(523*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_382_98 = Coupling(name = 'UVGC_382_98',
                       value = {-1:'(13*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                       order = {'QCD':4})

UVGC_382_99 = Coupling(name = 'UVGC_382_99',
                       value = {-1:'( 0 if MT else -(complex(0,1)*G**4)/(12.*cmath.pi**2) )',0:'( -(complex(0,1)*G**4*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                       order = {'QCD':4})

UVGC_383_100 = Coupling(name = 'UVGC_383_100',
                        value = {-1:'( 0 if MB else (complex(0,1)*G**4)/(12.*cmath.pi**2) ) - (complex(0,1)*G**4)/(24.*cmath.pi**2)',0:'( (complex(0,1)*G**4*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                        order = {'QCD':4})

UVGC_383_101 = Coupling(name = 'UVGC_383_101',
                        value = {-1:'(complex(0,1)*G**4)/(24.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_383_102 = Coupling(name = 'UVGC_383_102',
                        value = {-1:'(-341*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_383_103 = Coupling(name = 'UVGC_383_103',
                        value = {-1:'(-11*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_383_104 = Coupling(name = 'UVGC_383_104',
                        value = {-1:'( 0 if MT else (complex(0,1)*G**4)/(12.*cmath.pi**2) ) - (complex(0,1)*G**4)/(24.*cmath.pi**2)',0:'( (complex(0,1)*G**4*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                        order = {'QCD':4})

UVGC_384_105 = Coupling(name = 'UVGC_384_105',
                        value = {-1:'(-83*complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_384_106 = Coupling(name = 'UVGC_384_106',
                        value = {-1:'(-5*complex(0,1)*G**4)/(128.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_385_107 = Coupling(name = 'UVGC_385_107',
                        value = {-1:'( 0 if MB else (complex(0,1)*G**4)/(12.*cmath.pi**2) )',0:'( (complex(0,1)*G**4*reglog(MB/MU_R))/(12.*cmath.pi**2) if MB else 0 )'},
                        order = {'QCD':4})

UVGC_385_108 = Coupling(name = 'UVGC_385_108',
                        value = {-1:'(complex(0,1)*G**4)/(12.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_385_109 = Coupling(name = 'UVGC_385_109',
                        value = {-1:'(-85*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_385_110 = Coupling(name = 'UVGC_385_110',
                        value = {-1:'(-19*complex(0,1)*G**4)/(512.*cmath.pi**2)'},
                        order = {'QCD':4})

UVGC_385_111 = Coupling(name = 'UVGC_385_111',
                        value = {-1:'( 0 if MT else (complex(0,1)*G**4)/(12.*cmath.pi**2) )',0:'( (complex(0,1)*G**4*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else 0 )'},
                        order = {'QCD':4})

UVGC_386_112 = Coupling(name = 'UVGC_386_112',
                        value = {-1:'( (complex(0,1)*G**2)/(6.*cmath.pi**2) if MT else -(complex(0,1)*G**2)/(12.*cmath.pi**2) ) + (complex(0,1)*G**2)/(12.*cmath.pi**2)',0:'( (5*complex(0,1)*G**2)/(12.*cmath.pi**2) - (complex(0,1)*G**2*reglog(MT/MU_R))/(2.*cmath.pi**2) if MT else (complex(0,1)*G**2)/(12.*cmath.pi**2) ) - (complex(0,1)*G**2)/(12.*cmath.pi**2)'},
                        order = {'QCD':2})

UVGC_387_113 = Coupling(name = 'UVGC_387_113',
                        value = {-1:'( -(complex(0,1)*G**3)/(6.*cmath.pi**2) if MT else (complex(0,1)*G**3)/(12.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**3)/(12.*cmath.pi**2) + (complex(0,1)*G**3*reglog(MT/MU_R))/(2.*cmath.pi**2) if MT else -(complex(0,1)*G**3)/(12.*cmath.pi**2) ) + (complex(0,1)*G**3)/(12.*cmath.pi**2)'},
                        order = {'QCD':3})

UVGC_388_114 = Coupling(name = 'UVGC_388_114',
                        value = {-1:'( -(cw*complex(0,1)*G**2*g1)/(9.*cmath.pi**2) if MT else (cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2)',0:'( (-5*cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2) + (cw*complex(0,1)*G**2*g1*reglog(MT/MU_R))/(3.*cmath.pi**2) if MT else -(cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*g1)/(18.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_389_115 = Coupling(name = 'UVGC_389_115',
                        value = {-1:'( (complex(0,1)*G**2*GD3x3)/(12.*cmath.pi**2) if MB else -(complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) )',0:'( (5*complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else (complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_389_116 = Coupling(name = 'UVGC_389_116',
                        value = {-1:'( (complex(0,1)*G**2*GD3x3)/(12.*cmath.pi**2) if MT else -(complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) )',0:'( (5*complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) - (complex(0,1)*G**2*GD3x3*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else (complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*GD3x3)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_389_117 = Coupling(name = 'UVGC_389_117',
                        value = {-1:'(complex(0,1)*G**2*GD3x3)/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_390_118 = Coupling(name = 'UVGC_390_118',
                        value = {-1:'( -(complex(0,1)*G**2*GU3x3)/(12.*cmath.pi**2) if MB else (complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) + (complex(0,1)*G**2*GU3x3*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else -(complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_390_119 = Coupling(name = 'UVGC_390_119',
                        value = {-1:'( -(complex(0,1)*G**2*GU3x3)/(12.*cmath.pi**2) if MT else (complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) + (complex(0,1)*G**2*GU3x3*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else -(complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*GU3x3)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_390_120 = Coupling(name = 'UVGC_390_120',
                        value = {-1:'-(complex(0,1)*G**2*GU3x3)/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_391_121 = Coupling(name = 'UVGC_391_121',
                        value = {-1:'( -(complex(0,1)*G**2*gw)/(12.*cmath.pi**2*cmath.sqrt(2)) if MB else (complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) )',0:'( (-5*complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*gw*reglog(MB/MU_R))/(4.*cmath.pi**2*cmath.sqrt(2)) if MB else -(complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_391_122 = Coupling(name = 'UVGC_391_122',
                        value = {-1:'( -(complex(0,1)*G**2*gw)/(12.*cmath.pi**2*cmath.sqrt(2)) if MT else (complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) )',0:'( (-5*complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*gw*reglog(MT/MU_R))/(4.*cmath.pi**2*cmath.sqrt(2)) if MT else -(complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*gw)/(24.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_392_123 = Coupling(name = 'UVGC_392_123',
                        value = {-1:'( (complex(0,1)*G**2*MT)/(6.*cmath.pi**2) if MT else -(complex(0,1)*G**2*MT)/(12.*cmath.pi**2) ) + (complex(0,1)*G**2*MT)/(3.*cmath.pi**2)',0:'( (3*complex(0,1)*G**2*MT)/(4.*cmath.pi**2) - (complex(0,1)*G**2*MT*reglog(MT/MU_R))/cmath.pi**2 if MT else (complex(0,1)*G**2*MT)/(12.*cmath.pi**2) ) - (complex(0,1)*G**2*MT)/(12.*cmath.pi**2)'},
                        order = {'QCD':2})

UVGC_393_124 = Coupling(name = 'UVGC_393_124',
                        value = {-1:'( -(cw*complex(0,1)*G**2*gw)/(12.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(36.*cmath.pi**2) if MT else (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2)',0:'( (-5*cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (5*complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) + (cw*complex(0,1)*G**2*gw*reglog(MT/MU_R))/(4.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw*reglog(MT/MU_R))/(12.*cmath.pi**2) if MT else -(cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) + (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*gw)/(24.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw)/(72.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_394_125 = Coupling(name = 'UVGC_394_125',
                        value = {-1:'( (complex(0,1)*G**2*g1*sw)/(9.*cmath.pi**2) if MT else -(complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2) ) + (complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2)',0:'( (5*complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2) - (complex(0,1)*G**2*g1*sw*reglog(MT/MU_R))/(3.*cmath.pi**2) if MT else (complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2) ) - (complex(0,1)*G**2*g1*sw)/(18.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_395_126 = Coupling(name = 'UVGC_395_126',
                        value = {-1:'( -(cw*complex(0,1)*G**2*g1)/(36.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(12.*cmath.pi**2) if MT else (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) ) - (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2)',0:'( (-5*cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) - (5*complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) + (cw*complex(0,1)*G**2*g1*reglog(MT/MU_R))/(12.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else -(cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) - (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2) ) + (cw*complex(0,1)*G**2*g1)/(72.*cmath.pi**2) + (complex(0,1)*G**2*gw*sw)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_396_127 = Coupling(name = 'UVGC_396_127',
                        value = {-1:'( (G**2*GU3x3*TH3x3)/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(G**2*GU3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*GU3x3*TH3x3)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*G**2*GU3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) - (G**2*GU3x3*TH3x3*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else (G**2*GU3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*GU3x3*TH3x3)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_397_128 = Coupling(name = 'UVGC_397_128',
                        value = {-1:'( (complex(0,1)*G**2*yb)/(12.*cmath.pi**2) if MB else -(complex(0,1)*G**2*yb)/(24.*cmath.pi**2) )',0:'( (13*complex(0,1)*G**2*yb)/(24.*cmath.pi**2) - (3*complex(0,1)*G**2*yb*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else (complex(0,1)*G**2*yb)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*yb)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_397_129 = Coupling(name = 'UVGC_397_129',
                        value = {-1:'( (complex(0,1)*G**2*yb)/(12.*cmath.pi**2) if MT else -(complex(0,1)*G**2*yb)/(24.*cmath.pi**2) )',0:'( (5*complex(0,1)*G**2*yb)/(24.*cmath.pi**2) - (complex(0,1)*G**2*yb*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else (complex(0,1)*G**2*yb)/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*yb)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_397_130 = Coupling(name = 'UVGC_397_130',
                        value = {-1:'(complex(0,1)*G**2*yb)/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_398_131 = Coupling(name = 'UVGC_398_131',
                        value = {-1:'( -(complex(0,1)*G**2*yt)/(12.*cmath.pi**2) if MB else (complex(0,1)*G**2*yt)/(24.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**2*yt)/(24.*cmath.pi**2) + (complex(0,1)*G**2*yt*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else -(complex(0,1)*G**2*yt)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*yt)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_398_132 = Coupling(name = 'UVGC_398_132',
                        value = {-1:'( -(complex(0,1)*G**2*yt)/(12.*cmath.pi**2) if MT else (complex(0,1)*G**2*yt)/(24.*cmath.pi**2) )',0:'( (-13*complex(0,1)*G**2*yt)/(24.*cmath.pi**2) + (3*complex(0,1)*G**2*yt*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else -(complex(0,1)*G**2*yt)/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*yt)/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_398_133 = Coupling(name = 'UVGC_398_133',
                        value = {-1:'-(complex(0,1)*G**2*yt)/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_399_134 = Coupling(name = 'UVGC_399_134',
                        value = {-1:'( (G**2*yt)/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(G**2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (3*G**2*yt)/(4.*cmath.pi**2*cmath.sqrt(2)) - (G**2*yt*reglog(MT/MU_R))/(cmath.pi**2*cmath.sqrt(2)) if MT else (G**2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*yt)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_400_135 = Coupling(name = 'UVGC_400_135',
                        value = {0:'( (complex(0,1)*G**2*TH1x1*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else 0 )'},
                        order = {'QCD':2,'QED':1})

UVGC_401_136 = Coupling(name = 'UVGC_401_136',
                        value = {-1:'( (complex(0,1)*G**2*GU3x3*TH2x1)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yt)/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(complex(0,1)*G**2*GU3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*GU3x3*TH2x1)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*GU3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*GU3x3*TH2x1*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else (complex(0,1)*G**2*GU3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*GU3x3*TH2x1)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_402_137 = Coupling(name = 'UVGC_402_137',
                        value = {0:'( (complex(0,1)*G**2*TH1x2*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else 0 )'},
                        order = {'QCD':2,'QED':1})

UVGC_403_138 = Coupling(name = 'UVGC_403_138',
                        value = {-1:'( (complex(0,1)*G**2*GU3x3*TH2x2)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yt)/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(complex(0,1)*G**2*GU3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*GU3x3*TH2x2)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yt)/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*GU3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*GU3x3*TH2x2*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else (complex(0,1)*G**2*GU3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*GU3x3*TH2x2)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_404_139 = Coupling(name = 'UVGC_404_139',
                        value = {-1:'( (complex(0,1)*G**2*complexconjugate(GD3x3))/(12.*cmath.pi**2) if MB else -(complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) )',0:'( (5*complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) - (complex(0,1)*G**2*complexconjugate(GD3x3)*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else (complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_404_140 = Coupling(name = 'UVGC_404_140',
                        value = {-1:'( (complex(0,1)*G**2*complexconjugate(GD3x3))/(12.*cmath.pi**2) if MT else -(complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) )',0:'( (5*complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) - (complex(0,1)*G**2*complexconjugate(GD3x3)*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else (complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2) ) - (complex(0,1)*G**2*complexconjugate(GD3x3))/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_404_141 = Coupling(name = 'UVGC_404_141',
                        value = {-1:'(complex(0,1)*G**2*complexconjugate(GD3x3))/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_405_142 = Coupling(name = 'UVGC_405_142',
                        value = {-1:'( -(complex(0,1)*G**2*complexconjugate(GU3x3))/(12.*cmath.pi**2) if MB else (complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) + (complex(0,1)*G**2*complexconjugate(GU3x3)*reglog(MB/MU_R))/(4.*cmath.pi**2) if MB else -(complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_405_143 = Coupling(name = 'UVGC_405_143',
                        value = {-1:'( -(complex(0,1)*G**2*complexconjugate(GU3x3))/(12.*cmath.pi**2) if MT else (complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) )',0:'( (-5*complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) + (complex(0,1)*G**2*complexconjugate(GU3x3)*reglog(MT/MU_R))/(4.*cmath.pi**2) if MT else -(complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2) ) + (complex(0,1)*G**2*complexconjugate(GU3x3))/(24.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_405_144 = Coupling(name = 'UVGC_405_144',
                        value = {-1:'-(complex(0,1)*G**2*complexconjugate(GU3x3))/(3.*cmath.pi**2)'},
                        order = {'QCD':2,'QED':1})

UVGC_406_145 = Coupling(name = 'UVGC_406_145',
                        value = {-1:'( (complex(0,1)*G**2*TH1x1*yt)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*TH1x1*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x1*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3)*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else (complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*TH1x1*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x1*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_407_146 = Coupling(name = 'UVGC_407_146',
                        value = {-1:'( (complex(0,1)*G**2*TH1x2*yt)/(6.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else -(complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (complex(0,1)*G**2*TH1x2*yt)/(3.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (5*complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) + (5*complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH1x2*yt*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3)*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else (complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) + (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (complex(0,1)*G**2*TH1x2*yt)/(12.*cmath.pi**2*cmath.sqrt(2)) - (complex(0,1)*G**2*TH2x2*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

UVGC_408_147 = Coupling(name = 'UVGC_408_147',
                        value = {-1:'( -(G**2*TH3x3*complexconjugate(GU3x3))/(6.*cmath.pi**2*cmath.sqrt(2)) if MT else (G**2*TH3x3*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) - (G**2*TH3x3*complexconjugate(GU3x3))/(3.*cmath.pi**2*cmath.sqrt(2))',0:'( (-5*G**2*TH3x3*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) + (G**2*TH3x3*complexconjugate(GU3x3)*reglog(MT/MU_R))/(2.*cmath.pi**2*cmath.sqrt(2)) if MT else -(G**2*TH3x3*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2)) ) + (G**2*TH3x3*complexconjugate(GU3x3))/(12.*cmath.pi**2*cmath.sqrt(2))'},
                        order = {'QCD':2,'QED':1})

