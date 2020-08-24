#!/usr/bin/env python
#Author: Abhishek Nag (TU Dresden)
#email: abhishek.nag@cern.ch
import os, sys, yaml
#import lhapdf
import argparse, itertools, math, re
from stat import *

SystToolsPath=""
if 'SYSTTOOLSPATH' in os.environ.keys():
  SystToolsPath=os.environ['SYSTTOOLSPATH']
else:
  print("[ERROR] Environment variable SYSTTOOLSPATH  is not set. It should be set to the systematics-tools directory. Use setupSystematicsToolR21.sh")
  exit(1)
pdfsets=['251', '252', '10000', '10042', '10550', '10770', '10771', '10772', '10800', '10860', '10900', '10960', '10980', '10981', '10982', '10983', '11000', '11062', '11063', '11064', '11065', '11066', '11067', '11068', '11069', '11070', '11071', '11072', '11073', '11074', '11075', '11076', '11077', '11080', '11082', '11100', '11162', '11163', '11164', '11165', '11166', '11167', '11168', '11169', '11170', '11171', '11172', '11173', '11174', '11175', '11176', '11177', '11180', '11182', '11200', '11260', '11261', '11262', '11263', '11264', '11265', '11266', '11267', '11268', '11269', '11270', '11271', '11272', '11273', '11274', '11275', '11276', '11277', '11278', '11279', '11280', '12000', '12100', '12200', '12300', '12400', '13000', '13060', '13061', '13062', '13063', '13064', '13065', '13066', '13067', '13068', '13069', '13070', '13071', '13072', '13081', '13090', '13091', '13092', '13100', '13158', '13159', '13160', '13161', '13162', '13163', '13164', '13165', '13166', '13167', '13168', '13169', '13170', '13190', '13191', '13192', '13200', '13201', '13202', '13203', '13205', '13206', '13207', '13208', '13300', '13350', '13400', '13450', '20460', '20463', '20465', '20650', '20651', '21000', '21050', '21100', '21150', '21200', '21250', '22000', '22100', '22150', '22200', '22250', '22300', '22350', '22400', '22450', '22500', '22600', '22650', '22700', '22750', '22800', '22850', '22900', '22950', '23000', '23050', '23100', '23150', '23200', '23250', '23300', '23350', '23400', '23420', '23440', '23460', '23480', '23490', '23500', '23550', '23600', '23650', '23700', '23720', '23740', '23760', '23780', '23790', '23800', '23850', '23900', '23950', '24000', '24050', '24100', '24150', '25000', '25060', '25100', '25200', '25260', '25270', '25300', '25360', '25370', '25400', '25410', '25420', '25500', '25510', '25520', '25560', '25570', '25600', '25605', '25610', '25615', '25620', '25625', '25630', '25635', '25640', '25645', '25650', '25655', '25700', '25710', '25720', '25760', '25770', '25800', '25805', '25810', '25830', '25840', '25850', '40650', '40750', '40780', '40810', '40850', '40950', '42000', '42030', '42060', '42100', '42130', '42160', '42200', '42230', '42300', '42330', '42360', '42400', '42430', '42460', '42500', '42530', '42560', '42600', '42630', '42660', '42690', '42720', '42750', '42780', '42810', '42840', '60600', '60630', '60650', '60700', '60730', '60750', '60800', '61000', '61100', '61130', '61200', '61230', '61300', '61330', '61360', '61380', '61400', '61430', '61500', '61530', '61600', '61630', '61650', '61690', '61691', '61692', '61693', '61694', '61695', '61696', '61697', '61698', '61699', '61700', '61701', '61702', '61703', '61704', '61710', '61711', '61712', '61713', '61714', '61715', '61716', '61717', '61718', '61719', '61720', '61721', '61722', '61723', '61724', '61725', '61726', '61727', '61728', '61729', '61730', '61740', '61741', '61742', '61743', '61744', '61745', '61746', '61747', '61748', '61749', '61750', '61751', '61752', '61753', '61754', '61755', '61756', '61757', '61758', '61759', '61760', '65000', '65040', '65060', '65100', '65120', '70400', '80000', '80111', '80112', '80120', '80200', '81000', '81050', '81100', '81150', '81200', '81250', '82000', '82200', '82350', '90000', '90200', '90400', '90500', '90700', '90900', '90950', '91000', '91200', '91400', '91500', '91700', '91900', '91950', '92000', '100000', '100050', '100100', '100150', '100200', '100250', '100300', '100350', '100400', '100450', '100500', '100550', '100600', '100650', '100700', '100750', '100800', '100850', '100900', '100950', '101000', '101050', '101100', '102000', '102050', '102100', '102150', '102200', '102250', '102300', '102350', '102400', '102450', '102500', '102550', '102600', '102650', '102700', '102750', '102800', '102850', '102900', '102950', '103000', '103050', '103100', '104000', '104050', '104100', '104150', '104200', '104250', '104300', '104350', '104400', '104450', '104500', '104550', '104600', '104650', '104700', '104750', '104800', '104850', '104900', '104950', '105000', '105050', '105100', '106000', '106050', '106100', '106150', '106200', '106250', '106300', '106350', '106400', '106450', '106500', '106550', '106600', '106650', '106700', '106750', '106800', '106850', '106900', '106950', '107000', '107050', '107100', '200200', '200400', '200600', '200800', '229000', '229200', '229400', '229600', '229800', '230000', '230200', '230400', '230600', '230800', '231000', '231200', '231400', '231600', '231800', '232000', '232200', '232400', '232600', '232800', '233000', '233200', '233400', '233600', '233800', '234000', '234200', '234400', '234600', '234800', '235000', '235200', '235400', '235600', '235800', '236000', '236200', '236400', '236600', '236800', '237000', '237200', '237400', '237600', '237800', '238000', '238200', '238400', '238600', '238800', '239000', '239200', '239400', '239600', '239800', '240000', '240200', '240400', '240600', '240800', '241000', '241200', '241400', '241600', '241800', '242000', '242200', '242400', '242600', '242800', '243000', '243200', '243400', '243600', '243800', '244000', '244200', '244400', '244600', '244800', '245000', '245200', '245400', '245600', '245800', '246000', '246200', '246400', '246600', '246800', '247000', '247200', '247400', '250000', '251000', '260000', '260200', '260400', '260600', '260800', '261000', '261200', '261400', '261600', '261800', '262000', '262200', '262400', '262600', '263000', '263200', '263400', '263600', '264000', '264200', '264400', '264600', '265000', '265200', '265400', '265600', '266000', '266200', '266400', '266600', '267000', '267200', '267400', '267600', '268000', '268200', '268400', '268600', '269000', '269200', '269400', '269600', '270000', '270200', '270400', '270600', '271000', '271200', '271400', '271600', '272000', '272200', '272400', '273000', '273200', '273400', '274000', '274200', '274400', '275000', '275200', '275400', '280000', '280200', '280400', '281000', '281200', '281400', '282000', '282200', '282400', '283000', '283200', '283400', '284000', '284200', '284400', '285000', '285200', '285400', '288000', '290000', '290200', '290400', '291000', '291200', '291400', '292000', '292200', '292400', '292600', '293000', '295000', '297000', '299000', '301000', '303000', '303200', '303400', '303600', '303800', '304000', '304200', '304400', '304600', '304800', '305000', '305200', '305400', '305600', '305800', '306000', '306200', '306400', '307000', '309000', '311000', '313000', '315000', '315200', '315400', '315600', '315800', '316000', '316200', '316400', '316600', '316700', '316900', '317100', '317300', '317500', '317700', '317900', '318100', '318300', '318500', '318700', '318900', '319100', '319300', '319500', '319700', '319900', '320100', '320300', '320500', '320700', '320900', '321100', '321300', '321500', '321700', '321900', '322100', '322300', '322500', '322700', '322900', '323100', '323300', '323500', '323700', '323900', '324100', '324300', '324500', '324700', '324900', '325100', '500000', '502000', '504000', '506000', '900000', '900100', '900200', '900300', '900400', '900500', '900600', '900700', '900800', '900900', '901000', '901100', '901200', '901300', '2000000', '2000200', '2000400', '2000600', '2000800', '2001000', '2001200', '2001400', '2001600', '2001800', '2002000', '2002200', '2002400', '2002600', '2002800', '2003000', '2003200', '2003400', '2003600', '2003800', '2004000', '2004200', '2004400', '2004600', '2004800', '2005000', '2005200']

NNLO_PDF={'MMHT':['25200','25410', '25400'], 'CT14':['13060', '13061', '13062', '13063', '13064', '13165', '13166', '13167', '13168', '13169', '13170', '13190', '13192', '13200', '13201', '13202', '13203', '13205', '13206', '13207', '13208', '13065', '13191'], 'PDF4LHC15_NLO_30':['90900'], 'PDF4LHC15':['92000','90900','91900','91700','90700'], 'NNPDF_NLO_0117':['265000'], 'NNPDF_0117':['265000'],'NNPDF_NLO_0119':['266000'], 'NNPDF31_NLO_0118':['303400'], 'NNPDF30_NLO_0118':['260000']}

list_of_NNLOPDF=[y for x in NNLO_PDF.values() for y in x]

alpha_S={'265000':'NNPDF_NLO', '266000':'NNPDF_NLO', '270000':'NNPDF_NNLO', '269000':'NNPDF_NNLO', '91931':'PDF4LHC_NNLO', '91932':'PDF4LHC_NNLO', '90931':'PDF4LHC_NLO', '90932':'PDF4LHC_NLO'}

#alpha_ID=[y for x in alpha_S.values() for y in x]

NNLO={'NNLO_scale':['nnlops-nnloUp','nnlops-nnloDn','nnlops-nnloNom'], 'TopBottomMass_NLO':['mtmb-bminlo','mtinf','mtmb'], 'TopBottomMass_NNLO':['nnlops-bminlo', 'nnlops-mtinf'], 'NOMINAL_NNLO_NNPDF':['nnlops-nominal'], 'NOMINAL_NNLO_PDF4LHC':['nnlops-nominal-pdflhc']}

list_NNLO=[y for x in NNLO.values() for y in x]
#ext=['scale','hessian']

Hessian=['90900', '90700', '91900', '91700']

def makefile(filename):
  with open(filename, 'w+') as f:
      f.write('{}')

def file_is_empty(path):
  return os.stat(path).st_size==0

def hasNumbers(inputString):
  return any(char.isdigit() for char in inputString)

############      hessian      ###################
def hes(gen, dictionary, weight_dict):
  hessian=dict((key, value) for (key,value) in dictionary.items() if 'hessian' in key)
  hess_dict={}
  if hessian:
      h=dict((key, value) for (key,value) in dictionary.items() if '_0_hessian' in key)
      key=h.keys()[0]
      key=key.replace('_0_hessian','')
      pdf=next(re.finditer(r'\d+$',hessian.keys()[0].split('_')[0])).group(0)
      if len(hessian)>1:
          weight_dict.update({gen+'_ME_PDF'+pdf+'_Hessian':{'weights':sorted(hessian.values()), 'combination':'lhapdf', 'type':'PDF_ME', 'nominal':h.values()[0], 'nominal_pdf':pdf}})
      dictionary=dict(set(dictionary.items()) - set(hessian.items()))
      hess_dict.update({key:h.values()[0]})#make a copy without hessian tag
  dictionary.update(hess_dict)
  return dictionary, weight_dict, hess_dict

############      nom      ###################
def nom(gen, dictionary, weight_dict):
  nominal=dict((key, value) for (key,value) in dictionary.items() if 'Nominal' in key)
  p_nom='-1'
#  print nominal
  if nominal:
      n=nominal.values()[0].strip()
      if not hasNumbers(n):
          n=nominal.values()[1].strip()
      pdf=next(re.finditer(r'\d+$',n)).group(0)
      #pdf=str(getPDF(n))
      p_var=str(int(pdf)%100)
      if any(str(int(pdf)-int(p_var)) == pdfset for pdfset in pdfsets):
          p_nom=str(int(pdf)-int(p_var))
      else:
          p_var=str(int(pdf)%1000)
          p_nom=str(int(pdf)-(int(p_var)))
      if len(nominal)>1:
          weight_dict.update({gen+'_ME_PDF'+p_nom+'_var':{'weights':sorted(nominal.values()), 'combination':'lhapdf', 'type':'PDF_ME', 'nominal':nominal.get('Nominal'), 'nominal_pdf':p_nom}})
      else:
          weight_dict.update({gen+'_ME_PDF'+p_nom:{'weights':sorted(nominal.values()), 'combination':'lhapdf', 'type':'PDF_ME', 'nominal':nominal.get('Nominal'), 'nominal_pdf':p_nom}})
      dictionary=dict(set(dictionary.items()) - set(nominal.items()))
  return dictionary, weight_dict, p_nom

############      alph      ###################
def alph(gen, dictionary, weight_dict, nom_pdf, p_nom):
  alpha=dict((key, value) for (key,value) in dictionary.items() if key.endswith('_alphaS'))
  alpha_dict={}
  w=list()
  if alpha:
      for a in alpha.keys():
          pdf=next(re.finditer(r'\d+$',a.split('_')[0])).group(0)
          w.append(alpha_S.get(pdf))
          k=a.replace('_alphaS','')
          alpha_dict.update({k:alpha.get(a)})
      l=list(set(w))
      astr='_'.join(l)
      weight_dict.update({gen+'_ME_PDF'+p_nom+'_alphaS_'+astr:{'weights':sorted(alpha.values()), 'combination':'alphaS', 'type':'alphaS', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
  dictionary=dict(set(dictionary.items()) - set(alpha.items()))
  dictionary.update(alpha_dict)
  return dictionary, weight_dict, alpha_dict

############     alternate     ###################
def alternate(gen, dictionary, weight_dict, p_nom):
  alt=dict((key, value) for (key,value) in dictionary.items() if 'PDF' in key)
  flag=False
  w=[]
  if alt:
      for element in alt.keys():
          pdf=next(re.finditer(r'\d+$',element)).group(0)
          if any(pdf == pdfset for pdfset in pdfsets):
              weight_dict.update({gen+'_ME_PDF'+pdf:{'weights':[alt.get(element)], 'combination':'lhapdf', 'type':'altPDF', 'nominal':alt.get(element), 'nominal_pdf':pdf}})
          else:
              pdf=next(re.finditer(r'\d+$',element.split('_')[0])).group(0)
              w.append(pdf)
  l = list(set(w))
  for p in l:
      temp=dict((key, value) for (key, value) in alt.items() if p in key)
      weight_dict.update({gen+'_ME_PDF'+p+'_var':{'weights':sorted(temp.values()), 'combination':'lhapdf', 'type':'PDF_ME', 'nominal':sorted(temp.values())[0], 'nominal_pdf':p}})
  dictionary=dict(set(dictionary.items()) - set(alt.items()))
  return dictionary, weight_dict

############     pyth      ###################
def pyth(dictionary, weight_dict, p_nom, nom):
  py8 = dict((key,value) for (key,value) in dictionary.items() if key.startswith('Py8'))
  if py8:
      var=dict((key, value) for (key,value) in py8.items() if value.startswith('Var'))
      if var:
          weight_dict.update({'Pythia8_PDF'+p_nom+'_Var3c_alphaS':{'weights':sorted(var.values()), 'combination':'envelope', 'type':'alphaS', 'nominal':nom, 'nominal_pdf':p_nom}})
      nonsing=dict((key, value) for (key,value) in py8.items() if value.startswith('hard'))
      if nonsing:
          weight_dict.update({'Pythia8_PDF'+p_nom+'_non_singular':{'weights':sorted(nonsing.values()), 'combination':'envelope', 'type':'PDF_ME', 'nominal':nom, 'nominal_pdf':p_nom}})
      pdf=dict((key, value) for (key,value) in py8.items() if value.startswith('isr:PDF'))
      if pdf:
          weight_dict.update({'Pythia8_PDF'+p_nom+'_ISR_PDF':{'weights':sorted(pdf.values()), 'combination':'envelope','type':'PDF_ME',  'nominal':nom, 'nominal_pdf':p_nom}})
      isrfsr=dict((key, value) for (key,value) in py8.items() if value.startswith('isr') and 'fsr' in value)
      if isrfsr:
          weight_dict.update({'Pythia8_PDF'+p_nom+'_ISR_FSR_alphaS':{'weights':sorted(isrfsr.values()), 'combination':'envelope', 'type':'Scale_ME', 'nominal':nom, 'nominal_pdf':p_nom}})
      dictionary=dict(set(dictionary.items()) - set(py8.items()))
      py8=dict(set(py8.items()) - set(var.items()) -  set(nonsing.items()) - set(pdf.items()) - set(isrfsr.items()))
      if py8:
          weight_dict.update({'Pythia8_Others':{'weights':sorted(py8.values()), 'combination':'none', 'type':'none'}})
  return dictionary, weight_dict

############     gotoWeightData      ###################  
def gotoWeightData(weight_dict):
  keys=[] # list of keys for the dsid in database
  if not os.path.exists("%s/data//Weight_Database.yaml"%SystToolsPath):
      makefile("%s/data/Weight_Database.yaml"%SystToolsPath)
  w_file=open("%s/data/Weight_Database.yaml"%SystToolsPath,'r+')
  flag=False
  data=yaml.load(w_file)
  for key in weight_dict.keys():
      #if any(key in datakey for datakey in data.keys() if not any(s in datakey for s in ['scale','alphaS','Hessian', 'var'])):
      if any(key == datakey for datakey in data.keys()):
          if any(weight_dict.get(key) == data.get(k) for k in data.keys()):
              keys.append(data.keys()[data.values().index(weight_dict.get(key))])
          #else:
              #if any(weight_dict.get(key) == data.get(k) for k in data.keys() if key in k):
                  #keys.append(data.keys()[data.values().index(weight_dict.get(key))])
          else:
              l=len(list(k for k in data.keys() if key in k))
              new_key=key+'_type'+str(l+1)
              data.update({new_key:weight_dict.get(key)})
              flag=True
              keys.append(new_key)
      else:
          data.update({key:weight_dict.get(key)})
          flag=True
          keys.append(key)
  if flag:
      w_file.seek(0)
      yaml.dump(data, w_file, default_flow_style=False)
      w_file.truncate()
  return keys

############     getMGkeys      ###################
def getMGkeys(dsid, dictionary):
  PDFset = {'262000':"NNPDF30_lo_as_0118", '261000':"NNPDF30_nnlo_as_0118", '263000':"NNPDF30_lo_as_0130"}
  weight_dict={}
  flag2 = True
  nom_pdf = dictionary.get('Nominal')
  if nom_pdf:
    for key, val in PDFset.items():
      if val in nom_pdf:
        p_nom=key
        flag2 = False
  if flag2:
    d=str(int(dsid)/1000)
    filename='/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID'+d+'xxx/'
    for files in os.listdir(filename):
      if dsid in files:
        filename=filename+files
        break
    p_nom='-1'
    flag = True
    bfline=''
    f= open(filename, 'r')
    for fline in iter(f):
      if 'lhaid' in fline:
        fline=fline.replace('\'','&').replace('\"','&')
        p_nom=re.findall(r'\d+',fline)[0]
        flag = False
      elif "include" in fline and "MC15JobOptions/MadGraphControl" in fline:
        bfline=fline
    if flag:
      if '\''  in bfline:
        mgfile = bfline.split('\'')[1].replace('MC15JobOptions', '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/common/MadGraph')
      elif '\"' in bfline:
        mgfile = bfline.split('\"')[1].replace('MC15JobOptions', '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/common/MadGraph')
      else:
        mgfile = bfline.replace('MC15JobOptions', '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/common/MadGraph')
      if os.path.exists(mgfile):
        cf= open(mgfile, 'r')
        for fline in iter(cf):
          if 'lhaid' in fline:
            fline=fline.replace('\'','&').replace('\"','&')
            p_nom=re.findall(r'\d+',fline)[0]
  scale=dict((key,value) for (key,value) in dictionary.items() if key.endswith('scale') or key=='Nominal')
  if (p_nom== '-1' and len(dictionary)==1):
    return ['nominal']
  if scale:
      weight_dict.update({'MadGraph_ME_PDF'+p_nom+'_scale':{'weights':sorted(scale.values()),'combination':'envelope', 'type':'scale_ME', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
      dictionary=dict(set(dictionary.items()) - set(scale.items()))
      dictionary.update({'Nominal':nom_pdf})
  nominal=dict((key, value) for (key,value) in dictionary.items() if 'Nominal' in key)
  if len(nominal)>1:
      weight_dict.update({'MadGraph_ME_PDF'+p_nom+'_var':{'weights':sorted(nominal.values()), 'combination':'lhapdf', 'type':'PDF_ME', 'nominal':nominal.get('Nominal'), 'nominal_pdf':p_nom}})
  weight_dict.update({'MadGraph_PDF'+p_nom+'_Nominal':{'type':'Nominal', 'weights':[nom_pdf], 'combination':'none', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
  dictionary=dict(set(dictionary.items()) - set(nominal.items()))
  if dictionary:
          weight_dict.update({'MadGraph_Other':{'weights':sorted(dictionary.values()), 'combination':'none', 'type':'unknown'}})
  keys = gotoWeightData(weight_dict)
  return keys

############     weightData      ###################
def weightData(gen, dictionary):
  weight_dict={}
#####
  if gen == 'Sherpa':
      nom_pdf = dictionary.get('Nominal')
      #pdf=next(re.finditer(r'\d+$',scale.values()[0])).group(0)
      me_weights = dict((key,value) for (key,value) in dictionary.items() if key.startswith('ME_ONLY_'))
      dictionary=dict(set(dictionary.items()) - set(me_weights.items()))
      scale=dict((key,value) for (key,value) in dictionary.items() if key.endswith('scale') or key=='Nominal')
      pdf=str(getPDF(scale.values()[0]))
      weight_dict.update({'Sherpa_ME_ONLY_PDF'+pdf :{'weights':sorted(me_weights.values()), 'combination':'none', 'type':'matrix element'}})
      weight_dict.update({'Sherpa_ME_PDF'+pdf+'_scale':{'weights':sorted(scale.values()),'combination':'envelope', 'type':'scale_ME', 'nominal':nom_pdf, 'nominal_pdf':pdf}})
      dictionary=dict(set(dictionary.items()) - set(scale.items()))
      dictionary.update({'Nominal':nom_pdf})
      dictionary, weight_dict, p_nom = nom('Sherpa',dictionary, weight_dict)
      weight_dict.update({'Sherpa_PDF'+pdf+'_Nominal':{'type':'Nominal', 'weights':[nom_pdf], 'combination':'none', 'nominal':nom_pdf, 'nominal_pdf':pdf}})
      dictionary, weight_dict, alpha_dict = alph('Sherpa',dictionary, weight_dict, nom_pdf, pdf)
      dictionary, weight_dict, hess_dict = hes('Sherpa',dictionary, weight_dict)
      dictionary=dict(set(dictionary.items()) - set(alpha_dict.items()))
      dictionary=dict(set(dictionary.items()) - set(hess_dict.items()))
      dictionary, weight_dict = alternate('Sherpa',dictionary, weight_dict, nom_pdf)
      if dictionary:
          weight_dict.update({'Sherpa_Other':{'weights':sorted(dictionary.values()), 'combination':'none', 'type':'unknown'}})
####
  elif gen == 'Powheg' or gen == 'aMcAtNlo':
      nom_pdf = dictionary.get('Nominal')
      dictionary, weight_dict, p_nom = nom(gen, dictionary, weight_dict)
      weight_dict.update({gen+'_PDF'+p_nom+'_Nominal':{'type':'Nominal', 'weights':[nom_pdf], 'combination':'none', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
      dictionary.update({'Nominal':nom_pdf})
      dictionary, weight_dict=pyth(dictionary, weight_dict, p_nom, nom_pdf)
      scale=dict((key,value) for (key,value) in dictionary.items() if key.endswith('scale') or key=='Nominal')
      weight_dict.update({gen+'_ME_PDF'+p_nom+'_scale':{'weights':sorted(scale.values()),'combination':'envelope', 'type':'scale_ME', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
      dictionary=dict(set(dictionary.items()) - set(scale.items()))
      dictionary, weight_dict, alpha_dict = alph(gen,dictionary, weight_dict, nom_pdf ,p_nom)
      dictionary, weight_dict, hess_dict = hes(gen,dictionary, weight_dict)
      for pdf in list_of_NNLOPDF:
          temp=dict((key, value) for (key, value) in dictionary.items() if key.endswith(pdf))
          if len(temp)>1:
              weight_dict.update({gen+'_ME_PDF'+pdf+'_scale':{'weights':sorted(temp.values()),'combination':'envelope','type':'Scale_ME', 'nominal':sorted(temp.values())[-1], 'nominal_pdf':pdf}})
              dictionary=dict(set(dictionary.items()) - set(temp.items()))
              if any(pdf in d for d in dictionary.keys()):
                  dictionary.update({'PDF'+pdf:sorted(temp.values())[-1]})
                  #print({'PDF'+pdf:sorted(temp.values())[-1]})
      dictionary=dict(set(dictionary.items()) - set(alpha_dict.items()))
      for w in list_NNLO:
          temp=dict((key, value) for (key, value) in dictionary.items() if key.startswith(w))
          if temp:
              container=list(k for k, v in temp.items() if w in v)[0]
              if 'scale' in container:
                  weight_dict.update({gen+'_'+container:{'weights':sorted(temp.values()),'combination':'envelope','type':'Scale_ME', 'nominal':nom_pdf, 'nominal_pdf':p_nom}})
              elif 'NOMINAL' in container:
                  weight_dict.update({gen+'_'+container:{'weights':sorted(temp.values()),'combination':'none','type':'PDF_ME', 'nominal_pdf':p_nom}})
              else:
                  weight_dict.update({gen+'_'+container:{'weights':sorted(temp.values()),'combination':'none','type':'unknown', 'nominal_pdf':p_nom}})
              dictionary=dict(set(dictionary.items()) - set(temp.items())) 
      dictionary=dict(set(dictionary.items()) - set(hess_dict.items()))
      dictionary, weight_dict = alternate(gen,dictionary, weight_dict, p_nom)
      if dictionary:
          weight_dict.update({gen+'_Other':{'weights':sorted(dictionary.values()), 'combination':'none', 'type':'unknown'}})
#####
  keys=gotoWeightData(weight_dict)
  return keys

############     scale     ###################
def scale(value):
  ph_name=''
  if value.find('dyn_scale_choice'):
      ph_name= 'other_'+value.strip()
  elif 'mur=1' in value and 'muf=1' in value:
      ph_name='Nominal'
  elif 'muR=0.10000E+01' in value and 'muF=0.10000E+01' in value:
      ph_name='Nominal'
  elif 'muR=1.000000E+00' in value and 'muF=1.000000E+00' in value:
      ph_name='Nominal'
  else:
      w_name=re.split('_| ',value)
      for part in w_name:
          if 'mur' in part.lower():   
              if '25' in part:
                  ph_name=ph_name+'muR25'
              elif '2' in part:
                  ph_name=ph_name+'muR2'
              elif '5' in part:
                  ph_name=ph_name+'muR5'
              elif '4' in part:
                  ph_name=ph_name+'muR4'
          if 'muf' in part.lower():
              if '25' in part:
                  ph_name=ph_name+'muF25'
              elif '2' in part:
                  ph_name=ph_name+'muF2'
              elif '5' in part:
                  ph_name=ph_name+'muF5'
              elif '4' in part:
                  ph_name=ph_name+'muF4'
  return ph_name

##################     getPDF      #####################

def getPDF(ss):
  num, res = 0, 0
  # start traversing the given string  
  for i in range(len(ss)): 
       
    if ss[i] >= "0" and ss[i] <= "9": 
      num = num * 10 + int(int(ss[i]) - 0) 
    else: 
      res = max(res, num) 
      num = 0
          
  return max(res, num)

############     makeMGDatabase      ###################
def makeMGDatabase(values):
  dic={}
  for value in values:
      ph_name=''
      if 'mur' in value.lower():
          ph_name=scale(value)
          ph_name=ph_name+'_scale'
      elif 'Member' in value:
          #pdf=next(re.finditer(r'\d+$',value.strip())).group(0)
          pdf=value.split()[1]
          if pdf=='0':
              ph_name='Nominal'
          else:
              ph_name='Nominal_'+pdf
      elif value=='nominal':
          ph_name='Nominal'
      else:
          ph_name=value.strip()
      dic.update({ph_name:value})
  print(len(dic))
  return dic

############     makePowhegDatabase      ###################
def makePowhegDatabase(values):
  dic={}
  for value in values:
      flag=True
      ph_name=''
      if value.strip().lower() == 'nominal':
          ph_name='Nominal'
      elif ('mur' in value.lower() or 'muf' in value.lower()):
          if value.startswith('isr'):
              ph_name='Py8_'+value
          else:
              ph_name=scale(value.strip())
          if ph_name=='':
              s_name=value.split(',')
              for element in s_name:
                  if 'mur' in element.lower() and '5' in element:
                      ph_name=ph_name+'muRdw'
                  elif 'mur' in element.lower() and '2' in element:
                      ph_name=ph_name+'muRup'
                  elif 'muf' in element.lower() and '5' in element:
                      ph_name=ph_name+'muFdw'
                  elif 'muf' in element.lower() and '2' in element:
                      ph_name=ph_name+'muFup'
          if ph_name is not 'Nominal':
              ph_name=ph_name+'_scale'
          for key in NNLO_PDF.keys():
              if value.strip().endswith(key):
                  for pd in NNLO_PDF.get(key):
                      v=list(k for k in values if k.strip().endswith(pd))
                      if len(v)>0:
                          ph_name=ph_name+'_PDF'+pd
      elif 'pdf' in value.lower() and 'memberid' in value.lower():
          pdf = str(getPDF(value.split()[0]))
          p_var = str(getPDF(value.split()[1]))
          if any(pdf==val for val in list_of_NNLOPDF):
              if p_var == '0':
                  ph_name='Nominal'
              else:
                  ph_name='Nominal_'+p_var
          else:
              ph_name = 'PDF_'+pdf+'_'+p_var
      elif 'pdf' in value.lower() and any(char.isdigit() for char in value.strip()):#
          #pdf=next(re.finditer(r'\d+$',value.strip())).group(0)
          pdf = str(getPDF(value.strip()))
          if any(pdf == alpha for alpha in alpha_S.keys()):
              ph_name='PDF'+pdf+'_alphaS'
          elif any(str(int(pdf)-1) in val or str(int(pdf)+1) in val for val in values):
              p_var=str(int(pdf)%100)
              if any(str(int(pdf)-int(p_var)) == pdfset for pdfset in pdfsets):
                  p_nom=str(int(pdf)-int(p_var))
              else:
                  p_var=str(int(pdf)%1000)
                  p_nom=str(int(pdf)-(int(p_var)))
              if any(p_nom in val for val in values):
                  if any(p_nom == hess for hess in Hessian):#
                      ph_name='PDF'+p_nom+'_'+p_var+'_hessian'#
                  else:
                      #if any(value.strip().lower() == 'nominal' for value in values):
                      if p_var == '0':
                          ph_name='PDF'+p_nom
                      else:
                          ph_name='PDF'+p_nom+'_'+p_var
                      #else:#
                       #   if p_var == '0':#
                        #      ph_name='Nominal'#
                         # else:#
                          #    ph_name='Nominal_'+p_var#
              else:
                  ph_name='Nominal_'+p_var
          else:
              ph_name='PDF'+pdf
      else:
          if value.startswith('Var3c') or value.startswith('hard') or value.startswith('isr'):
              ph_name='Py8_'+value
          else:
              ph_name=value.strip()
      if value.strip().isdigit():
            ph_name='others_'+value.strip()
      ph_name=ph_name.replace(':','_')
      dic.update({ph_name:value})
#  print sorted(dic.iteritems())
  print len(dic)
  return dic

############     makeSherpaDatabase      ###################
def makeSherpaDatabase(values):
  dic={}
  if len(values)==1:
      ph_name='Nominal'
      dic.update({ph_name:values[0].strip()})
  else:
      for value in values:
          ph_name=''
          if value.startswith("ME_ONLY_"):
            ph_name= value.strip()
            dic.update({ph_name:value})
            continue
          if value.strip().isdigit():
            ph_name='others_'+value.strip()
          if 'mu' in value.lower():
              ph_name=scale(value)
              if ph_name=='':
                  if 'ASS' in value.split('_')[-1]:
                      ph_name=value.split('_')[-1]
                      dic.update({ph_name:value})
                      continue
                  pdf = str(getPDF(value))
                  if any(pdf == alpha for alpha in alpha_S.keys()):
                      ph_name='PDF'+pdf+'_alphaS'
                  elif any('0.5' in val and pdf in val for val in values) and any(pdf == pdfset for pdfset in pdfsets):
                      ph_name='Nominal'
                  elif any(str(int(pdf)-1) in val for val in values) or any(str(int(pdf)+1) in val for val in values):
                      p_var=str(int(pdf)%100)
                      if any(str(int(pdf)-int(p_var)) == pdfset for pdfset in pdfsets):
                          p_nom=str(int(pdf)-int(p_var))
                      else:
                          p_var=str(int(pdf)%1000)
                          p_nom=str(int(pdf)-(int(p_var)))
                      if any('0.5' in val and p_nom in val for val in values):
                          ph_name='Nominal_'+p_var
                      elif p_nom in Hessian:
                          ph_name='PDF'+p_nom+'_'+p_var+'_hessian'
                      else:
                          ph_name='PDF'+p_nom+'_'+p_var
                  else:
                      ph_name='PDF'+pdf
              else:
                  ph_name=ph_name+'_scale'
          else:
              ph_name=value.strip()
          dic.update({ph_name:value})
  print(len(dic))
  return dic

############     getKey     ###################
def getKey(filename):
  new_dict={}
  values='none'
  keys=[]
  flag=True
  if file_is_empty(filename):
      keys=['None']
  else:
      with open(filename) as f:
          values = f.read().splitlines()
          print(len(values))
          if 'MadGraph' in filename:
              dsid= filename.split('/')[-1].split('.')[0].split('_')[-1]
              new_dict=makePowhegDatabase(values)
              keys=getMGkeys(dsid, new_dict)
              #keys=weightData('Sherpa', new_dict)
          elif len(values)==1 and values[0].strip()=='nominal':
              keys=['nominal']
          else:
              if 'Sherpa' in filename or "Sh_" in filename:
                  new_dict=makeSherpaDatabase(values)
                  keys=weightData('Sherpa', new_dict)
              elif 'aMcAtNlo' in filename:
                  new_dict=makePowhegDatabase(values)
                  keys=weightData('aMcAtNlo', new_dict)
              elif 'Powheg' in filename or 'phpy' in filename.lower():
                  new_dict=makePowhegDatabase(values)
                  keys=weightData('Powheg', new_dict)
#  print keys
  return keys

############     buildDatabase      ###################
def buildDatabase(filename):
  dsid= filename.split('/')[-1].split('.')[0].split('_')[-1]
  key=getKey(filename)
  if not os.path.exists("%s/data/DSID_Database.yaml"%SystToolsPath):
      makefile("%s/data/DSID_Database.yaml"%SystToolsPath)
  d_file= open("%s/data/DSID_Database.yaml"%SystToolsPath,'r+')
  database=yaml.load(d_file)
  database.update({dsid:key})
  d_file.seek(0)
  yaml.dump(database, d_file, default_flow_style=False)

def main(argv):
  parser = argparse.ArgumentParser( description = 'Weight file needed' )
  parser.add_argument('file', type=str, nargs='+')
  args = parser.parse_args()
  for f in args.file:
      buildDatabase(f)

if __name__ == "__main__":
  main( sys.argv[1:] )
