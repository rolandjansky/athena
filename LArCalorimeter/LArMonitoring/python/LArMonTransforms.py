#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
#
# Author: Margherita Spalla (margherita.spalla@cern.ch)
# Function for post-processing of histograms from LArMonitoring
#
# The functions in this file are being implemented through the yaml config file inside the DataQuality package:
#   DataQuality/DataQualityUtils/data/postprocessing/LArMonPostProc.yaml
#


from ROOT import TMath, TH2F, TProfile
import cppyy

def setMaxMin(inputs,maxVal=0,minVal=0,useMax=True,useMin=True):
    """ This function sets the histogram max/min """
    assert len(inputs) == 1 , len(inputs)  # for testing 
    cl = inputs[0][1][0].Clone()
    
    if useMax:
        cl.SetMaximum(maxVal)
        pass

    if useMin:
        cl.SetMinimum(minVal)
        pass

    return [cl]

cppyy.cppdef("""
void takemax(int sc, const TH2I* inhist, TH2I* outhist) {
    for (int i = 0; i < outhist->GetSize(); ++i) {
        if (inhist->At(i) != 0 && sc > outhist->At(i)) {
            outhist->SetBinContent(i, sc);
        }
    }
}
""")

# @profile
def fillWithMaxCoverage(inputs,isFtSlotPlot=True):
    """ For each bin, fill the output with the max filled error code. All histograms should have the same bin content"""
    inputs = list(inputs) # evaluate this just once
    assert len(inputs) >= 1 , len(inputs)  # for testing

    cl = inputs[0][1][0].Clone()
    partition = inputs[0][0]['part']
    if 'side' in inputs[0][0]:
        partition=partition+inputs[0][0]['side']
        pass
    if 'sampling' in inputs[0][0]:
        sampling = int(inputs[0][0]['sampling'])
    else:
        sampling=-1 #a default value, != 0,1,2,3
        pass
    
    doFixEmptyBins = (not isFtSlotPlot and (('EMB' in partition and sampling==1) or ('EMEC' in partition and sampling==2) or ('HEC' in partition)))
    
    #if we only have one and doesn't need bin fix, return it
    if len(inputs)==1 and not doFixEmptyBins: 
        cl.SetMaximum(4.4)
        cl.SetMinimum(1)
        return [cl]
    
    if len(inputs)>1 :
        #check for incompatible histograms
        Nx=cl.GetNbinsX()
        Ny=cl.GetNbinsY()
        binsok=True
        for j in range(1,len(inputs)):
            if inputs[j][1][0].GetNbinsX()!=Nx:
                binsok=False
                break
            if inputs[j][1][0].GetNbinsY()!=Ny:
                binsok=False
                break
            pass
        assert binsok , "Histograms must all have the same number of bins"

        #fill with max 
        cl.Clear()
        for i in inputs:
            statusCode=int(i[0]['sc'])
            h = i[1][0]
            cppyy.gbl.takemax(statusCode, h, cl)


    cl.SetMaximum(4.4)
    cl.SetMinimum(1)
    
    if(not doFixEmptyBins): 
        return [cl]

    #only for some eta-phi plots: fix empty bins
    if 'EMBA' in partition:
        ieta_min = 1
        ieta_max = 447
        iphi_min = 1
        iphi_max = 256
        iphi_step = 4
        Diphi_toGet = 1
        Diphi_toSet = [0,2,3]
    elif 'EMBC' in partition:
        ieta_min = 4
        ieta_max = 450
        iphi_min = 1
        iphi_max = 256
        iphi_step = 4
        Diphi_toGet = 2
        Diphi_toSet = [0,1,3]
    elif 'EMECA' in partition:
        ieta_min = 44
        ieta_max = 51
        iphi_min = 1
        iphi_max = 256
        iphi_step = 4
        Diphi_toGet = 1
        Diphi_toSet = [0,2,3]
    elif 'EMECC' in partition:
        ieta_min = 1
        ieta_max = 7
        iphi_min = 1
        iphi_max = 256
        iphi_step = 4
        Diphi_toGet = 2
        Diphi_toSet = [0,1,3]
    else: #HEC
        hecetabinmin=[11,11,9,9]
        hecetabinmax=[14,13,12,12]
        if partition=='HECA':
            ieta_min = hecetabinmin[sampling]
            ieta_max = hecetabinmax[sampling]
        else:
            ieta_min = 1
            ieta_max = hecetabinmax[sampling]-hecetabinmin[sampling]
            pass
        iphi_min = 1
        iphi_max = 64
        iphi_step = 2
        Diphi_toGet = 1
        Diphi_toSet = [0]
        pass

    iphi=iphi_min
    while iphi<=iphi_max:
        ieta=ieta_min
        while ieta<=ieta_max:
            content = cl.GetBinContent(ieta,iphi+Diphi_toGet)
            error = cl.GetBinError(ieta,iphi+Diphi_toGet)
            for Di in Diphi_toSet:
                cl.SetBinContent(ieta,iphi+Di,content)
                cl.SetBinError(ieta,iphi+Di,error)
                pass
            ieta=ieta+1
            pass
        iphi=iphi+iphi_step
        pass

    return [cl]
                




def normToEntries(inputs,titleToReplace="",replaceWith=""):
    """ This function normalises histogram 1 to the number of entries of histogram 2 (which is supposed to represent the number of events) """
    assert len(inputs) == 1 , len(inputs)
    assert len(inputs[0][1]) == 2 , len(inputs[0][1])

    cl = inputs[0][1][0].Clone()
    Nen = inputs[0][1][1].GetEntries()
    if Nen!=0:
        cl.Scale(100./Nen)
        pass

    if titleToReplace=="":
        return [cl]

    tit = cl.GetTitle()
    tit=tit.replace(titleToReplace,replaceWith)
    cl.SetTitle(tit)

    return [cl]



def normToEntriesAndSetMin(inputs,minVal=0,maxVal=0,useMax=False):
    """ This function normalises histogram 1 to the number of entries of histogram 2 (which is supposed to represent the number of events) and sets the histogram max/min """

    assert len(inputs) == 1 , len(inputs)
    assert len(inputs[0][1]) == 2 , len(inputs[0][1])

    cl = inputs[0][1][0].Clone()
    Nen = inputs[0][1][1].GetEntries()
    if Nen!=0:
        cl.Scale(1./Nen)
        pass

    cl.SetMinimum(minVal)
    if useMax:
        cl.SetMaximum(maxVal)
        pass

    return [cl]



def divideHist(inputs,titleToReplace="",replaceWith=""):
    """ This function returns the ratio of two ROOT histograms """
    assert len(inputs) == 1  
    assert len(inputs[0][1]) == 2

    cl = inputs[0][1][0].Clone()
    cl.Divide(inputs[0][1][1])

    if titleToReplace=="":
        return [cl]

    tit = cl.GetTitle()
    tit=tit.replace(titleToReplace,replaceWith)
    cl.SetTitle(tit)

    return [cl]


def digitSummary(inputs,TreshOut=5,TreshSat=5,TreshNull=5):

    cl = TH2F("summary","LArDigit Summary;Status;Partition",4,0.,4.,8,0.,8.)
    cl.SetDirectory(0)
    cl.GetYaxis().SetBinLabel(1,"EMBC")
    cl.GetYaxis().SetBinLabel(2,"EMBA")
    cl.GetYaxis().SetBinLabel(3,"EMECC")
    cl.GetYaxis().SetBinLabel(4,"EMECA")
    cl.GetYaxis().SetBinLabel(5,"HECC")
    cl.GetYaxis().SetBinLabel(6,"HECA")
    cl.GetYaxis().SetBinLabel(7,"FCalC")
    cl.GetYaxis().SetBinLabel(8,"FCalA")
    cl.GetXaxis().SetBinLabel(1,"OutOfRange")
    cl.GetXaxis().SetBinLabel(2,"Saturation")
    cl.GetXaxis().SetBinLabel(3,"Null Digits")  
    cl.GetXaxis().SetBinLabel(4,"Mean Time")    
    cl.GetXaxis().SetLabelSize(0.055)
    cl.GetYaxis().SetLabelSize(0.055)           

    for i in inputs:
        assert len(i[1])==1, len(i[1])

        if i[1][0].GetEntries()==0:
            continue

        #y bin
        pr=i[0]['part']
        if "EMB" in pr:
            iy=1
        elif "EMEC" in pr:
            iy=3
        elif "HEC" in pr:
            iy=5
        elif "FCal" in pr:
            iy=7
        else:
            iy=-10
            pass
        
        if i[0]['side'] == "A":
            iy=iy+1
            pass

        #x bin
        hn=i[0]['histname']
        if hn=="EnVsTime":
            ix=4
            content = 0
            for xbin in range(1,i[1][0].GetNbinsX()+1):
                sumx=0
                for ybin in range(1,i[1][0].GetNbinsY()+1):
                    sumx=sumx+i[1][0].GetBinContent(xbin,ybin)
                    pass
                content=content+(sumx*i[1][0].GetXaxis().GetBinLowEdge(xbin))
                pass
            content=content/float(i[1][0].GetEntries())
        else:
            if "OutOfRange" in hn:
                ix=1
                treshold=TreshOut
            elif "Saturation" in hn:
                ix=2
                treshold=TreshSat
            elif "NullDigit" in hn:
                ix=3
                treshold=TreshNull
            else:
                ix=-1
                treshold=1e4
                pass

            content=0
            for xbin in range(i[1][0].GetNbinsX()):
                for ybin in range(i[1][0].GetNbinsY()):
                    if i[1][0].GetBinContent(xbin,ybin)>treshold :
                        content=content+1
                        pass
                    pass
                pass
            pass

        if ix>0 and iy>0:
            cl.SetBinContent(ix,iy,content)
            pass

        pass #end of input loop

    return [cl]


def computeCorrelations(inputs):
    #check that the inputs are ok    
    assert len(inputs)==2 , len(inputs)
    
    assert ((inputs[0][0]['name']=='average' and inputs[1][0]['name']=='partialSum') or (inputs[1][0]['name']=='average' and inputs[0][0]['name']=='partialSum')) , "Wrong inputs"

    if inputs[1][0]['name']=='partialSum':
        i_parSum=1
        i_av=0
    else:
        i_parSum=0
        i_av=1

    cl = inputs[i_parSum][1][0].Clone()
    cl.Clear()

    for i in range(1,cl.GetNbinsX()+1):
        mean1 = inputs[i_av][1][0].GetBinContent(i)
        sumVar1 = inputs[i_parSum][1][0].GetBinContent(i,i)
        N = inputs[i_av][1][0].GetBinEntries(i)
        if N==0:
            continue
        for j in range(i+1,cl.GetNbinsY()+1):
            mean2 = inputs[i_av][1][0].GetBinContent(j)
            sumVar2 = inputs[i_parSum][1][0].GetBinContent(j,j)
            if (sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2)==0:
                continue
            cor=(inputs[i_parSum][1][0].GetBinContent(i,j)-N*mean1*mean2)/TMath.Sqrt((sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2))
            cl.SetBinContent(i,j,cor) 
            cl.SetBinContent(j,i,cor) 


    return [cl]


def Mean(inputs):

    cl = TProfile("summary","Summary;Partition;Mean",8,0.,8.,0.,10., "s")
    cl.SetDirectory(0)
    cl.GetXaxis().SetBinLabel(1,"EMBA")
    cl.GetXaxis().SetBinLabel(2,"EMBC")
    cl.GetXaxis().SetBinLabel(3,"EMECA")
    cl.GetXaxis().SetBinLabel(4,"EMECC")
    cl.GetXaxis().SetBinLabel(5,"HECA")
    cl.GetXaxis().SetBinLabel(6,"HECC")
    cl.GetXaxis().SetBinLabel(7,"FCalA")
    cl.GetXaxis().SetBinLabel(8,"FCalC") 
    cl.GetXaxis().SetLabelSize(0.055)
    cl.GetYaxis().SetLabelSize(0.055)           

    for i in inputs:
        assert len(i[1])==1, len(i[1])
        
        #x bin
        pr=i[0]['part']
        if "EMB" in pr:
            ix=1
        elif "EMEC" in pr:
            ix=3
        elif "HEC" in pr:
            ix=5
        elif "FCal" in pr:
            ix=7
        else:
            ix=-10
            pass
        if i[0]['side'] == "C":
            ix=ix+1
            pass
        
        #y bin
        if i[1][0].GetEntries()==0:
            continue
        else: 
            if ix>=0:
                #filling the profile histogram for each x_bin separately
                for xbin in range(0,i[1][0].GetNbinsX()):
                    #cl.Fill (ix-1, i[1][0].GetBinContent(xbin)) #for all bins
                    if i[1][0].GetBinContent(xbin) > 0.: 
                        cl.Fill (ix-1, i[1][0].GetBinContent(xbin)) #only for non-zero bins
        pass #end of input loop

    return [cl]
