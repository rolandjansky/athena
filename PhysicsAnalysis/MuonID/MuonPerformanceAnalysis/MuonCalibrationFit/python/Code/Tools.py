# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, shlex, shutil, subprocess, sys
import ROOT

def SendCommand( the_command ):
  p = subprocess.Popen( shlex.split( the_command ), stdout = subprocess.PIPE, stderr = subprocess.PIPE )
  out, err = p.communicate()
  return ( out, err )

def CreateAndMoveIn( Folder, Mode = 'Ask' ):
  if os.path.isdir( Folder ):
    if Mode == 'Force':
      shutil.rmtree( Folder )
      os.mkdir( Folder )
    elif Mode == 'Allow':
      pass
    elif Mode == 'Stop':
      print '"Stop" option used, cannot create folder %s!' % Folder
      sys.exit()
    else:
      res = raw_input( 'Folder <%s> already exists, do you want to recreate it? [ y/n ]    ' % Folder )
      if res == 'y' or res == 'Y':
        shutil.rmtree( Folder )
        os.mkdir( Folder )
      elif res == 'n' or res == 'N':
        print 'No way to proceed, exiting!'
        sys.exit()
      else:
        print 'No answer given, exiting!'
        sys.exit()
  else:
    os.mkdir( Folder )
  os.chdir( Folder )
  return os.path.abspath( '.' )

def MoveFiles( List, Path ):
  if isinstance( List, list ):
    for Element in List:
      shutil.move( Element, Path )

def MergePaths( first, second ):
  if first == '' or first[ -1 ] == '/':
    pass
  else:
    first += '/'
  return first + second

def TrimRootFile( Input, Output ): 
  InputFile = ROOT.TFile( Input )
  OutputFile = ROOT.TFile( Output, 'recreate' )

  for Key in InputFile.GetListOfKeys():

    Name = Key.GetName()
    if 'FinalResults' in Name:
      Obj = InputFile.Get( Name )
      OutputFile.cd()
      Obj.Write( Name  )

  InputFile.Close()
  OutputFile.Close()
  return Output

def MergeRootFiles( final, the_list ):
  Com = 'hadd %s' % final
  for element in the_list:
    Com += ' %s' % element
  subprocess.Popen( Com, stdout=subprocess.PIPE, shell = True ).communicate()[ 0 ]
  return os.path.abspath( final )

def MergeTxtFiles( final, the_list ):
  my_file = open( os.path.basename( final ), 'w' )
  for index, element in enumerate( the_list ):
    if index == 0:
      my_file.write( open( element ).readlines()[ 0 ] )
    my_file.write( open( element ).readlines()[ 1 ] )
  return os.path.abspath( my_file.name )

def TryInt( s ):
  "Convert to integer if possible."
  try: return int( s )
  except: return s

def NatSortKey( s ):
  "Used internally to get a tuple by which s is sorted."
  import re
  return map( TryInt, re.findall( r'(\d+|\D+)', s ) )

def NatCompare( a, b ):
  "Natural string comparison, case sensitive."
  return cmp( NatSortKey( a ), NatSortKey( b ) )

def NatSort( seq, cmp = NatCompare ):
  "In-place natural string sort."
  seq.sort( cmp )

def NatSorted( seq, cmp = NatCompare ):
  "Returns a copy of seq, sorted by natural string sort."
  import copy
  temp = copy.copy( seq )
  NatSort( temp, cmp )
  return temp

def Beautify( var ):
  import re
  r = re.compile( '(\w+)_(\d+)_(\d+)_pt_(\d+)_(\d+)' )
  m = r.search( var )
  if m:
    return '%s, %s.%s < p_T < %s.%s GeV' % ( BeautifyVariable( m.group( 1 ) ), m.group( 2 ), m.group( 3 ), m.group( 4 ), m.group( 5 ) )
  else:
    return var

def BeautifyVariable( var ):
  if var == 'MassJpsi':
    return 'm(J/&psi;)'
  elif var == 'MassZ':
    return 'm(Z)'
  elif var == 'RhoZ':
    return '&rho;(Z)'
  if 'RoF' in var:
    newvar = var.replace( 'InRoF', '(in RoF)' )
    newvar = newvar.replace( 'OutRoF', '(outside RoF)' )
    newvar = newvar.replace( 'Jpsi', 'J/&psi; ' )
    newvar = newvar.replace( 'Z', 'Z ' )
    newvar = newvar.replace( 'DeltaEta', '&Delta;&eta; ' )
    newvar = newvar.replace( 'DeltaPhi', '&Delta;&phi; ' )
    newvar = newvar.replace( 'PairEta', '&eta;(&mu;&mu;) ' )
    newvar = newvar.replace( 'PairPhi', '&phi;(&mu;&mu;) ' )
    newvar = newvar.replace( 'PairPt', 'p_T(&mu;&mu;) ' )
    newvar = newvar.replace( 'Eta', '&eta; ' )
    newvar = newvar.replace( 'Phi', '&phi; ' )
    newvar = newvar.replace( 'Pt', 'p_T ' )
    return newvar
  else:
    return var

def GetAxisQuality( var ):
  if var == 'eta':
    return '#eta'
  if var == 'abseta':
    return '|#eta|'
  if var == 'phi':
    return '#phi'
  if var == 'absphi':
    return '|#phi|'

def GetHumanTimeFormat( seconds ):
  minutes, seconds = divmod( seconds, 60 )
  hours, minutes = divmod( minutes, 60 )
  result = ''
  for type in [ 'hours', 'minutes', 'seconds' ]:
    if locals()[ type ] != 0:
      result += '%02d %s, ' % ( locals()[ type ], type )
  return result.rstrip( ' ,' ) 

def Strip( Char, List ):
  tempList = []
  for El in List:
    tempList.append( El.rstrip( Char ) )
  return tempList

def GetInfoOnLink( theLink ):
  if 'Configuration.php' in theLink:
    return ( 1, '<a href="%s">Configuration</a>' % theLink )
  elif 'Distributions.php' in theLink:
    return ( 2, '<a href="%s">Control Distributions</a>' % theLink )
  elif 'BackgroundFits.php' in theLink:
    return ( 3, '<a href="%s">Background Fits</a>' % theLink )
  elif 'TemplateFits.php' in theLink:
    return ( 4, '<a href="%s">Template Fits</a>' % theLink )
  elif 'Scans.php' in theLink:
    return ( 5, '<a href="%s">Parameter Scans</a>' % theLink )
  elif 'Overview.php' in theLink:
    return ( 6, '<a href="%s">Overview Plots</a>' % theLink )
  else:
    return ( 0, '<a href="%s">General</a>' % theLink )

def PBSJobEnded( job_name, job_output_file = None ): 
  TempOut = SendCommand( 'qstat %s' % job_name )
  print 'qstat %s' % job_name
  print TempOut
  if TempOut[ 0 ] == '':
    if not job_output_file:
      return True
    with open( job_output_file, 'r') as inF:
      for line in inF:
        if 'INFO leaving with code 0: "successful run"' in line or 'the winner is:' in line:
          return True
  return False
