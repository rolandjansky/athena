#!/usr/bin/env python
"""
This module contains helper functions which allow users to manipluate multiple 
YODA files corresponding to different subsamples of a given process/generator,
different sources of theory uncertainty, etc. and combine them, plotting the 
theory error bands along the way. 

Can be used either as a standalone exectuable or the functions can be imported
intot a custom python script.

This module uses a loosely-defined datatype which shall be referred to as 
AnalysisObject, which is effectively a dict of np.arrays and strings.
This encodes the information from a yoda.Scatter2D(), yoda.Histo1D(),
root.TGraphAsymmErrors() or root.TH1D() in the same format, and which
can be easily manipulated since it consists of np.arrays or strings
An AO has the following keys:

`ao['name']`   the name of the object a la YODA   
`ao['path']`   the path of the object a la YODA  
`ao['annotations']`  a dict of annotations a la YODA  
`ao['bw']`     np.array of widths of each bin  
`ao['x']`      np.array of mid-points of bins  
`ao['xup']`    np.array of high bin edges  
`ao['xdn']`    np.array of low bin edges  
`ao['y']`      np.array of y values  
`ao['yup']`    np.array of y+err_up values  
`ao['ydn']`    np.array of y-err_dn values  
`ao['nbins']`  number of bins  

This should probably be replaced by a Class in the next version !

For a full list of functions and their usage, type:
pydoc systematicsTool

TODO (perspectives for future development):
- Also handle ROOT as input/output instead of YODA
- Replace AnalysisObject by an actual Class rather than loosely-defined dicts
- Implement combineVariationsReplicas()
- Support other types in readFromYODA


Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch  
"""
from random import randint
import tarfile, StringIO
try:
  import yoda
  import yaml
  import lhapdf
  import numpy as np
except:
  print("[ERROR], looks like the YODA, YAML, LHAPDF or NUMPy packages are not installed. Please run this command and try again:")
  print("source setupPMGSystematicsTool.sh")
  exit(1)
import cPickle as pickle
import os, sys, re
import optparse
import readDatabase as rDB
import copy, time
from multiprocessing.dummy import Pool as ThreadPool
import multiprocessing.dummy 
import ROOT as r
from array import array
from collections import OrderedDict


def renameFilesWithoutPrefix(directory):
  """
  `directory` String  
  `return` None  
  
  Renames the files in a given directory, such that the longest common prefix
  which occurs in all filenames is ommitted. Useful if your GRID jobs were
  submitted with the format <prefix>.<weights_names>.yoda and you want to 
  lose the <prefix>. bit !
  """

  fileList=[]
  for fn in os.listdir(directory):
    fileList+=[fn]
  
  if len(fileList)==1: return
  
  prefix=os.path.commonprefix(fileList)
  for fn in os.listdir(directory):
    os.system("mv %s/%s %s/%s" % (directory,fn, directory, fn.replace(prefix,"")))
  

def safeFileName(name,  removeExtension=True, reverse=False):
  """
  `name` String  
  `removeExtension` Bool [optional] (remove the file extension, eg .root, or .yoda)  
  `return` String  
  
  Converts an input string (eg a weight name) which may contain non-standard
  characters into a string which is safe to use as a filename.
  In particular, the following substitutions are made:  
  '.' --> 'p'  
  ' ' --> '_'  
  ':' --> '_'  
  """
  name=name.strip()
  #name= name.replace(".","p")
  for decimalPoint in re.findall("[0-9].[0-9]",name):
    name=name.replace(decimalPoint,decimalPoint.replace(".","p"))
  name= name.replace("pyoda",".yoda")
  name= name.replace("proot",".root")
  name= name.replace(" ","_")
  name= name.replace(":","_")
  name= name.replace("+","")
  name= name.replace("userp","user.")
  name= name.replace("/","over")
  if (removeExtension and "." in name): name=name.rpartition(".")[0]
  return name

def rivet_i_replacements(name):
  reps=OrderedDict()
  reps[" nominal "]=""
  reps[" set = "]="_"
  reps[" = "]="_"
  reps["="]=""
  reps[","]=""
  reps["."]=""
  reps[":"]=""
  reps[" "]="_"
  reps["#"]="num"
  reps["\n"]="_"
  reps["/"]="over"
  for find,rep in reps.items():
    name= name.replace(find,rep)
  return name


def lookupPDFSetName(lhapdfid):
  """
  `lhapdfid` Int  
  `return` String  
  
  Takes a LHAPDF ID code and figures out what is the name of the PDF Set
  which it belons to.
  """
  lhapdfid=int(lhapdfid)
  for a in lhapdf.availablePDFSets():
    b = lhapdf.getPDFSet(a)
    if (lhapdfid>= b.lhapdfID) and (lhapdfid< (b.lhapdfID+b.size)):
     return a
  # return empty sting if nothing is found
  return ""

def getSumOfWeights(path,nominalWeight="", sumwHistName=""):
  if 'root' in path:
    f=  r.TFile.Open(path)
    for i in f.GetListOfKeys():
      if sumwHistName in i.GetName():
         return f.Get(i.GetName()).Integral()
  else:
    aos=yoda.read(path, patterns='.*/_EVTCOUNT.*') 
    if not '/_EVTCOUNT' in aos.keys():
      return 1.
    elif  '/_EVTCOUNT[' in aos.keys():
      return  aos['/_EVTCOUNT[%s]'%nominalWeight].sumW()
    else:
      return  aos['/_EVTCOUNT'].sumW()
    

def weightCorrection(var, nom,sampleDir="", varWeightName="", nominalWeightName="", sumwHistName =""):
  """
  `var` String (name of YODA file for variation to get the correction for)  
  `nom` String (name of YODA file for nominal to get correct away from)  
  `sampleDir` String [optional] (directory path for a given subsample)  
  `varWeightName` String [optional] (name of the variation to get weight correction for)  
  `nominalWeightName` String [optional] (name of the nominal variation to get weight correction with respect to)  
  `sumwHistName` String [optional] (name of the sum-of-weights TH1 in the case of root files)  
  `return` Float  
  
  Computes the on-the-fly weight correction of the systematic variation with
  respect to the nominal. This is needed for YODA files because by default,
  Rivet normalises by the sum of weights for a given instance (ie variation)
  rather than by the nominal. This weight is used to fix the issue. 
  """
  w_var=1.
  w_nom=1.
  #Construct the filenames for variation and nominal
  #fn_var = '%s/%s.yoda' % (sampleDir,safeFileName(var))
  #fn_nom = '%s/%s.yoda' % (sampleDir,safeFileName(nom))
  fn={}
  fn['var']= '%s' % (var)
  fn['nom']= '%s' % (nom)
  for k,v in fn.items():
    #in case .yoda gets duplicated.
    #if '.yoda.yoda' in fn[k]: 
    #  fn[k]=v.replace('.yoda.yoda','.yoda')
    #if '.root.root' in fn[k]: 
    #  fn[k]=v.replace('.root.root','.root')
    #if '.root.yoda' in fn[k]: 
    #  fn[k]=v.replace('.root.yoda','.yoda')
    if not os.path.isfile(fn[k]):
      fn[k]=fn[k]+'.yoda'
    if not os.path.isfile(fn[k]):
      fn[k]=fn[k].replace('.yoda','.root')
    if not os.path.isfile(fn[k]):
      print("\n[ERROR] Could not open the file :", fn[k])
      print("[ERROR] Available files in ", sampleDir)
      for varFile in os.listdir(sampleDir):
        print(varFile)
      exit(1)
  # get the sum of weights for nom and var, using _EVTCOUNT
  w_var=getSumOfWeights(fn['var'],varWeightName,sumwHistName)
  w_nom=getSumOfWeights(fn['nom'],nominalWeightName, sumwHistName)
  return w_var / w_nom


def getXSFromYODA(path):
  """
  `path` String (path to YODA file)
  `return` Float
  """
  if not '/_XSEC' in yoda.read(path, patterns='.*/_XSEC.*').keys():
    return 1.
  else:
    return yoda.read(path, patterns='.*/_XSEC.*')['/_XSEC'].points()[0].x()


def getXS(dsid, campaign=15, userFile=None):
  """
  `dsid` Int (dataset ID of the ATLAS dataset you want to get the
  cross-section for  
  `campaign` String [optional] (the MC campaign number 15 for mc15_13TeV  
  datasets, 16 for mc16_13TeV. If campaign > 16 look instead for a local  
  file `data/PMGxsecDB_manual.txt` where the user can input the XS info)  
  `return` Float  
  
  Fetch the cross-section (XS) of the dataset whose ID you specified.
  This function checks for the DSID in the central PMG XS database on cvmfs
  but by setting campaign=999 you can force it to check in a local file:
  PMGxsecDB_manual.txt where the user can input the XS info manually.
  """
  if userFile is not None:
    campaign=14
  # this triggers a search in the manual, local XS file
  if campaign>16 or campaign < 15:
    dataDir=os.environ["SYSTTOOLSPATH"]+"/data/PMGSystematicsTools"
    if userFile is None:
      pmgXSFile="%s/PMGxsecDB_manual.txt" %dataDir
    else:
      pmgXSFile=userFile
  else:
  # this triggers a search in centeal XS files on cvmfs 
    pmgXSFile="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PMGTools/PMGxsecDB_mc%d.txt"%campaign
  
  # these are defined keep track of whether the desired DSID was found in the 
  # requestd XS file
  foundDataset=0
  thisSampleFinalCrossSection=0.
  thisSampleKFactorUncertainty=0.
  hasKFactor=0
  thisSampleKFactor=0

  # use a TTree to read the file, faster/more efficient than a python loop.
  pmgXSTree=r.TTree()
  pmgXSTree.ReadFile(pmgXSFile)
  for dsidLine in pmgXSTree:
    if int(dsid) == int(dsidLine.dataset_number): 
      thisSampleCrossSection=dsidLine.crossSection
      thisSampleFilterEff=dsidLine.genFiltEff
      thisSampleKFactor=dsidLine.kFactor
      try:
        thisSampleKFactorUncertainty=(dsidLine.relUncertUP+dsidLine.relUncertDOWN)*0.5
      except:
        thisSampleFinalCrossSection=0
      thisSampleFinalCrossSection= thisSampleCrossSection*thisSampleFilterEff*thisSampleKFactor
      foundDataset=1
      if dsidLine.kFactor != 1: hasKFactor=1
      break
  # if we can't fine the desired DSID, recursively try elsewhere before failing
  if not foundDataset:  
    
    print("[WARNING] could not find ", dsid, " in ", pmgXSFile, " for campaign", campaign)
    if campaign>16:
      print("[ERROR] could not find ", dsid, " in ", pmgXSFile)
      print("--> you'll need to add it in manually before continuing")
      return -1,-1
    else : thisSampleFinalCrossSection,hasKFactor,thisSampleKFactorUncertainty = getXS(dsid, campaign+1,userFile)
  
  return thisSampleFinalCrossSection,thisSampleKFactor,thisSampleKFactorUncertainty


def safeDiv(numerator, denominator):
  """
  `numerator` np.array()  
  `denominator` np.array()  
  `return` np.array  
  
  Old:Avoid NaNs when doing division. Replaces NaN with 0 in the array.
  New:Does the division 'where' denominator does not equal zero. When denominator is zero for an indice, 0 is put in the array    
  """
  ratio = np.divide(numerator, denominator, out=np.zeros_like(numerator), where=denominator!=0)
  #ratio = numerator / denominator
  #ratio[ ~ np.isfinite(ratio) ] = 0.0 # account for division by zero error
  return ratio


def splitOperand(operand, bracket="()"):
  """
  `operand` String (The operand which you want to decompose into chunks)  
  `bracket` String [optional] (The type of bracket you want to avoid splitting, 
  eg (), {}, []...)  
  `return` list(String)  
  
  Splits the operand of a formula into comma-separated chunks without splitting
  operands of nested functions.
  eg: Funcion1(foo, Function2( bar, foo), Function3(foo, Function4 (bar, foo)))  
  --> [foo,  Function2( bar, foo), Function3(foo, Function4 (bar, foo))]
  """
  parts = []
  bracket_level = 0
  current = []
  isQuote=False
  # trick to remove special-case of trailing chars
  for c in (operand + ","):
    if ((c == "," and not isQuote)  or c==bracket[1]) and bracket_level == 0:
      parts.append("".join(current).strip('"'))
      current = []
    else:
      if c == bracket[0]:
        bracket_level += 1
        current.append(c)
      elif c == bracket[1]:
        bracket_level -= 1
        current.append(c)
      elif c == '"':
        isQuote=not isQuote
        current.append(c)
      elif c == ' ':
        if isQuote: current.append(c)
      else:
        current.append(c)
  
  return parts


def getFormulaComponents(formula):
  """
  `formula` String  
  `return` list(String)  
  
  Take this formula written in this string and recusively optain a list of 
  basic components which is needs eg:  
  Funcion1(foo, Function2( bar, foo), Function3(foo, Function4 (bar, foo)))  
  --> [foo, bar, foo, foo, bar, foo]
  """
  result=[]
  if not ")" in formula: 
    # if there are no brackets in the formula, then there is just one element,
    # so add it to the list
    result+=[formula]
    return result 
  else:
    # if not, open the first level of brackets and recursively re-apply
    # this function to the arguments
    operation= formula.partition("(")[0]
    operand= formula.partition("(")[-1].rpartition(")")[0]
    arguments={}
    if 'Value'!=operation : 
      for argument in splitOperand(operand):
        result+=getFormulaComponents(argument)
    return result


def resolveFormula(nominal,formula, componentsMap, level=0, verbose=0):
  """
  `nominal` AnalysisObject (nominal object, which gives us the centeal values.
  The typ is a dict of np.arrays() encoding information equivalent
  to a Scatter2D, Histo1D, TGraphAsymmErrors or TH1D. See module description 
  for more information about this format. TODO: dedicated class for this?)  
  `formula` String (The formula with which to combine the components)  
  `componentsMap` dict(filenames,AnalysisObjects) (this is a map between the
  file which the component corresponds to and the AnalysisObject which it 
  corresponds to.  
  `level` Int [optional] (keeps track of how deep in the recursiom we have gone)  
  `verbose` Int [optional] (0 or 1, whether or not to print(a lot of debug messages)  )
  `return` AnalysisObject  
  
  Resolves the formula iteratively, using the AnalysisObjects listed in the
  componentsMap. resolveFormula supports the following functions:  
  
  `Central(arg1)`: Use the central value of arg1, and set errors to 0  
  
  `DownUpNominal(arg0, arg1, arg2)`: Produce a new AO where ydn is taken from 
  the central value ('y') of arg0, yup is taken from the central value of arg1,
  and y is taken from the central value of arg2.  
  
  `Envelope(arg1,...,argN)`: Take the min/max envelope of the arguments as the 
  up/down errors of the resulting AO. The central value is taken from 'nominal'.  
  
  `Inverse(arg1)`: Take the inverse of the AO, so y--> 1/y, and the errors are 
  propagated as yerr --> |yerr/y**2|  

  `Product(arg1,...,argN)`: Take the product of the arguments of the function. 
  The 'y' value is the product of the 'y' values of the arguments, the errors 
  are propagated according the relative errors in quadrature.  

  `QuadSum(arg1,..,argN)`: Sum in quadrature of the errors of the arguments,
  and central value taken from nominal AO.  
  
  `Value(arg1)`: Used to scan an AO and its errors by the float specified in argi1.  

  """
  # for debug statements, prepare a prefix which is proportional to the level
  # of iteration
  prefix=""
  y, yup, ydn ='y', 'yup', 'ydn'
  if 'z' in nominal.keys(): 
    y, yup, ydn ='z', 'zup', 'zdn'
  for i in range(level): prefix += "-->"
  if (verbose) : print(prefix+"processing ", formula)
  
  # if there is no bracket in the formula, this means we've reached a
  # basic component of the formula. In this case just return the relevant AO!
  if not ")" in formula: 
    result=None
    
    # now find the relevant AO from the name of the compoenent
    for k,v in componentsMap.items():
      componentName=k.split("/")[-1].replace(".yoda","").replace(".root","")
      if componentName==safeFileName(formula):
        result= v
    # print(error and quit if no matching AO is found!)
    if result is None:
      print("[ERROR] could not find a match for ", formula, )
      print(" (%s) " %safeFileName(formula), " in list of available weights:")
      for k,v in componentsMap.items():
        print(k.split("/")[-1])
      exit(1)
    
    # if all went well, return the AO
    if (verbose) :print(prefix+"--> result:", result )
    return result 

  # If there are brackets in the formula then it either needs to be evaluated 
  # of brocket down further
  else:
    
    # get the operation and the operand of the formula
    operation= formula.partition("(")[0]
    operand= formula.partition("(")[-1].rpartition(")")[0]
    arguments=[]
    argumentNames=[]
    
    # Split the operand into chunks (arguments). If the chunk is itself a 
    # a formula, it will need to be evluated first
    if operation=="Value":
      arguments=[float(operand)]
    else:
      for argument in splitOperand(operand):
        
        # evaluate each argument, recursively
        arg=resolveFormula(nominal,argument,componentsMap,level+1)
        argumentNames+=[argument]
        arguments+=[arg]
        if (verbose) :print(prefix+"argument of", operation , " : ",  arg)
      
    # Now that we have evluated the arguments, we can combine them according to
    # the specified operation.
    result=None

    # Perform a sum in quadrature of the errors of the arguments, and the
    # centreak value is taken from the nominal
    if operation=="QuadSum" :
      result=None
      for argument in arguments:
        if result is None: result = copy.deepcopy(argument)
        else:
          if not (np.allclose(nominal[y], argument[y])):
              print("[ERROR] trying to take the quadratic sum of uncertainties which don't have the same central value!")
              print("If you are using an alternative PDF directly int his function, please take the difference with the nominal and use that instead!")
              exit(1)
          result[yup]=nominal[y]+np.sqrt((argument[yup]-nominal[y])**2 + (result[yup]-nominal[y])**2 )
          result[ydn]=nominal[y]-np.sqrt((argument[ydn]-nominal[y])**2 + (result[ydn]-nominal[y])**2 )
    
    # Get the central value of this AO, setting the errors to 0
    elif operation=="Central" :
      result=copy.deepcopy(arguments[0])
      result[yup]=result[y]
      result[ydn]=result[y]
    
    # Produce a new AO where ydn is taken from the central value ('y') of arg0,
    # yup is taken from the central value of arg1, and y is taken from the 
    # central value of arg2.
    elif operation=="DownUpNominal" :
      if len(arguments)!=3: 
        print("[ERROR] DownUpNominal() must take exactly three arguments, found ",len(arguments))
        exit(1)
      result=copy.deepcopy(arguments[2])
      result[yup]=copy.deepcopy(arguments[1][y])
      result[ydn]=copy.deepcopy(arguments[0][y])
    
    # Take the product of the arguments of the function. The 'y' value is the
    # product of the 'y' values of the arguments, the errors are propagated 
    # according the relative errors in quadrature.
    elif operation=="Product" :
      result=None
      for arg in arguments:
        if result is None: result = copy.deepcopy(arg)
        else:
          centralProduct=result[y]*arg[y]
          upError=centralProduct*np.sqrt( (safeDiv(result[y]-result[yup],result[y]) )**2 + (safeDiv(arg[y]-arg[yup],arg[y]) )**2 )
          dnError=centralProduct*np.sqrt( (safeDiv(result[y]-result[ydn],result[y]) )**2 + (safeDiv(arg[y]-arg[ydn],arg[y]) )**2 )
          result[y]= centralProduct
          result[yup]= centralProduct+upError
          result[ydn]= centralProduct-dnError
    
    # Take the min/max envelope of the arguments as the up/down errors of the 
    # resulting AO. The central value is taken from 'nominal'
    elif operation=="Envelope" :
      argumentDict={}
      for i in range(len(arguments)):
        argumentDict[argumentNames[i]]=arguments[i]
      cen,dn,up=combineVariationsEnvelope(nominal, argumentDict,  asym=True, includeErrorsInEnvelope=True)
      result=copy.deepcopy(nominal)
      result[y]=cen
      result[yup]=up
      result[ydn]=dn
    elif operation=="EnvelopeIgnoreErrors" :
      argumentDict={}
      for i in range(len(arguments)):
        argumentDict[argumentNames[i]]=arguments[i]
      cen,dn,up=combineVariationsEnvelope(nominal, argumentDict,  asym=True, includeErrorsInEnvelope=False)
      result=copy.deepcopy(nominal)
      result[y]=cen
      result[yup]=up
      result[ydn]=dn
    
    # Take the inverse of the AO, so y--> 1/y, and the errors are propagated 
    # as yerr --> |yerr/y**2|
    elif operation=="Inverse" :
      if len(arguments)!=1: 
        print("[ERROR] Inverse() must take exactly 1 argument,found ",len(arguments))
        exit(1)
      arg=arguments[0]
      result=copy.deepcopy(arg)
      result[y]=safeDiv(np.ones(len(arg[y])),arg[y])
      result[yup]= result[y] +  np.abs(safeDiv(arg[y]-arg[yup],arg[y]**2))
      result[ydn]= result[y] -  np.abs(safeDiv(arg[y]-arg[ydn],arg[y]**2))
    
    elif operation=="Value" :
      if len(arguments)!=1: 
        print("[ERROR] Value() must take exactly 1 argument,found ",len(arguments))
        exit(1)
      arg=arguments[0]
      result=copy.deepcopy(nominal)
      result[y]=arg*np.ones(len(result[y]))
      result[yup]=arg*np.zeros(len(result[y])) 
      result[ydn]= arg*np.zeros(len(result[y]))
    
    # Anything else needs to be implemented here :)
    # elif operation=="FooBar":...
    else:
      print("[ERROR] this operation", operation, " is not yet supported! Please implement it in resolveFormula() before continuing")
      exit(1)
    
    # return the result of the operation on the arguments
    return result


def combineVariationsLHAPDF(nom, variations, pset, asym=False):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `pset` LHAPDF PDFSet (obtained from lhapdf.getPDFSet(pdfSetName))  
  `asym` Bool [optional] (return asymmetric errors? Or symmetrize?)  
  `return` AnalysisObject  

  Combines PDF variations according to the LHAPDF PDFSet prescription.
  """
  #if len (nom['y']) > 50:
  #  print("NOM %.6e"%nom['y'][45],  nom['name'])
  #  print(len(variations), "variations " , ["%.6e "%var['y'][45] for var in variations.values()]
  y='y'
  if 'z' in nom.keys(): y='z'
  nom_y=nom[y]
  pdfsyst = np.array(nom_y)
  nbins = len(nom_y)
  variationsArray=[]
  for k in sorted(variations.keys()):
    var=variations[k]
    if np.allclose(nom_y,var[y]):
      variationsArray=[var[y]]+variationsArray
    else:
      variationsArray.append(var[y])
  pdfvars=  np.asarray(variationsArray)
  pdfcen = np.array([ pset.uncertainty(pdfvars[:,i]).central  for i in range(nbins) ])
  pdfup  = np.array([ pset.uncertainty(pdfvars[:,i]).errplus  for i in range(nbins) ])
  pdfdn  = np.array([ pset.uncertainty(pdfvars[:,i]).errminus for i in range(nbins) ])
  pdfsym = np.array([ pset.uncertainty(pdfvars[:,i]).errsymm  for i in range(nbins) ])
  pdfrel = safeDiv(pdfsym, pdfcen)
  pdfrelup = safeDiv(pdfup, pdfcen)
  pdfreldn = safeDiv(pdfdn, pdfcen)
  pdfsyst = pdfrel * nom_y
  pdfsystup = pdfrelup * nom_y
  pdfsystdn = pdfreldn * nom_y
  if asym: return (nom_y, nom_y-pdfsystdn, nom_y+pdfsystup)
  else: return (nom_y, nom_y-pdfsyst, nom_y+pdfsyst)


def combineVariationsEnvelope(nom, variations, asym=True, includeErrorsInEnvelope=False):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `asym` Bool [optional] (return asymmetric errors? Or symmetrize?)  
  `includeErrorsInEnvelope` Bool [optional]  
  `return` AnalysisObject  

  Take the min/max envelope of the arguments as the up/down errors of the
  resulting AO. The central value is taken from 'nominal'.
  """
  y, yup, ydn ='y', 'yup', 'ydn'
  if 'z' in nom.keys(): 
    y, yup, ydn ='z', 'zup', 'zdn'
  systup = np.array(nom[y])
  systdn = np.array(nom[y])
  for var in variations.values():
    if includeErrorsInEnvelope:
      systup = map(max, zip(systup, var[yup],var[ydn]))
      systdn = map(min, zip(systdn, var[yup],var[ydn]))
    else:
      systup = map(max, zip(systup, var[y]))
      systdn = map(min, zip(systdn, var[y]))
  if not asym:
    var_tmp = 0.5*(abs(nom[y]-systup)+abs(nom[y]-systdn))
    systup= nom[y] + var_tmp
    systdn= nom[y] - var_tmp
  return np.array(nom[y]), np.array(systdn), np.array(systup)


def combineVariationsReplicas(nom, variations, asym=True):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `asym` Bool [optional] (return asymmetric errors? Or symmetrize?)  
  `return` AnalysisObject  

  Takes the standard deviation of the variations... in principle
  This is a placeholder for now... needs to be implemented TODO
  """
  return np.array(nom['y']), np.array(nom['y']), np.array(nom['y'])


def combineVariationsAlphaS(nom, variations):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `return` AnalysisObject  

  Get the alphaS variation. This is just a special case of Envelope
  where the error is symmetrized!
  """
  return combineVariationsEnvelope(nom, variations,  asym=False, includeErrorsInEnvelope=False)


def combineVariationsHessian(nom, variations, asym=True):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `asym` Bool [optional] (return asymmetric errors? Or symmetrize?)  
  `return` AnalysisObject  

  Combine the specified variations according to the Hession prescription
  Central value given by nom.
  """
  y='y'
  if 'z' in nom.keys(): y='z'
  nom_y=nom[y]
  systup = np.array(nom_y) 
  systdn = np.array(nom_y) 
  sumsq = np.array(nom_y) - np.array(nom_y)
  for var in variations:
      sumsq +=  (nom_y -var) ** 2
  systup += np.sqrt(sumsq)  
  systdn -= np.sqrt(sumsq)  
  
  return nom_y, systdn, systup


def combineVariationsFromFormula(nom, variations, formula):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `variations` dict(String, AnalysisObject)  
  `formula` String (custom formula to combine uncertainties)  
  `return` AnalysisObject  

  Combine the specified variations according to custom formula given by formula 
  """
  y, yup, ydn ='y', 'yup', 'ydn'
  if 'z' in nom.keys(): 
    y, yup, ydn ='z', 'zup', 'zdn'
  output= resolveFormula(nom,formula,variations)
  return output[y], output[ydn], output[yup]


def combineVariationsStat(nom):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `return` AnalysisObject  

  Dummy function which currently just grabs the dn/up errs as the stat errs.
  """
  y, yup, ydn ='y', 'yup', 'ydn'
  if 'z' in nom.keys(): 
    y, yup, ydn ='z', 'zup', 'zdn'
  return nom[y], nom[ydn], nom[yup]

def combineVariationsNorm(nom, val=0.05):
  """
  `nom` AnalysisObject (of the nominal variation)  
  `val` relative size of the normalisation uncertainty  
  `return` AnalysisObject  

  Apply a k-factor normalization uncertainty 
  """
  y, yup, ydn ='y', 'yup', 'ydn'
  if 'z' in nom.keys(): 
    y, yup, ydn ='z', 'zup', 'zdn'
  return nom[y], nom[y]*(1-val), nom[y]*(1+val)

def readFromFile(filename,regexFilter=None, regexVeto=None):
  """
  `filename` String (path to file which is to be read)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional](AOs whose names match regex are NOT processed)  
  `return` dict{String,AnalysisObject}  

  decides whether to process a file as ROOT or YODA depending on the file extension
  """
  rootExtensions=['.root','.root1','.root2']
  yodaExtensions=['.yoda','.yoda1','.yoda.gz']
  if filename==None : return {}
  filenameSplit=filename.split(":") 
  for re in rootExtensions:
    if filenameSplit[0].endswith(re): 
      return readFromROOT(filename,regexFilter,regexVeto)
  for ye in yodaExtensions:
    if  filenameSplit[0].endswith(ye): 
      return readFromYODA(filename,regexFilter,regexVeto)
  print("[ERROR] could not identify input file type! please name your files with one of the following extensions:")
  print("[ERROR]", rootExtensions, " for ROOT files")
  print("[ERROR]", yodaExtensions, " for YODA files")
  exit(1)

def getFileKeys(d, basepath="/"):
  """
  `d` TDirectory or  TFile  (name of file or directory to be read)  
  `basepath` String [optional] (absolute path leading to `d` in the file)  
  `return` list[String,AnalysisObject]  

  Recursively gets the list of Histogram/TGraph names inside a ROOT file directory (supports nested directories)
  """
  for key in d.GetListOfKeys():
    kname = key.GetName()
    if key.IsFolder() :
      if d.Get(kname).ClassName()=="TList": continue
      if d.Get(kname).ClassName()=="TTree": continue
      for i in getFileKeys(d.Get(kname), basepath+kname+"/"):
        yield i
    else:
      yield basepath+kname, d.Get(kname)

def readFromROOT(filename,regexFilter=None, regexVeto=None):
  """
  `filename` String (path to file which is to be read)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional](AOs whose names match regex are NOT processed)  
  `return` dict{String,AnalysisObject}  

  Open a ROOT file, read the contents and return them as AnalysisObjects.
  Control which AOs to select/reject using the optional regexFilter/regexVeto
  arguments.
  Only supports TH1D and TGraphAsymmErrors types for now. TODO Support other types.
  """
  result={}
  filename=filename.split(":")
  rootfile=r.TFile.Open(filename[0])
  tfd=rootfile
  path=""
  if len (filename)>1:
    if '/' in filename[1]: 
      path=filename[1].split("/")[0]
      tfd=rootfile.Get(path)
  for aoName in tfd.GetListOfKeys():
    #print("CONTAINS THIS AO", aoName)
    if path == "":
      aoName=aoName.GetName()
    else:
      aoName=path+"/"+aoName.GetName()
    # apply the filters and vetos
    if (regexFilter is not None):
      if (not aoName in re.findall(regexFilter,aoName)): continue
    if (regexVeto is not None):
      if ( aoName in re.findall(regexVeto,aoName)): continue
    rep=""
    newAoName=""
    if len (filename) > 1:
      rep=filename[1].replace("!AONAME","")
      filt=filename[1].replace("!AONAME",".*")
      capt=filename[1].replace("!AONAME","(.*)")
      if (not aoName in re.findall(filt,aoName)): 
        continue
      
      newAoName=re.findall(capt,aoName)[0]
    else:
      newAoName=aoName.replace(rep,"")
    ao=rootfile.Get(aoName)
    aoName=newAoName
    
    aoResult={}
    aoResult['name'] = aoName
    aoResult['path'] = filename[0]+'/'+aoName
    if (type(r.TH1D())== type(ao)) or  (type(r.TH1F())== type(ao)):
      binRange=range(1,ao.GetNbinsX()+1)
      #ao.Sumw2()
      aoResult['info'] ='Was_a_TH1'
      aoResult['bw']  = np.array([ ao.GetBinWidth(b)  for b in binRange])
      aoResult['y']  = np.array([ ao.GetBinContent(b)  for b in binRange])
      stat           = np.array([ ao.GetBinError(b)  for b in binRange])
      aoResult['yup']   = (aoResult['y'] + stat )#/ aoResult['bw']
      aoResult['ydn']   =( aoResult['y'] - stat )#/ aoResult['bw']
      aoResult['x'] = np.array([ ao.GetBinCenter(b)  for b in binRange])
      aoResult['xup'] = np.array([ ao.GetBinLowEdge(b+1)  for b in binRange])
      aoResult['xdn'] = np.array([ ao.GetBinLowEdge(b)  for b in binRange]) 
      aoResult['nbins'] = len(binRange)  
    elif (type(r.TH2D())== type(ao)) or  (type(r.TH2F())== type(ao)):
      xbinRange=range(1,ao.GetNbinsX()+1)
      ybinRange=range(1,ao.GetNbinsY()+1)
      #ao.Sumw2()
      aoResult['info'] ='Was_a_TH2'
      aoResult['xbw']  = np.array([ ao.GetXaxis().GetBinWidth(b)  for b in xbinRange for c in ybinRange])
      aoResult['ybw']  = np.array([ ao.GetYaxis().GetBinWidth(c)  for b in xbinRange for c in ybinRange])
      aoResult['z']  = np.array([ ao.GetBinContent(b,c)  for b in xbinRange for c in ybinRange])
      stat           = np.array([ ao.GetBinError(b,c)  for b in xbinRange for c in ybinRange])
      aoResult['zup']   = (aoResult['z'] + stat )#/ aoResult['bw']
      aoResult['zdn']   =( aoResult['z'] - stat )#/ aoResult['bw']
      aoResult['x'] = np.array([ ao.GetXaxis().GetBinCenter(b)  for b in xbinRange for c in ybinRange])
      aoResult['xup'] = np.array([ ao.GetXaxis().GetBinLowEdge(b+1)  for b in xbinRange for c in ybinRange])
      aoResult['xdn'] = np.array([ ao.GetXaxis().GetBinLowEdge(b)  for b in xbinRange for c in ybinRange]) 
      aoResult['y'] = np.array([ ao.GetYaxis().GetBinCenter(c)  for b in xbinRange for c in ybinRange])
      aoResult['yup'] = np.array([ ao.GetYaxis().GetBinLowEdge(c+1)  for b in xbinRange for c in ybinRange])
      aoResult['ydn'] = np.array([ ao.GetYaxis().GetBinLowEdge(c)  for b in xbinRange for c in ybinRange]) 
      aoResult['nbinsx'] = len(xbinRange)  
      aoResult['nbinsy'] = len(ybinRange) 
    elif type(r.TGraphAsymmErrors())== type(ao):
      binRange=range(0,ao.GetN())
      aoResult['info'] ='Was_a_TG'
      aoResult['bw']  = np.array([ ao.GetErrorXlow(b)+ao.GetErrorXhigh(b) for b in binRange ]) # dummy
      aoResult['y']  = np.array([ ao.GetY()[b]  for b in binRange ])
      aoResult['yup']   = np.array([ ao.GetY()[b] + ao.GetErrorYhigh(b)  for b in binRange ]) 
      aoResult['ydn']   = np.array([ ao.GetY()[b] - ao.GetErrorYlow(b) for b in binRange ])
      aoResult['x'] = np.array([ ao.GetX()[b]  for b in binRange ]) 
      aoResult['xup'] = np.array([ ao.GetX()[b] + ao.GetErrorXhigh(b)  for b in binRange ])  
      aoResult['xdn'] = np.array([ ao.GetX()[b] - ao.GetErrorXlow(b)  for b in binRange ])  
      aoResult['nbins'] = len(binRange)  
    elif type(r.TTree)== type(ao):
      aoResult['tree'] = ao.GetTree()  
    else :
       continue
    #assert(len(aoResult['y'])==len(aoResult['x']))
    result[aoName]=aoResult
  if len (result)==0:
    print("[ERROR] no analysis objects passed your filter, or your input files are empty of TH1 or TGraph objects. Exiting" )
    exit(1)
  return result

def readFromYODA(filename,regexFilter=None, regexVeto=None):
  """
  `filename` String (path to file which is to be read)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional](AOs whose names match regex are NOT processed)  
  `return` dict{String,AnalysisObject}  

  Open a YODA file, read the contents and return them as AnalysisObjects.
  Control which AOs to select/reject using the optional regexFilter/regexVeto
  arguments.
  """
  result={}
  filename=filename.split(":")
  patterns="" if regexFilter is None else '.*'+regexFilter+'.*' 
  unpatterns="" if regexVeto is None else '.*'+regexVeto+'.*' 
  histList=yoda.read(filename[0], patterns=patterns, unpatterns=unpatterns)
  for aoName,ao in histList.items():
    # apply the filters and vetos
    if (regexFilter is not None):
      if (not aoName in re.findall(regexFilter,aoName)): continue
    if (regexVeto is not None):
      if ( aoName in re.findall(regexVeto,aoName)): continue
    
    rep=""
    if len (filename) > 1:
      rep=filename[1].replace("!AONAME","")
      filt=filename[1].replace("!AONAME",".*")
      if (regexFilter is not None):
        if (not aoName in re.findall(filt,aoName)): continue
      aoName=aoName.replace(rep,"")                                                         

    aoResult={}
    aoResult['name'] = ao.name().replace(rep,"")
    aoResult['path'] = ao.path().replace(rep,"")
    aoResult['annotations'] = {ann: ao.annotation(ann) for ann in ao.annotations()}
    if type(yoda.Histo1D())== type(ao):
      aoResult['bw']  = np.array([ b.xWidth()  for b in ao.bins() ])
      aoResult['y']  = np.array([ b.sumW()  for b in ao.bins() ])/ aoResult['bw']
      stat           = np.array([ b.sumW2() for b in ao.bins() ])/ (aoResult['bw'])**2
      aoResult['yup']   = (aoResult['y'] + np.sqrt(stat) )#/ aoResult['bw']
      aoResult['ydn']   =( aoResult['y'] - np.sqrt(stat) )#/ aoResult['bw']
      aoResult['x'] = np.array([ b.xMid()  for b in ao.bins() ])
      aoResult['xup'] = np.array([ b.xMax()  for b in ao.bins() ])
      aoResult['xdn'] = np.array([ b.xMin()  for b in ao.bins() ])
      aoResult['nbins'] = len(ao.bins())   
    elif type(yoda.Scatter2D())== type(ao):
      aoResult['bw']  = np.array([ b.yErrs()[1]+b.yErrs()[0]  for b in ao.points() ]) # dummy
      aoResult['y']  = np.array([ b.y()  for b in ao.points() ])
      aoResult['yup']   = np.array([ b.y() + b.yErrs()[1]  for b in ao.points() ]) 
      aoResult['ydn']   = np.array([ b.y() - b.yErrs()[0] for b in ao.points() ])
      aoResult['x'] = np.array([ b.x()  for b in ao.points() ]) 
      aoResult['xup'] = np.array([ b.x() + b.xErrs()[1]  for b in ao.points() ])  
      aoResult['xdn'] = np.array([ b.x() - b.xErrs()[0]  for b in ao.points() ])  
      aoResult['nbins'] = len(ao.points())                                                     
    elif type(yoda.Histo2D())== type(ao):
      aoResult['xbw']  = np.array([ b.xWidth()  for b in ao.bins() ])
      aoResult['ybw']  = np.array([ b.yWidth()  for b in ao.bins() ])
      aoResult['z']  = np.array([ b.sumW()  for b in ao.bins() ])/ (aoResult['xbw']*aoResult['ybw'])
      stat           = np.array([ b.sumW2() for b in ao.bins() ])/ ((aoResult['xbw']*aoResult['ybw']))**2
      aoResult['zup']   = (aoResult['z'] + np.sqrt(stat) )#/ aoResult['bw']
      aoResult['zdn']   = (aoResult['z'] - np.sqrt(stat) )#/ aoResult['bw']
      aoResult['x'] = np.array([ b.xMid()  for b in ao.bins() ])
      aoResult['xup'] = np.array([ b.xMax()  for b in ao.bins() ])
      aoResult['xdn'] = np.array([ b.xMin()  for b in ao.bins() ])
      aoResult['y'] = np.array([ b.yMid()  for b in ao.bins() ])
      aoResult['yup'] = np.array([ b.yMax()  for b in ao.bins() ])
      aoResult['ydn'] = np.array([ b.yMin()  for b in ao.bins() ])
      aoResult['nbinsx'] = ao.numBinsX()
      aoResult['nbinsy'] = ao.numBinsY()
    else :
       continue
    result[aoName]=aoResult
  
  if len (result)==0:
    print("[ERROR] no analysis objects passed your filter, or your input files are empty of Histo1D or Scatter2D objects. Exiting" )
    print("[ERROR] FYI: filename=%s, filter=%s, veto=%s "%(filename,regexFilter, regexVeto) )
    exit(1)
  
  return result

def writeToFile(histDict,fOut):
  """
  `histDict` dict{String,AnalysisObject} (AOs to write out)  
  `fOut` String (output file to write to)  
  `return` None  

  Write AOs out to a file, auto-determines root or yoda from file extension of fOut.
  """
  rootExtensions=['.root','.root1','.root2']
  yodaExtensions=['.yoda','.yoda1','.yoda.gz']
  for re in rootExtensions:
    if fOut.endswith(re): 
      writeToROOT(histDict,fOut)
      return
  for ye in yodaExtensions:
    if  fOut.endswith(ye): 
      writeToYODA(histDict,fOut)
      return
  print("[ERROR] could not identify input file type to write to ! Assumign YODA.")
  writeToYODA(histDict,fOut)
  return

def writeToYODA(histDict,fOut):
  """
  `histDict` dict{String,AnalysisObject} (AOs to write out)  
  `fOut` String (output file to write to)  
  `return` None  

  Write AOs out to a YODA file.
  """
  f = open(fOut, 'w')
  for aoKey, inAO in histDict.items():
    if 'z' in inAO.keys() : continue # TODO... for now, need to rewrite this function for 2D histos
    outAO = yoda.Scatter2D()
    for k,v in inAO['annotations'].items():
      if k=="Type": continue
      outAO.setAnnotation(k,v)
    for i in range (inAO['nbins']):
      p = yoda.Point2D()
      p.setX(inAO['x'][i])
      p.setY(inAO['y'][i])
      p.setErrs(1,(abs(inAO['xdn'][i]-inAO['x'][i]),abs(inAO['xup'][i]-inAO['x'][i])))
      p.setErrs(2,(abs(inAO['ydn'][i]-inAO['y'][i]),abs(inAO['yup'][i]-inAO['y'][i])))
      outAO.addPoint(p)
    yoda.writeYODA(outAO, f)
  f.close()

def writeToROOT(histDict,fOut):
  """
  `histDict` dict{String,AnalysisObject} (AOs to write out)  
  `fOut` String (output file to write to)  
  `return` None  

  Write AOs out to a ROOT file.
  """
  f =r.TFile.Open(fOut,"RECREATE")
  for aoKey, inAO in histDict.items():
      if not 'z' in inAO.keys(): #no 'z' filled for 1D graphs and TH1Ds
          outAO = r.TGraphAsymmErrors()
          outAO.SetName(aoKey)
          outAO.SetTitle(aoKey)
          if len(inAO['xdn']) : binningInfo=[inAO['xdn'][0]]
          else: continue
          for xup in inAO['xup']: binningInfo+=[xup]
          binArray = array('d', binningInfo)
          outAO_up = r.TH1D(aoKey+"__1up",aoKey+"__1up", len(binArray)-1, binArray)
          outAO_down = r.TH1D(aoKey+"__1down",aoKey+"__1down", len(binArray)-1, binArray)
          outAO.SetName(aoKey)
          outAO.SetTitle(aoKey)
          for i in range (inAO['nbins']):
            outAO.SetPoint(i,inAO['x'][i],inAO['y'][i])
            outAO.SetPointError(i,inAO['x'][i]-inAO['xdn'][i],inAO['xup'][i]-inAO['x'][i],abs(inAO['y'][i]-inAO['ydn'][i]),abs(inAO['yup'][i]-inAO['y'][i]))
            outAO_up.SetBinContent(i+1,inAO['yup'][i] )
            outAO_up.SetBinError(i+1,0.)
            outAO_down.SetBinContent(i+1,inAO['ydn'][i] )
            outAO_down.SetBinError(i+1,0.)
          outAO.Write()
          outAO_up.Write()
          outAO_down.Write()

      else:  
          xbins = []
          ybins = []
          for y in inAO['ydn']:
            while y not in ybins:
              ybins.append(y)    
          for x in inAO['xdn']:
            while x not in xbins:
              xbins.append(x)
          xbins.append(inAO['xup'][np.where(inAO['xdn'] == x)][0])
          ybins.append(inAO['yup'][np.where(inAO['ydn'] == y)][0])
          xbinArray = array('d', xbins)
          ybinArray = array('d', ybins)
          outAO = r.TH2D(aoKey, "histo", len(xbinArray)-1, xbinArray, len(ybinArray)-1, ybinArray)
          outAO.Sumw2();
          outAO.SetName(aoKey)
          outAO.SetTitle(aoKey)
          for iBin in range(len(inAO['z'])):
              ix = inAO['x'][iBin] 
              iy = inAO['y'][iBin]
              xybin = outAO.GetBin(outAO.GetXaxis().FindBin(ix), outAO.GetYaxis().FindBin(iy))
              outAO.SetBinContent(xybin, inAO['z'][iBin])
              outAO.SetBinError(xybin, abs(inAO['z'][iBin] - inAO['zup'][iBin]))
          outAO.Write()                                                                                           
  f.Close()
 
   
def getAverageUncertaintySizePerBin(fIn, regexFilter=None, regexVeto=None):
  """
  `fIn` String (input file to evaluate)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional] (AOs whose names match regex are NOT processed)  
  `return` Float  

  Get a rough estimate of the average symmetric relative error per bin, used to
  determine what order to plot the uncertaities in.
  """
  averageUncertaintySizePerBin=None   
  nominalHists= readFromFile(fIn)     
  for ao in nominalHists.keys():      
    # apply filters/vetos             
    if (regexFilter is not None):
      if (not ao in re.findall(regexFilter,ao)): continue
    if (regexVeto is not None):
      if ( ao in re.findall(regexVeto,ao)): continue
    noms=nominalHists[ao]
    y, yup, ydn ='y', 'yup', 'ydn'
    if 'z' in noms.keys(): 
      y, yup, ydn ='z', 'zup', 'zdn'
    #get symmetrix, relative error
    relativeError=safeDiv(0.5*(abs(noms[yup]-noms[y]) + abs(noms[ydn]-noms[y]) ),abs(noms[y]))
    if (averageUncertaintySizePerBin is None): averageUncertaintySizePerBin= relativeError
    else : 
       if np.average(averageUncertaintySizePerBin)!=0: 
         # if it's much larger than the other AOs we've processed, something has probably
         # gone wrong and we should ignore it...
         if  np.average(relativeError) > 100* np.average(averageUncertaintySizePerBin): continue
       averageUncertaintySizePerBin = np.append(averageUncertaintySizePerBin,relativeError ) 
     

  return np.average(averageUncertaintySizePerBin)


def combineVariation(wName, wInfo, fOut, regexFilter=None, regexVeto=None):
  """
  `wName` String (Name of the systemtic weight to combine)  
  `wInfo` dict in output format of the readDatabase.getWeight() tool.
  See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PmgSystTool
  for more info.  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional] (AOs whose names match regex are NOT processed)  
  `return` None  

  Produce aand write a YODA file for the systematioc uncertainty wName by 
  combining the weights listed in wInfo['weights'], according to the procedure
  specified in wInfo['combination'] for each AO.
  
  The following values of wInfo['combination'] are suppored: stat,lhapdf,
  replicas, alphas, envelope, hessian, and customFunction ( in this case, the
  formula given in wInfo['function'] is evaluated).
  """
  # if this is a PDF set, set up LHAPDF
  if wInfo['combination']=='lhapdf':
    if (not 'nominal_pdf' in wInfo.keys()) or (len(wInfo['weights'])==1):
      # sometimes a variation claims to be a PDFSet, but if there is only
      # one weight, it should just be taken as its own nominal.
      wInfo['combination']='stat'
    else:
      nominalPDF =  int(wInfo['nominal_pdf'])
      pdfSetName = lookupPDFSetName(nominalPDF)
      pset = lhapdf.getPDFSet(pdfSetName)
      pdfs = pset.mkPDFs()
  
  # Get the nominal and variated AOs for each weight
  nominalHists= readFromFile(wInfo['nominal'],regexFilter, regexVeto)
  variationHists_Var_AO={}
  variationHists={}
  for var in wInfo['weights']:
    if ":" in var and 'yoda' in var : var=var.split(":")[0]
    variationHists_Var_AO[var]= readFromFile(var,regexFilter, regexVeto)
  #probably a more elegant way to do this....
  for var,aoDict in variationHists_Var_AO.items():
    for ao, info in aoDict.items():
      if not ao in variationHists.keys(): 
        variationHists[ao]={}
      variationHists[ao][var]=info

  # now loop though AOs and process them
  outputHists=copy.deepcopy(nominalHists)
  for ao in sorted(nominalHists.keys()):
    noms=nominalHists[ao]
    y, yup, ydn ='y', 'yup', 'ydn'
    if 'z' in noms.keys(): 
      y, yup, ydn ='z', 'zup', 'zdn'
    variations=variationHists[ao]
    syst_central = np.array(noms)
    syst_up = np.array(noms)
    syst_dn = np.array(noms)
     
    # calculate the value of the combined uncertainty according to the 
    # specified method
    if   wInfo['type']=='Nominal' or wInfo['combination']=='stat':
      syst_central,syst_dn,syst_up = combineVariationsStat(noms)
    elif wInfo['combination']=='lhapdf':
      syst_central,syst_dn,syst_up = combineVariationsLHAPDF(noms, variations,  pset, asym=False)
    elif wInfo['combination']=='replicas':
       syst_central,syst_dn,syst_up = combineVariationsReplicas(noms, variations, asym=True)
    elif wInfo['combination']=='envelope':
       syst_central,syst_dn,syst_up = combineVariationsEnvelope(noms, variations,  asym=True, includeErrorsInEnvelope=False)
    elif wInfo['combination']=='alphaS':
       syst_central,syst_dn,syst_up = combineVariationsAlphaS(noms, variations)
    elif wInfo['combination']=='hessian':
       syst_central,syst_dn,syst_up = combineVariationsHessian(noms, variations, asym=True)
    elif wInfo['combination']=='norm':
       syst_central,syst_dn,syst_up = combineVariationsNorm(noms, val=wInfo['value'])
    elif wInfo['combination']=='customFunction':
       syst_central,syst_dn,syst_up = combineVariationsFromFormula(noms, variations, wInfo['function'])
    else:
       print("[ERROR] combination type:", wInfo['combination'], "is not yet supported... skipping this systematic uncertainty:", wName)
    
    syst_up = (syst_up)  
    syst_dn = (syst_dn)  
    syst_central= (syst_central)  
    
    outputHists[ao][y]= syst_central
    # LC this is maybe not quite right? FIXME TODO
    outputHists[ao][yup]= syst_up# - syst_central)
    outputHists[ao][ydn]= syst_dn# - syst_central)
      
  # write the outputs!
  writeToFile(outputHists, fOut) 
  print("[INFO] done combining this systematic: ", wName, " using method: ", wInfo['combination'])
  if wInfo['combination']=='customFunction':
     print("[INFO] customFunction = ", wInfo['function'])

def arrayDictToTGraph(ao, isData=False, setYErrorsToZero=False,nominalAOForRatio=None):
  """
  `ao` AnalysisObject (the one to convert to a TGraph)  
  `isData` Bool [optional] (modify style of TGraph for data points)  
  `setYErrorsToZero` Bool [optional](Might want to do this if you don't care about the
  error bands for some uncertainty component)  
  `nominalAOForRatio` [optional]AnalysisObject or None (if not None, make a ratio plot
  with respect to this AnalysisObject)  
  `return` ROOT.TGraphAsymmErrors()  

  Fill a TGraphAsymmErrors from an AO, for plotting purposes. 
  Can format as MC or Data. If nominalAOForRatio is specified (ie not None), 
  the TGraphAsymmErrors is divided by the 'y' value of the nominalAOForRatio 
  in each bin.
  """
  tg=r.TGraphAsymmErrors()
  for i in range (len(ao['y'])):
    scaleBy=1.
    if (nominalAOForRatio is not None) :  
      if not (nominalAOForRatio['y'][i]==0. or np.isnan(nominalAOForRatio['y'][i])): scaleBy=1./nominalAOForRatio['y'][i]
    yVal=ao['y'][i]
    if(np.isnan(yVal)):
      yVal=0
    tg.SetPoint(i,ao['x'][i],yVal*scaleBy)
    yErrUp= 0. if (setYErrorsToZero or np.isnan(ao['yup'][i]-ao['y'][i])) else (ao['yup'][i]-ao['y'][i]) *scaleBy
    yErrDn= 0. if (setYErrorsToZero or np.isnan(ao['y'][i]-ao['ydn'][i])) else (ao['y'][i]-ao['ydn'][i]) *scaleBy
    tg.SetPointError(i,ao['x'][i]-ao['xdn'][i], 
                       ao['xup'][i]-ao['x'][i],
                       yErrDn,yErrUp, 
                       )
    if not isData:
      #tg.SetLineColor(colour)
      tg.SetLineWidth(2)
      #tg.SetFillColorAlpha(colour,0.5)
    else: 
      tg.SetMarkerSize(1)
      tg.SetMarkerStyle(20)
      tg.SetLineWidth(3)

  return tg


def getPlotInfo(aoName, pathInRivetEnv): 
  """
  `aoName` String (name to access plot info for)  
  `pathInRivetEnv` String (.plot file where to get the plot info from)  
  `return` dict{String,String} (the list of plotting specifications from the 
  .plot file, as a dict)  

  Rivet uses a separate .plot file to format plots. We want to use the same
  information to format our plots, and this function is a helper to retrieve
  that info in a more usable format from a given .plot file.
  """
  f= open(pathInRivetEnv, 'r')
  inBlock=False
  res={}
  for line in f.readlines():
    line=line.strip()
    if "BEGIN" in line:
      regex=line.split()[-1].replace("..",".*")+".*"
      try:
        match=re.findall(regex,aoName)
      except:
        print("[WARNING] this line in .plot does not provide a valid regex.. : ", line)
        return res
      if len(match)>0 : inBlock=True
      continue
    elif  "END" in line:
      inBlock=False
    elif inBlock:
      k=line.partition("=")[0]
      v=line.partition("=")[-1]
      res[k]=v
  return res


def safeRootLatex(unsafeLatex):
  """
  `unsafeLatex` String (unsafe Latex string to be converted)  
  `return` String (safe TLatex string which can be used on ROOT plots)  

  TLatex is not quite the same as regular latex, and won't compiled properly
  out of the box unless a few changes are made. This function does that
  hopefully in the majority of cases! No promises though... *sigh*
  """
  unsafeLatex=unsafeLatex.replace("$","")
  unsafeLatex=unsafeLatex.replace("\\","#")
  unsafeLatex=unsafeLatex.replace("^#text","^")
  while "#text" in unsafeLatex:
    operand=unsafeLatex.partition("#text{")[-1].partition("}")[0]
    unsafeLatex=unsafeLatex.replace("#text{%s}"%operand, operand)
  while "#mathrm" in unsafeLatex:
    operand=unsafeLatex.partition("#mathrm{")[-1].partition("}")[0]
    unsafeLatex=unsafeLatex.replace("#mathrm{%s}"%operand, operand)
  while "#dfrac" in unsafeLatex:
    unsafeLatex=unsafeLatex.replace("}{"," / ")
    right=unsafeLatex.partition("#dfrac{")[-1] 
    text=[]
    bracketLeunsafeLatexel=0
    for item in right:
      if item=="{" : bracketLeunsafeLatexel+=1
      if item=="}" : 
       if bracketLeunsafeLatexel==0:
         break
       bracketLeunsafeLatexel-=1
      text+=[item]
    text="".join(text)
    unsafeLatex=unsafeLatex.replace("#dfrac{"+text+"}",text)

  dfracs=re.findall("#dfrac{.*?}{.*?}",unsafeLatex)
  missingBrackets=re.findall("\^.",unsafeLatex)
  for mb in missingBrackets:
    if "{" in mb: continue
    if "#" in mb: continue
    unsafeLatex=unsafeLatex.replace(mb,mb.replace("^","^{")+"}")
  missingBrackets=re.findall("\_.",unsafeLatex)
  for mb in missingBrackets:
    if "{" in mb: continue
    if "#" in mb: continue
    unsafeLatex=unsafeLatex.replace(mb,mb.replace("_","_{")+"}")
  if "#ge" in unsafeLatex and not "#geq" in unsafeLatex:
     unsafeLatex=unsafeLatex.replace("#ge","#geq")
  unsafeLatex=unsafeLatex.replace("#ell","l") #not supported in TLatex
  unsafeLatex=unsafeLatex.replace("#cos","cos ") #not supported in TLatex
  unsafeLatex=unsafeLatex.replace("#sin","sin ") #not supported in TLatex

  return unsafeLatex
  

def makeDummyHisto(tg,isLog=False, XandYMinAndMax=None, ratioZoom=None, isRatio=False):
  """
  `tg` TGraphAsymmErrors (use this to build the dummy TH1D)  
  `isLog` Bool [optional] (Max/Min will need to be adjusted differently
  in case of a log plot)  
  `XandYMinAndMax` [Float,Float,Float,Float] or None (if not None, use this factor as the low/high limits of the
  plot axes)  
  `ratioZoom` [Float,Float] or None (if not None, use this factor as the low/high limits of the
  ratio plot)  
  `isRatio` Bool [optional] (Specify whether or not this is a ratio plot)  
  `return` TH2D (with appropriate max/min on each axis)  

  In order to control better what axis ranges to use, construct a dummy TH1D
  with the desired max/min on each axis to be able to show the TGraphs nicely 
  """
  multiplierUp=1.
  multiplierDn=1.
  if not isRatio:
    multiplierUp=2.
  nBins= tg.GetN()
  xmin=tg.GetX()[0]-  tg.GetErrorXlow(0)
  xmax=tg.GetX()[nBins-1]+  tg.GetErrorXhigh(nBins-1)
  ymin=min([ tg.GetY()[i]-tg.GetErrorYlow(i) for i in range(nBins)])
  ymax=max([ tg.GetY()[i]+tg.GetErrorYhigh(i) for i in range(nBins)])
  if isLog: 
    # if it's a log plot, the multpliers need to be much larger, and ymin >0
    if ymin<0: ymin=min([ tg.GetY()[i] for i in range(nBins)])
    multiplierUp=ymax/ymin if ymin>0 else ymax 
    multiplierUp*=50
    multiplierDn=0.5
    ymin*=multiplierDn
    ymax*=multiplierUp
  else:
    ymax*=multiplierUp
  if ratioZoom is not None:
    #if a particular axis range for the ratio plot is specified, use that
    #ymin=1-(ratioZoom-1) +0.001
    ymin=ratioZoom[0] +0.001
    ymax=ratioZoom[1]  -0.001
    multiplierUp=1.
    multiplierDn=1.
  elif isRatio:
    av=(abs(1-ymin) + abs(1-ymax))/2
    ymin=1 -av
    ymax=1 +av
    multiplierUp=1.1
    multiplierDn=1./1.1
    ymin*=multiplierDn
    ymax*=multiplierUp
  if XandYMinAndMax is not None:
   if XandYMinAndMax[0] is not None: xmin=float(XandYMinAndMax[0])
   if XandYMinAndMax[1] is not None: xmax=float(XandYMinAndMax[1])
   if XandYMinAndMax[2] is not None: ymin=float(XandYMinAndMax[2])
   if XandYMinAndMax[3] is not None: ymax=float(XandYMinAndMax[3])
  res= r.TH2D('dummy','',nBins,xmin,xmax,nBins,ymin,ymax)
  res.SetDirectory(0)
  return res
  

def makeSystematicsPlotsWithROOT(mergedSystDict, outdir, nominalName="Nominal", ratioZoom=None,regexFilter=None, regexVeto=None, label="", plotInfo=None):
  """
  `mergedSystDict` dict{String,String} (format is as follows {N!label:filename},
  where N is the order in which to plot the variations, labels is what to put on
  the legend, and filename is the name of the YODA/ROOT file to use as input)  
  `outdir` String (the output dir for the plots)  
  `nominalName` String (which entry of mergedSystDict is the nominal? One of the
  entries in mergedSystDict should have this as a label)  
  `ratioZoom` [Float,Float] or None (if not None, use this factor as the low/high limits of the
  ratio plot)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional](AOs whose names match regex are NOT processed)  
  `label` String [optional](additional label to add to the plots name)  
  `plotInfo` String [optional](path to plot info file (will try to find it dynamically if not specified)  
  `return` dict of output plots  

  This is a generic macro which will make the output plots of the AOs contained
  in the files listed in mergedSystDict.
  """
  r.gROOT.SetBatch()
  r.gStyle.SetOptStat(00000)
  outputPlots=[] 
  # Want to try to retrive a .plot and reference .yoda file for the relevant analysis!
  # This is where to check first..
  RIVET_ANALYSIS_PATH=os.environ.get("RIVET_ANALYSIS_PATH")
  os.system("mkdir -p %s" % (outdir))
  
  # sort the dict and collect <label>:<filename> pairs
  #fileNames=["%s:'%s'" % (v,k.split("!")[-1]) for k,v in sorted(mergedSystDict.items())]
  fileNames=["%s:'%s'" % (k,v) for k,v in sorted(mergedSystDict.items())]
  
  #holders for the aos to plot, and the ratios wrt nominal
  aos={}
  aos_ratio={}
  
  # The plots will be coloured in this order 
  colorList=[r.kMagenta, r.kRed, r.kOrange, r.kYellow, r.kGreen, r.kBlue,
             r.kViolet, r.kPink, r.kSpring, r.kTeal, r.kCyan,
             r.kAzure] 
  
  # holder to determine which RIVET analysis an AO came from
  rivetAnalyses=[]

  # holder for the plotting info for each AO
  plotInfoDict={}
  
  # Get the nominal variation, with respect to which the ratios are made
  fnIndex=-1
  nominalForRatio=None
  for fn in fileNames:
     if nominalName in fn: 
        nominalForRatio=copy.deepcopy(readFromFile(fn.split(":")[0]))
  if nominalForRatio is None:
    print("WARNING, could not identify nominal ! exit()")
    exit(1)

  # Now collect the AOs for each variation/filename, in the order they were specified 
  for fn in fileNames:
   fnIndex+=1
   aosThisFN= readFromFile(fn.split(":")[0],regexFilter, regexVeto) 
   for aoName in aosThisFN.keys():
     if "superAO" in aoName : continue
     aoNameNoRef=aoName.replace("/REF","")
     ao = aosThisFN[aoName]
     if not aoName in aos.keys(): 
       aos[aoNameNoRef]={}
       aos_ratio[aoNameNoRef]={}

     # We are not interested in the error bands for alternative PDFs
     setYErrorsToZero=False
     if "altPDF" in fn: 
       setYErrorsToZero=True
     
     # fill the holding dicts with TGraphs 
     #aos[aoName][fn]      = arrayDictToTGraph(ao,colorList[fnIndex],False, setYErrorsToZero)
     #aos_ratio[aoName][fn]= arrayDictToTGraph(ao,colorList[fnIndex],False, setYErrorsToZero, nominalForRatio[aoName])
     nfr = ""
     if aoName in nominalForRatio.keys():
       nfr = nominalForRatio[aoName] 
     elif "/REF"+aoName in nominalForRatio.keys():
       nfr = nominalForRatio["/REF"+aoName] 
     aos[aoNameNoRef][fn]      = arrayDictToTGraph(ao,False, setYErrorsToZero)
     aos_ratio[aoNameNoRef][fn]= arrayDictToTGraph(ao,False, setYErrorsToZero, nfr)
     
     # this is the best guess as to where the reference data might be !
     dataDir=os.environ["SYSTTOOLSPATH"]+"/data/PMGSystematicsTools"
     rivetAnalysis=aoName.split("/")[1] if len(aoName.split("/"))>1 else ""
     pathInRivetEnv="%s/Rivet/%s.yoda"%(RIVET_ANALYSIS_PATH ,rivetAnalysis)
     # for the first file, also get the .plot which will be needed to format
     # the output plot. Also look for any reference data!
     if fnIndex==len(fileNames)-1 and not 'Data' in nominalName:
       # now try to get the style .plot file and save the formatting info
       refDataPath=None
       if os.path.isfile(pathInRivetEnv):
          refDataPath=pathInRivetEnv
       elif os.path.isfile("%s.yoda"%rivetAnalysis):
          refDataPath="%s.yoda"%rivetAnalysis
       elif os.path.isfile("%s/%s.yoda"%(dataDir,rivetAnalysis)):
          refDataPath="%s/%s.yoda"%(dataDir,rivetAnalysis)
       else: 
         pass
             
       #if reference data was found, also produce TGraphs for that 
       # and ratio wrt Nominal
       theAOs = readFromFile(refDataPath,regexFilter, regexVeto)
       theAO=None
       if "/REF"+aoNameNoRef in theAOs.keys() or aoNameNoRef in theAOs.keys():
         if "/REF"+aoNameNoRef in theAOs.keys():
           theAO=theAOs["/REF"+aoNameNoRef]
         elif aoNameNoRef in theAOs.keys() :
           theAO=theAOs[aoNameNoRef]
         aos[aoNameNoRef][rivetAnalysis]= arrayDictToTGraph(
             theAO,
             #r.kBlack,
             True,
             )
         aos_ratio[aoNameNoRef][rivetAnalysis]= arrayDictToTGraph(
             theAO,
             #r.kBlack, #colour
             True, #isData
             False, #setYErrorsToZero
             nfr #divide by this nominal
             )
       else: 
         print("[WARNING] could not find AO with name " , aoNameNoRef , " or ", "/REF"+aoNameNoRef, " in REF file ",refDataPath )
         print(" Perhaps due to a version mis-match where the AO names were changed... skip this data file for now")
         

     if plotInfo is not None:
        plotInfoDict[aoNameNoRef]=getPlotInfo(aoNameNoRef, plotInfo)
     elif os.path.isfile(pathInRivetEnv.replace("yoda","plot")):
        plotInfoDict[aoNameNoRef]=getPlotInfo(aoNameNoRef, pathInRivetEnv.replace("yoda","plot"))
     elif os.path.isfile("%s.plot"%rivetAnalysis):
        plotInfoDict[aoNameNoRef]=getPlotInfo(aoNameNoRef,"%s.plot"%rivetAnalysis)
     elif os.path.isfile("%s/%s.plot"%(dataDir,rivetAnalysis)):
        plotInfoDict[aoNameNoRef]=getPlotInfo(aoNameNoRef,"%s/%s.plot"%(dataDir,rivetAnalysis))
     else: 
       pass
       #print("[WARNING] Could not locate plot info file (%s.plot) in either $PWD or $RIVET_ANALYSIS_PATH" % rivetAnalysis)
       #print("[WARNING] Will look for it recursively in $SYSTTOOLSPATH. Here is what was found:"
       #os.system("find $SYSTTOOLSPATH/. | grep %s.plot$ > tmp.find" % rivetAnalysis)
       #os.system("cat tmp.find")
       #lines=open("tmp.find",'r').readlines()
       #if len(lines)>0:
       #   print("[WARNING] Using this file: ", lines[0].strip())
       #   print("[WARNING] Copy it to your working directory next time to speed things up."
       #   plotInfoDict[aoNameNoRef]=getPlotInfo(aoNameNoRef,lines[0].strip())
       #else:
       #  print("[WARNING] Could not locate plot info file (%s.plot) in $SYSTTOOLSPATH either... " % rivetAnalysis)
       #  print("[WARNING] Formatting of your plots will be affected: missing labels, titles, axes.." 
  for k, v in plotInfoDict.items():
    pass
  #exit(1)
  # now loop through the AOs, and plot each variation
  #print(aos)
  #exit(1)
  aoNames = []
  for aoName in aos.keys(): 
    if 'RAW' in aoName: continue
    aoNames+=[aoName]

  for aoName in aoNames:
    print("[INFO] processing ao %d/%d : %s  "%(aoNames.index(aoName), len(aoNames), aoName))

    aoNameSafe=aoName.replace("/","__")
    
    # place the legend and format
    nFilesToProcess=len(fileNames)
    leg = r.TLegend(0.17,0.9-0.1*((2+nFilesToProcess)/2),0.88,0.80)
    #leg = r.TLegend(0.2,0.6,0.89,0.80)
    leg.SetLineWidth(0)
    leg.SetTextFont(43)
    leg.SetTextSize(13)
    leg.SetNColumns(2)
    
    # TLatex style and prepare holder for all the TLatex instances to write
    latexes=[] # format wil be [alignment, text size (wrt pixel) angle, ,x,y, text] 
    latexes.append([11,17,0.,0.15,0.905,"#scale[1.2]{#bf{#it{ATLAS}}} Internal"])
    latexes.append([33,15,90.,0.005,0.35,safeRootLatex("Ratio to %s"%nominalName)])
    latexes.append([31,15,0.,0.9,0.905,aoName])
    lat=r.TLatex()
    lat.SetTextFont(43)
    lat.SetNDC()
    
    # Now prepare the canvas and pads for main plot and ratio plot
    tc = r.TCanvas(aoNameSafe,aoNameSafe,500,500)
    pad1 = r.TPad("pad1","pad1",0.0,0.30,1.0,0.97)
    pad2 = r.TPad("pad2","pad2",0.0,0.05,1.0,0.35)
    r.SetOwnership(pad1,False)
    r.SetOwnership(pad2,False)
    pad1.SetLeftMargin(0.15)
    pad2.SetLeftMargin(0.15)
    pad1.SetBottomMargin(0.09)
    pad2.SetTopMargin(0.0)
    pad2.SetBottomMargin(0.15)
    pad1.Draw()
    pad2.Draw()
    pad2.SetGridy()
    pad1.cd()
    pad1.SetLogy(0)
    
    # determine which variations are to be processed, and add the ref data too
    # if we have it
    rivetAnalysis=aoName.split("/")[1] if len(aoName.split("/"))>1 else ""
    averageErrorSize={}
    for fn in fileNames:
      averageErrorSize[fn]=getAverageUncertaintySizePerBin(fn.split(":")[0], regexFilter=".*%s"%aoName)
    
    #order by size of uncertainty
    fileNamesToProcess=[]
    for k,v in sorted(averageErrorSize.items(), key=lambda x:x[1], reverse=True):
      fileNamesToProcess+=[k]
    
    if rivetAnalysis in aos[aoName].keys():
       fileNamesToProcess+=[rivetAnalysis]
    
    # now loop over the systematics and plot them
    fnIndex=-1
    for fn in fileNamesToProcess:
      fnIndex+=1
      
      # prepare leged entry for this systematic
      # legend is usually [<label>] <PDF Set Name>, or other relevant info
      
      if ":" in fn and not '[Data]' in fn: 
        isData=False
        legendName="%s"%fn.split(":")[1].replace("'","")
      else:
        isData=True
        legendName="[Data]"
      if 'Nominal' in fn:
          legendName+= " MC Stat." 
      if 'altPDF' in fn:
        aos[aoName][fn].SetLineStyle(2)
        aos_ratio[aoName][fn].SetLineStyle(2)
      if ("PDF" in fn) and (not 'Nominal' in fn):
          pdfstring=""
          if len (re.findall(r'PDF\d+',fn)) >0:
            pdfstring=re.findall(r'PDF\d+',fn)[0].replace("PDF","")
          if len(pdfstring)>2:
            pdfsetName= lookupPDFSetName(int(pdfstring))
            legendName+= " "+pdfsetName.partition("_")[0]
          legendName= (legendName[:20] + '...') if len(legendName) > 20 else legendName
      if 'Total Uncertainty' in fn:
        aos[aoName][fn].SetFillStyle(3004)
        aos[aoName][fn].SetFillColorAlpha(r.kBlack,0.5)
        aos[aoName][fn].SetLineColor(r.kBlack)
        aos_ratio[aoName][fn].SetFillStyle(3004)
        aos_ratio[aoName][fn].SetFillColorAlpha(r.kBlack,0.5)
        aos_ratio[aoName][fn].SetLineColor(r.kBlack)
      elif legendName!="[Data]":
        aos[aoName][fn].SetFillColorAlpha(colorList[fnIndex],0.5)
        aos[aoName][fn].SetLineColor(colorList[fnIndex])
        aos_ratio[aoName][fn].SetFillColorAlpha(colorList[fnIndex],0.5)
        aos_ratio[aoName][fn].SetLineColor(colorList[fnIndex])
      else:
        aos[aoName][fn].SetMarkerSize(1)
        aos[aoName][fn].SetMarkerStyle(20)
        aos[aoName][fn].SetLineWidth(3)
        aos_ratio[aoName][fn].SetMarkerSize(1)
        aos_ratio[aoName][fn].SetMarkerStyle(20)
        aos_ratio[aoName][fn].SetLineWidth(3)
      
      # for the first systematic (usually Total Uncertainty) we need to apply
      # the .plot formatting directives, and make a dummy histo to Draw on
      # with nice axis ranges
      oldRatioZoom=ratioZoom
      if (fnIndex==0):
        if not aoName in  plotInfoDict.keys():
          plotInfoDict[aoName]={}
        if not 'LogY' in plotInfoDict[aoName].keys(): 
          plotInfoDict[aoName]['LogY']=0
          pad1.SetLogy(0)
        else:
          pad1.SetLogy(int(plotInfoDict[aoName]['LogY']))
        if not 'LogX' in plotInfoDict[aoName].keys(): 
          plotInfoDict[aoName]['LogX']=0
          pad1.SetLogx(0)
          pad2.SetLogx(0)
        else:
          pad1.SetLogx(int(plotInfoDict[aoName]['LogX']))
          pad2.SetLogx(int(plotInfoDict[aoName]['LogX']))
          
        # this is where the dummy TH1s are made, with nice axis ranges
        # and sensible Font Styles/Sizes
        XandYMinAndMax=[None,None,None,None]
        XandYMinAndMax_ratio=[None,None,None,None]
        
        if "XMin" in plotInfoDict[aoName].keys():
           XandYMinAndMax[0]=plotInfoDict[aoName]['XMin']
           XandYMinAndMax_ratio[0]=plotInfoDict[aoName]['XMin']

        if "XMax" in plotInfoDict[aoName].keys():
           XandYMinAndMax[1]=plotInfoDict[aoName]['XMax']
           XandYMinAndMax_ratio[1]=plotInfoDict[aoName]['XMax']


        if "YMin" in plotInfoDict[aoName].keys():
           XandYMinAndMax[2]=plotInfoDict[aoName]['YMin']
        if "YMax" in plotInfoDict[aoName].keys():
           XandYMinAndMax[3]=plotInfoDict[aoName]['YMax']

        if "RatioPlotYMin" in plotInfoDict[aoName].keys():
          if ratioZoom is None: ratioZoom=[0.,2.]
          ratioZoom[0]=float(plotInfoDict[aoName]['RatioPlotYMin'])
        if "RatioPlotYMax" in plotInfoDict[aoName].keys():
          if ratioZoom is None: ratioZoom=[0.,2.]
          ratioZoom[1]=float(plotInfoDict[aoName]['RatioPlotYMax'])
        

        dummyHisto      =makeDummyHisto(aos[aoName][fn],int(plotInfoDict[aoName]['LogY']) , XandYMinAndMax)
        dummyHisto_ratio=makeDummyHisto(aos_ratio[aoName][fn],False,XandYMinAndMax, ratioZoom , True)
        ratioZoom=oldRatioZoom
        pad1.cd()
        dummyHisto.Draw("")
        pad2.cd()
        dummyHisto_ratio.Draw("")
        dummyHisto.GetYaxis().SetLabelFont(43)
        dummyHisto.GetYaxis().SetLabelSize(15)
        dummyHisto.GetXaxis().SetLabelFont(43)
        dummyHisto.GetXaxis().SetLabelSize(0)
        dummyHisto_ratio.GetXaxis().SetLabelFont(43)
        dummyHisto_ratio.GetXaxis().SetLabelSize(15)
        dummyHisto_ratio.GetYaxis().SetLabelFont(43)
        dummyHisto_ratio.GetYaxis().SetLabelSize(15)
        
        # apply the formatting directives from the .plot file
        for k,v in plotInfoDict[aoName].items():
         if k=='XTwosidedTicks': 
           pad1.SetTickx(int(v))
           pad2.SetTickx(int(v))
         if k=='YTwosidedTicks': 
           pad1.SetTicky(int(v))
           pad2.SetTicky(int(v))
         if k=='XCustomMajorTicks':
           for i in range(1,dummyHisto.GetNbinsX()+1):
               dummyHisto.GetXaxis().SetBinLabel(i,"")
               dummyHisto_ratio.GetXaxis().SetBinLabel(i,safeRootLatex(v.split()[2*i-1]))
         if k=='Title': 
           latexes.append([13,15,0.,0.2,0.88,safeRootLatex(v)])
         if k=='XLabel':                  
           latexes.append([33,15,0. ,0.9 ,0.05,safeRootLatex(v)])
         if k=='YLabel':                  
           latexes.append([33,15,90.,0.005,0.9,safeRootLatex(v)])
        
        # then draw the TGraph, and add entry to the legend
        pad1.cd()
        if isData:   
          # then draw the TGraph, and add entry to the legend
          pad1.cd()
          aos[aoName][fn].Draw("pe")
          pad2.cd()
          aos_ratio[aoName][fn].Draw("pe")
          leg.AddEntry(aos[aoName][fn],legendName,"pe")
        elif aos[aoName][fn].GetN()>1:
          aos[aoName][fn].Draw("l3")
          pad2.cd()
          aos_ratio[aoName][fn].Draw("l3")
          leg.AddEntry(aos[aoName][fn],legendName,"lf")
        else:
          aos[aoName][fn].Draw("e")
          aos[aoName][fn].Draw("e2")
          pad2.cd()
          aos_ratio[aoName][fn].Draw("e")
          aos_ratio[aoName][fn].Draw("e2")
          leg.AddEntry(aos[aoName][fn],legendName,"lf")
      
      elif isData:   
        # then draw the TGraph, and add entry to the legend
        pad1.cd()
        aos[aoName][fn].Draw("pe")
        pad2.cd()
        aos_ratio[aoName][fn].Draw("pe")
        leg.AddEntry(aos[aoName][fn],legendName,"pe")
      
      else:          
        # then draw the TGraph, and add entry to the legend
        pad1.cd()
        if aos[aoName][fn].GetN()>1:
          aos[aoName][fn].Draw("l3")
          pad2.cd()
          aos_ratio[aoName][fn].Draw("l3")
        else:
          aos[aoName][fn].Draw("e")
          aos[aoName][fn].Draw("e2")
          pad2.cd()
          aos_ratio[aoName][fn].Draw("e")
          aos_ratio[aoName][fn].Draw("e2")
        legendMarker="lf"
        if 'altPDF' in fn: legendMarker="l"
        leg.AddEntry(aos[aoName][fn],legendName,legendMarker)
    
    # finally, draw the TLatex instances
    pad1.cd()
    leg.Draw()
    tc.cd()
    for latex in latexes:
      lat.SetTextAlign(latex[0])
      lat.SetTextSize(latex[1])
      lat.SetTextAngle(latex[2])
      lat.DrawLatex(latex[3],latex[4],latex[5])
      #lat.DrawMathText(latex[3],latex[4],latex[5])
    
    #now print(the canvas and move to next AO)
    tc.Print("%s/%s%s.pdf"%(outdir, label,aoNameSafe))
    tc.Print("%s/%s%s.png"%(outdir, label,aoNameSafe))
    outputPlots+=["%s/%s%s.pdf"%(outdir, label, aoNameSafe)]
  return outputPlots

def makeSystematicsPlotsWithRIVET(mergedSystDict, plotsDir, nominalName="Nominal", ratioZoom=None,regexFilter=None, regexVeto=None, label="", plotInfo=None, normalize=False):
  """
  `mergedSystDict` dict{String,String} (format is as follows {N!label:filename},
  where N is the order in which to plot the variations, labels is what to put on
  the legend, and filename is the name of the YODA/ROOT file to use as input)  
  `return` None   

  Make some ugly plots using modified rivet make-plots. I think they are kinda 
  ugly and would suggest using makeSystematicsPlotsWithROOT instead.
  """
  print("[INFO] Compare histos")
  fileNames=mergedSystDict.keys()
  print("[INFO] fileNames", fileNames)
  fPlot=open("plot.sh","w")
  colors=["GREEN","PURPLE","YELLOW","RED","GREY","BLUE","ORANGE"]
  cc=0
  plotText=""
  plotText+='MBLUE="blue!40!white" ; '
  plotText+='MGREEN="green!40!white" ; '
  plotText+='MRED="red!40!white" ; '
  plotText+='DBLUE="blue!60!white" ; '
  plotText+='DGREEN="green!60!white" ; '
  plotText+='DRED="red!60!white" ; '
  plotText+='MYELLOW="yellow!40!white" ; '
  plotText+='DYELLOW="yellow!60!white" ; '
  plotText+='MORANGE="orange!40!white" ; '
  plotText+='DORANGE="orange!60!white" ; '
  plotText+='MGREY="black!60!white" ; '
  plotText+='DGREY="black!40!white" ; '
  plotText+='MPURPLE="purple!60!white" ; '
  plotText+='DPURPLE="purple!40!white" ; '
  if normalize: 
    plotText+='COMMON_TAGS="RatioPlotSameStyle=1:ErrorBars=0:ErrorBands=1:ErrorBandOpacity=0.3:NormalizeToIntegral=1" ; '
    plotText+=" rivet-mkhtml --errs "
  else: 
    plotText+='COMMON_TAGS="RatioPlotSameStyle=1:ErrorBars=0:ErrorBands=1:ErrorBandOpacity=0.3"; '
    plotText+=" rivet-mkhtml --reftitle=Data --errs "
  for kn, lab in mergedSystDict.items() :
  #for kn, lab in sorted(mergedSystDict.items(), key=lambda k: k[1]) :
    if "Nominal" in lab:
      plotText+=' %s:RatioPlotSameStyle=1:ErrorBars=1:LineColor=black:"Title=%s" '%(kn,lab)
    elif "PDF_ME" in lab:
      col="RED"
      plotText+=' %s:$COMMON_TAGS:ErrorBandColor=$M%s:LineColor=%s:"Title=%s" '%(kn, col,col.lower(),lab)
    elif "scale_ME" in lab:
      col="ORANGE"
      plotText+=' %s:$COMMON_TAGS:ErrorBandColor=$D%s:LineColor=%s:"Title=%s" '%(kn, col,col.lower(),lab)
    elif "alphaS" in lab:
      col="BLUE"
      plotText+=' %s:$COMMON_TAGS:ErrorBandColor=$D%s:LineColor=%s:"Title=%s" '%(kn, col,col.lower(),lab)
    elif "altPDF" in lab:
      col=colors[cc%len(colors)]
      cc+=1
      plotText+=' %s:LineStyle=dashed:RatioPlotSameStyle=1:ErrorBars=0:LineColor=%s:"Title=%s" '%(kn, col.lower(),lab)
    elif "Total" in lab:
      col="GREY"
      plotText+=' %s:$COMMON_TAGS:ErrorBandStyle=hlines:ErrorBandColor=$D%s:LineColor=$M%s:"Title=%s" '%(kn, col,col,lab)
    else:
      col=colors[cc%len(colors)]
      cc+=1
      plotText+=' %s:$COMMON_TAGS:ErrorBandStyle=hlines:ErrorBandColor=$D%s:LineColor=%s:"Title=%s" '%(kn, col,col.lower(),lab)
  plotText='%s -o %s \n'% (plotText, "tmp")
    
  os.system("mkdir -p tmp")
  os.system("mkdir -p %s" % (plotsDir))
  os.system(plotText)
  res=[]
  for dirname in os.listdir("tmp"):
     ananame= dirname
     dirname= "tmp/"+dirname
     if os.path.isdir(dirname):
       for f in os.listdir(dirname):
         if not "pdf" in f : continue
         fpng = f.replace("pdf","png")
         fnew=ananame+"__"+f
         fnewpng=ananame+"__"+fpng
         f=dirname+"/"+f
         res+=["%s/%s"%(plotsDir,fnew)]
         fpng=dirname+"/"+fpng
         os.system("convert -density 150 %s %s "%(f,fpng))
         os.system("mv %s %s/%s"%(f,plotsDir,fnew ))
         os.system("mv %s %s/%s"%(fpng,plotsDir, fnewpng))
  os.system("rm -r tmp")
  print("your plots are here :", plotsDir)
  return res
   

def getCombinationRecipe(systWeights,  combinationRecipeFile=None, combinationRecipeName=None,):
  """
  `systWeights` list(String) (list of weight types which are avaliable
  for a given dsid, to identify the correct combination recipe)  
  `combinationRecipeName` String [optional] (specify if you want to use a 
  specific comboination uncertainty, otherwise, it will try to auto-
  determine frim the systWeights)
  `combinationRecipeFile` String [optional] (specify if the target file is in
  a different location to $SYSTTOOLSPATH/data/Syst_Database.yaml
  `return` String,dict{components,formula}  

  Each sample type should have a unique set of weight types/names which allow us 
  to identify in the Syst_Database.yaml what the correct combination recipe is.
  """
  if len(systWeights)==1:
    #if there is only one weight, this is not a systematics sample anyway, so skip
    # this and return a dummy dictionary
    print("[WARNING] the provided systWeights only contain one component, assuming this is nominal and moving on")
    return {},{'combination':systWeights[0]}
  else:
    dataBaseDir=os.environ["SYSTTOOLSPATH"]
    if combinationRecipeFile is not None:
      data=yaml.load(open(combinationRecipeFile,'r'))
    else:
      data=yaml.load(open('%s/data/PMGSystematicsTools/Syst_Database.yaml'%dataBaseDir,'r'))
    for recipe, details in data.items():
      if combinationRecipeName is not None:
        if recipe==combinationRecipeName:
          if set(details['components']).issubset(set(systWeights)):
             return recipe, details
          else: 
             print("[ERROR] requested combination recipe", combinationRecipeName, 'uses weights ' , details['components'], 'which are not all present in ' ,systWeights )
             exit(1)
             

      if sorted(details['components']) == sorted(systWeights):
        return recipe, details
    
    print("[WARNING] could not find systematics recipe for samples with the following ME Weights", systWeights )
    return None, None


def combineAllVariations(weightList, indir, outdir, regexFilter=None, regexVeto=None, combinationRecipe=None, returnOnlyVariationsInComination=True, schema="!INDIR/!INFILE.yoda:!AONAME[!WEIGHTNAME]", orderedWeights=None, inFile=None  ):
  """
  `weightList` dict in output format of the readDatabase.getWeight() tool.
  See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PmgSystTool
  for more info.  
  `indir` String (input dir, where to find samples for each weight (merged
  across jobs/DSIDs if needed)  
  `outdir` String (output dir, where to write the output files where the
  individual weights are combined into systematic variations)  
  `regexFilter` String [optional] (AOs whose names match regex are processed)  
  `regexVeto` String [optional](AOs whose names match regex are NOT processed) 
  `combinationRecipe` String [optional] (if you want to use a specific 
  combination recipe, specify it here. If None, then the code will try to
  auto-determine the correct recipe from the $SYSTTOOLSPATH/data/Syst_Database.yaml.
  supports String:String where the first part is the yaml file containing the recipe)
  `returnOnlyVariationsInComination` Bool [optional] (by default the code will return 
  only a list of files for variations which take part in the combination. Set to False
  to return all possible variations)
  `schema` String [optional] (tells the code how the naming convention for histograms is  
  structured within the input file.)
  `schema` List[weight names] [optional] (if the `!WEIGHTINDEX!` keyword is used in `schema`  
  this option is needed so the code can convert from weight name to index)
  `inFile` String [optional] (needed if the `!INFILE!` keyword is used in `schema`)
  `return` dict{String,String} (format is as follows {N!label:filename},
  where N is the order in which to plot the variations, labels is what to put on
  the legend, and filename is the name of the YODA/ROOT file to use as input)  

  Using the information provided by the readDatabase tool (see 
  https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PmgSystTool),
  combine individual YODA/ROOT files for each Matrix Element weights into
  individual YODA/ROOT files for each systematic uncertainty.
  """
  os.system("mkdir -p %s" %outdir)
  
  # various holders for weights, labels, filenames
  weightsToProcess=[]
  allWeights=[]
  labels={}
  fOut={}
  averageErrorSize={}
  weightList=copy.deepcopy(weightList)

  # find the overall nominal file, and use the weight names to figure out the
  # corresponding yoda filenames, and fill holders
  NominalFile=""
  for k,v in weightList.items():
    weights=[]
    isRoot=False if not 'root' in schema  else 'True'
    for oldWeight in v['weights']:
      #replace weight names by the path of the YODA files
      fileName=schema.replace("!NSFWEIGHTNAME",oldWeight)
      fileName=fileName.replace("!WEIGHTNAME",safeFileName(oldWeight,removeExtension=True))
      fileName=fileName.replace("!INDIR",indir)
      if orderedWeights is not None: fileName=fileName.replace("!WEIGHTINDEX","%d"%orderedWeights.index(oldWeight))
      if inFile is not None: 
        fileName=fileName.replace("!INFILE",inFile)
        fileName=fileName.replace(".root.root",".root")
        fileName=fileName.replace(".yoda.yoda",".yoda")
      if( os.path.isfile(fileName.split(":")[0])):
        weights+=[fileName]
        fOut[oldWeight]=fileName
      else: 
        print("[ERROR] couldn't find file of name ", fileName, " with either .root or .yoda extension!")
        exit(1)
    v['weights']=weights
    if v['type']=='Nominal' : 
      NominalFile=weights[0]
      v['nominal']=NominalFile
      weightsToProcess+=[k]
    elif v['combination']=='none' : pass
    else: 
      fileName=schema.replace("!NSFWEIGHTNAME",v['nominal'])
      fileName=fileName.replace("!WEIGHTNAME",safeFileName(v['nominal'],removeExtension=True))
      fileName=fileName.replace("!INDIR",indir)
      if orderedWeights is not None: fileName=fileName.replace("!WEIGHTINDEX","%d"%orderedWeights.index(v['nominal']))
      if inFile is not None: 
        fileName=fileName.replace("!INFILE",inFile)
        fileName=fileName.replace(".root.root",".root")
        fileName=fileName.replace(".yoda.yoda",".yoda")
      if( os.path.isfile(fileName.split(":")[0])):
        v['nominal']=fileName
      else:
        print("[ERROR] couldn't find file of name ", fileName, " with either .root or .yoda extension!")
        exit(1)
      weightsToProcess+=[k]
    allWeights+=weights
  
  # get the Total uncertainty combination recipe and formula
  combinationRecipeFile, combinationRecipeName =None, combinationRecipe
  if combinationRecipe:
    if ":" in combinationRecipe :
      combinationRecipeFile, combinationRecipeName = combinationRecipeName.split(":")
  recipe , recipeDetails = getCombinationRecipe(weightList.keys(), combinationRecipeFile, combinationRecipeName)
  if recipe is not None: formula= recipeDetails['combination']
  
  # for each systematic, make a new output file, process the 
  # variation and check how large it's effect is over all AOs
  # so that they can be sorted by size for plotting
  for syst in weightsToProcess:
     if isRoot: fOut[syst]= '%s/%s.root' % (outdir, syst)
     else: fOut[syst]= '%s/%s.yoda' % (outdir, syst)
     labels[syst]=weightList[syst]['type']
     #this is the function that does the hard work!
     print("Doing "+syst+"with weights", weightList[syst])
     combineVariation(syst, weightList[syst], fOut[syst], regexFilter, regexVeto)
     averageErrorSize[syst]=getAverageUncertaintySizePerBin(fOut[syst], regexFilter, regexVeto)
     os.system("ls %s"%fOut[syst])
  
  # in Syst_Database, one may define a number of 'user-defined' intermediary systematics 
  # which need to be constructed according to some formula before the total uncertainty
  # is evaluated. So let's do those first
  if recipe is not None:
    if 'user_defined_components' in recipeDetails.keys():
     for syst,details in recipeDetails['user_defined_components'].items():
       if isRoot: fOut[syst]= '%s/%s.root' % (outdir, syst)
       else: fOut[syst]= '%s/%s.yoda' % (outdir, syst)
       labels[syst]=details['type']
       
       #build a dummy 'weightList' object
       combinationInfo={}
       combinationInfo['type']=details['type']
       combinationInfo['nominal']=NominalFile
       combinationInfo['weights']=[fOut[i] for i in getFormulaComponents(details['combination'])]
       combinationInfo['combination']="customFunction"
       combinationInfo['function']=details['combination']
       
       #build the systematic according to the custom Formula and then check it's size
       combineVariation(syst,  combinationInfo, fOut[syst], regexFilter, regexVeto)
       averageErrorSize[syst]=getAverageUncertaintySizePerBin(fOut[syst], regexFilter, regexVeto)
       os.system("ls %s"%fOut[syst])

    # now we construct the overall uncertainty according to the recipe
    print("[INFO] combining uncertainties using the following recipe:", recipe)
    print("[INFO] which uses formula: ", formula)
  
    #build a dummy 'weightList' object
    combinationInfo={}
    combinationInfo['type']='All'
    combinationInfo['nominal']=NominalFile
    combinationInfo['weights']=fOut.values()
    combinationInfo['combination']="customFunction"
    combinationInfo['function']=formula
    
    #build the total uncertainty according to the custom Formula and then check it's size
    fOut['all']= '%s/all.yoda' % outdir
    if isRoot: fOut['all']= '%s/all.root' % outdir
    else: fOut['all']= '%s/all.yoda' % outdir
    combineVariation('All', combinationInfo, fOut['all'], regexFilter, regexVeto)
    averageErrorSize['all']=getAverageUncertaintySizePerBin(fOut['all'], regexFilter, regexVeto)*10
    os.system("ls %s"%fOut[syst])

  # now we want to order the output files by size of the corresponding
  # uncertainties, so they look nice when plotted.
  #result={}
  result=OrderedDict()
  counter=0
  formulaComponents=averageErrorSize.keys()
  if recipe is not None: formulaComponents = getFormulaComponents(formula)
  for k,v in sorted(averageErrorSize.items(), key=lambda x:x[1], reverse=True):
      if k=='all':
        #result['%d![%s]' % (counter, "Total Uncertainty") ]= fOut[k]
        result[fOut[k]]="[Total Uncertainty]"
      else:
        isComponent=False
        for component in formulaComponents:
          if component == k: isComponent=True
        if (not isComponent) and returnOnlyVariationsInComination : continue
        #result['%d![%s]' % (counter, labels[k]) ]= fOut[k]
        result[fOut[k]]='[%s]' %  labels[k]
      counter+=1
  # finally, return the odered files
  return result


def extractTarballsFromDirectory(fulldir, force=False, verbose=False, rootOrYoda='yoda', notTGZ=False):
  """
  `fulldir` String (directory containing tarballs to unpack)  
  `force` Bool [optional] (re-do untarring even if already done)  
  `verbose` Bool [optional] (Print more output if set to True)  
  `rootOrYoda` String [optional] (specify file type of tarred objects)   

  `return` list of [sample,newfn,filepath]  

  This function goes through a directpry and unpacks all the tarballs it finds.
  By default if a matching unpacked file has been found,
  a tarball is not needlessly re-unpacked unless option `force` is used.
  """
  res=[]
  if fulldir[-1]=="/": fulldir=fulldir[:-1]
  #print(fulldir)
  sample=fulldir.split("/")[-1]
  counter=-1
  os.system("cd %s"  % fulldir)
  print("\r[INFO] Processing ", sample)
  nSubSamples = len([name for name in os.listdir(fulldir)])
  sys.stdout.flush()
  if force:
    os.system("rm %s/merged/*" % fulldir)
    os.system("rm %s/merged_tmp/*" % fulldir)
  if len(os.listdir(fulldir))==0:
    print("[ERROR] can't run over empty directory: ", fulldir)
    exit(1)
  for subsample in os.listdir(fulldir):
     #print(subsample)
     if 'root' in subsample : 
       newfulldir= fulldir+"/"+subsample.replace(".root.tgz",".untarred")
     else  :  
       newfulldir= fulldir+"/"+subsample.replace(".yoda.tgz",".untarred").replace(".yoda.gz.tgz",".untarred")
     if (not subsample.endswith('tgz') and not notTGZ ): continue
     if  ("untarred" in subsample and not force): continue
     if  ("merged" in subsample and not force): continue
     #if  ("tgz" in subsample and not os.path.isdir(newfulldir)): continue
     counter+=1
     if ('tgz' in  subsample) and not (os.path.isdir(newfulldir) and not notTGZ) :
       os.system("mkdir -p %s" % newfulldir)
       #if verbose: sys.stdout.write('\r Thread %d: untar job output %s (%d/%d)     '%(multiprocessing.dummy.current_process().ident, subsample,counter, nSubSamples))
       if verbose: sys.stdout.write('\r Unpacking job output %s (%d/%d)     '%(subsample,counter, nSubSamples))
       sys.stdout.flush()
       with tarfile.TarFile.open(fulldir+"/"+subsample, 'r:gz') as tar:
         fnames = tar.getnames()
         prefix=os.path.commonprefix(fnames)
         if len(fnames)==1: prefix=""
         if len(fnames)>0:
           tar.extractall(newfulldir)
           for fn in fnames:
             newfn=fn.replace(prefix, "")
             if not ".yoda.gz" in fn:
                 os.rename(newfulldir+"/"+fn,newfulldir+"/"+safeFileName(newfn) +"."+rootOrYoda)
             variationType=""
             res.append([fulldir,newfn,newfulldir+"/"+safeFileName(newfn,removeExtension=False)])
             #infiles.setdefault(sample,{}).setdefault(newfn,[]).append(newfulldir+"/"+safeFileName(newfn,removeExtension=False))
     elif (not notTGZ) : 
       for fn in os.listdir(newfulldir):
          #infiles.setdefault(sample,{}).setdefault(fn,[]).append(newfulldir+"/"+safeFileName(fn,removeExtension=False))
          res.append([fulldir,fn,newfulldir+"/"+safeFileName(fn,removeExtension=False)])
     else : 
       fn = os.path.basename(newfulldir)
       newfulldir = os.path.dirname(newfulldir)
       os.system("mkdir -p %s/%s"%(newfulldir,safeFileName(fn,removeExtension=True)))
       renamedfn='_Nominal.yoda'
       os.system("cp %s/%s %s/%s/%s" % (newfulldir, fn, newfulldir, safeFileName(fn,removeExtension=True),renamedfn))
       #infiles.setdefault(sample,{}).setdefault(fn,[]).append(newfulldir+"/"+safeFileName(fn,removeExtension=False))
       res.append([fulldir,renamedfn,newfulldir+"/"+safeFileName(fn,removeExtension=True)+"/"+renamedfn])
  print('\r[INFO] Done untar-ing %s' % sample)
  return res

def updateProgress(progress,key, message):
  """
  `progress` dict (a dictionary of the job labels:status)  
  `key` String (the job label to update)  
  `message` String (the status to update)  
  `return` Void  

  This is a helper function to update and print(a progress update)
  when multi-threading
  """
  progress[key]=message
  printProgress(progress)

def printProgress(progress):
  """
  `progress` dict (a dictionary of the job labels:status)  
  `return` Void  

  This is a helper function print(the progress update in multi-)
  threader processes
  """
  printString=""
  nChar=160
  counter=-1
  pend=0
  run=0
  done=0
  printString="--> Running with %d threads: "%progress['N']
  for k,v in sorted(progress.items(), key=lambda x : x[0]) :
    if v=='done': done+=1
    if v=='pend': pend+=1
    if v=='run':  run+=1
  printString+=" DONE=%d, RUN=%d, PENDING=%d " %  (done, run, pend)
  if len (printString) > nChar:
    printString=printString[:nChar]+"..."
  sys.stdout.write('\r %s' % printString)
  sys.stdout.flush()

def mergeInChunks(outfn, paths, progressDict=None, nFilesPerChunk=100, force=False, rootOrYoda='yoda'):
  """
  `outfn` String (The name of the output Yoda file you wish to produce, with full path)  
  `paths` list[String] (List of Yoda files to merge, with full paths)  
  `progressText` String [optional] (An extra string to print(at the end of the progress message)  )
  `nFilesPerChunk` int [optional] (How many files to do per chunk)  
  `force` Bool [optional] by default, if there is already a matching merged file, this function does nothing
  but `force` forces the merge again  
  `rootOrYoda` String [optional] (specify file type of objects to merge)   
  `return` None
  
  This function safely merges multiple yoda files in chunks of nFilesToProcess at a time (if too many, yodamerge can fail!)
  Recommended is nFilesPerChunk=100, but definitely less than 300.
  """
  fulldir=os.path.dirname(os.path.abspath(outfn))
  os.system("mkdir -p %s" % fulldir)
  os.system("mkdir -p %s/tmp" % fulldir)
  fnShort=os.path.basename(outfn)
  mergeTool="yodamerge_tmp -o "
  gzsuffix=""
  if 'root'==rootOrYoda:
    assert('root' in fnShort and not 'yoda' in fnShort)
    fnShort=fnShort.partition(".root")[0]
    mergeTool="hadd -f "
  else:
    assert(not 'root' in fnShort and  'yoda' in fnShort)
    fnShort=fnShort.partition(".yoda")[0]
    gzsuffix = fnShort.partition(".yoda")[2]
  if progressDict is not None:
    updateProgress(progressDict, fnShort, "run")
  if os.path.isfile(outfn) and not force:
     pass
  else:
    time.sleep(0.1*randint(1,50))
    #sys.stdout.write('\r[INFO] merging files of type %s (%s)                                    '%(fnShort,progressText))
    #sys.stdout.flush()
    filesToMerge=" "
    if len(paths)> nFilesPerChunk:
      for iChunk in range(0, len(paths), nFilesPerChunk):
         #if progressDict is not None:
         #  updateProgress(progressDict, multiprocessing.dummy.current_process(), fnShort+"chunk%d"%iChunk)
         #  printProgress(progressDict))
         chunkPaths=paths[iChunk:iChunk + nFilesPerChunk]
         chunkFilesToMerge = ' '.join(chunkPaths)
         chunkMergedFile="%s/tmp/%s.chunk%d.%s"%(fulldir,fnShort,iChunk,rootOrYoda + gzsuffix)
         #for p in chunkPaths:
         #  if not os.path.isfile(p): print("ERROR, issue with ", p )
         os.system("%s %s %s &> out1.%d.log "%(mergeTool,chunkMergedFile,chunkFilesToMerge,iChunk))
         filesToMerge+=chunkMergedFile+" "
    else :
         filesToMerge = ' '.join(paths)
         if 'yoda'==rootOrYoda: #this breaks for root files
           for p in paths:
             if not ".yoda.gz" in p:       # breaks for .yoda.gz files
               os.system("sed -i 's/|//g' %s"%p)
    os.system("%s %s %s &> out2.log "%(mergeTool,outfn,filesToMerge))
  if progressDict is not None:
    updateProgress(progressDict, fnShort, "done")
    #printProgress(progressDict))

def getCrossSectionCorrection(xsList, systFiles, nomFiles, rivetNormalised=False, applyKFactorCorrection=False, varWeightName="", nominalWeightName="",sumwHistName=""):
  """
  `xsList` list[Float] (list of nominal XS values for the subsamples)  
  `systFiles` list[String] (list of file names for the variated subsamples 
  (same order as xsList)  
  `nomFiles` list[String] (list of file names for the nominal subsamples 
  `rivetNormalised` Bool [optional] (whether or not to apply the total yield  
  correction for variation AOs, since by default rivet normalises by sum-of-weights  
  for that variation rather than sum-of-weight of the nominal)
  `applyKFactorCorrection` Bool [optional] (If the sample to merge has a k-factor, there  
  is an additional correction needed to avoid double counting the normalisation uncertainty)  
  `varWeightName` String [optional] (name of the variation to get weight correction for)  
  `nominalWeightName` String [optional] (name of the nominal variation to get weight correction with respect to)  
  (same order as xsList)  
  `sumwHistName` String [optional] (name of the TH1 with sum-of-weights in the root file output scenario)  

  This function takes a list of nominal and variated YODA/ROOT files, and a list of
  nominal cross-sections in the same order, to calculate the on-the-fly cross-
  section corrections needed for each systFile. For yoda files produced in the Rivet 2.x  
  series, a correction is needed to undo the fact that Rivet normalises systematics  
  by the varied sum of weights instead of the nominal sum of weights.  
  If using a k-factor, an additional correction is needed to avoid double-counting  
  the normalisation uncertainty. For details, see  
  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgSystematicUncertaintyRecipes#On_the_fly_systematic_variations
  """

  res=[]
  inclusive_xs_nom=0.
  inclusive_xs_syst=0.
  assert len(xsList)==len(nomFiles)
  assert len(xsList)==len(systFiles)
  
  for i in range(len(xsList)):
    nom=nomFiles[i]
    syst=systFiles[i]
    xs= xsList[i]
    inclusive_xs_nom+=xs
    if rivetNormalised : inclusive_xs_syst+=xs*weightCorrection(syst,nom, varWeightName=varWeightName, nominalWeightName=nominalWeightName, sumwHistName=sumwHistName)
    else: inclusive_xs_syst+=xs*1.
              
  for i in range(len(xsList)):
    nom=nomFiles[i]
    syst=systFiles[i]
    xs= xsList[i]
    weight=xs
    if rivetNormalised: weight *= weightCorrection(syst,nom,varWeightName=varWeightName, nominalWeightName=nominalWeightName, sumwHistName=sumwHistName)
    if applyKFactorCorrection: weight *= (inclusive_xs_nom/inclusive_xs_syst)
    res.append([syst,weight])
  
  return res
  

def main(argv):
  """
  This module can also be run as a standalone executable. 
  For info about the options try:
  systematicsTool.py -h

  This is useful for unpacking large GRID job outputs, as it will do all the 
  book-keeping, untarring of tarballs, merging across jobs, across subsamples,
  across samples weighted by DSID, and then finally processing the systematics
  and plotting them. Multithreading available.
  """
  parser = optparse.OptionParser( usage = "%prog [options]" )
  parser.add_option("-d","--directory", help="Directory containing the rucio downloads.", dest="directory", default="samples" )
  parser.add_option("-p","--process", help="Name of process to merge. Will only apply merge to directories containing this name. Also accepts the format regex:label, where the label is used to name the output directory, and the regex to select which directories to merge. Accepts comma separated list of regex:label pairs", dest="process", default="Zee" )
  parser.add_option("-g","--generator", help="Name of generator to merge. Will only apply merge to directories containing this name. Also accepts the format regex:label, where the label is used to name the output directory, and the regex to select which directories to merge. Accepts comma separated list of regex:label pairs", dest="generator", default="Sherpa" )
  parser.add_option("--force", help="By default, if a subsample has already been untarred and merged, it is not done again. This option forces the script to re-untar and re-merge. Slow, unless you use a lot of threads...", dest="force", default=False )
  parser.add_option("--plotsDir", help="Where to save the plots to? Default is <generator>_<process>_<label>/plots", dest="plotsDir", default=None)
  parser.add_option("-l","--label", help="An additional label for the output directory of the merge and as a prefix to the plots", dest="label", default="")
  parser.add_option("-f","--filter", help="Only process AOs matching this regex", dest="filter", default=".*")
  parser.add_option("-e","--exclude", help="Do NOT process AOs matching this regex", dest="exclude", default=None)
  parser.add_option("-j","--nThreads", help="Multithread some of the merging steps", dest="nThreads", default=4)
  parser.add_option("--noSyst", help="Ignore systematics, only merge/process/draw the nominal weight", dest="noSyst", default=False , action="store_true")
  parser.add_option("--notTGZ", help="Use if your input files were just yoda/root files not tarballs", dest="notTGZ", default=False , action="store_true")
  parser.add_option("--uxs","--userXS", help="When looking up cross-sections for each DSID, check the specified file first before defaulting to CVMFS values", dest="userXS", default=None)
  parser.add_option("--nw","--nominalWeight", help="Bypass the DSID_weights database, and use the specified name as the nominal. Only for us with --noSyst", dest="nominalWeight", default="")
  parser.add_option("--sp","--skipPlots", help="Do the merging of files, but just skip the plotting at the end", dest="skipPlots", default=False, action="store_true" )
  parser.add_option("--normalize", help="Normalize any output plots to unit integral", dest="normalize", default=False, action="store_true" )
  parser.add_option("--r2n","--rivet2Normalised", help="Are your histograms normalised by the sum of weights in each variation, as is done typically for Rivet2 analyses? specify 1/0 for True/False", dest="rivetNormalised", default=False)
  parser.add_option("--sw","--sumwHistName", help="Name of the TH1 that stores the sum-of-weights in the case of root file", dest="sumwHistName", default="__EVTCOUNT")
  parser.add_option("--schema", help="Tells the tool how to access the analysis objects from the inputs provided. Format is PathToFile:ObjectName. For example, if you have one YODA file per MC Weight, and each histogram is named the same in each input file, you schema would be !INDIR/!WEIGHTNAME.yoda:!AONAME. If you instead store in a single root file all the variations, where the weight name is appended to the histo name, your schema would be !INDIR/!INFILE.yoda:!AONAME[!WEIGHTNAME]. The available identifiers are: !INDIR (path to input dir), !INFILE (name of input file), !AONAME (analysis object name), !WEIGHTNAME (the MC weight name, assuming that safeFileName was applied to changes spaces to underscores etc...)", dest="schema", default='!INDIR/!INFILE.yoda:!AONAME[!WEIGHTNAME]')
  parser.add_option("--plotInfo", help="A rivet-style .plot file to tell the tool how to format your output plots. See data/ExclusivePlusInclusive.plot as an example. Also works for plots make from root files!", dest="plotInfo", default=None)
  #parser.add_option("-s","--skipSteps", help="If you don't need to re-do certain steps of the process, you can skip them with this comma-separated list. Possible steps to skip include: UNTAR, MERGEJOBS, MERGESAMPLES, COMBINEVARIATIONS, PLOT ", dest="skipSteps", default="")
  (opts, args) = parser.parse_args()
  

  r.gErrorIgnoreLevel = r. kError
  #opts.skipSteps=opts.skipSteps.split(",")
  # Loop through each process, 
  # and get hold of the corresponding regexes
  for process in opts.process.split(","):
   if ":" in process:
      tokens=process.split(":")
      processRegex=tokens[0]
      process=tokens[1]
   else :
      processRegex= process

    # Loop through each generator, 
    # and get hold of the corresponding regexes
   for generator in opts.generator.split(","):
    if ":" in generator:
      tokens=generator.split(":")
      generatorRegex=tokens[0]
      generator=tokens[1]
    else :
      generatorRegex= generator
    
    if opts.rivetNormalised is not None: 
      opts.rivetNormalised=bool(int(opts.rivetNormalised))
    
    # for backwards-compatibility with default rivet2-style outputs
    if opts.rivetNormalised is True: 
      if not "--schema" in sys.argv:
        opts.schema="!INDIR/!WEIGHTNAME.yoda:!AONAME"
    
    rootOrYoda='root' if '.root' in opts.schema else 'yoda'
    if "WEIGHT" in opts.schema.split(":")[0]:
      structure="OneFilePerVariation"
    else:
      structure="AllVariationsInFile"

    sumwHistName = opts.sumwHistName  
    
    # constrct the filter for generato/process pair
    thisFilter='.*%s.*%s.*' % (generatorRegex, processRegex)
    
    # construct output dir
    if opts.plotsDir is None:
      opts.plotsDir="%s_%s_%s/plots"% (process,generator,opts.label)
      os.system("mkdir -p %s"% opts.plotsDir )
    
         
     
    infiles={} # sample  : pdftype : [files]
    pwd=os.getcwd()
    jobsMergedFiles={}
    print("================================================================================")
    print("[INFO] Processing files which match this regex ", thisFilter)
    print("================================================================================")
    print()
    print()

    # unatr grid outputs, merged across jobs
    #TODO modularize this
    print("-----------------------------------------------------------------------")
    print("[INFO] Untar Grid outputs ") 
    print("------------------------------------------------------------------------")
    
    #opts.directory, thisFilter, opts.force
    #setup the untar jobs
    pool = ThreadPool(int(opts.nThreads))
    threadInputs=[]
    counter=-1
    untarredFiles={}
    samplesToProcess=[]
    fullDirToSample={}
    workingDir=opts.directory
    if not '/afs/' in workingDir: workingDir=pwd+"/"+opts.directory 
    # loop through samples and select those passing the regex
    for sample in os.listdir(workingDir):
      counter+=1
      fulldir  = workingDir+"/"+sample
      if not  re.search(thisFilter,sample): continue
      # needed?
      #if not  re.search('user.%s'%os.getlogin(),sample): continue
      if not  re.search('user.',sample): continue
      
      # for each each job start by extracting the tarball if not already done
      if not  os.path.isfile(fulldir):
        samplesToProcess+=[fulldir]
    for fulldir in samplesToProcess:
      threadInputs.append([fulldir,opts.force])
      untarredFiles[fulldir]={}
    if len(threadInputs)==0: 
      print("[ERROR] no directories in %s matches the requested regex! exit !"%workingDir)
      exit(1)
    
    def threadJobFunction(inputs):
      return extractTarballsFromDirectory(inputs[0],inputs[1], rootOrYoda=rootOrYoda, notTGZ=opts.notTGZ)

    results = pool.map(threadJobFunction, threadInputs)
    pool.close() 
    pool.join()
    for jobResult in results:
      for res in jobResult:
         if structure=="OneFilePerVariation":
            untarredFiles[res[0]].setdefault(res[1],[]).append(res[2])
         else:
            untarredFiles[res[0]].setdefault('AllVariations',[]).append(res[2])


    print()
    print()
    print("------------------------------------------------------------------------")
    print("[INFO] Merging across jobs for each DSID")
    print("------------------------------------------------------------------------")

    for fulldir in sorted(untarredFiles.keys()):
         
         print("[INFO] Merging files for jobs of ", os.path.basename(fulldir))

         infiles=untarredFiles[fulldir]
         nFileTypes = len(infiles.keys())
         if nFileTypes==0:
           print("[ERROR] 0 file types found in inputs! Exit!")
           print(untarredFiles)
           exit(1)
         # multi-threaded merge of files across jobs
         #fn, pathsToMerged, fulldir, progress
         def threadJobFunction(inputs):
             mergeInChunks(inputs[0], inputs[1], progressDict=inputs[2], rootOrYoda=rootOrYoda)
         
         #setup the threads and do merge
         pool = ThreadPool(int(opts.nThreads))
         threadInputs=[]
         counter=-1
         progressDict={}
         progressDict['N']=int(opts.nThreads)
         for fn,paths in infiles.items():
           counter+=1
           basename=safeFileName(os.path.basename(fn))
           progressDict[basename]='pend'
           outfn="%s/merged/%s.%s" % (fulldir, basename,rootOrYoda)
           sampleName=os.path.basename(fulldir)
           threadInputs.append([outfn,paths,progressDict])
           jobsMergedFiles.setdefault(basename,{})[sampleName]=outfn
         results = pool.map(threadJobFunction, threadInputs)
         pool.close() 
         pool.join()
         print('\r[INFO] Done Merging %s' % os.path.basename(fulldir))

    print()
    print()
    print("------------------------------------------------------------------------")
    print("[INFO] Get available MEweights from DSID database and make sure they are consistent!")
    print("------------------------------------------------------------------------")
    
    xsDict={}
    
    def threadJobFunction(inputs):
      fulldir=inputs[0]
      noSyst=inputs[1]
      xsDict=inputs[2]
      #dsid=fulldir.split("mc15_13TeV.")[-1].split(".")[0]
      dsid=re.findall("\.[0-9]{6,6}\.",fulldir)[0].replace(".","")
      print("\r[INFO] Looking up XS and MEWeights for DSID=",dsid)
      if rootOrYoda=='yoda' and getXSFromYODA(jobsMergedFiles.values()[0][os.path.basename(fulldir)]) != 1.0:
        print("\r[INFO] Histograms in YODA file of DSID=%s are already scaled by xsec. Set xsec to 1.0 to avoid double scaling!"%dsid)
        xsDict[dsid]= 1.0, 0, 0
      else:
        xsDict[dsid]=getXS(int(dsid), userFile=opts.userXS)
      availableWeights=None
      nominalWeight=None
      if not noSyst:
        if availableWeights is None:
           availableWeights=rDB.getWeights(dsid)[0]
           for k,v in availableWeights.items():
             if v['type']=='Nominal':
               nominalWeight=v['nominal']

      else:
        nominalWeight=jobsMergedFiles.keys()[0]
        if len(opts.nominalWeight)> 0: nominalWeight=opts.nominalWeight
        availableWeights={'_Nominal':
                           {'nominal_pdf': '-1', 'type': 'Nominal', 'nominal': '%s' %nominalWeight,
                              'weights': ['%s'% nominalWeight], 'combination': 'none'}}
      if xsDict[dsid][2]>0 :
        availableWeights["Normalization"]= {'nominal_pdf': '-1', 'type': 'Normalization', 'nominal': nominalWeight, 'weights': [nominalWeight], 'combination': 'norm', "value" : xsDict[dsid][2]}
      
      return [availableWeights,dsid]
    
    
    pool = ThreadPool(int(opts.nThreads))
    threadInputs=[]
    for fulldir in sorted(untarredFiles.keys()):
      threadInputs.append([fulldir,opts.noSyst,xsDict])
    
    results = pool.map(threadJobFunction, threadInputs)
    pool.close() 
    pool.join()
    
    print()
    print("[INFO] Consistentcy Checks:")
    availableWeights=None
    uniqueWeights=[]
    for dsid,xs in xsDict.items():
       if (xsDict[dsid][0]==-1):
         print("\r[INFO] ERROR: could not find Xs for DSID=%s. Exit." %( str(dsid)))
         exit(1)
       else:
         print("\r[INFO] DSID=%s has XS=%.6f pb (kFactor=%.2f, normUnvertainty=%.2f)" %( str(dsid), xsDict[dsid][0], xsDict[dsid][1], xsDict[dsid][2]) ," and found ME weights")
    for res in results:
      if availableWeights==None: 
         availableWeights=res[0]
         print("[INFO] DSID", res[1], " has weights", availableWeights.keys())

      elif(sorted(availableWeights.keys())==sorted(res[0].keys())):
         print("[INFO] DSID", res[1], " has weights consistent with the above")
      else:
         print("[ERROR] DSID", res[1], " has weights NOT consistent with the above!!")
         exit(1)
      
    for k,v in availableWeights.items():
     if "_Nominal" in k:
       nominalWeight=v
     for w in v['weights']:
       if w in uniqueWeights: continue
       else: uniqueWeights+=[w]

    if (len(uniqueWeights) ==0 and not opts.noSyst):
      print("[ERROR] Your sample DISD=", res[1], " has no weights... so it might be missing from the database!")
      print("[ERROR] Please contact lcorpe@cern.ch to get your sample added...")
      print("[ERROR] In the meantime, we will exit since no combination recipe is availale... you can still run with --noSyst")
      exit(1)

    print("UNIQUE WEIGHTS", uniqueWeights)

    
    print()
    print()
    print("------------------------------------------------------------------------")
    print("[INFO] Calculating on the fly corrections to XSs for each systematic")
    print("------------------------------------------------------------------------")
    
    if opts.rivetNormalised is None:
      if rootOrYoda=='yoda': opts.rivetNormalised=True
      else : opts.rivetNormalised=True



    allFilesToMerge={}
    
    
    def threadJobFunction(inputs):
      systName=safeFileName(inputs[0])
      progressDict=inputs[4]
      updateProgress(progressDict, systName, "run")
      #printProgress(progressDict))
      res = getCrossSectionCorrection(inputs[1],inputs[2],inputs[3], inputs[5], inputs[6], inputs[7], inputs[8])
      updateProgress(progressDict, systName, "done")
      #printProgress(progressDict))
      return systName, res
    
    pool = ThreadPool(int(opts.nThreads))
    threadInputs=[]
    
    progressDict={}
    applyKFactorCorrection=None
    progressDict['N']=int(opts.nThreads)
    #for syst,sample in jobsMergedFiles.items():
    for syst in uniqueWeights:
      if structure=="AllVariationsInFile":
        sample=jobsMergedFiles.values()[0]
      else:
        sample=jobsMergedFiles[safeFileName(syst)]
      systSampleFiles=[]
      xsList=[]
      nomSampleFiles=[]
      for sampleName, path  in sample.items():
        #dsid=sampleName.split("mc15_13TeV.")[-1].split(".")[0]
        dsid=re.findall("[0-9]{6,6}",sampleName)[0].replace(".","")
        xsList+=[xsDict[dsid][0]]
        if applyKFactorCorrection is None : applyKFactorCorrection=xsDict[dsid][1]
        elif applyKFactorCorrection!=xsDict[dsid][1]:
          print("[ERROR], cannot merge samples with inconsistent k-factors !")
          print(xsDict)
          exit(1)
        else: pass
        systSampleFiles+=[path]
        if structure=="AllVariationsInFile":
          nomSampleFiles+=[jobsMergedFiles.values()[0][sampleName]]
        else:
          nomSampleFiles+=[jobsMergedFiles[safeFileName(nominalWeight['nominal'])][sampleName]]
      
      if structure=="AllVariationsInFile":
        threadInputs+=[[syst,xsList,systSampleFiles,nomSampleFiles,progressDict,opts.rivetNormalised,applyKFactorCorrection, safeFileName(syst),safeFileName(nominalWeight['nominal'])]]
      else:
        threadInputs+=[[syst,xsList,systSampleFiles,nomSampleFiles,progressDict,opts.rivetNormalised,applyKFactorCorrection,"",""]]
      progressDict[syst]='pend'
    

    results = pool.map(threadJobFunction, threadInputs)
    
    pool.close() 
    pool.join()
    for jobResult in results:
      syst=jobResult[0]
      for systFile, weight in jobResult[1]:
        allFilesToMerge.setdefault(syst,[]).append(" %s:%f"%(systFile, weight))

    
    
    print()
    print("------------------------------------------------------------------------")
    print("[INFO] Merging files across DSIDs for each systematic")
    print("------------------------------------------------------------------------")
    
    def threadJobFunction(inputs):
      s=inputs[0]
      generator=inputs[1]
      process=inputs[2]
      progressText=inputs[3]
      updateProgress(progressDict, s, "run")
      #printProgress(progressDict))
      #sys.stdout.write('\r[INFO] Merging files for %s  %s                                                 '%(s,progressText))
      #sys.stdout.flush()
      os.system("mkdir -p %s_%s_%s/merged_sample"% (  process,generator,opts.label))
      #fileExists=os.path.isfile(mergedFile)
      if rootOrYoda=='root':   
        
        if structure=="AllVariationsInFile":
          mergedFile="%s_%s_%s/merged_sample/%s.%s" % ( process,generator,opts.label,safeFileName(s),rootOrYoda)
          thisFilter="%s__.*"%safeFileName(s)
          thisFindAndReplace="%s__:"%safeFileName(s)
          command="haddw -f %s -r %s %s %s &> out.txt  "%(thisFilter, thisFindAndReplace, mergedFile, " ".join(allFilesToMerge[safeFileName(s)]))
          os.system(command)
        
        if structure=="OneFilePerVariation":
          mergedFile="%s_%s_%s/merged_sample/%s.%s" % ( process,generator,opts.label,safeFileName(s),rootOrYoda)
          command="haddw  %s %s  &> out.txt "%(mergedFile," ".join(allFilesToMerge[s]))
          #print(command)
          os.system(command)
      else: 
        mergedFile="%s_%s_%s/merged_sample/%s.%s" % ( process,generator,opts.label,safeFileName(s),rootOrYoda)
        if structure=="AllVariationsInFile":
          if rivet_i_replacements(s)!="" and not opts.noSyst:
            command="yodamerge_tmp --add -o  %s %s -m '.*%s.*'  &> out.txt "%(mergedFile," ".join(allFilesToMerge[safeFileName(s)]), rivet_i_replacements(s))
            os.system(command)
            os.system("sed -i 's/\[%s\]//g' %s"%(rivet_i_replacements(s),mergedFile ))
          else:
            command="yodamerge_tmp --add -o  %s %s -M .*\\\[.*\\\].*  &> out.txt "%(mergedFile," ".join(allFilesToMerge[safeFileName(s)]))
            os.system(command)
            #os.system("sed -i 's/\[%s\]//g' %s"%(rivet_i_replacements(s),mergedFile ))
        if structure=="OneFilePerVariation":
          if len(allFilesToMerge[s])==1:
           sampleToMerge=allFilesToMerge[s][0].split(":")
           command="cp %s %s ; yodascale -c '.* %fx' -i %s  &> out.txt \n"%(sampleToMerge[0],mergedFile, float(sampleToMerge[1]), mergedFile)
          else:
           command="yodamerge_tmp --add -o  %s %s  &> out.txt "%(mergedFile," ".join(allFilesToMerge[s]))
          
          os.system(command)
    
      #print("yodamerge --add -o  %s %s  &> out.txt "%(mergedFile," ".join(allFilesToMerge[s]))))
      updateProgress(progressDict, s, "done")
      #printProgress(progressDict))
    
     
    if rootOrYoda=='root': nThreads = 1  #for the merging of root files, IO errors if running with more than 1 thread
    else: nThreads = int(opts.nThreads)
    pool = ThreadPool(nThreads)
    threadInputs=[]
    progressDict={}
    progressDict['N']=nThreads
    if (structure=="AllVariationsInFile"):
      for i,s in enumerate(uniqueWeights):
        progressDict[s]='pend'
        threadInputs.append([s,generator,process,progressDict])
    elif (structure=="OneFilePerVariation"):
      for i,s in enumerate(allFilesToMerge.keys()):
        #progressText="%d/%d"%(i, len(allFilesToMerge.keys()))
        progressDict[s]='pend'
        threadInputs.append([s,generator,process,progressDict])
    
    
    results = pool.map(threadJobFunction, threadInputs)
    pool.close() 
    pool.join()
    
    print()
    print()
    print("------------------------------------------------------------------------")
    print("[INFO] Combining variations into named sources of uncertainty")
    print("------------------------------------------------------------------------")
    
    mergedSystDict = combineAllVariations(availableWeights,"%s_%s_%s/merged_sample" % (process,generator,opts.label), "%s_%s_%s/merged_final/."% (process,generator,opts.label), opts.filter, opts.exclude, schema="!INDIR/!WEIGHTNAME.%s:!AONAME"%rootOrYoda)
    
    print()
    print("-----------------------------------------------")
    print("[INFO] Produce plots of the systematic uncertainties (with bands)")
    print("-----------------------------------------------")
  
    #makeSystematicsPlotsWithRIVET(mergedSystDict)
    if not opts.skipPlots and rootOrYoda == "yoda":
      #plots = makeSystematicsPlotsWithROOT(mergedSystDict,opts.plotsDir,"Nominal",label=opts.label, plotInfo=opts.plotInfo)
      plots = makeSystematicsPlotsWithRIVET(mergedSystDict,opts.plotsDir,"Nominal",label=opts.label, plotInfo=opts.plotInfo, normalize=opts.normalize)
      for p in plots:
        print("[INFO] printed: ", p)

if __name__ == "__main__":
  main( sys.argv[1:] )
