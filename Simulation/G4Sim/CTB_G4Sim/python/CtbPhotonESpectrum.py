# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import math
from G4AtlasApps.atlas_utilhisto import HistoAtHistoSvc
class Photon_Energy_spectrum(object):
    """
    This Photon Energy spectrum data was produced by T. Koffas.
    
    """
    def __init__(self,run_number):
        self.run_number=run_number
        self.init_Par()
        
    def build_efunction(self,theApp):
        """
        build spectrum for electron energy calculation. needed by SingleParticleGenerator
        
        """
        
        nbins=120000*0.01
        self.e_spectrum=HistoAtHistoSvc('espectrum')
        self.e_spectrum.book_histo(theApp,int(nbins),0,120000)
        e_spectrumaxis=self.e_spectrum.HistoSvcEntry.axis()
        for i in range(e_spectrumaxis.bins()):
            x=(e_spectrumaxis.binUpperEdge(i)+e_spectrumaxis.binLowerEdge(i))/2.
            y = 180000-x
            self.e_spectrum.HistoSvcEntry.fill(x,y)
            
    def init_Par(self):

        if(self.run_number==2777):
            self.norm= 106660.
            self.histoRange=[0.,80000.]
            self.npar=15
            self.ndim=1
            self.imqfun=2
            self.vconst=2.
            self.sigvmi=[0.]
            self.sigvt=[200.]
            self.sigv=[0.1049999967217, 0.2949999868870, 0.4650000035763,
                       0.4849999845028, 0.000000000000, 1.000000000000,
                       0.2999999932945e-1, 0.1899999976158, 0.3299999833107,
                       0.3499999940395, 0.3899999856949, 0.4099999964237,
                       0.5399999618530, 0.5799999833107, 0.6800000071526]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                         0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                         0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                         0.7999999821186e-1, 0.7999999821186e-1, 0.1599999964237]
            self.siga=[-2.598207731747, 4.312246446869, -31.46227216602,
                       40.90442603333, 10.19151770072, 10.28616655621,
                       -2.329979537579, 1.368288158710, 26.47073822099,
                       -88.11523715853, -21.79638506140, 79.09065390897,
                       -114.5304311642, 115.7673575446, -34.35996528351]
            self.photon=0.
            
        elif(self.run_number==2857):
            self.norm=365664.
            self.histoRange=[0.,80000.]
            self.npar=17
            self.ndim=1
            self.imqfun=2
            self.vconst=2.
            self.sigvmi=[0.]
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.7499999552965e-1,
                       0.1949999928474, 0.2249999940395, 0.2549999952316,
                       0.2750000059605, 0.3050000071526, 0.3149999976158,
                       0.3249999880791, 0.3349999785423, 0.000000000000,
                       1.000000000000, 0.1299999952316, 0.1700000017881,
                       0.3799999952316, 0.5199999809265]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                         0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                         0.7999999821186e-1, 0.1599999964237]
            self.siga=[-1476.889556520, 764.5997024809, 20.39032204037,
                       13.93251046364, -17.19068470999, -10.30051418087,
                       -23.91052791023, -87.05159048677, 164.6286518027,
                       -286.9730931994, 204.9446308145, 689.8503539772,
                       3.411995820446, 2.781409973380, 11.14755985026,
                       26.68922609713, -7.732407966270]
            self.photon=0.
        elif(self.run_number==2775):
            self.norm=108259.
            self.histoRange=[0.,100000.]
            self.npar=13 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1499999966472e-1, 0.9499999880791e-1, 0.2049999982119,    
                       0.3349999785423, 0.3649999797344, 0.3949999809265,
                       0.000000000000, 1.000000000000, 0.1299999952316,    
                       0.2899999916553, 0.4599999785423, 0.5999999642372,    
                       0.6800000071526]    
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                         0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                         0.3999999910593e-1, 0.7999999821186e-1, 0.1599999964237,  
                         0.1599999964237]
            self.siga=[-152.7190885954, 35.48247587761, 8.104305042246,
                       -19.62208011139, -30.12432826417, 14.52757428360,
                       139.7616185499, -7.526751401043, -24.98750743251,    
                       0.1701115668216, 59.16559249394, -172.1215214067,
                       140.1786981310]
            self.photon=0.

        elif(self.run_number==2778):
            
            self.norm=102392.
            self.histoRange=[0.,100000.]
            self.npar=13 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.8500000089407e-1, 0.3849999904633, 0.000000000000,
                  1.000000000000, 0.2500000000000, 0.2899999916553,
                  0.3299999833107, 0.3499999940395, 0.4099999964237,
                  0.4299999773502, 0.5699999928474, 0.1799999922514,
                  0.5000000000000]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                    0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                    0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                    0.3999999910593e-1, 0.3999999910593e-1, 0.7999999821186e-1,
                    0.7999999821186e-1]
            self.siga=[6.516935638161, -51.53242538090, -5.985440975344,
                  10.71717088688, 9.487970703482, -6.754409720428,
                  19.63931783482, -84.51171037399, 256.1127676640,
                  -213.4994667573, -62.16238932562, -8.460112041532,
                  110.7723886847]
            self.photon=0.
        elif(self.run_number==2779):
            
            self.norm=74268.9
            self.histoRange=[0.,120000.]
            self.npar=14 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1149999946356, 0.4249999821186, 0.4549999833107,
                  0.000000000000, 1.000000000000, 0.3299999833107,
                  0.3499999940395, 0.3899999856949, 0.4899999797344,
                  0.2199999988079, 0.2599999904633, 0.5399999618530,
                  0.5799999833107, 0.7599999904633]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                    0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                    0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                    0.7999999821186e-1, 0.7999999821186e-1, 0.7999999821186e-1,
                    0.7999999821186e-1, 0.1599999964237]
            self.siga=[17.29220352013, 21.62634261800, -41.90599447647,
                  0.7481309185077, 3.021035997494, -10.43705744186,
                  29.71680574492, -58.10556048829, 66.30307584860,
                  -73.74468119486, 74.04344297681, -100.2089558151,    
                  70.93218554771, -1.739743452950]
            self.photon=0.
 
        elif(self.run_number==2780):
            
            self.norm=33537.1
            self.histoRange=[0.,120000.]
            self.npar=16 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1649999916553, 0.1850000023842, 0.2649999856949,
                 0.2750000059605, 0.4849999845028, 0.4949999749660,
                 0.5049999952316, 0.000000000000, 1.000000000000,
                 0.7000000029802e-1, 0.2099999934435, 0.3100000023842,    
                 0.3299999833107, 0.3499999940395, 0.4199999868870,
                 0.5799999833107]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.7999999821186e-1,
                   0.7999999821186e-1]
            self.siga=[3.356677438944, 23.58548892063, 15.12208325199,    
                 -0.9603672617075e-1, -116.8541776068, 264.2275243155,
                 -175.7529308476, 0.2876695377515, -4.588616194708,    
                 5.905855988647, -30.43429532578, -108.3487244593,    
                 157.3446310493, -95.12081690181, 17.66927586646,    
                 38.74929437616]
            self.photon=0.

        elif(self.run_number==2782):
            
            self.norm=26260.
            self.histoRange=[0.,120000.]
            self.npar=11 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.3349999785423, 0.000000000000, 1.000000000000,    
                 0.4999999701977e-1, 0.7000000029802e-1, 0.2500000000000,   
                 0.5099999904633, 0.3799999952316, 0.4199999868870,
                 0.4599999785423, 0.5999999642372]
            self.sigdel=[0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.7999999821186e-1, 0.7999999821186e-1,
                   0.7999999821186e-1, 0.1599999964237]
            self.siga=[11.20566522025, -61.27536363253, 348.9016798262, 
                 -195.3058604642, 168.0898330379, 11.60328335659,   
                 211.6019990459, -146.3760941543, 342.2693244055,    
                 -181.6180788787, -760.5425686748]
            self.photon=0.

        elif(self.run_number==2783):
            
            self.norm=90556.1
            self.histoRange=[0.,120000.]
            self.npar=12
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.3500000014901e-1, 0.4499999806285e-1, 0.5349999666214,    
                 0.000000000000, 1.000000000000, 0.1299999952316,
                 0.2699999809265, 0.3499999940395, 0.4299999773502,    
                 0.4699999988079, 0.5699999928474, 0.2199999988079]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.7999999821186e-1]
            self.siga=[-29.97494565479, 8.580860883212, 15.94534847282,   
                 16.42709945331, 8.943015973389, 13.21214962226,    
                 0.3946416587598, -27.87273713360, 12.46518115880,   
                 -15.10349393620, -11.27854893653, -7.434229895740]
            self.photon=0.

        elif(self.run_number==2787):
            
            self.norm=86892.7
            self.histoRange=[0.,120000.]
            self.npar=12 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1149999946356, 0.1749999970198, 0.2349999994040,    
                 0.2449999898672, 0.4449999928474, 0.000000000000,    
                 1.000000000000, 0.1499999910593, 0.3100000023842,    
                 0.4099999964237, 0.4899999797344, 0.5999999642372]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.1599999964237] 
            self.siga=[-7.443799486777, -23.64896935054, 21.92079286120,   
                 -23.38613822431, 2.103280281472, 0.1247256994986,    
                 -3.834375941567, 38.58029285821, -18.43729353296,    
                 -12.25021831672, -4.592136450460, 25.52617404017]    
            self.photon=0.

        elif(self.run_number==2788):
            
            self.norm=32626.3
            self.histoRange=[0.,120000.]
            self.npar=14 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1149999946356, 0.1550000011921, 0.4949999749660,    
                 0.000000000000, 1.000000000000, 0.1899999976158,    
                 0.2500000000000, 0.3899999856949, 0.4299999773502,    
                 0.4499999880791, 0.4699999988079, 0.2999999821186,    
                 0.3400000035763, 0.5999999642372]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.7999999821186e-1,
                   0.7999999821186e-1, 0.1599999964237]
            self.siga=[23.78032101273, -21.06622156703, -80.60419831813,    
                 -5.345549886721, 10.88385344309, 13.35888855702,   
                 20.04332384024, -68.84217172149, 295.3031052539,    
                 -561.9346170998, 403.6825634406, -103.6863168573,    
                 75.86517108612, -16.23657006050]

        elif(self.run_number==2800):
            
            self.norm=224213.
            self.histoRange=[0.,100000.]
            self.npar=17 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1550000011921, 0.1649999916553, 0.2750000059605,    
                 0.3249999880791, 0.3649999797344, 0.3750000000000,    
                 0.4050000011921, 0.4449999928474, 0.000000000000,    
                 1.000000000000, 0.2999999932945e-1, 0.7000000029802e-1,
                 0.1099999994040, 0.1299999952316, 0.2500000000000,
                 0.5000000000000, 0.5999999642372]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.7999999821186e-1, 0.1599999964237]
            self.siga=[-41.10183674935, -28.50234184014, 6.236835692289,   
                 -41.27211108041, -98.44217454651, 102.7482656638,   
                 -15.10321437470, -19.44683645259, 146.7377763838,   
                 53.70586532227, -346.4598447219, 325.4542527234,    
                 -379.0210469959, 321.1164462638, 8.979821125991,    
                 133.3091769018, -160.2055635596]

        elif(self.run_number==2801):
            
            self.norm=218226.
            self.histoRange=[0.,100000.]
            self.npar=18 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.7499999552965e-1, 0.8500000089407e-1, 0.9499999880791e-1,
                 0.2549999952316, 0.2750000059605, 0.2849999964237,    
                 0.3050000071526, 0.3249999880791, 0.3649999797344,    
                 0.3849999904633, 0.000000000000, 1.000000000000,    
                 0.2299999892712, 0.4299999773502, 0.4499999880791,    
                 0.1400000005960, 0.1799999922514, 0.5000000000000]    
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.7999999821186e-1, 0.7999999821186e-1, 0.7999999821186e-1]
            self.siga=[-275.5751767152, 405.3660045871, -142.0390735360,    
                 -0.4013255184139, 42.34742413022, -41.61237605284,   
                 9.631536780440, -35.21700244100, -59.31407735828,    
                 94.99078479574, 9.167685581621, 12.10754430083,    
                 31.73911341565, -222.5841528838, 225.6090538173,    
                 58.30287834242, -84.48209471067, -47.79953770925]  

        elif(self.run_number==2816):
            
            self.norm=177540.
            self.histoRange=[0.,100000.]
            self.npar=17 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.3500000014901e-1,
                 0.1049999967217, 0.1749999970198, 0.2949999868870,    
                 0.3149999976158, 0.3449999988079, 0.000000000000,    
                 1.000000000000, 0.2299999892712, 0.2500000000000,    
                 0.2699999809265, 0.3700000047684, 0.3899999856949,
                 0.4099999964237, 0.5199999809265]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.1599999964237]
            self.siga=[-1478.961204434, 669.7288727422, 95.11290229111,   
                 9.129502913175, -15.56759808340, -28.43015754184,    
                 -34.71875506183, -34.34197908192, 707.5343760639,   
                 3.978721280463, 47.74933177064, -29.37653388655,    
                 12.49300040335, 280.5398610170, -476.8020081099,    
                 282.4206989620, -15.68947383459]

        elif(self.run_number==2819):
            
            self.norm=144575.
            self.histoRange=[0.,100000.]
            self.npar=14 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.] 
            self.sigv=[0.1499999966472e-1, 0.3500000014901e-1, 0.4499999806285e-1,
                 0.2349999994040, 0.2949999868870, 0.3349999785423,    
                 0.000000000000, 1.000000000000, 0.1099999994040,
                 0.1700000017881, 0.2099999934435, 0.2699999809265,    
                 0.3899999856949, 0.4599999785423]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.7999999821186e-1]
            self.siga=[-724.6348804428, 225.3655327922, 15.74261061397,   
                 7.589271400057, -42.15673808647, 15.80542601610,    
                 482.7328168033, -2.039834251557, 9.149511231159,    
                 -31.60321406196, 29.81363270503, -10.37601541331,    
                 -20.34542464681, 38.97789286480]

        elif(self.run_number==2824):
            
            self.norm=39627.5
            self.histoRange=[0.,100000.]
            self.npar=11 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.000000000000,   
                 1.000000000000, 0.2299999892712, 0.2899999916553,    
                 0.3299999833107, 0.3499999940395, 0.3700000047684,    
                 0.3899999856949, 0.1799999922514]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.7999999821186e-1]
            self.siga=[-1665.948927868, 956.0166997358, 686.3601622101,   
                 20.53682565793, 37.39182497894, -30.61264971416,    
                 -348.7119836566, 844.4451947885, -746.5546346311,    
                 209.2482941417, -17.92290565721]   

        elif(self.run_number==2841):
            
            self.norm=138074.
            self.histoRange=[0.,100000.]
            self.npar=16
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.6499999761581e-1,
                 0.1449999958277, 0.1550000011921, 0.1649999916553,    
                 0.2849999964237, 0.000000000000, 1.000000000000,
                 0.2299999892712, 0.2500000000000, 0.3299999833107,    
                 0.3499999940395, 0.3899999856949, 0.4099999964237,    
                 0.4299999773502]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1]
            self.siga=[-1783.583303722, 887.5309854665, 28.54279722093,   
                 -21.41393020411, 53.34201407571, -43.01173497038,    
                 -44.17894865823, 866.2679083250, 0.3572528605633,    
                 9.456034723727, 32.10710425315, -163.7857853731,    
                 348.9910953744, -797.4507692247, 897.2987753409,    
                 -275.8354725111]
            
        elif(self.run_number==2845):
            
            self.norm=128685.
            self.histoRange=[0.,100000.]
            self.npar=14 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.2750000059605,   
                 0.3050000071526, 0.3149999976158, 0.3349999785423,    
                 0.000000000000, 1.000000000000, 0.7000000029802e-1,
                 0.1499999910593, 0.2099999934435, 0.2299999892712,    
                 0.2500000000000, 0.4099999964237]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999994950e-5, 0.1999999994950e-5, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1]
            self.siga=[-1985.627136741, 1004.357358350, -16.62279680803,    
                 -93.29194959448, 72.38371779190, -7.159641402214,    
                 933.5811605472, 11.66754462223, 41.19910037717,    
                 -15.96327198670, -18.54381900793, 47.19495299197,    
                 1.088200065103, -6.254592085301]

        elif(self.run_number==2851):
            
            self.norm=215917.
            self.histoRange=[0.,100000.]
            self.npar=13 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.2049999982119,    
                 0.2449999898672, 0.2849999964237, 0.3249999880791,    
                 0.3349999785423, 0.3849999904633, 0.000000000000,    
                 1.000000000000, 0.1700000017881, 0.4099999964237,    
                 0.9999999403954e-1]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.7999999821186e-1]
            self.siga=[-2926.920619385, 1507.245803789, 27.89974527771,   
                 6.379645919377, -31.54627305387, -68.09751580845,    
                 66.05837205639, -70.96044401490, 1380.511740982,    
                 -2.127957640269, -47.15506060408, 87.50411712368,    
                 62.72988480687]

        elif(self.run_number==2856):
            
            self.norm=24321.8
            self.histoRange=[0.,80000.]
            self.npar=9 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1449999958277, 0.3449999988079, 0.000000000000,
                 1.000000000000, 0.2999999932945e-1, 0.4999999701977e-1,
                 0.1899999976158, 0.2899999916553, 0.4199999868870]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
                   0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.7999999821186e-1]
            self.siga=[-15.96763887470, 13.51097731847, 1.133479758950,   
                 40.30017423767, -117.4878517738, 77.92411474580,    
                 27.63971808535, -43.56847012954, -82.90815383861]  

        elif(self.run_number==2859):
            
            self.norm=268734.
            self.histoRange=[0.,80000.]
            self.npar=17 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.4499999806285e-1,
                 0.1550000011921, 0.1949999928474, 0.2149999886751,    
                 0.2349999994040, 0.2649999856949, 0.2849999964237,    
                 0.3050000071526, 0.3149999976158, 0.3249999880791,    
                 0.3549999892712, 0.000000000000, 1.000000000000,    
                 0.1099999994040, 0.4299999773502]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1]
            self.siga=[-1782.346018223, 883.0987992968, 46.06261226632,   
                 4.044193514732, 14.58078800499, -0.9473677905354,    
                 -7.858339232289, -30.23179142322, 13.29180754567,   
                 -198.6122744236, 339.8974459981, -266.8646685248,    
                 134.4623866882, 841.0564546676, 5.338112193602,    
                 11.27299683070, -20.57263997444]

        elif(self.run_number==2860):
            
            self.norm=366907.
            self.histoRange=[0.,80000.]
            self.npar=19 
            self.ndim=1 
            self.imqfun=2 
            self.vconst=2. 
            self.sigvmi=[0.] 
            self.sigvt=[200.]
            self.sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.1049999967217,    
                 0.1149999946356, 0.2049999982119, 0.2249999940395,    
                 0.2449999898672, 0.2649999856949, 0.2849999964237,    
                 0.3149999976158, 0.3249999880791, 0.3349999785423,    
                 0.3449999988079, 0.000000000000, 1.000000000000,
                 0.1700000017881, 0.3700000047684, 0.5999999642372,    
                 0.6800000071526]
            self.sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
                   0.1999999955297e-1, 0.1999999994950e-5, 0.1999999994950e-5,
                   0.3999999910593e-1, 0.3999999910593e-1, 0.1599999964237, 
                   0.1599999964237]    
            self.siga=[-1980.592349838, 1074.673853627, 50.33332589102,    
                 -41.20804315428, 5.299796500432, -0.9852921451069,    
                 -19.94209720792, -9.840014806111, -17.03063739502,    
                 -177.9149975125, 317.5291443167, -344.9206249079,    
                 226.2995897249, 879.5401944367, 65.92211795654,    
                 23.49930221282, -31.62748808614, 266.0833027776,    
                 -312.7757961452]


        else: 
            raise RuntimeError('ERROR: CTB_G4Sim::CtbPhotonESpectrum.py\
            There is not energy spectrum for this run !!!')
        
        
    def function(self,v):
        self.photon=0
        for j in range(self.npar):
            hqdj=0.
            for i in range(self.ndim):
                vv = (v - self.sigvmi[i])/self.sigvt[i]
                hqdj = hqdj + (vv - self.sigv[j])*(vv - self.sigv[j])
                hqdj = hqdj + self.sigdel[j] * self.sigdel[j]
                hqdj = math.sqrt(hqdj)
                self.photon = self.photon + self.siga[j] * hqdj
        if self.imqfun==2:
            self.photon = self.vconst*math.exp(self.photon)
            self.photon = self.photon /self.norm
        return self.photon
        
    def build_spectrum(self,theApp):
        nbins=(self.histoRange[1]-self.histoRange[0])*0.01
        print (theApp,nbins,self.histoRange[0],self.histoRange[1])
        p_espectrum=HistoAtHistoSvc(repr(self.run_number))
        p_espectrum.book_histo(theApp,int(nbins),self.histoRange[0],self.histoRange[1])
        p_espectrumaxis=p_espectrum.HistoSvcEntry.axis()
        for i in range(p_espectrumaxis.bins()):
            x=(p_espectrumaxis.binUpperEdge(i)+p_espectrumaxis.binLowerEdge(i))/2.
            y = self.function(0.1*i)
            print (x,y)
            p_espectrum.HistoSvcEntry.fill(x,y)
        return p_espectrum

