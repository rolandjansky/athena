/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALORECOSTATUS_H
#define CALOEVENT_CALORECOSTATUS_H


#include <vector>

class CaloRecoStatus
{
 public: 

  /*! \brief Internal store type */
  typedef unsigned int     store_type;

  /*! \brief reconstruction status word
   *
   *  Indicates cluster tag and calibration status. Note that tags
   *  may only be applied to (calibrated) topological clusters.
   *
   *  \param TAGGEDEM      tagged as electromagnetic
   *  \param TAGGEDHAD     tagged as hadronic
   *  \param TAGGEDMUON    tagged as muon
   *  \param TAGGEDUNKNOWN no safe tags from any classification
   *  \param CALIBRATED    calibrated begond em scale
   *  \param CALIBRATEDLHC calibrated by local hadronic approach
   *  \param CALIBRATEDALT calibrated by alternative calibration, typically
   *                       with cell weights from jets
   * 
   *  Note that several tags can be valid at the same time, especially 
   *  CALIBRATED & CALIBRATEDLHC & CALIBRATEDALT and subsequent combinations
   *  of those.
   */
  enum StatusIndicator 
  { 
    TAGGEDEM      = 0x00000001, /*< tagged as electromagnetic object */  
    TAGGEDHAD     = 0x00000002, /*< tagged as hadronic object        */
    TAGGEDMUON    = 0x00000004, /*< tagged as muonic cluster         */
    TAGGEDUNKNOWN = 0x00000008, /*< tagged as unknown                */
    CALIBRATED    = 0x10000000, /*< cluster calibrated (non-emscale) */
    CALIBRATEDLHC = 0x10000010, /*< cluster calibrated local had     */
    CALIBRATEDALT = 0x10000020, /*< cluster calibrated alternatively */
    UNKNOWNSTATUS = 0x00000000  /*< unknown status                   */
  };
  /*! \brief Default constructor */ 
  CaloRecoStatus(StatusIndicator statusIndicator = UNKNOWNSTATUS);
  CaloRecoStatus(store_type store);
  /*! \brief Destructor */
  virtual ~CaloRecoStatus();

  /*! \brief Set status 
   *
   *  \param statusIndicator status bit pattern
   */
  virtual void setStatus(const StatusIndicator& statusIndicator);
  /*! \brief Check status
   *
   *  \param statusIndicator status indicator
   * 
   *  Note that the client can request a single status set, or a 
   *  combination of status indicators set
   */
  virtual bool checkStatus(const StatusIndicator& statusIndicator) const;
  /*! \brief retrieve the entire status word */
  //  virtual const StatusIndicator & getStatusWord() const;
  virtual const store_type& getStatusWord() const;
  /*! \brief retrieve all status indicators
   *
   *  \param pStatusList  list of all status indicators 
   *
   *  Returns true if one or more indicators are available
   */
  virtual bool getStatus(std::vector<CaloRecoStatus::StatusIndicator>& 
			 pStatusList) const;
  /*! \brief reset reconstruction status
   *
   */
  virtual void resetStatus();
  virtual void resetStatus(const StatusIndicator& statusIndicator);


 private:

  /*! \brief Status store */
  //  StatusIndicator m_status;
 store_type m_status;

};
/*! \class CaloRecoStatus 
 *  
 *  \brief reconstruction status indicator
 */

inline CaloRecoStatus::CaloRecoStatus(StatusIndicator aStatus)
  : m_status((store_type)aStatus)
{ }

inline CaloRecoStatus::CaloRecoStatus(store_type aStatus)
  : m_status(aStatus)
{ }

inline CaloRecoStatus::~CaloRecoStatus()
{ }

inline void CaloRecoStatus::setStatus(const StatusIndicator& aStatus)
{
  m_status = m_status | (store_type)aStatus;
}

inline const CaloRecoStatus::store_type& CaloRecoStatus::getStatusWord() const
{
  return m_status;
}

inline bool CaloRecoStatus::checkStatus(const StatusIndicator& aStatus) const
{
  return ( m_status & (store_type)aStatus ) == (store_type)aStatus;
}

inline void CaloRecoStatus::resetStatus()
{
  m_status = (store_type)UNKNOWNSTATUS;
}

inline void CaloRecoStatus::resetStatus(const StatusIndicator& aStatus)
{
  m_status = m_status & ~((store_type)aStatus);
}
#endif
