#ifndef PATINTERFACES_STREAMTESTWRAPPER_H
#define PATINTERFACES_STREAMTESTWRAPPER_H

#include <vector>

#include <TObject.h>

#include <PATInterfaces/SystematicVariation.h>
#include <PATInterfaces/SystematicSet.h>

namespace SysStreamTest
{

  /// Wrapper class for testing the streaming of Systematic objects
  /// @author Steve Farrell <steven.farrell@cern.ch>
  class StreamTestWrapper : public TObject
  {

    public:

      /// Default constructor
      StreamTestWrapper(){};

      /// Constructor
      StreamTestWrapper(CP::SystematicVariation&,
                        CP::SystematicSet&,
                        std::vector<CP::SystematicSet>&);

      /// Print the systematics
      void print();

      /// Get the systematics
      const CP::SystematicVariation& sysVar() { return m_sysVar; }
      const CP::SystematicSet& sysSet() { return m_sysSet; }
      const std::vector<CP::SystematicSet>& sysList() { return m_sysList; }

      ClassDef(StreamTestWrapper, 1)

    private:

      CP::SystematicVariation m_sysVar;
      CP::SystematicSet m_sysSet;
      std::vector<CP::SystematicSet> m_sysList;

  };

}

#endif
