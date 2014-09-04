# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A collection of UNIX inspired functions for working with the filesystem.

This module is inspired by Ruby's FileUtils library and of course UNIX.

see: http://github.com/clutchski/fileutils
"""

from __future__ import with_statement

__author__ = 'clutchski@gmail.com'

# FIXME: still to implement ls, ln, du, df

import itertools
import grp
import os
import pwd as password_db
import shutil
import stat

#
# statics
#

NL = os.linesep

# chmod modes by permission type, ordered by user, group, other

READ_MODES  = (stat.S_IRUSR, stat.S_IRGRP, stat.S_IROTH)
WRITE_MODES = (stat.S_IWUSR, stat.S_IWGRP, stat.S_IWOTH)
EXEC_MODES  = (stat.S_IXUSR, stat.S_IXGRP, stat.S_IXOTH)
MODES = (READ_MODES, WRITE_MODES, EXEC_MODES)

# chmod modes by target, ordered by read, write, execute

USER_MODES  = [m[0] for m in MODES]
GROUP_MODES = [m[1] for m in MODES]
OTHER_MODES = [m[2] for m in MODES]

# chmod permission bits

READ_BIT       = 4  # e.g. chmod XYZ is user readable if X >= 4
WRITE_BIT      = 2
EXECUTABLE_BIT = 1

# error messages

ERROR_FILE_EXISTS = '[Errno 17] File exists'

def _is_str(obj):
    return isinstance(obj, basestring)

def _list(paths):
    return [paths] if _is_str(paths) else paths

def cd(path):
    """ Change the working directory to the given path. """
    os.chdir(path)

def pwd():
    """ Return the current working directory. """
    return os.getcwd()

def mkdir(dirs):
    """ Create the given directory or list of directories. """
    dirs = _list(dirs)
    map(os.mkdir, dirs)

def mkdir_p(dirs):
    """ Create the given directory or list of directories, along with any
        missing parent directories. This function is idempotent, so no 
        errors will be raised if a directory already exists.
    """
    dirs = _list(dirs)
    for dir_ in dirs:
        try:
            os.makedirs(dir_)
        except OSError, err:
            #FIXME: possible race condition in the isdir check. is a there a
            #way to avoid it?
            if ERROR_FILE_EXISTS in str(err) and os.path.isdir(dir_):
                # mkdir_p is idempotent in UNIX, thus here as well
                pass
            else:
                raise

def cp(paths, dest):
    """ Copy the given file or list of files to the destination. When copying 
        more than one file, the destination must be a directory.
    """
    paths = _list(paths)
    if len(paths) > 1:
        if not os.path.exists(dest) or not os.path.isdir(dest):
            raise OSError("target '%s' is not a directory" % dest)
    # use imap because it terminates at the end of the shortest iterable
    for _ in itertools.imap(shutil.copy, paths, itertools.repeat(dest)):
        pass

def _rm_path(path, force=False, recursive=False):
    if not os.path.exists(path):
        if force:
            # rm -f ignores missing paths
            return
        raise OSError('no such file or directory: %s' % path)
    elif not is_writeable(path) and not force:
        msg = 'cannot rm write-protected file or directory: %s' % path
        raise OSError(msg)
    if os.path.isdir(path):
        if not recursive:
            raise OSError("cannot remove directory: %s" % path)
        for child_path in os.listdir(path):
            _rm(os.path.join(path, child_path), force, recursive)
        os.rmdir(path)
    else:
        os.remove(path)

def _rm(paths, force=False, recursive=False):
    paths = _list(paths)
    for path in paths:
        _rm_path(path, force, recursive)

def rm(files):
    """ Remove the given file or list of files. """
    _rm(files)

def rm_f(files):
    """ Remove the given file or list of files, ignoring non-existant 
        and write-protected files.
    """
    _rm(files, force=True)

def rm_r(paths):
    """ Recursively remove the given paths or list of paths. """
    _rm(paths, recursive=True)

def rm_rf(paths):
    """ Recursively remove the given paths or list of paths, ignoring
        non-existant and write-protected files.
    """
    _rm(paths, force=True, recursive=True)

def rmdir(paths):
    """ Alias for "rm_r" """
    rm_r(paths)

def _is_valid_mode(mode):
    # mode must be a string because literal ints cannot start with zero
    return _is_str(mode)            \
       and len(mode) == 4           \
       and mode.isdigit()           \
       and mode[0] in ('0', '1')    \
       and not any((d in mode for d in ['8','9'])) 

def chmod(mode, paths):
    """ Apply the given permissions to the path or list of paths. The 
        permissions mode must be specified in octal notation, for example,
        "0755". 
    """
    paths = _list(paths)
    if not _is_valid_mode(mode):
        raise OSError('invalid chmod mode: %s' % mode)
    sticky_bit, user_bit, group_bit, other_bit = [int(c) for c in mode]
    bit_to_modes = ( (user_bit, USER_MODES)
                   , (group_bit, GROUP_MODES)
                   , (other_bit, OTHER_MODES)
                   )
    new_mode = 0
    for bit, (read_mode, write_mode, exec_mode) in bit_to_modes:
        if bit >= READ_BIT:
            new_mode = new_mode | read_mode
            bit = bit - READ_BIT
        if bit >= WRITE_BIT:
            new_mode = new_mode | write_mode
            bit = bit - WRITE_BIT
        if bit >= EXECUTABLE_BIT:
            new_mode = new_mode | exec_mode
    #FIXME: handle sticky bit
    for path in paths:
        os.chmod(path, new_mode)

def chmod_R(mode, paths):
    """ Apply the given permissions recursively to the given paths. The
        "chmod" function documentation describes the mode argument.
    """
    for path in _list(paths):
        if not os.path.exists(path):
            raise OSError("no such file or directory: '%s'" % path)
        chmod(mode, path)
        if os.path.isdir(path):
            child_paths = (os.path.join(path, c) for c in os.listdir(path))
            for child_path in child_paths:
                chmod_R(mode, child_path)

def mv(paths, dest):
    """ Move the given files or directories to the destination path. If more
        that one element is being moved, the destination must be a directory.
    """
    paths = _list(paths)
    if len(paths) > 1:
        if not os.path.exists(dest):
            raise OSError("no such file or directory: '%s'" % dest)
        if not os.path.isdir(dest):
            raise OSError("target '%s' is not a directory" % dest)
    for path in paths:
        if not os.path.exists(path):
            raise OSError('no such file or directory: %s' % path)
        shutil.move(path, dest)

def touch(paths):
    """ Update the access and modification times of the given path or list of
        paths. Any non-existant files will be created.
    """
    for path in _list(paths):
        if os.path.exists(path) and not is_writeable(path):
            raise OSError("can't touch write-protected path: %s" % path)
        with open(path, 'a'):
            os.utime(path, None)

def chown(user, group, paths):
    """ Set the user and group ownership of the given path or list 
        of paths. If the user or group is None, that attribute is unchanged.
    """
    paths = _list(paths)
    user_id = group_id = -1 # defaults which leave ownership unchanged
    if user is not None:
        try:
            user_id = password_db.getpwnam(user)[2]
        except KeyError:
            raise OSError("no such user: %s" % user)
    if group is not None:
        try:
            group_id = grp.getgrnam(group)[2]
        except KeyError:
            raise OSError("no such group: %s" % group)

    for path in paths:
        os.chown(path, user_id, group_id)

def chown_R(user, group, paths):
    """ Recursively set the user and group ownership of the given path or
        list of paths.
    """
    for path in _list(paths):
        if not os.path.exists(path):
            raise OSError("no such file or directory: '%s'" % path)
        chown(user, group, path)
        if os.path.isdir(path):
            child_paths = (os.path.join(path, c) for c in os.listdir(path))
            for child_path in child_paths:
                chown_R(user, group, child_path)
        
def _path_has_permissions(path, modes):
    """ Return True if the given path has each of the permissions
        corresponding to the given stat modes (e.g stat.S_IXOTH).
    """
    if not os.path.exists(path):
        msg = "no such file or directory: %s" % path
        raise OSError(msg)
    if not modes:
        raise OSError("must specify permissions to check")
    return all((os.stat(path).st_mode & m for m in modes))

def _get_modes_for_target(target, u_mode, g_mode, o_mode):
    modes = []
    target = target.lower()
    all_ = 'a' in target
    if all_ or 'u' in target:
        modes.append(u_mode)
    if all_ or 'g' in target:
        modes.append(g_mode)
    if all_ or 'o' in target:
        modes.append(o_mode)
    return modes

def is_readable(path, by='u'):
    """ Return True if the path is readable by all of the populations
    specified, False otherwise.
    """
    modes = _get_modes_for_target(by, *READ_MODES)
    return _path_has_permissions(path, modes)

def is_writeable(path, by='u'):
    """ Return True if the path is writeable by all of the populations
    specified, False otherwise.
    """
    modes = _get_modes_for_target(by, *WRITE_MODES)
    return _path_has_permissions(path, modes)

def is_executable(path, by='u'):
    """ Return True if the path is executable by all of the populations
    specified, False otherwise.
    """
    modes = _get_modes_for_target(by, *EXEC_MODES)
    return _path_has_permissions(path, modes)
