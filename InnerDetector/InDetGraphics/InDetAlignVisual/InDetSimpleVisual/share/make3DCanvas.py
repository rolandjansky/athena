# Code that generates the 3D canvases for the ID layers
# Everything is stored in lists because pyROOT doesn't play well
#   with the 3D objects...
# @author: Will DiClemente <wdic@sas.upenn.edu>

def make_axes():
    rules = TAxis3D()
    rules.SetAxisColor(1)
    rules.SetLabelColor(1)
    rules.SetTitleOffset(1.5)
    rules.SetTitleOffset(1.8,"Z")
    rules.SetXTitle("x [mm]")
    rules.SetYTitle("y [mm]")
    rules.SetZTitle("z [mm]")
    return rules

def make_title(text):
    title = TText(0,.9,text+" x"+str(TRASL_FACTOR))
    title.SetTextAlign(22)
    title.SetTextFont(42)
    return title
    

ibl3DCan = TCanvas("IBL 3D","IBL differences 3D",1000,1000)
iblLines = [[],[],[],[],[],[],[],[],[],[],[],[],[],[]] # 14 staves
iblPoints = [[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
ibl3DStaves = []
iblAxes = []
iblTitles = []
for i in range(0,14):
    ibl3DStaves.append(TCanvas("IBL 3D Stave "+str(i+1),"IBL differences 3D Stave "+str(i+1),1000,1000))
    iblAxes.append(make_axes())
    iblTitles.append(make_title("IBL Visualization Stave "+str(i+1)))
    
pix3DCan = TCanvas("PIX 3D","PIX differences",1000,1000)
pixLines = [[],[],[]] # 3 layers
pixPoints = [[],[],[]]
pix3DLayers = []
pixAxes = []
pixTitles = []
for i in range(0,3):
    pix3DLayers.append(TCanvas("PIX 3D Layer "+str(i+1),"PIX differences 3D L"+str(i+1),1000,1000))
    pixAxes.append(make_axes())
    pixTitles.append(make_title("PIX Visualization Layer "+str(i+1)))

sct3DCan = TCanvas("SCT 3D","SCT differences 3D",1000,1000)
sctLines = [[],[],[],[]] # 4 layers
sctPoints = [[],[],[],[]]
sct3DLayers = []
sctAxes = []
sctTitles = []
for i in range(0,4):
    sct3DLayers.append(TCanvas("SCT 3D Layer "+str(i+1),"SCT differences 3D L"+str(i+1),1000,1000))
    sctAxes.append(make_axes())
    sctTitles.append(make_title("SCT Visualization Layer "+str(i+1)))
