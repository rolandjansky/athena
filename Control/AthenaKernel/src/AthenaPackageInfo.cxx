/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>

#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/tools/AthenaPackageInfo.h"
using Athena::PackageInfo;

using namespace std;

PackageInfo::PackageInfo(const string& cmtPackageVersion) {
  string::size_type i(cmtPackageVersion.find('-'));
  m_name = cmtPackageVersion.substr(0, i);
  m_version = cmtPackageVersion.substr(i+1);
  if (i==0 || i==string::npos || m_version.empty()) {
    cerr << "PackageInfo::PackageInfo("
	 << cmtPackageVersion <<"): invalid argument" << endl
         << "PackageInfo should be constructed passing a string of the form "
         << "PACKAGE-VERSION." << endl
         << "Usually this error occurs because CMT can not determine the version "
         << "of your package. Please make sure that there is a version.cmt file "
         << "in the cmt directory of your package and that version.cmt contains "
         << "a single line of the form" <<  endl << "MyPackage-01-02-03" << endl
         << "Notice that version.cmt is created automatically by CMT when you use 'cmt co' " << endl
         << endl;
    throw invalid_argument("PackageInfo::PackageInfo");
  }
}
 
MsgStream&
operator << (MsgStream& ost, const Athena::PackageInfo& info) {
  return (ost << info.name() << '-' << info.version());
}
std::ostream&
operator << (std::ostream& ost, const Athena::PackageInfo& info) {
  return (ost << info.name() << '-' << info.version());
}
