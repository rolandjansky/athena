# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def photon_energy_spectrum(v): 
    import math 
    npar=17 
    ndim=1 
    imqfun=2 
    vconst=2. 
    sigvmi=[0.] 
    sigvt=[200.] 
    sigv=[0.1499999966472e-1, 0.2499999850988e-1, 0.7499999552965e-1,
          0.1949999928474, 0.2249999940395, 0.2549999952316,
          0.2750000059605, 0.3050000071526, 0.3149999976158,
          0.3249999880791, 0.3349999785423, 0.000000000000,
          1.000000000000, 0.1299999952316, 0.1700000017881,
          0.3799999952316, 0.5199999809265]
    sigdel=[0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
            0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
            0.1999999955297e-1, 0.1999999955297e-1, 0.1999999955297e-1,
            0.1999999955297e-1, 0.1999999955297e-1, 0.1999999994950e-5,
            0.1999999994950e-5, 0.3999999910593e-1, 0.3999999910593e-1,
            0.7999999821186e-1, 0.1599999964237]
    siga=[-1476.889556520, 764.5997024809, 20.39032204037,
          13.93251046364, -17.19068470999, -10.30051418087,
          -23.91052791023, -87.05159048677, 164.6286518027,
          -286.9730931994, 204.9446308145, 689.8503539772,
          3.411995820446, 2.781409973380, 11.14755985026,
          26.68922609713, -7.732407966270]
    photon=0.
    for j in range(npar):    
        hqdj=0.
        for i in range(ndim):
            vv = (v - sigvmi[i])/sigvt[i]
            hqdj = hqdj + (vv - sigv[j])*(vv - sigv[j]) 
        hqdj = hqdj + sigdel[j] * sigdel[j]
        hqdj = math.sqrt(hqdj)
        photon = photon + siga[j] * hqdj
    if imqfun==2:
        photon = vconst*math.exp(photon);
        photon = photon /365664.;
    return photon

def build_spectrum(theApp,run_number):
    from G4AtlasApps.atlas_utilhisto import HistoAtHistoSvc
    p_espectrum=HistoAtHistoSvc(repr(run_number))
    p_espectrum.book_histo(theApp,800,0.,80000.)
    p_espectrumaxis=p_espectrum.HistoSvcEntry.axis()
    for i in range(p_espectrumaxis.bins()):
        x=(p_espectrumaxis.binUpperEdge(i)+p_espectrumaxis.binLowerEdge(i))/2.
        y = photon_energy_spectrum(0.1*i) 
        print x,y 
        p_espectrum.HistoSvcEntry.fill(x,y)
    return p_espectrum
