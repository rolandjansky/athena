from AthenaCommon.Logging import logging

##
# Basic wrapper around pyAMI for getting MC dataset cross sections & maniuplating dataset name strings
##
class GetCrossSectionAMI:

  def __init__(self):
    self.log = logging.getLogger('GetCrossSectionAMI')

  def getDatasetNameFromPath(self, path):
    import re
    datasetRegex = 'mc.._.*TeV\..*' # mc??_????TeV.???
    parts = path.split('/')
    for part in parts: # Do any of these parts look like a dataset name?
      if re.match(datasetRegex, part):
        return part
    self.log.error("Unable to figure out the MC dataset name from the path, you must supply it yourself as MCDatasetName")

  def queryAmi(self, dataset):
    if not 'EVNT' in dataset:
      dataset = self.convertDatasetNameToEVNT(dataset)

    self.dataset = dataset
    try:
      import pyAMI.client
      import pyAMI.atlas.api as api
      import json
    except ImportError:
      log.fatal("Unable to import the pyAMI client. Did you run 'lsetup pyami; voms-proxy-init -voms atlas'?")
      exit()

    self.log.info("Looking up details of " + self.dataset + " on AMI")

    if not "EVNT" in self.dataset:
      self.log.error("Dataset must be of type EVNT")
      exit()

    api.init()

    # INSTANTIATE THE PYAMI CLIENT FOR ATLAS
    client = pyAMI.client.Client('atlas')

    # search for EVNT file
    fields = 'files.cross_section,files.gen_filt_eff,nfiles'
    try:
      resDict = api.list_datasets(client, patterns = self.dataset, fields = fields)
    except Exception as reason:
      self.log.fatal("Unable to query AMI. Do you have a grid certificate? 'voms-proxy-init -voms atlas'")
      self.log.fatal("You can also supply the MCCrossSection and MCFilterEfficiency by hand for this sample")
      self.log.fatal("The reason for failure was:" + str(reason))
      exit();

    # loop over files in dataset, calculate avg filter efficiency
    numFiles = 0
    avgFiltEff = 0.0
    avgXSec = 0.0
    for results in resDict:
      numFiles = (float)(results['nfiles'])
      if (results['files_gen_filt_eff'] != 'NULL'): avgFiltEff += (float) (results['files_gen_filt_eff'])
      if (results['files_cross_section'] != 'NULL'): avgXSec += (float) (results['files_cross_section'])
      pass # end loop over files

    if(numFiles != 0):
      self.filtereff = avgFiltEff/numFiles
      self.xsec      = avgXSec/numFiles

    if self.xsec and self.filtereff:
      self.log.info("################## ################## AMI Query Complete ################## ################## ")
      self.log.info("    Cross section:" + str(self.xsec) + " nb")
      self.log.info("Filter efficiency:" + str(self.filtereff))
      self.log.info("You may want to supply these numbers directly in future as MCCrossSection and MCFilterEfficiency for speed")
      self.log.info("################## ################## ################## ################## ################### ")
    else:
      self.log.fatal("Unable to read in cross section and filter efficency from AMI.")
      exit();

  def getCrossSection(self):
    return self.xsec

  def getFilterEfficiency(self):
  	return self.filtereff

  def convertDatasetNameToEVNT(self, dataset):
    datasetReplace = dataset
    datasetReplace = datasetReplace.replace('merge.AOD', 'evgen.EVNT')
    datasetReplace = datasetReplace.replace('recon.AOD', 'evgen.EVNT')
    datasetReplace = datasetReplace.replace('merge.ESD', 'evgen.EVNT')
    datasetReplace = datasetReplace.replace('recon.ESD', 'evgen.EVNT')
    explodeDot = datasetReplace.split('.')
    datasetEVNT = ''
    isTags = False
    for part in explodeDot:
      if not 'EVNT' in part:
        if isTags: # Last section, just take the exxxx tag
          explodeScore = part.split('_')
          datasetEVNT += explodeScore[0]
        else:
          datasetEVNT += part + '.'
      else:
        isTags = True
        datasetEVNT += part + '.'
    self.log.info("Tried to converted dataset name to EVNT.")
    self.log.info("Went from: " + dataset)
    self.log.info("       To: " + datasetEVNT)
    return datasetEVNT
