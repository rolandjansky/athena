// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EntityResolver.h 360889 2011-04-21 15:06:59Z krasznaa $
#ifndef TRIGCONFMUCTPI_ENTITYRESOLVER_H
#define TRIGCONFMUCTPI_ENTITYRESOLVER_H

// Xerces-c include(s):
#include <xercesc/sax/EntityResolver.hpp>

/**
 *  @short Private namespace for the MuCTPI configuration class(es)
 *
 *         To make it easier to define classes/functions in this shared
 *         library, it seemed easiest to introduce a new namespace...
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 360889 $
 * $Date: 2011-04-21 17:06:59 +0200 (Thu, 21 Apr 2011) $
 */
namespace Muctpi {

   /**
    *  @short Class finding the DTD for reading the MuCTPI XMLs
    *
    *         Since the MuCTPI XMLs don't have their DTDs in a convenient location
    *         like on a website, and specifying the full filesystem path to these
    *         files would be stupid, we have to use this class for looking up the
    *         DTD files while the XMLs are parsed.
    *
    *         It could probably also be used to find the LUT XMLs themselves,
    *         but that's usually done by the user code.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 360889 $
    * $Date: 2011-04-21 17:06:59 +0200 (Thu, 21 Apr 2011) $
    */
   class EntityResolver : public xercesc::EntityResolver {

   public:
      /// Look up the location of the specified file
      xercesc::InputSource* resolveEntity( const XMLCh* const publicId,
                                           const XMLCh* const systemId );

   }; // class EntityResolver

} // namespace Muctpi

#endif // TRIGCONFMUCTPI_ENTITYRESOLVER_H
