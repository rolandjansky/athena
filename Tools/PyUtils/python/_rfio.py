# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ctypes import *

def _load_library (libname):
    """
    Helper method to load a library by its natural name, not the OS-native name.
    But if the OS-native name is given, it is safely handled too.
    usage:
     >>> load_library ('AthenaServices')
     >>> load_library ('AthenaServicesDict')
    """
    import ctypes, sys
    plat = sys.platform
    if plat.count('linux')>0:
        lib_prefix,lib_suffix = 'lib', '.so'
    elif plat == 'win32':
        lib_prefix,lib_suffix = '', '.dll'
    elif plat == 'darwin':
        lib_prefix,lib_suffix = 'lib','.dylib'
    else:
        raise RuntimeError ("sorry platform [%s] is not (yet?) supported"%plat)
    _sys_libname = libname
    if not _sys_libname.startswith (lib_prefix):
        _sys_libname = ''.join([lib_prefix,_sys_libname])
    if not _sys_libname.endswith (lib_suffix):
        _sys_libname = ''.join([_sys_libname, lib_suffix])
    return ctypes.cdll.LoadLibrary (_sys_libname)

STRING = c_char_p
_libraries = {}
_libraries['libshift.so'] = _load_library('libshift.so')


rfio_access = _libraries['libshift.so'].rfio_access
rfio_access.restype = c_int
rfio_access.argtypes = [STRING, c_int]
rfio_chdir = _libraries['libshift.so'].rfio_chdir
rfio_chdir.restype = c_int
rfio_chdir.argtypes = [STRING]
rfio_chmod = _libraries['libshift.so'].rfio_chmod
rfio_chmod.restype = c_int
rfio_chmod.argtypes = [STRING, c_int]
rfio_chown = _libraries['libshift.so'].rfio_chown
rfio_chown.restype = c_int
rfio_chown.argtypes = [STRING, c_int, c_int]
rfio_close = _libraries['libshift.so'].rfio_close
rfio_close.restype = c_int
rfio_close.argtypes = [c_int]
rfio_close_v3 = _libraries['libshift.so'].rfio_close_v3
rfio_close_v3.restype = c_int
rfio_close_v3.argtypes = [c_int]
rfio_end = _libraries['libshift.so'].rfio_end
rfio_end.restype = c_int
rfio_end.argtypes = []
rfio_symend = _libraries['libshift.so'].rfio_symend
rfio_symend.restype = c_int
rfio_symend.argtypes = []
rfio_unend = _libraries['libshift.so'].rfio_unend
rfio_unend.restype = c_int
rfio_unend.argtypes = []
rfio_fchmod = _libraries['libshift.so'].rfio_fchmod
rfio_fchmod.restype = c_int
rfio_fchmod.argtypes = [c_int, c_int]
rfio_fchown = _libraries['libshift.so'].rfio_fchown
rfio_fchown.restype = c_int
rfio_fchown.argtypes = [c_int, c_int, c_int]
class stat(Structure):
    pass
rfio_fstat = _libraries['libshift.so'].rfio_fstat
rfio_fstat.restype = c_int
rfio_fstat.argtypes = [c_int, POINTER(stat)]
rfio_getcwd = _libraries['libshift.so'].rfio_getcwd
rfio_getcwd.restype = STRING
rfio_getcwd.argtypes = [STRING, c_int]
rfio_lockf = _libraries['libshift.so'].rfio_lockf
rfio_lockf.restype = c_int
rfio_lockf.argtypes = [c_int, c_int, c_long]
__off_t = c_long
off_t = __off_t
rfio_lseek = _libraries['libshift.so'].rfio_lseek
rfio_lseek.restype = off_t
rfio_lseek.argtypes = [c_int, off_t, c_int]
rfio_lstat = _libraries['libshift.so'].rfio_lstat
rfio_lstat.restype = c_int
rfio_lstat.argtypes = [STRING, POINTER(stat)]
rfio_mkdir = _libraries['libshift.so'].rfio_mkdir
rfio_mkdir.restype = c_int
rfio_mkdir.argtypes = [STRING, c_int]
rfio_mstat = _libraries['libshift.so'].rfio_mstat
rfio_mstat.restype = c_int
rfio_mstat.argtypes = [STRING, POINTER(stat)]
rfio_munlink = _libraries['libshift.so'].rfio_munlink
rfio_munlink.restype = c_int
rfio_munlink.argtypes = [STRING]
rfio_msymlink = _libraries['libshift.so'].rfio_msymlink
rfio_msymlink.restype = c_int
rfio_msymlink.argtypes = [STRING, STRING]
rfio_mstat_reset = _libraries['libshift.so'].rfio_mstat_reset
rfio_mstat_reset.restype = c_int
rfio_mstat_reset.argtypes = []
rfio_munlink_reset = _libraries['libshift.so'].rfio_munlink_reset
rfio_munlink_reset.restype = c_int
rfio_munlink_reset.argtypes = []
rfio_msymlink_reset = _libraries['libshift.so'].rfio_msymlink_reset
rfio_msymlink_reset.restype = c_int
rfio_msymlink_reset.argtypes = []
rfio_open = _libraries['libshift.so'].rfio_open
rfio_open.restype = c_int
rfio_open.argtypes = [STRING, c_int]
rfio_open_v3 = _libraries['libshift.so'].rfio_open_v3
rfio_open_v3.restype = c_int
rfio_open_v3.argtypes = [STRING, c_int, c_int]
rfio_perror = _libraries['libshift.so'].rfio_perror
rfio_perror.restype = None
rfio_perror.argtypes = [STRING]
class iovec(Structure):
    pass
rfio_preseek = _libraries['libshift.so'].rfio_preseek
rfio_preseek.restype = c_int
rfio_preseek.argtypes = [c_int, POINTER(iovec), c_int]
rfio_read = _libraries['libshift.so'].rfio_read
rfio_read.restype = c_int
rfio_read.argtypes = [c_int, c_void_p, c_int]
rfio_read_v3 = _libraries['libshift.so'].rfio_read_v3
rfio_read_v3.restype = c_int
rfio_read_v3.argtypes = [c_int, STRING, c_int]
rfio_readlink = _libraries['libshift.so'].rfio_readlink
rfio_readlink.restype = c_int
rfio_readlink.argtypes = [STRING, STRING, c_int]
rfio_rename = _libraries['libshift.so'].rfio_rename
rfio_rename.restype = c_int
rfio_rename.argtypes = [STRING, STRING]
rfio_rmdir = _libraries['libshift.so'].rfio_rmdir
rfio_rmdir.restype = c_int
rfio_rmdir.argtypes = [STRING]
rfio_serrno = _libraries['libshift.so'].rfio_serrno
rfio_serrno.restype = c_int
rfio_serrno.argtypes = []
rfio_serror = _libraries['libshift.so'].rfio_serror
rfio_serror.restype = STRING
rfio_serror.argtypes = []
rfio_stat = _libraries['libshift.so'].rfio_stat
rfio_stat.restype = c_int
rfio_stat.argtypes = [STRING, POINTER(stat)]
class rfstatfs(Structure):
    pass
rfio_statfs = _libraries['libshift.so'].rfio_statfs
rfio_statfs.restype = c_int
rfio_statfs.argtypes = [STRING, POINTER(rfstatfs)]
rfio_symlink = _libraries['libshift.so'].rfio_symlink
rfio_symlink.restype = c_int
rfio_symlink.argtypes = [STRING, STRING]
rfio_unlink = _libraries['libshift.so'].rfio_unlink
rfio_unlink.restype = c_int
rfio_unlink.argtypes = [STRING]
rfio_write = _libraries['libshift.so'].rfio_write
rfio_write.restype = c_int
rfio_write.argtypes = [c_int, c_void_p, c_int]
rfio_write_v3 = _libraries['libshift.so'].rfio_write_v3
rfio_write_v3.restype = c_int
rfio_write_v3.argtypes = [c_int, STRING, c_int]
rfio_smstat = _libraries['libshift.so'].rfio_smstat
rfio_smstat.restype = c_int
rfio_smstat.argtypes = [c_int, STRING, POINTER(stat), c_int]
rfio_lseek_v3 = _libraries['libshift.so'].rfio_lseek_v3
rfio_lseek_v3.restype = c_int
rfio_lseek_v3.argtypes = [c_int, c_int, c_int]
rfio_close64_v3 = _libraries['libshift.so'].rfio_close64_v3
rfio_close64_v3.restype = c_int
rfio_close64_v3.argtypes = [c_int]
class stat64(Structure):
    pass
rfio_fstat64 = _libraries['libshift.so'].rfio_fstat64
rfio_fstat64.restype = c_int
rfio_fstat64.argtypes = [c_int, POINTER(stat64)]
__quad_t = c_longlong
__off64_t = __quad_t
off64_t = __off64_t
rfio_lockf64 = _libraries['libshift.so'].rfio_lockf64
rfio_lockf64.restype = c_int
rfio_lockf64.argtypes = [c_int, c_int, off64_t]
rfio_lseek64 = _libraries['libshift.so'].rfio_lseek64
rfio_lseek64.restype = off64_t
rfio_lseek64.argtypes = [c_int, off64_t, c_int]
rfio_lseek64_v3 = _libraries['libshift.so'].rfio_lseek64_v3
rfio_lseek64_v3.restype = off64_t
rfio_lseek64_v3.argtypes = [c_int, off64_t, c_int]
rfio_lstat64 = _libraries['libshift.so'].rfio_lstat64
rfio_lstat64.restype = c_int
rfio_lstat64.argtypes = [STRING, POINTER(stat64)]
rfio_mstat64 = _libraries['libshift.so'].rfio_mstat64
rfio_mstat64.restype = c_int
rfio_mstat64.argtypes = [STRING, POINTER(stat64)]
rfio_open64 = _libraries['libshift.so'].rfio_open64
rfio_open64.restype = c_int
rfio_open64.argtypes = [STRING, c_int]
rfio_open64_v3 = _libraries['libshift.so'].rfio_open64_v3
rfio_open64_v3.restype = c_int
rfio_open64_v3.argtypes = [STRING, c_int, c_int]
class iovec64(Structure):
    pass
rfio_preseek64 = _libraries['libshift.so'].rfio_preseek64
rfio_preseek64.restype = c_int
rfio_preseek64.argtypes = [c_int, POINTER(iovec64), c_int]
rfio_read64_v3 = _libraries['libshift.so'].rfio_read64_v3
rfio_read64_v3.restype = c_int
rfio_read64_v3.argtypes = [c_int, STRING, c_int]
rfio_stat64 = _libraries['libshift.so'].rfio_stat64
rfio_stat64.restype = c_int
rfio_stat64.argtypes = [STRING, POINTER(stat64)]
rfio_write64_v3 = _libraries['libshift.so'].rfio_write64_v3
rfio_write64_v3.restype = c_int
rfio_write64_v3.argtypes = [c_int, STRING, c_int]
rfio_smstat64 = _libraries['libshift.so'].rfio_smstat64
rfio_smstat64.restype = c_int
rfio_smstat64.argtypes = [c_int, STRING, POINTER(stat64), c_int]
class __dirstream(Structure):
    pass
DIR = __dirstream
rfio_closedir = _libraries['libshift.so'].rfio_closedir
rfio_closedir.restype = c_int
rfio_closedir.argtypes = [POINTER(DIR)]
class _IO_FILE(Structure):
    pass
FILE = _IO_FILE
rfio_fclose = _libraries['libshift.so'].rfio_fclose
rfio_fclose.restype = c_int
rfio_fclose.argtypes = [POINTER(FILE)]
rfio_feof = _libraries['libshift.so'].rfio_feof
rfio_feof.restype = c_int
rfio_feof.argtypes = [POINTER(FILE)]
rfio_ferror = _libraries['libshift.so'].rfio_ferror
rfio_ferror.restype = c_int
rfio_ferror.argtypes = [POINTER(FILE)]
rfio_fflush = _libraries['libshift.so'].rfio_fflush
rfio_fflush.restype = c_int
rfio_fflush.argtypes = [POINTER(FILE)]
rfio_fileno = _libraries['libshift.so'].rfio_fileno
rfio_fileno.restype = c_int
rfio_fileno.argtypes = [POINTER(FILE)]
rfio_fopen = _libraries['libshift.so'].rfio_fopen
rfio_fopen.restype = POINTER(FILE)
rfio_fopen.argtypes = [STRING, STRING]
rfio_fread = _libraries['libshift.so'].rfio_fread
rfio_fread.restype = c_int
rfio_fread.argtypes = [c_void_p, c_int, c_int, POINTER(FILE)]
rfio_fseek = _libraries['libshift.so'].rfio_fseek
rfio_fseek.restype = c_int
rfio_fseek.argtypes = [POINTER(FILE), c_long, c_int]
rfio_ftell = _libraries['libshift.so'].rfio_ftell
rfio_ftell.restype = c_long
rfio_ftell.argtypes = [POINTER(FILE)]
rfio_fwrite = _libraries['libshift.so'].rfio_fwrite
rfio_fwrite.restype = c_int
rfio_fwrite.argtypes = [c_void_p, c_int, c_int, POINTER(FILE)]
rfio_getc = _libraries['libshift.so'].rfio_getc
rfio_getc.restype = c_int
rfio_getc.argtypes = [POINTER(FILE)]
rfio_pclose = _libraries['libshift.so'].rfio_pclose
rfio_pclose.restype = c_int
rfio_pclose.argtypes = [POINTER(FILE)]
rfio_popen = _libraries['libshift.so'].rfio_popen
rfio_popen.restype = POINTER(FILE)
rfio_popen.argtypes = [STRING, STRING]
rfio_pread = _libraries['libshift.so'].rfio_pread
rfio_pread.restype = c_int
rfio_pread.argtypes = [STRING, c_int, c_int, POINTER(FILE)]
rfio_pwrite = _libraries['libshift.so'].rfio_pwrite
rfio_pwrite.restype = c_int
rfio_pwrite.argtypes = [STRING, c_int, c_int, POINTER(FILE)]
rfio_opendir = _libraries['libshift.so'].rfio_opendir
rfio_opendir.restype = POINTER(DIR)
rfio_opendir.argtypes = [STRING]
class dirent(Structure):
    pass
rfio_readdir = _libraries['libshift.so'].rfio_readdir
rfio_readdir.restype = POINTER(dirent)
rfio_readdir.argtypes = [POINTER(DIR)]
rfio_rewinddir = _libraries['libshift.so'].rfio_rewinddir
rfio_rewinddir.restype = c_int
rfio_rewinddir.argtypes = [POINTER(DIR)]
rfio_fopen64 = _libraries['libshift.so'].rfio_fopen64
rfio_fopen64.restype = POINTER(FILE)
rfio_fopen64.argtypes = [STRING, STRING]
rfio_fseeko64 = _libraries['libshift.so'].rfio_fseeko64
rfio_fseeko64.restype = c_int
rfio_fseeko64.argtypes = [POINTER(FILE), off64_t, c_int]
rfio_ftello64 = _libraries['libshift.so'].rfio_ftello64
rfio_ftello64.restype = off64_t
rfio_ftello64.argtypes = [POINTER(FILE)]
class dirent64(Structure):
    pass
rfio_readdir64 = _libraries['libshift.so'].rfio_readdir64
rfio_readdir64.restype = POINTER(dirent64)
rfio_readdir64.argtypes = [POINTER(DIR)]
size_t = c_uint
rfio_errmsg_r = _libraries['libshift.so'].rfio_errmsg_r
rfio_errmsg_r.restype = STRING
rfio_errmsg_r.argtypes = [c_int, c_int, STRING, size_t]
rfio_errmsg = _libraries['libshift.so'].rfio_errmsg
rfio_errmsg.restype = STRING
rfio_errmsg.argtypes = [c_int, c_int]
rfio_serror_r = _libraries['libshift.so'].rfio_serror_r
rfio_serror_r.restype = STRING
rfio_serror_r.argtypes = [STRING, size_t]
__ino_t = c_ulong
dirent._fields_ = [
    ('d_ino', __ino_t),
    ('d_off', __off_t),
    ('d_reclen', c_ushort),
    ('d_type', c_ubyte),
    ('d_name', c_char * 256),
]
__u_quad_t = c_ulonglong
__ino64_t = __u_quad_t
dirent64._pack_ = 4
dirent64._fields_ = [
    ('d_ino', __ino64_t),
    ('d_off', __off64_t),
    ('d_reclen', c_ushort),
    ('d_type', c_ubyte),
    ('d_name', c_char * 256),
]
__dev_t = __u_quad_t
__mode_t = c_uint
__nlink_t = c_uint
__uid_t = c_uint
__gid_t = c_uint
__blksize_t = c_long
__blkcnt_t = c_long
class timespec(Structure):
    pass
__time_t = c_long
timespec._fields_ = [
    ('tv_sec', __time_t),
    ('tv_nsec', c_long),
]
stat._pack_ = 4
stat._fields_ = [
    ('st_dev', __dev_t),
    ('__pad1', c_ushort),
    ('st_ino', __ino_t),
    ('st_mode', __mode_t),
    ('st_nlink', __nlink_t),
    ('st_uid', __uid_t),
    ('st_gid', __gid_t),
    ('st_rdev', __dev_t),
    ('__pad2', c_ushort),
    ('st_size', __off_t),
    ('st_blksize', __blksize_t),
    ('st_blocks', __blkcnt_t),
    ('st_atim', timespec),
    ('st_mtim', timespec),
    ('st_ctim', timespec),
    ('__unused4', c_ulong),
    ('__unused5', c_ulong),
]
__blkcnt64_t = __quad_t
stat64._pack_ = 4
stat64._fields_ = [
    ('st_dev', __dev_t),
    ('__pad1', c_uint),
    ('__st_ino', __ino_t),
    ('st_mode', __mode_t),
    ('st_nlink', __nlink_t),
    ('st_uid', __uid_t),
    ('st_gid', __gid_t),
    ('st_rdev', __dev_t),
    ('__pad2', c_uint),
    ('st_size', __off64_t),
    ('st_blksize', __blksize_t),
    ('st_blocks', __blkcnt64_t),
    ('st_atim', timespec),
    ('st_mtim', timespec),
    ('st_ctim', timespec),
    ('st_ino', __ino64_t),
]
iovec._fields_ = [
    ('iov_base', c_void_p),
    ('iov_len', size_t),
]
__dirstream._fields_ = [
]
class _IO_marker(Structure):
    pass
_IO_lock_t = None
_IO_FILE._pack_ = 4
_IO_FILE._fields_ = [
    ('_flags', c_int),
    ('_IO_read_ptr', STRING),
    ('_IO_read_end', STRING),
    ('_IO_read_base', STRING),
    ('_IO_write_base', STRING),
    ('_IO_write_ptr', STRING),
    ('_IO_write_end', STRING),
    ('_IO_buf_base', STRING),
    ('_IO_buf_end', STRING),
    ('_IO_save_base', STRING),
    ('_IO_backup_base', STRING),
    ('_IO_save_end', STRING),
    ('_markers', POINTER(_IO_marker)),
    ('_chain', POINTER(_IO_FILE)),
    ('_fileno', c_int),
    ('_flags2', c_int),
    ('_old_offset', __off_t),
    ('_cur_column', c_ushort),
    ('_vtable_offset', c_byte),
    ('_shortbuf', c_char * 1),
    ('_lock', POINTER(_IO_lock_t)),
    ('_offset', __off64_t),
    ('__pad1', c_void_p),
    ('__pad2', c_void_p),
    ('__pad3', c_void_p),
    ('__pad4', c_void_p),
    ('__pad5', size_t),
    ('_mode', c_int),
    ('_unused2', c_char * 40),
]
rfstatfs._fields_ = [
    ('totblks', c_long),
    ('freeblks', c_long),
    ('bsize', c_long),
    ('totnods', c_long),
    ('freenods', c_long),
]
iovec64._pack_ = 4
iovec64._fields_ = [
    ('iov_base', off64_t),
    ('iov_len', c_int),
]
_IO_marker._fields_ = [
    ('_next', POINTER(_IO_marker)),
    ('_sbuf', POINTER(_IO_FILE)),
    ('_pos', c_int),
]
__all__ = ['rfio_lstat', 'rfio_msymlink_reset', 'FILE', '__off64_t',
           'size_t', 'rfio_preseek', 'rfio_fstat', 'rfio_fflush',
           'rfio_closedir', '__ino64_t', 'rfio_serrno', 'rfio_feof',
           'rfio_lseek_v3', 'rfio_ftell', 'rfio_fchown', 'rfio_lseek',
           'rfio_close64_v3', 'rfio_fchmod', '__time_t',
           'rfio_serror', 'rfio_fseeko64', 'rfio_read64_v3',
           '__nlink_t', 'rfio_open', 'rfio_smstat', '_IO_lock_t',
           'timespec', '__off_t', 'rfio_ftello64', 'rfio_close',
           'rfio_write_v3', 'rfio_munlink_reset', 'rfio_mkdir',
           'rfio_lockf64', 'rfio_statfs', 'rfio_errmsg',
           'rfio_readlink', 'rfio_close_v3', 'dirent64',
           'rfio_lseek64_v3', 'rfio_end', 'rfio_popen', 'rfio_symend',
           'rfio_fopen64', 'rfio_pread', 'rfio_fopen', '__dirstream',
           'rfio_preseek64', '__blkcnt_t', 'rfio_msymlink',
           'rfio_fclose', 'rfio_readdir', 'rfio_readdir64',
           'rfio_rename', '__mode_t', 'off64_t', 'rfio_pwrite',
           '__blksize_t', 'rfio_fwrite', 'rfio_fseek', 'rfio_lockf',
           'rfio_stat', 'rfio_ferror', 'rfio_rewinddir', 'rfio_getc',
           'rfio_getcwd', 'iovec64', 'rfio_chmod', 'rfio_chdir',
           'rfio_serror_r', 'rfio_smstat64', 'rfio_mstat64',
           'rfio_rmdir', 'rfio_symlink', '__blkcnt64_t', '__dev_t',
           'rfio_unlink', 'rfio_lseek64', 'rfio_munlink',
           'rfio_pclose', 'rfio_open64_v3', 'rfio_write', '_IO_FILE',
           'rfio_fileno', 'rfio_mstat', 'rfio_perror', 'off_t',
           'iovec', 'rfio_chown', 'stat64', 'rfio_fread',
           '_IO_marker', 'rfio_fstat64', '__u_quad_t', 'DIR',
           'rfstatfs', 'dirent', '__gid_t', 'rfio_opendir', 'stat',
           'rfio_read_v3', 'rfio_open64', '__ino_t', 'rfio_access',
           'rfio_stat64', 'rfio_mstat_reset', 'rfio_lstat64',
           'rfio_read', '__quad_t', 'rfio_unend', 'rfio_errmsg_r',
           'rfio_open_v3', '__uid_t', 'rfio_write64_v3']

