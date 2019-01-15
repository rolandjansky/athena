// this is -*- C++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HDF_TUPLE_HH
#define HDF_TUPLE_HH

/**
 * HDF5 Writer
 *
 * Skip down to the `Writer` and `Consumers` classes below to see the
 * stuff that you'll have to interact with.
 *
 **/

#include "H5Traits.h"
#include "common.h"

#include "H5Cpp.h"

#include <functional>
#include <vector>
#include <memory>
#include <cassert>
#include <set>

namespace H5Utils {

  /// @brief internal clssses and code
  /// @{

  namespace internal {

    /** @brief DataConsumer classes
     *
     * These are the constituents of the `Consumers` class, which is
     * an argument to the `Writer` constructor. Each consumer is a
     * wrapper on a std::function, which is called each time the user
     * calls `Writer::fill(...)`.
     *
     **/
    template <typename I>
    class IDataConsumer
    {
    public:
      virtual ~IDataConsumer() {}
      virtual data_buffer_t getBuffer(I) const = 0;
      virtual data_buffer_t getDefault() const = 0;
      virtual H5::DataType getType() const = 0;
      virtual std::string name() const = 0;
      typedef I input_type;
    };

    /// implementation for variable filler
    template <typename T, typename I>
    class DataConsumer: public IDataConsumer<I>
    {
    public:
      DataConsumer(const std::string&,
                   const std::function<T(I)>&,
                   const T default_value = T());
      data_buffer_t getBuffer(I) const override;
      data_buffer_t getDefault() const override;
      H5::DataType getType() const override;
      std::string name() const override;
    private:
      std::function<T(I)> m_getter;
      std::string m_name;
      T m_default_value;
    };
    template <typename T, typename I>
    DataConsumer<T, I>::DataConsumer(const std::string& name,
                                     const std::function<T(I)>& func,
                                     const T default_value):
      m_getter(func),
      m_name(name),
      m_default_value(default_value)
    {
    }
    template <typename T, typename I>
    data_buffer_t DataConsumer<T, I>::getBuffer(I args) const {
      data_buffer_t buffer;
      H5Traits<T>::ref(buffer) = m_getter(args);
      return buffer;
    }
    template <typename T, typename I>
    data_buffer_t DataConsumer<T, I>::getDefault() const {
      data_buffer_t default_value;
      H5Traits<T>::ref(default_value) = m_default_value;
      return default_value;
    }
    template <typename T, typename I>
    H5::DataType DataConsumer<T, I>::getType() const {
      return H5Traits<T>::type;
    }
    template <typename T, typename I>
    std::string DataConsumer<T, I>::name() const {
      return m_name;
    }
  }
  /// @}

  /** @brief Consumer Class
   *
   * The elements added to this container each specify one element in
   * the output HDF5 DataSet. You need to give each variable a name
   * and a function that fills the variable.
   *
   **/
  /// @{
  template <typename I>
  using SharedConsumer = std::shared_ptr<internal::IDataConsumer<I> >;
  template <typename I>
  class Consumers
  {
  public:

    /// This should be the only method you need in this class
    template <typename T>
    void add(const std::string& name, const std::function<T(I)>&,
             const T& default_value = T());

    /// overload to cast lambdas into functions
    template <typename T, typename F>
    void add(const std::string& name, const F func, const T& def = T()) {
      add(name, std::function<T(I)>(func), def);
    }

    std::vector<SharedConsumer<I> > getConsumers() const;

    typedef I input_type;

  private:
    std::vector<SharedConsumer<I> > m_consumers;
    std::set<std::string> m_used;
  };
  ///@}

  template <typename I>
  template <typename T>
  void Consumers<I>::add(const std::string& name,
                         const std::function<T(I)>& fun,
                         const T& def_val)
  {
    if (m_used.count(name)) {
      throw std::logic_error("tried to insert '" + name + "' twice");
    }
    m_consumers.push_back(
      std::make_shared<internal::DataConsumer<T, I> >(name, fun, def_val));
    m_used.insert(name);
  }
  template <typename I>
  std::vector<SharedConsumer<I> > Consumers<I>::getConsumers() const {
    return m_consumers;
  }


  /// @brief internal code for the `Writer` class
  /// @{
  namespace internal {

    // This class exists to inspect the way the fill(...) function is
    // called, so that errors can be caught with a static assert and
    // give a much less cryptic error message.
    //
    // We check to make sure the depth of the input matches the rank
    // of the output, and that the types match.
    using std::begin;
    template <size_t N, typename T, typename I, typename = void>
    struct CheckType {
      static const bool ok_type = std::is_convertible<T,I>::value;
      static const bool any_type = ok_type;
      static const int depth = 0;
    };
    template <size_t N, typename T, typename I>
    struct CheckType <N,T,I,decltype(*begin(std::declval<T&>()), void())> {
      typedef CheckType<N-1,decltype(*begin(std::declval<T&>())),I> subtype;
      static const bool ok_type = subtype::ok_type;
      static const bool any_type = (
        subtype::any_type || std::is_convertible<T,I>::value);
      static const int depth = subtype::depth + 1;
    };
    template <typename T, typename I>
    struct CheckType <0,T,I,decltype(*begin(std::declval<T&>()), void())> {
      typedef CheckType<0,decltype(*begin(std::declval<T&>())),I> subtype;
      static const bool ok_type = std::is_convertible<T,I>::value;
      static const bool any_type = subtype::any_type || ok_type;
      static const int depth = subtype::depth + 1;
    };

    /// Data flattener class: this is used by the writer to read in the
    /// elements one by one and put them in an internal buffer.
    ///
    /// We use a struct rather than a full blown class. Like all
    /// things in the internal namespace, you should not mess with
    /// this unless you really know what you are doing.
    ///
    template <size_t N, typename F, typename T, size_t M = N>
    struct DataFlattener {

      std::vector<data_buffer_t> buffer;
      std::vector<std::array<hsize_t, N> > element_offsets;
      DataFlattener(const F& filler, T args,
                    const std::array<hsize_t,M>& extent):
        buffer() {
        hsize_t offset = 0;
        for (const auto& arg: args) {
          const size_t this_dim_max = extent.at(N-1);
          if (offset > this_dim_max) return;
          DataFlattener<N-1, F, decltype(arg), M> in(filler, arg, extent);
          buffer.insert(buffer.end(), in.buffer.begin(), in.buffer.end());
          for (const auto& in_ele: in.element_offsets){
            std::array<hsize_t, N> element_pos;
            element_pos.at(0) = offset;
            std::copy(in_ele.begin(), in_ele.end(), element_pos.begin() + 1);
            element_offsets.push_back(element_pos);
          }
          offset++;
        }
      }
    };
    template <typename F, typename T, size_t M>
    struct DataFlattener<0, F, T, M> {
      std::vector<data_buffer_t> buffer;
      std::vector<std::array<hsize_t, 0> > element_offsets;
      DataFlattener(const F& f, T args,
                    const std::array<hsize_t,M>& /*extent*/):
        buffer(),
        element_offsets(1) {
        for (const auto& filler: f) {
          buffer.push_back(filler->getBuffer(args));
        }
      }
    };

    /// Adapter to translate configuration info into the objects
    /// needed by the writer.
    template<typename I>
    H5::CompType buildType(const std::vector<SharedConsumer<I> >& consumers) {
      if (consumers.size() < 1) {
        throw std::logic_error(
          "you must specify at least one consumer when initializing the HDF5"
          "writer");
      }

      H5::CompType type(consumers.size() * sizeof(data_buffer_t));
      size_t dt_offset = 0;
      for (const SharedConsumer<I>& filler: consumers) {
        type.insertMember(filler->name(), dt_offset, filler->getType());
        dt_offset += sizeof(data_buffer_t);
      }
      return type;
    }

    /// Constant parameters for the writer
    template <typename I, size_t N>
    struct DSParameters {
      DSParameters(const std::vector<SharedConsumer<I> >& fillers,
                   const std::array<hsize_t,N>& extent,
                   hsize_t batch_size);
      H5::CompType type;
      std::array<hsize_t,N> extent;
      hsize_t batch_size;
    };

    // DS parameters
    template <typename I, size_t N>
    DSParameters<I,N>::DSParameters(const std::vector<SharedConsumer<I> >& cons,
      const std::array<hsize_t,N>& extent_,
      hsize_t batch_size_):
      type(buildType(cons)),
      extent(extent_),
      batch_size(batch_size_)
    {
    }


    template<typename I>
    std::vector<data_buffer_t> buildDefault(const std::vector<SharedConsumer<I> >& f) {
      std::vector<data_buffer_t> def;
      for (const SharedConsumer<I>& filler: f) {
        def.push_back(filler->getDefault());
      }
      return def;
    }

    // some internal functions take a vector, others take arrays
    template <size_t N>
    std::vector<hsize_t> vec(std::array<hsize_t,N> a) {
      return std::vector<hsize_t>(a.begin(),a.end());
    }

    // default initalizer for writers where the extent isn't specified
    template <hsize_t N>
    std::array<hsize_t, N> uniform(size_t val) {
      std::array<hsize_t, N> ar;
      ar.fill(val);
      return ar;
    }


  }
  /// @}

  /** @brief Writer
   *
   * You'll have to specify the H5::Group to write the dataset to, the
   * name of the new dataset, and the extent of the dataset.
   *
   * To fill, use the `fill(...)` method.
   **/
  template <size_t N, typename I>
  class Writer {
  public:
    Writer(H5::Group& group, const std::string& name,
           const Consumers<I>& consumers,
           const std::array<hsize_t, N>& extent = internal::uniform<N>(5),
           hsize_t batch_size = 2048);
    Writer(const Writer&) = delete;
    Writer(Writer&&) = default;
    Writer& operator=(Writer&) = delete;
    ~Writer();
    template <typename T>
    void fill(T);
    void flush();
    size_t index() const;
  private:
    const internal::DSParameters<I,N> m_par;
    hsize_t m_offset;
    hsize_t m_buffer_rows;
    std::vector<internal::data_buffer_t> m_buffer;
    std::vector<SharedConsumer<I> > m_consumers;
    H5::DataSet m_ds;
    H5::DataSpace m_file_space;
  };

  template <size_t N, typename I>
  Writer<N, I>::Writer(H5::Group& group, const std::string& name,
                       const Consumers<I>& consumers,
                       const std::array<hsize_t,N>& extent,
                       hsize_t batch_size):
    m_par(consumers.getConsumers(), extent, batch_size),
    m_offset(0),
    m_buffer_rows(0),
    m_consumers(consumers.getConsumers()),
    m_file_space(H5S_SIMPLE)
  {
    using internal::packed;
    using internal::data_buffer_t;
    if (batch_size < 1) {
      throw std::logic_error("batch size must be > 0");
    }
    // create space
    H5::DataSpace space = internal::getUnlimitedSpace(internal::vec(extent));

    // create params
    H5::DSetCreatPropList params = internal::getChunckedDatasetParams(
      internal::vec(extent), batch_size);
    std::vector<data_buffer_t> default_value = internal::buildDefault(
      consumers.getConsumers());
    params.setFillValue(m_par.type, default_value.data());

    // create ds
    internal::throwIfExists(name, group);
    m_ds = group.createDataSet(name, packed(m_par.type), space, params);
    m_file_space = m_ds.getSpace();
    m_file_space.selectNone();
  }

  template <size_t N, typename I>
  Writer<N, I>::~Writer() {
    using namespace H5Utils;
    try {
      flush();
    } catch (H5::Exception& err) {
      internal::printDestructorError(err.getDetailMsg());
    } catch (std::exception& err) {
      internal::printDestructorError(err.what());
    }
  }

  template <size_t N, typename I>
  template <typename T>
  void Writer<N, I>::fill(T arg) {
    if (m_buffer_rows == m_par.batch_size) {
      flush();
    }

    // make some assertions
    typedef internal::CheckType<N, T, I> checkType;
    static_assert(
      checkType::depth >= N,
      "\n\n"
      " ** H5 Writer rank is greater than the depth of fill(...) input! **"
      " \n");
    static_assert(
      !(checkType::any_type && !checkType::ok_type),
      "\n\n"
      " ** H5 Writer input type matches fill(...), but rank is incorrect! **"
      " \n");
    static_assert(
      checkType::any_type,
      "\n\n"
      " ** H5 Writer input type doesn't match input for fill(...)! **"
      " \n");

    internal::DataFlattener<N, decltype(m_consumers), T> buf(
      m_consumers, arg, m_par.extent);
    hsize_t n_el = buf.element_offsets.size();
    std::vector<hsize_t> elements;
    for (const auto& el_local: buf.element_offsets) {
      std::array<hsize_t, N+1> el_global;
      el_global[0] = m_offset + m_buffer_rows;
      std::copy(el_local.begin(), el_local.end(), el_global.begin() + 1);
      elements.insert(elements.end(), el_global.begin(), el_global.end());
    }
    if (n_el > 0) {
      m_file_space.selectElements(H5S_SELECT_APPEND, n_el, elements.data());
    }
    m_buffer.insert(m_buffer.end(), buf.buffer.begin(), buf.buffer.end());
    m_buffer_rows++;
  }

  template <size_t N, typename I>
  void Writer<N, I>::flush() {
    const hsize_t buffer_size = m_buffer_rows;
    if (buffer_size == 0) return;

    // extend the ds
    std::vector<hsize_t> total_dims{buffer_size + m_offset};
    total_dims.insert(total_dims.end(),
                      m_par.extent.begin(),
                      m_par.extent.end());
    m_ds.extend(total_dims.data());
    m_file_space.setExtentSimple(total_dims.size(), total_dims.data());

    // write out
    hsize_t n_buffer_pts = m_buffer.size() / m_consumers.size();
    assert(m_file_space.getSelectNpoints() == n_buffer_pts);
    H5::DataSpace mem_space(1, &n_buffer_pts);
    m_ds.write(m_buffer.data(), m_par.type, mem_space, m_file_space);
    m_offset += buffer_size;
    m_buffer.clear();
    m_buffer_rows = 0;
    m_file_space.selectNone();
  }

  template <size_t N, typename I>
  size_t Writer<N, I>::index() const {
    return m_buffer_rows + m_offset;
  }


}

#endif
