// Dear emacs, this is -*- c++ -*-
// $Id: visitor.h 630795 2014-11-25 04:05:50Z ssnyder $
/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton

***********************************************/
#ifndef TRIGBUNCHCROSSINGTOOL_JSON_VISITOR_H
#define TRIGBUNCHCROSSINGTOOL_JSON_VISITOR_H

// Local include(s):
#ifndef TRIGBUNCHCROSSINGTOOL_JSON_ELEMENTS_H
#include "elements.h"
#endif

namespace json {

   class Visitor {

   public:
      virtual ~Visitor() {}

      virtual void Visit( Array& array ) = 0;
      virtual void Visit( Object& object ) = 0;
      virtual void Visit( Number& number ) = 0;
      virtual void Visit( String& string ) = 0;
      virtual void Visit( Boolean& boolean ) = 0;
      virtual void Visit( Null& null ) = 0;
   };

   class ConstVisitor {

   public:
      virtual ~ConstVisitor() {}

      virtual void Visit( const Array& array ) = 0;
      virtual void Visit( const Object& object ) = 0;
      virtual void Visit( const Number& number ) = 0;
      virtual void Visit( const String& string ) = 0;
      virtual void Visit( const Boolean& boolean ) = 0;
      virtual void Visit( const Null& null ) = 0;
   };

} // End namespace

#endif // TRIGBUNCHCROSSINGTOOL_JSON_VISITOR_H
