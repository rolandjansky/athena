#===========================================================================
#===========================================================================
# Description:
#  This file compares the Track Segments distributions, is called from makeComparision
#
# Author:
#  John Alison johnda@hep.upenn.edu
#===========================================================================

# Charge Mis-Id rates
cChargeID = []
chargeCanvasName = ["Charge mis-ID Vs D0","Charge mis-ID Vs Z0","Charge mis-ID Vs Pt Low","Charge mis-ID Vs Pt Up"]
for i in range(len(chargeCanvasName)):
    cChargeID.append(TCanvas(chargeCanvasName[i],"Charge Mis ID",1100, 800))
    
chargeHistName = ["ratio_delta_chargeVsD0",
                  "ratio_delta_chargeVsZ0",
                  "ratio_delta_chargeVsPtLow",
                  "ratio_delta_chargeVsPtUp"]


# Delta Track Parameters
cDeltaTrackParam = []
deltaCanvasName = ["Delta D0","Delta Phi0","Delta z0","Delta qOverpT","Delta Eta","Delta nHits"]
m_units = ["mm","radians","mm","GeV^{-1}","",""]
m_canvasNameHelper = [""," (Positive Tracks)"," (Negative Tracks)"]
for j in range(3):
    for i in range(6):
        cDeltaTrackParam.append(TCanvas(deltaCanvasName[i]+m_canvasNameHelper[j],"Track Segment Comparison", 1100,800))
      

trackSegmentsName = ["delta_d0","delta_phi0","delta_z0","delta_qOverPt","delta_eta0","delta_nHits"]

cAveVsTrackParam = []
cSigVsTrackParam = []
vsCanvasName = [" D0 vs D0", " D0 vs Phi", " D0 vs Pt"," D0 vs Z0"," D0 vs Eta",
                " Eta0 vs D0", " Eta0 vs Phi", " Eta0 vs Pt"," Eta0 vs Z0"," Eta0 vs Eta0",
                " Phi0 vs D0", " Phi0 vs Phi", " Phi0 vs Pt"," Phi0 vs Z0"," Phi0 vs Eta0",
                " qOverPt vs D0", " qOverPt vs Phi", " qOverPt vs Pt"," qOverPt vs Z0"," qOverPt vs Eta0",
                " Z0 vs D0", " Z0 vs Phi", " Z0 vs Pt"," Z0 vs Z0"," Z0 vs Eta0",
                " nHits vs D0", " nHits vs Phi", " nHits vs Pt"," nHits vs Z0"," nHits vs Eta0"]

for j in range(3):
    for i in range(30):
        cAveVsTrackParam.append(TCanvas("Delta "+vsCanvasName[i]+m_canvasNameHelper[j],"Track Segment Comparison Vs Track Parameter",1100,800))
        cSigVsTrackParam.append(TCanvas("Sigma "+vsCanvasName[i]+m_canvasNameHelper[j],"Track Segment Comparison Vs Track Parameter",1100,800))
        
vsTrackParamName = ["_delta_d0VsD0","_delta_d0VsPhi0","_delta_d0VsPt","_delta_d0VsZ0","_delta_d0VsEta",
                    "_delta_eta0VsD0","_delta_eta0VsPhi0","_delta_eta0VsPt","_delta_eta0VsZ0","_delta_eta0VsEta",
                    "_delta_phi0VsD0","_delta_phi0VsPhi0","_delta_phi0VsPt","_delta_phi0VsZ0","_delta_phi0VsEta",
                    "_delta_qOverPtVsD0","_delta_qOverPtVsPhi0","_delta_qOverPtVsPt","_delta_qOverPtVsZ0","_delta_qOverPtVsEta",
                    "_delta_z0VsD0","_delta_z0VsPhi0","_delta_z0VsPt","_delta_z0VsZ0","_delta_z0VsEta",
                    "_delta_nHitsVsD0","_delta_nHitsVsPhi0","_delta_nHitsVsPt","_delta_nHitsVsZ0","_delta_nHitsVsEta"]

m_histNameHelper = ["","_p","_n"]

cAveVsTrackParamAll_mon = []
cAveVsTrackParamAll_ref = []
for i in range(30):
    cAveVsTrackParamAll_ref.append(TCanvas("Delta "+vsCanvasName[i]+" All (Ref)", "Track Segment Comparison Vs Track Parameter", 550,400))
    cAveVsTrackParamAll_mon.append(TCanvas("Delta "+vsCanvasName[i]+" All (Mon)", "Track Segment Comparison Vs Track Parameter", 550,400))

tpMins = [-0.4,-0.4,-0.4,-0.4,-0.4,
          -1,-1,-1,-1,-1,
          -0.003,-0.003,-0.003,-0.003,-0.003,
          -0.005,-0.005,-0.005,-0.005,-0.005,
          -1,-1,-1,-1,-1,
          -5,-5,-5,-5,-5]

tpMaxes = [0.4,0.4,0.4,0.4,0.4,
           1,1,1,1,1,
           0.003,0.003,0.003,0.003,0.003,
           0.005,0.005,0.005,0.005,0.005,
           1,1,1,1,1,
           5,5,5,5,5]

for thisTrackSelection in trackSegmentsTrackSelection:
    for i in range(3):
        for j in range(6):
        
            cDeltaTrackParam[6*i+j].cd()
            #processAndQuoteHistogram(trackSegmentsName[j]+m_histNameHelper[i],thisTrackSelection+"TrackSegments/",m_units[j])
            processAndFitHistogram(trackSegmentsName[j]+m_histNameHelper[i],thisTrackSelection+"TrackSegments/",m_units[j])

    if PostProcessing:
        for i in range(len(chargeCanvasName)):
            cChargeID[i].cd()
            procProfile(chargeHistName[i],thisTrackSelection+"TrackSegments/")
            
        for i in range(3):
            for j in range(30):
                cAveVsTrackParam[(30*i+j)].cd()
                procProfile("average"+vsTrackParamName[j]+m_histNameHelper[i],thisTrackSelection+"TrackSegments/")
                if i ==2:
                    cAveVsTrackParamAll_ref[j].cd()
                    procAllProfiles(["average"+vsTrackParamName[j]+m_histNameHelper[0],
                                    "average"+vsTrackParamName[j]+m_histNameHelper[1],
                                    "average"+vsTrackParamName[j]+m_histNameHelper[2]],
                                    thisTrackSelection+"TrackSegments/",
                                    a_file,
                                    [kBlack, kRed+1,kBlue+1],
                                    "All (Black), Positive(Red), Negative(Blue)",
                                    tpMins[i],
                                    tpMaxes[i])

                    cAveVsTrackParamAll_mon[j].cd()
                    procAllProfiles(["average"+vsTrackParamName[j]+m_histNameHelper[0],
                                    "average"+vsTrackParamName[j]+m_histNameHelper[1],
                                    "average"+vsTrackParamName[j]+m_histNameHelper[2]],
                                    thisTrackSelection+"TrackSegments/",
                                    b_file,
                                    [kBlack, kRed+1,kBlue+1],
                                    "All (Black), Positive(Red), Negative(Blue)",
                                    tpMins[i],
                                    tpMaxes[i])

                                        
                cSigVsTrackParam[(30*i+j)].cd()
                procProfile("sigma"+vsTrackParamName[j]+m_histNameHelper[i],thisTrackSelection+"TrackSegments/")
                                
    dirName = thisTrackSelection.split('/')[1]+"TrackSegments"
    rootfile.cd(thisTrackSelection.split('/')[1])
    gDirectory.cd("Barrel")
    gDirectory.mkdir("TrackSegments",dirName).cd()
    for i in range(len(chargeCanvasName)):
        cChargeID[i].Write()

    for i in range(3*6):
        cDeltaTrackParam[i].Write()

    if PostProcessing:
        for i in range(30*3):
            cAveVsTrackParam[i].Write()
            cSigVsTrackParam[i].Write()

        for i in range(30):
            cAveVsTrackParamAll_mon[i].Write()
            cAveVsTrackParamAll_ref[i].Write()
        
