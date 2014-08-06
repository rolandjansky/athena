// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerObjectSourceFillerTool.h 358946 2011-04-15 08:58:14Z krasznaa $
#ifndef TRIGGERD3PDMAKER_TRIGGEROBJECTSOURCEFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGGEROBJECTSOURCEFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// Trigger include(s):
#include "TrigObjectMatching/TrigMatchTool.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Base class for "object source" filler tools
    *
    *         In some cases it's not easy to differentiate between trigger
    *         objects which have been reconstructed by different algorithms,
    *         because they often end up in the same SG collection.
    *
    *         The prime example is the EF jets. There we only get one SG
    *         collection for all the jets reconstructed at EF, and because
    *         there are a few algorithms running in parallel, most physical
    *         objects appear >2 times in the collection.
    *
    *         The tool takes a list of trigger name - variable name pairs.
    *         It checks if a given object was reconstructed in the specified
    *         trigger chain (the object is not required to have passed the
    *         chain, only to have been reconstructed in it), and if it was
    *         set the appropriate flag to true.
    *
    *         Should move some of the implementation into a non-template class
    *         similar to how TriggerObjectDecisionFillerTool is implemented,
    *         but that will come in a later revision...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 358946 $
    * $Date: 2011-04-15 10:58:14 +0200 (Fri, 15 Apr 2011) $
    */
   template< class T >
   class TriggerObjectSourceFillerTool : public BlockFillerTool< T > {

   public:
      /// Regular AlgTool constructor
      TriggerObjectSourceFillerTool( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent );

      /// Standard AlgTool initialization function
      virtual StatusCode initialize();

      /// Book variables for this block
      virtual StatusCode book();
      /// Fill the variables for this block
      virtual StatusCode fill( const T& p );

   private:
      ToolHandle< TrigMatchTool > m_trigMatch; ///< TrigMatchTool handle

      /// List of sources, see format description in the constructor
      std::vector< std::string > m_sourceNames;

      /**
       * @short Configuration for one filled variable
       *
       *        One variable created by this tool is made up of a trigger chain
       *        name, and a variable name for the D3PD. Something like:
       *        <code>trig="EF_j135_a4_EFFS";name="a4"</code>
       */
      struct Source {
         std::string trig; ///< Trigger chain name
         std::string name; ///< Variable name for the D3PD
         bool* var; ///< The actual variable written into the D3PD
      }; // struct Source

      std::vector< Source > m_sources; ///< List of source objects

   }; // class TriggerObjectSourceFillerTool

} // namespace D3PD

// Include the implementation:
#include "TriggerObjectSourceFillerTool.icc"

#endif // TRIGGERD3PDMAKER_TRIGGEROBJECTSOURCEFILLERTOOL_H
