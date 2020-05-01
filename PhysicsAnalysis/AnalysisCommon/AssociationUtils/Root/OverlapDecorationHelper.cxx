/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "AssociationUtils/OverlapDecorationHelper.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  OverlapDecorationHelper::
  OverlapDecorationHelper(const std::string& inputLabel,
                          const std::string& outputLabel,
                          bool outputPassValue)
    : m_useInputLabel(!inputLabel.empty()),
      m_inputAccessor("OverlapDecorationHelper_Dummy"),
      m_outputDecorator(outputLabel),
      m_outputPassValue(outputPassValue)
  {
    if(m_useInputLabel)
      m_inputAccessor = ort::inputAccessor_t(inputLabel);
  }

  //---------------------------------------------------------------------------
  // Determine if object is currently OK for input to OR
  //---------------------------------------------------------------------------
  bool OverlapDecorationHelper::isInputObject
  (const xAOD::IParticle& obj) const
  {
    // Input label is turned off if empty string
    if(!m_useInputLabel) return true;
    return m_inputAccessor(obj);
  }

  //---------------------------------------------------------------------------
  // Determine if object is currently rejected by OR
  //---------------------------------------------------------------------------
  bool OverlapDecorationHelper::isRejectedObject
  (const xAOD::IParticle& obj) const
  {
    // isRejected = isInput && (output == fail)
    return isInputObject(obj) && ( m_outputDecorator(obj) != m_outputPassValue );
  }
  //---------------------------------------------------------------------------
  // Determine if object is NOT currently rejected by OR (is surviving)
  //---------------------------------------------------------------------------
  bool OverlapDecorationHelper::isSurvivingObject
  (const xAOD::IParticle& obj) const
  {
    // isSurviving = isInput && (output == pass)
    return isInputObject(obj) && ( m_outputDecorator(obj) == m_outputPassValue );
  }
  //---------------------------------------------------------------------------
  // Retrieve the user-defined object priority
  //---------------------------------------------------------------------------
  char OverlapDecorationHelper::
  getObjectPriority(const xAOD::IParticle& obj) const
  {
    // We current reuse the input decoration as the priority score
    return m_inputAccessor(obj);
  }

  //---------------------------------------------------------------------------
  // Set the overlap (output) decoration
  //---------------------------------------------------------------------------
  void OverlapDecorationHelper::setOverlapDecoration
  (const xAOD::IParticle& obj, bool result)
  {
    m_outputDecorator(obj) = result;
  }
  //---------------------------------------------------------------------------
  void OverlapDecorationHelper::setObjectPass(const xAOD::IParticle& obj)
  {
    setOverlapDecoration(obj, m_outputPassValue);
  }
  //---------------------------------------------------------------------------
  void OverlapDecorationHelper::setObjectFail(const xAOD::IParticle& obj)
  {
    setOverlapDecoration(obj, !m_outputPassValue);
  }
  
  //---------------------------------------------------------------------------
  // Check if output decoration has been applied to a container
  //---------------------------------------------------------------------------
  bool OverlapDecorationHelper::isDecorated
  (const xAOD::IParticleContainer& container) const
  {
    return container.size() > 0 &&
           m_outputDecorator.isAvailable(*container[0]);
  }
  
  //---------------------------------------------------------------------------
  // Initialize output decoration
  //---------------------------------------------------------------------------
  void OverlapDecorationHelper::initializeDecorations
  (const xAOD::IParticleContainer& container)
  {
    if(!isDecorated(container))
      resetDecorations(container);
  }
  
  //---------------------------------------------------------------------------
  // Reset output decoration
  //---------------------------------------------------------------------------
  void OverlapDecorationHelper::resetDecorations
  (const xAOD::IParticleContainer& container)
  {
    for(auto obj : container){
      // This isn't terrible intuitive, but in order to support both output
      // logic modes in a reasonable way, we initialize the output flag to the
      // logical AND of isInput and outputPassValue. This results in non-input
      // objects being initialized to 'false' regardless of output logic.
      bool result = ( isInputObject(*obj) && m_outputPassValue );
      setOverlapDecoration(*obj, result);
    }
    //for(auto obj : container) setObjectPass(*obj);
  }

} // namespace ORUtils
