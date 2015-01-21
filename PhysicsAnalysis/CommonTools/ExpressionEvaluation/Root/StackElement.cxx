/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// StackElement.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/StackElement.h"
#include "ExpressionEvaluation/IProxyLoader.h"

#define UNARY_MATH_FUNCTION(FUNC, BASEFUNC) StackElement StackElement::FUNC() const \
  { \
    StackElement temp(*this); \
    temp.makeDouble(); \
    if (temp.type == SE_DOUBLE) { \
      temp.doubleval = BASEFUNC(temp.doubleval); \
      return temp; \
    } else if (temp.type == SE_VECDOUBLE) { \
      for (std::vector<double>::iterator it = temp.vecdoubleval.begin(); it != temp.vecdoubleval.end(); ++it) { \
        *it = BASEFUNC(*it); \
      } \
      return temp; \
    } else { \
      return 0; \
    } \
  }

#define IMPL_ASSIGN_OP(OP) template <> StackElement &StackElement::operator OP(const StackElement &rhs) \
  { \
    makeVectorIfNecessary(rhs); \
    makeDoubleIfNecessary(rhs); \
    if (type == SE_INT)            intval     OP rhs.scalarValue<int>(); \
    else if (type == SE_DOUBLE)    doubleval  OP rhs.scalarValue<double>(); \
    else if (type == SE_VECINT)    *this      OP rhs.vectorValue<int>(vecintval.size()); \
    else if (type == SE_VECDOUBLE) *this      OP rhs.vectorValue<double>(vecdoubleval.size()); \
    else                        throw std::runtime_error("StackElement ill-defined in " #OP); \
    return *this; \
  }

#define IMPL_BINARY_COMPARISON_OP(OP, BASEOP) template <> StackElement StackElement::OP(const StackElement &other) const \
  { \
    if (isScalar() && other.isScalar()) { \
      if ((type == StackElement::SE_INT) && (other.type == StackElement::SE_INT)) { \
        return (scalarValue<int>() BASEOP other.scalarValue<int>()); \
      } else { \
        return (scalarValue<double>() BASEOP other.scalarValue<double>()); \
      } \
    } else if (isVector() && other.isVector()) { \
      if ((type == StackElement::SE_VECINT) && (other.type == StackElement::SE_VECINT)) { \
        return (this->OP(other.vectorValue<int>())); \
      } else { \
        return (this->OP(other.vectorValue<double>())); \
      } \
    } else if (isVector() && other.isScalar()) { \
      if ((type == StackElement::SE_VECINT) && (other.type == StackElement::SE_INT)) { \
        return (this->OP(other.vectorValue<int>(vecintval.size()))); \
      } else { \
        return (this->OP(other.vectorValue<double>(vectorValue<int>().size()))); \
      } \
    } else if (isScalar() && other.isVector()) { \
      if (other.type == StackElement::SE_INT) { \
        return (this->OP(other.vectorValue<int>())); \
      } else { \
        return (this->OP(other.vectorValue<double>())); \
      } \
    }  else { \
      throw std::runtime_error("ERROR: Can't operate on SE_UNK StackElements"); \
    } \
  }

namespace ExpressionParsing {

  StackElement::StackElement()
    : type(SE_UNK),
      intval(0),
      doubleval(0),
      m_proxyLoader(nullptr),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(unsigned int val)
    : type(SE_INT),
      intval(val), 
      doubleval(0),
      m_proxyLoader(nullptr),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(int val)
    : type(SE_INT),
      intval(val),
      doubleval(0),
      m_proxyLoader(nullptr),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(double val)
    : type(SE_DOUBLE),
      intval(0),
      doubleval(val),
      m_proxyLoader(nullptr),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(const std::vector<int> &val)
    : type(SE_VECINT),
      intval(0),
      doubleval(0),
      vecintval(val),
      m_proxyLoader(0),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(const std::vector<double> &val)
    : type(SE_VECDOUBLE),
      intval(0),
      doubleval(0),
      vecdoubleval(val),
      m_proxyLoader(0),
      m_variableType(),
      m_determinedVariableType(false) { }
  StackElement::StackElement(const std::string &val, IProxyLoader *proxyLoader)
    : type(SE_UNK),
      intval(0),
      doubleval(0),
      varname(val),
      m_proxyLoader(proxyLoader),
      m_variableType(),
      m_determinedVariableType(false) { }

  StackElement StackElement::operator =(const int &rhs)
  {
    type = SE_INT; intval = rhs;
    return *this;
  }

  StackElement StackElement::operator =(const double &rhs)
  {
    type = SE_DOUBLE; doubleval = rhs; 
    return *this;
  }

  StackElement StackElement::operator =(const std::vector<int> &rhs)
  {
    type = SE_VECINT; vecintval = rhs;
    return *this;
  }

  StackElement StackElement::operator =(const std::vector<double> &rhs)
  {
    type = SE_VECDOUBLE; vecdoubleval = rhs;
    return *this;
  }

  StackElement StackElement::operator !() const
  {
    StackElement temp(*this);
    if (type == SE_INT)         temp.intval    = !intval;
    else if (type == SE_DOUBLE) {
      temp.makeInt();
      temp.intval = !doubleval;
    } else if (type == SE_VECINT) {
      for (std::vector<int>::iterator it = temp.vecintval.begin(); it != temp.vecintval.end(); ++it) {
        *it = !(*it);
      }
    } else if (type == SE_VECDOUBLE) {
      temp.makeInt();
      for (size_t i = 0; i < temp.vecintval.size(); ++i) {
        temp.vecintval[i] = !temp.vecdoubleval[i];
      }
    }
    return temp;
  }

  StackElement StackElement::operator -() const
  {
    StackElement temp(*this);
    if (type == SE_INT)         temp.intval    = -intval;
    else if (type == SE_DOUBLE) temp.doubleval = -doubleval;
    else if (type == SE_VECINT) {
      for (std::vector<int>::iterator it = temp.vecintval.begin(); it != temp.vecintval.end(); ++it) {
        *it = -(*it);
      }
    } else if (type == SE_VECDOUBLE) {
      for (std::vector<double>::iterator it = temp.vecdoubleval.begin(); it != temp.vecdoubleval.end(); ++it) {
        *it = -(*it);
      }
    }
    return temp;
  }


  StackElement StackElement::_sum() const
  {
    if (type == SE_INT) return intval;
    else if (type == SE_DOUBLE) return doubleval;
    else if (type == SE_VECINT) {
      int total = 0;
      for (std::vector<int>::const_iterator it = vecintval.begin(); it != vecintval.end(); ++it) {
        total += *it;
      }
      return total;
    } else if (type == SE_VECDOUBLE) {
      double total = 0.;
      for (std::vector<double>::const_iterator it = vecdoubleval.begin(); it != vecdoubleval.end(); ++it) {
        total += *it;
      }
      return total;
    } else {
      return 0;
    }
  }

  StackElement StackElement::_count() const
  {
    if (type == SE_INT) return !!intval;
    else if (type == SE_DOUBLE) return !!doubleval;
    else if (type == SE_VECINT) {
      int total = 0;
      for (std::vector<int>::const_iterator it = vecintval.begin(); it != vecintval.end(); ++it) {
        total += !!*it;
      }
      return total;
    } else if (type == SE_VECDOUBLE) {
      int total = 0;
      for (std::vector<double>::const_iterator it = vecdoubleval.begin(); it != vecdoubleval.end(); ++it) {
        total += !!*it;
      }
      return total;
    } else {
      return 0;
    }
  }

  StackElement StackElement::_abs() const
  {
    if (type == SE_INT) {
      if (intval < 0) return -intval;
      else return intval;
    } else if (type == SE_DOUBLE) {
      return fabs(doubleval);
    } else if (type == SE_VECINT) {
      std::vector<int> temp;
      for (std::vector<int>::const_iterator it = vecintval.begin(); it != vecintval.end(); ++it) {
        if (*it < 0) temp.push_back(-*it);
        else         temp.push_back(*it);
      }
      return temp;
    } else if (type == SE_VECDOUBLE) {
      std::vector<double> temp;
      for (std::vector<double>::const_iterator it = vecdoubleval.begin(); it != vecdoubleval.end(); ++it) {
        temp.push_back(fabs(*it));
      }
      return temp;
    } else {
      return 0;
    }
  }


  IMPL_BINARY_COMPARISON_OP(_eq, ==)
  IMPL_BINARY_COMPARISON_OP(_neq, !=)
  IMPL_BINARY_COMPARISON_OP(_and, &&)
  IMPL_BINARY_COMPARISON_OP(_or, ||)
  IMPL_BINARY_COMPARISON_OP(_gt, >)
  IMPL_BINARY_COMPARISON_OP(_gte, >=)
  IMPL_BINARY_COMPARISON_OP(_lt, <)
  IMPL_BINARY_COMPARISON_OP(_lte, <=)

  IMPL_ASSIGN_OP(-=)
  IMPL_ASSIGN_OP(+=)
  IMPL_ASSIGN_OP(*=)
  IMPL_ASSIGN_OP(/=)

  template <> StackElement StackElement::_pow(const StackElement &n) const
  {
    if (n.isVector()) throw std::runtime_error("Can't use vector as exponent");
    else return _pow(n.scalarValue<double>());
  }

  UNARY_MATH_FUNCTION(_sqrt, sqrt)
  UNARY_MATH_FUNCTION(_cbrt, cbrt)
  UNARY_MATH_FUNCTION(_sin, sin)
  UNARY_MATH_FUNCTION(_cos, cos)
  UNARY_MATH_FUNCTION(_tan, tan)
  UNARY_MATH_FUNCTION(_asin, asin)
  UNARY_MATH_FUNCTION(_acos, acos)
  UNARY_MATH_FUNCTION(_atan, atan)
  UNARY_MATH_FUNCTION(_sinh, sinh)
  UNARY_MATH_FUNCTION(_cosh, cosh)
  UNARY_MATH_FUNCTION(_tanh, tanh)
  UNARY_MATH_FUNCTION(_asinh, asinh)
  UNARY_MATH_FUNCTION(_acosh, acosh)
  UNARY_MATH_FUNCTION(_atanh, atanh)
  UNARY_MATH_FUNCTION(_log, log)
  UNARY_MATH_FUNCTION(_exp, exp)

  StackElement::ElementType StackElement::getType() const
  {
    return type;
  }

  bool StackElement::isScalar() const
  {
    return (type == SE_INT || type == SE_DOUBLE);
  }

  bool StackElement::isVector() const
  {
    return (type == SE_VECINT || type == SE_VECDOUBLE);
  }

  bool StackElement::isProxy() const
  {
    return (varname.length() > 0);
  }


  void StackElement::setValueFromProxy()
  {
    if (!isProxy()) return;

    if (!m_determinedVariableType) {
      m_variableType = m_proxyLoader->variableTypeFromString(varname);
      m_determinedVariableType = true;
    }
    switch (m_variableType) {
      case IProxyLoader::VT_INT:
        type = SE_INT;
        intval = m_proxyLoader->loadIntVariableFromString(varname);
        break;
      case IProxyLoader::VT_DOUBLE:
        type = SE_DOUBLE;
        doubleval = m_proxyLoader->loadDoubleVariableFromString(varname);
        break;
      case IProxyLoader::VT_VECINT:
        type = SE_VECINT;
        vecintval = m_proxyLoader->loadVecIntVariableFromString(varname);
        break;
      case IProxyLoader::VT_VECDOUBLE:
        type = SE_VECDOUBLE;
        vecdoubleval = m_proxyLoader->loadVecDoubleVariableFromString(varname);
        break;
      case IProxyLoader::VT_UNK:
      default:
        throw std::runtime_error("Got VT_UNK - unknown identifier: " + varname);
        break;
    }
  }

  void StackElement::clearValueFromProxy()
  {
    if (!isProxy()) return;
    type = SE_UNK;
    intval = 0;
    doubleval = 0.;
    vecintval.clear();
    vecdoubleval.clear();
  }

  int StackElement::asInt() const
  {
    if (type != SE_INT) {
      throw std::runtime_error("asInt() only valid for SE_INT");
    }
    return scalarValue<int>();
  }

  bool StackElement::asBool() const
  {
    if (!((type == SE_INT) || (type == SE_DOUBLE))) {
      throw std::runtime_error("asInt() only valid for non-vector types");
    }
    return scalarValue<bool>();
  }

  void StackElement::makeInt()
  {
    if (type == SE_DOUBLE) {
      type = SE_INT;
      intval = doubleval;
    } else if (type == SE_VECDOUBLE) {
      type = SE_VECINT;
      vecintval.clear();
      for (std::vector<double>::const_iterator it = vecdoubleval.begin(); it != vecdoubleval.end(); ++it) {
        vecintval.push_back((int) *it);
      }
    }
  }

  void StackElement::makeDouble()
  {
    if (type == SE_INT) {
      type = SE_DOUBLE;
      doubleval = intval;
    } else if (type == SE_VECINT) {
      type = SE_VECDOUBLE;
      vecdoubleval.clear();
      for (std::vector<int>::const_iterator it = vecintval.begin(); it != vecintval.end(); ++it) {
        vecdoubleval.push_back((double) *it);
      }
    }
  }

  void StackElement::makeVector(size_t n)
  {
    if (isVector()) return;

    if (type == SE_INT) {
      type = SE_VECINT;
      vecintval.clear();
      vecintval.resize(n, intval);
    } else if (type == SE_DOUBLE) {
      type = SE_VECDOUBLE;
      vecdoubleval.clear();
      vecdoubleval.resize(n, doubleval);
    }
  }

  void StackElement::makeVectorIfNecessary(const StackElement &other)
  {
    if (isVector()) return;
    if (other.isVector()) {
      makeVector(other.vectorValue<int>().size());
    }
  }

  void StackElement::makeVectorIfNecessary(const std::vector<int> &other)
  {
    if (isVector()) return;
    makeVector(other.size());
  }

  void StackElement::makeVectorIfNecessary(const std::vector<double> &other)
  {
    if (isVector()) return;
    makeVector(other.size());
  }

  void StackElement::makeDoubleIfNecessary(const StackElement &other) 
  {
    if ((type == SE_INT) && (other.type == SE_DOUBLE)) {
      makeDouble();
    } else if ((type == SE_VECINT) && (other.type == SE_VECDOUBLE)) {
      makeDouble();
    }
  }

  void StackElement::makeDoubleIfNecessary(const int &)
  {
  }

  void StackElement::makeDoubleIfNecessary(const double &)
  {
    if (this->type == SE_INT) {
      makeDouble();
    }
  }

  void StackElement::makeDoubleIfNecessary(const std::vector<int> &)
  {
  }

  void StackElement::makeDoubleIfNecessary(const std::vector<double> &)
  {
    if (this->type == SE_VECINT) {
      makeDouble();
    }
  }

  void StackElement::ensureCompatible(const StackElement &other) const
  {
    if (this->type != other.type) {
      throw std::runtime_error("Incompatible stack elements");
    }
  }

  template <> std::vector<int> StackElement::vectorValue(size_t sizeIfScalar) const
  {
    if (type == SE_VECINT) {
      return vecintval;
    } else if (type == SE_VECDOUBLE) {
      std::vector<int> retval;
      for (std::vector<double>::const_iterator it = vecdoubleval.begin(); it != vecdoubleval.end(); ++it) {
        retval.push_back((int) *it);
      }
      return retval;
    } else if (type == SE_INT) {
      return std::vector<int>(sizeIfScalar, intval);
    } else if (type == SE_DOUBLE) {
      return std::vector<int>(sizeIfScalar, doubleval);
    } else {
      std::cerr << "ERROR: Shouldn't be here! Throwing. SE: " << *this << std::endl;
      throw std::runtime_error("(int) vectorValue(): Unsupported StackElement");
    }
  }

  template <> std::vector<double> StackElement::vectorValue(size_t sizeIfScalar) const
  {
    if (type == SE_VECDOUBLE) {
      return vecdoubleval;
    } else if (type == SE_VECINT) {
      std::vector<double> retval;
      for (std::vector<int>::const_iterator it = vecintval.begin(); it != vecintval.end(); ++it) {
        retval.push_back((double) *it);
      }
      return retval;
    } else if (type == SE_INT) {
      return std::vector<double>(sizeIfScalar, intval);
    } else if (type == SE_DOUBLE) {
      return std::vector<double>(sizeIfScalar, doubleval);
    } else {
      std::cerr << "ERROR: Shouldn't be here! Throwing. SE: " << *this << std::endl;
      throw std::runtime_error("(dbl) vectorValue(): Unsupported StackElement");
    }
  }


  template <> void StackElement::ensureCompatibleVectors(const StackElement &other) const
  {
    if (isScalar()) return;
    size_t ourlen = vectorValue<int>().size();
    if (ourlen != other.vectorValue<int>().size()) {
      throw std::runtime_error("Incompatible vectors - different length");
    }
  }

  std::ostream &operator <<(std::ostream &os, const StackElement &el)
  {
    switch (el.type) {
      case StackElement::SE_INT: os  << "(int)" << el.intval; break;
      case StackElement::SE_DOUBLE: os << "(double)" << el.doubleval; break;
      case StackElement::SE_VECINT: os << "(vec<int>)" << el.vecintval; break;
      case StackElement::SE_VECDOUBLE: os << "(vec<double>)" << el.vecdoubleval; break;
      case StackElement::SE_UNK: os << "SE_UNK"; break;
      default: break;
    }
    return os;
  }

}
