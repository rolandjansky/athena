##########################################################################
#
# Retrieve trigger EDM objects
#
##########################################################################

# init application mgr
theApp.initialize()

# run 1 event
theApp.nextEvent()

# CTP
ctp = PyTriggerTools.getCTP_Decision ("CTP_Decision")
print ("CTP")
print (ctp.getTriggerTypeWord())
print ("-------------------------")

# L1 ROI
L1rois = PyTriggerTools.getLVL1_ROI ("LVL1_ROI")

print ("Muon")
rois = L1rois.getMuonROIs()
for roi in rois:
    print ("Eta:%s" % roi.getEta())
    print ("Phi:%s" % roi.getPhi())
print ("-------------------------")

print ("EmTau")
rois = L1rois.getEmTauROIs()
for roi in rois:    
    print ("Eta:%s" % roi.getEta())
    print ("Phi:%s" % roi.getPhi())
print ("-------------------------")

print ("Jet")
rois = L1rois.getJetROIs()
for roi in rois:
    print ("Eta:%s" % roi.getEta())
    print ("Phi:%s" % roi.getPhi())
print ("-------------------------")

print ("JetET")
rois = L1rois.getJetEtROIs()
print ("-------------------------")

print ("EnergySum")
rois = L1rois.getEnergySumROIs()
for i in range(len(rois)):
    print ("EX:%s" % rois[i].getEnergyX())
    print ("EY:%s" % rois[i].getEnergyY())
    print ("ET:%s" % rois[i].getEnergyT())
print ("-------------------------")
