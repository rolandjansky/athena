/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <sstream>
#include <string>
#include <vector>
#include <atomic>
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"

class MessageHelper
{
public:
    /// Pass to the constructor the number of messages.
    MessageHelper				(const AthAlgTool& parent,
                                                 unsigned int num);
    void	incrementCount			(unsigned int messageNumber);    
    void	printSummary			(void) const;
    void	printWarning			(unsigned int messageNumber) const;
    void	printWarning			(unsigned int messageNumber, const std::string& addition) const;
    void	setMaxNumberOfMessagesPrinted	(unsigned int num);
    void	setMessage			(unsigned int messageNumber, const std::string& message);
    bool	wouldPrintWarning		(unsigned int messageNumber) const;

private:
    const AthAlgTool&                 m_parent; 
    unsigned int                      m_maxWarnings;	//!< Maximum number of WARNING messages permitted. 
    mutable std::vector<std::atomic<unsigned int> > m_warningCounts ATLAS_THREAD_SAFE;	//!< The counts per error (the index is the error number)
    std::vector<std::string>  m_warningText;	//!< The text for the WARNINGs (the index is the error number). Filled in ctor. 
};

inline
MessageHelper::MessageHelper (const AthAlgTool& parent,
                              unsigned int num)
    : m_parent		(parent),
      m_maxWarnings	(3),
      m_warningCounts   (num),
      m_warningText     (num)
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
MessageHelper::printWarning (unsigned int messageNumber) const
{
    unsigned int count = ++m_warningCounts[messageNumber];
    if (! m_parent.msgLvl(MSG::WARNING)
	|| count > m_maxWarnings)	return;

    m_parent.msg(MSG::WARNING) << m_warningText[messageNumber] << endmsg;
    if (count == m_maxWarnings)
	m_parent.msg(MSG::WARNING) << "Limit reached. No more messages of this type will be printed."
				   << endmsg;
}

inline void
MessageHelper::printWarning (unsigned int messageNumber, const std::string& addition) const
{
    unsigned int count = ++m_warningCounts[messageNumber];
    if (! m_parent.msgLvl(MSG::WARNING)
	|| count > m_maxWarnings)	return;

    m_parent.msg(MSG::WARNING) << m_warningText[messageNumber] << addition << endmsg;
    if (count == m_maxWarnings)
	m_parent.msg(MSG::WARNING) << "Limit reached. No more messages of this type will be printed."
				   << endmsg;
}

inline void
MessageHelper::setMaxNumberOfMessagesPrinted (unsigned int num)
{
    m_maxWarnings	= num;
}

inline void
MessageHelper::setMessage (unsigned int messageNumber, const std::string& message)
{
    if (messageNumber > m_warningCounts.size()) std::abort();
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
