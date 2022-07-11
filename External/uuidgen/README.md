# `uuidgen` executable

This package builds the `uuidgen` executable against whatever version of
`libuuid` is being used by the build.

This is meant to be used in situations where an LCG release provides the
`libuuid` library without providing the `uuidgen` executable as well. Which
can lead to `/usr/bin/uuidgen` not being functional in the environment
setup of that given release.

The source code was copied from https://github.com/util-linux/util-linux
on 7th July 2022, and slightly modified to work with the somewhat older
version of `libuuid` than what the latest version of this source code
was written for.

The `LICENSE` file in this package/directory is only describing the code in
this package, and does not apply to other parts of this repository.
