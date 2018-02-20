/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysMetadataBase.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// - w.w., 2017-01-22: Added use of BPhysMetaDataTool.
//
// Store JO metadata in the output file.
//
// It uses the BPhysMetaDataTool (default) or the IOVDbMetaDataTool to
// store job option information as metadata in a specific branch whose
// name needs to prefixed by the deriviation format name.
// Note: Metadata stored by the IOVDbMetaDataTool is not readable on
// 'RootCore' level.
//
// This is a base class.  Inherit from it to add the job options you want
// to store.  For a usage example, see
//   Bmumu_metadata.h / Bmumu_metadata.cxx
// and
//   BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BPhysMetadataBase_H
#define DERIVATIONFRAMEWORK_BPhysMetadataBase_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class BPhysMetadataBase : virtual public AthAlgTool,
    virtual public IAugmentationTool {
    public: 
      BPhysMetadataBase(const std::string& t, const std::string& n,
			const IInterface* p);

      virtual StatusCode initialize();
      virtual StatusCode finalize();
      
      virtual StatusCode addBranches() const;

  protected:
      virtual void recordPropertyI(const std::string& name, int         val);
      virtual void recordPropertyD(const std::string& name, double      val);
      virtual void recordPropertyB(const std::string& name, bool        val);
      virtual void recordPropertyS(const std::string& name, const std::string& val);
	
      virtual void recordPropertyVI(const std::string& name, const std::vector<int>&    val);
      virtual void recordPropertyVD(const std::string& name, const std::vector<double>& val);
      virtual void recordPropertyVB(const std::string& name, const std::vector<bool>&   val);
      virtual void recordPropertyVS(const std::string& name,
				    const std::vector<std::string>& val);
	
  private:
      virtual StatusCode saveMetaDataBPhys()                      const;
      virtual std::string buildFolderName(const std::string& fname="")   const;
      virtual std::string vecToString(const std::vector<int>& v)         const;
      virtual std::string vecToString(const std::vector<double>& v)      const;
      virtual std::string vecToString(const std::vector<bool>& v)        const;
      virtual std::string vecToString(const std::vector<std::string>& v) const;
      
  private:
      /// Object accessing the output metadata store
      mutable ServiceHandle< StoreGateSvc > m_outputMetaStore;
      
      // job options
      std::string m_derivationName;
      std::string m_mdFolderName;
      std::string m_prefix;
      
      // maps for different types of JOs
      std::map<std::string, int>                       m_propInt;
      std::map<std::string, double>                    m_propDouble;
      std::map<std::string, bool>                      m_propBool;
      std::map<std::string, std::string>               m_propString;
      std::map<std::string, std::vector<int> >         m_propVInt;
      std::map<std::string, std::vector<double> >      m_propVDouble;
      std::map<std::string, std::vector<bool> >        m_propVBool;
      std::map<std::string, std::vector<std::string> > m_propVString;
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BPhysMetadataBase_H
