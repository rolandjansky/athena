# Code that generates the ID trans can from CompareGeometries
# Author John Alison <johnda@hep.upenn.edu>IDTransCan = TCanvas("ID Translation Visualization ","ID differences ",800,800) 
IDTransCan = TCanvas("ID Translation Visualization ","ID differences ",800,800) 
IDDim = 1300
h1 = gPad.DrawFrame(-1*IDDim,-1*IDDim,IDDim,IDDim)
h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
h1.GetXaxis().SetTitle("x [mm]") 
h1.GetYaxis().SetTitle("y [mm]") 
line = TArrow()
line.DrawArrow(500-TRASL_FACTOR/2./10,-1000,500+TRASL_FACTOR/2/10.,-1000,0.01,"|-|") 
text = TText() 
text.DrawText(500,-1100,"0.1 mm") 

# IBL Trans Cans
iblTransCan = []
for i in range(20):
    iblTransCan.append(TCanvas("IBL Translation Visualization "+str(i-10),"IBL Eta module "+str(i-10),800,800) )
    iblDim = 100
    h1 = gPad.DrawFrame(-1*iblDim,-1*iblDim,iblDim,iblDim)
    h1.SetTitle("Translations x"+str(TRASL_FACTOR)+" Eta Module "+str(i-10))
    h1.GetXaxis().SetTitle("x [mm]")
    h1.GetYaxis().SetTitle("y [mm]")
    # gPad.SetGridx()
    # gPad.SetGridy()
    line = TArrow()
    line.DrawArrow(150-TRASL_FACTOR/2./10,-200,150+TRASL_FACTOR/2/10.,-200,0.01,"|-|")
    text = TText()
    text.DrawText(150,-180,"0.1 mm")

# Pixel Trans Cans
pixelTransCan = []
for i in range(13):
    pixelTransCan.append(TCanvas("Pixel Translation Visualization "+str(i-6),"Pixel Eta module "+str(i-6),800,800) )
    pixDim = 250
    h1 = gPad.DrawFrame(-1*pixDim,-1*pixDim,pixDim,pixDim)
    h1.SetTitle("Translations x"+str(TRASL_FACTOR)+" Eta Module "+str(i-6)) 
    h1.GetXaxis().SetTitle("x [mm]") 
    h1.GetYaxis().SetTitle("y [mm]") 
    # gPad.SetGridx() 
    # gPad.SetGridy() 
    line = TArrow()
    line.DrawArrow(150-TRASL_FACTOR/2./10,-200,150+TRASL_FACTOR/2/10.,-200,0.01,"|-|") 
    text = TText() 
    text.DrawText(150,-180,"0.1 mm") 

# SCT Trans Cans
sctTransCan = []
for i in range(12):
    sctTransCan.append(TCanvas("SCT Translation Visualization "+str(i-6),"Sct Eta module "+str(i-6),800,800) )
    sctDim = 650
    h1 = gPad.DrawFrame(-1*sctDim,-1*sctDim,sctDim,sctDim)
    h1.SetTitle("translation x"+str(TRASL_FACTOR)+" Eta Module "+str(i-6)) 
    h1.GetXaxis().SetTitle("x [mm]") 
    h1.GetYaxis().SetTitle("y [mm]") 
    # gPad.SetGridx() 
    # gPad.SetGridy() 
    line = TArrow()
    line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
    text = TText() 
    text.DrawText(0,-100,"0.1 mm") 


# TRT Trans Cans
trtTransCan = []
for i in range(2):
    if i:
        i = 1
    else:
        i = -1
        
    trtTransCan.append(TCanvas("TRT Translation Visualization Moludes "+str(i),"TRT differences Modules "+str(i),800,800) )
    trtDim = 1300
    h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
    h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
    h1.GetXaxis().SetTitle("x [mm]") 
    h1.GetYaxis().SetTitle("y [mm]") 
    line = TArrow()
    line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
    text = TText() 
    text.DrawText(0,-100,"0.1 mm")

# TRT Module Trans Cans

# A list of pairs of transCans (one for side A and one for side C)
trtModTransCan = {}

# loop over the phi sectors to draw
for thisMod in range(len(drawTRTModules)):
    thisLay = drawTRTModules[thisMod][0]
    thisPhi = drawTRTModules[thisMod][1]

    # A temp. holder for the pair of transCans for this phi sector.
    tempTransCanList = []
    for side in range(2):
        if side:
            side = 1
        else:
            side = -1
        
        tempTransCanList.append(TCanvas("TRT Translation Visualization Lay: "+str(thisLay)+" Phi: "+str(thisPhi)+" Side "+str(side)
                                        ,"TRT differences Straws in Lay: "+str(thisLay)+" Phi: "+str(thisPhi)+" Side "+str(side)
                                        ,800,800) )
        trtDim = 1300
        h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
        h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
        h1.GetXaxis().SetTitle("x [mm]") 
        h1.GetYaxis().SetTitle("y [mm]") 
        line = TArrow()
        line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
        text = TText() 
        text.DrawText(0,-100,"0.1 mm") 

    # Add the pair to our total list
    trtModTransCan[(thisLay,thisPhi)] = tempTransCanList

# Barrel Elements

# x1 position
x1_Aside = {}
x1_Cside = {}

# y1 position
y1_Aside = {}
y1_Cside = {}

# z1 position
z1_Aside = {}

# x2 position
x2_Aside = {}
x2_Cside = {}

# y2 position
y2_Aside = {}
y2_Cside = {}



# TRT Straw plane translations
# ================================
trtEndcapAStrawPlanes = []
for i in range(17):
        
    trtEndcapAStrawPlanes.append(TCanvas("TRT Visualization Straw Plane "+str(i)+" Endcap A","TRT Visualization Straw Plane "+str(i)+" Endcap A",800,800) )
    trtDim = 1300
    h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
    h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
    h1.GetXaxis().SetTitle("x [mm]") 
    h1.GetYaxis().SetTitle("y [mm]") 
    line = TArrow()
    line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
    text = TText() 
    text.DrawText(0,-100,"0.1 mm")


trtEndcapCStrawPlanes = []
for i in range(17):
    trtEndcapCStrawPlanes.append(TCanvas("TRT Visualization Straw Plane "+str(i)+" Endcap C","TRT Visualization Straw Plane "+str(i)+" Endcap C",800,800) )
    trtDim = 1300
    h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
    h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
    h1.GetXaxis().SetTitle("x [mm]") 
    h1.GetYaxis().SetTitle("y [mm]") 
    line = TArrow()
    line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
    text = TText() 
    text.DrawText(0,-100,"0.1 mm")

