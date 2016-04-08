BarrelTrans = TCanvas("TRT Barrel Translations","TRT Barrel Translations ",800,800) 
trtDim = 1300
h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
h1.SetTitle("translation x"+str(TRASL_FACTOR)) 
h1.GetXaxis().SetTitle("x [mm]") 
h1.GetYaxis().SetTitle("y [mm]") 
line = TArrow()
line.DrawArrow(-TRASL_FACTOR/2./10,0,TRASL_FACTOR/2/10.,0,0.01,"|-|") 
text = TText() 
text.DrawText(0,-100,"0.1 mm")

for mod in x1_Aside:

    x1 = (x1_Aside[mod] + x1_Cside[mod]) / 2
    y1 = (y1_Aside[mod] + y1_Cside[mod]) / 2

    x2 = (x2_Aside[mod] + x2_Cside[mod]) / 2 
    y2 = (y2_Aside[mod] + y2_Cside[mod]) / 2 

    dx = x2 - x1
    dy = y2 - y1
    
    line.SetLineColor(kGreen+1)
    line.DrawArrow(x1,y1,x1+TRASL_FACTOR*(dx),y1+TRASL_FACTOR*(dy),0.01,"") 


trtDir.cd()
BarrelTrans.Write()
rootFile.cd()


BarrelRotxy = TCanvas("TRT Barrel Ratations r-phi","TRT Barrel Rotations r-phi ",800,800) 
trtDim = 1300
ROTXY_FACTOR = 100000
h1 = gPad.DrawFrame(-1*trtDim,-1*trtDim,trtDim,trtDim)
h1.SetTitle("rotation x"+str(ROTXY_FACTOR)) 
h1.GetXaxis().SetTitle("x [mm]") 
h1.GetYaxis().SetTitle("y [mm]") 
line = TArrow()
line.DrawArrow(-ROTXY_FACTOR/2./1000,0,ROTXY_FACTOR/2/1000.,0,0.01,"|-|") 
text = TText() 
text.DrawText(0,-100,"pi/4 mrad")

for mod in x1_Aside:

    x1 = (x1_Aside[mod] + x1_Cside[mod]) / 2
    y1 = (y1_Aside[mod] + y1_Cside[mod]) / 2

    x2 = (x2_Aside[mod] + x2_Cside[mod]) / 2 
    y2 = (y2_Aside[mod] + y2_Cside[mod]) / 2 

    
    # Divide by z to get an angle, divide by 2 b/c were drawing it twice
    dxA = (x2_Aside[mod] - x2)/z1_Aside[mod]/2
    dyA = (y2_Aside[mod] - y2)/z1_Aside[mod]/2

    dxC = (x2_Cside[mod] - x2)/z1_Aside[mod]/2
    dyC = (y2_Cside[mod] - y2)/z1_Aside[mod]/2

        
    line.SetLineColor(kGreen-2)
    line.SetLineWidth(3)
    line.DrawArrow(x1,y1,x1+ROTXY_FACTOR*(dxA),y1+ROTXY_FACTOR*(dyA),0.01,"-")

    line.SetLineColor(kGreen+2)
    line.SetLineWidth(3)
    line.DrawArrow(x1,y1,x1+ROTXY_FACTOR*(dxC),y1+ROTXY_FACTOR*(dyC),0.01,"-") 


trtDir.cd()
BarrelRotxy.Write()
rootFile.cd()
