#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# create all the histograms for each analysis - this will get called once for each 
# configured chain - we can set the HistPath either already here, or from the c++ 
# code 
        



# actuall create the monTool to go along with a specifi chain
# stores the histogram, binning, creates all the histograms etc

def createMonTool( slicetag, chain ) :

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

    monTool = GenericMonitoringTool( chain )

    mypath = "EXPERT"

    # monTool.HistPath would this be better set in the c++ code ?
 
    from TrigInDetAnalysisExample.chainString import chainString

    cs = chainString( chain )
        
    monTool.HistPath = slicetag + monGroup(chain)
    
#   print( "TIDAMonTool.py CreateMonTool ", chain, "  path:", monTool.HistPath )

    ptbins = [ 1, 1.20226, 1.44544, 1.7378, 2.0893, 2.51189, 3.01995, 3.63078, 4.36516, 5.24807, 6.30957, 
               7.58578, 9.12011, 10.9648, 13.1826, 15.8489, 19.0546, 22.9087, 27.5423, 33.1131, 39.8107, 
               47.863, 57.544, 69.1831, 83.1764, 100 ]

    vnbins = [
           -0.5, 
            0.5,      1.5,      2.5,      3.5,      4.5,      5.5,     6.5,     7.5,     8.5,     9.5,     10.5,     11.5,     12.5,    
           13.5,     14.5,     15.5,     17.5,     18.5,     19.5,    21.5,    23.5,    24.5,    26.5,     28.5,     30.5,     32.5,    
           35.5,     37.5,     40.5,     43.5,     46.5,     50.5,    53.5,    57.5,    61.5,    66.5,     71.5,     76.5,     81.5,   
           87.5,     93.5,    100.5,    107.5,    114.5,    123.5,    131.5,   141.5,   150.5,   161.5,   172.5,    185.5,    198.5,    
          211.5,    226.5,    242.5,    259.5,    277.5,    297.5, 
          317.5,    340.5,    363.5,    389.5,    416.5,    445.5,   476.5,   509.5,
          544.5,    582.5,    623.5,    666.5,    713.5,    762.5,   815.5,   872.5,   933.5,   998.5,   1067.5,
         1141.5,   1221.5,   1305.5,   1396.5,   1493.5,   1597.5,
         1708.5,   1827.5,   1953.5,   2089.5,
         2234.5,   2389.5,   2555.5,
         2733.5,   2923.5,   3125.5,
         3342.5,   3574.5,
         3823.5,   4088.5,
         4372.5,   4675.5,
         5000.5
         ]
    
    d0bins = [ -5.0,  -4.0,  -3.0,  -2.5,   
               -2.0,  -1.8,  -1.6,  -1.4,  -1.2,  
               -1.05, -0.95, -0.85, -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 
                0.05,  0.15,  0.25,  0.35,  0.45,  0.55,  0.65,  0.75,  0.85,  0.95,  1.05,  
                1.2,   1.4,   1.6,   1.8,   2.0,  
                2.5,   3.0,   4.0,   5.0 ]
    
    if  "LRT" in chain : 
            d0bins = [ -100.5, -97.5, -93.5, -90.5,                              
                    -86.5, -83.5, -80.5, 
                    -77.5, -74.5, -71.5,  
                    -69.5, -66.5, -63.5, -61.5, 
                    -59.5, -57.5, -54.5, -52.5, -50.5, 
                    -49.5, -47.5, -45.5, -43.5, -42.5, -40.5, 
                    -39.5, -37.5, -36.5, -34.5, -33.5, -32.5, -31.5, 
                    -29.5, -28.5, -27.5, -26.5, -25.5, -24.5, -23.5, -22.5, -21.5, -20.5, 
                    -19.5, -18.5, -17.5, -16.5, -15.5, -14.5, -13.5, -12.5, -11.5, -10.5, 
                     -9.5,  -8.5,  -7.5,  -6.5,  -5.5,  -4.5,  -3.5,  -2.5,  -1.5,  -0.5,
                      0.5,   1.5,   2.5,   3.5,   4.5,   5.5,   6.5,   7.5,   8.5,   9.5,  
                     10.5,  11.5,  12.5,  13.5,  14.5,  15.5,  16.5,  17.5,  18.5,  19.5, 
                     20.5,  21.5,  22.5,  23.5,  24.5,  25.5,  26.5,  27.5,  28.5,  29.5, 
                     31.5,  32.5,  33.5,  34.5,  36.5,  37.5,  39.5, 
                     40.5,  42.5,  43.5,  45.5,  47.5,  49.5, 
                     50.5,  52.5,  54.5,  57.5,  59.5, 
                     61.5,  63.5,  66.5,  69.5, 
                     71.5,  74.5,  77.5,    
                     80.5,  83.5,  86.5,   
                     90.5,  93.5,  97.5,    
                    100.5 ]
    
#   use a rounded up float(M_PI) values to overcome edge effects of using float 
#   values - will math.pi be appropriate ? double(float(M_PI)) > M_PI so it would
#   depend on whether floats or double are used for the histogram limits in the
#   histograms in defineHistogram  
#   pi = 3.14159265359 
    pi = 3.14159275 

    binlabels = [ "Nrois", "Nevents", "N ref tracks", "N matched tracks", "N total test tracks" ]

    
    defineHisto( monTool,  "Chain",    weight="Chain_weight",   path=mypath, type="TH1F", title=chain,  xlabels=binlabels,  xbins=5, xmin=0, xmax=5 )
    
    defineHisto( monTool,  "reftrk_N",     path=mypath, type="TH1F", title="Reference tracks", xbins=vnbins )
    
    defineHisto( monTool,  "reftrk_pT",    path=mypath, type="TH1F", title="Reference track pT",                xbins=ptbins   )
    defineHisto( monTool,  "reftrk_phi",   path=mypath, type="TH1F", title="Reference track Phi",               xbins=25,   xmin=-pi,    xmax=pi  )
    defineHisto( monTool,  "reftrk_eta",   path=mypath, type="TH1F", title="Reference track Eta",               xbins=25,   xmin=-2.5,     xmax=2.5   )
    
    if "LRT" in chain:
            defineHisto( monTool,  "reftrk_d0",    path=mypath, type="TH1F", title="Reference track d0",                xbins=201,  xmin=-100.0,   xmax=100.0 ) 
    else:
            defineHisto( monTool,  "reftrk_d0",    path=mypath, type="TH1F", title="Reference track d0",                xbins=101,  xmin=-5.0,     xmax=5.0   ) 

    defineHisto( monTool,  "reftrk_z0",    path=mypath, type="TH1F", title="Reference track z0",                xbins=50,   xmin=-225.,    xmax=225.  )
    defineHisto( monTool,  "reftrk_dd0",   path=mypath, type="TH1F", title="Reference track sigma(d0)",         xbins=50,   xmin=0,        xmax=0.5 )
    defineHisto( monTool,  "reftrk_dz0",   path=mypath, type="TH1F", title="Reference track sigma(z0)",         xbins=50,   xmin=0,        xmax=2.5 )
    defineHisto( monTool,  "reftrk_d0sig", path=mypath, type="TH1F", title="Reference track d0 significance",   xbins= 101, xmin=-5.,      xmax=5. )
    
    defineHisto( monTool,  "testtrk_N",     path=mypath, type="TH1F", title="Test tracks", xbins=vnbins )
    
    defineHisto( monTool,  "testtrk_pT",    path=mypath, type="TH1F", title="Test track pT",              xbins=ptbins   )
    defineHisto( monTool,  "testtrk_phi",   path=mypath, type="TH1F", title="Test track Phi",             xbins=25,   xmin=-pi,  xmax=pi  )
    defineHisto( monTool,  "testtrk_eta",   path=mypath, type="TH1F", title="Test track Eta",             xbins=25,   xmin=-2.5,   xmax=2.5   )

    if "LRT" in chain:
            defineHisto( monTool,  "testtrk_d0",    path=mypath, type="TH1F", title="Test track d0",              xbins=201,  xmin=-100.0, xmax=100.0 ) 
    else:
            defineHisto( monTool,  "testtrk_d0",    path=mypath, type="TH1F", title="Test track d0",              xbins=101,  xmin=-5.0,   xmax=5.0   )

    defineHisto( monTool,  "testtrk_z0",    path=mypath, type="TH1F", title="Test track z0",              xbins=50,   xmin=-225.,  xmax=225.  )
    defineHisto( monTool,  "testtrk_dd0",   path=mypath, type="TH1F", title="Test track sigma(d0)",       xbins=50,   xmin=0,      xmax=0.5 )
    defineHisto( monTool,  "testtrk_dz0",   path=mypath, type="TH1F", title="Test track sigma(z0)",       xbins=50,   xmin=0,      xmax=2.5 )
    defineHisto( monTool,  "testtrk_d0sig", path=mypath, type="TH1F", title="Test track d0 significance", xbins=101,  xmin=-5.0,   xmax=5.0  )
    
    defineHisto( monTool,  "layer_rec",     path=mypath, type="TH1F", title="hit layers",                 xbins=32,   xmin=-0.5,   xmax=31.5 )
    defineHisto( monTool,  "layer",         path=mypath, type="TH1F", title="hit layers",                 xbins=32,   xmin=-0.5,   xmax=31.5  )
    
    defineHisto( monTool,  "residual_pT",   path=mypath, type="TH1F", title="track pT residual",         xbins=201,   xmin=-100.0, xmax=100.0 )
    defineHisto( monTool,  "residual_ipT",  path=mypath, type="TH1F", title="track ipT residual",        xbins=55,    xmin=-5.5,   xmax=5.5   )
    defineHisto( monTool,  "residual_phi",  path=mypath, type="TH1F", title="track Phi residual",        xbins=50,    xmin=-0.02,  xmax=0.02  )
    defineHisto( monTool,  "residual_eta",  path=mypath, type="TH1F", title="track Eta residual",        xbins=50,    xmin=-0.02,  xmax=0.02  )
    defineHisto( monTool,  "residual_d0",   path=mypath, type="TH1F", title="track d0 residual ",        xbins=251,   xmin=-2.5,   xmax=2.5   )
    defineHisto( monTool,  "residual_z0",   path=mypath, type="TH1F", title="track z0 residual",         xbins=401,   xmin=-20.0,  xmax=20.0  )
    defineHisto( monTool,  "residual_dd0",  path=mypath, type="TH1F", title="track sigma d0 residual ",  xbins=251,   xmin=-0.5,   xmax=0.5  )
    defineHisto( monTool,  "residual_dz0",  path=mypath, type="TH1F", title="track sigma z0 residual",   xbins= 401,  xmin=-1.0,   xmax=1.0  )
    
    defineHisto( monTool,  "npix",          path=mypath, type="TH1F", title="npix",                 xbins=26,    xmin=-0.5,   xmax=25.5  )
    defineHisto( monTool,  "npix_rec",      path=mypath, type="TH1F", title="npix_rec",             xbins=26,    xmin=-0.5,   xmax=25.5  )
    
    defineHisto( monTool,  "nsct",          path=mypath, type="TH1F", title="nsct",                 xbins=31,    xmin=-0.5,   xmax=30.5  )
    defineHisto( monTool,  "nsct_rec",      path=mypath, type="TH1F", title="nsct_rec",             xbins=31,    xmin=-0.5,   xmax=30.5  )
    
    defineHisto( monTool,  "nsiHits",       path=mypath, type="TH1F", title="nsiHits",              xbins=41,    xmin=-0.5,   xmax=40.5  )
    defineHisto( monTool,  "nsiHits_rec",   path=mypath, type="TH1F", title="nsiHits_rec",          xbins=41,    xmin=-0.5,   xmax=40.5  )
    
    defineHisto( monTool,  "ntrt",          path=mypath, type="TH1F", title="ntrt",                 xbins=91,     xmin=-0.5,   xmax=91.5  )
    defineHisto( monTool,  "ntrt_rec",      path=mypath, type="TH1F", title="ntrt_rec",             xbins=91,    xmin=-0.5,   xmax=91.5  )
    
    
    defineHisto( monTool,  "Eff_overall",       path=mypath, type="TProfile", title="total efficiency",    xbins=1,    xmin=0.,   xmax=1.    )
    
    defineHisto( monTool,  "Eff_pT",     path=mypath, type="TProfile", title="pT efficiency",       xbins=ptbins   )
    defineHisto( monTool,  "Eff_Eta",    path=mypath, type="TProfile", title="eta efficiency",      xbins=25,   xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Eff_Phi",    path=mypath, type="TProfile", title="phi  efficiency",     xbins=25,   xmin=-pi, xmax=pi )
    defineHisto( monTool,  "Eff_d0",     path=mypath, type="TProfile", title="d0 efficiency",       xbins=d0bins     ) 
    defineHisto( monTool,  "Eff_z0",     path=mypath, type="TProfile", title="z0 efficiency",       xbins=50,   xmin=-225., xmax=225. )
    defineHisto( monTool,  "Eff_nVtx",   path=mypath, type="TProfile", title="nVtx efficiency",     xbins=71,   xmin=-0.5,  xmax=70.5 )
    defineHisto( monTool,  "Eff_lb",     path=mypath, type="TProfile", title="efficinecy vs lumiblock",      xbins=301, xmin=-0.5, xmax=3009.5 )
    
    defineHisto( monTool,  "trkvtx_x_vs_lb", path=mypath, type="TProfile", title="track vertex x vs lumiblock", xbins=301, xmin=-0.5, xmax=3009.5 )
    defineHisto( monTool,  "trkvtx_y_vs_lb", path=mypath, type="TProfile", title="track vertex y vs lumiblock", xbins=301, xmin=-0.5, xmax=3009.5 )
    defineHisto( monTool,  "trkvtx_z_vs_lb", path=mypath, type="TProfile", title="track vertex z vs lumiblock", xbins=301, xmin=-0.5, xmax=3009.5 )

    defineHisto( monTool,  "npix_vs_eta",     path=mypath, type="TProfile", title="offline npix vs eta;offline #eta; mean number of offline pixel hits", xbins=30,   xmin=-2.5,  xmax=2.5 )
    defineHisto( monTool,  "npix_vs_eta_rec", path=mypath, type="TProfile", title="trigger npix vs eta;offline #eta; mean number of trigger pixel hits", xbins=30,   xmin=-2.5,  xmax=2.5 )
    defineHisto( monTool,  "nsct_vs_eta",     path=mypath, type="TProfile", title="offline nsct vs eta;offline #eta; mean number of offline sct hits",   xbins=30,   xmin=-2.5,  xmax=2.5 )
    defineHisto( monTool,  "nsct_vs_eta_rec", path=mypath, type="TProfile", title="trigger nsct vs eta;offline #eta; mean number of trigger sct hits",   xbins=30,   xmin=-2.5,  xmax=2.5 )
    defineHisto( monTool,  "ntrt_vs_eta",     path=mypath, type="TProfile", title="offline ntrt vs eta;offline #eta; mean number of offline trt hits",   xbins=30,   xmin=-2.5,  xmax=2.5 )
    defineHisto( monTool,  "ntrt_vs_eta_rec", path=mypath, type="TProfile", title="trigger ntrt vs eta;offline #eta; mean number of trigger trt hits",   xbins=30,   xmin=-2.5,  xmax=2.5 )
    
    defineHisto( monTool,  "npix_vs_phi",     path=mypath, type="TProfile", title="offline npix vs phi;offline #phi; mean number of offline pixel hits", xbins=30,   xmin=-pi, xmax=pi ) 
    defineHisto( monTool,  "npix_vs_phi_rec", path=mypath, type="TProfile", title="trigger npix vs phi;offline #phi; mean number of trigger pixel hits", xbins=30,   xmin=-pi, xmax=pi )
    defineHisto( monTool,  "nsct_vs_phi",     path=mypath, type="TProfile", title="offline nsct vs phi;offline #phi; mean number of offline sct hits",   xbins=30,   xmin=-pi, xmax=pi )
    defineHisto( monTool,  "nsct_vs_phi_rec", path=mypath, type="TProfile", title="trigger nsct vs phi;offline #phi; mean number of trigger sct hits",   xbins=30,   xmin=-pi, xmax=pi )
    defineHisto( monTool,  "ntrt_vs_phi",     path=mypath, type="TProfile", title="offline ntrt vs phi;offline #phi; mean number of offline trt hits",   xbins=30,   xmin=-pi, xmax=pi )
    defineHisto( monTool,  "ntrt_vs_phi_rec", path=mypath, type="TProfile", title="trigger ntrt vs phi;offline #phi; mean number of trigger trt hits",   xbins=30,   xmin=-pi, xmax=pi )
    
    defineHisto( monTool,  "npix_vs_d0",        path=mypath, type="TProfile", title="offline npix vs d0;offline #d0; mean number of offline pixel hits",   xbins=d0bins )
    defineHisto( monTool,  "npix_vs_d0_rec",    path=mypath, type="TProfile", title="trigger npix vs d0;offline #d0; mean number of trigger pixel hits",   xbins=d0bins )
    defineHisto( monTool,  "nsct_vs_d0",        path=mypath, type="TProfile", title="offline nsct vs d0;offline #d0; mean number of offline sct hits",     xbins=d0bins ) 
    defineHisto( monTool,  "nsct_vs_d0_rec",    path=mypath, type="TProfile", title="trigger nsct vs d0;offline #d0; mean number of trigger sct hits",     xbins=d0bins )
    defineHisto( monTool,  "npix_vs_pT",        path=mypath, type="TProfile", title="offline npix vs pT;offline #p_{T}; mean number of offline pixel hits",   xbins=ptbins )
    defineHisto( monTool,  "npix_vs_pT_rec",    path=mypath, type="TProfile", title="trigger npix vs pT;offline #p_{T}; mean number of trigger pixel hits",   xbins=ptbins )
    defineHisto( monTool,  "nsct_vs_pT",        path=mypath, type="TProfile", title="offline nsct vs pT;offline #p_{T} [GeV]; mean number of offline pixel hits", xbins=ptbins )
    defineHisto( monTool,  "nsct_vs_pT_rec",    path=mypath, type="TProfile", title="trigger nsct vs pT;offline #p_{T} [GeV]; mean number of trigger pixel hits", xbins=ptbins )
    defineHisto( monTool,  "nsihits_lb",        path=mypath, type="TProfile", title="offline n sihits vs lumiblock",   xbins=301, xmin=-0.5, xmax=3009.5 )
    defineHisto( monTool,  "nsihits_lb_rec",    path=mypath, type="TProfile", title="trigger n sihits vs lumiblock",   xbins=301, xmin=-0.5, xmax=3009.5 )
    
    defineHisto( monTool,  "Res_pT",    path=mypath, type="TProfile", title="pT residual;#eta_{ref};pT_{trig}-pT_{ref} [GeV]",           xbins=25,  xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Res_ipT",   path=mypath, type="TProfile", title="ipT residual;#eta_{ref};1/pT_{trig}-1/pT_{ref} [GeV^{-1}]", xbins=25,  xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Res_eta",   path=mypath, type="TProfile", title="Eta residual;#eta_{ref};eta_{trig}-#eta_{ref}",             xbins=25,  xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Res_phi",   path=mypath, type="TProfile", title="Phi residual;#eta_{ref};phi_{trig}-#phi_{ref}",             xbins=25,  xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Res_d0",    path=mypath, type="TProfile", title="d0 residual;#eta_{ref};Delta d0 [mm]",                      xbins=25,  xmin=-2.5,  xmax=2.5  )
    defineHisto( monTool,  "Res_z0",    path=mypath, type="TProfile", title="z0 residual;#eta_{ref};Delta z0 [mm]",                       xbins=25,  xmin=-2.5,  xmax=2.5  )

    defineHisto( monTool,  "d0_vs_phi_prof",     path=mypath, type="TProfile", title="d0 vs phi_prof",      xbins=25,     xmin=-pi,  xmax=pi )
    defineHisto( monTool,  "d0_vs_phi_rec_prof", path=mypath, type="TProfile", title="d0 vs phi_rec_prof",  xbins=25,     xmin=-pi,  xmax=pi )

    if "probe" in cs.extra: 
            defineHisto( monTool,  "invmass",      path=mypath, type="TH1F", title="invariant mass;mass [GeV]", xbins=320, xmin=0, xmax=200 )
            defineHisto( monTool,  "invmass_obj",  path=mypath, type="TH1F", title="invariant mass;mass [GeV]", xbins=320, xmin=0, xmax=200 )
            

    if cs.vtx != "" :

            vnbins_short = [ 
                    -0.5,
                     0.5,     1.5,    2.5,    3.5,    4.5,    5.5,    6.5,    7.5,    8.5,   9.5,   
                    10.5,    11.5,   12.5,   13.5,   14.5,   15.5,   17.5,   18.5,   19.5,   
                    21.5,    23.5,   24.5,   26.5,   28.5,   
                    30.5,    32.5,   35.5,   37.5,   
                    40.5,    43.5,   46.5,   
                    50.5,    53.5,   57.5,   
                    61.5,    66.5,   
                    71.5,    76.5,   
                    81.5,    87.5,
                    93.5,   
                    100.5,  107.5,  114.5,  123.5,  131.5,  141.5,  150.5,   161.5,   172.5,   185.5,   198.5,   
                    211.5,  226.5,  242.5,  259.5,  277.5,  297.5,   
                    317.5,  340.5,  363.5,  389.5,   
                    416.5,  445.5,  476.5,   
                    509.5,  544.5,  582.5,   
                    623.5,  666.5,   
                    713.5,  762.5,   
                    815.5,  872.5,   
                    933.5,  998.5,   
                    1067.5
            ]

            defineHisto( monTool, "vx_nvtx",  path=mypath, type="TH1F", title=";number of vertices",  xbins=101, xmin=-0.5,  xmax=100.5 )
            defineHisto( monTool, "vx_zed",   path=mypath, type="TH1F", title=";vtx z [mm]",          xbins=100, xmin=-250,  xmax=250   )
            defineHisto( monTool, "vx_ntrax", path=mypath, type="TH1F", title=";number of tracks",    xbins=vnbins )
            
            defineHisto( monTool, "vx_nvtx_rec",  path=mypath, type="TH1F", title=";number of vertices",   xbins=101, xmin=-0.5,  xmax=100.5 )
            defineHisto( monTool, "vx_zed_rec",   path=mypath, type="TH1F", title=";vtx z [mm]",           xbins=100, xmin=-250,  xmax=250   )
            defineHisto( monTool, "vx_ntrax_rec", path=mypath, type="TH1F", title=";number of tracks",     xbins=vnbins )
            
            defineHisto( monTool, "vx_zed_res",   path=mypath, type="TH1F", title="Delta z [mm]", xbins=400, xmin=-10, xmax=10 )
            
            
            defineHisto( monTool, "vx_rdz_vs_zed",   path=mypath, type="TProfile", title="rdz_vs_zed; vtx z [mm];z residual [mm]",         xbins=100, xmin=-250,  xmax=250 ) 
            defineHisto( monTool, "vx_rdz_vs_ntrax", path=mypath, type="TProfile", title="rdz_vs_ntrax;number of tracks;z residual [mm]",  xbins=vnbins_short )
            defineHisto( monTool, "vx_rdz_vs_nvtx",  path=mypath, type="TProfile", title="rdz_vs_nvtx;number of vertices;z residual [mm]", xbins=51,  xmin=-0.125, xmax=50.125 )
            
            # defineHistogram( monTool, "rdz_vs_mu",    30,     0,    30,    400, -20, 20 ); 
            
            defineHisto( monTool, "vx_zed_eff",    path=mypath, type="TProfile", title="zed_eff;efficiency;offline vtx z [mm]",   xbins=50,  xmin=-250,  xmax=250    )
            defineHisto( monTool, "vx_ntrax_eff",  path=mypath, type="TProfile", title="ntrax_eff;number of tracks;efficiency",   xbins=vnbins )
            defineHisto( monTool, "vx_nvtx_eff",   path=mypath, type="TProfile", title="nvtx_eff;number of vertices;efficiency",  xbins=101, xmin=-0.5,  xmax=100.5  )
            defineHisto( monTool, "vx_mu_eff",     path=mypath, type="TProfile", title="mu_eff;<#mu>;efficiency",                 xbins=61,  xmin=-0.5,  xmax=60.5   )
            defineHisto( monTool, "vx_lb_eff",     path=mypath, type="TProfile", title="lb_eff;lumi block;efficiency",            xbins=151, xmin=-0.5,  xmax=3019.5 )
 

    
    return monTool



# generate the mongroup hist path from the analysis string 

def monGroup( analysis_chain ) :

        from TrigInDetAnalysisExample.chainString import chainString

        chain = chainString( analysis_chain )

        mg = ""

        if chain.head == "" :
            mg = "/Fullscan"
        else :
            mg = "/"+chain.head

        mg += "/"+chain.tail

        if chain.extra != "" :
            mg += "_" + chain.extra

        if chain.roi != "" :
            mg += "_"+chain.roi
            
        if chain.vtx != "" :
            mg += "_"+chain.vtx
            
        if chain.element != "" :
            mg += "_" + chain.element
        
        if chain.passed :
            mg += "/DTE"

        return mg


# wrapper around montool.defineHistogram to simplify the required histogram names
# eg for a TProfile, automatically add the second variable, and create the histogram
# alias to avoid having to write pages of tedious boier plate functions

def defineHisto( montool, name, **args ) : 

        if "type" in args and args["type"] == "TProfile" : 
                name = name + "," + name + "_weight;" + name

        montool.defineHistogram( name, **args )


