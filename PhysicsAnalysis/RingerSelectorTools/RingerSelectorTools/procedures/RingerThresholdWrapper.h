/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerThresholdWrapper.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_RINGERTHRESHOLDWRAPPER_H
#define RINGERSELECTORTOOLS_PROCEDURES_RINGERTHRESHOLDWRAPPER_H

#define RINGER_PROCEDURE_INCLUDE
#include "RingerProcedureWrapper.h"
#undef RINGER_PROCEDURE_INCLUDE

//#undef NDEBUG

/** 
 * @brief Namespace dedicated for Ringer utilities 
 **/
namespace Ringer {

/**
 * @class IRingerProcedureWrapper
 * @brief Specialization for Threshold procedure.
 **/
template<>
class IRingerProcedureWrapper< Discrimination::IThreshold > : 
  virtual public IRedirectMsgStream,
  virtual public IRingerProcedureWrapperBase
{
  public:
    /**
     * @brief Collection type of PreProcessor Wrappers.
     **/
    typedef typename std::vector< IRingerProcedureWrapper* > WrapperCollection;

    /**
     * @brief Apply threshold to obtain a decision.
     **/
    virtual void getOutput(
        const DepVarStruct &depVar,
        const std::vector<float> &discrOutput,
        std::vector<bool> &decision) const = 0;

    /**
     * @brief Returns whether holden interface collection is empty.
     **/
    virtual bool empty() const ATH_RINGER_OVERRIDE = 0;

    /**
     * @brief Returns this wrapper name
     **/
    virtual const char* name() const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE {
      return wrapName;
    }

#if RINGER_USE_NEW_CPP_FEATURES
    static constexpr const char* wrapName = "RingerThresholdWrapper"; 
#else
    static const char* wrapName; 
#endif

    /** Ensure virtual destructor */
    virtual ~IRingerProcedureWrapper(){;}

    /**
     * Write wrapper to file.
     **/
    static void writeWrapper(const IRingerProcedureWrapper *thresWrapper, 
        const char *fileName);
    
    /**
     * Read wrapper from file.
     **/
    static void read(IRingerProcedureWrapper *&thresWrapper, 
        const char* fileName);

  protected:
    IRingerProcedureWrapper(){;}

};

/**
 * @brief Facilitate access into Threshold Wrappers.
 **/
typedef IRingerProcedureWrapper< Discrimination::IThreshold > IThresWrapper;
/**
 * @brief Facilitate access into Threshold Wrappers collection.
 **/
typedef IThresWrapper::WrapperCollection IThresWrapperCollection;

/**
 * @class RingerProcedureWrapper
 * @brief Specialization for Threshold procedure.
 **/
template < class procedure_t,
  /*EtaDependency*/int etaDependency,
  /*EtDependency*/int etDependency>
class RingerProcedureWrapper<
  procedure_t,
  etaDependency,
  etDependency,
  SegmentationType::NoSegmentation, // Threshold is always unique for
    // all layers, as it is applied for the discriminator output space.
  false,    // isPreProcessor
  false,    // isDiscriminator
  true> :   // isThreshold
    public IThresWrapper,
    public RedirectMsgStream
{
    RINGER_STATIC_ASSERT(
        (Ringer::is_base_of<VariableDependency,procedure_t>::value),
        "RingerProcedureWrapper procedure_t type must have IVariableDependecy inheritance.");
  public:
    /// RingerProcedureWrapper for Threshold procedures typedefs:
    ///@{
    /**
     * @brief typedef to the Ringer Interface variable dependency collection
     *
     * Collection Dimension: [etBin][etaBin]:
     *  
     **/
    typedef typename std::vector < 
        std::vector < procedure_t* > 
      > ThresDepProcCollection;
    ///@}

    /// Ctors:
    ///@{
    /**
     * @brief Build RProc Wrapper 
     **/
    RingerProcedureWrapper(
        const ThresDepProcCollection &thresDepCol):
      m_thresCol(thresDepCol)
    { 
      checkThresCol(); 
      m_thres = m_thresCol[0][0];
    }
    ///@}

    /// Main methods:
    /// @{
    /**
     * @brief Apply threshold to obtain a decision.
     **/
    virtual void getOutput(
        const DepVarStruct &depVar,
        const std::vector<float> &discrOutput,
        std::vector<bool> &decision) const ATH_RINGER_OVERRIDE;
    /// @}


    /// Other utilities
    /// @{
    /**
     * @brief Returns whether holden interface collection is empty.
     **/
    virtual bool empty() const ATH_RINGER_OVERRIDE { return m_thresCol.empty();}

    /**
     * @brief Overloads the setMsgStream from RedirectMsgStream.
     **/
    virtual void setMsgStream(MsgStream *msg) const ATH_RINGER_OVERRIDE;

    /**
     * @brief Release all holden pointer memory
     **/
    void releaseMemory() ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Returns eta dependecy for this wrapper
     **/
    EtaDependency etaDep() const
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL
    {
      return static_cast<EtaDependency>(etaDependency);
    }

    /**
     * @brief Returns et dependecy for this wrapper
     **/
    EtDependency etDep() const
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL
    {
      return static_cast<EtDependency>(etDependency);
    }

    /**
     * @brief Get full wrapper name, static method
     **/
    static std::string staticFullName();

    /**
     * @brief Get full wrapper name
     **/
    std::string fullName() const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Print wrapper content
     **/
    void print(MSG::Level lvl = MSG::DEBUG) const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Write wrapper to TDirectory
     **/
    void write(TDirectory *baseDir, const char *idxStr = "") const
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Read wrapper from TDirectory
     **/
    static RingerProcedureWrapper* read(TDirectory *configDir, 
        unsigned version);
    ///@}

  private:
    /// Private Methods:
    ///@{
    /**
     * @brief Check if threshold interface collection is in good status 
     * (Throws otherwise)
     **/
    void checkThresCol(); 
    ///@}

    /// Private Properties:
    ///@{
    /// @brief holden threshold collection:
    ThresDepProcCollection m_thresCol;
    /// @brief Hold pointer to first threshold from collection
    /// Used when it has no variable dependency.
    procedure_t* m_thres;
    ///@}
};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_RINGERTHRESHOLDWRAPPER_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "RingerThresholdWrapper.icc"
#endif // INCLUDE_HEADER_ONLY
