#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "hdf5.h"
//hdf5 reader
#ifndef LHEH5_H
#define LHEH5_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>

//#include <highfive/H5File.hpp> --Already included
//#include <highfive/H5DataSet.hpp> --Already included
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

///H5File///
////////////


#ifndef H5FILE_HPP
#define H5FILE_HPP

#include <string>

//#include "H5FileDriver.hpp"
///H5FileDriver///
//////////////////

#ifndef H5FILEDRIVER_HPP
#define H5FILEDRIVER_HPP

//#include "H5PropertyList.hpp"
///H5PropertyList///
////////////////////

#ifndef H5PROPERTY_LIST_HPP
#define H5PROPERTY_LIST_HPP

#include <H5Ppublic.h>

//#include "H5Exception.hpp"
///H5Exception///
/////////////////

#ifndef H5EXCEPTION_HPP
#define H5EXCEPTION_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include <H5Ipublic.h>

//#include "bits/H5Utils.hpp"
///H5Utils///
/////////////

#ifndef H5UTILS_HPP
#define H5UTILS_HPP

// internal utilities functions
#include <algorithm>
#include <array>
#include <cstddef> // __GLIBCXX__
#include <exception>
#include <string>
#include <type_traits>
#include <vector>

#ifdef H5_USE_BOOST
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

#ifdef H5_USE_EIGEN
#include <Eigen/Eigen>
#endif

#include <H5public.h>

namespace HighFive {

namespace details {

// determine at compile time number of dimensions of in memory datasets
template <typename T>
struct array_dims {
    static constexpr size_t value = 0;
};

template <typename T>
struct array_dims<std::vector<T> > {
    static constexpr size_t value = 1 + array_dims<T>::value;
};

template <typename T>
struct array_dims<T*> {
    static constexpr size_t value = 1 + array_dims<T>::value;
};

template <typename T, std::size_t N>
struct array_dims<T[N]> {
    static constexpr size_t value = 1 + array_dims<T>::value;
};

// Only supporting 1D arrays at the moment
template<typename T, std::size_t N>
struct array_dims<std::array<T,N>> {
    static constexpr size_t value = 1 + array_dims<T>::value;
};

#ifdef H5_USE_BOOST
template <typename T, std::size_t Dims>
struct array_dims<boost::multi_array<T, Dims> > {
    static constexpr size_t value = Dims;
};

template <typename T>
struct array_dims<boost::numeric::ublas::matrix<T> > {
    static constexpr size_t value = 2;
};
#endif

#ifdef H5_USE_EIGEN
template<typename T, int M, int N>
struct array_dims<Eigen::Matrix<T, M, N>> {
    static constexpr size_t value = 2;
};

template<typename T, int M, int N>
struct array_dims<std::vector<Eigen::Matrix<T, M, N>>> {
    static constexpr size_t value = 2;
};
#endif

// determine recursively the size of each dimension of a N dimension vector
template <typename T>
void get_dim_vector_rec(const T& vec, std::vector<size_t>& dims) {
    (void)dims;
    (void)vec;
}

template <typename T>
void get_dim_vector_rec(const std::vector<T>& vec, std::vector<size_t>& dims) {
    dims.push_back(vec.size());
    get_dim_vector_rec(vec[0], dims);
}

template <typename T>
std::vector<size_t> get_dim_vector(const std::vector<T>& vec) {
    std::vector<size_t> dims;
    get_dim_vector_rec(vec, dims);
    return dims;
}

// determine at compile time recursively the basic type of the data
template <typename T>
struct type_of_array {
    typedef T type;
};

template <typename T>
struct type_of_array<std::vector<T>> {
    typedef typename type_of_array<T>::type type;
};

template <typename T, std::size_t N>
struct type_of_array<std::array<T, N>> {
    typedef typename type_of_array<T>::type type;
};

#ifdef H5_USE_BOOST
template <typename T, std::size_t Dims>
struct type_of_array<boost::multi_array<T, Dims>> {
    typedef typename type_of_array<T>::type type;
};

template <typename T>
struct type_of_array<boost::numeric::ublas::matrix<T>> {
    typedef typename type_of_array<T>::type type;
};
#endif

#ifdef H5_USE_EIGEN
template<typename T, int M, int N>
struct type_of_array<Eigen::Matrix<T, M, N>> {
    typedef T type;
};
#endif

template <typename T>
struct type_of_array<T*> {
    typedef typename type_of_array<T>::type type;
};

template <typename T, std::size_t N>
struct type_of_array<T[N]> {
    typedef typename type_of_array<T>::type type;
};

// check if the type is a container ( only vector supported for now )
template <typename>
struct is_container {
    static const bool value = false;
};

template <typename T>
struct is_container<std::vector<T> > {
    static const bool value = true;
};

// check if the type is a basic C-Array
// check if the type is a container ( only vector supported for now )
template <typename>
struct is_c_array {
    static const bool value = false;
};

template <typename T>
struct is_c_array<T*> {
    static const bool value = true;
};

template <typename T, std::size_t N>
struct is_c_array<T[N]> {
    static const bool value = true;
};



// converter function for hsize_t -> size_t when hsize_t != size_t
template<typename Size>
inline std::vector<std::size_t> to_vector_size_t(std::vector<Size> vec){
    static_assert(std::is_same<Size, std::size_t>::value == false, " hsize_t != size_t mandatory here");
    std::vector<size_t> res(vec.size());
    std::transform(vec.begin(), vec.end(), res.begin(), [](Size e) { return static_cast<size_t>(e); });
    return res;
}

// converter function for hsize_t -> size_t when size_t == hsize_t
inline std::vector<std::size_t> to_vector_size_t(std::vector<std::size_t> vec){
    return vec;
}

} // end details
}

#endif // H5UTILS_HPP

namespace HighFive {

///
/// \brief Basic HighFive Exception class
///
///
class Exception : public std::exception {
  public:
    Exception(const std::string& err_msg)
        : _errmsg(err_msg), _next(), _err_major(0), _err_minor(0) {}

    virtual ~Exception() throw() {}

    ///
    /// \brief get the current exception error message
    /// \return
    ///
    inline const char* what() const throw() override { return _errmsg.c_str(); }

    ///
    /// \brief define the error message
    /// \param errmsg
    ///
    inline virtual void setErrorMsg(const std::string& errmsg) {
        _errmsg = errmsg;
    }

    ///
    /// \brief nextException
    /// \return pointer to the next exception in the chain, or NULL if not
    /// existing
    ///
    inline Exception* nextException() const { return _next.get(); }

    ///
    /// \brief HDF5 library error mapper
    /// \return HDF5 major error number
    ///
    inline hid_t getErrMajor() const { return _err_major; }

    ///
    /// \brief HDF5 library error mapper
    /// \return HDF5 minor error number
    ///
    inline hid_t getErrMinor() const { return _err_minor; }

  protected:
    std::string _errmsg;
    std::shared_ptr<Exception> _next;
    hid_t _err_major, _err_minor;

    friend struct HDF5ErrMapper;
};

///
/// \brief Exception specific to HighFive Object interface
///
class ObjectException : public Exception {
  public:
    ObjectException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive DataType interface
///
class DataTypeException : public Exception {
  public:
    DataTypeException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive File interface
///
class FileException : public Exception {
  public:
    FileException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive DataSpace interface
///
class DataSpaceException : public Exception {
  public:
    DataSpaceException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive Attribute interface
///
class AttributeException : public Exception {
  public:
    AttributeException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive DataSet interface
///
class DataSetException : public Exception {
  public:
    DataSetException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive Group interface
///
class GroupException : public Exception {
  public:
    GroupException(const std::string& err_msg) : Exception(err_msg) {}
};

///
/// \brief Exception specific to HighFive Property interface
///
class PropertyException : public Exception {
  public:
    PropertyException(const std::string& err_msg) : Exception(err_msg) {}
};
}

//#include "bits/H5Exception_misc.hpp"
///H5Exception_misc///
//////////////////////

#ifndef H5EXCEPTION_MISC_HPP
#define H5EXCEPTION_MISC_HPP

#include <cstdlib>
#include <sstream>

#include <H5Epublic.h>

namespace HighFive {

struct HDF5ErrMapper {

    template <typename ExceptionType>
    static inline herr_t stackWalk(unsigned n, const H5E_error2_t* err_desc,
                                   void* client_data) {
        auto** e_iter = static_cast<ExceptionType**>(client_data);
        (void)n;

        const char* major_err = H5Eget_major(err_desc->maj_num);
        const char* minor_err = H5Eget_minor(err_desc->min_num);

        std::ostringstream oss;
        oss << '(' << major_err << ") " << minor_err;

        auto* e = new ExceptionType(oss.str());
        e->_err_major = err_desc->maj_num;
        e->_err_minor = err_desc->min_num;
        (*e_iter)->_next.reset(e);
        *e_iter = e;
        return 0;
    }

    template <typename ExceptionType>
    [[noreturn]] static inline void ToException(const std::string& prefix_msg) {

        hid_t err_stack = H5Eget_current_stack();
        if (err_stack >= 0) {
            ExceptionType e("");
            ExceptionType* e_iter = &e;

            H5Ewalk2(err_stack, H5E_WALK_UPWARD,
                     &HDF5ErrMapper::stackWalk<ExceptionType>, &e_iter);
            H5Eclear2(err_stack);

            const char* next_err_msg = (e.nextException() != NULL)
                                           ? (e.nextException()->what())
                                           : ("");

            e.setErrorMsg(prefix_msg + " " + next_err_msg);
            throw e;
        }
        // throw generic error, unrecognized error
        throw ExceptionType(prefix_msg + ": Unknown HDF5 error");
    }
};
}

#endif // H5OBJECT_MISC_HPP

#endif // H5EXCEPTION_HPP

//#include "H5Object.hpp"
///H5Object///
//////////////

#ifndef H5OBJECT_HPP
#define H5OBJECT_HPP

#include <ctime>

#include <H5Ipublic.h>
#include <H5Opublic.h>

//#include "H5Exception.hpp" -Already included

namespace HighFive {


template <typename Derivate>
class NodeTraits;

template <typename Derivate>
class AnnotateTraits;

class ObjectInfo;


///
/// \brief Enum of the types of objects (H5O api)
///
enum class ObjectType {
    File,
    Group,
    UserDataType,
    DataSpace,
    Dataset,
    Attribute,
    Other  // Internal/custom object type
};


class Object {
  public:
    // decrease reference counter
    ~Object();

    ///
    /// \brief isValid
    /// \return true if current Object is a valid HDF5Object
    ///
    bool isValid() const;

    ///
    /// \brief getId
    /// \return internal HDF5 id to the object
    ///  provided for C API compatibility
    ///
    hid_t getId() const;

    ///
    /// \brief Retrieve several infos about the current object (address, dates, etc)
    ///
    ObjectInfo getInfo() const;

    ///
    /// \brief Gets the fundamental type of the object (dataset, group, etc)
    /// \exception ObjectException when the _hid is negative or the type
    ///     is custom and not registered yet
    ///
    ObjectType getType() const;

  protected:
    // empty constructor
    Object();

    // copy constructor, increase reference counter
    Object(const Object& other);

    Object& operator=(const Object& other);

    hid_t _hid;

  private:
    // Init with an low-level object id
    explicit Object(hid_t);

    template <typename Derivate>
    friend class NodeTraits;
    template <typename Derivate>
    friend class AnnotateTraits;
};


///
/// \brief A class for accessing hdf5 objects info
///
class ObjectInfo  {
  public:
    /// \brief Retrieve the address of the object (within its file)
    haddr_t getAddress() const noexcept;

    /// \brief Retrieve the number of references to this object
    size_t getRefCount() const noexcept;

    /// \brief Retrieve the object's creation time
    time_t getCreationTime() const noexcept;

    /// \brief Retrieve the object's last modification time
    time_t getModificationTime() const noexcept;

  protected:
    H5O_info_t raw_info;

    friend class Object;
};

}  // namespace HighFive

//#include "bits/H5Object_misc.hpp"
///H5Object_misc///
///////////////////

#ifndef H5OBJECT_MISC_HPP
#define H5OBJECT_MISC_HPP

#include <iostream>

namespace HighFive {

inline Object::Object() : _hid(H5I_INVALID_HID) {}

inline Object::Object(hid_t hid) : _hid(hid) {}

inline Object::Object(const Object& other) : _hid(other._hid) {
    if (other.isValid() && H5Iinc_ref(_hid) < 0) {
        throw ObjectException("Reference counter increase failure");
    }
}

inline Object& Object::operator=(const Object& other) {
    if (this != &other) {
        if (_hid != H5I_INVALID_HID)
            H5Idec_ref(_hid);

        _hid = other._hid;
        if (other.isValid() && H5Iinc_ref(_hid) < 0) {
            throw ObjectException("Reference counter increase failure");
        }
    }
    return *this;
}

inline Object::~Object() {
    if (isValid() && H5Idec_ref(_hid) < 0) {
        std::cerr << "HighFive::~Object: reference counter decrease failure"
                  << std::endl;
    }
}

inline bool Object::isValid() const {
    return (_hid != H5I_INVALID_HID) && (H5Iis_valid(_hid) != false);
}

inline hid_t Object::getId() const { return _hid; }

static inline ObjectType _convert_object_type(const H5I_type_t& h5type) {
    switch (h5type) {
        case H5I_FILE:
            return ObjectType::File;
        case H5I_GROUP:
            return ObjectType::Group;
        case H5I_DATATYPE:
            return ObjectType::UserDataType;
        case H5I_DATASPACE:
            return ObjectType::DataSpace;
        case H5I_DATASET:
            return ObjectType::Dataset;
        case H5I_ATTR:
            return ObjectType::Attribute;
        default:
            return ObjectType::Other;
    }
}

inline ObjectType Object::getType() const {
    // H5Iget_type is a very lightweight func which extracts the type from the id
    H5I_type_t h5type;
    if ((h5type = H5Iget_type(_hid)) == H5I_BADID) {
        HDF5ErrMapper::ToException<ObjectException>("Invalid hid or object type");
    }
    return _convert_object_type(h5type);
}

inline ObjectInfo Object::getInfo() const {
    ObjectInfo info;
    if (H5Oget_info(_hid, &info.raw_info) < 0) {
        HDF5ErrMapper::ToException<ObjectException>("Unable to obtain info for object");
    }
    return info;
}

inline haddr_t ObjectInfo::getAddress() const noexcept {
    return raw_info.addr;
}
inline size_t ObjectInfo::getRefCount() const noexcept {
    return raw_info.rc;
}
inline time_t ObjectInfo::getCreationTime() const noexcept {
    return raw_info.btime;
}
inline time_t ObjectInfo::getModificationTime() const noexcept {
    return raw_info.mtime;
}



}  // namespace

#endif // H5OBJECT_MISC_HPP


#endif // H5OBJECT_HPP

namespace HighFive {

///
/// \brief Types of property lists
///
enum class PropertyType : int {
    OBJECT_CREATE,
    FILE_CREATE,
    FILE_ACCESS,
    DATASET_CREATE,
    DATASET_ACCESS,
    DATASET_XFER,
    GROUP_CREATE,
    GROUP_ACCESS,
    DATATYPE_CREATE,
    DATATYPE_ACCESS,
    STRING_CREATE,
    ATTRIBUTE_CREATE,
    OBJECT_COPY,
    LINK_CREATE,
    LINK_ACCESS,
};

///
/// \brief Base HDF5 property List
///
template <PropertyType T>
class PropertyList {
  public:
    ~PropertyList();

    PropertyList(const PropertyList<T>&) = delete;
    PropertyList& operator=(const PropertyList<T>&) = delete;
    PropertyList(PropertyList&& other);
    PropertyList& operator=(PropertyList&& other);
    constexpr PropertyType getType() const { return T; }

    hid_t getId() const { return _hid; }

    PropertyList();

    template <typename P>
    PropertyList(std::initializer_list<P>);

    ///
    /// Add a property to this property list.
    /// A property is an object which is expected to have a method with the
    /// following signature void apply(hid_t hid) const
    ///
    template <typename P>
    void add(const P& property);

  protected:
    void _initializeIfNeeded();

    hid_t _hid;
};

typedef PropertyList<PropertyType::FILE_CREATE> FileCreateProps;
typedef PropertyList<PropertyType::FILE_ACCESS> FileAccessProps ;
typedef PropertyList<PropertyType::DATASET_CREATE> DataSetCreateProps;
typedef PropertyList<PropertyType::DATASET_ACCESS> DataSetAccessProps;
typedef PropertyList<PropertyType::DATASET_XFER> DataTransferProps;

///
/// RawPropertieLists are to be used when advanced H5 properties
/// are desired and are not part of the HighFive API.
/// Therefore this class is mainly for internal use.
template <PropertyType T>
class RawPropertyList : public PropertyList<T> {
  public:
    template <typename F, typename... Args>
    void add(const F& funct, const Args&... args);
};


class Chunking {
  public:
    Chunking(const std::vector<hsize_t>& dims)
        : _dims(dims) {}

    Chunking(std::initializer_list<hsize_t> items)
        : Chunking(std::vector<hsize_t>{items}) {}

    template <typename... Args>
    Chunking(hsize_t item, Args... args)
        : Chunking(std::vector<hsize_t>{item, static_cast<hsize_t>(args)...}) {}

    const std::vector<hsize_t>& getDimensions() const { return _dims; }

  private:
    friend DataSetCreateProps;
    void apply(hid_t hid) const;
    const std::vector<hsize_t> _dims;
};

class Deflate {
  public:
    Deflate(unsigned level)
        : _level(level) {}

  private:
    friend DataSetCreateProps;
    void apply(hid_t hid) const;
    const unsigned _level;
};

class Shuffle {
  public:
    Shuffle() {}

  private:
    friend DataSetCreateProps;
    void apply(hid_t hid) const;
};

/// Dataset access property to control chunk cache configuration.
/// Do not confuse with the similar file access property for H5Pset_cache
class Caching {
  public:
    /// https://support.hdfgroup.org/HDF5/doc/RM/H5P/H5Pset_chunk_cache.html for
    /// details.
    Caching(const size_t numSlots,
            const size_t cacheSize,
            const double w0 = static_cast<double>(H5D_CHUNK_CACHE_W0_DEFAULT))
        : _numSlots(numSlots)
        , _cacheSize(cacheSize)
        , _w0(w0) {}

  private:
    friend DataSetAccessProps;
    void apply(hid_t hid) const;
    const size_t _numSlots;
    const size_t _cacheSize;
    const double _w0;
};

}  // namespace HighFive

//#include "bits/H5PropertyList_misc.hpp"
///H5PropertyList_misc///
/////////////////////////

#ifndef H5PROPERTY_LIST_MISC_HPP
#define H5PROPERTY_LIST_MISC_HPP

#include <H5Ppublic.h>

namespace HighFive {

namespace {
inline hid_t convert_plist_type(PropertyType propertyType) {
    // The HP5_XXX are macros with function calls so we can't assign
    // them as the enum values
    switch (propertyType) {
    case PropertyType::OBJECT_CREATE:
        return H5P_OBJECT_CREATE;
    case PropertyType::FILE_CREATE:
        return H5P_FILE_CREATE;
    case PropertyType::FILE_ACCESS:
        return H5P_FILE_ACCESS;
    case PropertyType::DATASET_CREATE:
        return H5P_DATASET_CREATE;
    case PropertyType::DATASET_ACCESS:
        return H5P_DATASET_ACCESS;
    case PropertyType::DATASET_XFER:
        return H5P_DATASET_XFER;
    case PropertyType::GROUP_CREATE:
        return H5P_GROUP_CREATE;
    case PropertyType::GROUP_ACCESS:
        return H5P_GROUP_ACCESS;
    case PropertyType::DATATYPE_CREATE:
        return H5P_DATATYPE_CREATE;
    case PropertyType::DATATYPE_ACCESS:
        return H5P_DATATYPE_ACCESS;
    case PropertyType::STRING_CREATE:
        return H5P_STRING_CREATE;
    case PropertyType::ATTRIBUTE_CREATE:
        return H5P_ATTRIBUTE_CREATE;
    case PropertyType::OBJECT_COPY:
        return H5P_OBJECT_COPY;
    case PropertyType::LINK_CREATE:
        return H5P_LINK_CREATE;
    case PropertyType::LINK_ACCESS:
        return H5P_LINK_ACCESS;
    default:
        HDF5ErrMapper::ToException<PropertyException>(
            "Unsupported property list type");
    }
}

}  // namespace

template <PropertyType T>
inline PropertyList<T>::PropertyList()
    : _hid(H5P_DEFAULT) {}

template <PropertyType T>
inline PropertyList<T>::PropertyList(PropertyList<T>&& other)
    : _hid(other._hid) {
    other._hid = H5P_DEFAULT;
}

template <PropertyType T>
inline PropertyList<T>& PropertyList<T>::operator=(PropertyList<T>&& other) {
    // This code handles self-assignment without ifs
    const auto hid = other._hid;
    other._hid = H5P_DEFAULT;
    _hid = hid;
    return *this;
}

template <PropertyType T>
inline PropertyList<T>::~PropertyList() {
    // H5P_DEFAULT and H5I_INVALID_HID are not the same Ensuring that ~Object
    if (_hid != H5P_DEFAULT) {
        H5Pclose(_hid);
    }
}

template <PropertyType T>
inline void PropertyList<T>::_initializeIfNeeded() {
    if (_hid != H5P_DEFAULT) {
        return;
    }
    if ((_hid = H5Pcreate(convert_plist_type(T))) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Unable to create property list");
    }
}

template <PropertyType T>
template <typename P>
inline void PropertyList<T>::add(const P& property) {
    _initializeIfNeeded();
    property.apply(_hid);
}

template <PropertyType T>
template <typename F, typename... Args>
inline void RawPropertyList<T>::add(const F& funct, const Args&... args) {
    this->_initializeIfNeeded();
    if (funct(this->_hid, args...) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting raw hdf5 property.");
    }
}

inline void Chunking::apply(const hid_t hid) const {
    if (H5Pset_chunk(hid, static_cast<int>(_dims.size()), _dims.data()) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting chunk property");
    }
}

inline void Deflate::apply(const hid_t hid) const {
    if (!H5Zfilter_avail(H5Z_FILTER_DEFLATE)) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting deflate property");
    }

    if (H5Pset_deflate(hid, _level) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting deflate property");
    }
}

inline void Shuffle::apply(const hid_t hid) const {
    if (!H5Zfilter_avail(H5Z_FILTER_SHUFFLE)) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting shuffle property");
    }

    if (H5Pset_shuffle(hid) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting shuffle property");
    }
}

inline void Caching::apply(const hid_t hid) const {
    if (H5Pset_chunk_cache(hid, _numSlots, _cacheSize, _w0) < 0) {
        HDF5ErrMapper::ToException<PropertyException>(
            "Error setting dataset cache parameters");
    }
}
}  // namespace HighFive
#endif  // H5PROPERTY_LIST_HPP

#endif  // H5PROPERTY_LIST_HPP
namespace HighFive {

///
/// \brief file driver base concept
///
class FileDriver : public FileAccessProps {};

///
/// \brief MPIIO Driver for Parallel HDF5
///
class MPIOFileDriver : public FileDriver {
  public:
    template <typename Comm, typename Info>
    inline MPIOFileDriver(Comm mpi_comm, Info mpi_info);

  private:
};

}  // namespace HighFive

//#include "bits/H5FileDriver_misc.hpp"
///H5FileDriver_misc///
///////////////////////


#ifndef H5FILEDRIVER_MISC_HPP
#define H5FILEDRIVER_MISC_HPP

#include <H5Ppublic.h>

#ifdef H5_HAVE_PARALLEL
#include <H5FDmpi.h>
#endif

namespace HighFive {

namespace {

template <typename Comm, typename Info>
class MPIOFileAccess
{
public:
  MPIOFileAccess(Comm comm, Info info)
      : _comm(comm)
      , _info(info)
  {}

  void apply(const hid_t list) const {
    if (H5Pset_fapl_mpio(list, _comm, _info) < 0) {
        HDF5ErrMapper::ToException<FileException>(
            "Unable to set-up MPIO Driver configuration");
    }
  }
private:
  Comm _comm;
  Info _info;
};

}  //namespace

template <typename Comm, typename Info>
inline MPIOFileDriver::MPIOFileDriver(Comm comm, Info info) {
    add(MPIOFileAccess<Comm, Info>(comm, info));
}

} // namespace HighFive

#endif // H5FILEDRIVER_MISC_HPP


#endif // H5FILEDRIVER_HPP

//#include "H5Object.hpp" -Already included  

//#include "bits/H5Annotate_traits.hpp"
///H5Annotate_traits///
///////////////////////


#ifndef H5ANNOTATE_TRAITS_HPP
#define H5ANNOTATE_TRAITS_HPP

#include <string>

namespace HighFive {

class Attribute;
class DataSpace;
class DataType;

template <typename Derivate>
class AnnotateTraits {
  public:
    ///
    /// \brief create a new attribute with the name attribute_name
    /// \param attribute_name identifier of the attribute
    /// \param space Associated \ref DataSpace
    /// \param type
    /// \return the attribute object
    ///
    Attribute createAttribute(const std::string& attribute_name,
                              const DataSpace& space, const DataType& type);

    ///
    /// \brief createAttribute create a new attribute on the current dataset with
    /// size specified by space
    /// \param attribute_name identifier of the attribute
    /// \param space Associated DataSpace
    /// informations
    /// \return Attribute Object
    template <typename Type>
    Attribute createAttribute(const std::string& attribute_name,
                              const DataSpace& space);

    ///
    /// \brief createAttribute create a new attribute on the current dataset and
    /// write to it, inferring the DataSpace from data.
    /// \param attribute_name identifier of the attribute
    /// \param data Associated data to write, must support DataSpace::From, see
    /// \ref DataSpace for more information
    /// \return Attribute Object
    ///
    template <typename T>
    Attribute createAttribute(const std::string& attribute_name,
                              const T& data);

    ///
    /// \brief deleteAttribute let you delete an attribute by its name.
    /// \param attribute_name identifier of the attribute
    void deleteAttribute(const std::string& attribute_name);

    ///
    /// \brief open an existing attribute with the name attribute_name
    /// \param attribute_name identifier of the attribute
    /// \return the attribute object
    Attribute getAttribute(const std::string& attribute_name) const;

    ///
    /// \brief return the number of attributes of the node / group
    /// \return number of attributes
    size_t getNumberAttributes() const;

    ///
    /// \brief list all attribute name of the node / group
    /// \return number of attributes
    std::vector<std::string> listAttributeNames() const;

    ///
    /// \brief checks an attribute exists
    /// \return number of attributes
    bool hasAttribute(const std::string& attr_name) const;

  private:
    typedef Derivate derivate_type;
};
}


#endif // H5ANNOTATE_TRAITS_HPP


//#include "bits/H5Node_traits.hpp"
///H5Node_traits///
///////////////////

#ifndef H5NODE_TRAITS_HPP
#define H5NODE_TRAITS_HPP

#include <string>

//#include "../H5PropertyList.hpp" -Already included

namespace HighFive {

class Attribute;
class DataSet;
class DataSpace;
class DataType;
class Group;


enum class LinkType;


template <typename Derivate>
class NodeTraits {
  public:
    ///
    /// \brief createDataSet Create a new dataset in the current file of
    /// datatype type and of size space
    /// \param dataset_name identifier of the dataset
    /// \param space Associated DataSpace, see \ref DataSpace for more
    /// informations
    /// \param type Type of Data
    /// \param createProps A property list with data set creation properties
    /// \param accessProps A property list with data set access properties
    /// \return DataSet Object
    DataSet
    createDataSet(const std::string& dataset_name,
                  const DataSpace& space,
                  const DataType& type,
                  const DataSetCreateProps& createProps = DataSetCreateProps(),
                  const DataSetAccessProps& accessProps = DataSetAccessProps());

    ///
    /// \brief createDataSet create a new dataset in the current file with a
    /// size specified by space
    /// \param dataset_name identifier of the dataset
    /// \param space Associated DataSpace, see \ref DataSpace for more
    /// information
    /// \param createProps A property list with data set creation properties
    /// \param accessProps A property list with data set access properties
    /// \return DataSet Object
    template <typename Type>
    DataSet
    createDataSet(const std::string& dataset_name,
                  const DataSpace& space,
                  const DataSetCreateProps& createProps = DataSetCreateProps(),
                  const DataSetAccessProps& accessProps = DataSetAccessProps());

    ///
    /// \brief createDataSet create a new dataset in the current file and
    /// write to it, inferring the DataSpace from the data.
    /// \param dataset_name identifier of the dataset
    /// \param data Associated data, must support DataSpace::From, see
    /// \ref DataSpace for more information
    /// \param createProps A property list with data set creation properties
    /// \param accessProps A property list with data set access properties
    /// \return DataSet Object
    template <typename T>
    DataSet
    createDataSet(const std::string& dataset_name,
                  const T& data,
                  const DataSetCreateProps& createProps = DataSetCreateProps(),
                  const DataSetAccessProps& accessProps = DataSetAccessProps());

    ///
    /// \brief get an existing dataset in the current file
    /// \param dataset_name
    /// \param accessProps property list to configure dataset chunk cache
    /// \return return the named dataset, or throw exception if not found
    DataSet getDataSet(
        const std::string& dataset_name,
        const DataSetAccessProps& accessProps = DataSetAccessProps()) const;

    ///
    /// \brief create a new group, and eventually intermediate groups
    /// \param group_name
    /// \param parents Whether it shall create intermediate groups if
    ///      necessary. Default: true
    /// \return the group object
    Group createGroup(const std::string& group_name, bool parents = true);

    ///
    /// \brief open an existing group with the name group_name
    /// \param group_name
    /// \return the group object
    Group getGroup(const std::string& group_name) const;

    ///
    /// \brief return the number of leaf objects of the node / group
    /// \return number of leaf objects
    size_t getNumberObjects() const;

    ///
    /// \brief return the name of the object with the given index
    /// \return the name of the object
    std::string getObjectName(size_t index) const;

    ///
    /// \brief list all leaf objects name of the node / group
    /// \return number of leaf objects
    std::vector<std::string> listObjectNames() const;

    ///
    /// \brief check a dataset or group exists in the current node / group
    /// \param node_name dataset/group name to check
    /// \return true if a dataset/group with the associated name exists, or false
    bool exist(const std::string& node_name) const;

    ///
    /// \brief Returns the kind of link of the given name (soft, hard...)
    /// \param node_name The entry to check, path relative to the current group
    LinkType getLinkType(const std::string& node_name) const;

    ///
    /// \brief A shorthand to get the kind of object pointed to (group, dataset, type...)
    /// \param node_name The entry to check, path relative to the current group
    inline ObjectType getObjectType(const std::string& node_name) const;

  private:
    typedef Derivate derivate_type;

    // A wrapper over the low-level H5Lexist
    bool _exist(const std::string& node_name) const;

    // Opens an arbitrary object to obtain info
    Object _open(const std::string& node_name,
                 const DataSetAccessProps& accessProps=DataSetAccessProps()) const;
};


///
/// \brief The possible types of group entries (link concept)
///
enum class LinkType {
    Hard,
    Soft,
    External,
    Other  // Reserved or User-defined
};


}  // namespace HighFive


#endif  // H5NODE_TRAITS_HPP
namespace HighFive {

///
/// \brief File class
///
class File : public Object,
             public NodeTraits<File>,
             public AnnotateTraits<File> {
 public:
    enum : unsigned {
        /// Open flag: Read only access
        ReadOnly = 0x00u,
        /// Open flag: Read Write access
        ReadWrite = 0x01u,
        /// Open flag: Truncate a file if already existing
        Truncate = 0x02u,
        /// Open flag: Open will fail if file already exist
        Excl = 0x04u,
        /// Open flag: Open in debug mode
        Debug = 0x08u,
        /// Open flag: Create non existing file
        Create = 0x10u,
        /// Derived open flag: common write mode (=ReadWrite|Create|Truncate)
        Overwrite = Truncate,
        /// Derived open flag: Opens RW or exclusively creates
        OpenOrCreate = ReadWrite | Create
    };

    ///
    /// \brief File
    /// \param filename: filepath of the HDF5 file
    /// \param openFlags: Open mode / flags ( ReadOnly, ReadWrite)
    /// \param fileAccessProps: the file access properties
    ///
    /// Open or create a new HDF5 file
    explicit File(const std::string& filename, unsigned openFlags = ReadOnly,
                  const FileAccessProps& fileAccessProps = FileDriver());

    ///
    /// \brief Return the name of the file
    ///
    const std::string& getName() const;

    ///
    /// \brief flush
    ///
    /// Flushes all buffers associated with a file to disk
    ///
    void flush();

 private:
    std::string _filename;
};
}  // namespace HighFive

//#include "bits/H5File_misc.hpp"
//////////////////////////////////
//H5File_misc//
///////////////

#ifndef H5FILE_MISC_HPP
#define H5FILE_MISC_HPP

#include <string>

#include <H5Fpublic.h>

//#include "../H5Utility.hpp"
///////////////////////
///H5Utility///
///////////////

#ifndef H5UTILITY_HPP
#define H5UTILITY_HPP

#include <H5Epublic.h>

namespace HighFive {

///
/// \brief Utility class to disable HDF5 stack printing inside a scope.
///
class SilenceHDF5 {
public:
    inline SilenceHDF5()
        : _client_data(0)
    {
        H5Eget_auto2(H5E_DEFAULT, &_func, &_client_data);
        H5Eset_auto2(H5E_DEFAULT, 0, 0);
    }

    inline ~SilenceHDF5() { H5Eset_auto2(H5E_DEFAULT, _func, _client_data); }
private:
    H5E_auto2_t _func;
    void* _client_data;
};
}

#endif // H5UTIL_HPP

namespace HighFive {

namespace {

// libhdf5 uses a preprocessor trick on their oflags
// we can not declare them constant without a mapper
inline unsigned convert_open_flag(unsigned openFlags) {
    unsigned res_open = 0;
    if (openFlags & File::ReadOnly)
        res_open |= H5F_ACC_RDONLY;
    if (openFlags & File::ReadWrite)
        res_open |= H5F_ACC_RDWR;
    if (openFlags & File::Create)
        res_open |= H5F_ACC_CREAT;
    if (openFlags & File::Truncate)
        res_open |= H5F_ACC_TRUNC;
    if (openFlags & File::Excl)
        res_open |= H5F_ACC_EXCL;
    return res_open;
}
}  // namespace

inline File::File(const std::string& filename, unsigned openFlags,
                  const FileAccessProps& fileAccessProps)
    : _filename(filename) {

    openFlags = convert_open_flag(openFlags);

    unsigned createMode = openFlags & (H5F_ACC_TRUNC | H5F_ACC_EXCL);
    unsigned openMode = openFlags & (H5F_ACC_RDWR | H5F_ACC_RDONLY);
    bool mustCreate = createMode > 0;
    bool openOrCreate = (openFlags & H5F_ACC_CREAT) > 0;

    // open is default. It's skipped only if flags require creation
    // If open fails it will try create() if H5F_ACC_CREAT is set
    if (!mustCreate) {
        // Silence open errors if create is allowed
        std::unique_ptr<SilenceHDF5> silencer;
        if (openOrCreate) silencer.reset(new SilenceHDF5());

        _hid = H5Fopen(_filename.c_str(), openMode, fileAccessProps.getId());

        if (isValid()) return;  // Done

        if (openOrCreate) {
            // Will attempt to create ensuring wont clobber any file
            createMode = H5F_ACC_EXCL;
        } else {
            HDF5ErrMapper::ToException<FileException>(
                std::string("Unable to open file " + _filename));
        }
    }

    if ((_hid = H5Fcreate(_filename.c_str(), createMode, H5P_DEFAULT,
                          fileAccessProps.getId())) < 0) {
        HDF5ErrMapper::ToException<FileException>(
            std::string("Unable to create file " + _filename));
    }
}

inline const std::string& File::getName() const {
    return _filename;
}

inline void File::flush() {
    if (H5Fflush(_hid, H5F_SCOPE_GLOBAL) < 0) {
        HDF5ErrMapper::ToException<FileException>(
            std::string("Unable to flush file " + _filename));
    }
}
}  // namespace HighFive

#endif  // H5FILE_MISC_HPP

//#include "H5Attribute.hpp"  // for AnnotateTraits
//////////////////////////////
///H5Attribute///
/////////////////

#ifndef H5ATTRIBUTE_HPP
#define H5ATTRIBUTE_HPP

#include <vector>

//#include "H5DataSpace.hpp"
///////////////////////////
///H5DataSpace///
/////////////////

#ifndef H5DATASPACE_HPP
#define H5DATASPACE_HPP

#include <vector>
#include <array>
#include <cstdint>
#include <type_traits>
#include <initializer_list>

#ifdef H5_USE_BOOST
// starting Boost 1.64, serialization header must come before ublas
#include <boost/serialization/vector.hpp>
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

//#include "H5Object.hpp" --Already Included

namespace HighFive {

class File;
class DataSet;

///
/// \brief Class representing the space (dimensions) of a dataset
///
class DataSpace : public Object {
  public:

    static const size_t UNLIMITED = SIZE_MAX;

    /// dataspace type
    enum DataspaceType {
        datascape_scalar,
        datascape_null
        // simple dataspace are handle directly from their dimensions
    };

    /// create a dataspace of N-dimensions
    /// Each dimension is configured this way
    ///  size(dim1) = vec[0]
    ///  size(dim2) = vec[1]
    ///  etc...
    explicit DataSpace(const std::vector<size_t>& dims);

    /// Make sure that DataSpace({1,2,3}) works on GCC. This is
    /// the shortcut form of the vector initializer, but one some compilers (gcc)
    /// this does not resolve correctly without this constructor.
    explicit DataSpace(std::initializer_list<size_t> items);

    /// Allow directly listing 1 or more dimensions to initialize,
    /// that is, DataSpace(1,2) means DataSpace(std::vector<size_t>{1,2}).
    template<typename... Args>
    explicit DataSpace(size_t dim1, Args... dims);

    /// Create a dataspace from an iterator pair
    ///
    /// Explicitly disable DataSpace(int_like, int_like) from trying to use this constructor
    template <typename IT, typename = typename std::enable_if<!std::is_integral<IT>::value,IT>::type>
    DataSpace(const IT begin,
              const IT end);

    /// \brief Create a resizable N-dimensional dataspace
    /// \param dims Initial size of dataspace
    /// \param maxdims Maximum size of the dataspace
    explicit DataSpace(const std::vector<size_t>& dims,
                       const std::vector<size_t>& maxdims);

    ///
    /// \brief DataSpace create a scalar dataspace or a null dataset
    ///
    explicit DataSpace(DataspaceType dtype);

    /// Create a new DataSpace
    ///  with a different id available for modifications
    DataSpace clone() const;

    ///
    /// \brief getNumberDimensions
    /// \return the number of dimensions in the current dataspace
    ///
    size_t getNumberDimensions() const;

    /// \brief getDimensions
    /// \return return a vector of N-element, each element is the size of the
    /// associated dataset dimension
    std::vector<size_t> getDimensions() const;

    /// \brief getElementCount
    /// \return the total number of elements in the dataspace
    size_t getElementCount() const;

    /// \brief getMaxDimensions
    /// \return return a vector of N-element, each element is the size of the
    /// associated dataset maximum dimension
    std::vector<size_t> getMaxDimensions() const;

    /// Create a dataspace matching a single element of a basic type
    ///  supported type are integrals (int,long), floating points (float,double)
    ///  and std::string
    template <typename ScalarValue>
    static DataSpace From(const ScalarValue& scalar_value);

    /// Create a dataspace matching the container dimensions and size
    /// Supported Containers are:
    ///  - vector of fundamental types
    ///  - vector of std::string
    ///  - boost::multi_array (with H5_USE_BOOST defined)
    template <typename Value>
    static DataSpace From(const std::vector<Value>& vec);

    /// Create a dataspace matching the container dimensions for a
    /// std::array.
    template <typename Value, std::size_t N>
    static DataSpace From(const std::array<Value, N>&);


#ifdef H5_USE_BOOST
    template <typename Value, std::size_t Dims>
    static DataSpace From(const boost::multi_array<Value, Dims>& container);

    template <typename Value>
    static DataSpace From(const boost::numeric::ublas::matrix<Value>& mat);
#endif

#ifdef H5_USE_EIGEN
    template <typename Value, int M, int N>
    static DataSpace From(const Eigen::Matrix<Value, M, N>& mat);

    template <typename Value, int M, int N>
    static DataSpace From(const std::vector<Eigen::Matrix<Value, M, N>>& vec);
#ifdef H5_USE_BOOST
    template <typename Value, int M, int N, size_t Dims>
    static DataSpace From(const boost::multi_array<Eigen::Matrix<Value, M, N>, Dims>& vec);
#endif
#endif

  protected:
    explicit DataSpace();

    friend class Attribute;
    friend class File;
    friend class DataSet;
};
}

//#include "bits/H5Dataspace_misc.hpp"
////////////////////////////////
///H5Dataspace_misc///
//////////////////////

#ifndef H5DATASPACE_MISC_HPP
#define H5DATASPACE_MISC_HPP

#include <array>
#include <initializer_list>
#include <vector>
#include <numeric>

#include <H5Spublic.h>

//#include "H5Utils.hpp" --Already included

namespace HighFive {

inline DataSpace::DataSpace(const std::vector<size_t>& dims)
    : DataSpace(dims.begin(), dims.end()) {}

inline DataSpace::DataSpace(std::initializer_list<size_t> items)
    : DataSpace(std::vector<size_t>(items)) {}

template<typename... Args>
    inline DataSpace::DataSpace(size_t dim1, Args... dims)
    : DataSpace(std::vector<size_t>{static_cast<size_t>(dim1),
                                    static_cast<size_t>(dims)...}){}

template <class IT, typename>
inline DataSpace::DataSpace(const IT begin, const IT end) {
    std::vector<hsize_t> real_dims(begin, end);

    if ((_hid = H5Screate_simple(int(real_dims.size()), real_dims.data(),
                                 NULL)) < 0) {
        throw DataSpaceException("Impossible to create dataspace");
    }
}

inline DataSpace::DataSpace(const std::vector<size_t>& dims,
                            const std::vector<size_t>& maxdims) {

    if (dims.size() != maxdims.size()) {
        throw DataSpaceException("dims and maxdims must be the same length.");
    }

    std::vector<hsize_t> real_dims(dims.begin(), dims.end());
    std::vector<hsize_t> real_maxdims(maxdims.begin(), maxdims.end());

    // Replace unlimited flag with actual HDF one
    std::replace(real_maxdims.begin(), real_maxdims.end(),
                 static_cast<hsize_t>(DataSpace::UNLIMITED), H5S_UNLIMITED);

    if ((_hid = H5Screate_simple(int(dims.size()), real_dims.data(),
                                 real_maxdims.data())) < 0) {
        throw DataSpaceException("Impossible to create dataspace");
    }
} // namespace HighFive

inline DataSpace::DataSpace(DataSpace::DataspaceType dtype) {
    H5S_class_t h5_dataspace_type;
    switch (dtype) {
    case DataSpace::datascape_scalar:
        h5_dataspace_type = H5S_SCALAR;
        break;
    case DataSpace::datascape_null:
        h5_dataspace_type = H5S_NULL;
        break;
    default:
        throw DataSpaceException("Invalid dataspace type: should be "
                                 "datascape_scalar or datascape_null");
    }

    if ((_hid = H5Screate(h5_dataspace_type)) < 0) {
        throw DataSpaceException("Unable to create dataspace");
    }
}

inline DataSpace::DataSpace() {}

inline DataSpace DataSpace::clone() const {
    DataSpace res;
    if ((res._hid = H5Scopy(_hid)) < 0) {
        throw DataSpaceException("Unable to copy dataspace");
    }
    return res;
}

inline size_t DataSpace::getNumberDimensions() const {
    const int ndim = H5Sget_simple_extent_ndims(_hid);
    if (ndim < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Unable to get dataspace number of dimensions");
    }
    return size_t(ndim);
}

inline std::vector<size_t> DataSpace::getDimensions() const {
    std::vector<hsize_t> dims(getNumberDimensions());
    if (dims.size() > 0) {
        if (H5Sget_simple_extent_dims(_hid, dims.data(), NULL) < 0) {
            HDF5ErrMapper::ToException<DataSetException>(
                "Unable to get dataspace dimensions");
        }
    }
    return details::to_vector_size_t(std::move(dims));
}

inline size_t DataSpace::getElementCount() const {
    std::vector<size_t> dims = getDimensions();
    return std::accumulate(dims.begin(), dims.end(), size_t{1u},
                           std::multiplies<size_t>());
}

inline std::vector<size_t> DataSpace::getMaxDimensions() const {
    std::vector<hsize_t> maxdims(getNumberDimensions());
    if (H5Sget_simple_extent_dims(_hid, NULL, maxdims.data()) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Unable to get dataspace dimensions");
    }

    std::replace(maxdims.begin(), maxdims.end(), H5S_UNLIMITED,
                 static_cast<hsize_t>(DataSpace::UNLIMITED));
    return details::to_vector_size_t(maxdims);
}

template <typename ScalarValue>
inline DataSpace DataSpace::From(const ScalarValue& scalar) {
    (void)scalar;
    static_assert(
        (std::is_arithmetic<ScalarValue>::value ||
         std::is_enum<ScalarValue>::value ||
         std::is_same<std::string, ScalarValue>::value),
        "Only the following types are supported by DataSpace::From: \n"
        "  signed_arithmetic_types = int | long | float | double \n"
        "  unsigned_arithmetic_types = unsigned signed_arithmetic_types \n"
        "  string_types = std::string \n"
        "  all_basic_types = string_types | unsigned_arithmetic_types | "
        "signed_arithmetic_types \n "
        "  stl_container_types = std::vector<all_basic_types> "
        "  boost_container_types = "
        "boost::numeric::ublas::matrix<all_basic_types> | "
        "boost::multi_array<all_basic_types> \n"
        "  all_supported_types = all_basic_types | stl_container_types | "
        "boost_container_types"
        "  eigen_matrix_type = Eigen::Matrix<signed_arithmetic_type> | Eigen::VectorXX");
    return DataSpace(DataSpace::datascape_scalar);
}

template <typename Value>
inline DataSpace DataSpace::From(const std::vector<Value>& container) {
    return DataSpace(details::get_dim_vector<Value>(container));
}

/// Currently only supports 1D std::array
template <typename Value, std::size_t N>
inline DataSpace DataSpace::From(const std::array<Value, N>& ) {
    std::vector<size_t> dims;
    dims.push_back(N);
    return DataSpace(dims);
}

#ifdef H5_USE_BOOST
template <typename Value, std::size_t Dims>
inline DataSpace
DataSpace::From(const boost::multi_array<Value, Dims>& container) {
    std::vector<size_t> dims(container.shape(), container.shape() + Dims);
    return DataSpace(dims);
}

template <typename Value>
inline DataSpace
DataSpace::From(const boost::numeric::ublas::matrix<Value>& mat) {
    std::vector<size_t> dims(2);
    dims[0] = mat.size1();
    dims[1] = mat.size2();
    return DataSpace(dims);
}
#endif

#ifdef H5_USE_EIGEN
template <typename Value, int M, int N>
inline DataSpace
DataSpace::From(const Eigen::Matrix<Value, M, N>&  mat ) {
    std::vector<size_t> dims{static_cast<size_t>(mat.rows()), static_cast<size_t>(mat.cols())};
    return DataSpace(dims);
}

template <typename Value, int M, int N>
inline DataSpace
DataSpace::From(const std::vector<Eigen::Matrix<Value, M, N>>& vec) {
    std::vector<size_t> dims{std::accumulate(vec.begin(),
                                             vec.end(),
                                             size_t{0u},
                                             [](size_t so_far, const auto& v) {
                                                 return so_far + static_cast<size_t>(v.rows());
                                             }),
                             static_cast<size_t>(vec[0].cols())};
    return DataSpace(dims);
}

#ifdef H5_USE_BOOST
template <typename Value, int M, int N, size_t Dims>
inline DataSpace DataSpace::From(const boost::multi_array<Eigen::Matrix<Value, M, N>, Dims>& vec) {
    std::vector<size_t> dims(vec.shape(), vec.shape() + Dims);
    dims[Dims - 1] *= static_cast<size_t>(vec.origin()->rows() * vec.origin()->cols());
    return DataSpace(dims);
}
#endif

#endif

namespace details {

/// dimension checks @internal
inline bool checkDimensions(const DataSpace& mem_space, size_t input_dims) {
    size_t dataset_dims = mem_space.getNumberDimensions();
    if (input_dims == dataset_dims)
        return true;

    const std::vector<size_t> dims = mem_space.getDimensions();
    for (std::vector<size_t>::const_reverse_iterator i = dims.rbegin();
         i != --dims.rend() && *i == 1; ++i)
        --dataset_dims;

    if (input_dims == dataset_dims)
        return true;

    dataset_dims = dims.size();
    for (std::vector<size_t>::const_iterator i = dims.begin();
         i != --dims.end() && *i == 1; ++i)
        --dataset_dims;

    if (input_dims == dataset_dims)
        return true;

    // The final tests is for scalars
    return input_dims == 0 && dataset_dims == 1 && dims[dims.size() - 1] == 1;
}

} // namespace details
} // namespace HighFive

#endif // H5DATASPACE_MISC_HPP

#endif // H5DATASPACE_HPP
//#include "H5DataType.hpp"
///////////////////////////
///H5DataType///
////////////////

#ifndef H5DATATYPE_HPP
#define H5DATATYPE_HPP

//#include "H5Object.hpp" --Already included

namespace HighFive {


///
/// \brief Enum of Fundamental data classes
///
enum class DataTypeClass {
    Time,
    Integer,
    Float,
    String,
    BitField,
    Opaque,
    Compound,
    Reference,
    Enum,
    VarLen,
    Array,
    Invalid
};


///
/// \brief HDF5 Data Type
///
class DataType : public Object {
  public:
    DataType();

    bool operator==(const DataType& other) const;

    bool operator!=(const DataType& other) const;

    ///
    /// \brief Return the fundamental type.
    ///
    DataTypeClass getClass() const;

    ///
    /// \brief Returns the length (in bytes) of this type elements
    ///
    /// Notice that the size of variable length sequences may have limited applicability
    ///   given that it refers to the size of the control structure. For info see
    ///   https://support.hdfgroup.org/HDF5/doc/RM/RM_H5T.html#Datatype-GetSize
    size_t getSize() const;

    ///
    /// \brief Returns a friendly description of the type (e.g. Float32)
    ///
    std::string string() const;

  protected:
    friend class Attribute;
    friend class File;
    friend class DataSet;
};

///
/// \brief create an HDF5 DataType from a C++ type
///
///  Support only basic data type
///
template <typename T>
class AtomicType : public DataType {
  public:
    AtomicType();

    typedef T basic_type;
};
}

//#include "bits/H5DataType_misc.hpp"
///////////////////////////////////////
///H5DataType_misc///
/////////////////////

#ifndef H5DATATYPE_MISC_HPP
#define H5DATATYPE_MISC_HPP

#include <string>
#include <complex>

#include <H5Tpublic.h>


namespace HighFive {

namespace {  // unnamed
inline DataTypeClass convert_type_class(const H5T_class_t& tclass);
inline std::string type_class_string(DataTypeClass);
}


inline DataType::DataType() {}

inline DataTypeClass DataType::getClass() const {
    return convert_type_class(H5Tget_class(_hid));
}

inline size_t DataType::getSize() const {
    return H5Tget_size(_hid);
}

inline bool DataType::operator==(const DataType& other) const {
    return (H5Tequal(_hid, other._hid) > 0);
}

inline bool DataType::operator!=(const DataType& other) const {
    return !(*this == other);
}

inline std::string DataType::string() const {
    return type_class_string(getClass()) + std::to_string(getSize() * 8);
}


// char mapping
template <>
inline AtomicType<char>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_CHAR);
}

template <>
inline AtomicType<signed char>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_CHAR);
}

template <>
inline AtomicType<unsigned char>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_UCHAR);
}

// short mapping
template <>
inline AtomicType<short>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_SHORT);
}

template <>
inline AtomicType<unsigned short>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_USHORT);
}

// integer mapping
template <>
inline AtomicType<int>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_INT);
}

template <>
inline AtomicType<unsigned>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_UINT);
}

// long mapping
template <>
inline AtomicType<long>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_LONG);
}

template <>
inline AtomicType<unsigned long>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_ULONG);
}

// long long mapping
template <>
inline AtomicType<long long>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_LLONG);
}

template <>
inline AtomicType<unsigned long long>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_ULLONG);
}

// float and double mapping
template <>
inline AtomicType<float>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_FLOAT);
}

template <>
inline AtomicType<double>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_DOUBLE);
}

// boolean mapping
template <>
inline AtomicType<bool>::AtomicType() {
    _hid = H5Tcopy(H5T_NATIVE_HBOOL);
}

// std string
template <>
inline AtomicType<std::string>::AtomicType() {
    _hid = H5Tcopy(H5T_C_S1);
    if (H5Tset_size(_hid, H5T_VARIABLE) < 0) {
        HDF5ErrMapper::ToException<DataTypeException>(
            "Unable to define datatype size to variable");
    }
    // define encoding to UTF-8 by default
    H5Tset_cset(_hid, H5T_CSET_UTF8);
}

template <>
inline AtomicType<std::complex<double> >::AtomicType()
{
    static struct ComplexType : public Object
    {
        ComplexType()
        {
            _hid = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<double>));
            // h5py/numpy compatible datatype
            H5Tinsert(_hid, "r", 0, H5T_NATIVE_DOUBLE);
            H5Tinsert(_hid, "i", sizeof(double), H5T_NATIVE_DOUBLE);
        };
    } complexType;
    _hid = H5Tcopy(complexType.getId());
}


namespace {

inline DataTypeClass convert_type_class(const H5T_class_t& tclass) {
    switch(tclass) {
        case H5T_TIME:
            return DataTypeClass::Time;
        case H5T_INTEGER:
            return DataTypeClass::Integer;
        case H5T_FLOAT:
            return DataTypeClass::Float;
        case H5T_STRING:
            return DataTypeClass::String;
        case H5T_BITFIELD:
            return DataTypeClass::BitField;
        case H5T_OPAQUE:
            return DataTypeClass::Opaque;
        case H5T_COMPOUND:
            return DataTypeClass::Compound;
        case H5T_REFERENCE:
            return DataTypeClass::Reference;
        case H5T_ENUM:
            return DataTypeClass::Enum;
        case H5T_VLEN:
            return DataTypeClass::VarLen;
        case H5T_ARRAY:
            return DataTypeClass::Array;
        case H5T_NO_CLASS:
        case H5T_NCLASSES:
        default:
            return DataTypeClass::Invalid;
    }
}


inline std::string type_class_string(DataTypeClass tclass) {
    switch(tclass) {
        case DataTypeClass::Time:
            return "Time";
        case DataTypeClass::Integer:
            return "Integer";
        case DataTypeClass::Float:
            return "Float";
        case DataTypeClass::String:
            return "String";
        case DataTypeClass::BitField:
            return "BitField";
        case DataTypeClass::Opaque:
            return "Opaque";
        case DataTypeClass::Compound:
            return "Compound";
        case DataTypeClass::Reference:
            return "Reference";
        case DataTypeClass::Enum:
            return "Enum";
        case DataTypeClass::VarLen:
            return "Varlen";
        case DataTypeClass::Array:
            return "Array";
        default:
            return "(Invalid)";
    }
}

}  // namespace


}  // namespace HighFive



#endif // H5DATATYPE_MISC_HPP


#endif // H5DATATYPE_HPP
//#include "H5Object.hpp" --Already included

//#include "bits/H5Annotate_traits.hpp" --Already included

namespace HighFive {

///
/// \brief Class representing an attribute of a dataset or group
///
class Attribute : public Object {
  public:
    size_t getStorageSize() const;

    ///
    /// \brief getDataType
    /// \return return the datatype associated with this dataset
    ///
    DataType getDataType() const;

    ///
    /// \brief getSpace
    /// \return return the dataspace associated with this dataset
    ///
    DataSpace getSpace() const;

    ///
    /// \brief getMemSpace
    /// \return same than getSpace for DataSet, compatibility with Selection
    /// class
    ///
    DataSpace getMemSpace() const;

    ///
    /// Read the attribute into a buffer
    /// An exception is raised if the numbers of dimension of the buffer and of
    /// the attribute are different
    ///
    /// The array type can be a N-pointer or a N-vector ( e.g int** integer two
    /// dimensional array )
    template <typename T>
    void read(T& array) const;

    ///
    /// Write the integrality N-dimension buffer to this attribute
    /// An exception is raised if the numbers of dimension of the buffer and of
    /// the attribute are different
    ///
    /// The array type can be a N-pointer or a N-vector ( e.g int** integer two
    /// dimensional array )
    template <typename T>
    void write(const T& buffer);

  private:
    Attribute();
    template <typename Derivate>
    friend class ::HighFive::AnnotateTraits;
};
}

//#include "bits/H5Annotate_traits_misc.hpp"
///H5Annotate_traits_misc///
////////////////////////////

#ifndef H5ANNOTATE_TRAITS_MISC_HPP
#define H5ANNOTATE_TRAITS_MISC_HPP

#include <string>
#include <vector>

#include <H5Apublic.h>
#include <H5Ppublic.h>

//#include "H5Iterables_misc.hpp"
///H5Iterables_misc///
//////////////////////

#ifndef H5ITERABLE_MISC_HPP
#define H5ITERABLE_MISC_HPP

#include <exception>
#include <string>
#include <vector>

#include <H5Ipublic.h>

namespace HighFive {

namespace details {

// iterator for H5 iterate

struct HighFiveIterateData {
    inline HighFiveIterateData(std::vector<std::string>& my_names)
        : names(my_names), err(NULL) {}

    std::vector<std::string>& names;
    std::exception* err;

    inline void throwIfError() {
        if (err) {
            throw * err;
        }
    }
};

template <typename InfoType>
inline herr_t internal_high_five_iterate(hid_t id, const char* name,
                                         const InfoType* info, void* op_data) {
    (void)id;
    (void)info;

    HighFiveIterateData* data = static_cast<HighFiveIterateData*>(op_data);
    try {
        data->names.push_back(name);
        return 0;
    } catch (...) {
        data->err =
            new ObjectException("Exception during H5Iterate, abort listing");
    }
    return -1;
}

} // end details
}

#endif // H5ITERABLE_MISC_HPP
namespace HighFive {

template <typename Derivate>
inline Attribute
AnnotateTraits<Derivate>::createAttribute(const std::string& attribute_name,
                                          const DataSpace& space,
                                          const DataType& dtype) {
    Attribute attribute;
    if ((attribute._hid = H5Acreate2(
             static_cast<Derivate*>(this)->getId(), attribute_name.c_str(),
             dtype._hid, space._hid, H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            std::string("Unable to create the attribute \"") + attribute_name +
            "\":");
    }
    return attribute;
}

template <typename Derivate>
template <typename Type>
inline Attribute
AnnotateTraits<Derivate>::createAttribute(const std::string& attribute_name,
                                          const DataSpace& space) {
    return createAttribute(attribute_name, space, AtomicType<Type>());
}

template <typename Derivate>
template <typename T>
inline Attribute
AnnotateTraits<Derivate>::createAttribute(const std::string& attribute_name,
                                          const T& data) {
    Attribute att = createAttribute(
        attribute_name, DataSpace::From(data),
        AtomicType<typename details::type_of_array<T>::type>());
    att.write(data);
    return att;
}

template<typename Derivate>
inline void
AnnotateTraits<Derivate>::deleteAttribute(const std::string& attribute_name) {
    if (H5Adelete(static_cast<const Derivate*>(this)->getId(), attribute_name.c_str()) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
                std::string("Unable to delete attribute \"") + attribute_name + "\":");
    }
}

template <typename Derivate>
inline Attribute AnnotateTraits<Derivate>::getAttribute(
    const std::string& attribute_name) const {
    Attribute attribute;
    if ((attribute._hid = H5Aopen(static_cast<const Derivate*>(this)->getId(),
                                  attribute_name.c_str(), H5P_DEFAULT)) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            std::string("Unable to open the attribute \"") + attribute_name +
            "\":");
    }
    return attribute;
}

template <typename Derivate>
inline size_t AnnotateTraits<Derivate>::getNumberAttributes() const {
    int res = H5Aget_num_attrs(static_cast<const Derivate*>(this)->getId());
    if (res < 0) {
        HDF5ErrMapper::ToException<AttributeException>(std::string(
            "Unable to count attributes in existing group or file"));
    }
    return static_cast<size_t>(res);
}

template <typename Derivate>
inline std::vector<std::string>
AnnotateTraits<Derivate>::listAttributeNames() const {

    std::vector<std::string> names;
    details::HighFiveIterateData iterateData(names);

    size_t num_objs = getNumberAttributes();
    names.reserve(num_objs);

    if (H5Aiterate2(static_cast<const Derivate*>(this)->getId(), H5_INDEX_NAME,
                    H5_ITER_INC, NULL,
                    &details::internal_high_five_iterate<H5A_info_t>,
                    static_cast<void*>(&iterateData)) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            std::string("Unable to list attributes in group"));
    }

    return names;
}

template <typename Derivate>
inline bool
AnnotateTraits<Derivate>::hasAttribute(const std::string& attr_name) const {
    int res = H5Aexists(static_cast<const Derivate*>(this)->getId(),
                        attr_name.c_str());
    if (res < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            std::string("Unable to check for attribute in group"));
    }
    return res;
}
}

#endif // H5ANNOTATE_TRAITS_MISC_HPP

//#include "bits/H5Attribute_misc.hpp"
///H5Attribute_misc///
//////////////////////

#ifndef H5ATTRIBUTE_MISC_HPP
#define H5ATTRIBUTE_MISC_HPP

#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>

#ifdef H5_USE_BOOST
#include <boost/multi_array.hpp>
#endif

#include <H5Apublic.h>
#include <H5Ppublic.h>

//#include "H5Converter_misc.hpp"
///H5Converter_misc///
//////////////////////


#ifndef H5CONVERTER_MISC_HPP
#define H5CONVERTER_MISC_HPP

#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>
#include <array>

#ifdef H5_USE_BOOST
// starting Boost 1.64, serialization header must come before ublas
#include <boost/serialization/vector.hpp>
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

#include <H5Dpublic.h>
#include <H5Ppublic.h>

//#include "H5Utils.hpp" --Already included

namespace HighFive {

namespace details {

inline bool is_1D(const std::vector<size_t>& dims)
{
    return std::count_if(dims.begin(), dims.end(),
                         [](size_t i){ return i > 1; }) < 2;
}

inline size_t compute_total_size(const std::vector<size_t>& dims)
{
    return std::accumulate(dims.begin(), dims.end(), size_t{1u},
                           std::multiplies<size_t>());
}

inline void check_dimensions_vector(size_t size_vec, size_t size_dataset,
                                    size_t dimension) {
    if (size_vec != size_dataset) {
        std::ostringstream ss;
        ss << "Mismatch between vector size (" << size_vec
           << ") and dataset size (" << size_dataset;
        ss << ") on dimension " << dimension;
        throw DataSetException(ss.str());
    }
}

// copy multi dimensional vector in C++ in one C-style multi dimensional buffer
template <typename T>
inline void vectors_to_single_buffer(const std::vector<T>& vec_single_dim,
                                     const std::vector<size_t>& dims,
                                     const size_t current_dim,
                                     std::vector<T>& buffer) {

    check_dimensions_vector(vec_single_dim.size(), dims[current_dim], current_dim);
    buffer.insert(buffer.end(), vec_single_dim.begin(), vec_single_dim.end());
}

template <typename T, typename U = typename type_of_array<T>::type>
inline void
vectors_to_single_buffer(const std::vector<T>& vec_multi_dim,
                         const std::vector<size_t>& dims,
                         size_t current_dim,
                         std::vector<U>& buffer) {

    check_dimensions_vector(vec_multi_dim.size(), dims[current_dim], current_dim);
    for (const auto& it : vec_multi_dim) {
        vectors_to_single_buffer(it, dims, current_dim + 1, buffer);
    }
}

// copy single buffer to multi dimensional vector, following dimensions
// specified
template <typename T>
inline typename std::vector<T>::iterator
single_buffer_to_vectors(typename std::vector<T>::iterator begin_buffer,
                         typename std::vector<T>::iterator end_buffer,
                         const std::vector<size_t>& dims,
                         const size_t current_dim,
                         std::vector<T>& vec_single_dim) {
    const auto n_elems = static_cast<long>(dims[current_dim]);
    const auto end_copy_iter = std::min(begin_buffer + n_elems, end_buffer);
    vec_single_dim.assign(begin_buffer, end_copy_iter);
    return end_copy_iter;
}

template <typename T, typename U = typename type_of_array<T>::type>
inline typename std::vector<U>::iterator
single_buffer_to_vectors(typename std::vector<U>::iterator begin_buffer,
                         typename std::vector<U>::iterator end_buffer,
                         const std::vector<size_t>& dims,
                         const size_t current_dim,
                         std::vector<std::vector<T>>& vec_multi_dim) {
    const size_t n_elems = dims[current_dim];
    vec_multi_dim.resize(n_elems);

    for (auto& subvec : vec_multi_dim) {
        begin_buffer = single_buffer_to_vectors(
            begin_buffer, end_buffer, dims, current_dim + 1, subvec);
    }
    return begin_buffer;
}

// apply conversion operations to basic scalar type
template <typename Scalar, class Enable = void>
struct data_converter {
    inline data_converter(Scalar&, DataSpace&) {

        static_assert((std::is_arithmetic<Scalar>::value ||
                       std::is_enum<Scalar>::value ||
                       std::is_same<std::string, Scalar>::value),
                      "supported datatype should be an arithmetic value, a "
                      "std::string or a container/array");
    }

    inline Scalar* transform_read(Scalar& datamem) { return &datamem; }

    inline Scalar* transform_write(Scalar& datamem) { return &datamem; }

    inline void process_result(Scalar&) {}
};

// apply conversion operations to the incoming data
// if they are a cstyle array
template <typename CArray>
struct data_converter<CArray,
                      typename std::enable_if<(is_c_array<CArray>::value)>::type> {
    inline data_converter(CArray&, DataSpace&) {}

    inline CArray& transform_read(CArray& datamem) { return datamem; }

    inline CArray& transform_write(CArray& datamem) { return datamem; }

    inline void process_result(CArray&) {}
};

// apply conversion for vectors 1D
template <typename T>
struct data_converter<
    std::vector<T>,
    typename std::enable_if<(
        std::is_same<T, typename type_of_array<T>::type>::value)>::type> {
    inline data_converter(std::vector<T>&, DataSpace& space)
        : _space(space) {
        assert(is_1D(_space.getDimensions()));
    }

    inline typename type_of_array<T>::type*
    transform_read(std::vector<T>& vec) {
        vec.resize(compute_total_size(_space.getDimensions()));
        return vec.data();
    }

    inline typename type_of_array<T>::type*
    transform_write(std::vector<T>& vec) {
        return vec.data();
    }

    inline void process_result(std::vector<T>&) {}

    DataSpace& _space;
};

// apply conversion to std::array
template <typename T, std::size_t S>
struct data_converter<
    std::array<T, S>,
    typename std::enable_if<(
        std::is_same<T, typename type_of_array<T>::type>::value)>::type> {
    inline data_converter(std::array<T, S>&, DataSpace& space) {
        const std::vector<size_t> dims = space.getDimensions();
        if (!is_1D(dims)) {
            throw DataSpaceException("Only 1D std::array supported currently.");
        }
        if (compute_total_size(dims) != S) {
            std::ostringstream ss;
            ss << "Impossible to pair DataSet with " << compute_total_size(dims)
               << " elements into an array with " << S << " elements.";
            throw DataSpaceException(ss.str());
        }
    }

    inline typename type_of_array<T>::type*
    transform_read(std::array<T, S>& vec) {
        return vec.data();
    }

    inline typename type_of_array<T>::type*
    transform_write(std::array<T, S>& vec) {
        return vec.data();
    }

    inline void process_result(std::array<T, S>&) {}
};

#ifdef H5_USE_BOOST
// apply conversion to boost multi array
template <typename T, std::size_t Dims>
struct data_converter<boost::multi_array<T, Dims>, void> {

    typedef typename boost::multi_array<T, Dims> MultiArray;

    inline data_converter(MultiArray&, DataSpace& space)
        : _dims(space.getDimensions()) {
        assert(_dims.size() == Dims);
    }

    inline typename type_of_array<T>::type* transform_read(MultiArray& array) {
        if (std::equal(_dims.begin(), _dims.end(), array.shape()) == false) {
            boost::array<typename MultiArray::index, Dims> ext;
            std::copy(_dims.begin(), _dims.end(), ext.begin());
            array.resize(ext);
        }
        return array.data();
    }

    inline typename type_of_array<T>::type* transform_write(MultiArray& array) {
        return array.data();
    }

    inline void process_result(MultiArray&) {}

    std::vector<size_t> _dims;
};

// apply conversion to boost matrix ublas
template <typename T>
struct data_converter<boost::numeric::ublas::matrix<T>, void> {

    typedef typename boost::numeric::ublas::matrix<T> Matrix;

    inline data_converter(Matrix&, DataSpace& space)
        : _dims(space.getDimensions()) {
        assert(_dims.size() == 2);
    }

    inline typename type_of_array<T>::type* transform_read(Matrix& array) {
        boost::array<std::size_t, 2> sizes = {{array.size1(), array.size2()}};

        if (std::equal(_dims.begin(), _dims.end(), sizes.begin()) == false) {
            array.resize(_dims[0], _dims[1], false);
            array(0, 0) = 0; // force initialization
        }

        return &(array(0, 0));
    }

    inline typename type_of_array<T>::type* transform_write(Matrix& array) {
        return &(array(0, 0));
    }

    inline void process_result(Matrix&) {}

    std::vector<size_t> _dims;
};
#endif

#ifdef H5_USE_EIGEN
//compute size for single Eigen Matrix
template <typename T, int M, int N>
inline size_t compute_total_size(const Eigen::Matrix<T,M,N>& matrix)
{
    return matrix.rows() * matrix.cols();
}

//compute size for  std::vector of Eigens
template <typename T, int M, int N>
inline size_t compute_total_size(const std::vector<Eigen::Matrix<T,M,N>>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), size_t{0u}, [](size_t so_far, const auto& v) {
        return so_far + static_cast<size_t>(v.rows()) * static_cast<size_t>(v.cols());
    });
}

#ifdef H5_USE_BOOST
// compute size for  boost::multi_array of Eigens
template <typename T, size_t Dims>
inline size_t compute_total_size(const boost::multi_array<T, Dims>& vec) {
    return std::accumulate(vec.origin(),
                           vec.origin() + vec.num_elements(),
                           size_t{0u},
                           [](size_t so_far, const auto& v) {
                               return so_far +
                                      static_cast<size_t>(v.rows()) * static_cast<size_t>(v.cols());
                           });
}
#endif

//compute total row size for std::vector of Eigens
template <typename T, int M, int N>
inline size_t compute_total_row_size(const std::vector<Eigen::Matrix<T,M,N>>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), size_t{0u}, [](size_t so_far, const auto& v) {
        return so_far + static_cast<size_t>(v.rows());
    });
}


// apply conversion to eigen matrix
template <typename T, int M, int N>
struct data_converter<Eigen::Matrix<T, M, N>, void> {

    typedef typename Eigen::Matrix<T, M, N> MatrixTMN;

    inline data_converter(MatrixTMN&, DataSpace& space)
        : _dims(space.getDimensions()) {
        assert(_dims.size() == 2);
    }

    inline typename type_of_array<T>::type* transform_read(MatrixTMN& array) {
        if (_dims[0] != static_cast<size_t>(array.rows()) ||
            _dims[1] != static_cast<size_t>(array.cols())) {
            array.resize(static_cast<Eigen::Index>(_dims[0]), static_cast<Eigen::Index>(_dims[1]));
        }
        return array.data();
    }

    inline typename type_of_array<T>::type* transform_write(MatrixTMN& array) {
        return array.data();
    }

    inline void process_result(MatrixTMN&) {}

    std::vector<size_t> _dims;
};

template <typename T, int M, int N>
inline void vectors_to_single_buffer(const std::vector<Eigen::Matrix<T,M,N>>& vec,
                                     const std::vector<size_t>& dims,
                                     const size_t current_dim,
                                     std::vector<T>& buffer) {

    check_dimensions_vector(compute_total_row_size(vec), dims[current_dim], current_dim);
    for (const auto& k : vec) {
        std::copy(k.data(), k.data()+k.size(), std::back_inserter(buffer));
    }
}

// apply conversion to std::vector of eigen matrix
template <typename T, int M, int N>
struct data_converter<std::vector<Eigen::Matrix<T,M,N>>, void> {

    typedef typename Eigen::Matrix<T, M, N> MatrixTMN;

    inline data_converter(const std::vector<MatrixTMN>& , DataSpace& space)
        : _dims(space.getDimensions()), _space(space) {
        assert(_dims.size() == 2);
    }

    inline typename type_of_array<T>::type*
    transform_read(std::vector<MatrixTMN>& /* vec */) {
        _vec_align.resize(compute_total_size(_space.getDimensions()));
        return _vec_align.data();
    }

    inline typename type_of_array<T>::type*
    transform_write(std::vector<MatrixTMN>& vec) {
        _vec_align.reserve(compute_total_size(vec));
        vectors_to_single_buffer<T, M, N>(vec, _dims, 0, _vec_align);
        return _vec_align.data();
    }

    inline void process_result(std::vector<MatrixTMN>& vec) {
        T* start = _vec_align.data();
        if(vec.size() > 0) {
            for(auto& v : vec){
                v = Eigen::Map<MatrixTMN>(start, v.rows(), v.cols());
                start += v.rows()*v.cols();
            }
        }
        else {
            if(M == -1 || N == -1) {
                std::ostringstream ss;
                ss << "Dynamic size(-1) used without pre-defined vector data layout.\n"
                   << "Initiliaze vector elements using Zero, i.e.:\n"
                   << "\t vector<MatrixXd> vec(5, MatrixXd::Zero(20,5))";
                throw DataSetException(ss.str());
            }
            else
            {
                for (size_t i = 0; i < _dims[0] / static_cast<size_t>(M); ++i) {
                    vec.emplace_back(Eigen::Map<MatrixTMN>(start, M, N));
                    start += M * N;
                }
            }
        }
    }

    std::vector<size_t> _dims;
    std::vector<typename type_of_array<T>::type> _vec_align;
    DataSpace& _space;
};

#ifdef H5_USE_BOOST
template <typename T, int M, int N, std::size_t Dims>
struct data_converter<boost::multi_array<Eigen::Matrix<T, M, N>, Dims>, void> {
    typedef typename boost::multi_array<Eigen::Matrix<T, M, N>, Dims> MultiArrayEigen;

    inline data_converter(const MultiArrayEigen&, DataSpace& space)
        : _dims(space.getDimensions())
        , _space(space) {
        assert(_dims.size() == Dims);
    }

    inline typename type_of_array<T>::type*
    transform_read(const MultiArrayEigen& /*array*/) {
        _vec_align.resize(compute_total_size(_space.getDimensions()));
        return _vec_align.data();
    }

    inline typename type_of_array<T>::type* transform_write(MultiArrayEigen& array) {
        _vec_align.reserve(compute_total_size(array));
        for (auto e = array.origin(); e < array.origin() + array.num_elements(); ++e) {
            std::copy(e->data(), e->data() + e->size(), std::back_inserter(_vec_align));
        }
        return _vec_align.data();
    }

    inline void process_result(MultiArrayEigen& vec) {
        T* start = _vec_align.data();
        if (M != -1 && N != -1) {
            for (auto v = vec.origin(); v < vec.origin() + vec.num_elements(); ++v) {
                *v = Eigen::Map<Eigen::Matrix<T, M, N>>(start, v->rows(), v->cols());
                start += v->rows() * v->cols();
            }
        } else {
            if (vec.origin()->rows() > 0 && vec.origin()->cols() > 0) {
                const auto VEC_M = vec.origin()->rows(), VEC_N = vec.origin()->cols();
                for (auto v = vec.origin(); v < vec.origin() + vec.num_elements(); ++v) {
                    assert(v->rows() == VEC_M && v->cols() == VEC_N);
                    *v = Eigen::Map<Eigen::Matrix<T, M, N>>(start, VEC_M, VEC_N);
                    start += VEC_M * VEC_N;
                }
            } else {
                throw DataSetException(
                    "Dynamic size(-1) used without pre-defined multi_array data layout.\n"
                    "Initialize vector elements using  MatrixXd::Zero");
            }
        }
    }

    std::vector<size_t> _dims;
    DataSpace& _space;
    std::vector<typename type_of_array<T>::type> _vec_align;
};
#endif

#endif

// apply conversion for vectors nested vectors
template <typename T>
struct data_converter<std::vector<T>,
                      typename std::enable_if<(is_container<T>::value)>::type> {
    inline data_converter(std::vector<T>&, DataSpace& space)
        : _dims(space.getDimensions()) {}

    inline typename type_of_array<T>::type*
    transform_read(std::vector<T>&) {
        _vec_align.resize(compute_total_size(_dims));
        return _vec_align.data();
    }

    inline typename type_of_array<T>::type*
    transform_write(std::vector<T>& vec) {
        _vec_align.reserve(compute_total_size(_dims));
        vectors_to_single_buffer<T>(vec, _dims, 0, _vec_align);
        return _vec_align.data();
    }

    inline void process_result(std::vector<T>& vec) {
        single_buffer_to_vectors(
            _vec_align.begin(), _vec_align.end(), _dims, 0, vec);
    }

    std::vector<size_t> _dims;
    std::vector<typename type_of_array<T>::type> _vec_align;
};


// apply conversion to scalar string
template <>
struct data_converter<std::string, void> {
    inline data_converter(std::string& vec, DataSpace& space)
        : _c_vec(nullptr),  _space(space) {
        (void)vec;
    }

    // create a C vector adapted to HDF5
    // fill last element with NULL to identify end
    inline char** transform_read(std::string&) { return (&_c_vec); }

    static inline char* char_converter(const std::string& str) {
        return const_cast<char*>(str.c_str());
    }

    inline char** transform_write(std::string& str) {
        _c_vec = const_cast<char*>(str.c_str());
        return &_c_vec;
    }

    inline void process_result(std::string& str) {
        assert(_c_vec != nullptr);
        str = std::string(_c_vec);

        if (_c_vec != NULL) {
            AtomicType<std::string> str_type;
            (void)H5Dvlen_reclaim(str_type.getId(), _space.getId(), H5P_DEFAULT,
                                  &_c_vec);
        }
    }

    char* _c_vec;
    DataSpace& _space;
};

// apply conversion for vectors of string (dereference)
template <>
struct data_converter<std::vector<std::string>, void> {
    inline data_converter(std::vector<std::string>& vec, DataSpace& space)
        : _space(space) {
        (void)vec;
    }

    // create a C vector adapted to HDF5
    // fill last element with NULL to identify end
    inline char** transform_read(std::vector<std::string>& vec) {
        (void)vec;
        _c_vec.resize(_space.getDimensions()[0], NULL);
        return (&_c_vec[0]);
    }

    static inline char* char_converter(const std::string& str) {
        return const_cast<char*>(str.c_str());
    }

    inline char** transform_write(std::vector<std::string>& vec) {
        _c_vec.resize(vec.size() + 1, NULL);
        std::transform(vec.begin(), vec.end(), _c_vec.begin(), &char_converter);
        return (&_c_vec[0]);
    }

    inline void process_result(std::vector<std::string>& vec) {
        (void)vec;
        vec.resize(_c_vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i] = std::string(_c_vec[i]);
        }

        if (_c_vec.empty() == false && _c_vec[0] != NULL) {
            AtomicType<std::string> str_type;
            (void)H5Dvlen_reclaim(str_type.getId(), _space.getId(), H5P_DEFAULT,
                                  &(_c_vec[0]));
        }
    }

    std::vector<char*> _c_vec;
    DataSpace& _space;
};
}
}

#endif // H5CONVERTER_MISC_HPP


//#include "H5Utils.hpp" --Already included

namespace HighFive {

inline Attribute::Attribute() {}

inline size_t Attribute::getStorageSize() const {
    return static_cast<size_t>(H5Aget_storage_size(_hid));
}

inline DataType Attribute::getDataType() const {
    DataType res;
    res._hid = H5Aget_type(_hid);
    return res;
}

inline DataSpace Attribute::getSpace() const {
    DataSpace space;
    if ((space._hid = H5Aget_space(_hid)) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            "Unable to get DataSpace out of Attribute");
    }
    return space;
}

inline DataSpace Attribute::getMemSpace() const { return getSpace(); }

template <typename T>
inline void Attribute::read(T& array) const {
    typedef typename std::remove_const<T>::type type_no_const;

    type_no_const& nocv_array = const_cast<type_no_const&>(array);

    const size_t dim_array = details::array_dims<type_no_const>::value;
    DataSpace space = getSpace();
    DataSpace mem_space = getMemSpace();

    if (!details::checkDimensions(mem_space, dim_array)) {
        std::ostringstream ss;
        ss << "Impossible to read attribute of dimensions "
           << mem_space.getNumberDimensions() << " into arrays of dimensions "
           << dim_array;
        throw DataSpaceException(ss.str());
    }

    // Create mem datatype
    const AtomicType<typename details::type_of_array<type_no_const>::type>
        array_datatype;

    // Apply pre read conversions
    details::data_converter<type_no_const> converter(nocv_array, mem_space);

    if (H5Aread(getId(), array_datatype.getId(),
                static_cast<void*>(converter.transform_read(nocv_array))) < 0) {
        HDF5ErrMapper::ToException<AttributeException>(
            "Error during HDF5 Read: ");
    }

    // re-arrange results
    converter.process_result(nocv_array);
}

template <typename T>
inline void Attribute::write(const T& buffer) {
    typedef typename std::remove_const<T>::type type_no_const;

    type_no_const& nocv_buffer = const_cast<type_no_const&>(buffer);

    const size_t dim_buffer = details::array_dims<type_no_const>::value;
    DataSpace space = getSpace();
    DataSpace mem_space = getMemSpace();

    if (!details::checkDimensions(mem_space, dim_buffer)) {
        std::ostringstream ss;
        ss << "Impossible to write buffer of dimensions " << dim_buffer
           << " into attribute of dimensions "
           << mem_space.getNumberDimensions();
        throw DataSpaceException(ss.str());
    }

    const AtomicType<typename details::type_of_array<type_no_const>::type>
        array_datatype;

    // Apply pre write conversions
    details::data_converter<type_no_const> converter(nocv_buffer, mem_space);

    if (H5Awrite(getId(), array_datatype.getId(),
                 static_cast<const void*>(
                     converter.transform_write(nocv_buffer))) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Error during HDF5 Write: ");
    }
}
}

#endif // H5ATTRIBUTE_MISC_HPP


#endif // H5ATTRIBUTE_HPP
//#include "H5Group.hpp"      // for Node
///H5Group///
//////////////

#ifndef HIGHFIVE_H5GROUP_HPP
#define HIGHFIVE_H5GROUP_HPP

//#include "H5Object.hpp" --Already included
//#include "bits/H5Annotate_traits.hpp" --Already included
//#include "bits/H5Node_traits.hpp" --Already included

namespace HighFive {

class File;

///
/// \brief Represents an hdf5 group
class Group : public Object,
              public NodeTraits<Group>,
              public AnnotateTraits<Group> {
  public:
    Group();

    friend class File;
};
}

//#include "bits/H5Node_traits_misc.hpp"
//////////////////////////////////////
///H5Node_traits_misc///
////////////////////////

#ifndef H5NODE_TRAITS_MISC_HPP
#define H5NODE_TRAITS_MISC_HPP

#include <string>
#include <vector>

#include <H5Apublic.h>
#include <H5Dpublic.h>
#include <H5Fpublic.h>
#include <H5Gpublic.h>
#include <H5Ppublic.h>
#include <H5Tpublic.h>

//#include "H5Iterables_misc.hpp" --Already included

//#include "../H5DataSet.hpp"
///H5DataSet///
///////////////

#ifndef H5DATASET_HPP
#define H5DATASET_HPP

#include <vector>

//#include "H5DataSpace.hpp" --Already included
//#include "H5DataType.hpp" --Already included
//#include "H5Object.hpp" --Already included
//#include "bits/H5Annotate_traits.hpp" --Already included
//#include "bits/H5Slice_traits.hpp"
///H5Slice_traits///
////////////////////

#ifndef H5SLICE_TRAITS_HPP
#define H5SLICE_TRAITS_HPP

#include <cstdlib>
#include <vector>

//#include "H5Utils.hpp" --Already included

namespace HighFive {

class Selection;

template <typename>
class SliceTraits;

class ElementSet {
  public:
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param list List of continuous coordinates (eg.: in 2 dimensions space `ElementSet{1, 2, 3 ,4}` create points
    /// `(1, 2)` and `(3, 4)`).
    explicit ElementSet(std::initializer_list<std::size_t> list);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param list List of N-dim points.
    explicit ElementSet(std::initializer_list<std::vector<std::size_t>> list);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param element_ids List of continuous coordinates (eg.: in 2 dimensions space `ElementSet{1, 2, 3 ,4}` create points
    /// `(1, 2)` and `(3, 4)`).
    explicit ElementSet(const std::vector<std::size_t>& element_ids);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param element_ids List of N-dim points.
    explicit ElementSet(const std::vector<std::vector<std::size_t>>& element_ids);

  private:
    std::vector<std::size_t> _ids;

    template <typename Derivate>
    friend class SliceTraits;
};

template <typename Derivate>
class SliceTraits {
  public:
    ///
    /// \brief Select a region in the current Slice/Dataset of \p count points at
    /// \p offset separated by \p stride. If strides are not provided they will
    /// default to 1 in all dimensions.
    ///
    /// vector offset and count have to be from the same dimension
    ///
    Selection select(const std::vector<size_t>& offset,
                     const std::vector<size_t>& count,
                     const std::vector<size_t>& stride = std::vector<size_t>())
        const;

    ///
    /// \brief Select a set of columns in the last dimension of this dataset.
    ///
    /// The column indices must be smaller than the dimension size.
    ///
    Selection select(const std::vector<size_t>& columns) const;

    ///
    /// \brief Select a region in the current Slice/Dataset out of a list of elements.
    ///
    Selection select(const ElementSet& elements) const;

    ///
    /// Read the entire dataset into a buffer
    /// An exception is raised is if the numbers of dimension of the buffer and
    /// of the dataset are different.
    ///
    /// The array type can be a N-pointer or a N-vector. For plain pointers
    /// not dimensionality checking will be performed, it is the user's
    /// responsibility to ensure that the right amount of space has been
    /// allocated.
    template <typename T>
    void read(T& array) const;

    ///
    /// Read the entire dataset into a raw buffer
    ///
    /// No dimensionality checks will be performed, it is the user's
    /// responsibility to ensure that the right amount of space has been
    /// allocated.
    template <typename T>
    void read(T* array) const;

    ///
    /// Write the integrality N-dimension buffer to this dataset
    /// An exception is raised is if the numbers of dimension of the buffer and
    /// of the dataset are different
    ///
    /// The array type can be a N-pointer or a N-vector ( e.g int** integer two
    /// dimensional array )
    template <typename T>
    void write(const T& buffer);

    ///
    /// Write from a raw buffer into this dataset
    ///
    /// No dimensionality checks will be performed, it is the user's
    /// responsibility to ensure that the buffer holds the right amount of
    /// elements. For n-dimensional matrices the buffer layout follows H5
    /// default conventions.
    template <typename T>
    void write(const T* buffer);

  private:
    typedef Derivate derivate_type;
};
}

#endif // H5SLICE_TRAITS_HPP

namespace HighFive {

template <typename Derivate>
class NodeTraits;

///
/// \brief Class representing a dataset.
///
class DataSet : public Object,
                public SliceTraits<DataSet>,
                public AnnotateTraits<DataSet> {
  public:
    ///
    /// \brief getStorageSize
    /// \return returns the amount of storage allocated for a dataset.
    ///
    uint64_t getStorageSize() const;

    ///
    /// \brief getOffset
    /// \return returns DataSet address in file
    ///
    uint64_t getOffset() const;

    ///
    /// \brief getDataType
    /// \return return the datatype associated with this dataset
    ///
    DataType getDataType() const;

    ///
    /// \brief getSpace
    /// \return return the dataspace associated with this dataset
    ///
    DataSpace getSpace() const;

    ///
    /// \brief getMemSpace
    /// \return same than getSpace for DataSet, compatibility with Selection
    /// class
    ///
    DataSpace getMemSpace() const;


    /// \brief Change the size of the dataset
    ///
    /// This requires that the dataset was created with chunking, and you would
    /// generally want to have set a larger maxdims setting
    /// \param dims New size of the dataset
    void resize(const std::vector<size_t>& dims);


    /// \brief Get the dimensions of the whole DataSet.
    ///       This is a shorthand for getSpace().getDimensions()
    /// \return The shape of the current HighFive::DataSet
    ///
    inline std::vector<size_t> getDimensions() const {
        return getSpace().getDimensions();
    }

    /// \brief Get the total number of elements in the current dataset.
    ///       E.g. 2x2x2 matrix has size 8.
    ///       This is a shorthand for getSpace().getTotalCount()
    /// \return The shape of the current HighFive::DataSet
    ///
    inline size_t getElementCount() const {
        return getSpace().getElementCount();
    }

  private:
    DataSet();
    template <typename Derivate>
    friend class ::HighFive::NodeTraits;
};

}  // namespace HighFive

//#include "bits/H5DataSet_misc.hpp"
///H5DataSet_misc///
////////////////////

#ifndef H5DATASET_MISC_HPP
#define H5DATASET_MISC_HPP

#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>

#ifdef H5_USE_BOOST
#include <boost/multi_array.hpp>
#endif

#include <H5Dpublic.h>
#include <H5Ppublic.h>

//#include "H5Utils.hpp" --Already included

namespace HighFive {

inline DataSet::DataSet() {}

inline uint64_t DataSet::getStorageSize() const {
    return H5Dget_storage_size(_hid);
}

inline DataType DataSet::getDataType() const {
    DataType res;
    res._hid = H5Dget_type(_hid);
    return res;
}

inline DataSpace DataSet::getSpace() const {
    DataSpace space;
    if ((space._hid = H5Dget_space(_hid)) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Unable to get DataSpace out of DataSet");
    }
    return space;
}

inline DataSpace DataSet::getMemSpace() const {
    return getSpace();
}

inline uint64_t DataSet::getOffset() const {
    uint64_t addr = H5Dget_offset(_hid);
    if (addr == HADDR_UNDEF) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Cannot get offset of DataSet.");
    }
    return addr;
}

inline void DataSet::resize(const std::vector<size_t>& dims) {

    const size_t numDimensions = getSpace().getDimensions().size();
    if (dims.size() != numDimensions) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Invalid dataspace dimensions, got " + std::to_string(dims.size()) +
            " expected " + std::to_string(numDimensions));
    }

    std::vector<hsize_t> real_dims(dims.begin(), dims.end());

    if (H5Dset_extent(getId(), real_dims.data()) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Could not resize dataset.");
    }
}

} // namespace HighFive

#endif // H5DATASET_MISC_HPP

// To avoid loops, we must bring the respective top-level header
// Only the headers representing a trait can include directly their implementation
///#include "H5Attribute.hpp"  // top-level header of AnnotateTraits               --Already included
//#include "H5Selection.hpp"  // top-level header of SliceTraits
/////////////////////////////
///H5Selection///
/////////////////

#ifndef H5SELECTION_HPP
#define H5SELECTION_HPP

//#include "H5DataSet.hpp" --Already Included
//#include "H5DataSpace.hpp" --Already included

//#include "bits/H5Slice_traits.hpp"
///H5Slice_traits///
////////////////////

#ifndef H5SLICE_TRAITS_HPP
#define H5SLICE_TRAITS_HPP

#include <cstdlib>
#include <vector>

//#include "H5Utils.hpp" --Already included

namespace HighFive {

class Selection;

template <typename>
class SliceTraits;

class ElementSet {
  public:
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param list List of continuous coordinates (eg.: in 2 dimensions space `ElementSet{1, 2, 3 ,4}` create points
    /// `(1, 2)` and `(3, 4)`).
    explicit ElementSet(std::initializer_list<std::size_t> list);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param list List of N-dim points.
    explicit ElementSet(std::initializer_list<std::vector<std::size_t>> list);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param element_ids List of continuous coordinates (eg.: in 2 dimensions space `ElementSet{1, 2, 3 ,4}` create points
    /// `(1, 2)` and `(3, 4)`).
    explicit ElementSet(const std::vector<std::size_t>& element_ids);
    ///
    /// \brief Create a list of points of N-dimension for selection.
    ///
    /// \param element_ids List of N-dim points.
    explicit ElementSet(const std::vector<std::vector<std::size_t>>& element_ids);

  private:
    std::vector<std::size_t> _ids;

    template <typename Derivate>
    friend class SliceTraits;
};

template <typename Derivate>
class SliceTraits {
  public:
    ///
    /// \brief Select a region in the current Slice/Dataset of \p count points at
    /// \p offset separated by \p stride. If strides are not provided they will
    /// default to 1 in all dimensions.
    ///
    /// vector offset and count have to be from the same dimension
    ///
    Selection select(const std::vector<size_t>& offset,
                     const std::vector<size_t>& count,
                     const std::vector<size_t>& stride = std::vector<size_t>())
        const;

    ///
    /// \brief Select a set of columns in the last dimension of this dataset.
    ///
    /// The column indices must be smaller than the dimension size.
    ///
    Selection select(const std::vector<size_t>& columns) const;

    ///
    /// \brief Select a region in the current Slice/Dataset out of a list of elements.
    ///
    Selection select(const ElementSet& elements) const;

    ///
    /// Read the entire dataset into a buffer
    /// An exception is raised is if the numbers of dimension of the buffer and
    /// of the dataset are different.
    ///
    /// The array type can be a N-pointer or a N-vector. For plain pointers
    /// not dimensionality checking will be performed, it is the user's
    /// responsibility to ensure that the right amount of space has been
    /// allocated.
    template <typename T>
    void read(T& array) const;

    ///
    /// Read the entire dataset into a raw buffer
    ///
    /// No dimensionality checks will be performed, it is the user's
    /// responsibility to ensure that the right amount of space has been
    /// allocated.
    template <typename T>
    void read(T* array) const;

    ///
    /// Write the integrality N-dimension buffer to this dataset
    /// An exception is raised is if the numbers of dimension of the buffer and
    /// of the dataset are different
    ///
    /// The array type can be a N-pointer or a N-vector ( e.g int** integer two
    /// dimensional array )
    template <typename T>
    void write(const T& buffer);

    ///
    /// Write from a raw buffer into this dataset
    ///
    /// No dimensionality checks will be performed, it is the user's
    /// responsibility to ensure that the buffer holds the right amount of
    /// elements. For n-dimensional matrices the buffer layout follows H5
    /// default conventions.
    template <typename T>
    void write(const T* buffer);

  private:
    typedef Derivate derivate_type;
};
}

#endif // H5SLICE_TRAITS_HPP

namespace HighFive {

///
/// \brief Selection: represent a view on a slice/part of a dataset
///
/// A Selection is valid only if its parent dataset is valid
///
class Selection : public SliceTraits<Selection> {
  public:
    ///
    /// \brief getSpace
    /// \return Dataspace associated with this selection
    ///
    DataSpace getSpace() const;

    ///
    /// \brief getMemSpace
    /// \return Dataspace associated with the memory representation of this
    /// selection
    ///
    DataSpace getMemSpace() const;

    ///
    /// \brief getDataSet
    /// \return parent dataset of this selection
    ///
    DataSet& getDataset();
    const DataSet& getDataset() const;

  private:
    Selection(const DataSpace& memspace, const DataSpace& file_space,
              const DataSet& set);

    DataSpace _mem_space, _file_space;
    DataSet _set;

    template <typename Derivate>
    friend class ::HighFive::SliceTraits;
    // absolute namespace naming due to GCC bug 52625
};
}

//#include "bits/H5Slice_traits_misc.hpp"
///H5Slice_traits_misc///
/////////////////////////

#ifndef H5SLICE_TRAITS_MISC_HPP
#define H5SLICE_TRAITS_MISC_HPP

#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>

#ifdef H5_USE_BOOST
// starting Boost 1.64, serialization header must come before ublas
#include <boost/serialization/vector.hpp>
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

#include <H5Dpublic.h>
#include <H5Ppublic.h>

//#include "H5Converter_misc.hpp"--Already Included

namespace HighFive {

namespace details {

// map the correct reference to the dataset depending of the layout
// dataset -> itself
// subselection -> parent dataset
inline const DataSet& get_dataset(const Selection* ptr) {
    return ptr->getDataset();
}

inline const DataSet& get_dataset(const DataSet* ptr) { return *ptr; }

// map the correct memspace identifier depending of the layout
// dataset -> entire memspace
// selection -> resolve space id
inline hid_t get_memspace_id(const Selection* ptr) {
    return ptr->getMemSpace().getId();
}

inline hid_t get_memspace_id(const DataSet* ptr) {
    (void)ptr;
    return H5S_ALL;
}
}

inline ElementSet::ElementSet(std::initializer_list<std::size_t> list)
    : _ids(list) {}

inline ElementSet::ElementSet(std::initializer_list<std::vector<std::size_t>> list)
    : ElementSet(std::vector<std::vector<std::size_t>>(list)) {}


inline ElementSet::ElementSet(const std::vector<std::size_t>& element_ids)
    : _ids(element_ids) {}

inline ElementSet::ElementSet(const std::vector<std::vector<std::size_t>>& element_ids) {
    for (const auto& vec: element_ids) {
        std::copy(vec.begin(), vec.end(), std::back_inserter(_ids));
    }
}

template <typename Derivate>
inline Selection
SliceTraits<Derivate>::select(const std::vector<size_t>& offset,
                              const std::vector<size_t>& count,
                              const std::vector<size_t>& stride) const {
    // hsize_t type conversion
    // TODO : normalize hsize_t type in HighFive namespace
    std::vector<hsize_t> offset_local(offset.size());
    std::vector<hsize_t> count_local(count.size());
    std::vector<hsize_t> stride_local(stride.size());
    std::copy(offset.begin(), offset.end(), offset_local.begin());
    std::copy(count.begin(), count.end(), count_local.begin());
    std::copy(stride.begin(), stride.end(), stride_local.begin());

    DataSpace space = static_cast<const Derivate*>(this)->getSpace().clone();
    if (H5Sselect_hyperslab(space.getId(), H5S_SELECT_SET, offset_local.data(),
                            stride.empty() ? NULL : stride_local.data(),
                            count_local.data(), NULL) < 0) {
        HDF5ErrMapper::ToException<DataSpaceException>(
            "Unable to select hyperslap");
    }

    return Selection(DataSpace(count), space,
                     details::get_dataset(static_cast<const Derivate*>(this)));
}

template <typename Derivate>
inline Selection
SliceTraits<Derivate>::select(const std::vector<size_t>& columns) const {

    const DataSpace& space = static_cast<const Derivate*>(this)->getSpace();
    const DataSet& dataset =
        details::get_dataset(static_cast<const Derivate*>(this));
    std::vector<size_t> dims = space.getDimensions();
    std::vector<hsize_t> counts(dims.size());
    std::copy(dims.begin(), dims.end(), counts.begin());
    counts[dims.size() - 1] = 1;
    std::vector<hsize_t> offsets(dims.size(), 0);

    H5Sselect_none(space.getId());
    for (std::vector<size_t>::const_iterator i = columns.begin();
         i != columns.end(); ++i) {

        offsets[offsets.size() - 1] = *i;
        if (H5Sselect_hyperslab(space.getId(), H5S_SELECT_OR, offsets.data(),
                                0, counts.data(), 0) < 0) {
            HDF5ErrMapper::ToException<DataSpaceException>(
                "Unable to select hyperslap");
        }
    }

    dims[dims.size() - 1] = columns.size();
    return Selection(DataSpace(dims), space, dataset);
}

template <typename Derivate>
inline Selection
SliceTraits<Derivate>::select(const ElementSet& elements) const {
    const hsize_t* data = NULL;
    DataSpace space = static_cast<const Derivate*>(this)->getSpace().clone();
    const std::size_t length = elements._ids.size();
    if (length % space.getNumberDimensions() != 0) {
        throw DataSpaceException("Number of coordinates in elements picking "
                "should be a multiple of the dimensions.");
    }

    const std::size_t num_elements = length / space.getNumberDimensions();
    std::vector<hsize_t> raw_elements;

    // optimised at compile time
    // switch for data conversion on 32bits platforms
    if (std::is_same<std::size_t, hsize_t>::value) {
        // `if constexpr` can't be used, thus a reinterpret_cast is needed.
        data = reinterpret_cast<const hsize_t*>(&(elements._ids[0]));
    } else {
        raw_elements.resize(length);
        std::copy(elements._ids.begin(), elements._ids.end(),
                  raw_elements.begin());
        data = raw_elements.data();
    }

    if (H5Sselect_elements(space.getId(), H5S_SELECT_SET, num_elements, data) < 0) {
        HDF5ErrMapper::ToException<DataSpaceException>(
            "Unable to select elements");
    }

    return Selection(DataSpace(num_elements), space,
                     details::get_dataset(static_cast<const Derivate*>(this)));
}

template <typename Derivate>
template <typename T>
inline void SliceTraits<Derivate>::read(T& array) const {
    typedef typename std::remove_const<T>::type type_no_const;

    type_no_const& nocv_array = const_cast<type_no_const&>(array);

    const size_t dim_array = details::array_dims<type_no_const>::value;
    DataSpace space = static_cast<const Derivate*>(this)->getSpace();
    DataSpace mem_space = static_cast<const Derivate*>(this)->getMemSpace();

    if (!details::checkDimensions(mem_space, dim_array)) {
        std::ostringstream ss;
        ss << "Impossible to read DataSet of dimensions "
           << mem_space.getNumberDimensions() << " into arrays of dimensions "
           << dim_array;
        throw DataSpaceException(ss.str());
    }

    // Create mem datatype
    const AtomicType<typename details::type_of_array<type_no_const>::type>
        array_datatype;

    // Apply pre-read conversions
    details::data_converter<type_no_const> converter(nocv_array, mem_space);

    if (H5Dread(
            details::get_dataset(static_cast<const Derivate*>(this)).getId(),
            array_datatype.getId(),
            details::get_memspace_id((static_cast<const Derivate*>(this))),
            space.getId(), H5P_DEFAULT,
            static_cast<void*>(converter.transform_read(nocv_array))) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Error during HDF5 Read: ");
    }

    // re-arrange results
    converter.process_result(array);
}

template <typename Derivate>
template <typename T>
inline void SliceTraits<Derivate>::read(T* array) const {

    DataSpace space = static_cast<const Derivate*>(this)->getSpace();
    DataSpace mem_space = static_cast<const Derivate*>(this)->getMemSpace();

    // Create mem datatype
    const AtomicType<typename details::type_of_array<T>::type> array_datatype;

    if (H5Dread(
            details::get_dataset(static_cast<const Derivate*>(this)).getId(),
            array_datatype.getId(),
            details::get_memspace_id((static_cast<const Derivate*>(this))),
            space.getId(), H5P_DEFAULT,
            static_cast<void*>(array)) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Error during HDF5 Read: ");
    }
}

template <typename Derivate>
template <typename T>
inline void SliceTraits<Derivate>::write(const T& buffer) {
    typedef typename std::remove_const<T>::type type_no_const;

    type_no_const& nocv_buffer = const_cast<type_no_const&>(buffer);

    const size_t dim_buffer = details::array_dims<type_no_const>::value;
    DataSpace space = static_cast<const Derivate*>(this)->getSpace();
    DataSpace mem_space = static_cast<const Derivate*>(this)->getMemSpace();

    if (!details::checkDimensions(mem_space, dim_buffer)) {
        std::ostringstream ss;
        ss << "Impossible to write buffer of dimensions " << dim_buffer
           << " into dataset of dimensions " << mem_space.getNumberDimensions();
        throw DataSpaceException(ss.str());
    }

    const AtomicType<typename details::type_of_array<type_no_const>::type>
        array_datatype;

    // Apply pre write conversions
    details::data_converter<type_no_const> converter(nocv_buffer, mem_space);

    if (H5Dwrite(details::get_dataset(static_cast<Derivate*>(this)).getId(),
                 array_datatype.getId(),
                 details::get_memspace_id((static_cast<Derivate*>(this))),
                 space.getId(), H5P_DEFAULT,
                 static_cast<const void*>(
                     converter.transform_write(nocv_buffer))) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Error during HDF5 Write: ");
    }
}

template <typename Derivate>
template <typename T>
inline void SliceTraits<Derivate>::write(const T* buffer) {

    DataSpace space = static_cast<const Derivate*>(this)->getSpace();
    DataSpace mem_space = static_cast<const Derivate*>(this)->getMemSpace();

    const AtomicType<typename details::type_of_array<T>::type> array_datatype;

    if (H5Dwrite(details::get_dataset(static_cast<Derivate*>(this)).getId(),
                 array_datatype.getId(),
                 details::get_memspace_id((static_cast<Derivate*>(this))),
                 space.getId(), H5P_DEFAULT,
                 static_cast<const void*>(buffer)) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            "Error during HDF5 Write: ");
    }
}
}

#endif // H5SLICE_TRAITS_MISC_HPP
//#include "bits/H5Selection_misc.hpp"
///H5Selection_misc///
//////////////////////

#ifndef H5SELECTION_MISC_HPP
#define H5SELECTION_MISC_HPP

namespace HighFive {

inline Selection::Selection(const DataSpace& memspace,
                            const DataSpace& file_space, const DataSet& set)
    : _mem_space(memspace)
    , _file_space(file_space)
    , _set(set) {}

inline DataSpace Selection::getSpace() const {
    return _file_space;
}

inline DataSpace Selection::getMemSpace() const {
    return _mem_space;
}

inline DataSet& Selection::getDataset() {
    return _set;
}

inline const DataSet& Selection::getDataset() const {
    return _set;
}
}

#endif // H5SELECTION_MISC_HPP

#endif // H5SELECTION_HPP

#endif // H5DATASET_HPP


//#include "../H5Selection.hpp"  --Already included

//#include "../H5Utility.hpp"--Already included

namespace HighFive {


template <typename Derivate>
inline DataSet
NodeTraits<Derivate>::createDataSet(const std::string& dataset_name,
                                    const DataSpace& space,
                                    const DataType& dtype,
                                    const DataSetCreateProps& createProps,
                                    const DataSetAccessProps& accessProps) {
    DataSet set;
    if ((set._hid = H5Dcreate2(static_cast<Derivate*>(this)->getId(),
                               dataset_name.c_str(), dtype._hid, space._hid,
                               H5P_DEFAULT, createProps.getId(),
                               accessProps.getId())) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            std::string("Unable to create the dataset \"") + dataset_name +
            "\":");
    }
    return set;
}

template <typename Derivate>
template <typename Type>
inline DataSet
NodeTraits<Derivate>::createDataSet(const std::string& dataset_name,
                                    const DataSpace& space,
                                    const DataSetCreateProps& createProps,
                                    const DataSetAccessProps& accessProps) {
    return createDataSet(dataset_name, space, AtomicType<Type>(), createProps,
                         accessProps);
}

template <typename Derivate>
template <typename T>
inline DataSet
NodeTraits<Derivate>::createDataSet(const std::string& dataset_name,
                                    const T& data,
                                    const DataSetCreateProps& createProps,
                                    const DataSetAccessProps& accessProps) {
    DataSet ds = createDataSet(
        dataset_name, DataSpace::From(data),
        AtomicType<typename details::type_of_array<T>::type>(), createProps,
        accessProps);
    ds.write(data);
    return ds;
}

template <typename Derivate>
inline DataSet
NodeTraits<Derivate>::getDataSet(const std::string& dataset_name,
                                 const DataSetAccessProps& accessProps) const {
    DataSet set;
    if ((set._hid = H5Dopen2(static_cast<const Derivate*>(this)->getId(),
                             dataset_name.c_str(), accessProps.getId())) < 0) {
        HDF5ErrMapper::ToException<DataSetException>(
            std::string("Unable to open the dataset \"") + dataset_name + "\":");
    }
    return set;
}

template <typename Derivate>
inline Group NodeTraits<Derivate>::createGroup(const std::string& group_name,
                                               bool parents) {
    RawPropertyList<PropertyType::LINK_CREATE> lcpl;
    if (parents) {
        lcpl.add(H5Pset_create_intermediate_group, 1u);
    }
    Group group;
    if ((group._hid = H5Gcreate2(static_cast<Derivate*>(this)->getId(),
                                 group_name.c_str(), lcpl.getId(), H5P_DEFAULT,
                                 H5P_DEFAULT)) < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to create the group \"") + group_name + "\":");
    }
    return group;
}

template <typename Derivate>
inline Group
NodeTraits<Derivate>::getGroup(const std::string& group_name) const {
    Group group;
    if ((group._hid = H5Gopen2(static_cast<const Derivate*>(this)->getId(),
                               group_name.c_str(), H5P_DEFAULT)) < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to open the group \"") + group_name + "\":");
    }
    return group;
}

template <typename Derivate>
inline size_t NodeTraits<Derivate>::getNumberObjects() const {
    hsize_t res;
    if (H5Gget_num_objs(static_cast<const Derivate*>(this)->getId(), &res) < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to count objects in existing group or file"));
    }
    return static_cast<size_t>(res);
}

template <typename Derivate>
inline std::string NodeTraits<Derivate>::getObjectName(size_t index) const {
    const size_t maxLength = 255;
    char buffer[maxLength + 1];
    ssize_t retcode = H5Lget_name_by_idx(
        static_cast<const Derivate*>(this)->getId(), ".", H5_INDEX_NAME, H5_ITER_INC,
        index, buffer, static_cast<hsize_t>(maxLength) + 1, H5P_DEFAULT);
    if (retcode < 0) {
        HDF5ErrMapper::ToException<GroupException>("Error accessing object name");
    }
    const size_t length = static_cast<std::size_t>(retcode);
    if (length <= maxLength) {
        return std::string(buffer, length);
    }
    std::vector<char> bigBuffer(length + 1, 0);
    H5Lget_name_by_idx(
        static_cast<const Derivate*>(this)->getId(), ".", H5_INDEX_NAME, H5_ITER_INC,
        index, bigBuffer.data(), static_cast<hsize_t>(length) + 1, H5P_DEFAULT);
    return std::string(bigBuffer.data(), length);
}

template <typename Derivate>
inline std::vector<std::string> NodeTraits<Derivate>::listObjectNames() const {

    std::vector<std::string> names;
    details::HighFiveIterateData iterateData(names);

    size_t num_objs = getNumberObjects();
    names.reserve(num_objs);

    if (H5Literate(static_cast<const Derivate*>(this)->getId(), H5_INDEX_NAME,
                   H5_ITER_INC, NULL,
                   &details::internal_high_five_iterate<H5L_info_t>,
                   static_cast<void*>(&iterateData)) < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to list objects in group"));
    }

    return names;
}

template <typename Derivate>
inline bool NodeTraits<Derivate>::_exist(const std::string& node_name) const {
    htri_t val = H5Lexists(static_cast<const Derivate*>(this)->getId(),
                           node_name.c_str(), H5P_DEFAULT);

    if (val < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Invalid link for exist() "));
    }

    // The root path always exists, but H5Lexists return 0 or 1
    // depending of the version of HDF5, so always return true for it
    // We should call H5Lexists anyway to check that no error is returned
    if (node_name == "/")
        return true;

    return (val > 0);
}

template <typename Derivate>
inline bool NodeTraits<Derivate>::exist(const std::string& group_path) const {
    // When there are slashes, first check everything is fine
    // so that subsequent errors are only due to missing intermediate groups
    if (group_path.find('/') != std::string::npos) {
        _exist("/");  // Shall not throw under normal circumstances
        try {
            SilenceHDF5 silencer;
            return _exist(group_path);
        } catch (const GroupException&) {
            return false;
        }
    }
    return _exist(group_path);
}


// convert internal link types to enum class.
// This function is internal, so H5L_TYPE_ERROR shall be handled in the calling context
static inline LinkType _convert_link_type(const H5L_type_t& ltype) noexcept {
    switch (ltype) {
        case H5L_TYPE_HARD:
            return LinkType::Hard;
        case H5L_TYPE_SOFT:
            return LinkType::Soft;
        case H5L_TYPE_EXTERNAL:
            return LinkType::External;
        default:
            // Other link types are possible but are considered strange to HighFive.
            // see https://support.hdfgroup.org/HDF5/doc/RM/H5L/H5Lregister.htm
            return LinkType::Other;
    }
}

template <typename Derivate>
inline LinkType NodeTraits<Derivate>::getLinkType(const std::string& node_name) const {
    H5L_info_t linkinfo;
    if (H5Lget_info(static_cast<const Derivate*>(this)->getId(),
                    node_name.c_str(), &linkinfo, H5P_DEFAULT) < 0
            || linkinfo.type == H5L_TYPE_ERROR) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to obtain info for link ") + node_name);
    }
    return _convert_link_type(linkinfo.type);
}

template <typename Derivate>
inline ObjectType NodeTraits<Derivate>::getObjectType(const std::string& node_name) const {
    return _open(node_name).getType();
}


template <typename Derivate>
inline Object NodeTraits<Derivate>::_open(const std::string& node_name,
                                          const DataSetAccessProps& accessProps) const {
    hid_t id = H5Oopen(static_cast<const Derivate*>(this)->getId(),
                       node_name.c_str(),
                       accessProps.getId());
    if (id < 0) {
        HDF5ErrMapper::ToException<GroupException>(
            std::string("Unable to open \"") + node_name + "\":");
    }
    return Object(id);
}



}  // namespace HighFive

#endif  // H5NODE_TRAITS_MISC_HPP
//#include "bits/H5Group_misc.hpp"
///Group_misc///
////////////////

#include <H5Gpublic.h>

namespace HighFive {

inline Group::Group() {}

} // namespace HighFive

//#include "H5Attribute.hpp"  // for AnnotateTraits --Already included

#endif // HIGHFIVE_H5GROUP_HPP
#endif  // H5FILE_HPP

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


using namespace HighFive;

namespace lheh5 {


    struct Particle {
       int id, status, mother1, mother2, color1, color2;
       double px, py, pz, e, m, lifetime, spin;
          // id        .. IDUP
          // color1/2  .. ICOLUP firt/second
          // mother1/2 .. MOTHUP first/second
          // status    .. ISTUP
          // px ... m  .. PUP[..]
          // lifetime  .. VTIMUP  (UP ... user process)
          // spin      .. SPINUP
    };

    std::ostream& operator << (std::ostream& os, Particle const & p) {
      os << "\tpx: " << p.px  << " py: " << p.py  << " pz: " << p.pz  << " e: " << p.e <<  "\n";
      return os;
    }

    
    struct EventHeader {
    // Event info
        int    nparticles  ; // corr to NUP
        int    pid         ; // this is all LHAu-::setProcess
        double weight      ;
        size_t    trials      ;
        double scale       ;
        double rscale      ;
        double fscale      ;
        double aqed        ;
        double aqcd        ;
        int npLO        ;
        int npNLO        ;
    };
    
    std::ostream& operator << (std::ostream& os, EventHeader const & eh) {
      os << "\tnparticles: " << eh.nparticles  << " procid: " << eh.pid  << " weight: " << eh.weight  << " trials: " << eh.trials <<  "\n";
      os << "\tscale: " << eh.scale  << " rscale: " << eh.rscale  << " fscale: " << eh.fscale  << " aqed: " << eh.aqed << " aqcd: " << eh.aqcd <<  "\n";
      os << "\tnpLO: " << eh.npLO  << " npNLO: " << eh.npNLO <<  "\n";
      return os;
    }


    struct Events {
        // Lookup 
        std::vector<size_t> _vstart;
        std::vector<size_t> _vend;
        // Particles 
        std::vector<int> _vid         ;
        std::vector<int> _vstatus     ;
        std::vector<int> _vmother1    ;
        std::vector<int> _vmother2    ;
        std::vector<int> _vcolor1     ;
        std::vector<int> _vcolor2     ;
        std::vector<double> _vpx      ;
        std::vector<double> _vpy      ;
        std::vector<double> _vpz      ;
        std::vector<double> _ve       ;
        std::vector<double> _vm       ;
        std::vector<double> _vlifetime;
        std::vector<double> _vspin    ;
        // Event info
        std::vector<int>    _vnparticles  ;
        std::vector<int>    _vpid         ;
        std::vector<double> _vweight      ;
        std::vector<size_t>    _vtrials      ;
        std::vector<double> _vscale       ;
        std::vector<double> _vrscale      ;
        std::vector<double> _vfscale      ;
        std::vector<double> _vaqed        ;
        std::vector<double> _vaqcd        ;
        std::vector<int> _vnpLO        ;
        std::vector<int> _vnpNLO        ;
        size_t _particle_offset;

        Particle mkParticle(size_t idx) const;
        std::vector<Particle> mkEvent(size_t ievent) const;
        EventHeader mkEventHeader(int ievent) const;
    };
    
    struct Events2 {
        // Lookup 
        std::vector<size_t> _vstart;
        // Particles 
        std::vector<int> _vid         ;
        std::vector<int> _vstatus     ;
        std::vector<int> _vmother1    ;
        std::vector<int> _vmother2    ;
        std::vector<int> _vcolor1     ;
        std::vector<int> _vcolor2     ;
        std::vector<double> _vpx      ;
        std::vector<double> _vpy      ;
        std::vector<double> _vpz      ;
        std::vector<double> _ve       ;
        std::vector<double> _vm       ;
        std::vector<double> _vlifetime;
        std::vector<double> _vspin    ;
        // Event info
        std::vector<int>    _vnparticles  ;
        std::vector<int>    _vpid         ;
        std::vector<double> _vweight      ;
        std::vector<size_t>    _vtrials      ;
        std::vector<double> _vscale       ;
        std::vector<double> _vrscale      ;
        std::vector<double> _vfscale      ;
        std::vector<double> _vaqed        ;
        std::vector<double> _vaqcd        ;
        int npLO        ;
        int npNLO       ;
        size_t _particle_offset;

        Particle mkParticle(size_t idx) const;
        std::vector<Particle> mkEvent(size_t ievent) const;
        EventHeader mkEventHeader(int ievent) const;
    };


    Events  readEvents(Group& g_index, Group& g_particle, Group& g_event, size_t first_event, size_t n_events);
    Events2 readEvents(Group& g_particle, Group& g_event, size_t first_event, size_t n_events, int npLO, int npNLO);
}

#endif


namespace lheh5 {
   

    Particle Events::mkParticle(size_t idx) const {
       return {std::move(_vid[idx]), std::move(_vstatus[idx]), std::move(_vmother1[idx]), std::move(_vmother2[idx]),
          std::move(_vcolor1[idx]), std::move(_vcolor2[idx]), std::move(_vpx[idx]), std::move(_vpy[idx]), std::move(_vpz[idx]),
          std::move(_ve[idx]), std::move(_vm[idx]), std::move(_vlifetime[idx]),std::move(_vspin[idx])};
    }

    std::vector<Particle> Events::mkEvent(size_t ievent) const {
         std::vector<Particle> _E;
         // NOTE we need to subtract the particle offset here as the 
         // particle properties run from 0 and not the global index when using batches
         for (size_t id=_vstart[ievent] - _particle_offset; id <_vend[ievent] - _particle_offset ;++id){
             _E.push_back(mkParticle( id));
         }

         // Make sure beam particles are ordered according to convention i.e. first particle has positive z-momentum
         if (_E[0].pz <0) {
           std::swap<Particle>(_E[0], _E[1]);
         }

         return _E;
    }

    EventHeader Events::mkEventHeader(int iev) const {
        return {std::move(_vnparticles[iev]), std::move(_vpid[iev]),
          std::move(_vweight[iev]), std::move(_vtrials[iev]),
          std::move(_vscale[iev]), std::move(_vrscale[iev]), std::move(_vfscale[iev]),
          std::move(_vaqed[iev]), std::move(_vaqcd[iev]),
          std::move(_vnpLO[iev]), std::move(_vnpNLO[iev]),
        };
    }

    // Read function, returns an Events struct
    Events readEvents(Group& g_index, Group& g_particle, Group& g_event, size_t first_event, size_t n_events) {
        // Lookup 
        std::vector<size_t> _vstart, _vend;
        // Particles 
        std::vector<int>    _vid, _vstatus, _vmother1, _vmother2, _vcolor1, _vcolor2;
        std::vector<double> _vpx, _vpy, _vpz, _ve, _vm, _vlifetime, _vspin;
        // Event info
        std::vector<int>    _vnparticles, _vpid, _vnpLO, _vnpNLO;
        std::vector<size_t> _vtrials      ;
        std::vector<double> _vweight, _vscale, _vrscale, _vfscale, _vaqed, _vaqcd;

        //double starttime, endtime;
        //starttime = MPI_Wtime();
        // Lookup
        DataSet _start = g_index.getDataSet("start");
        DataSet _end   = g_index.getDataSet("end");
        // Particles 
        DataSet _id       =  g_particle.getDataSet("id");
        DataSet _status   =  g_particle.getDataSet("status");
        DataSet _mother1  =  g_particle.getDataSet("mother1");
        DataSet _mother2  =  g_particle.getDataSet("mother2");
        DataSet _color1   =  g_particle.getDataSet("color1");
        DataSet _color2   =  g_particle.getDataSet("color2");
        DataSet _px       =  g_particle.getDataSet("px");
        DataSet _py       =  g_particle.getDataSet("py");
        DataSet _pz       =  g_particle.getDataSet("pz");
        DataSet _e        =  g_particle.getDataSet("e");
        DataSet _m        =  g_particle.getDataSet("m");
        DataSet _lifetime =  g_particle.getDataSet("lifetime");
        DataSet _spin     =  g_particle.getDataSet("spin");
        // Event info
        DataSet _nparticles =  g_event.getDataSet("nparticles");
        DataSet _pid        =  g_event.getDataSet("pid");
        DataSet _weight     =  g_event.getDataSet("weight");
        DataSet _trials     =  g_event.getDataSet("trials");
        DataSet _scale      =  g_event.getDataSet("scale");
        DataSet _rscale     =  g_event.getDataSet("rscale");
        DataSet _fscale     =  g_event.getDataSet("fscale");
        DataSet _aqed       =  g_event.getDataSet("aqed");
        DataSet _aqcd       =  g_event.getDataSet("aqcd");
        DataSet _npLO       =  g_event.getDataSet("npLO");
        DataSet _npNLO       =  g_event.getDataSet("npNLO");

        //endtime   = MPI_Wtime();
        //printf("DS took %f seconds\n", endtime-starttime);
        std::vector<size_t> offset_e = {first_event};
        std::vector<size_t> readsize_e = {n_events};
        //_vstart.reserve(n_events);
        //_vend.reserve(n_events);

        _start.select(offset_e, readsize_e).read(_vstart);
        _end.select(  offset_e, readsize_e).read(_vend  );
        std::vector<size_t> offset_p   = {_vstart.front()};
        std::vector<size_t> readsize_p = {_vend.back()-_vstart.front()};

        int RESP = _vend.back()-_vstart.front();
        _vid.reserve(RESP);
        _vstatus  .reserve(RESP);
        _vmother1 .reserve(RESP);
        _vmother2 .reserve(RESP);
        _vcolor1  .reserve(RESP);
        _vcolor2  .reserve(RESP);
        _vpx      .reserve(RESP);
        _vpy      .reserve(RESP);
        _vpz      .reserve(RESP);
        _ve       .reserve(RESP);
        _vm       .reserve(RESP);
        _vlifetime.reserve(RESP);
        _vspin    .reserve(RESP);
        
        _vnparticles.reserve(n_events);
        _vpid       .reserve(n_events);
        _vweight    .reserve(n_events); 
        _vtrials    .reserve(n_events);
        _vscale     .reserve(n_events);
        _vrscale    .reserve(n_events);
        _vfscale    .reserve(n_events);
        _vaqed      .reserve(n_events);
        _vaqcd      .reserve(n_events);
        _vnpLO      .reserve(n_events);
        _vnpNLO     .reserve(n_events);

        //starttime = MPI_Wtime();
        // This is using HighFive's read
        _id      .select(offset_p, readsize_p).read(_vid      );
        _status  .select(offset_p, readsize_p).read(_vstatus  );
        _mother1 .select(offset_p, readsize_p).read(_vmother1 );
        _mother2 .select(offset_p, readsize_p).read(_vmother2 );
        _color1  .select(offset_p, readsize_p).read(_vcolor1  );
        _color2  .select(offset_p, readsize_p).read(_vcolor2  );
        _px      .select(offset_p, readsize_p).read(_vpx      );
        _py      .select(offset_p, readsize_p).read(_vpy      );
        _pz      .select(offset_p, readsize_p).read(_vpz      );
        _e       .select(offset_p, readsize_p).read(_ve       );
        _m       .select(offset_p, readsize_p).read(_vm       );
        _lifetime.select(offset_p, readsize_p).read(_vlifetime);
        _spin    .select(offset_p, readsize_p).read(_vspin    );

        //endtime   = MPI_Wtime();
        //printf("SELP took %f seconds\n", endtime-starttime);
        //starttime = MPI_Wtime();
        _nparticles.select(offset_e, readsize_e).read(_vnparticles);
        _pid       .select(offset_e, readsize_e).read(_vpid       );
        _weight    .select(offset_e, readsize_e).read(_vweight    ); 
        _trials    .select(offset_e, readsize_e).read(_vtrials    );
        _scale     .select(offset_e, readsize_e).read(_vscale     );
        _rscale    .select(offset_e, readsize_e).read(_vrscale    );
        _fscale    .select(offset_e, readsize_e).read(_vfscale    );
        _aqed      .select(offset_e, readsize_e).read(_vaqed      );
        _aqcd      .select(offset_e, readsize_e).read(_vaqcd      );
        _npLO      .select(offset_e, readsize_e).read(_vnpLO      );
        _npNLO     .select(offset_e, readsize_e).read(_vnpNLO     );
        //endtime   = MPI_Wtime();
        //printf("SELE took %f seconds\n", endtime-starttime);

        return { 
           std::move(_vstart     ),
           std::move(_vend       ),
           std::move(_vid        ),
           std::move(_vstatus    ),
           std::move(_vmother1   ),
           std::move(_vmother2   ),
           std::move(_vcolor1    ),
           std::move(_vcolor2    ),
           std::move(_vpx        ),
           std::move(_vpy        ),
           std::move(_vpz        ),
           std::move(_ve         ),
           std::move(_vm         ),
           std::move(_vlifetime  ),
           std::move(_vspin      ),
           std::move(_vnparticles),
           std::move(_vpid       ),
           std::move(_vweight    ),
           std::move(_vtrials    ),
           std::move(_vscale     ),
           std::move(_vrscale    ),
           std::move(_vfscale    ),
           std::move(_vaqed      ),
           std::move(_vaqcd      ), 
           std::move(_vnpLO      ), 
           std::move(_vnpNLO     ), 
           offset_p[0],
              } ; 
    }


    Particle Events2::mkParticle(size_t idx) const {
       return {std::move(_vid[idx]), std::move(_vstatus[idx]), std::move(_vmother1[idx]), std::move(_vmother2[idx]),
          std::move(_vcolor1[idx]), std::move(_vcolor2[idx]), std::move(_vpx[idx]), std::move(_vpy[idx]), std::move(_vpz[idx]),
          std::move(_ve[idx]), std::move(_vm[idx]), std::move(_vlifetime[idx]),std::move(_vspin[idx])};
    }

    std::vector<Particle> Events2::mkEvent(size_t ievent) const {
         std::vector<Particle> _E;
         // NOTE we need to subtract the particle offset here as the 
         // particle properties run from 0 and not the global index when using batches
         size_t partno = _vstart[ievent] - _particle_offset;
         for (int id=0; id <_vnparticles[ievent];++id){
             _E.push_back(mkParticle(partno));
             partno++;
         }

         // Make sure beam particles are ordered according to convention i.e. first particle has positive z-momentum
         if (_E[0].pz <0) {
           std::swap<Particle>(_E[0], _E[1]);
         }

         return _E;
    }

    EventHeader Events2::mkEventHeader(int iev) const {
        return {std::move(_vnparticles[iev]), std::move(_vpid[iev]),
          std::move(_vweight[iev]), std::move(_vtrials[iev]),
          std::move(_vscale[iev]), std::move(_vrscale[iev]), std::move(_vfscale[iev]),
          std::move(_vaqed[iev]), std::move(_vaqcd[iev]),
          npLO, npNLO,
        };
    }
    
    // Read function, returns an Events struct --- this is for the new structure
    Events2 readEvents(Group& g_particle, Group& g_event, size_t first_event, size_t n_events, int npLO, int npNLO) {
        // Lookup 
        std::vector<size_t> _vstart;
        // Particles 
        std::vector<int>    _vid, _vstatus, _vmother1, _vmother2, _vcolor1, _vcolor2;
        std::vector<double> _vpx, _vpy, _vpz, _ve, _vm, _vlifetime, _vspin;
        // Event info
        std::vector<int>    _vnparticles, _vpid;
        std::vector<size_t> _vtrials      ;
        std::vector<double> _vweight, _vscale, _vrscale, _vfscale, _vaqed, _vaqcd;

        //double starttime, endtime;
        // Lookup
        DataSet _start = g_event.getDataSet("start");
        // Particles 
        DataSet _id       =  g_particle.getDataSet("id");
        DataSet _status   =  g_particle.getDataSet("status");
        DataSet _mother1  =  g_particle.getDataSet("mother1");
        DataSet _mother2  =  g_particle.getDataSet("mother2");
        DataSet _color1   =  g_particle.getDataSet("color1");
        DataSet _color2   =  g_particle.getDataSet("color2");
        DataSet _px       =  g_particle.getDataSet("px");
        DataSet _py       =  g_particle.getDataSet("py");
        DataSet _pz       =  g_particle.getDataSet("pz");
        DataSet _e        =  g_particle.getDataSet("e");
        DataSet _m        =  g_particle.getDataSet("m");
        DataSet _lifetime =  g_particle.getDataSet("lifetime");
        DataSet _spin     =  g_particle.getDataSet("spin");
        // Event info
        DataSet _nparticles =  g_event.getDataSet("nparticles");
        DataSet _pid        =  g_event.getDataSet("pid");
        DataSet _weight     =  g_event.getDataSet("weight");
        DataSet _trials     =  g_event.getDataSet("trials");
        DataSet _scale      =  g_event.getDataSet("scale");
        DataSet _rscale     =  g_event.getDataSet("rscale");
        DataSet _fscale     =  g_event.getDataSet("fscale");
        DataSet _aqed       =  g_event.getDataSet("aqed");
        DataSet _aqcd       =  g_event.getDataSet("aqcd");
       
        std::vector<size_t> offset_e = {first_event};
        std::vector<size_t> readsize_e = {n_events};

        // We now know the first event to read
        _start.select(offset_e, readsize_e).read(_vstart);

        // That's the first particle
        std::vector<size_t> offset_p   = {_vstart.front()};
        // The last particle is last entry in start + nparticles of that event
        _vnparticles.reserve(n_events);
        _nparticles.select(offset_e, readsize_e).read(_vnparticles);
        
        size_t RESP = _vstart.back() -   _vstart.front() + _vnparticles.back();
        std::vector<size_t> readsize_p = {RESP};

        _vid.reserve(RESP);
        _vstatus  .reserve(RESP);
        _vmother1 .reserve(RESP);
        _vmother2 .reserve(RESP);
        _vcolor1  .reserve(RESP);
        _vcolor2  .reserve(RESP);
        _vpx      .reserve(RESP);
        _vpy      .reserve(RESP);
        _vpz      .reserve(RESP);
        _ve       .reserve(RESP);
        _vm       .reserve(RESP);
        _vlifetime.reserve(RESP);
        _vspin    .reserve(RESP);
        
        _vpid       .reserve(n_events);
        _vweight    .reserve(n_events); 
        _vtrials    .reserve(n_events);
        _vscale     .reserve(n_events);
        _vrscale    .reserve(n_events);
        _vfscale    .reserve(n_events);
        _vaqed      .reserve(n_events);
        _vaqcd      .reserve(n_events);

        // This is using HighFive's read
        _id      .select(offset_p, readsize_p).read(_vid      );
        _status  .select(offset_p, readsize_p).read(_vstatus  );
        _mother1 .select(offset_p, readsize_p).read(_vmother1 );
        _mother2 .select(offset_p, readsize_p).read(_vmother2 );
        _color1  .select(offset_p, readsize_p).read(_vcolor1  );
        _color2  .select(offset_p, readsize_p).read(_vcolor2  );
        _px      .select(offset_p, readsize_p).read(_vpx      );
        _py      .select(offset_p, readsize_p).read(_vpy      );
        _pz      .select(offset_p, readsize_p).read(_vpz      );
        _e       .select(offset_p, readsize_p).read(_ve       );
        _m       .select(offset_p, readsize_p).read(_vm       );
        _lifetime.select(offset_p, readsize_p).read(_vlifetime);
        _spin    .select(offset_p, readsize_p).read(_vspin    );

        _pid       .select(offset_e, readsize_e).read(_vpid       );
        _weight    .select(offset_e, readsize_e).read(_vweight    ); 
        _trials    .select(offset_e, readsize_e).read(_vtrials    );
        _scale     .select(offset_e, readsize_e).read(_vscale     );
        _rscale    .select(offset_e, readsize_e).read(_vrscale    );
        _fscale    .select(offset_e, readsize_e).read(_vfscale    );
        _aqed      .select(offset_e, readsize_e).read(_vaqed      );
        _aqcd      .select(offset_e, readsize_e).read(_vaqcd      );

        return { 
           std::move(_vstart     ),
           std::move(_vid        ),
           std::move(_vstatus    ),
           std::move(_vmother1   ),
           std::move(_vmother2   ),
           std::move(_vcolor1    ),
           std::move(_vcolor2    ),
           std::move(_vpx        ),
           std::move(_vpy        ),
           std::move(_vpz        ),
           std::move(_ve         ),
           std::move(_vm         ),
           std::move(_vlifetime  ),
           std::move(_vspin      ),
           std::move(_vnparticles),
           std::move(_vpid       ),
           std::move(_vweight    ),
           std::move(_vtrials    ),
           std::move(_vscale     ),
           std::move(_vrscale    ),
           std::move(_vfscale    ),
           std::move(_vaqed      ),
           std::move(_vaqcd      ),
           npLO,
           npNLO,
           offset_p[0],
              } ; 
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// main20.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program. It shows how PYTHIA 8 can write
// a Les Houches Event File based on its process-level events.

//#include "Pythia8/Pythia.h"

#include "Pythia8Plugins/HepMC2.h"

using namespace Pythia8;

class LHAupH5 : public Pythia8::LHAup {
  public:
    LHAupH5( HighFive::File* file_, size_t firstEvent, size_t readSize, size_t nTotal, bool verbose=true, bool readNTrials=true, bool noRead=false) : _numberRead(0),  _sumW(0) {
      file = file_;
      _index       = file->getGroup("index");
      _particle    = file->getGroup("particle");
      _event       = file->getGroup("event");
      _init        = file->getGroup("init");
      _procInfo    = file->getGroup("procInfo");
      _readSize = readSize;
      _noRead=noRead;
      // This reads and holds the information of readSize events, starting from firstEvent
      setInit();
      if (noRead){
         lheevents = lheh5::readEvents(_index, _particle, _event, firstEvent, 1);
      }
      else {
         lheevents = lheh5::readEvents(_index, _particle, _event, firstEvent, readSize);
      }
      if (readNTrials) {
         // Sum of trials for ALL to be processed events!!!
         DataSet _trials     =  _event.getDataSet("trials");
         std::vector<size_t>    _vtrials;
         _trials    .select({0}, {nTotal}).read(_vtrials);
         _nTrials = std::accumulate(_vtrials.begin(), _vtrials.end(), 0.0);
      }
      // This is for measurement only
      else {
         _nTrials = 100;
      }
    }
    
    // Read and set the info from init and procInfo
    bool setInit() override;// override;
    bool setEvent(int idProc=0) override;// override;

    size_t nTrials() { return _nTrials; }

    int getSize() { return lheevents._vnparticles.size(); }
  
  private:

    HighFive::File*                         file;
    // Connect with groups
    HighFive::Group                         _index, _particle, _event, _init, _procInfo;
    lheh5::Events                        lheevents;
    size_t                                     _numberRead;
    size_t                                     _nTrials;
    double                                  _sumW;
    bool                                  _noRead;
    size_t                                _readSize;

    // Flag to set particle production scales or not.
    LHAscales scalesNow;


};

bool LHAupH5::setInit()
{
   int beamA, beamB;
   double energyA, energyB;
   int PDFgroupA, PDFgroupB;
   int PDFsetA, PDFsetB;

   _init.getDataSet("beamA")    .read(beamA);
   _init.getDataSet("energyA")  .read(energyA);
   _init.getDataSet("PDFgroupA").read(PDFgroupA);
   _init.getDataSet("PDFsetA")  .read(PDFsetA);
   
   _init.getDataSet("beamB")    .read(beamB);
   _init.getDataSet("energyB")  .read(energyB);
   _init.getDataSet("PDFgroupB").read(PDFgroupB);
   _init.getDataSet("PDFsetB")  .read(PDFsetB);

   setBeamA(beamA, energyA, PDFgroupA, PDFsetA);
   setBeamB(beamB, energyB, PDFgroupB, PDFsetB);
   
   int weightingStrategy;
   _init.getDataSet("weightingStrategy").read(weightingStrategy);
   setStrategy(-4);
   
   int numProcesses;
   _init.getDataSet("numProcesses").read(numProcesses);

   // NOTE this is a hack for testing only
   numProcesses = 1;


   vector<int> procId;        // NOTE: C++17 allows int[numProcesses]
   vector<double> xSection;   // NOTE: C++17 allows double[numProcesses]
   vector<double> error;      // NOTE: C++17 allows double[numProcesses]
   vector<double> unitWeight; // NOTE: C++17 allows double[numProcesses]
   _procInfo.getDataSet("procId").read(procId);
   _procInfo.getDataSet("xSection").read(xSection);
   _procInfo.getDataSet("error").read(error);
   _procInfo.getDataSet("unitWeight").read(unitWeight);
   for (size_t np=0; np<numProcesses;++np) {
     addProcess(procId[np], xSection[np], error[np], unitWeight[np]);
     xSecSumSave += xSection[np];
     xErrSumSave += pow2(error[np]);
   }

  return true;
}

bool LHAupH5::setEvent(int idProc)
{

  lheh5::EventHeader eHeader = lheevents.mkEventHeader( _numberRead );

  //setProcess(eHeader.pid,eHeader.weight*(1. / (1e9*_nTrials)),eHeader.scale,eHeader.aqed,eHeader.aqcd);
  setProcess(eHeader.pid,eHeader.weight,eHeader.scale,eHeader.aqed,eHeader.aqcd);

  nupSave    = eHeader.nparticles;
  idprupSave = eHeader.pid;
  xwgtupSave = eHeader.weight;//*(1. / (1e9*_nTrials));
  scalupSave = eHeader.scale; // TODO which scale?
  aqedupSave = eHeader.aqed;
  aqcdupSave = eHeader.aqcd;
  std::vector<lheh5::Particle> particles;
  double scalein = -1.;

  // TEMPorary hack for mothers not being set in Sherpa
  if (_noRead) {
     particles = lheevents.mkEvent( 0 );
  }
  else {
     particles = lheevents.mkEvent( _numberRead );
  }

  for (unsigned int ip=0;ip< particles.size(); ++ip) {
     lheh5::Particle part = particles[ip];
     if (ip < 2) {
       addParticle(part.id,part.status,0, 0,part.color1,part.color2,
                   part.px,part.py,part.pz,part.e,part.m,part.lifetime,part.spin,scalein);
     }
     else {
    addParticle(part.id,1,part.mother1,part.mother2,part.color1,part.color2,
                part.px,part.py,part.pz,part.e,part.m,part.lifetime,part.spin,scalein);
     }
  }
    
  // Scale setting
  scalesNow.clear();
  scalesNow.muf   = eHeader.fscale;
  scalesNow.mur   = eHeader.rscale;
  scalesNow.mups  = eHeader.scale;

  infoPtr->scales = &scalesNow;
  
  infoPtr->setEventAttribute("npLO",  std::to_string(eHeader.npLO));
  infoPtr->setEventAttribute("npNLO", std::to_string(eHeader.npNLO));

  _numberRead++;
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UserHooksUtils.h"
#include "Pythia8_i/UserHooksFactory.h"
#include <stdexcept>
#include <iostream>

#include "Pythia8_i/Pythia8_i.h"

/////////////////////////////////////////////////////////////////////////////////////////////
