/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerPreProcessorWrapper.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_RINGERPREPROCESSORWRAPPER_H
#define RINGERSELECTORTOOLS_PROCEDURES_RINGERPREPROCESSORWRAPPER_H

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
 * @brief Specialization for PreProcessor procedure.
 **/
template<>
class IRingerProcedureWrapper< PreProcessing::IPreProcessor > : 
  virtual public IRedirectMsgStream,
  virtual public IRingerProcedureWrapperBase
{
  public:
    /**
     * @brief Collection type of PreProcessor Wrappers.
     **/
    typedef typename std::vector< IRingerProcedureWrapper* > WrapperCollection;

#ifndef RINGER_STANDALONE
    /**
     * @brief Apply PP to transform input space into a new representation
     *
     * This method will pass the information within xAOD::CaloRings and
     * xAOD::TrackParticle to the pre-processors. If any pointer if set to
     * null, then it won't pass this information. Make sure to feed
     * pre-processors with the same information it was adjusted to work with.
     **/
    virtual void applyPreProcessing(
        const DepVarStruct &depVar,
        const xAOD::CaloRings *clrings,
        const TrackPatternsHolder *trackPat,
        std::vector<float> &transformVec) const = 0;

    /**
     * @brief Set the holden CaloRings raw configuration collection.
     **/
    virtual void setRawConfCol(
        const xAOD::RingSetConf::RawConfCollection *crRawConfCol) = 0;

    /**
     * @brief Get the holden CaloRings raw configuration collection.
     **/
    virtual void getRawConfCol(
        const xAOD::RingSetConf::RawConfCollection *&crRawConfCol) const = 0;

    /**
     * @brief Get segmentation type for this pre-processing
     **/
    virtual SegmentationType getSegType() const = 0;
#endif

    /**
     * @brief Apply PP to transform input space into a new representation.
     *
     * This method directly uses transformVec as input and transforms it into
     * the new representation.
     **/
    virtual void applyPreProcessing(
        const DepVarStruct &depVar, 
        std::vector<float> &transformVec) const = 0;

    /**
     * @brief Returns this wrapper name
     **/
    virtual const char* name() const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE {
      return wrapName;
    }

    /**
     * @brief Write all wrappers on ppWrapperCol to TDirectory
     **/
    static void writeCol(const WrapperCollection &ppWrapperCol, 
        TDirectory *configDir);

    /**
     * @brief Read all pre-processing on configDir and append them to
     * IPreProcWrapperCollection
     **/
    static void read(WrapperCollection &ppWrapperCol, 
        TDirectory *configDir, 
        unsigned version);

#if RINGER_USE_NEW_CPP_FEATURES
    static constexpr const char* wrapName = "RingerPreProcessorWrapper"; 
#else
    static const char* wrapName; 
#endif

    /** Ensure virtual destructor */
    virtual ~IRingerProcedureWrapper(){;}

  protected:
    IRingerProcedureWrapper(){;}

};

/**
 * @brief Facilitate access into Threshold Wrappers.
 **/
typedef IRingerProcedureWrapper< PreProcessing::IPreProcessor > IPreProcWrapper;
/**
 * @brief Facilitate access into Threshold Wrappers collection.
 **/
typedef IPreProcWrapper::WrapperCollection IPreProcWrapperCollection;

/**
 * @class RingerProcedureWrapper
 * @brief Specialization for PreProcessing procedure.
 **/
template < class procedure_t,
  /*EtaDependency*/int etaDependency,
  /*EtDependency*/int etDependency,
  /*SegmentationType*/int segType>
class RingerProcedureWrapper<
  procedure_t, 
  etaDependency,
  etDependency,
  segType,
  true,    // isPreProcessor
  false,   // isDiscriminator
  false> : // isThreshold
    public IPreProcWrapper,
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
     * Vector Dimension: [segType][etBin][etaBin]:
     **/
    typedef typename std::vector < 
        std::vector < 
        std::vector < procedure_t* > >
      > PPDepProcCollection;
    ///@}

    /// Ctors:
    ///@{
    /**
     * @brief Build RProc Wrapper 
     **/
    RingerProcedureWrapper(
        const PPDepProcCollection &ppDepCol):
      m_ppCol(ppDepCol),
      m_pp(nullptr),
      m_rsRawConfCol(nullptr),
      m_nRings(0)
    { 
      checkPPCol(); 
      m_pp = m_ppCol[0][0][0];
    }
    ///@}

    /// Main Methods:
    ///@{
#ifndef RINGER_STANDALONE
    /**
     * @brief Apply PP to transform input space into a new representation
     *
     * This method will pass the information within xAOD::CaloRings and
     * xAOD::TrackParticle to the pre-processors. If any pointer if set to
     * null, then it won't pass this information. Make sure to feed
     * pre-processors with the same information it was adjusted to work with.
     **/
    virtual void applyPreProcessing(
        const DepVarStruct &depVar,
        const xAOD::CaloRings *clrings,
        const TrackPatternsHolder *trackPat,
        std::vector<float> &transformVec) const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE;

#endif

    /**
     * @brief Apply PP to transform input space into a new representation.
     *
     * This method directly uses transformVec as input and transforms it into
     * the new representation.
     **/
    virtual void applyPreProcessing(
        const DepVarStruct &depVar, 
        std::vector<float> &transformVec) const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE;
    /// @}

    /// Other utilities
    /// @{
#ifndef RINGER_STANDALONE
    /**
     * @brief Set the holden CaloRings raw configuration collection.
     **/
    virtual void setRawConfCol(
        const xAOD::RingSetConf::RawConfCollection *crRawConfCol) 
      ATH_RINGER_FINAL ATH_RINGER_OVERRIDE 
    {
      m_rsRawConfCol = crRawConfCol;
      m_nRings = xAOD::RingSetConf::totalNumberOfRings( *crRawConfCol );
    }

    /**
     * @brief Get the holden CaloRings raw configuration collection.
     **/
    virtual void getRawConfCol(
        const xAOD::RingSetConf::RawConfCollection *&crRawConfCol) const
      ATH_RINGER_FINAL ATH_RINGER_OVERRIDE
    {
      crRawConfCol = m_rsRawConfCol;
    }
#endif

    /**
     * @brief Get segmentation type for this pre-processor
     **/
    virtual SegmentationType getSegType() const 
      ATH_RINGER_FINAL ATH_RINGER_OVERRIDE
    {
      return static_cast<SegmentationType>(segType);
    }

    /**
     * @brief Returns whether holden interface collection is empty.
     **/
    virtual bool empty() const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE 
    { 
      return m_ppCol.empty();
    }

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
     * @brief Release all holden pointer memory
     **/
    void releaseMemory() ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;


    /**
     * @brief Overloads the setMsgStream from RedirectMsgStream.
     **/
    virtual void setMsgStream(MsgStream *msg) const ATH_RINGER_OVERRIDE
      ATH_RINGER_FINAL;

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
    void print(MSG::Level lvl = MSG::DEBUG) const ATH_RINGER_OVERRIDE
      ATH_RINGER_FINAL;

    /**
     * @brief Write collection to TDirectory
     **/
    void write(TDirectory *baseDir, const char *idxStr = "") const
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Read collection from TDirectory
     **/
    static RingerProcedureWrapper* read(TDirectory *configDir, 
        unsigned version);
    ///@}

  private:
    /// Private Methods:
    ///@{
    /**
     * @brief Check if pre-processing interface collection is in good status 
     * (Throws otherwise)
     **/
    void checkPPCol(); 
    ///@}

    /// Private Properties:
    ///@{
    /// @brief holden pre-processing collection:
    PPDepProcCollection m_ppCol;
    /// Hold pointer to the first position on the collection
    procedure_t *m_pp;

#ifndef RINGER_STANDALONE
    /// @brief contains a pointer into the CaloRings configuration:
    const xAOD::RingSetConf::RawConfCollection *m_rsRawConfCol;
    /// @short contains the total number of rings in the vectorized
    ///        representation
    unsigned m_nRings;
#endif
    ///@}
 
};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_RINGERPREPROCESSORWRAPPER_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#ifndef RINGER_DISCRIMINATOR_WRAPPER_INCLUDE
#include "RingerPreProcessorWrapper.icc"
#endif // RINGER_DISCRIMINATOR_WRAPPER_INCLUDE
#endif // INCLUDE_HEADER_ONLY

