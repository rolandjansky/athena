/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_TRACKGRADE
#define JETTAGINFO_TRACKGRADE
#include <string>

namespace Analysis {

class TrackGrade {
 public:

  TrackGrade();

  TrackGrade(int,const std::string&);

  TrackGrade(const TrackGrade & );
  TrackGrade( TrackGrade && ) noexcept = default;
  TrackGrade &operator= (const TrackGrade& rhs);
  TrackGrade &operator= ( TrackGrade&& rhs) noexcept = default;
  bool operator== (const std::string &) const;
  bool operator== (const int &) const;
  bool operator== (const TrackGrade& rhs) const;

  virtual ~TrackGrade();

  const std::string & gradeString() const;

  int gradeNumber() const;

  operator int () const;

  operator const std::string () const;

 private:

  int m_gradeNumber;
  std::string m_gradeName;
};

}
#endif // JETTAGINFO_TRACKGRADE
