/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// StackElement.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef STACK_ELEMENT_H
#define STACK_ELEMENT_H

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "ExpressionEvaluation/IProxyLoader.h"

#define DEFINE_BINARY_COMPARISON_OP(OP) template <typename T> StackElement OP(const T &other) const; \
                                        template <typename T> StackElement OP(const std::vector<T> &other) const

namespace ExpressionParsing {
  class StackElement {
    public:
      enum ElementType { SE_UNK, SE_INT, SE_DOUBLE, SE_VECINT, SE_VECDOUBLE };

      StackElement();
      StackElement(unsigned int val);
      StackElement(int val);
      StackElement(double val);
      StackElement(const std::vector<int> &val);
      StackElement(const std::vector<double> &val);
      StackElement(const std::string &val, IProxyLoader *proxyLoader);
      StackElement(const StackElement &other);

      StackElement operator =(const int &rhs);
      StackElement operator =(const double &rhs);
      StackElement operator =(const std::vector<int> &rhs);
      StackElement operator =(const std::vector<double> &rhs);

      template <typename T> bool operator ==(const T &other) const;
      template <typename T> bool operator !=(const T &other) const;
      template <typename T> bool operator &&(const T &other) const;
      template <typename T> bool operator ||(const T &other) const;
      template <typename T> bool operator >(const T &other) const;
      template <typename T> bool operator >=(const T &other) const;
      template <typename T> bool operator <(const T &other) const;
      template <typename T> bool operator <=(const T &other) const;

      DEFINE_BINARY_COMPARISON_OP(_eq);
      DEFINE_BINARY_COMPARISON_OP(_neq);
      DEFINE_BINARY_COMPARISON_OP(_and);
      DEFINE_BINARY_COMPARISON_OP(_or);
      DEFINE_BINARY_COMPARISON_OP(_gt);
      DEFINE_BINARY_COMPARISON_OP(_gte);
      DEFINE_BINARY_COMPARISON_OP(_lt);
      DEFINE_BINARY_COMPARISON_OP(_lte);

      template <typename T> StackElement &operator -=(const T &rhs);
      template <typename T> StackElement &operator -=(const std::vector<T> &rhs);
      template <typename T> StackElement &operator +=(const T &rhs);
      template <typename T> StackElement &operator +=(const std::vector<T> &rhs);
      template <typename T> StackElement &operator *=(const T &rhs);
      template <typename T> StackElement &operator *=(const std::vector<T> &rhs);
      template <typename T> StackElement &operator /=(const T &rhs);
      template <typename T> StackElement &operator /=(const std::vector<T> &rhs);

      template <typename T> StackElement operator -(const T &rhs) const;
      template <typename T> StackElement operator +(const T &rhs) const;
      template <typename T> StackElement operator *(const T &rhs) const;
      template <typename T> StackElement operator /(const T &rhs) const;

      StackElement operator !() const;
      StackElement operator -() const;

      template <typename T> StackElement _pow(const T &n) const;
      StackElement _sum() const;
      StackElement _count() const;
      StackElement _abs() const;
      StackElement _sqrt() const;
      StackElement _cbrt() const;
      StackElement _sin() const;
      StackElement _cos() const;
      StackElement _tan() const;
      StackElement _asin() const;
      StackElement _acos() const;
      StackElement _atan() const;
      StackElement _sinh() const;
      StackElement _cosh() const;
      StackElement _tanh() const;
      StackElement _asinh() const;
      StackElement _acosh() const;
      StackElement _atanh() const;
      StackElement _log() const;
      StackElement _exp() const;

      ElementType getType() const;
      bool isScalar() const;
      bool isVector() const;
      bool isProxy() const;
      void setValueFromProxy();
      void clearValueFromProxy();

      int asInt() const;
      bool asBool() const;
      void makeInt();
      void makeDouble();

      template <typename T> T scalarValue() const;
      template <typename T> std::vector<T> vectorValue(size_t sizeIfScalar = 0) const;

      friend std::ostream &operator <<(std::ostream &os, const StackElement &el);

    private:
      void makeVector(size_t n);

      void makeVectorIfNecessary(const StackElement &other);
      void makeVectorIfNecessary(const std::vector<int> &other);
      void makeVectorIfNecessary(const std::vector<double> &other);

      void makeDoubleIfNecessary(const StackElement &other);
      void makeDoubleIfNecessary(const int &other);
      void makeDoubleIfNecessary(const double &other);
      void makeDoubleIfNecessary(const std::vector<int> &other);
      void makeDoubleIfNecessary(const std::vector<double> &other);

      void ensureCompatible(const StackElement &other) const;
      template <typename T> void ensureCompatibleVectors(const T &other) const;
      template <typename T> void ensureCompatibleVectors(const std::vector<T> &other) const;

    private:
      ElementType type;
      int intval;
      double doubleval;
      std::vector<int> vecintval;
      std::vector<double> vecdoubleval;
      std::string varname;
      IProxyLoader *m_proxyLoader;
      IProxyLoader::VariableType m_variableType;
      bool m_determinedVariableType;
  };
}

#include "ExpressionEvaluation/StackElement.icc"

#endif // STACK_ELEMENT_H
