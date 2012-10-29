// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingTool.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/StaticBunchCrossingToolSA.h"

namespace Trig {

   /**
    *  @short Athena front for the StaticBunchCrossingToolSA implementation
    *
    *         This implementation of the IBunchCrossingTool interface can be used
    *         in Athena if for some reason none of the more sophisticated implementations
    *         can be made to work. (DB problems for instance.)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class StaticBunchCrossingTool : public AthAlgTool,
                                   public virtual IBunchCrossingTool,
                                   public virtual StaticBunchCrossingToolSA {

   public:
      /// Standard AlgTool constructor
      StaticBunchCrossingTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Gaudi method for querying to tool
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();
      /// Regular AlgTool finalization function
      virtual StatusCode finalize();

   private:
      /// Function setting the maximum bunch spacing parameter
      virtual void setMaxBunchSpacing( int spacing );
      /// Get the maximum bunch spacing parameter
      virtual int maxBunchSpacing() const;

      /// Function setting the length of the front of the bunch train
      virtual void setFrontLength( int length );
      /// Get the length of the front of the bunch train
      virtual int frontLength() const;
      /// Function setting the length of the tail of the bunch train
      virtual void setTailLength( int length );
      /// Get the length of the tail of the bunch train
      virtual int tailLength() const;

      int m_maxBunchSpacing; ///< The maximum bunch spacing that the tool should consider
      int m_frontLength; ///< Length of the "front" of a bunch train
      int m_tailLength; ///< Length of the "tail" of a bunch train

      int m_bgKey; ///< Key of the bunch group configuration to load

   }; // class StaticBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H
