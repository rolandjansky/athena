/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AsgElectronRingerSelector.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_ASGELECTRONCALORINGERSELECTOR_H
#define RINGERSELECTORTOOLS_ASGELECTRONCALORINGERSELECTOR_H

// STL includes:
#include <string>

// ROOT includes:
#include <TFile.h>

#ifndef RINGER_STANDALONE
// Athena includes
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/ToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"

// xAOD includes:
#include "xAODCaloRings/CaloRingsContainer.h" // needed for caloRingsReader_t
#include "xAODCaloRings/RingSetConfContainerFwd.h"

// Local includes:
#include "RingerSelectorTools/IAsgElectronRingerSelector.h"
#include "RingerSelectorTools/ElectronTAccept.h"
#include "RingerSelectorTools/procedures/RingerProcedureWrapper.h"
#include "RingerSelectorTools/tools/TrackPatternsHolder.h"
#include "RingerSelectorTools/tools/RingerCommonSelector.h"
#include "RingerSelectorTools/tools/cxx/mutable.h"
#include "RingerSelectorTools/tools/cxx/final.h"
#include "RingerSelectorTools/tools/cxx/override.h"

namespace Ringer {

class RingerConfStruct;
/*
 *  TODO List:
 *    - Add metadata on xAOD file with the AsgSelector configuration.
 */

/**
 * @class AsgElectronRingerSelector
 * @brief Electron selector using Ringer patterns to apply discrimination
 *
 * @author Werner Spolidoro Freund <wsfreund@cern.ch>
 *
 * Based on AsgElectronIsEMSelector by Jovan Mitrevski (UCSC)
 *   and Karsten Koeneke (CERN)
 *
 * and Metadata implementation based on xAODConfigTool by
 *   Attila Krasznahorkay
 *
 **/
class AsgElectronRingerSelector : public asg::AsgMetadataTool,
                                  virtual public IAsgElectronRingerSelector
{

  public:

    // Use ASG_TOOL macro for 2 interface base classes.
    ASG_TOOL_CLASS2(AsgElectronRingerSelector,
                    IAsgElectronRingerSelector,
                    IAsgSelectionTool)

  public:

    /** Standard constructor */
    AsgElectronRingerSelector(std::string myname);

    /** Standard destructor */
    virtual ~AsgElectronRingerSelector();

    /** Gaudi Service Interface method implementations */
    virtual StatusCode initialize() override;

    /** External initialize method */
    StatusCode initialize( const char *metaName, unsigned int cutMask
        , IDiscrWrapperCollection &discrWrapper, IThresWrapper* thresWrapper
        , RingerConfStruct &confStruct);

    /** Gaudi Service Interface method implementations */
    virtual StatusCode finalize() override;

    /// Main methods for IAsgSelectionTool interface
    ///@{
    /**
     * @brief Set the discrimination configuration file
     **/
    virtual void setDiscrFile( const std::string path ) override;

    /**
     * @brief Set the threshold configuration file
     **/
    virtual void setThresFile( const std::string path ) override;

    /**
     * @brief Set the threshold configuration file
     **/
    virtual void setCutMask( const unsigned int cutMask ) override;

    /**
     * @brief Set the CutIDSelector to be used
     **/
    virtual void setCutIDSelector( IAsgElectronIsEMSelector *cutID ) override;

    /**
     * @brief Set the RingSetConfContainer (MetaData) key
     **/
    virtual void setRSMetaName( const std::string name ) override;

    /**
     * @brief Set whether to cache the meta data and assume it will be the same
     *        through all the analysis.
     **/
    void setCacheMetaData( bool flag );

    /**
     * @brief This method will bypass accept to xAOD::Electron if it is possible.
     **/
    virtual asg::AcceptData accept( const xAOD::IParticle* part ) const override;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData accept( const xAOD::Electron* part ) const override;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData accept( const xAOD::Electron* part, const double mu ) const;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData accept( const xAOD::Egamma* eg ) const;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData  accept( const xAOD::Egamma* eg, const double mu ) const;

    /**
     * @brief Accept using Electron reference
     **/
    virtual asg::AcceptData accept( const xAOD::Electron& part ) const override;
    /**
     * @brief Accept using Electron reference
     **/
    virtual asg::AcceptData  accept( const xAOD::Electron& part, const double mu ) const;
    /**
     * @brief Accept using IParticle reference
     **/
    virtual asg::AcceptData accept( const xAOD::IParticle& part ) const override;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData accept( const xAOD::Egamma& eg ) const;

    /**
     * @brief Identification procedure is done in this method
     **/
    virtual asg::AcceptData accept( const xAOD::Egamma& eg, const double mu ) const;

    /**
     * @brief Method to get the plain TAccept for the last particle.
     **/
    virtual const asg::AcceptInfo& getAcceptInfo() const override;

    /** Method to get output space representation */
    virtual const std::vector<float>& getOutputSpace() const override;

    /**
     * @brief Execute without mu information
     **/
    virtual StatusCode execute(const xAOD::Electron* eg,
                               asg::AcceptData& acceptData) const override;

    /**
     * @brief Main execute method
     **/
    StatusCode execute(const xAOD::Electron* eg, const double mu,
                       asg::AcceptData& acceptData) const;

    /**
     * @brief Execute without tracking information, potentially used by trigger.
     **/
    virtual StatusCode execute(const xAOD::Egamma* eg,
                               asg::AcceptData& acceptData) const override;

    /**
     * @brief Execute without tracking information, potentially used by trigger.
     **/
    StatusCode execute(const xAOD::Egamma* eg, const double mu,
                       asg::AcceptData& acceptData) const;
    ///@}

    /**
     * @brief Retrieve the primary vertices
     **/
    unsigned int getNPrimVertices() const;

  protected:

    /**
     * @brief Update metadata information held
     */
    virtual StatusCode beginInputFile() override;

    /**
     * @brief Called when entering a new event
     **/
    virtual StatusCode beginEvent() override;

  private:

    /// @name AsgElectronRingerSelector props (python configurables):
    /// @{

    /// @brief Conf file in which we will retrieve the discrimination chain
    std::string m_discrFileName;

    /// @brief Conf file in which we will retrieve the threshold values
    std::string m_thresFileName;

    /// @brief The CutID Tool Handle to run track (if required by config file)
    ToolHandle<IAsgElectronIsEMSelector> m_cutIDSelector;

    /// @brief The unsigned mask from which we will set m_cutsToUse (IsEM)
    unsigned m_cutsMask;

    /// @brief RingSetConfContainer key in the MetaData StoreGate
    std::string m_rsMetaName;

    /// @brief Whether to cache metadata configuration or not
    bool m_cacheMetaData;

    /// The primary vertex container name
    std::string m_primVtxContName;
    /// @}

    /// @name AsgElectronRingerSelector props (non-python configurables):
    /// @{
    /// @brief The discriminator chain collection
    IDiscrWrapperCollection m_discrWrapperCol;

    /// @brief The discriminator chain collection
    IThresWrapper *m_thresWrapper;

    /// @brief The Ringer Common Selector which will be run by this AsgSelector:
    RingerCommonSelector *m_ringSelCommon;

    /// @brief Hold the track patterns:
    ATH_RINGER_MUTABLE TrackPatternsHolder *m_trackPat;

    /// @brief The particles CaloRings decorations reader
    xAOD::caloRingsReader_t* m_ringsELReader;

    /// @brief Last particle accept bitmask (already applying the m_cutsToUse)
    asg::AcceptInfo m_accept;

    /// @brief Which subset of decisions to use
    ElectronTAccept::bitMskWord m_cutsToUse;

    /// Whether to use CutID track decision
    bool m_useCutIDDecision;

    /// Flag to whether metadata is cached or not
    bool m_metaDataCached;

    /// Hold pointer into the CaloRingsConfContainer
    const xAOD::RingSetConfContainer* m_rsConfCont;

    /// Hold the raw configuration collection:
    xAOD::RingSetConf::RawConfCollection m_rawConfCol;

    /// Cache if configuration info is on inputData
    bool m_metaIsOnOutput;

    /// Default number of vertices when not using it
    double m_defaultNvtx;

    /// Whether to use or not the number of vertices
    bool m_useNvtx;

    /// Cached number of vertices for this event
    double m_nVtx;
    /// @}

    /// Private methods:
    /// @{

    /**
     * @brief Read the configuration file
     **/
    StatusCode readConfigFiles();

    /**
     * @brief Register decision bitmask word
     **/
    StatusCode registerDecisionBitMask();

    /**
     * @brief Configure wrappers
     **/
    StatusCode configureFromStruct(RingerConfStruct &fileConf);
    /// @}
};

#endif

/**
 * @class RingerConfStruct
 * @brief Configuration structure for parameters in file
 **/
class RingerConfStruct {
public:
  bool useTrackPat;
  bool useRawTrackPat;
  //bool useCalStdPat;
  //bool useRawCalStdPat(false),
  bool useCaloCommittee;
  bool useBLOutliers;
  bool usePIXOutliers;
  bool useSCTOutliers;
  bool useTRTOutliers;
  bool useTRTXenonHits;
  bool useNvtx;

  unsigned version = 1;

  RingerConfStruct();

  void retrieveConf(const char *fileName);
  void writeConf( char* fileName );
  void printConf( MsgStream *msg, MSG::Level lvl = MSG::INFO);
};

// Inline methods:
//=============================================================================
inline
RingerConfStruct::RingerConfStruct()
  : useTrackPat(false),
    useRawTrackPat(false),
    //useCalStdPat,
    //useRawCalStdPat(false),
    useCaloCommittee(false),
    useBLOutliers(false),
    usePIXOutliers(false),
    useSCTOutliers(false),
    useTRTOutliers(false),
    useTRTXenonHits(false),
    useNvtx(false){;}


#ifndef RINGER_STANDALONE

//=============================================================================
inline
void AsgElectronRingerSelector::setDiscrFile( const std::string path )
{
  m_discrFileName = path;
}

//=============================================================================
inline
void AsgElectronRingerSelector::setThresFile( const std::string path )
{
  m_thresFileName = path;
}

//=============================================================================
inline
void AsgElectronRingerSelector::setCutMask( const unsigned int cutMask )
{
  m_cutsMask = cutMask;
}

//=============================================================================
inline
void AsgElectronRingerSelector::setCutIDSelector(
      IAsgElectronIsEMSelector *cutID )
{
  m_cutIDSelector = ToolHandle<IAsgElectronIsEMSelector>(cutID);
}

//=============================================================================
inline
void AsgElectronRingerSelector::setRSMetaName( const std::string name )
{
  m_rsMetaName = name;
}

//=============================================================================
inline
void AsgElectronRingerSelector::setCacheMetaData( bool flag )
{
  m_cacheMetaData = flag;
}

//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept(
    const xAOD::Electron& part, double mu = -99. ) const
{
  return accept (&part, mu);
}

//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept(
    const xAOD::IParticle& part ) const
{
  return accept(&part);
}

//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept(
    const xAOD::Egamma& part ) const
{
  return accept (&part, -99);
}

//=============================================================================
inline
StatusCode AsgElectronRingerSelector::execute(const xAOD::Egamma* eg,
                                              asg::AcceptData& acceptData) const
{
  return execute( eg, -99, acceptData);
}
//=============================================================================
inline
StatusCode AsgElectronRingerSelector::execute(const xAOD::Electron* eg,
                                              asg::AcceptData& acceptData) const
{
  return execute( eg, -99, acceptData );
}
//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept( const xAOD::Egamma& eg, const double mu ) const
{
  return accept( &eg, mu );
}
//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept( const xAOD::Egamma* eg ) const
{
  return accept( eg, -99 );
}
//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept( const xAOD::Electron* part ) const
{
  return accept( part, -99 );
}
//=============================================================================
inline
asg::AcceptData AsgElectronRingerSelector::accept( const xAOD::Electron& part ) const
{
  return accept( &part );
}

//=============================================================================
inline
const asg::AcceptInfo& AsgElectronRingerSelector::getAcceptInfo() const
{
  return m_accept;
}

//=============================================================================
inline
const std::vector<float>& AsgElectronRingerSelector::getOutputSpace() const
{
  if ( m_ringSelCommon ){
    return m_ringSelCommon->getOutputSpace();
  } else {
    throw std::runtime_error(std::string("Cannot return outut space "
          "because RingerCommonSelector was not yet allocated."));
  }
}

} // namespace Ringer
#endif // RINGER_STANDALONE

#endif // RINGERSELECTORTOOLS_ASGELECTRONCALORINGERSELECTOR_H
