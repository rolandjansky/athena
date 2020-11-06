#!/usr/bin/env python
#
# Generate cut files for ARTs from validation-cuts.txt.
# To be run from directory PhysicsAnalysis/TopPhys/xAOD/TopAnalysis/share.

import sys
import re


def rewriteConfig(artConfigName, inputConfigPath, campaign, overrides=None, klfitter=False, actualmurw=False):
   overrides2 = {}
   if overrides is not None:
      overrides2.update(overrides)
   overrides = overrides2
   if not actualmurw:
      overrides.setdefault('PRWActualMu_FS', None)
      overrides.setdefault('PRWActualMu_AF', None)
   rexEol = re.compile(r'[\n\r]*$')
   rexLine = re.compile(r'(\s*)(\S+)(\s+)')
   rexSelection = re.compile(r'\s*(?:SELECTION|SUB)\b')
   rexCampaignSpecificStart = re.compile(r'(?i)\s*#\s*(mc\w+) configuration\s*$')
   rexCampaignSpecificEnd = re.compile(r'#?PRWLumiCalcFiles\b')
   section = 1 # 1 = config options, 2 = selections
   campaignSpecificState = None # True (False) = forcefully enable (disable) current campaign-specific set of config options
   emitted = set() # which config keys have already been written
   def writeln(line):
      out.write(line)
      out.write('\n')
   with open(inputConfigPath, 'rb') as f:
      with open('ART/artcut_test_' + artConfigName + '.txt', 'wb') as out:
         for line in f:
            line = rexEol.sub('', line)
            if section == 1:
               if not rexSelection.match(line):

                  # hacks for per-campaign PRW configuration
                  m = rexCampaignSpecificStart.match(line)
                  if m:
                     campaignSpecificState = m.group(1).lower() == campaign.lower()
                     if campaignSpecificState and campaign.lower() == 'mc16a':
                        campaignSpecificState = None # mc16a enabled by default
                  if campaignSpecificState is False:
                     if rexCampaignSpecificEnd.match(line):
                        campaignSpecificState = None
                     continue
                  if campaignSpecificState is True and line.startswith('#PRW'):
                     line = line[1:]

                  m = rexLine.match(line)
                  if m:
                     k = m.group(2)
                     if k in overrides:
                        if k not in emitted:
                           v = overrides[k]
                           emitted.add(k)
                           if v is not None:
                              writeln(' '.join((k, v)))
                     else:
                        writeln(line)
                  else:
                     writeln(line)
                  continue
               # emit remaining options that did not appear in the original config file
               for k, v in overrides.iteritems():
                  if v is not None and k not in emitted:
                     writeln(' '.join((k, v)))
               # done with config options
               section = 2

            if section == 2:
               m = re.match('^#?((?:RECO::)?KLFITTER.*)', line)
               if m:
                  line = m.group(1)
                  if not klfitter:
                     line = '#' + line
            writeln(line)


rewriteConfig('410470afmc16a', 'validation-cuts.txt', 'mc16a')
rewriteConfig('410470afmc16d', 'validation-cuts.txt', 'mc16d')
rewriteConfig('410470fsmc16a', 'validation-cuts.txt', 'mc16a')
rewriteConfig('410470fsmc16d', 'validation-cuts.txt', 'mc16d')
rewriteConfig('410470fsmc16dactualmurw', 'validation-cuts.txt', 'mc16d', actualmurw=True)
rewriteConfig('410470fsmc16apflow', 'validation-cuts.txt', 'mc16a', overrides={
       'JetCollectionName': 'AntiKt4EMPFlowJets_BTagging201810',
   })
rewriteConfig('410470fsmc16dpflow', 'validation-cuts.txt', 'mc16d', overrides={
       'JetCollectionName': 'AntiKt4EMPFlowJets_BTagging201810',
   })
rewriteConfig('410470fsmc16asysts', 'validation-cuts.txt', 'mc16a', overrides={
       'Systematics': 'All',
   })
rewriteConfig('410470fsmc16dsysts', 'validation-cuts.txt', 'mc16d', overrides={
       'Systematics': 'All',
   })
rewriteConfig('410470fsmc16dklfitter', 'validation-cuts.txt', 'mc16d', klfitter=True)
