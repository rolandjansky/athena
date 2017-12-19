/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerInputReader.h 667886 2015-05-18 17:26:59Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGERINPUTREADER_H
#define CALORINGERTOOLS_CALORINGERINPUTREADER_H

// STL includes:
#include <string>

// Base includes:
#include "CaloRingerTools/ICaloRingerInputReader.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

//CxxUtils for override final  
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// Other interfaces include:
#include "CaloRingerTools/ICaloRingsBuilder.h"

namespace Ringer {

class CaloRingerInputReader : public ::AthAlgTool, 
                              virtual public ICaloRingerInputReader
{

  public:

    /// @name CaloRingerInputReader ctors and dtors:
    /// @{
    /** 
     * @brief Default constructor
     **/
    CaloRingerInputReader(const std::string& type,
                     const std::string& name,
                     const ::IInterface* parent);

    /** 
     * @brief Destructor
     **/
    ~CaloRingerInputReader();
    /// @}
    
    /// Tool main methods:
    /// @{
    /** 
     * @brief initialize method 
     **/
    virtual StatusCode initialize() ATH_OVERRIDE;
    /** 
     * @brief read electrons and populates @name decoMap with them and their
     * respective CaloRings.
     **/
    virtual StatusCode execute() ATH_OVERRIDE;
    /** 
     * @brief finalize method
     **/
    virtual StatusCode finalize() ATH_OVERRIDE;
    /// @}


  protected:

    /// Tool CaloRingerInputReader props (python configurables):
    /// @{
    /** 
     * @brief Tool to build CaloRings.
     **/
    PublicToolHandle<ICaloRingsBuilder> m_crBuilder {this,
	"crBuilder", "", "CaloRingsBuilder Tool"};
    /// @}

    /// Tool CaloRingerInputReader props (non configurables):
    /// @{
    /// If CaloRings builder is available
    Gaudi::Property<bool> m_builderAvailable {this, 
	"builderAvailable", false, "Whether Builder Tool is available."};
    ///  @}

    template<class T> class writeDecorHandles;

    /** @brief helper class to contain write docoration handle keys for selectors */
    template<class T> class writeDecorHandleKeys {
    public:
      void setContName(const std::string &contName) {m_contName = contName;};

      StatusCode addSelector(const std::string &selName);

      friend class writeDecorHandles<T>;

    private:
      std::vector<SG::WriteDecorHandleKey<T> > m_selKeys;
      std::vector<SG::WriteDecorHandleKey<T> > m_isEMKeys;
      std::vector<SG::WriteDecorHandleKey<T> > m_lhoodKeys;
      std::string m_contName;
    };
    
    /** @brief helper class to contain write decoration handles for selectors*/
    template<class T> class writeDecorHandles {
    public:
      writeDecorHandles(const writeDecorHandleKeys<T>& keys); // constructor
      
      SG::WriteDecorHandle<T, char>& sel(size_t i) {return m_sel[i];};
      SG::WriteDecorHandle<T, unsigned int>& isEM(size_t i) {return m_isEM[i];};
      SG::WriteDecorHandle<T, float>& lhood(size_t i) {return m_lhood[i];};

    private:
      std::vector<SG::WriteDecorHandle<T, char> > m_sel;
      std::vector<SG::WriteDecorHandle<T, unsigned int> > m_isEM;
      std::vector<SG::WriteDecorHandle<T, float> > m_lhood;

    };

};
  
  template<class T> 
  StatusCode 
  CaloRingerInputReader::writeDecorHandleKeys<T>::addSelector(const std::string &selName)
  {
    m_selKeys.emplace_back(m_contName + "." + selName);
    ATH_CHECK(m_selKeys.back().initialize());
    
    m_isEMKeys.emplace_back(m_contName + "." + selName + "_isEM");
    ATH_CHECK(m_isEMKeys.back().initialize());
    
    m_lhoodKeys.emplace_back(m_contName + "." + selName + "_output");
    ATH_CHECK(m_lhoodKeys.back().initialize());
    
    return StatusCode::SUCCESS;
  } 
  
  template<class T> 
  CaloRingerInputReader::writeDecorHandles<T>::writeDecorHandles(const writeDecorHandleKeys<T>& keys)
  {
    for (size_t i = 0; i < keys.m_selKeys.size(); i++) {
      m_sel.emplace_back(keys.m_selKeys[i]);
      m_isEM.emplace_back(keys.m_isEMKeys[i]);
      m_lhood.emplace_back(keys.m_lhoodKeys[i]);
    }
  }
  
} // namespace Ringer

#endif // CALORINGERTOOLS_CALORINGERINPUTREADER_H
