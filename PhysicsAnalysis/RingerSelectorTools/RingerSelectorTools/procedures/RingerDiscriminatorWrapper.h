/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerDiscriminatorWrapper.h 713528 2015-12-09 08:58:44Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_RINGERDISCRIMINATIONWRAPPER_H
#define RINGERSELECTORTOOLS_PROCEDURES_RINGERDISCRIMINATIONWRAPPER_H

#define RINGER_PROCEDURE_INCLUDE
#include "RingerProcedureWrapper.h"
#undef RINGER_PROCEDURE_INCLUDE

//#undef NDEBUG

#define RINGER_DISCRIMINATOR_WRAPPER_INCLUDE
// We are also dependent on the PreProcessorWrapper, but we want only to define
// the collection for now:
#include "RingerPreProcessorWrapper.h"
#undef RINGER_DISCRIMINATOR_WRAPPER_INCLUDE

/** 
 * @brief Namespace dedicated for Ringer utilities 
 **/
namespace Ringer {

/**
 * @class IRingerProcedureWrapper
 * @brief Specialization for Discriminator procedure.
 **/
template<>
class IRingerProcedureWrapper< Discrimination::IDiscriminator > :
  virtual public IRedirectMsgStream,
  virtual public IRingerProcedureWrapperBase
{
  public:
    /**
     * @brief Collection type of Discriminators Wrappers.
     **/
    typedef typename std::vector< IRingerProcedureWrapper* > WrapperCollection;

    /// Main methods:
    ///@{
#ifndef RINGER_STANDALONE
    /**
     * @brief Apply discriminator to obtain its output representation
     *
     * This method will pass the information within xAOD::CaloRings and
     * xAOD::TrackParticle to the pre-processors (if any) and
     * afterwards feed the classifier. If any pointer if set to null, then it
     * won't pass it into the classifier.  Make sure to feed classifier with
     * the same information it was adjusted to work with.
     **/
    virtual void execute(
        const DepVarStruct &depVar,
        const xAOD::CaloRings *clrings,
        const TrackPatternsHolder *trackPat,
        std::vector<float> &output) const = 0;

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
     * @brief Apply discriminator to obtain its output representation
     *
     * This method will pass @name input information to the
     * pre-processors (if any) and afterwards feed the classifier. 
     *
     * This overload is only available if SegmentationType is set to 
     * NoSegmentation (there is no section/layer segmentation
     * information available on this method).
     **/
    virtual void execute(
        const DepVarStruct &depVar,
        const std::vector<float> &input,
        std::vector<float> &output) const = 0;
    ///@}
 
    /**
     * @brief Returns this wrapper name
     **/
    virtual const char* name() const ATH_RINGER_FINAL ATH_RINGER_OVERRIDE {
      return wrapName;
    }

#if RINGER_USE_NEW_CPP_FEATURES
    static constexpr const char* wrapName = "RingerDiscriminatorWrapper"; 
#else
    static const char* wrapName; 
#endif

    /**
     * @brief Returns whether it has Pre-Processing Collection Wrapper.
     **/
    virtual bool hasPP() const = 0;

    /**
     * @brief Write all wrappers on discrWrapperCol to TDirectory
     **/
    static void writeCol(const WrapperCollection &discrWrapperCol, 
        const char *fileName);

    /**
     * @brief Read all discriminator on file at the path and append them to
     * IPreProcWrapperCollection
     **/
    static void read(WrapperCollection &discrWrapperCol, 
        const char* fileName);

  protected:

    IRingerProcedureWrapper(){;}

};

/**
 * @brief Facilitate access into Discrimination Wrappers.
 **/
typedef IRingerProcedureWrapper< Discrimination::IDiscriminator >
  IDiscrWrapper;

/**
 * @brief Facilitate access into Discrimination Wrappers.
 **/
typedef IDiscrWrapper::WrapperCollection IDiscrWrapperCollection;

/**
 * @class RingerProcedureWrapper
 * @brief Specialization for Discriminator procedure.
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
  false,     // isPreProcessor
  true,      // isDiscriminator
  false> :   // isThreshold
    public IDiscrWrapper,
    public RedirectMsgStream
{
    RINGER_STATIC_ASSERT(
        (Ringer::is_base_of<VariableDependency,procedure_t>::value),
        "RingerProcedureWrapper procedure_t type must have IVariableDependecy inheritance.");
  public:
    /// RingerProcedureWrapper for Discrimination procedures typedefs:
    ///@{
    /**
     * @brief typedef to base wrapper
     **/
    //typedef typename RingerProcedureWrapper::template
    //    IRingerProcedureWrapper< Discrimination::IDiscriminator > 
    //    wrapper_t;

    /**
     * @brief typedef to the Ringer Interface variable dependency collection
     *
     * Collection Dimension: [segType][etBin][etaBin]:
     *  
     **/
    typedef typename std::vector< 
        std::vector < 
        std::vector < procedure_t* > >
      > DiscrDepProcCollection;
    ///@}

    /// Ctors:
    ///@{
    /**
     * @brief Build RProc Wrapper with no pre-processor
     **/
    RingerProcedureWrapper(
        const DiscrDepProcCollection &discrDepCol):
      m_ppWrapperCol(IPreProcWrapperCollection(0)),
      m_discrCol(discrDepCol),
      m_discr(nullptr),
      m_rsRawConfCol(nullptr),
      m_nRings(0)
    { 
      checkDiscrCol(); 
    }

    /**
     * @brief Build RProc Wrapper with all functionallities
     **/
    RingerProcedureWrapper(
        const IPreProcWrapperCollection &ppCol, 
        const DiscrDepProcCollection &discrDepCol):
      m_ppWrapperCol(ppCol),
      m_discrCol(discrDepCol),
      m_discr(nullptr),
      m_rsRawConfCol(nullptr),
      m_nRings(0)
    { 
      checkPPWrapperCol(); 
      checkDiscrCol(); 
    }
    ///@}

    /// Main methods:
    ///@{
#ifndef RINGER_STANDALONE
    /**
     * @brief Apply discriminator to obtain its output representation
     *
     * This method will pass the information within xAOD::CaloRings and
     * xAOD::TrackParticle to the pre-processors (if any) and
     * afterwards feed the classifier. If any pointer if set to null, then it
     * won't pass it into the classifier.  Make sure to feed classifier with
     * the same information it was adjusted to work with.
     **/
    virtual void execute(
        const DepVarStruct &depVar,
        const xAOD::CaloRings *clrings,
        const TrackPatternsHolder *trackPat,
        std::vector<float> &output) const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;
#endif

    /**
     * @brief Apply discriminator to obtain its output representation
     *
     * This method will pass @name input information to the
     * pre-processors (if any) and afterwards feed the classifier. 
     *
     * This overload is only available if SegmentationType is set to 
     * NoSegmentation (there is no section/layer segmentation
     * information available on this method).
     **/
    virtual void execute(
        const DepVarStruct &depVar,
        const std::vector<float> &input,
        std::vector<float> &output) const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;
    ///@}

    /// Other utilities:
    ///@{
#ifndef RINGER_STANDALONE
    /**
     * @brief Set the holden CaloRings raw configuration collection.
     **/
    virtual void setRawConfCol(
        const xAOD::RingSetConf::RawConfCollection *crRawConfCol) 
      ATH_RINGER_FINAL ATH_RINGER_OVERRIDE 
    {
      for ( auto &ppWrapper : m_ppWrapperCol ) {
        ppWrapper->setRawConfCol( crRawConfCol );
      }
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
    virtual bool empty() const ATH_RINGER_OVERRIDE { return m_discrCol.empty();}

    /**
     * @brief Returns whether it has pre-processing Collection Wrapper.
     **/
    virtual bool hasPP() const ATH_RINGER_OVERRIDE { return !m_ppWrapperCol.empty();}

    /**
     * @brief Overloads the setMsgStream from RedirectMsgStream.
     **/
    virtual void setMsgStream(MsgStream *msg) const ATH_RINGER_OVERRIDE;

    /**
     * @brief Write collection to TDirectory
     **/
    void write(TDirectory *baseDir, const char *idxStr = "") const
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

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
     * @brief Get full wrapper name, static method
     **/
    static std::string staticFullName();

    /**
     * @brief Print wrapper content
     **/
    void print(MSG::Level lvl = MSG::DEBUG) const ATH_RINGER_OVERRIDE 
      ATH_RINGER_FINAL;

    /**
     * @brief Get full wrapper name
     **/
    std::string fullName() const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /**
     * @brief Read collection from TDirectory
     **/
    static RingerProcedureWrapper* read(TDirectory *configDir, 
        unsigned version);
    ///@}

  private:
    /// Private methods
    ///@{
    /**
     * @brief Check if input PP Wrapper collection is in good status
     * (Throws otherwise)
     **/
    void checkPPWrapperCol();
    /**
     * @brief Check if discriminators interface collection is in good status 
     * (Throws otherwise)
     **/
    void checkDiscrCol(); 
    ///@}

    /// Properties
    ///@{
    /// @brief Discriminator preprocessing collection routines
    const IPreProcWrapperCollection m_ppWrapperCol;
    /// @brief holden discriminator collection:
    DiscrDepProcCollection m_discrCol;
    /// @brief hold pointer to first collection position:
    procedure_t *m_discr;


#ifndef RINGER_STANDALONE
    /// @brief contains a pointer into the CaloRings configuration
    const xAOD::RingSetConf::RawConfCollection *m_rsRawConfCol;
    /// @short contains the total number of rings in the vectorized
    ///        representation
    unsigned m_nRings;
#endif
    ///@}

};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_RINGERDISCRIMINATIONWRAPPER_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "RingerDiscriminatorWrapper.icc"
#endif // INCLUDE_HEADER_ONLY

