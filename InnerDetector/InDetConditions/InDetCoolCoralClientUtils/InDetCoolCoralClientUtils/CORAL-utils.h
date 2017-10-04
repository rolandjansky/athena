/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORAL_UTILS_HDR
#define CORAL_UTILS_HDR

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <time.h>
#include <sys/time.h>
#include <fstream>

#include <errno.h>


#define VECTOR2STRING( my_vector, my_string) { \
    std::ostringstream os_; \
    size_t k_; \
    os_ << my_vector.size() << ' '; \
    for (k_ = 0; k_ < my_vector.size(); k_ ++) { \
        os_ << my_vector[k_] << ' '; \
    } \
    my_string = os_.str(); \
}
                                                                                                                   
#define STRING2VECTOR( my_string, my_vector) { \
    std::istringstream is_ (my_string); \
    size_t new_size_, k_; \
    is_ >> new_size_; \
    if (new_size_ >9000000) {throw std::out_of_range("bad array limit in CORAL-utils.h");}\
     __typeof__ (my_vector[0]) tmp_; \
    my_vector.clear(); \
    for (k_ = 0; k_ < new_size_; k_ ++) { \
        is_ >> tmp_; \
        my_vector.push_back (tmp_); \
    } \
}

template <typename T>
bool FromString(T &aValue, const std::string &aStr)
{
  std::stringstream ss(aStr);
  ss >> aValue;
  return !ss.fail();
}

template <typename T>
std::string ToString(T aValue)
{
  std::stringstream ss;
  ss << aValue;
  return ss.str();
}


#endif // COOL_RAL_UTILS_HDR


