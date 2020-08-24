#!/usr/bin/env python
"""
The `covarianceToolsLibrary.py` can be used directly to manipulate covariance info and evaluate goodness of fit, inmported into your own python script.
Examples of how to do this are provided in the `examples/covarianceTool-examples/` directory

Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch  
"""

import os, sys, yaml

import ROOT as r
from yoda import *
from array import array

from xml.etree import ElementTree
import numpy as np
from numpy import linalg as LA
from scipy import stats

from collections import defaultdict
r.gErrorIgnoreLevel=r.kError


def setCorrInfo(h, binErrors):
  """
  `h` AO (the Scatter you want to set the correlation info for) 
  `binErrors`
  """
  corrIn = yaml.dump(binErrors, default_flow_style=True, default_style='',width=1e6)
  h.setAnnotation("ErrorBreakdown",corrIn)

def addSystematicToCorrInfo(h, systName, binErrorsToAdd):
  corr= h.annotation('ErrorBreakdown')
  binErrors = yaml.load(corr)
  if binErrorsToAdd.keys() != binErrors.keys() :
    print("[ERROR], could not add ", systName , " to ", h, " because the bins are inconsistent")
    print(binErrors.keys(), " vs ", binErrorsToAdd.keys())
    return
  for k,v in binErrorsToAdd.items():
    binErrors[k][systName]=v
  setCorrInfo(h,binErrors)

def getCorrInfo(h,names=[]):
  if not (h.hasAnnotation('ErrorBreakdown')) : 
    print("[ERROR] analsyis object", h, " has no stored correlation info!" )
    return None
  binErrors= yaml.load(h.annotation('ErrorBreakdown'))
  if len (names)> 0: 
    binErrorsFiltered= {}
    for k,v in binErrors.items(): #loop through bins
      foundName=False
      for name in names:
        for k2,v2 in v.items(): #loop through systematics
          if k2==name : 
            foundName=True
            binErrorsFiltered.setdefault(k,{})[k2]=v2
      if not foundName:
        print("[WARNING] cannot find systematic with name ", name)
    binErrors=binErrorsFiltered
  return binErrors

def hasAsymmetricErrors(ao):
   nbins = ao.numPoints()
   corr = yaml.load(ao.annotation('ErrorBreakdown'))
   if len(corr)==0 : return True
   systList= corr[0].keys()
   hasAsymmErrs=False
   for sname in systList:
     for ibin in range (nbins):
       up=((corr[ibin][sname]['up']))
       dn=((corr[ibin][sname]['dn']))
       if (up!=-1*dn) :
         hasAsymmErrs=True
         break
   return hasAsymmErrs

def makeCovarianceMatrix( ao, ignore_corrs=False ):
   nbins = ao.numPoints()
   corr = ao.annotation('ErrorBreakdown') if '1.7' in yoda.version() else yaml.load(ao.annotation('ErrorBreakdown'))
   dummyM = np.outer(range(nbins), range(nbins) )
   covM = np.zeros(dummyM.shape)
   if len (corr)==0 : 
     for ibin in range (nbins):
       covM[ibin][ibin]= ((ao.points()[ibin].yErrs()[0]+ao.points()[ibin].yErrs()[1])/2)**2
       if covM[ibin][ibin]==0 :  covM[ibin][ibin]==1
     print("[WARNING], ao ", ao.path , " has no errors. Setting cov martix to unit... but consider excluding it !")
     return covM
   systList= corr[0].keys()
   totErr = np.zeros(nbins)
   for sname in systList:
     systErrs = np.zeros(nbins)
     for ibin in range (nbins):
       nomVal =  ao.points()[ibin].y()
       systErrs[ibin]=((abs(corr[ibin][sname]['up']))+(abs(corr[ibin][sname]['dn'])))*0.5 #up/dn are considered the same since NoToys assumes symmetric errors
       #systErrs[ibin]=((abs(corr[ibin][sname]['up']))) #up/dn are considered the same since NoToys assumes symmetric errors
     if (ignore_corrs or 'stat' in sname):
        covM += np.diag(systErrs*systErrs)
     else:
        covM += np.outer(systErrs,systErrs)
   if LA.det(covM)==0:
     printMatrix(covM)
     print("[ERROR], the above cov matrix is singular for ", ao.path(), " ! exit")
     print("( if this is a cov matrix for a super-AO, perhaps you have several highly correlated distributions in your set. Try vetoing some.)")
   return covM

def hasValidErrorBreakdown(ao):
  if not ao.hasAnnotation("ErrorBreakdown") : return False
  if len( ao.annotation("ErrorBreakdown"))==0 : return False
  corrs = getCorrInfo(ao)
  for iBin, binErrs in corrs.items():
    binTotal=[0.,0.]
    for sys,err in binErrs.items():
      binTotal[0]=(binTotal[0]**2 + err['up']**2)**0.5
      binTotal[1]=(binTotal[1]**2 + err['dn']**2)**0.5
    if binTotal[0]==0 and binTotal[1]==0 : return False
  return True

def makeCovarianceMatrixFromToys( ao , ntoys=10000, ignore_corrs=False):
   nbins = ao.numPoints()
   corr = yaml.load(ao.annotation('ErrorBreakdown'))
   systList= corr[0].keys()
   toys  = np.zeros( [ ntoys, nbins ] )
   
   for itoy in range( ntoys ):
      shift = np.zeros( len(systList) )
      delta = np.zeros( nbins )
      isyst = 0
      for sname in systList:
         if (ignore_corrs or 'stat' in sname):
            for n in range(nbins):
               l = np.random.normal(0,1) #bin-bin independent stat fluctuations
               s = ((corr[n][sname]['up'])) if l >= 0. else ((corr[n][sname]['dn'])) 
               delta[n] += abs(l) * s
         else:
            l = np.random.normal(0,1)
            shift[isyst] = l #bin-bin correlated fluctuations

            for n in range(nbins):
               s = ((corr[n][sname]['up'])) if l >= 0. else ((corr[n][sname]['dn'])) 
               delta[n] += abs(l)  * s # linear interpolation
         isyst+=1

      for n in range(nbins):
         toys[itoy][n] =  ( 1. + delta[n] )

   cov = np.cov( toys, rowvar=0 )
   if LA.det(cov)==0:
     printMatrix(cov)
     print("[ERROR], the above cov matrix is singular for ", ao.path, " ! exit")
     print("( if this is a cov matrix for a super-AO, perhaps you have several highly correlated distributions in your set. Try vetoing some.)")
   return cov 

def plotVariations( ao , outdir, label, threshold=0.05, xLabel=None, title=None):
   #print(ao, ao.numPoints())
   if ao.dim() !=2 : return
   tgraphs ={}
   tgraphs['all'] =[r.TGraphAsymmErrors(),r.TGraphAsymmErrors(),r.TGraphAsymmErrors(),r.TGraphAsymmErrors()]
   maxErr=0
     
   nbins = ao.numPoints()
   corr = ao.annotation('ErrorBreakdown') if '1.7' in yoda.version() else yaml.load(ao.annotation('ErrorBreakdown'))

   if len(corr)==0 : return
   systList= corr[0].keys()
   
   all_up=[0 for i in range(nbins)]
   all_dn=[0 for i in range(nbins)]
   for sname in systList :
     tgu=r.TGraphAsymmErrors()
     tgu_line=r.TGraphAsymmErrors()
     tgd=r.TGraphAsymmErrors()
     tgd_line=r.TGraphAsymmErrors()
     pt=0
     draw=0
     for i in range(nbins):
        nominalx=ao.points()[i].x()
        nominaly=ao.points()[i].y()
        xerr=abs(nominalx - ao.points()[i].xMin()) 
        tgu.SetPoint(pt,nominalx,1)
        tgd.SetPoint(pt,nominalx,1)
        if nominaly==0 : continue
        eup= 1 - (((corr[i][sname]['up']))+nominaly)/nominaly
        edn= 1 - (((corr[i][sname]['dn']))+nominaly)/nominaly
        all_up[i]=(all_up[i]**2 + eup**2)**0.5
        all_dn[i]=(all_dn[i]**2 + edn**2)**0.5
        if (abs(eup) > threshold): draw=1
        if (abs(edn) > threshold): draw=1
        if (abs(edn) > threshold): draw=1
        if abs(all_up[i])> maxErr: maxErr=abs(all_up[i])
        #if ('ident' in sname): draw=1
        #if ('umi' in sname): draw=1
        tgu.SetPointError(pt,xerr,xerr,0,eup)
        tgd.SetPointError(pt,xerr,xerr,0,edn)
        tgu_line.SetPoint(pt,nominalx,1+eup)
        tgd_line.SetPoint(pt,nominalx,1+edn)
        tgu_line.SetPointError(pt,xerr,xerr,0,0)
        tgd_line.SetPointError(pt,xerr,xerr,0,0)
        tgraphs['all'][0].SetPoint(pt,nominalx,nominaly/nominaly)
        tgraphs['all'][1].SetPoint(pt,nominalx,nominaly/nominaly)
        tgraphs['all'][2].SetPoint(pt,nominalx,1-all_dn[i])
        tgraphs['all'][3].SetPoint(pt,nominalx,1+all_up[i])
        tgraphs['all'][0].SetPointError(pt,xerr,xerr,0,-1*all_dn[i])
        tgraphs['all'][1].SetPointError(pt,xerr,xerr,0,all_up[i])
        tgraphs['all'][2].SetPointError(pt,xerr,xerr,0,0)
        tgraphs['all'][3].SetPointError(pt,xerr,xerr,0,0)
        pt+=1
     if (draw): tgraphs[sname]=[tgd,tgu,tgd_line,tgu_line]
   r.gROOT.SetBatch()
   tc = r.TCanvas("c","c",500,500)
   #tc.SetLogy()
   same=0
   itg=0
   leg = r.TLegend(.13,.6,.9,.8)
   leg.SetNColumns(2)
   leg.SetBorderSize(0)
   leg.SetFillColor(0)
   leg.SetFillStyle(0)
   leg.SetTextFont(42)
   leg.SetTextSize(0.025)
       
   for tgname,tg in sorted(tgraphs.items(), key=lambda x:abs(x[1][1].GetErrorYhigh(0)), reverse=True):
      itg+=1
      isCorr = not (('stat' in tgname) or ('all' in tgname))
      if 'nominal' == tgname: continue
      if 'mc' == tgname: continue
      #tg[1].SetMarkerColor(1) #kBlack
      #tg[1].SetMarkerStyle(21)
      tg[0].SetFillColorAlpha(0+itg, 0.3)
      tg[1].SetFillColorAlpha(0+itg, 0.3)
      tg[0].SetMarkerColor(1) #kBlack
      tg[2].SetMarkerStyle(23)
      tg[2].SetMarkerColor(0+itg)
      tg[2].SetLineColor(0+itg)
      tg[3].SetMarkerStyle(22)
      tg[3].SetMarkerColor(0+itg)
      tg[3].SetLineColor(0+itg)
      leg.AddEntry(tg[0],tgname,"f")
      if same:
        tg[0].Draw("p2 same")
        tg[1].Draw("p2 same")
        if isCorr: tg[2].Draw("P same")
        if isCorr: tg[3].Draw("P same ")
      else :
        tg[0].SetMaximum((1.+maxErr)*1.33)
        tg[0].SetMinimum(1.-maxErr)
        tg[0].Draw("AP2")
        tg[1].Draw("P2 same ")
        if isCorr: tg[2].Draw("P same")
        if isCorr: tg[3].Draw("P same")
        same=1
   leg.Draw()
   lat= r.TLatex()
   leg.SetTextFont(42)
   lat.SetNDC()
   lat.SetTextSize(0.05)
   lat.DrawLatex(0.1,0.91,"#it{ATLAS} #bf{Internal}" )
   lat.SetTextSize(0.035)
   #lat.DrawLatex(0.13,0.85,"#bf{Error analysis for } %s" % (title.replace("$","") if title else label))
   lat.DrawLatex(0.13,0.83,"#bf{All variations >%1.1f%% anywhere in range:}" % (threshold*100))
   if xLabel : 
      lat.SetTextAlign(33)
      lat.SetTextSize(0.04)
      lat.DrawLatex(0.9,0.05,"#bf{%s}" %xLabel.replace("$","").replace("\\text","") )
   lat.SetTextAngle(90)
   lat.DrawLatex(0.02,0.9,"#bf{Relative Uncertainty}" )
   outfile="%s/%s-variations"%(outdir,label)
   tc.SaveAs(outfile+".pdf")
   tc.SaveAs(outfile+".png")


def getDiagonalCovMatricesFromScatter(scat):
 npts=scat.numPoints()
 cov_up = np.zeros( [npts, npts] )
 cov_dn = np.zeros( [npts, npts] )
 for i in xrange(npts):
   yed_, yeu_ = scat.points()[i].yErrs()
   cov_up[i][i]=yeu_*yeu_
   cov_dn[i][i]=yed_*yed_
 return[cov_dn,cov_up]

def getDiagonalCovMatricesFromHisto(scat):
 npts=scat.numBins
 cov_up = np.zeros( [npts, npts] )
 cov_dn = np.zeros( [npts, npts] )
 for i in xrange(npts):
   err = scat.bins[i].xStdErr()
   cov_up[i][i]=err*err
   cov_dn[i][i]=err*err
 return[cov_dn,cov_up]

def scatterToMatrix( h, bw = False ):
  npts = h.numPoints() 
  dim = h.dim()
  mo = np.zeros( [npts, npts] )
  for i in xrange(npts):
    for j in xrange(npts):
      mo[0][i] = h.points()[i].val(dim)
      if bw == True:
        width= (abs(h.points()[i].errs(dim)[0]))
        width += (abs(h.points()[i].errs(dim)[1]))
        mo[0][i] *= 1./width
  return mo

def histoToMatrix( h, bw = False ):
  npts = h.numBins 
  mo = np.zeros( [npts, npts] )
  for i in xrange(npts):
    for j in xrange(npts):
      mo[0][i] = h.bins[i].sumW()
      if bw == True:
        width= h.bins[i].xWidth()
        mo[0][i] *= 1./width
  return mo


def printMatrix(matrix, outfile=None ):
   if type(outfile)==type(str()): outfile= open("%s.txt" %outfile, w)
   nCols=matrix.shape[0]
   nRows=matrix.shape[1]
   colNames="X  "
   for i in range(nCols):
     colNames+="%d  "%i
   if outfile is not None :outfile.write(colNames+"\n")  
   else: print(colNames+"\n" )
   
   for j in range(nRows):
      rowString="%d|  "%j
      for i in range(nCols):
        rowString+="%.6f  "%matrix[i][j]
      if outfile is not None :outfile.write(rowString+"\n") 
      else: print(rowString+"\n")
   if outfile is not None : outfile.write("Eigenvalues  %s\n"%("  ".join(["%.3f"%val for val in LA.eigvals( (matrix) )]))) 
   else : print("Eigenvalues  %s\n"%("  ".join(["%.6f"%val for val in LA.eigvals( (matrix) )])))
   #evals, evecs = LA.eig( matrix)
   #print(evecs)

   #print(" integral=%f" % integral)

def drawMatrix(matrix, outfile , xLabel=None):
   
   nCols=matrix.shape[0]
   nRows=matrix.shape[1]
   assert(nCols==nRows)
   nBins=nCols
   r.gROOT.SetBatch()
   lat= r.TLatex()
   lat.SetTextSize(0.02)
   lat.SetTextAngle(45)
   lat.SetTextAlign(22)
   Red= array('d',   [1.0,1.0,0.0])
   Grn= array('d',   [0.0,1.0,0.0])
   Blu=array('d',    [0.0,1.0,1.0])
   Length= array('d',[0.0,0.5,1.0])
   r.TColor.CreateGradientColorTable(len(Length),Length,Red,Grn,Blu,50);
   if ('chi2' in outfile): nRows+=1 
   #th2=r.TH2D(outfile.split("/")[-1],outfile.split("/")[-1],nRows,0,nRows,nBins,0,nBins) 
   th2=r.TH2D(outfile.split("/")[-1],outfile.split("/")[-1],nRows,0,nRows,nBins,0,nBins)
   th2.SetTitle("")
   totalOverall=0.
   totalsPerRow=[]
   for j in range(nBins):
      th2.GetYaxis().SetBinLabel(j+1,"%d"%(nBins-j))
      total=0.
      for i in range(nBins):
        th2.GetXaxis().SetBinLabel(i+1,"%d"%(i+1))
        th2.Fill(i,nBins-j-1,matrix[i][j])
        total+=matrix[i][j]
      totalOverall+=total
      if('chi2' in outfile): th2.Fill(nBins,nBins-j-1,total)
      if('chi2' in outfile): th2.GetXaxis().SetBinLabel(nBins+1,"TOTAL=%.2f" % totalOverall)
      totalsPerRow.append(total)
   tc = r.TCanvas("c","c",500,500)
   tc.SetRightMargin(0.2)
   if('covariance' in outfile) : tc.SetLogz()
   th2.Draw("colz")
   for j in range(nBins):
      for i in range(nBins):
        lat.DrawLatex(i+0.5,nBins-j-0.5,"%.2f"%matrix[i][j])
      if('chi2' in outfile):lat.DrawLatex(nBins+0.5,nBins-j-0.5,"%.2f"%totalsPerRow[j])
   #if('chi2' in outfile):lat.DrawLatex(nBins+0.5,0-0.5,"%.2f"%totalOverall)
   th2.SetMaximum(max(abs(th2.GetMaximum()),abs(th2.GetMinimum())))
   th2.SetMinimum(-1*(th2.GetMaximum()))
   th2.SetStats(0)
   lat= r.TLatex()
   lat.SetNDC()
   lat.SetTextSize(0.05)
   lat.DrawLatex(0.1,0.91,"#it{ATLAS} #bf{Internal}" )
   lat.SetTextSize(0.035)
   #lat.DrawLatex(0.13,0.85,"#bf{Error analysis for } %s" % (title.replace("$","") if title else label))
   if xLabel : 
      lat.SetTextAlign(33)
      lat.SetTextSize(0.04)
      lat.DrawLatex(0.9,0.05,"#bf{%s}" %xLabel.replace("$","").replace("\\text","") )
      lat.SetTextAngle(90)
      lat.DrawLatex(0.02,0.9,"#bf{%s}" %xLabel.replace("$","").replace("\\text","") )
   tc.SaveAs(outfile+".pdf")
   tc.SaveAs(outfile+".png")
   


def manualChi2(data,mc,cov, verbosity=0):
   if (verbosity>2):
     print()
     print()
     print("[INFO] Manual Chi2 Calculation, (verbosity)=", (opts.verbosity >0))
   Nbins = cov.shape[0]
   chi2=0
   for i in range (Nbins):
    nEvData=data[0][i]
    nEvMC=mc[0][i]
    sigmaSq=cov[i][i]
    sigma = (sigmaSq)**0.5
    residual=nEvData-nEvMC
    if (verbosity>2):
      print("[INFO] Manual Chi2 Bin i=%d: data=%f, mc=%f, res=%f vs sigma=%f res/sigma=%.2f , (r/s)**2=%.2f"%(i,nEvData,nEvMC,residual,sigma,float(residual)/sigma, (float(residual)/sigma)**2))
    chi2+=(residual/sigma)**2
   if (verbosity>2):
     print("[INFO] FINAL MANUAL CHI2=", chi2)
   return chi2
  

def calculateChi2(data, mc, cov=None, verbosity=0):
    ndf = data.numPoints()
    values={}
    for name,obj in {'data':data,'mc':mc}.items():
      if type(obj) in (yoda.Scatter1D,yoda.Scatter2D,yoda.Scatter3D):
         #tmpErrors = GetDiagonalCovMatricesFromScatter(obj)
         #TODO handle asymmetric errors...
         #errorsSq[name] = tmpErrors[0]
         #errorsSq[name] += tmpErrors[1]
         #errorsSq[name] *= float(0.5)
         #ndf=errorsSq[name].shape[0]
         values[name]= scatterToMatrix(obj) 
      else: #Histo*D hopefully !
        #ndf=obj.numBins
        #errorsSq[name]=  np.zeros( [ndf, ndf] )
        values[name]= HistoToMatrix(obj, bw=True) #bw true ??
   
    
    v = values['data'].copy()
    v -= values['mc'].copy()
    meas=None
    if cov is None:
     covData=makeCovarianceMatrix(data)
     covMC=makeCovarianceMatrix(mc)
     cov=covData+covMC
    
    if (cov.shape[0]!=cov.shape[1]):
      print("[ERROR] the specified covariance matrix is not square!", cov)
      exit(1)
    if (cov.shape[0]!=v.shape[0]):
      print("[ERROR] the specified covariance matrix is incompatible with the vector of data-MC", cov , v)
      exit(1)
    #cov0=makeCovarianceMatrix(data,True)
    #print("diag cov this matrix has determinant ",  LA.det(cov0))
    precision_matrix = LA.inv(cov.copy())
    
    if (verbosity >1) :
      print("[INFO]: precision matrix:" ,precision_matrix)

    vT=v.copy().transpose()
    chi2tmp = (v.dot(precision_matrix)).dot(vT)
    chi2 = chi2tmp[0][0]
    pvalue = (1 - stats.chi2.cdf(chi2, ndf))
    
    return chi2, ndf, pvalue, chi2ContributionMatrix(v,precision_matrix)

def correlationMatrix(covMatrix):
  nbins  = covMatrix.shape[0]
  corr = np.zeros( [nbins, nbins] )
  for i  in range(nbins):
     sigma_i = covMatrix[i][i]
     for j in range(nbins):
        sigma_j  = covMatrix[j][j]
        corr[i][j] = covMatrix[i][j] / np.sqrt(sigma_i * sigma_j)

  return corr


def updateAOinYODA(ao,infile):
    aos = yoda.read(infile) 
    aos[ao.path]=ao
    yoda.write(sorted(aos.values()), infile)
    

def chi2ContributionMatrix(diff,prec):
    nbins = prec.shape[0]
    diffT = diff.copy().transpose()
    mo= np.zeros( [nbins, nbins] )
    for j in range( nbins ):
       for i in range( nbins ):
         mo[i][j]=diff[0][j]*prec[i][j]*diffT[i][0]
         #print("row ",i ," col ", j ," diff ", diff[0][j] , " diffT ", diffT[i][0] , " prc " , prec[i][j]  , " = ", mo[i][j])
    return mo

def chi2ContribsByRow(chi2contribs):
    nbins = chi2contribs.shape[0]
    mo={}
    for j in range( nbins ):
       mo[j]=0.0
       for i in range( nbins ):
         mo[j]+=float(chi2contribs[i][j])
         #print("row ",i ," col ", j ," diff ", diff[0][j] , " diffT ", diffT[i][0] , " prc " , prec[i][j]  , " = ", mo[i][j])
    return mo

def makeSuperAO(aoList, verbosity=0):
    aoOut=yoda.core.Scatter2D()
    pointCounter=0
    corrOut={}
    pointsOut=[]
    allSystematics=[]
    for aoIn in aoList:
      corrIn = aoIn.annotation('ErrorBreakdown') if '1.7' in yoda.version() else yaml.load(aoIn.annotation('ErrorBreakdown'))
      if len(corrIn)==0 : continue
      for ipt in range(aoIn.numPoints()):
        for syst in corrIn[ipt].keys():
          if not syst in allSystematics: allSystematics.append(syst)
    
    for aoIn in sorted(aoList, key= lambda x : x.name):
      corrIn = aoIn.annotation('ErrorBreakdown') if '1.7' in yoda.version() else yaml.load(aoIn.annotation('ErrorBreakdown'))
      if len(corrIn)==0 : continue
      for ipt in range(aoIn.numPoints()):
        aoOut.addPoint(yoda.core.Point2D(pointCounter,aoIn.points()[ipt].y()))
        totalErrorUp=0
        for syst in allSystematics:
          if syst in corrIn[ipt].keys():
             corrOut.setdefault(pointCounter,{})[syst]=corrIn[ipt][syst]
             totalErrorUp+=corrIn[ipt][syst]['up']*corrIn[ipt][syst]['up']
          else:
             corrOut.setdefault(pointCounter,{})[syst]={'up':0.0,'dn':0.0}
        pointsOut.append(aoIn.points()[ipt])
        pointCounter+=1
    #aoOut.addPoints(pointsOut)
    for i in range (aoOut.numPoints()):
      if (verbosity)>1 : print("[INFO] Super AO points ", aoOut.points()[i])
    corrs = yaml.dump(corrOut, default_flow_style=True, default_style='',width=1e6)
    aoOut.setAnnotation("ErrorBreakdown",corrs)
    name=aoList[0].path.replace(aoList[0].name,"")
    name+="superAO"
    for ao in sorted(aoList, key= lambda x : x.name):
      name+="_%s"%ao.name
    aoOut.setAnnotation("Path",name)
    return aoOut
 
