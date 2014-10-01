#include <stdexcept>

#ifndef partition_H
#define partition_H

#include <iterator>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>

class partition
{
public:
  class iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           const std::vector<unsigned> >
  {
  public:
    iterator(unsigned n, bool first = true);

    reference operator*()  const { return kappa;  }
    pointer   operator->() const { return &kappa; }

    unsigned  size()     const { return kappa.size();      }
    unsigned  subsets()  const { return M[size() - 1] + 1; }

    iterator& operator++();
    iterator& operator--();

    template <typename Elem>
    std::auto_ptr<std::vector<std::vector<Elem> > >
    operator[](const std::vector<Elem> &v) const;

  protected:
    std::vector<unsigned> kappa, M;

    void integrityCheck();
  };

  class iterator_k
    : public iterator
  {
  public:
    iterator_k(unsigned n, unsigned psize, bool first = true);

    // optimized version
    unsigned subsets() const { return psize; }
    
    iterator_k& operator++();
    iterator_k& operator--();

  private:
    const unsigned psize;

    void integrityCheck();
  };
};

extern std::ostream& operator<<(std::ostream& out,
                                partition::iterator &it);

template <typename Elem>
std::auto_ptr<std::vector<std::vector<Elem> > >
partition::iterator::
operator[](const std::vector<Elem> &v)
  const
{
  std::vector<std::vector<Elem> > *part =
    new std::vector<std::vector<Elem> >(subsets());

  for (unsigned i = 0; i < size(); ++i)
    (*part)[kappa[i]].push_back(v[i]);

  return std::auto_ptr<std::vector<std::vector<Elem> > >(part);
}

template <typename Elem>
std::ostream&
operator<<(std::ostream &out, std::vector<Elem> &s)
{
  out << '(';

  for (unsigned i = 0; i < s.size() - 1; ++i) {
    out << s[i] << ' ';
  }
  out << *(s.end()-1) << ')';
  
  return out;
}

template <typename Elem>
std::ostream&
operator<<(std::ostream &out,
           std::vector<std::vector<Elem> > &part)
{
  out << '(';

  for (unsigned i = 0; i < part.size() - 1; ++i) {
    out << part[i] << ' ';
  }
  out << *(part.end()-1) << ')';
  
  return out;
}




partition::iterator::
iterator(unsigned n, bool first)
  : kappa(n), M(n)
{
  if (n <= 0)
    throw std::invalid_argument
      ("partition::iterator: n must be positive");

  if (! first)
    for (unsigned i = 1; i < n; ++i) {
      kappa[i] = i;
      M[i] = i;
    }

  // integrityCheck();
}

partition::iterator&
partition::iterator::
operator++()
{
  const unsigned n = size();

  for (unsigned i = n-1; i > 0; --i)
    if (kappa[i] <= M[i-1]) {
      ++kappa[i];

      const unsigned new_max = std::max(M[i], kappa[i]);
      M[i] = new_max;
      for (unsigned j = i + 1; j < n; ++j) {
        kappa[j] = 0;
        M[j] = new_max;
      }

      // integrityCheck();
      return *this;
    }

  throw std::overflow_error("no following partitions");
}

partition::iterator&
partition::iterator::
operator--()
{
  const unsigned n = size();

  for (unsigned i = n-1; i > 0; --i)
    if (kappa[i] > 0) {
      --kappa[i];

      const unsigned m_i = M[i-1];
      M[i] = m_i;
      for (unsigned j = i + 1; j < n; ++j) {
        const unsigned new_max = m_i + j - i;
        kappa[j] = new_max;
        M[j] = new_max;
      }

      // integrityCheck();
      return *this;
    }

  throw std::underflow_error("no preceding partitions");
}

void
partition::iterator::
integrityCheck()
{
  const unsigned n = size();
  unsigned max = kappa[0];

  for (unsigned i = 0; i < n; ++i) {
    max = std::max(max,kappa[i]);

    if (max != M[i])
      throw std::domain_error("integrity check failed");
  }
}

std::ostream&
operator<<(std::ostream& out, partition::iterator &it)
{
  out << '(';

  if (it->size() > 1)
    std::copy(it->begin(), it->end()-1,
              std::ostream_iterator<unsigned>(out, " "));
  
  out << *(it->end()-1) << ')';

  return out;
}


partition::iterator_k::
iterator_k(unsigned n, unsigned psize, bool first)
  : iterator(n, first), psize(psize)
{
  if (psize <= 0 || psize > n)
    throw std::invalid_argument
      ("partition::iterator_k: psize must be in [1..n]");

  if (first) {
    const unsigned offset = n - psize;
    for (unsigned i = offset + 1; i < n; ++i) {
      kappa[i] = i - offset;
      M[i] = i - offset;
    }
  } else {
    std::fill(kappa.begin() + psize, kappa.end(), psize - 1);
    std::fill(M.begin() + psize,     M.end(),     psize - 1);
  }
}

partition::iterator_k&
partition::iterator_k::
operator++()
{
  const unsigned n = size();

  for (unsigned i = n-1; i > 0; --i)
    if (kappa[i] < psize-1 && kappa[i] <= M[i-1]) {
      ++kappa[i];

      const unsigned new_max = std::max(M[i], kappa[i]);
      M[i] = new_max;
      
      for (unsigned j = i + 1; j <= n - (psize-new_max); ++j) {
        kappa[j] = 0;
        M[j] = new_max;
      }

      for (unsigned j = n - (psize-new_max) + 1; j < n; ++j) {
        const unsigned new_max = psize - (n - j);
        kappa[j] = new_max;
        M[j] = new_max;
      }

      // integrityCheck();
      return *this;
    }

  throw std::overflow_error("no following k-partitions");
}

partition::iterator_k&
partition::iterator_k::
operator--()
{
  const unsigned n = size();

  for (unsigned i = n-1; i > 0; --i)
    if (kappa[i] > 0 && psize - M[i-1] <= n - i) {
      --kappa[i];

      const unsigned m_i = M[i-1];
      M[i] = m_i;

      for (unsigned j = i + 1; j < i + (psize - m_i); ++j) {
        const unsigned new_max = m_i + j - i;
        kappa[j] = new_max;
        M[j] = new_max;
      }

      for (unsigned j = i + (psize - m_i); j < n; ++j) {
        kappa[j] = psize - 1;
        M[j] = psize - 1;
      }

      // integrityCheck();
      return *this;
    }

  throw std::underflow_error("no preceding k-partitions");
}

void
partition::iterator_k::
integrityCheck()
{
  iterator::integrityCheck();

  if (subsets() != iterator::subsets())
    throw std::domain_error("integrity check 2 failed");
}



#endif

