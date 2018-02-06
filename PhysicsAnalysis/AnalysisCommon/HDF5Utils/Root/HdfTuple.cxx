/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "HDF5Utils/HdfTuple.h"

#include "H5Cpp.h"

#include <cassert>
#include <iostream>             // for printing errors in the destructor

namespace ah5 {
  namespace internal {

    template<>
    H5::DataType get_type<float>() {
      return H5::PredType::NATIVE_FLOAT;
    }
    template<>
    H5::DataType get_type<int>() {
      return H5::PredType::NATIVE_INT;
    }
    template<>
    H5::DataType get_type<long long>() {
      return H5::PredType::NATIVE_LLONG;
    }
    template<>
    H5::DataType get_type<unsigned int>() {
      return H5::PredType::NATIVE_UINT;
    }
    template<>
    H5::DataType get_type<double>() {
      return H5::PredType::NATIVE_DOUBLE;
    }
    template<>
    H5::DataType get_type<bool>() {
      bool TRUE = true;
      bool FALSE = false;
      H5::EnumType btype(sizeof(bool));
      btype.insert("TRUE", &TRUE);
      btype.insert("FALSE", &FALSE);
      return btype;
    }

    template<> int& get_ref<int>(data_buffer_t& buf) {
      return buf._int;
    }
    template<> long long& get_ref<long long>(data_buffer_t& buf) {
      return buf._llong;
    }
    template<> unsigned int& get_ref<unsigned int>(data_buffer_t& buf) {
      return buf._uint;
    }
    template<> float& get_ref<float>(data_buffer_t& buf) {
      return buf._float;
    }
    template<> double& get_ref<double>(data_buffer_t& buf) {
      return buf._double;
    }
    template<> bool& get_ref<bool>(data_buffer_t& buf) {
      return buf._bool;
    }
  }
}


namespace {
  using namespace ah5;
  // packing utility
  H5::CompType packed(H5::CompType in) {
    // TODO: Figure out why a normal copy constructor doesn't work here.
    //       The normal one seems to create shallow copies.
    auto out = H5::CompType(H5Tcopy(in.getId()));
    out.pack();
    return out;
  }
  H5::CompType build_type(const VariableFillers& fillers) {
    using internal::data_buffer_t;
    H5::CompType type(fillers.size() * sizeof(data_buffer_t));
    size_t dt_offset = 0;
    for (const auto& filler: fillers) {
      type.insertMember(filler->name(), dt_offset, filler->get_type());
      dt_offset += sizeof(data_buffer_t);
    }
    return type;
  }

  void print_destructor_error(const std::string& msg) {
    std::cerr << "ERROR: an exception was thrown in the destructor of an "
      "HDF5 file, the output buffer may be corrupted";
    std::cerr << " (error message: " << msg << ")" << std::endl;
  }
}

// _______________________________________________________________________
// Xd writter
//

namespace ah5 {
  std::vector<size_t> WriterXd::NONE = {};

  WriterXd::WriterXd(H5::Group& group, const std::string& name,
                     VariableFillers fillers,
                     std::vector<hsize_t> max_length,
                     hsize_t batch_size):
    m_type(build_type(fillers)),
    m_max_length(max_length),
    m_dim_stride(max_length),
    m_batch_size(batch_size),
    m_offset(0),
    m_fillers(fillers)
  {
    if (batch_size < 1) {
      throw std::logic_error("batch size must be > 0");
    }
    // create space
    std::vector<hsize_t> initial{0};
    initial.insert(initial.end(), max_length.begin(), max_length.end());
    std::vector<hsize_t> eventual{H5S_UNLIMITED};
    eventual.insert(eventual.end(), max_length.begin(), max_length.end());
    H5::DataSpace space(eventual.size(), initial.data(), eventual.data());

    // create params
    H5::DSetCreatPropList params;
    std::vector<hsize_t> chunk_size{batch_size};
    chunk_size.insert(chunk_size.end(), max_length.begin(), max_length.end());
    params.setChunk(chunk_size.size(), chunk_size.data());
    params.setDeflate(7);

    // calculate striding
    m_dim_stride.push_back(1);
    for (size_t iii = m_dim_stride.size(); iii - 1 != 0; iii--) {
      m_dim_stride.at(iii-2) = m_dim_stride.at(iii-2) * m_dim_stride.at(iii-1);
    }

    // create ds
    if (H5Lexists(group.getLocId(), name.c_str(), H5P_DEFAULT)) {
      throw std::logic_error("tried to overwrite '" + name + "'");
    }
    m_ds = group.createDataSet(name, packed(m_type), space, params);
  }

  WriterXd::~WriterXd() {
    try {
      flush();
    } catch (H5::Exception& err) {
      print_destructor_error(err.getDetailMsg());
    } catch (std::exception& err) {
      print_destructor_error(err.what());
    }
  }

  void WriterXd::fillWhileIncrementing(std::vector<size_t>& indices) {
    if (buffer_size() == m_batch_size) {
      flush();
    }
    indices.resize(m_max_length.size());

    // build buffer and _then_ insert it so that exceptions don't leave
    // the buffer in a weird state
    std::vector<internal::data_buffer_t> temp;

    std::fill(indices.begin(), indices.end(), 0);
    for (size_t gidx = 0; gidx < m_dim_stride.front(); gidx++) {

      // we might be able to make this more efficient and less cryptic
      for (size_t iii = 0; iii < indices.size(); iii++) {
        indices.at(iii) = (gidx % m_dim_stride.at(iii)) /
          m_dim_stride.at(iii+1);
      }

      for (const auto& filler: m_fillers) {
        temp.push_back(filler->get_buffer());
      }
    }
    m_buffer.insert(m_buffer.end(), temp.begin(), temp.end());
  }
  void WriterXd::flush() {
    if (buffer_size() == 0) return;
    // extend the ds
    std::vector<hsize_t> slab_dims{buffer_size()};
    slab_dims.insert(slab_dims.end(), m_max_length.begin(), m_max_length.end());
    std::vector<hsize_t> total_dims{buffer_size() + m_offset};
    total_dims.insert(total_dims.end(), m_max_length.begin(), m_max_length.end());
    m_ds.extend(total_dims.data());

    // setup dataspaces
    H5::DataSpace file_space = m_ds.getSpace();
    H5::DataSpace mem_space(slab_dims.size(), slab_dims.data());
    std::vector<hsize_t> offset_dims{m_offset};
    offset_dims.resize(slab_dims.size(), 0);
    file_space.selectHyperslab(H5S_SELECT_SET,
                               slab_dims.data(), offset_dims.data());

    // write out
    assert(static_cast<size_t>(file_space.getSelectNpoints())
           == m_buffer.size() / m_fillers.size());
    m_ds.write(m_buffer.data(), m_type, mem_space, file_space);
    m_offset += buffer_size();
    m_buffer.clear();
  }

  hsize_t WriterXd::buffer_size() const {
    size_t n_entries = m_buffer.size() / m_fillers.size();
    assert(n_entries % m_dim_stride.front() == 0);
    return n_entries / m_dim_stride.front();
  }

}
