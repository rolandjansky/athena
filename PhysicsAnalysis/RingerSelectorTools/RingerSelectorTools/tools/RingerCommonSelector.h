/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerCommonSelector.h 668868 2015-05-20 20:26:18Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_RINGERCOMMONSELECTOR_H
#define RINGERSELECTORTOOLS_TOOLS_RINGERCOMMONSELECTOR_H

// Local includes:
#include "RingerSelectorTools/procedures/RingerProcedureWrapper.icc"
#include "RingerSelectorTools/tools/VariableDependency.h"

// STL includes:
#include <vector>

// xAOD includes:
#include "xAODCaloRings/CaloRingsFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

// ROOT includes:
#include "PATCore/AcceptData.h"

namespace Ringer {

class RingerConfStruct;

/**
 *
 * @class RingerCommonSelecto
 *
 **/
class RingerCommonSelector : public RedirectMsgStream
{

  public:
    /**
     * @brief Main ctor
     **/
    RingerCommonSelector(
      const Ringer::IDiscrWrapperCollection *discrWrapperCol,
      const Ringer::IThresWrapper *thresWrapper,
      const Ringer::RingerConfStruct& fileConf );

    /**
     * @brief Clear output space representation
     **/
    void clear();

    /**
     * @brief Execute Ringer common selector
     **/
    StatusCode execute(
        const DepVarStruct &depVar,
        const xAOD::CaloRings* clRings,
        const TrackPatternsHolder *trackPat,
        asg::AcceptData& acceptData);

    /**
     * @brief Get output space
     **/
    const std::vector<float> &getOutputSpace() const;

  private:

    /// Properties to be given by AsgSelector:
    /// @{
    const Ringer::IDiscrWrapperCollection *m_discrWrapperCol;
    const Ringer::IThresWrapper *m_thresWrapper;
    const bool m_useTrackPat;
    const bool m_useRawTrackPat;
    const bool m_useCaloCommittee;
    /// @}

    /// Local properties:
    /// @{
    /// Pointer to firlastst discrimination layer
    const IDiscrWrapper *m_fstDiscrLayer;
    /// Pointer to last discrimination layer
    const IDiscrWrapper *m_lastDiscrLayer;
    /// The discrimination wrapper collection size
    size_t m_discrWrapperColSize;
    /// Hold discriminators input
    std::vector<float> m_input;
    /// Hold discriminators output
    std::vector<float> m_output;
    /// Hold decision
    std::vector<bool> m_decVec;
    /**
     * @brief Hold track discriminator output
     *
     * This vector will be used only in the following special occasion:
     *   - It is wanted to feed the last classifier with the output from the track
     *  classifier. In this case, we need
     **/
    std::vector<float> m_trackDiscr_output;
    /// Flag if discrimination approach is non segmented
    bool m_nonSegmentedDiscr;
    /// @}

};

//==============================================================================
inline
void RingerCommonSelector::clear(){
  m_output.clear();
}

//==============================================================================
inline
const std::vector<float>& RingerCommonSelector::getOutputSpace() const {
  return m_output;
}

} // namespace Ringer


#endif // RINGERSELECTORTOOLS_TOOLS_RINGERCOMMONSELECTOR_H
