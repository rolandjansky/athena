/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



/**

@page XMLCoreParser_page 

This package provides a simplified API towards XML DOM Parsers. 
It hides most of what is required to setup a proper DOM parser, while keeping free access to the DOM nodes.


It supports declaration of keyed callbacks associated with element names.

   @author Christian Arnault arnault@lal.in2p3.fr

   @b Main @b Features

   <ul>
   
     <li>The XMLCoreParser class

     A generic interface for building simple XML DOM parsers.

     This basic parser may be directly instanciated or derived to
     supply new features.


     <ul>
     
     <li>the XMLCoreParser::parse function calls the XML parser and
     returns the top node corresponding to the document.</li>

     <li>the XMLCoreParser::visit function calls the XML parser and
     traverses the document tree while applying all registered factory
     objects when appropriate (ie for XML element names that match the
     registered keys)</li>

     <li>the XMLCoreParser::register_factory function registers one
     specialized factory object associated with a key, expecting a
     match with XML element names.</li>

     <li>the XMLCoreParser::register_default_factory function
     registers one factory object used during the visit when no
     registered keyed factory matches an XML element name.</li>

     <li>the XMLCoreParser::register_external_entity function registers one
     entry in the external entity lookup table. This lookup table is able to override the default
     association made between external entities and file names originally specified in
     XML source files.
     <p>
     One may associate an external entity with either:

        <ol>
          <li>A file name, in which case this file name will be read in place of the one specified in the XML source file</li>
	  <li>the string "NULL", in which case the external entity is ignored</li>
          <li>an empty string, in which case the value specified in the XML source file is used</li>
	</ol>

	<p>Examples are:

	<pre>

	XMLCoreParser p;

        p.register_external_entity ("InnerDetector", "OtherInDet.xml");
        p.register_external_entity ("LArCalorimeter", "NULL");
        p.register_external_entity ("MuonSpectrometer", "");

	p.parse (file_name);

	</pre>

     </li>

     <li>the XMLCoreParser::register_text_entity function registers one
     entry in the external entity lookup table to be associated with an XML text during the parsing.

	<p>Examples are:

	<pre>

	XMLCoreParser p;

        p.register_text_entity ("ABC", "<abc>some text</abc>");

	p.parse (file_name);

	</pre>

     </li>

     </ul>

     <li>The XMLCoreFactory class 

     This is the abstract base class used to create keyed callbacks to
     XML elements


     Specialized classes can be derived, to act as callbacks
     associated with XML elements when encoutered during the
     XMLCoreParser::visit operation.

     </ul>

*/
