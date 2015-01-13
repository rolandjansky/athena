/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PILEUPREWEIGHTING_PILEUPREWEIGHTINGTOOL_H
#define PILEUPREWEIGHTING_PILEUPREWEIGHTINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"


// Local include(s):
#include "PileupReweighting/IPileupReweightingTool.h"
#include "PileupReweighting/TPileupReweighting.h"

namespace CP {

   /// Implementation of the PileupReweighting tool
   ///
   /// @author Will Buttinger
   ///

   class PileupReweightingTool : public virtual IPileupReweightingTool,
                             public asg::AsgTool, public Root::TPileupReweighting {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( PileupReweightingTool, CP::IPileupReweightingTool )

   public:
      /// Constructor for standalone usage
      PileupReweightingTool( const std::string& name );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Finalize - can call the WriteToFile for us
      virtual StatusCode finalize();

      /// Uses Input and Output properties to get and decorate an eventinfo object
      virtual StatusCode execute();

      /// Decorates with: 
      /// MC: PileupWeight , RandomRunNumber, RandomLumiBlockNumber 
      /// Data: data weights from trigger expressions you've specified 
      virtual void apply ( const xAOD::EventInfo* eventInfo );

      /// Get the dataWeight used to 'unprescale' data collected from a given trigger combination
      virtual double dataWeight ( const xAOD::EventInfo* eventInfo, TString trigger, bool mu_dependent=true );


      /// Get pointer to the underlying tool - expert use only
      virtual Root::TPileupReweighting* get() { return this; }


   private:
      bool m_inConfigMode;
      //Root::TPileupReweighting* m_tool;

      std::vector<std::string> m_prwFiles;
      std::vector<std::string> m_lumicalcFiles;
      std::vector<std::string> m_lumicalcTriggers;

      std::vector<std::string> m_dataWeightTriggers;
      bool m_useMuDependent;

      std::string m_prefix;
      std::string m_inputKey, m_outputKey;

      int m_unrepdataAction;
      int m_defaultChannel;
      double m_dataScaleFactor;
      std::string m_usePeriodConfig;


      void checkInit(const xAOD::EventInfo* eventInfo);

      // MN:  this prevents ROOT dict generator from complaining about lack of ClassDef()
      // Note: inheriting from TObject and not having ClassDef makes this class unsuitable for I/O
   public:
      static Version_t Class_Version() { return 0; }

   }; // class PileupReweightingTool

} // namespace CP

#endif // 
