#!/usr/bin/env python
"""
The `covarianceTool.py` executable dis a helper which uses the functions provided by `covarianceToolsLibrary.py` to perform the  
comparison between MC predictions and HEPData records.

However, the `local/bin/covarianceToolsLibrary.py` can also be used directly to manipulate covariance info and evaluate goodness of fit.
Examples of how to do this are provided in the `examples` subdirectory: 
- `examples/covarianceTool-examples/README.md`

The first thing to do is test you can access the `covarianceTool.py` executable and see available options:
```
covarianceTool.py -h
```

Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch  
"""
import os, sys, yaml, yoda, rivet
import optparse
import urllib2, tarfile, StringIO
import systematicsTool as st

parser = optparse.OptionParser( usage = "%prog [options]" )
parser.add_option("--data", help="Input yoda file for data [%default]", dest="data", default="" )
parser.add_option("--mc", help="Input yoda files for mc (comma separarated list), specify mc label with ':', eg mc1path:'MC1',mc2path:'MC2'. If no label is specified, the name of the yoda file is used as a label [%default]", dest="mc", default="" )
parser.add_option("--use_toys", help="Force the covariance matrix to be produced with toys. By default the cov matrix is produced by direct propagation is errors are symmetric and with toys if errors are asymmetric.  [%default]", dest="use_toys", default=False ,  action='store_true')
parser.add_option("--ntoys", help="number of toys to throw if cov matrix produced from toys [%default]", dest="ntoys", default=1000 )
parser.add_option("--use_direct", help="Force the covariance matrix to be produced via direct propagation (the errors are symmetrized if needed). By default the cov matrix is produced by direct propagation if errors are symmetric and with toys if errors are asymmetric.  [%default]", dest="use_direct", default=False,  action='store_true' )
parser.add_option("--corr_data", help="Input file/dir to build the covariance matrices (comma separarated list) [%default]", dest="corr_data", default=None )
parser.add_option("--corr_mc", help="Input file/dir to build the covariance matrices (comma separarated list) [%default]", dest="corr_mc", default=None )
parser.add_option( "--ignore_corrs", help="ignore correlations in both data and mc and just assume total error is uncorrelated between bins [%default]", dest="ignore_corrs", default=False, action='store_true' )
parser.add_option( "--ignore_corrs_mc", help="Ignore correlations in the MC  [%default]", dest="ignore_corrs_mc", default=False, action='store_true' )
parser.add_option( "--ignore_corrs_data", help="Ignore correlations in theData  [%default]", dest="ignore_corrs_data", default=False, action='store_true' )
parser.add_option( "--ignore_errs_data", help="Ignore all errors in the data, and use only MC cov matrix [%default]", dest="ignore_errs_data", default=False, action='store_true' )
parser.add_option( "--ignore_errs_mc", help="Ignore all erros in the MC, and use only the data cov matrix [%default]", dest="ignore_errs_mc", default=False, action='store_true' )
parser.add_option( "--get_corr_from_web", help="Try to get correlation info from HEPMC record. Specify the inspire ID eg I1514251 [%default]", dest="get_corr_from_web", default=None )
parser.add_option( "--filter", help="Only process a subset of histograms (comma separated) [%default]", dest="filter", default="all" )
parser.add_option( "-v","--verbosity", help="Verbosity level  (0-9) [%default]", dest="verbosity", default=0 )
parser.add_option( "--make_super_aos", help="Make super ao of all measuremnts passing --filter, and calculate the chi2 for all measurements using a super-cov-matrix [%default]", dest="make_super_aos", default=False, action='store_true' )
parser.add_option( "--slides", help="Make summary slides [%default]", dest="slides", default=False, action='store_true' )
(opts, args) = parser.parse_args()


import covarianceToolsLibrary as ct
#hack
os.environ['TERM'] = 'linux'

histograms = {
   'data'       : None,
   'covariance' : {},
   'models'     : {},
}

def isfloat(x):
  # check if the object can be cast as a float
  try:
    float(x)
    return True
  except ValueError:
    return False

def convertHistToScatter(ao):
  print("convertHistToScatter: This is a placeholder, need to implement")
  exit(1)

def renameAOs(fin_name, aoMap):
  # make a new YODA file where the AOs are renamed as per the reference file
  fout_name=fin_name.replace(".yoda",".renamed.yoda")
  fout = open(fout_name,'w')
  aos = yoda.read(fin_name, unpatterns=".*RAW.*")
  for aopath,ao in aos.items():
    foundMatch=False
    for ao1,ao2 in aoMap.items():
      if ao1.name()==ao.name():
        ao.setAnnotation("Path",ao2.path().replace("REF/",""))
        foundMatch=True
      if ao2.name()==ao.name():
        ao.setAnnotation("Path",ao1.path().replace("REF/",""))
        foundMatch=True
    if foundMatch : yoda.writeYODA(ao, fout)
  fout.close()
  return fout_name

def makeBeamer(plotDictionary,outdir):
  # produce summary slides using beamer
  frames=[]
  dataDir=os.environ["SYSTTOOLSPATH"]+"/data/"
  os.system("mkdir -p %s " % outdir)
  samples=plotDictionary['chi2-value'].keys()
  histos=plotDictionary['chi2-value'][samples[0]].keys()
  os.system('cp %s/title.tex %s/title.tex' % (dataDir,outdir))
  os.system('cp %s/title.tex %s/title.tex' % (dataDir,outdir))
  os.system("sed -i -e 's|!TITLE!|%s|g' %s/title.tex" % (opts.data.replace('.yoda','').replace('_',' '),outdir))
  frames.append("%s/title.tex" % outdir)
  for histo in histos:
     if 'superAO' in histo:continue
     os.system('cp %s/section.tex %s/%s_section.tex' % (dataDir, outdir, histo))
     os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_section.tex" % (histo,outdir, histo))
     frames.append('%s/%s_section.tex' % (outdir, histo))
     os.system('cp %s/errorAnalysis.tex %s/%s_%s_errorAnalysis.tex' % (dataDir, outdir,'data',histo))
     os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_%s_errorAnalysis.tex" % (histo,outdir,'data',histo))
     os.system("sed -i -e 's|!SAMPLE!|%s|g' %s/%s_%s_errorAnalysis.tex" % ('data',outdir,'data',histo))
     os.system("sed -i -e 's|!ERRORBREAKDOWN!|%s|g' %s/%s_%s_errorAnalysis.tex" % (plotDictionary['error-analysis']['data'][histo],outdir,'data',histo))
     #os.system("sed -i -e 's|!COVMATRIX!|%s|g' %s/%s_%s_errorAnalysis.tex" % (plotDictionary['covariance-matrix']['data'][histo],outdir,'data',histo))
     os.system("sed -i -e 's|!COVMATRIX!|%s|g' %s/%s_%s_errorAnalysis.tex" % (plotDictionary['correlation-matrix']['data'][histo],outdir,'data',histo))
     os.system("sed -i -e 's|!COVDETAILS!|%s|g' %s/%s_%s_errorAnalysis.tex" % (plotDictionary['covDetails']['data'][histo],outdir,'data',histo))
     frames.append("%s/%s_%s_errorAnalysis.tex" % (outdir,'data',histo))
     for sample in samples:
       sampleNameClean=sample.replace("_"," ").replace("-"," ")
       os.system('cp %s/dataMCComparison.tex %s/%s_%s_dataMCComparison.tex' % (dataDir, outdir,sample,histo))
       os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_%s_dataMCComparison.tex" % (histo,outdir,sample,histo))
       os.system("sed -i -e 's|!SAMPLE!|%s|g' %s/%s_%s_dataMCComparison.tex" % (sampleNameClean,outdir,sample,histo))
       os.system("sed -i -e 's|!DATAVSMC!|%s|g' %s/%s_%s_dataMCComparison.tex" % (plotDictionary['data-vs-mc'][sample][histo],outdir,sample,histo))
       os.system("sed -i -e 's|!CHI2CONTRIBS!|%s|g' %s/%s_%s_dataMCComparison.tex" % (plotDictionary['chi2-contribs'][sample][histo],outdir,sample,histo))
       frames.append("%s/%s_%s_dataMCComparison.tex" % (outdir,sample,histo))
     #summary
     os.system('cp %s/sectionSummary.tex %s/%s_sectionSummary.tex' % (dataDir,outdir,histo))
     os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_sectionSummary.tex" % (histo,outdir,histo))
     os.system("sed -i -e 's|!SUMMARYPLOT!|%s|g' %s/%s_sectionSummary.tex" % (plotDictionary['summary-plot'][histo],outdir,histo))
     os.system("sed -i -e 's|!SUMMARYTABLE!|%s|g' %s/%s_sectionSummary.tex" % (plotDictionary['summary-table'][histo],outdir,histo))
     frames.append("%s/%s_sectionSummary.tex" % (outdir,histo))
  os.system('cp %s/section.tex %s/%s_section.tex' % (dataDir, outdir, 'overall'))
  os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_section.tex" % ('overall summary',outdir, 'overall'))
  frames.append('%s/%s_section.tex' % (outdir, 'overall'))
  os.system('cp %s/overallSummary.tex %s/%s_overallSummary.tex' % (dataDir, outdir,'overall'))
  os.system("sed -i -e 's|!AOREF!|%s|g' %s/%s_overallSummary.tex" % ('overall summary',outdir,'overall'))
  os.system("sed -i -e 's|!SUMMARYTABLE!|%s|g' %s/%s_overallSummary.tex" % (plotDictionary['summary-table']['all'],outdir,'overall'))
  frames.append("%s/%s_overallSummary.tex" % (outdir,'overall'))
  frames.append("%s/end.tex" % dataDir)
  catLine=" ".join(frames)
  os.system(" cat %s > %s/slides_%s.tex" % (catLine,outdir, opts.data.replace('.yoda',''))) 
  return " %s/slides_%s.tex" % (outdir,opts.data.replace('.yoda',''))
       

def makeSummaryTable(plotDictionary,outfile, hdataName=None ):
   # make a summary latex table of the chi2 values for each AO 
   outf = open(outfile,'w')
   samples=plotDictionary['chi2-value'].keys()
   if (hdataName):
     histos=[hdataName]
   else:
     histos=sorted(plotDictionary['chi2-value'][samples[0]].keys())

   line = "\\begin{tabular}{|l|"
   for hist in histos:
    line+="c|"
   line+="}"
   outf.write(line+'\n')
   outf.write("\\hline "+'\n')
   line = "  $\\chi^2$ / ndof "
   for hist in histos:
     if 'superAO' in hist: hist='global agreement'
     line+=" & %s " % hist
   line+="\\\\"
   outf.write(line+'\n')
   outf.write("\\hline"+'\n')
   for sample in samples:
     line = " %s " % sample.replace("-"," ").replace("_"," ")
     for hist in histos:
       line+=" & %s" % plotDictionary['chi2-value'][sample][hist]#.replace("chi2","$\\chi^2$")
     line+="\\\\"
     outf.write(line+'\n')
   outf.write("\\hline"+'\n')
   outf.write("\\end{tabular}"+'\n')
   outf.close()


def matchAOs(aoList1, aoList2, strategy=0, recursive=True):
  # try to establish a 1-1 mapping between AOs in two YODA files
  # eg in case of different naming conventions
  aoMap={}
  for ao1 in aoList1:
   aoMap.setdefault(ao1,[])
   for ao2 in aoList2:
     nBinsMatch=False
     namesMatch=False
     namesPartialMatch=False
     binWidthsMatch=False
     if (ao2.name() in ao1.name() ) : namesMatch=True
     if (ao1.name() in ao2.name() ) : namesMatch=True
     ao1NameTokens = ao1.name().split("/")[-1].split("-")
     ao2NameTokens = ao2.name().split("/")[-1].split("-")
     if len(list(set(ao1NameTokens).intersection(ao2NameTokens)))>=2 : namesPartialMatch= True
     if (ao1.numPoints() == ao2.numPoints() ) : 
       nBinsMatch=True
       for ipt in range(ao1.numPoints()):
         ao1BinLabels= "%.2f - %.2f"% (ao1.points()[ipt].xMin(),ao1.points()[ipt].xMax())
         ao2BinLabels= "%.2f - %.2f"% (ao2.points()[ipt].xMin(),ao2.points()[ipt].xMax())
         if (ao1BinLabels == ao2BinLabels): binWidthsMatch=True
     if strategy == 0:
       if (namesMatch and nBinsMatch and binWidthsMatch):
         aoMap.setdefault(ao1,[]).append(ao2) 
     elif strategy ==1 :
       if (nBinsMatch and binWidthsMatch):
         aoMap.setdefault(ao1,[]).append(ao2) 
     elif strategy ==2 :
       if (nBinsMatch and binWidthsMatch and namesPartialMatch):
         aoMap.setdefault(ao1,[]).append(ao2) 
 
  if (len(aoMap.keys()) == len(aoList1)) and ([len(aoMap[ao]) for ao in aoList1])==([1 for ao in aoList1]):
    if(opts.verbosity>0) : print("[INFO] found 1-1 mapping between aoLists using strategy %d :" % strategy)
    for ao1 in aoList1:
        aoMap[ao1] = aoMap[ao1][0]
        if(opts.verbosity>1) : print( "%s --> %s" %( ao1.name() , aoMap[ao1].name() )) 
  elif (recursive and strategy<3) :
    if(opts.verbosity>0) :
      print("[WARNING] Could not establish  1-1 mapping between aoLists using strategy", strategy, ", try strategy ", strategy+1)
    for ao1 in aoList1:
        if(opts.verbosity>1) : print( "%s --> %s" %( ao1.name() , aoMap[ao1] ) )
    strategy+=1
    aoMap=matchAOs(aoList1,aoList2,strategy)
  else :
    print("[ERROR] could not match AOS in the Data and MC files. Please make sure the AOs have the same binning/ number of bins (and name if possible!)")
    print("[ERROR] aos from list 1", aoList1)
    print("[ERROR] aos from list 2", aoList2)
    exit(1)
   
  return aoMap


def markupYODAwithCorrelationsFromYODAs(reference, correlationsDir):
  # produce a new YODA file where the AOs have been marked up with 
  # covariance info from an uncertainty breakdown provided as
  # a separate YODA file for each systematic variation
  reference_out = reference.replace(".yoda","_corrs.yoda")
  reference_outf = open(reference_out,'w')
  infiles={}
  if correlationsDir=='self':
    infiles.setdefault(reference,[])
  else:
    for f in os.listdir(correlationsDir):
      if "all" in f: continue
      infiles.setdefault(correlationsDir+"/"+f,[])
  nBins=-1
  binLabels={}
  binValues={}
  binErrors={}
  
  histos = [ name for  name in yoda.read(reference, unpatterns=".*RAW.*")]
  for histname in  histos:
    hAll = yoda.read(reference, unpatterns=".*RAW.*")[histname]
    if type(hAll) ==yoda.core.Histo1D: hAll=hAll.mkScatter()
    for f in infiles.keys():
      systname=f.split("/")[-1].replace(".yoda","")
      h = yoda.read(f, unpatterns=".*RAW.*")[histname]
      if type(h) ==yoda.core.Scatter2D : 
        nBins = h.numPoints()
        for ipt in range(nBins):
           binValues[ipt]= h.points()[ipt].y()
           binLabels[ipt]= "%f - %f"% (h.points()[ipt].xMin(),h.points()[ipt].xMax())
           errs=  h.points()[ipt].yErrs()
           errAv = (abs(errs[1]) + abs(errs[0]))*0.5
           binErrors.setdefault(ipt,{}).setdefault(systname,{})['up']=errs[1]
           binErrors.setdefault(ipt,{}).setdefault(systname,{})['dn']=-1*errs[0]
      elif type(h) ==yoda.core.Histo1D:
        nBins = h.numBins
        for ipt in range(nBins):
           binValues[ipt]= h.bins()[ipt].sumW()
           binLabels[ipt]= "%f - %f"% (h.bins()[ipt].xMin(),h.bins()[ipt].xMax())
           errAv=  h.bins()[ipt].sumW2()
           binErrors.setdefault(ipt,{}).setdefault(systname,{})['up']=errAv
           binErrors.setdefault(ipt,{}).setdefault(systname,{})['dn']=-1*errAv
      else: continue
    nearestMatch=""
    if (reference is not ""):
      smallestDiff=999.
      for f in reference.split(","):
        hists = yoda.read(f, unpatterns=".*RAW.*")
        for name in hists:
            if not type(hists[name]) ==yoda.core.Scatter2D : continue
            nominal = [ (hists[name].points()[i].y() ) for i in range(hists[name].numPoints()) ]
            if (len(nominal) != nBins): continue
            totalDiff=0
            for i in range(nBins):
              totalDiff+= abs(binValues[i]-nominal[i])
            if (totalDiff < smallestDiff):
              nearestMatch=f+"/"+name
              smallestDiff=totalDiff
            #print("candidate",  nominal, totalDiff," nearest match is ", nearestMatch)
      #print("[INFO] mapping",  histname," ---> ", nearestMatch)
    
    dirName=nearestMatch.split('.yoda')[0]
    plotName=nearestMatch.split('/')[-1]
    
    #binErrors={..} # a python dictionary 
    corrs = yaml.dump(binErrors, default_flow_style=True, default_style='',width=1e6)
    hAll.setAnnotation("ErrorBreakdown",corrs)
    yoda.writeYODA(hAll, reference_outf)
  reference_outf.close()
  return reference_out

def markupYODAwithCorrelationsFromYAML(reference, correlationsDir):
  # produce a new YODA file where the AOs have been marked up with 
  # covariance info from an uncertainty breakdown provided as
  # one or more YAML files from HEPData 
  reference_out = reference.replace(".yoda","_corrs.yoda")
  reference_outf = open(reference_out,'w')
  infiles={}
  for f in os.listdir(correlationsDir):
    if 'submission' in f: continue
    if 'comb' in f: continue
    if "syst" in f:
      infiles.setdefault(correlationsDir+"/"+f.replace('syst','xsect'),[])
    else : #experimental
      infiles.setdefault(correlationsDir+"/"+f,[])

  for f in infiles.keys():
    stream = open(f, "r")
    doc = yaml.load(stream)
    binLabels={}
    binValues={}
    binErrors={}
    for itemtype,itemarray in doc.items():
      if itemtype=="independent_variables": #bin labels
        for line in itemarray:
          binLabels['name']=itemarray[0]['header']['name']
          counter=0
          for entry in itemarray[0]['values']:
            if ('low' in entry.keys() and 'high' in entry.keys()):
              binLabels[counter]="%f - %f"%(entry['low'],entry['high'])
            else:
              binLabels[counter]="%f"%(entry['value'])
            counter+=1

      if itemtype=="dependent_variables": #values
        for line in itemarray:
          units=""
          if 'units' in  line['header'].keys(): units=line['header']['units']
          binValues['name']="%s [%s]"%(line['header']['name'],'')
          binErrors['name']="uncertainties"
        counter=0
        for entry in line['values']:
           binValues[counter]= entry['value']
           binErrors[counter]={}
           if 'errors' in entry.keys():
             for error in entry['errors'] :  
               binErrors[counter][error['label']]={}
               if 'symerror' in error.keys():
                 if not isfloat(error['symerror']) and '%' in (error['symerror']):
                   binErrors[counter][error['label']]['up']=binValues[counter]*0.01*float(error['symerror'].replace('%',''))
                   binErrors[counter][error['label']]['dn']=binValues[counter]*-0.01*float(error['symerror'].replace('%',''))
                 else :
                   binErrors[counter][error['label']]['up']=float(error['symerror'])
                   binErrors[counter][error['label']]['dn']=-1*float(error['symerror'])
               elif 'asymerror' in error.keys():
                 if not isfloat(error['asymerror']['plus']) and '%' in error['asymerror']['plus']:
                   binErrors[counter][error['label']]['up']=binValues[counter]*0.01*float(error['asymerror']['plus'].replace('%',''))
                   binErrors[counter][error['label']]['dn']=binValues[counter]*0.01*float(error['asymerror']['minus'].replace('%',''))
                 else :
                   binErrors[counter][error['label']]['up']=float(error['asymerror']['plus'])
                   binErrors[counter][error['label']]['dn']=1*float(error['asymerror']['minus'])
               else:
                 print('[ERROR] errors are neither symmetric or asymmetric... exit!')
                 exit(1)
           counter+=1
    
    if 'xsect' in f:
      systNames=binErrors[0].keys()
      binErrorsGranular={}
      systFile=f.replace("xsect","syst")
      streamsyst = open(systFile, "r")
      docsyst = yaml.load(streamsyst)
      binErrorsGranular['name']="uncertainties"
      for itemtype,itemarray in docsyst.items():
        #print()
        #print(itemtype,itemarray)
        if  not "variables" in itemtype: continue 
        if (itemtype == "independent_variables"):
          systNames=[v['value'] for v in itemarray[0]['values']]
        else:
          binCounter=-1
          for line in itemarray:
            binCounter+=1
            binErrorsGranular[binCounter]={}
            #print(line)
            if not 'values' in line.keys(): continue
            systCounter=-1
            for error in line['values']:
              systCounter+=1
              binErrorsGranular[binCounter][systNames[systCounter]]={}
              binErrorsGranular[binCounter][systNames[systCounter]]['up']=float(error['value'])*binValues[binCounter]*0.01
              binErrorsGranular[binCounter][systNames[systCounter]]['dn']=-1*float(error['value'])*binValues[binCounter]*0.01
      for i in range(len(binErrorsGranular.keys())-1):
        binErrorsGranular[i]['stat']={}
        binErrorsGranular[i]['stat']['up']=binErrors[i]['stat']['up']
        binErrorsGranular[i]['stat']['dn']=binErrors[i]['stat']['dn']
      binErrors=binErrorsGranular
    nBins=len(binLabels)-1
    infiles[f]=[binValues,binErrors]
  
  
  hists = yoda.read(reference, unpatterns=".*RAW.*")
  for name in hists:
    #print("[INFO] Looking for systematics breakdown matching %s " % (name))
    nearestMatch=""
    smallestDiff=999.
    nearestMatchErrors=""
    nominal = [ (hists[name].points()[i].y ) for i in range(hists[name].numPoints()) ]
    for f,corrs in infiles.items():
      binValues=corrs[0]
      binErrors=corrs[1]
      nBins=len(binValues)-1
      if (len(nominal) != nBins): continue
      totalDiff=0
      for i in range(nBins):
        totalDiff+= abs(binValues[i]-nominal[i])
      if (totalDiff < smallestDiff):
        nearestMatch=f
        smallestDiff=totalDiff
        nearestMatchErrors=binErrors
    if (opts.verbosity>1) : print("[DEBUG] candidate",  nominal, " nearest match is ", nearestMatch)
    if smallestDiff < 1.0:
      if (opts.verbosity>1) : print("[INFO] mapping",  name," ---> ", nearestMatch, (smallestDiff))
      corrs = yaml.dump(nearestMatchErrors, default_flow_style=True, default_style='',width=1e6)
      hists[name].setAnnotation("ErrorBreakdown",corrs)
      yoda.writeYODA(hists[name], reference_outf)
    else:
      print("[WARNING] Warning, no match found for",  name)
  print("[INFO] %s has been marked up with correlation info from %s in this file:" % (reference , correlationsDir))
  print("[INFO] ---> %s " % (reference_out))
  print("[INFO] next time, you can use this file as an input instead of ", reference)
  reference_outf.close()
  return reference_out

def markupYODAwithCorrelations (ref, corrDir,ignoreCorrelations):
  # produce a new YODA file where the AOs have been marked up with 
  # covariance info from an uncertainty breakdown provided as
  # either YAML or YODA files  
  isYoda=False
  if ignoreCorrelations: 
     return markupYODAwithCorrelationsFromYODAs(ref,'self') 
  else:
    if (corrDir==None) : return ref 
    for f in os.listdir(corrDir):
      if '.yoda' in f: isYoda=True
  if isYoda: 
     return markupYODAwithCorrelationsFromYODAs(ref, corrDir) 
  else:
     return markupYODAwithCorrelationsFromYAML(ref, corrDir) 

def returnFileContents(filename,inputType='data'):
    result =[]
    aos = yoda.read(filename, unpatterns=".*RAW.*")
    for aopath, ao in aos.items():
      aoType = type(ao)
      if  aoType in [yoda.Histo1D, yoda.Histo2D]: ao = ao.mkScatter() 
      if  aoType in [yoda.Counter] : continue
      if not 'all' in opts.filter and inputType=='data':
        for f in opts.filter :
          if f in aopath:
            result.append(ao)
      else:
        result.append(ao)
        
    covInfoMissing=[]
    for ao in result:
      corr=ao.annotation("ErrorBreakdown")
      if (corr==None) :
        covInfoMissing.append(ao)
    if (len(covInfoMissing) and inputType=='data' and not opts.ignore_corrs and not opts.corr_data  ):
      print("[WARNING] These DATA analysis objects are missing the correlation/covariance information:", covInfoMissing)
      print("  --> To add correlation information, you can use the eg options: --get_corr_from_web, --corr_data ")
      print("  --> You can also proceed by adding the --ignore_corrs_data in which case the chi2 is calculated  ")
      print("  --> from the total uncertainties and assumed to be bin-bin uncorrelated.")
      inspireID=[t for t in opts.data.split("_") if "I" in t]
      inspireID=inspireID[0].replace("I","")
      if os.path().isdir("corrInfo/ins%s" % inspireID):
        print("[INFO] Attempting to use the information in corrInfo/ins%s to build correlation info " % inspireID)
        opts.corr_data="corrInfo/ins%s" % inspireID
      else:
        print("[INFO] If the correlations are missing from data, would you like to try to download correlation information from HEPData? yes/no")
        response  = raw_input()
        if response not in ['yes','y']:
          print("[INFO] Exiting.")
          exit(1)
        else:
          getCorrelationInfoFromWeb()  
    
    if (len(covInfoMissing) and inputType=='mc' and not opts.ignore_corrs and not opts.corr_mc ):
      print("[WARNING] These MC analysis objects are missing the correlation/covariance information:", covInfoMissing)
      print("  --> To add correlation information, you can use the eg options: --corr_mc ")
      print("  --> You can also proceed by adding the --ignore_corrs_mc in which case the chi2 is calculated  ")
      print("  --> from the total uncertainties and assumed to be bin-bin uncorrelated.")
      if os.path.isdir("corrInfoMC/%s" % filename.replace(".yoda","")):
        print("[INFO] Attempting to use the information in corrInfoMC/%s to build correlation info " %  filename.replace(".yoda",""))
        opts.corr_mc="corrInfoMC/%s" % filename.replace(".yoda","")
    return result

def getCorrelationInfoFromWeb(name=None,dlformat="yoda"):
  # try to get the YAML files for the corresponding HEPData record
  # download and untar them...
  if (name==None) : name=opts.data
  #dlformat='yaml'
  inspireID=[t for t in name.split("_") if "I" in t]
  inspireID=inspireID[0].replace("I","")
  hdurl = "http://www.hepdata.net/record/ins%s?format=%s" % (inspireID, dlformat)
  print("[INFO] Trying to download information from %s", hdurl)
  if hdurl:
    response = urllib2.urlopen(hdurl)
    download=response.read()
    if not download or "<html" in download:
        print(("Problem encountered when getting data from HepData (%s). No reference data file written." % hdurl))
    else:
        tar = tarfile.open(mode= "r:gz", fileobj = StringIO.StringIO(download))
        fnames = tar.getnames()
        if len(fnames)>0 and dlformat=='yaml':
            tar.extractall()
            os.system("mkdir -p corrInfo/ins%s" % (inspireID)) 
            os.system ("mv %s/* corrInfo/ins%s/." % (fnames[0], inspireID))
            os.system ("rm -r %s" % (fnames[0]))
            print("[INFO] successfully downloaded YAML files from HEPMC, will see if they can be used for correlations")
        if len(fnames)==1 and dlformat=='yoda':
            tar.extractall()
            os.system("mkdir -p corrInfo/ins%s" % (inspireID)) 
            os.system ("mv %s corrInfo/ins%s/." % (fnames[0], inspireID))
            print("[INFO] successfully downloaded YODA files from HEPMC, will see if they can be used for correlations")
    opts.corr_data="corrInfo/ins%s" % inspireID
    response.close()

 
   

########################################################################

if __name__ == "__main__":

   opts.ntoys =int(opts.ntoys)
   opts.verbosity =int(opts.verbosity)
   opts.filter =opts.filter.split(',')
   if opts.ignore_corrs:
     opts.ignore_corrs_data=True
     opts.ignore_corrs_mc=True
   
   mcNames={}
   plotDictionary={'error-analysis':{}, 'covariance-matrix':{}, 'correlation-matrix':{}, 'data-vs-mc':{},'chi2-contribs':{}, 'chi2-value':{}, 'covDetails':{}, 'summary-plot':{}, 'summary-table':{}}
   # access the data and mc histograms
   if (opts.corr_data==None and opts.get_corr_from_web):
     getCorrelationInfoFromWeb(opts.get_corr_from_web)
   if opts.data == "" : 
     pass
   else:
     histograms['data']=returnFileContents(opts.data, 'data')
     if opts.corr_data is not None: 
       opts.data = markupYODAwithCorrelations(opts.data, opts.corr_data, opts.ignore_corrs_data) 
       #if there are correlations to add, do so and then get the aos from the new file
       histograms['data']=returnFileContents(opts.data, 'data')
     outdir="outputs/%s/data/plots" % (opts.data.replace(".yoda",""))
     for hdata in histograms['data']: 
       if not ct.hasValidErrorBreakdown(hdata): continue
       os.system("mkdir -p %s" % outdir )
       plotparser = rivet.mkStdPlotParser([],[])
       headers= plotparser.getHeaders(hdata.path())
       if 'XLabel' in headers.keys(): XLabel=headers['XLabel']
       else: XLabel=None
       if 'Title' in headers.keys(): Title=headers['Title']
       else: Title=None
       ct.plotVariations(hdata,outdir,'data-%s' % hdata.name(),0.05, xLabel=XLabel, title=Title)
       plotDictionary['error-analysis'].setdefault('data',{})
       plotDictionary['error-analysis']['data'][hdata.name()]= outdir+'/data-%s-variations.pdf' % hdata.name()
     print("[INFO] See plots of data error analysis here:", outdir )
     if  'data' in plotDictionary['error-analysis'].keys() :
       for k, v in plotDictionary['error-analysis']['data'].items():
         print(v)

   for mc in opts.mc.split(","):
     if mc == "" : continue
     if ":" in mc :
       mcName= mc.split(":")[1]
       mc= mc.split(":")[0]
     else :
       mcName = mc.split("/")[-1].replace(".yoda","")
     histograms['models'][mc]=returnFileContents(mc,'mc') 
     mcnew = markupYODAwithCorrelations(mc, opts.corr_mc, opts.ignore_corrs_mc) 
     opts.mc=opts.mc.replace(mc,mcnew)
     #if there are correlations to add, do so and then get the aos from the new file
     histograms['models'].pop(mc,None)
     histograms['models'][mcnew]=returnFileContents(mcnew,'mc') 
     if (opts.data!=''):
      aoMap = matchAOs(histograms['data'],histograms['models'][mcnew]) 
      newmc=renameAOs(mcnew,aoMap)
     else : newmc=mc
     opts.mc=opts.mc.replace(mc,newmc)
     histograms['models'].pop(mcnew,None)
     histograms['models'][newmc]=returnFileContents(newmc,'mc') 
     mcNames[newmc]=mcName
     for hmc in histograms['models'][newmc]: 
       outdir="outputs/%s/%s/plots" % (opts.data.replace(".yoda",""),mcName)
       os.system("mkdir -p %s" % outdir )
       if not (opts.ignore_errs_mc) : 
         ct.plotVariations(hmc,outdir,'%s-%s' % (mcName,hmc.name()),0.01)
         plotDictionary['error-analysis'].setdefault(mcName,{})
         plotDictionary['error-analysis'][mcName][hmc.name()]= outdir+'/%s-%s-variations.pdf' % (mcName,hmc.name())
     if not (opts.ignore_errs_mc) : 
       print("[INFO] See plots of %s variations here" % mcName , outdir+"/validation-plots/")
       for k, v in plotDictionary['error-analysis'][mcName].items():
         print(v)
     
   # make the super AOs 
   if(opts.make_super_aos):
     dataSuperAO=ct.makeSuperAO(histograms['data'])
     histograms['data'].append(dataSuperAO)
     yoda.writeYODA(dataSuperAO,opts.data.replace('.yoda','.super-ao.yoda' ))
     for mc in histograms['models'].keys():
       mcSuperAO=ct.makeSuperAO(histograms['models'][mc])
       histograms['models'][mc].append(mcSuperAO)
       yoda.writeYODA(mcSuperAO,mc.replace('.yoda','super-ao.yoda' ))
   
   # Check that both that and MC histograms are available before proceeding
   if (len(opts.data)==0):
     print("[INFO] No data histograms to be processed. Cannot make comparisons, so exiting.")
     exit(1)
   if (len(opts.mc)==0):
     print("[INFO] No MC histograms to be processed. Cannot make comparisons, so exiting.")
     exit(1)
   for hdata in sorted(histograms['data']):
       if not ct.hasValidErrorBreakdown(hdata): continue
       mcResults={}
       if not ("all" in opts.filter):
         passFilter=False
         for fil in opts.filter:
           if fil in hdata.name() : passFilter=True
         if not (passFilter or 'superAO' in hdata.name()): continue 
       for model in histograms['models']:
         aoMap = matchAOs(histograms['data'],histograms['models'][model])
         hmc=aoMap[hdata]
         mcName=mcNames[model]
         #if (opts.verbosity >1) :
         #print("processing %s:%s vs %s:%s" % (opts.data,hdata.path() ,model, hmc.path()))
         outdir="outputs/%s" % (opts.data.replace(".yoda",""))
         os.system("mkdir -p %s/data-vs-%s/plots" % (outdir, mcName) )
         #os.system("mkdir -p %s/tables" % outdir )
         #outfile= open("%s/tables/summary.txt"%outdir, 'w')
         
         #ct.plotVariations(hdata,outdir+"/validation-plots/",'data')
         #if not (opts.ignore_errs_mc) : ct.plotVariations(hmc,outdir+"/validation-plots/",'mc',0.01)

         # produce covariance matrix for data
         #covDetailsData="Data Covariance Matrix"
         covDetailsData="Size of uncertainties across range, and Data Correlation Matrix"
         if ((opts.use_toys or ct.hasAsymmetricErrors(hdata)) and not opts.use_direct): 
           print("[WARNING] Data has asymmetric errors, producing covariance matrix from %d toys" % opts.ntoys)
           covData = ct.makeCovarianceMatrixFromToys(hdata, opts.ntoys ,opts.ignore_corrs_data)
           covDetailsData += " produced from %d toys%s" % (opts.ntoys , ", ignoring correlations" if opts.ignore_corrs_data else "")
         else:
           covData = ct.makeCovarianceMatrix(hdata, opts.ignore_corrs_data) 
           covDetailsData += " produced from direct propagation%s" % ( ", ignoring correlations" if opts.ignore_corrs_data else "")
         #outfile.write("%s \n" % covDetails)
         #ct.printMatrix(covData, outfile)
         plotparser = rivet.mkStdPlotParser([],[])
         headers= plotparser.getHeaders(hdata.path())
         if 'XLabel' in headers.keys(): XLabel=headers['XLabel']
         else: XLabel=None
         ct.drawMatrix(covData,outdir+"/data/plots/data-%s-covariance-matrix" % hdata.name(), xLabel=XLabel)
         plotDictionary['covariance-matrix'].setdefault('data',{})
         plotDictionary['covariance-matrix']['data'][hdata.name()]= outdir+"/data/plots/data-%s-covariance-matrix.pdf" % hdata.name()
         ct.drawMatrix(ct.correlationMatrix(covData),outdir+"/data/plots/data-%s-correlation-matrix" % hdata.name())
         plotDictionary['correlation-matrix'].setdefault('data',{})
         plotDictionary['correlation-matrix']['data'][hdata.name()]= outdir+"/data/plots/data-%s-correlation-matrix.pdf" % hdata.name()
         #outfile.write("\n")
         
         # produce covariance matrix for MC
         covDetailsMC="MC Covariance Matrix"
         if ((opts.use_toys or ct.hasAsymmetricErrors(hmc)) and not opts.use_direct): 
           print("[WARNING] MC has asymmetric errors, producing covariance matrix from %d toys" % opts.ntoys)
           covMC = ct.makeCovarianceMatrixFromToys(hmc, opts.ntoys, opts.ignore_corrs_mc)
           covDetailsMC += " produced from %d toys%s" % (opts.ntoys , ", ignoring correlations" if opts.ignore_corrs_mc else "")
         else:
           covMC = ct.makeCovarianceMatrix(hmc,  opts.ignore_corrs_mc)
           covDetailsMC += " produced from direct propagation%s" % ( ", ignoring correlations" if opts.ignore_corrs_mc else "")
         #outfile.write("%s \n" % covDetails)
         #ct.printMatrix(covMC, outfile)
         ct.drawMatrix(covMC,outdir+"/%s/plots/%s-%s-covariance-matrix" % (mcName, mcName, hdata.name()))
         plotDictionary['covariance-matrix'].setdefault(mcName,{})
         plotDictionary['covariance-matrix'][mcName][hmc.name()]= outdir+"/%s/plots/%s-%s-covariance-matrix.pdf" % (mcName, mcName, hmc.name())
         ct.drawMatrix(ct.correlationMatrix(covMC),outdir+"/%s/plots/%s-%s-correlation-matrix" % (mcName, mcName, hmc.name()))
         plotDictionary['correlation-matrix'].setdefault(mcName,{})
         plotDictionary['correlation-matrix'][mcName][hmc.name()]= outdir+"/%s/plots/%s-%s-correlation-matrix.pdf" % (mcName, mcName,hmc.name())
         #outfile.write("\n")
         
         # sum covariance matrices
         covTotal=covData + covMC
         if opts.ignore_errs_data and opts.ignore_errs_mc :
           print("[ERROR] cannot ignore both data errros and MC errors at the same time!")
           exit(1)
         elif opts.ignore_errs_data :
           covTotal = covMC
           covDetailsData="Ignoring Data uncertainties (MC cov matrix only)"
         elif opts.ignore_errs_mc :
           covTotal = covData
           covDetailsMC="Ignoring MC uncertainties (Data cov matrix only)"
           covDetailsData += ". Ignoring MC uncertainties"
         #outfile.write(" Total Covariance Matrix %s \n" % covDetails)
         #ct.printMatrix(covTotal, outfile)
         plotDictionary['covDetails'].setdefault("%s" % mcName,{})
         plotDictionary['covDetails'].setdefault("data" ,{})
         plotDictionary['covDetails']["data"][hdata.name()]= covDetailsData 
         plotDictionary['covDetails'][mcName][hmc.name()]= covDetailsMC
         
         ct.drawMatrix(covMC,outdir+"/data-vs-%s/plots/data-vs-%s-%s-covariance-matrix" % (mcName, mcName, hdata.name()))
         plotDictionary['covariance-matrix'].setdefault("data-vs-%s" % mcName,{})
         plotDictionary['covariance-matrix']["data-vs-%s" % mcName][hmc.name()]= outdir+"/data-vs-%s/plots/data-vs-%s-%s-covariance-matrix.pdf" % (mcName, mcName, hdata.name())
         ct.drawMatrix(ct.correlationMatrix(covMC),outdir+"/data-vs-%s/plots/data-vs-%s-%s-correlation-matrix" % (mcName, mcName, hdata.name()))
         plotDictionary['correlation-matrix'].setdefault("data-vs-%s" % mcName,{})
         plotDictionary['correlation-matrix']["data-vs-%s" % mcName][hmc.name()]= outdir+"/data-vs-%s/plots/data-vs-%s-%s-correlation-matrix.pdf" % (mcName, mcName, hdata.name())
         #outfile.write("\n")
         
         #outfile.write("Total Correlation Matrix %s  \n" % covDetails )
         #ct.printMatrix(ct.correlationMatrix(covTotal),outfile)
         #outfile.write("\n")
    
         chi2,ndf,prob,chi2contribs=ct.calculateChi2(hdata,hmc, covTotal)
         #outfile.write("Chi2 Contributions Matrix %s  \n" % covDetails )
         #ct.printMatrix(chi2contribs,outfile)
         ct.drawMatrix(chi2contribs,outdir+"/data-vs-%s/plots/data-vs-%s-%s-chi2-contribs" % (mcName, mcName, hdata.name()))
         plotDictionary['chi2-contribs'].setdefault("data-vs-%s" % mcName,{})
         plotDictionary['chi2-contribs']["data-vs-%s" % mcName][hmc.name()]= outdir+"/data-vs-%s/plots/data-vs-%s-%s-chi2-contribs.pdf" % (mcName, mcName, hdata.name())
         
         chi2ContribsByRow=ct.chi2ContribsByRow(chi2contribs)
         chi2ContribsByRowYAML= yaml.dump(chi2ContribsByRow, default_flow_style=True, default_style='',width=1e6)
         hmc.setAnnotation("Chi2Contribs",chi2ContribsByRowYAML)
         ct.updateAOinYODA(hmc,model) 
         plotDictionary['chi2-value'].setdefault("data-vs-%s" % mcName,{})
         plotDictionary['chi2-value']["data-vs-%s" % mcName][hmc.name()]= "$%.2f / %d$" % ( chi2,ndf)
         #uoutfile.write("\n")
         
         #outfile.write("\n")
         #outfile.write("Total Chi2=%.3f/%d \n"%(chi2,ndf))
         
         mcResults[model]=[chi2,ndf]
         print("[INFO] %s:%s vs %s:%s --> chi2=%.2f/%d  (prob=%.2f) "%(opts.data,hdata.name(),model,hmc.name(),chi2,ndf,prob))


         if "superAO" in hdata.name(): continue
         #print(("rivet-cmphistos  %s:Title='%s ($\chi^2=%.2f/%d$)' -m .*%s -o %s "%(model,mcName,chi2,ndf,hmc.name,outdir+"/data-vs-%s/plots/" % mcName)))
         #os.system("rivet-cmphistos  %s:Title='%s ($\chi^2=%.2f/%d$)' -m .*%s -o %s "%(model,mcName,chi2,ndf,hmc.name,outdir+"/data-vs-%s/plots/" % mcName))
         #os.system("local/bin/make-plots-lcompare %s/.%s.dat -q "%(outdir+"/data-vs-%s/plots/" % mcName,hmc.path().replace("/%s" %hmc.name,"_%s" %hmc.name)))
         #os.system("make-plots %s/.%s.dat -q "%(outdir+"/data-vs-%s/plots/" % mcName,hmc.path().replace("/%s" %hmc.name,"_%s" %hmc.name)))
         res={ '%s'%opts.data : '[Data]', '%s'%model : '[%s (#chi^2=%.2f/%d)]'%(mcName, chi2,ndf) }
         outdirplots=outdir+"/data-vs-%s/plots/" % mcName
         plots=st.makeSystematicsPlotsWithROOT(res, outdirplots, nominalName='Data', ratioZoom=None, regexFilter=".*%s.*"%hmc.name(), regexVeto=None)
         
         plotDictionary['data-vs-mc'].setdefault("data-vs-%s" % mcName,{})
         #plotDictionary['data-vs-mc']["data-vs-%s" % mcName][hmc.name]= "%s/.%s.pdf"%(outdir+"/data-vs-%s/plots/" % mcName,hmc.path().replace("/%s" %hmc.name,"_%s" %hmc.name))
         plotDictionary['data-vs-mc']["data-vs-%s" % mcName][hmc.name()]=plots[0] 


       
       #commandLine_cmphistos='rivet-cmphistos  '
       #commandLine_makeplots='make-plots '
       #commandLine_makeplots='local/bin/make-plots-lcompare '
       outdir="outputs/%s/summary" % (opts.data.replace(".yoda",""))
       pathName=hdata.path().replace("/REF","")
       #commandLine_makeplots+="%s/.%s.dat "%(outdir, pathName.replace("/%s" % hdata.name,"_%s" % hdata.name))
       res={ '%s'%opts.data : '[Data]'}
       for model in histograms['models']:
         mcName=mcNames[model]
         mcR=mcResults[model]
         res[model]='[%s (#chi^2=%.2f/%d)]'%(mcName, mcR[0],mcR[1])
         if  "superAO" in hdata.name(): continue
       
       
       if  "superAO" in hdata.name(): continue
       plots=st.makeSystematicsPlotsWithROOT(res, outdir, nominalName='Data', ratioZoom=None, regexFilter=".*%s.*"%hmc.name(), regexVeto=None)
       #os.system('mkdir -p %s' % outdir)
       #commandLine_cmphistos+="-o %s" %outdir
       #commandLine_makeplots+="-q"
       #print(commandLine_cmphistos)
       #print(commandLine_makeplots)
       #os.system(commandLine_cmphistos)
       #os.system(commandLine_makeplots)
       #plotDictionary['summary-plot'][hmc.name()]= "%s/.%s.pdf"%(outdir, pathName.replace("/%s" % hdata.name(),"_%s" % hdata.name()))
       plotDictionary['summary-plot'][hmc.name()]= plots[0] 
       makeSummaryTable(plotDictionary,outdir+"/summary_%s.tex" % hdata.name() ,hdata.name() ) 
       plotDictionary['summary-table'][hmc.name()]= outdir+"/summary_%s.tex" % hdata.name() 
     
     
   print("[INFO] output rivet files with Chi2: %s" % outdir)
   os.system('ls %s/* | grep pdf' % outdir )
   if opts.slides:
     #having looped through all data: 
     outdir="outputs/%s/summary" % (opts.data.replace(".yoda",""))
     if len(plotDictionary['chi2-value'].keys())==0:
       print("[WARNING] no histograms were processed, could not make summary slides! Exit")
       exit(1)
     else:
       makeSummaryTable(plotDictionary,outdir+"/summary_all.tex")  
       plotDictionary['summary-table']['all']= outdir+"/summary_all.tex"
       beamerPath = makeBeamer(plotDictionary, outdir )
       os.system('rm slides_*' )
       os.system('pdflatex %s > /dev/null' % beamerPath)
       os.system('pdflatex %s > /dev/null' % beamerPath)
       beamerPath=beamerPath.replace("tex","pdf").split("/")[-1]
       print("[INFO] your summary slides are here: \n", beamerPath)
     #os.system('cp %s ~/www/UCL/QualTask/sandbox/summarySlides/%s '  % (beamerPath, beamerPath))
     #print("https://lcorpe.web.cern.ch/lcorpe/UCL/QualTask/sandbox/summarySlides/%s" %  beamerPath)

