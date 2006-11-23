/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetWebDisplay Package
@author  shaun.roe@cern.ch
	
@section InDetWebDisplayIntro Introduction

This package contains the tools which enable a Jive_XML file containing an inner detector (barrel) to be viewed in the Firefox web browser as a cross section in r-phi.

@section InDetWebDisplayOverview Tool Overview
The principle tools are to be found in the 'run' directory, and should be copied to the run directory of your job, where the JiveXML files will be produced.
There are two files:
	ffGeometry.svg
	JiveXML_event.xsl

Once the JiveXML file has been produced, open the firefox browser and use the 'file:open file..' menu item to open the JiveXML file. After a second or two (depending on your computers speed), a display should appear.
   
@section InDetWebDisplayCompat Compatibility
The tool is currently only compatible with the Firefox browser, versions 1.5 or later.

@section InDetWebDisplayEx Examples
Examples of the output can be found in the 'examples' subdirectory. There are two examples in svg format and two which were read into Adobe illustrator and then exported as a pdf and a tiff file. The two svg files are viewable in the Macintosh 'Safari' browser *if* you have the adobe svg plug-in, and then only on PPC macintoshes or on Intel macintoshes if they are running Safari in 'Rosetta'. The second svg, Combined_2015_87.svg, is also viewable in Firefox.

In addition, there is a sample JiveXML file in the 'xml' directory which can be used to test the transform.

@section InDetWebDisplayPrint
If you wish to print or save the image, you will quickly discover that firefox does not allow you to do this. The simple approach would be to take a screen shot and save that, but there is the possibility to generate the image in stand-alone mode to produce an svg document, viewable in (for example) Illustrator, Gimp, or Eye Of Gnome. Editing of the image is also possible.

@section InDetWebDisplayAlone Running Stand-Alone
You will need an xslt transform engine; the most common, installed as standard on linux, is xsltproc. To run, type:

xsltproc --novalid -o eventDisplay.svg JiveXML_event.xsl JiveXML_rrrr_eeeee.xml

where rrrr= run number and eeeee= event number for your JiveXML file. The result is the eventDisplay.svg file.

*/
