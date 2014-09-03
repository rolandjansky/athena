/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_TOOLS_ATHENA_PACKAGEINFO_H
#define ATHENAKERNEL_TOOLS_ATHENA_PACKAGEINFO_H 1

#include <iostream>
#include <string>
class MsgStream;

namespace Athena {

/**
 * @class PackageInfo
 * @author Paolo Calafiura
 * @brief decodes cmt PACKAGE_VERSION macro
 *
 * FIXME class works fine technically, but it does not satisfy
 * the use case it was designed for, namely to add package information
 * to the ClassID_traits definition in SGTools/CLASS_DEF.h
 *
 * $Id: AthenaPackageInfo.h,v 1.4 2007-12-14 01:58:50 binet Exp $
 */

  class PackageInfo {
  private:
    std::string m_name;
    std::string m_version;
  public:
    /**
     * @brief construct PackageInfo from cmt-generated PACKAGE_VERSION #define
     * @param a string of the form PACKAGE-XX-YY-NN 
     * @throws invalid_argument
     */
    PackageInfo(const std::string& cmtPackageVersion);
    PackageInfo() : m_name(), m_version() {}
    const std::string& name() const { return m_name; }
    const std::string& version() const { return m_version; }
  };
}

MsgStream&
operator << (MsgStream& ost, const Athena::PackageInfo& info);

std::ostream&
operator << (std::ostream& ost, const Athena::PackageInfo& info);

inline
bool 
operator == (const Athena::PackageInfo& lhs, const Athena::PackageInfo& rhs) {
  return ( (lhs.name() == rhs.name()) && (lhs.version() == rhs.version()) );
}
inline
bool 
operator != (const Athena::PackageInfo& lhs, const Athena::PackageInfo& rhs) {
  return !(lhs == rhs);
}
inline
bool 
operator < (const Athena::PackageInfo& lhs, const Athena::PackageInfo& rhs) {
  return ( (lhs.name() < rhs.name()) || 
	   ( (lhs.name() == rhs.name()) && (lhs.version() < rhs.version()) ) );
}

#endif
