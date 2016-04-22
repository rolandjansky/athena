# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, re, shutil, sys
import Tools

def CreateWebPage( Name, List, InputTemplate, OutputFile ):
  Prefix = set()
  Variables = set()
  Levels = set()
  Regions = set()
  for El in List:
    El = El.split( '.' )[ 0 ]
    Split = El.split( '___' )
    if len( Split ) == 4:
      Prefix.add( os.path.basename( Split[ 0 ] ) )
      Regions.add( Split[ 1 ] )
      Variables.add( Split[ 2 ] )
      Levels.add( Split[ 3 ] )
    else:
      Prefix.add( os.path.basename( Split[ 0 ] ) )
      Variables.add( Split[ 1 ] )
      Levels.add( Split[ 2 ] )
  if not Name in Prefix: return ''
  #####
  File = open( OutputFile, 'w' ) 
  replacing = '<select id="VARIABLES" onchange="update()" onload="update()" multiple>\n'
  replacing += '<option value="none" disabled="disabled">Choose the variable(s)</option>\n'
  for Var in Tools.NatSorted( list( Variables ) ):
    replacing += '<option value="%s">%s</option>\n' % ( Var, Tools.Beautify( Var ) )
  replacing += '</select>\n'
  #####
  replacing += '<select id="LEVEL" onchange="update()" onload="update()" multiple>\n'
  replacing += '<option value="none" disabled="disabled">Choose the level(s)</option>\n'
  for Level in Levels:
    replacing += '<option value="%s">%s</option>\n' % ( Level, Level )
  replacing += '</select>\n'
  #####
  if Regions:
    replacing += '<select id="REGION" onchange="update()" onload="update()" multiple>\n'
    replacing += '<option value="none" disabled="disabled">Choose the region(s)</option>\n'
    for Region in Tools.NatSorted( list( Regions ) ):
      replacing += '<option value="%s">%s</option>\n' % ( Region, Region )
    replacing += '</select>\n'
  #####
  replacing += '<script>\n'
  replacing += 'function update(){\n'
  replacing += '  var sel_elements = document.getElementsByTagName( "select" );\n'
  replacing += '  var variables = getSelectedValues( sel_elements[ 0 ] )\n'
  replacing += '  var levels = getSelectedValues( sel_elements[ 1 ] )\n'
  if Regions:
    replacing += '  var regions = getSelectedValues( sel_elements[ 2 ] )\n'
  replacing += '  document.getElementById( "myfigure" ).innerHTML = \'\';\n'
  replacing += '  for( var i = 0; i < variables.length; i++ ) {\n'
  replacing += '    for( var j = 0; j < levels.length; j++ ) {\n'
  if Regions:
    replacing += '      for( var k = 0; k < regions.length; k++ ) {\n'
    replacing += '        var plot_name = "plots/%s___" + regions[ k ] + "___" + variables[ i ] + "___" + levels[ j ] + ".png"\n' % Name #list( Prefix )[ 0 ]
  else:
    replacing += '        var plot_name = "plots/%s___" + variables[ i ] + "___" + levels[ j ] + ".png"\n' % Name #list( Prefix )[ 0 ]
  replacing += '        document.getElementById( "myfigure" ).innerHTML += \'<img src="\' + plot_name + \'" alt="\' + plot_name + \'" height=400px>\';\n'
  if Regions:
    replacing += '      }\n'
  replacing += '    }\n'
  replacing += '  }\n'
  replacing += '}\n'
  replacing += '</script>\n'
  #####
  for Line in open( InputTemplate ):
    Line = Line.replace( 'PAGE_HERE', replacing )
    File.write( Line )
  File.close()
  #####
  return os.path.abspath( File.name ) 

def CreateJobInfoPage( OutputFile, theJob ): 
  File = open( OutputFile, 'w' ) 
  File.write( '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html><?php include("https://gartoni.web.cern.ch/gartoni/menu.php"); ?><div id="center">' )
  File.write( '<h3>Configuration of Job: %s</h3><br>' % theJob.Name )
  File.write( str( theJob ) )
  File.write( '</div> <div id="footer"> <p></p> </div> </body> </html>' )
  File.close()
  return os.path.abspath( File.name ) 

def MakeCopy( File ):
  shutil.copy( File, os.path.basename( File ) )
  return os.path.abspath( os.path.basename( File ) )
