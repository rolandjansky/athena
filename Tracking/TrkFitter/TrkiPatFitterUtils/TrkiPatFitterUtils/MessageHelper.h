/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <sstream>
#include <string>
#include <vector>
#include "AthenaBaseComps/AthMessaging.h"

class MessageHelper
{
public:
    MessageHelper				(const AthMessaging& parent);
    void	incrementCount			(unsigned int messageNumber);    
    void	printSummary			(void) const;
    void	printWarning			(unsigned int messageNumber);
    void	printWarning			(unsigned int messageNumber, const std::string& addition);
    void	setMaxNumberOfMessagesPrinted	(unsigned int num);
    void	setNumberOfMessages		(unsigned int num);	//!< For optimising creation (does resize() internally)
    void	setMessage			(unsigned int messageNumber, const std::string& message);
    bool	wouldPrintWarning		(unsigned int messageNumber) const;

private:
    const AthMessaging&               m_parent; 
    unsigned int                      m_maxWarnings;	//!< Maximum number of WARNING messages permitted. 
    mutable std::vector<unsigned int> m_warningCounts;	//!< The counts per error (the index is the error number)
    mutable std::vector<std::string>  m_warningText;	//!< The text for the WARNINGs (the index is the error number). Filled in ctor. 
};

inline
MessageHelper::MessageHelper (const AthMessaging& parent)
    : m_parent		(parent),
      m_maxWarnings	(3)
{}

inline void
MessageHelper::incrementCount (unsigned int messageNumber)
{
    m_warningCounts[messageNumber]++;
}

inline void
MessageHelper::printSummary (void) const
{
    bool haveWarnings	= false;
    for (unsigned int i = 0; i < m_warningCounts.size(); ++i)
    {
	if (! m_warningCounts[i])	continue;
	haveWarnings	= true;
	break;
    }
    if (! haveWarnings)			return;
    
    std::stringstream out;
    out << " summary of WARNING occurences (maximum shown during job: " << m_maxWarnings << "):";
    m_parent.msg(MSG::INFO) << out.str() << endmsg;

    for (unsigned int i = 0; i < m_warningCounts.size(); ++i)
    {
	if (! m_warningCounts[i])	continue;
	out.str("");
	out << "   " << m_warningCounts[i] << ":\t" << m_warningText[i];
	m_parent.msg(MSG::INFO) << out.str() << endmsg;
    }
}

inline void
MessageHelper::printWarning (unsigned int messageNumber)
{
    m_warningCounts[messageNumber]++;
    if (! m_parent.msgLvl(MSG::WARNING)
	|| m_warningCounts[messageNumber] > m_maxWarnings)	return;

    m_parent.msg(MSG::WARNING) << m_warningText[messageNumber] << endmsg;
    if (m_warningCounts[messageNumber] == m_maxWarnings)
	m_parent.msg(MSG::WARNING) << "Limit reached. No more messages of this type will be printed."
				   << endmsg;
}

inline void
MessageHelper::printWarning (unsigned int messageNumber, const std::string& addition)
{
    m_warningCounts[messageNumber]++;
    if (! m_parent.msgLvl(MSG::WARNING)
	|| m_warningCounts[messageNumber] > m_maxWarnings)	return;

    m_parent.msg(MSG::WARNING) << m_warningText[messageNumber] << addition << endmsg;
    if (m_warningCounts[messageNumber] == m_maxWarnings)
	m_parent.msg(MSG::WARNING) << "Limit reached. No more messages of this type will be printed."
				   << endmsg;
}

inline void
MessageHelper::setMaxNumberOfMessagesPrinted (unsigned int num)
{
    m_maxWarnings	= num;
}

inline void
MessageHelper::setNumberOfMessages (unsigned int num)
{
    m_warningCounts.resize(num);
    m_warningText.resize(num);
}

inline void
MessageHelper::setMessage (unsigned int messageNumber, const std::string& message)
{
    if (messageNumber > m_warningCounts.size()) setNumberOfMessages(messageNumber+10);
    m_warningCounts[messageNumber]	= 0;
    m_warningText[messageNumber]	= message;
}

inline bool
MessageHelper::wouldPrintWarning (unsigned int messageNumber) const
{
    if (m_parent.msgLvl(MSG::WARNING) && m_warningCounts[messageNumber] <= m_maxWarnings) return true;
    return false;
}

#endif
