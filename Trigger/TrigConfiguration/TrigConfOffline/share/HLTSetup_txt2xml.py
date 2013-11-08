#!/usr/bin/env python
# $Id: HLTSetup_txt2xml.py,v 1.6 2009/05/15 13:57:42 nozicka Exp $

# converts the python style output from the HistorySvc into xml
# Joerg Stelzer, CERN, 10.Sept.2006

# PJB 21/11/08 Ignore the DataFlowConfig component (savannah 43616)
# but also need to ignore CalBufferName comp of MuFast (set to "")
# which is otherwise set by this

import getopt, sys, re, os.path, time
import xml.dom.minidom
from TrigConfOffline.HLTAlgorithm import HLTAlgorithm

flatComponentStructure = False
ConversionForPointOne = False # Sylvie 11/07/2008
                              # This is a tempory hack to set "OnlineRun" to "True" for muFast_Muon (muFast)
                              # when preparing configuration for Point1.
                              # This can be set to True by using the option --ForPointOne

# List of the special handled parameters, algorithms
# Special properties which value should be ignored and set manually
propertyNameSpecial = ('OutputLevel',
  'rfac','energies','regions','correction','interp_barriers',
  'wtEMB0''wtEME0''wtEMB1''wtEMB2''wtEME1''wtEME2''wtTile1', 'wtTile2','wtHec1''wtHec2','wtFCal1','wtFCal2','etaFit','sampling_depth',
  'MuonCalBufferName','DoRecursiveLoad')
  
algAliasSpecial = ('DataFlowConfig','CosmicEgammaTrigCaloClusterMaker_slw.trigslw','HistorySvc','muFast_Muon')
algNameSpecial = ('muFast')

typeLUT = {
    'AthAlgSeq'                                                                              :'AthSequencer',
    'ServiceManager'                                                                         :'ServiceManager',
    'Streams'                                                                                :'AthSequencer',
    'DataFlowConfig'                                                                         :'DataFlowConfig'
    }


def convertLinesToAlg(lines):
  alg = None
  alg = HLTAlgorithm(name='unknown',OutputLevel=5)
  lineind = 0
  nlines = len(lines)
  while lineind < nlines:
    line = lines[lineind].rstrip("\n").rstrip()
    #print 'convertLinesToAlg:\t[%d]' % (lineind),line
    if line.startswith('Type:') :
      algName = line[len('Type:'):].strip()
      alg.setName(algName)
      lineind += 1
      continue
    elif line.startswith('Name:') :
      algAlias = line[len('Name:'):].strip()
      # Remove spaces from alias
      aliases = algAlias.split('.')
      for ind in  range(len(aliases)):
        aliases[ind] = aliases[ind].strip()
      algAlias = '.'.join(aliases)
      alg.setAlias(algAlias)
      lineind += 1
      continue
      
    elif line.startswith('Py_Name:') :
      algPyName = line[len('Py_Name:'):].strip()
      alg.py_name = algPyName
      lineind += 1
      continue
    
    elif line.startswith('Py_Package:') :
      algPyPackage = line[len('Py_Package:'):].strip()
      alg.py_package = algPyPackage
      lineind += 1
      continue
    # Expect the Properties of the algorithm
    elif line.startswith('Properties:') :
      # Skip the line with Properties:  [
      lineind +=1
      propsStart = lineind
      propsEnd = len(lines)
      for propind in range(lineind,nlines) :
        propline = lines[propind].rstrip("\n").strip()
        propsEnd = propind
        # Test for end of the properties
        if propline == ']': break
      properties = convertLinesToProperties(lines[propsStart:propsEnd])
      # Set the algorithm properties
      for k,v in properties.items() :
        alg.setProperty(k,v)
        
      # Increase the line counter to skip the properties
      lineind = propsEnd+1
      continue
        
    # Expect the children of the algorithm
    elif line.startswith('Subalgorithms:') :
      lineind += convertSubalgorithmsToChildAlg(lines[lineind:],alg)
      lineind += 1
      continue

    # increase Line counter
    lineind += 1
    
  #print alg
  return alg

# Return number of processed lines
def convertSubalgorithmsToChildAlg(lines,parentAlg) :
  # Format :
  # Subalgorithms: {
  # ----------
  # Type: InDet::Pixel_TrgClusterization
  # Name: PixelClustering_Bphysics_EFID
  # Version: TrigInterfaces-01-01-09
  # Properties: [
  #  .......
  # ]
  # No subalgorithms
  # ----------
  # Type: InDet::TRT_TrgRIO_Maker
  # Name: TRTDriftCircleMaker_Bphysics_EFID
  # Version: TrigInterfaces-01-01-09
  #  .......
  # No subalgorithms.
  # ----------
  # }

  #
  #print 'convertSubalgorithmsToChildAlg - %s/%s: start:\t[%d]' % (parentAlg.getName(), parentAlg.getAlias(),len(lines)),lines[0].rstrip('\n').strip()
  lineind = 0
  # Skip the 1st line in case the Subalgorithms name on it
  if lines[0].rstrip("\n").rstrip().startswith('Subalgorithms:') :
    #print 'convertSubalgorithmsToChildAlg - %s/%s:\t[%d]' % (parentAlg.getName(), parentAlg.getAlias(),lineind),lines[lineind].rstrip('\n').strip()
    lineind += 1
  alg = HLTAlgorithm(name='unknown', alias='', OutputLevel=5, alg_type='PrivateTool')
  nlines = len(lines)
  while lineind < nlines :
    #print 'convertSubalgorithmsToChildAlg - %s/%s:\t[%d]' % (parentAlg.getName(), parentAlg.getAlias(),lineind),lines[lineind].rstrip('\n').strip()
    line = lines[lineind].rstrip("\n").strip()
    if line.startswith('----------') :
      lineind+=1
      continue
    elif line.startswith('Type:') :
      algName = line[len('Type:'):].strip()
      alg.setName(algName)
      lineind+=1
      continue
    elif line.startswith('Name:') :
      algAlias = '%s.%s' % (parentAlg.getAlias(),line[len('Name:'):].strip())
      alg.setAlias(algAlias)
      lineind+=1
      continue
    elif line.startswith('Py_Name:') :
      algPyName = line[len('Py_Name:'):].strip()
      alg.py_name = algPyName
      lineind+=1
      continue
    elif line.startswith('Py_Package:') :
      algPyPackage = line[len('Py_Package:'):].strip()
      alg.py_package = algPyPackage
      lineind+=1
      continue
    elif line.startswith('Properties:') :
      propsStart=lineind
      propsEnd=propsStart
      for propind in range(propsStart,nlines) :
        propsEnd = propind
        if lines[propind].rstrip("\n").strip() == ']' : break
        
      # Set the algorithm properties
      for k,v in convertLinesToProperties(lines[propsStart:propsEnd]).items() :
        alg.setProperty(k,v)
      
      # Increase the line counter to skip the properties
      lineind = propsEnd+1
      continue
    
    elif line.startswith('No subalgorithms.') : # End of the child algorithm append it to the parent, create new one
      if alg and alg.getAlias() :
        #print 'convertSubalgorithmsToChildAlg - %s/%s: append a child alg %s/%s' % (parentAlg.getName(), parentAlg.getAlias(), alg.getName(), alg.getAlias())
        parentAlg.appendChild(alg)
      
      alg = HLTAlgorithm(name='unknown', alias='',OutputLevel=5, alg_type='PrivateTool')
      lineind += 1
      continue
    
    elif line.startswith('Subalgorithms:') : # Child algorithm has sub algorithms - call recursively
      #print 'convertSubalgorithmsToChildAlg - %s/%s: Subalgorithms found - recursive call' % (parentAlg.getName(), parentAlg.getAlias())
      lineind += convertSubalgorithmsToChildAlg(lines[lineind:],alg)
      lineind += 1
      continue
    
    elif line == '}' : # Subalgorithms finished
      #print 'convertSubalgorithmsToChildAlg - %s/%s: Subalgorithms finished at }' % (parentAlg.getName(), parentAlg.getAlias())
      if alg and alg.getAlias() :
        #print 'convertSubalgorithmsToChildAlg - %s/%s: append a child alg %s/%s' % (parentAlg.getName(), parentAlg.getAlias(), alg.getName(), alg.getAlias())
        parentAlg.appendChild(alg)
        #print 'convertSubalgorithmsToChildAlg:\n%s' % alg
      else : # An empty algorithms may be created at the end
        pass
        #print 'convertSubalgorithmsToChildAlg -  %s/%s: empty child alg' % (parentAlg.getName(), parentAlg.getAlias())
      alg = HLTAlgorithm(name='unknown', alias='',OutputLevel=5, alg_type='PrivateTool')
      #lineind += 1
      break

    # In all the other cases - skip the line
    lineind += 1
  
  if lineind > len(lines) : 
    print 'WARNING:\t','Processed lines: %d' % (lineind), 'vs. nr lines %d' % (len(lines))
    lineind = len(lines)
    
  # Before quitting - make sure the last child algorithm was appended
  if alg and alg.getAlias() and not parentAlg.getChild(alg.getAlias()):
    #print 'convertSubalgorithmsToChildAlg - %s/%s: additional append a child alg %s/%s' % (parentAlg.getName(), parentAlg.getAlias(), alg.getName(), alg.getAlias())
    parentAlg.appendChild(alg)
  
  #print 'convertSubalgorithmsToChildAlg - %s/%s: quit:\t[%d]' % (parentAlg.getName(), parentAlg.getAlias(),lineind),lines[lineind-1].rstrip('\n').strip()
  return lineind

# Apply property exceptions - return corrected value
def applyPropertyValueExceptions(name, value) :
  # Exception for the OutputLevel
  if name == 'OutputLevel': return 3
  if name == 'OutputLevel' and value < 3: return 3
  # Special properties of the Calorimeter Tools
  #if name == 'rfac' and value in ('',None): return '""'
  #if name == 'energies' and value in ('',None): return '""'
  #if name == 'regions' and value in ('',None): return '""'
  #if name == 'correction' and value in ('',None): return '""'
  #if name == 'interp_barriers' and value in ('',None): return '""'
  #if name == 'wtEMB0'  and value in ('',None): return '""'
  #if name == 'wtEME0'  and value in ('',None): return '""'
  #if name == 'wtEMB1'  and value in ('',None): return '""'
  #if name == 'wtEMB2'  and value in ('',None): return '""'
  #if name == 'wtEME1'  and value in ('',None): return '""'
  #if name == 'wtEME2'  and value in ('',None): return '""'
  #if name == 'wtTile1' and value in ('',None): return '""'
  #if name == 'wtTile2' and value in ('',None): return '""'
  #if name == 'wtHec1'  and value in ('',None): return '""'
  #if name == 'wtHec2'  and value in ('',None): return '""'
  #if name == 'wtFCal1' and value in ('',None): return '""'
  #if name == 'wtFCal2' and value in ('',None): return '""'
  #if name == 'etaFit'  and value in ('',None): return '""'
  #if name == 'sampling_depth' and value in ('',None): return '""'
  
  #if name == 'MuonCalBufferName': return ''
  #if name == 'DoRecursiveLoad': return ''
  
  return value

# Convert lines to properties
def convertLinesToProperties(lines):
  properties = {}
  lineind = 0
  for line in lines:
    line = line.rstrip("\n").strip()
    #print 'convertLinesToProperties:\t %d' % (lineind),line
    # Skip the first line
    if line.startswith('Properties:') : continue
    # End of the properties
    lineind += 1
    if line==']' : break
    items = line.split(':')
    if len(items) >=2 :
      key = eval(items[0])
      value = line.replace(items[0],'',1).lstrip(':').strip()
      try :
        value = eval(value)
      except :
        pass
      value = applyPropertyValueExceptions(key,value)
      properties[key]=value
      
  #print 'convertLinesToProperties:\t',properties
  return properties

# Fix the alone standing algorithms in setup
def fixLonelyAlgs(setup) :
  # 2D list where the child alias and matching lonely alg alias are saved
  matchChildLonelyAlg = []
  for alias in setup.keys():
    # Split alias to particular algorithms
    childrenAliases = alias.split('.')
    # Single algorithm in the setup - not what I am searching for
    if len(childrenAliases) == 1 : continue
    # Cut the parent alias
    childrenAliases = childrenAliases[1:]
    for childind in range(len(childrenAliases)) :
      childAlias = '.'.join(childrenAliases[childind:])
      if setup.has_key(childAlias) : # A lonely algorithm found
        matchChildLonelyAlg.append([childAlias,alias])
        if setup[childAlias].getName() == setup[alias].getName():
          # Compare properties whether the algorithms may be merged
          matchingProperties = True
          lonelyAlgProperties = setup[childAlias].getProperties()
          for k,v in setup[alias].getProperties().items() :
            if lonelyAlgProperties.has_key(k) and lonelyAlgProperties[k] != v :
              matchingProperties = False
              break
            
          #if matchingProperties : matchChildLonelyAlg.append([childAlias,alias])

  for item in matchChildLonelyAlg :
    print "Lonely algorithm %s/%s may be merged with %s/%s" % (setup[item[0]].getName(), setup[item[0]].getAlias(), setup[item[1]].getName(), setup[item[1]].getAlias())

# Fix the alone standing algorithms in setup
def fixLonelySteerAlgs(setup) :
  matchChildLonelyAlg = []
  # 2D list where the child alias and matching lonely alg alias are saved
  for alias in setup.keys():
    if 'TrigSteer_' != alias[:10] : continue
    # Split alias to particular algorithms
    childrenAliases = alias.split('.')
    # Single algorithm in the setup - not what I am searching for
    if len(childrenAliases) == 1 : continue
    # Cut the parent alias
    childrenAliases = childrenAliases[1:]
    for childind in range(len(childrenAliases)) :
      childAlias = '.'.join(childrenAliases[childind:])
      if setup.has_key(childAlias) : # A lonely algorithm found
        if setup[childAlias].getName() == setup[alias].getName():
          matchChildLonelyAlg.append([childAlias,alias])
          #if matchingProperties : matchChildLonelyAlg.append([childAlias,alias])

  for item in matchChildLonelyAlg :
    print "Merge Steering algorithms: %s/%s and %s/%s" % (setup[item[1]].getName(), setup[item[1]].getAlias(), setup[item[0]].getName(), setup[item[0]].getAlias())
    setup[item[0]].setAlias(item[1])
    success = setup[item[1]].mergeAlgorithm(setup[item[0]])
    if success : del setup[item[0]]

# Fix the child parent relations accordingly to the name of the algorithm
def fixChildParentRelations(algorithms):
  if isinstance(algorithms, list) :
    # Suppose the algorithms does not have parents
    # Sorting of the algorithms accordingly to the alias assure the consequence of the parent and child
    def hltAlgAliases_cmp(a1, a2) :
      return cmp(a1.getAlias(), a2.getAlias())
      
    algorithms.sort(hltAlgAliases_cmp)
    i = 0
    while i < len(algorithms):
      alias = algorithms[i].getAlias()
      # Remove the ToolSvc. from alias
      #if alias[:8] == 'ToolSvc.' :
        #alias = alias[8:]
        
      # Check the alias
      if '.' in alias :
        #self._log.warning('Attempt to fix child algorithm %s/%s' % (algorithms[i].getName(), algorithms[i].getAlias()))
        childAlias = algorithms[i].getAlias().split('.')[-1]
        parentAlias = algorithms[i].getAlias().rstrip(childAlias)
        parentAlias = parentAlias.rstrip('.')
        
        parentFound = False
        flattenAlgs = []
        for ii in range(len(algorithms)) :
          if ii != i :
            flattenAlgs.extend(algorithms[ii].flattenAlg())
        
        for alg in flattenAlgs :
          if alg.getAlias() == parentAlias :
            #self._log.debug("Parent found: %s/%s - algorithm will be appended as child" % (alg.getName(), alg.getAlias()))
            parentFound = True
            # Set the type of the algorithm
            alg_type = 'PrivateTool'
            if alg.getAlias() == 'ToolSvc' : alg_type = 'PublicTool'
            algorithms[i].alg_type = alg_type
            alg.appendChild(algorithms[i])
        
        # If the algorithm was assigned as a child alg to some parent - delete it
        if parentFound :
          #self._log.debug("Appended algorithm %s/%s will be deleted from top parents list" % (algorithms[i].getName(), algorithms[i].getAlias()))
          #algorithms[i].delete()
          del algorithms[i]
        else :
          print "WARNING - No parent algorithm found for %s/%s" % (algorithms[i].getName(), algorithms[i].getAlias())
          i = i+1
          
      else :
        i = i+1
    
    # Fix the children within the parents
    for parent in algorithms : parent.fixChildren()
    
  #return algorithms

# Get the TopAlg from the  AthSequencer
def getCorrectTopAlgsFromAthSequencer(setup,athSequencerAlias) :
  output = []
  if setup.has_key(athSequencerAlias) and setup[athSequencerAlias].getProperties().has_key('Members') :
    for member in setup[athSequencerAlias].getProperties()['Members'] :
      memberName = member.split('/')[0].strip()
      memberAlias = member.replace(member.split('/')[0],'',1).lstrip('/').strip()
      # Recursive call
      if memberName == 'AthSequencer' : 
        output.extend(getCorrectTopAlgsFromAthSequencer(setup,memberAlias))
      else :
        output.append(member)
      
    # At the end remove the AthSequencer from the setup
    del setup[athSequencerAlias]
    
  return output

# Convert HLTAlgorithm class to XML Element
def hltAlgorithmtoXMLElement(alg,document) :
  algtype = 'algorithm'
  try:
    algtype = alg.alg_type.lower()
  except :
    pass
  
  algorithm=topalg=steeralg=svc=pubt=privt=aud='0'
  if algtype == 'algorithm' : algorithm='1'
  if algtype == 'topalg': topalg='1'
  if algtype == 'steeralg': 
    algorithm='1'
    steeralg='1'
  if algtype == 'service': svc='1'
  if algtype == 'publictool': pubt='1'
  if algtype == 'privatetool': privt='1'
  if algtype == 'auditor': aud='1'
  
  component = document.createElement('component')
  component.setAttribute('name', str(alg.getName()))
  component.setAttribute('alias', str(alg.getAlias()))
  py_name = alg.getPy_name()
  if py_name == None : py_name = ''
  component.setAttribute('py_name', str(py_name))
  py_package = alg.getPy_package()
  if py_package == None : py_package = ''
  component.setAttribute('py_package', str(py_package))
  component.setAttribute('topalg', topalg)
  component.setAttribute('alg', algorithm)
  component.setAttribute('steeralg', steeralg)
  component.setAttribute('svc', svc)
  component.setAttribute('pubt', pubt)
  component.setAttribute('privt', privt)
  component.setAttribute('aud', aud)

  # append the properties
  for k,v in alg.getProperties().items() :
    component.appendChild(propertyToXmlElement(document,k,v))
  
  for child in alg.getChildren() :
    component.appendChild(hltAlgorithmtoXMLElement(child, document))
  
  return component

def propertyToXmlElement(document, name, value) :
  parameter = document.createElement('parameter')
  parameter.setAttribute('name', str(name))
  if name in propertyNameSpecial: value = applyPropertyValueExceptions(name, value)
  if value == None: value=''
  if isinstance(value,(str,unicode)) :
    parameter.setAttribute('value', value)
  else:
    parameter.setAttribute('value', str(value))
  parameter.setAttribute('op','set')

  return parameter

def usage():
    """Prints a help message"""
    print "\nusage:"
    print "%s [options] inputfile.txt" % sys.argv[0]
    print "options are:"
    print "  -h|--help        : prints this message"
    print "  --ForPointOne    : temporary hack to run at Point 1 (muFast_Muon)\n"
    print "Minimum required   : you have to specify the .txt file to be converted.\n"
    return

# Strip the AthSequencer and create a list of algorithm 
def stripAthSequencer(athSequencer) :
  output = []
  if athSequencer.getName() != 'AthSequencer' :
    output.append(athSequencer)
    return output
  # Create a 1st item in the list - empty - place for the athSequencer
  output.append(None)
  athSeqAlias = athSequencer.getAlias()
  for child in athSequencer.getChildren() :
    # Remove the parent alias from child alias
    childAlias = child.getAlias()
    childAlias = childAlias.replace('%s.' % (athSeqAlias),'',1)
    #print '2. stripAthSequencer:', childAlias
    child.setAlias(childAlias)
    if child.alg_type.lower() == 'privatetool' : child.alg_type = 'Algorithm'
    
    # Call recursively if child is an AthSequencer
    if child.getName() == 'AthSequencer' :
      output.extend(stripAthSequencer(child))
    else :
      output.append(child)
      
    athSequencer.removeChild(child.getAlias())
    del child
  # Set the 1st algorithm in the list 
  output[0] = athSequencer
  
  # Just for debugging
  #for alg in output :
    #print '3. stripAthSequencer:', alg
  return output
  
# Functions for application of any exceptions which should be handled manually
# input value is a dictionary of HLTAlgorithm's referenced by their alias
def applyManualExceptions(setup) :
  # Remove DataFlowConfig
  if setup.has_key('DataFlowConfig') :
    print "WARNING\t",'Manual delete %s/%s' % (setup['DataFlowConfig'].getName(),setup['DataFlowConfig'].getAlias())
    setup['DataFlowConfig'].delete()
    del setup['DataFlowConfig']
  
  # Remove ServiceManager
  if setup.has_key('ServiceManager') :
    print "WARNING\t",'Manual delete %s/%s' % (setup['ServiceManager'].getName(),setup['ServiceManager'].getAlias())
    setup['ServiceManager'].delete()
    del setup['ServiceManager']
    
  # Disable HistorySvc
  if setup.has_key('HistorySvc') :
    print "WARNING\t",'Manual disable %s/%s' % (setup['HistorySvc'].getName(),setup['HistorySvc'].getAlias())
    setup['HistorySvc'].setProperty('Dump', False)
    setup['HistorySvc'].setProperty('Activate', False)
    setup['HistorySvc'].setProperty('OutputFile', '')
  if setup.has_key('ApplicationMgr') :
    setup['ApplicationMgr'].setProperty('ActivateHistory', False)
  
  #Delete CaloRunClusterCorrections tools
  masterAlgAliases = []
  for alg in setup.values() :
    if alg.getName() == "CaloRunClusterCorrections" :
      masterAlgAliases += [alg.getAlias()]

  for masterAlgAlias in masterAlgAliases :
    subtools = [algAlias for algAlias in setup.keys() if algAlias.startswith(masterAlgAlias+".")]
    for subtool in subtools :
      print "WARNING\t",'Manual delete of CaloRunClusterCorrections tool %s/%s' % (setup[subtool].getName(), subtool)
      setup.pop(subtool)
      
    # Delete the children of masterAlgAlias
    for child in setup[masterAlgAlias].getChildren() :
      print "WARNING\t",'Manual delete of CaloRunClusterCorrections tool %s/%s' % (child.getName(), child.getlAlias())
      setup[masterAlgAlias].removeChild(child.getAlias())
  
  # Replace empty properties of the H1WeightToolCSC12Generic
  for alg in setup.values() :
    if alg.getName() in ('H1WeightToolCSC12Generic', 'MCNumInversionResponse') :
      properties = alg.getProperties()
      for k,v in properties.items() :
        if k not in ('prefix') and v in ('',None):
          alg.getProperties()[k]='""'
          print "WARNING\t",'Manual set of %s/%s.%s = \'%s\'' % (alg.getName(),alg.getAlias(),k, setup[alg.getAlias()].getProperties()[k])
      
  # Set the exceptions for the point 1
  if ConversionForPointOne :
    #if setup.has_key('muFast_Muon') :
      #setup['muFast_Muon'].setProperty('OnlineRun',True)
    
    for key in setup.keys() :
      if setup[key].getName() == 'muFast' :
        print "WARNING\t",'Modification for P1 - %s/%s.%s = %s' % (setup[key].getName(),setup[key].getAlias(),'OnlineRun',True)
        setup[key].setProperty('OnlineRun',True)
  
  
if __name__ == '__main__':

    short_opt = "h?"
    long_opt = ['help','ForPointOne']
    
    if len(sys.argv) <= 1:
        usage()
        sys.exit(1)
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    # Read the options
    for o, a in opts:
        if o in ('-h', '-?', '--help'):
            usage()
            sys.exit(0)
        # Sylvie 11/07/2008
        # This is a tempory hack to set "OnlineRun" to "True" for muFast_Muon (muFast)
        # when preparing configuration for Point1.
        if o in ('--ForPointOne'):
            ConversionForPointOne = True
        else:
            ConversionForPointOne = False

    infilename  = sys.argv[-1] # The input text file is always the last argument
    if len(infilename)-infilename.rfind(".txt") == 4:
        (root,ext) = os.path.splitext(infilename)
        if ext==".txt":
            outfilename = root+".xml"
        else:
            outfilename = root+ext+".xml"
    else:
        print "\nError, the file to be converted (last argument) is not a .txt file.\n"
        usage()
        sys.exit(1)


    print "Input   : ", infilename
    print "Output  : ", outfilename

    fin  = open(infilename, 'r')
    lines = fin.readlines()
    fin.close()


    currentPart = ''
    globalProp = {}

    globalStart = -1
    globalEnd = -1
    servicesStart = -1
    servicesEnd = -1
    algsStart = -1
    algsEnd = -1
    toolsStart = -1
    toolsEnd = -1
    pyStart = -1
    pyEnd = -1
    
    # All the read algorithms will be stored in the setup dictionairy
    setup = {}
    
    # Loop trough lines of the file - read GLOBAL part last
    startPart = start = time.time()
    startPartLine = lineind = 0
    nlines = len(lines)
    currentPart = None
    while lineind < nlines :
      currentLine = lines[lineind].rstrip('\n').strip()
      if currentLine=='GLOBAL':
        if currentPart and currentPart != 'GLOBAL' :
          print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
          startPart = time.time()
          startPartLine = lineind+1
        currentPart = 'GLOBAL'
        print 'PROCESSING %s ....' % (currentPart)
        globalStart = lineind
        lineind += 1
        continue
      elif currentLine=='SERVICES':
        if currentPart and currentPart != 'SERVICES' :
          print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
          startPart = time.time()
          startPartLine = lineind+1
        servicesStart = lineind
        currentPart = 'SERVICES'
        print 'PROCESSING %s ....' % (currentPart)
        lineind += 1
        continue
      elif currentLine=='ALGORITHMS':
        if currentPart and currentPart != 'ALGORITHMS' :
          print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
          startPart = time.time()
          startPartLine = lineind+1
        algsStart = lineind
        currentPart = 'ALGORITHMS'
        print 'PROCESSING %s ....' % (currentPart)
        lineind += 1
        continue
      elif currentLine=='ALGTOOLS':
        if currentPart and currentPart != 'ALGTOOLS' :
          print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
          startPart = time.time()
          startPartLine = lineind+1
        toolsStart = lineind
        currentPart = 'ALGTOOLS'
        print 'PROCESSING %s ....' % (currentPart)
        lineind += 1
        continue
      elif currentLine=='SETUP':
        if currentPart and currentPart != 'SETUP' :
          print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
          startPart = time.time()
          startPartLine = lineind+1
        pyStart = lineind
        currentPart = 'SETUP'
        print 'PROCESSING %s ....' % (currentPart)
        lineind += 1
        continue
      
      # Skip the global part - until other part isn't found
      if currentPart == 'GLOBAL' :
        line = lines[lineind].rstrip('\n').strip()
        if len(line.split()) > 1 :
          componentAlias = line.split()[0].strip()
          if not setup.has_key(componentAlias) : # New component - this one should probably be corrected - by typeLUT
            hcpname = 'unknown'
            alg = HLTAlgorithm(alias=componentAlias, name=hcpname, OutputLevel=5)
            # Set highest priority to the GLOBAL part properties
            alg.setLevel('BOTH')
            setup[componentAlias] = alg
          
          property = line.replace(line.split()[0],'',1).strip()
          propertyName = eval(property.split(':')[0])
          propertyValue= property.replace(property.split(':')[0],'',1).lstrip(':').strip()
          # Convert the property
          try :
            propertyValue = eval(propertyValue)
          except :
            pass
          propertyValue = applyPropertyValueExceptions(propertyName, propertyValue)
          # Set the property of the algorithm
          setup[componentAlias].setProperty(propertyName,propertyValue)
        lineind += 1
        continue
      
      if currentPart in ('SETUP','SERVICES','ALGORITHMS','ALGTOOLS') :
        if currentLine.startswith('>>') :
          componentStart = lineind+1
          componentEnd = componentStart
          for componentind in range(componentStart, nlines) :
            if lines[componentind].startswith('>>') : break
            if lines[componentind].rstrip('\n').strip() in ('SETUP','SERVICES','ALGORITHMS','ALGTOOLS','GLOBAL') : break
            componentEnd = componentind
          
          alg = convertLinesToAlg(lines[componentStart:componentEnd])
          # Skip the lines already used for the component
          lineind = componentEnd +1
          # Set default type of algorithm
          alg.alg_type='Algorithm'
          if currentPart == 'SETUP' : # If an algorithm exist - don't set the properties: set the lowest trigger level of the algorithm
            alg.setLevel('L2')
            #alg.setLevel('BOTH')
          elif currentPart in ('SERVICES','ALGORITHMS','ALGTOOLS') : # Set the properties of the algorithms
            alg.setLevel('EF')
            # Set the type of the algorithm
            if currentPart == 'SERVICES' :
              alg.alg_type='Service'
            elif currentPart == 'ALGORITHMS' :
              alg.alg_type='Algorithm'
            elif currentPart == 'ALGTOOLS' :
              alg.alg_type='PublicTool'

          algs = []
          
          # Strip the AthSequencer Algorithm
          if alg.getName() == 'AthSequencer' :
            algs.extend(stripAthSequencer(alg))
          else :
            algs.append(alg)
          
          # Remove steering algorithms from private Tools of the HLT::TrigSteer algorithm
          for alg in algs :
            if alg.getName() == 'HLT::TrigSteer' and setup.has_key(alg.getAlias()):
              for child in alg.getChildren():
                steerAlgAlias = '.'.join((child.getAlias().split('.')[1:]))
                # Steering algorithms stay in the GLOBAL part alone
                if setup.has_key(steerAlgAlias) :
                  #print "Trigger Steering algorithm found: %s/%s" % (child.getName(),child.getAlias())
                  setup[steerAlgAlias].alg_type='SteerAlg'
                  setup[steerAlgAlias].setName(child.getName())
                  child.setAlias(steerAlgAlias)
                  child.alg_type ='SteerAlg'
                  algs.append(child)
                  alg.removeChild(child.getAlias())
              #print alg
          
          # Flatten the algorithms and process them
          flatalgs = []
          for alg in algs :
            flatalgs.extend(alg.flattenAlg())
          
          # Test if the algorithm is present in the setup already
          for alg in flatalgs :
            if currentPart == 'SETUP' : # If an algorithm exist - don't set the properties: set the lowest trigger level of the algorithm
              alg.setLevel('L2')
              #alg.setLevel('BOTH')
            elif currentPart in ('SERVICES','ALGORITHMS','ALGTOOLS') : # Set the properties of the algorithms
              alg.setLevel('EF')
              
            alias = alg.getAlias()
            if setup.has_key(alias) :
              # Set the algorithm name
              if setup[alias].getName().lower() in ('unknown','',None):
                setup[alias].setName(alg.getName())
              elif setup[alias].getName() != alg.getName(): # Compare existing names
                print "WARNING:\t","Incompatible algorithms: existing - %s/%s vs. new - %s/%s" % (setup[alias].getName(), setup[alias].getAlias(), alg.getName(),alg.getAlias())
              
              # Set python package and python class 
              if currentPart=='SETUP' :
                # Set the python class name
                if setup[alias].getPy_name() and setup[alias].getPy_name() != alg.getPy_name() :
                  print 'WARNING:\t','Reset %s/%s algorithm py_name to %s (previous value: %s)' % (setup[alias].getName(),alias,setup[alias].getPy_name(),alg.getPy_name())
                setup[alias].py_name = alg.getPy_name()
                
                # Set the python class package
                if setup[alias].getPy_package() and setup[alias].getPy_package() != alg.getPy_package() :
                  print 'WARNING:\t','Reset %s/%s algorithm py_package to %s (previous value: %s)' % (setup[alias].getName(),alias,setup[alias].getPy_package(),alg.getPy_package())
                setup[alias].py_package = alg.getPy_package()
              # End of if currentPart=='SETUP' :
              
              # Compare properties of existing algorithm and current algorithm if one of the algorithms is from python dump
              if ("L2" in (alg.getLevel(), setup[alias].getLevel())) and not ((alg.getLevel(), setup[alias].getLevel())==("L2","L2")):
                pyAlgProperties = alg.getProperties()
                glAlgProperties = setup[alias].getProperties()
                if alg.getLevel()!="L2" :
                  pyAlgProperties = setup[alias].getProperties()
                  glAlgProperties = alg.getProperties()

                for k,v in pyAlgProperties.items():
                  # Compare all the simple type properties
                  #if glAlgProperties.has_key(k) and isinstance(pyAlgProperties[k],(str,unicode, bool,int,float,long)):
                  #if glAlgProperties.has_key(k) and isinstance(pyAlgProperties[k],(bool,int,float,long)):
                  if glAlgProperties.has_key(k):
                    if pyAlgProperties[k] !=  glAlgProperties[k] and not k.startswith("Audit") and k not in ('OutputLevel'):
                      if isinstance(pyAlgProperties[k],(str,unicode)) and "%s/%s" % (glAlgProperties[k],glAlgProperties[k]) != pyAlgProperties[k]:
                        print "WARNING:\t", "C++/Python property %s/%s.%s value mismatch" % (alg.getName(), alg.getAlias(),k )
                        print "\tC++ value:\t%s" % (glAlgProperties[k])
                        print "\tPython value:\t%s" % (pyAlgProperties[k])
                        # Set the property to the default Python value
                        if glAlgProperties.has_key(k) and isinstance(pyAlgProperties[k],(bool,int,long)):
                          setup[alias].setProperty(k,pyAlgProperties[k])
                          alg.setProperty(k,pyAlgProperties[k])
              
              # Merge algorithms:
              success = setup[alias].mergeAlgorithm(alg)
            else :
              # Append new algorithm
              #if currentPart == 'SETUP' : print 'SETUP\t new algorithm %s/%s appended to setup' % (alg.getName(),alg.getAlias())
              setup[alias] = alg

        continue
      lineind += 1
    # End of loop over lines 
    if currentPart :
      print 'Finished %s in %fs - %d lines processed' % (currentPart, time.time()-startPart, lineind-startPartLine)
    
    print '%d algorithms found' % len(setup)
    
    startCorrections = time.time()
    # Here the setup has been filled with correct information
    # Fix the unknowns using typeLUT
    for k in setup.keys() :
      if setup[k].getName().lower() in ('unknown','') :
        # remove ToolSvc from the alias:
        alias = k
        if alias[:8] == 'ToolSvc.' :
          alias = alias.replace('ToolSvc.','',1)
        # Exception for ApplicationMgr
        if alias == 'ApplicationMgr' :
          setup[k].setName('ApplicationMgr')
          continue
        
        if alias in typeLUT.keys() :
          print 'WARNING\t','fixing %s type of algorithm %s using typeLUT value %s' % (setup[k].getName(), setup[k].getAlias(), typeLUT[alias])
          setup[k].setName(typeLUT[alias])
        else :
          print 'ERROR\t','can not determine type of the algorithm %s/%s' % (setup[k].getName(),setup[k].getAlias())
     
    corrtime = time.time() - startCorrections
    if corrtime > 0 : print 'The unknown types fixed in %fs' % (corrtime)
    
    # Fix ApplicationMgr TopAlg property and find and set top algorithms type to TopAlg
    #if setup.has_key('ApplicationMgr') and setup['ApplicationMgr'].getProperties().has_key('TopAlg') :
      #print "Fixing ApplicationMgr.TopAlg ...."
      #startAppTopAlg = time.time()
      #topAlgs = setup['ApplicationMgr'].getProperties()['TopAlg']
      #correctedTopAlgs = []
      #for topAlg in topAlgs :
        #topAlgName = topAlg.split('/')[0].strip()
        #topAlgAlias = topAlg.replace(topAlgName,'',1).lstrip('/').strip()
        ## If the type is AthSequencer - look for its property Members and is deleted from the setup
        #if topAlgName == 'AthSequencer' :
          #correctedTopAlgs.extend(getCorrectTopAlgsFromAthSequencer(setup,topAlgAlias ))
        #else :
          #correctedTopAlgs.append(topAlg)
      
      #setup['ApplicationMgr'].setProperty('TopAlg',correctedTopAlgs)
      #print "ApplicationMgr.TopAlg fixed in %fs" % (time.time()-startAppTopAlg)
      
      #print "Setting TopAlg type ...."
      #startAppTopAlg = time.time()
      ## Fix ApplicationMgr TopAlg property
      #topAlgs = setup['ApplicationMgr'].getProperties()['TopAlg']
      #ntopAlgs = 0
      #if isinstance(topAlgs,list) :
        #for topAlg in topAlgs :
          #topAlgAlias = topAlg.split('/')[0].strip()
          #if len(topAlg.split('/')) >= 2 :
            #topAlgAlias = topAlg.replace(topAlgAlias,'',1).lstrip('/').strip()
          
          #if setup.has_key(topAlgAlias) :
            #setup[topAlgAlias].alg_type='TopAlg'
            #ntopAlgs += 1
          #else :
            #print 'WARNING - Top algorithm %s not found in the setup' % (topAlgAlias)
      #print "%d TopAlg algorithms type assigned in %fs" % (ntopAlgs,time.time()-startAppTopAlg)
    
    # Think about any exceptions which has to be set manually
    applyManualExceptions(setup)
    
    # Fix the child - parent relations
    algorithms = setup.values()
    startFixCPRel = time.time()
    print "Fixing child-parent relations ...."
    #algorithms = fixChildParentRelations(algorithms)
    fixChildParentRelations(algorithms)
    print 'Child - Parent relations fixed in %fs' % (time.time()-startFixCPRel)
    print 'Number of parent algorithms: %d' % (len(algorithms))
    
    # On this place the algorithms should be already fixed and may be converted to the XML format and saved in the XML file
    print "Convert the setup to XML"
    startXML = time.time()
    fout  = open(outfilename, 'w')
    import xml.dom.minidom
    impl = xml.dom.minidom.getDOMImplementation()
    document = impl.createDocument(None, "setup", None)
    
    for alg in algorithms :
      alias = alg.getAlias()
      #if alias[:8] == 'ToolSvc.' :
        #alias = alias.replace('ToolSvc.','',1)
      if '.' in alias: print "WARNING - %s/%s should be a child algorithm" % (alg.getName(), alg.getAlias())
      docElement = document.documentElement
      docElement.appendChild(hltAlgorithmtoXMLElement(alg, document))
    print "Setup converted to XML document in %fs" % (time.time() - startXML)
    # Save the XML document
    document.writexml(fout, addindent='  ', newl='\n')
    fout.close()
    
    sys.exit(0)
